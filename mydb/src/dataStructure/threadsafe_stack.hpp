#pragma once

#include <iostream>
#include <exception>
#include <memory>
#include <thread>
#include <mutex>

#include <stack>

/**
  * @brief          线程安全的stack
  * @retval         none
  */
struct empty_stack: std::exception{

    const char* what() const throw();
};

template<typename T>
class threadsafe_stack{
    private:
        std::stack<T> data;
        mutable std::mutex m; // mutable关键字 被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中。
    public:
        threadsafe_stack() {}

        threadsafe_stack( const threadsafe_stack& other ){
            std::lock_guard<std::mutex> lock(other.m); // std::lock_guard<std::mutex> lock 加锁
            data = other.data; // 在构造函数中执行复制
        }

        threadsafe_stack& operator=(const threadsafe_stack& ) = delete; // 不使用赋值操作符

        void push(T new_value){
            std::lock_guard<std::mutex> lock(m);
            data.push(new_value);
        }

        std::shared_ptr<T> pop(){
            std::lock_guard<std::mutex> lock(m);
            // if(data.empty()) throw empty_stack(); // 检验是否为空
            if(data.empty()) return std::shared_ptr<T>();
            std::shared_ptr<T> const res( std::make_shared<T>(data.top()) ); // 使用shared_ptr来处理内存分配，避免new、delete的过多调用
            // shared_ptr 共享对象在最后一个强引用离开的时候释放
            // make_shared 类似于new，提高了效率（std::make_shared申请一个单独的内存块来同时存放Widget对象和控制块。这个优化减少了程序的静态大小，只分配一次内存）
            data.pop();
            return res;
        }

        void pop(T &value){
            std::lock_guard<std::mutex> lock(m);
            // if(data.empty()) throw empty_stack();
            if(data.empty()) return;
            value = data.top();
            data.pop();
        }

        bool empty(){
            std::lock_guard<std::mutex> lock(m);
            return data.empty();
        }
};