#pragma once

#include <iostream>
#include <exception>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <queue>

#define USE_STD_QUEUE 1

// 效果对比：简单类型下使用std::queue更加快捷，但当queue为class用户自定义类型且数据量较大时使用自己构造的queue会更快
// USE_STD_QUEUE        0       1       
// 使用int      4字节   5.54s   2.70s
// 使用Person() 96字节  5.93s   6.53s

#if USE_STD_QUEUE
/**
  * @brief          线程安全的队列
  * @retval         none
  * 优化：std::queue<T> data_queue;变为shared_ptr类型，在push、pop获取锁之前完成内存的分配，减小了锁的粒度，提高了性能；
  */
template<typename T>
class threadsafe_queue
{
    private:
        mutable std::mutex mut; // mut
        std::queue<T> data_queue; // queue
        std::condition_variable data_cond; // data_cond
    public:
        threadsafe_queue(){}

        threadsafe_queue(threadsafe_queue const& other) // 拷贝构造
        {
            std::lock_guard<std::mutex> lk(other.mut);
            data_queue=other.data_queue;
        }

        void push(T new_value) // push ,lock_guard + notify_one实现
        {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(new_value);
            data_cond.notify_one();
        }

        // wait_and_pop 会阻塞等待条件变量，然后返回front
        void wait_and_pop(T& value) // wait_and_pop ,unique_lock + wait实现 , 传出参数
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk,[this]{return !data_queue.empty();});
            value=std::move(data_queue.front()); // 使用右值
            data_queue.pop();
        }

        std::shared_ptr<T> wait_and_pop() // wait_and_pop ,unique_lock + wait ,shared_ptr实现
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk,[this]{return !data_queue.empty();});
            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return res;
        }

        // try_pop 会判断队列是否为空，然后返回front
        bool try_pop(T& value) // try_pop ,lock_guard实现
        {
            std::lock_guard<std::mutex> lk(mut);
            if(data_queue.empty)
                return false;
            value=std::move(data_queue.front()); // 使用右值
            data_queue.pop();
        }

        std::shared_ptr<T> try_pop() // try_pop ,lock_guard ,shared_ptr实现
        {
            std::lock_guard<std::mutex> lk(mut);
            if(data_queue.empty())
                return std::shared_ptr<T>();
            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return res;
        }

        bool empty() const // empty ,lock_guard实现
        {
            std::lock_guard<std::mutex> lk(mut);
            return data_queue.empty();
        }
};

#else

template<typename T>
class threadsafe_queue
{
private:
    /*
    节点说明：
        通过傀儡节点实现了，head tail始终指向不同的节点，防止了访问的竞争
        始终tail->next == nullptr , tail->data = nullptr
        head == tail说明是一个空链表
        只有一个元素时，head->next == tail
    */
    struct node
    {
        std::shared_ptr<T> data; // 使用shared_ptr方便管理，减少内存申请次数，提高了性能
        std::unique_ptr<node> next;
    };
    
    std::mutex head_mutex;
    std::unique_ptr<node> head;

    std::mutex tail_mutex;
    node* tail;

    std::condition_variable data_cond;

    node* get_tail() // 获取tail节点
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head() // 弹出head节点
    {
        std::unique_ptr<node> old_head=std::move(head);
        head=std::move(old_head->next);
        return old_head;
    }

     std::unique_lock<std::mutex> wait_for_data() // 等待数据到来
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock,[&]{return head!=get_tail();});
        return std::move(head_lock);
    }

    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail()) // 比较而不是检查NULL，傀儡节点意味着不可能是NULL
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail()) 
        {
            return std::unique_ptr<node>();
        }
        value=std::move(*head->data);
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        value=std::move(*head->data);
        return pop_head();
    }

public:
    threadsafe_queue():
        head(new node),tail(head.get()) // 初始化一个傀儡节点，使得try_pop()和push()不会再同一节点上操作
    {}

    threadsafe_queue(const threadsafe_queue& other)=delete;
    threadsafe_queue& operator=(const threadsafe_queue& other)=delete;

    /*
        push任何时候只访问tail，不访问head（不修改的时候空队列push同时访问了head和tail），
        互斥元在访问tail的全程都需要锁定。
    */
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(
            std::make_shared<T>(std::move(new_value))); // 避免之后为引用分配内存的开销
        
        std::unique_ptr<node> p(new node); // 新的傀儡节点
        { // 尾插
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data=new_data;
            node* const new_tail=p.get();
            tail->next=std::move(p);
            tail=new_tail;
        }

        data_cond.notify_one(); // 通知条件变量
    }

    /*
        try_pop访问head和tail，但对tail的访问是很短暂的。
        需要锁定head的互斥元直到修改完head，
        对tail的锁定只需要读取访问一次，封装为get_tail()函数。

        wait_pop等待条件变量的到来，实现过程与try_pop()类似。
    */
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_ptr<node> const old_head=wait_pop_head();
        return old_head->data;
    }

    void wait_and_pop(T& value)
    {
        std::unique_ptr<node> const old_head=wait_pop_head(value);
    }

    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> const old_head=try_pop_head();
        return old_head?old_head->data:std::shared_ptr<T>();
    }

    bool try_pop(T& value)
    {
        std::unique_ptr<node> const old_head=try_pop_head(value);
        return old_head;
    }

    void empty()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head==get_tail());
    }
};

#endif
