#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <list>
#include <utility>
#include <boost/thread/shared_mutex.hpp>

#include <iostream>

/**
  * @brief          线程安全的list
  * @retval         none
  */
template<typename T>
class threadsafe_list
{
    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;

        node():
            next()
        {}
        
        node(T const& value):
            data(std::make_shared<T>(value))
        {}
    };
    
    node head;

public:
    threadsafe_list()
    {}

    ~threadsafe_list() // 析构，删除所有节点
    {
        remove_if([](T const&){return true;});
    }

    threadsafe_list(threadsafe_list const& other)=delete;
    threadsafe_list& operator=(threadsafe_list const& other)=delete;
    
    void push_front(T const& value) // 添加新节点
    {
        std::unique_ptr<node> new_node(new node(value)); // 锁定头结点并插入list
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next=std::move(head.next);
        head.next=std::move(new_node);
    }

    template<typename Function>
    void for_each(Function f) // 对所有节点执行Function操作
    {
        node* current=&head;
        std::unique_lock<std::mutex> lk(head.m); // 锁定头结点
        while(node* const next=current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m); // 锁定访问的节点 并 解锁上一个节点
            lk.unlock();
            f(*next->data);
            current=next;
            lk=std::move(next_lk);
        }
    }

    template<typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p) // 查找第一个满足条件的节点
    {
        node* current=&head;
        std::unique_lock<std::mutex> lk(head.m);
        while(node* const next=current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            // std::cout << *next->data << std::endl;
            if(p(*next->data))
            {
                return next->data;
            }
            current=next;
            lk=std::move(next_lk);
        }
        return std::shared_ptr<T>();
    }

    template<typename Predicate>
    void remove_if(Predicate p) // 删除第一个满足条件的节点
    {
        node* current=&head;
        std::unique_lock<std::mutex> lk(head.m);
        while(node* const next=current->next.get())
        {
            std::unique_lock<std::mutex> next_lk(next->m);
            if(p(*next->data))
            {
                // 删除节点
                std::unique_ptr<node> old_next=std::move(current->next);
                current->next=std::move(next->next); // 删除节点
                next_lk.unlock();
            }
            else
            {
                lk.unlock();
                current=next;
                lk=std::move(next_lk);
            }
        }
    }
};

