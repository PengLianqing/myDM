// sudo find ../../src/  -name '*.hpp' | xargs grep safe
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

#include <thread>
#include "threadsafe_queue.hpp"
#include "threadsafe_stack.hpp"
#include "threadsafe_map.hpp"
#include "threadsafe_list.hpp"


#define TEST_THREAD_SAFE_DATE_STRUCTURE 0

/**
  * @brief          threadsafe_stack测试
  * @retval         none
  */
threadsafe_stack<std::string> ts;

void f1(){
    int i = 100000;
    while(i-->0){
        ts.push( "M2020707"+std::to_string(rand()%64) );
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void f2(){
    int i = 100000;
    while(i-->0){
        ts.pop();
        // if(sp!=nullptr){
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void threadsafe_stack_test(){
    // 使用vector容器初始化线程并执行回收
    std::vector< std::thread > threads; // thread对象容器
    for( int i=0; i<50 ;i++){
        threads.push_back( std::thread(f1) );
        threads.push_back( std::thread(f2) );
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join
}

/**
  * @brief          threadsafe_queue测试
  * @retval         none
  */
class Person{
    public:
    int age;
    std::string name;
    std::string company; 
    std::vector<int> v;
    Person():age(10),name("Peng"),company("HUST"),v(std::vector<int>(10,0)){}
};

// threadsafe_queue<int> tq;
threadsafe_queue<Person> tq;

void f3(){
    int i = 100000;
    Person p;
    while(i-->0){
        tq.push( p );
        // tq.push( rand()%1024 );
        std::this_thread::sleep_for( std::chrono::milliseconds(0) ); // 延时10ms
    }
}

void f4(){
    int i = 100000;
    while(i-->0){
        std::shared_ptr<Person> sp = tq.try_pop( );
        if(sp!=nullptr){
            // std::cout << "pop " << *sp << std::endl;
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(0) ); // 延时10ms
    }
}

void threadsafe_queue_test(){
    std::cout << "int " << sizeof(int) << std::endl;
    std::cout << "Person " << sizeof(Person) << std::endl;
    // 使用vector容器初始化线程并执行回收
    std::vector< std::thread > threads; // thread对象容器
    for( int i=0; i<50 ;i++){
        threads.push_back( std::thread(f3) );
        threads.push_back( std::thread(f4) );
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join
}

/**
  * @brief          threadsafe_map测试
  * @retval         none
  */
threadsafe_map< std::string , int > tm;

void f5(){
    int i = 100;
    Person p;
    while(i-->0){
        tm.add_or_update_mapping( "M20207"+std::to_string(rand()%2560) , rand()%2560 );
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void f6(){
    int i = 100;
    while(i-->0){
        tm.value_for( "M20207"+std::to_string(rand()%2560) , -1 );
        // std::cout << tm.value_for( "M20207"+std::to_string(rand()%2560) , -1 ) << std::endl;
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void threadsafe_map_test(){
    // 使用vector容器初始化线程并执行回收
    std::vector< std::thread > threads; // thread对象容器
    for( int i=0; i<50 ;i++){
        threads.push_back( std::thread(f5) );
        threads.push_back( std::thread(f6) );
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join
}

/**
  * @brief          threadsafe_list测试
  * @retval         none
  */
threadsafe_list< std::string > tl;

void f7(){
    int i = 10000;
    Person p;
    while(i-->0){
        tl.push_front( "M2020707"+std::to_string(rand()%64) );
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void f8(){
    int i = 10000;
    while(i-->0){
        std::string target = "M2020707"+std::to_string(rand()%64);
        auto ans = tl.find_first_if( [& target](std::string temp){ return temp == target ; } );
        // if(ans) std::cout << "find: " << target << "," << *ans << std::endl;
        // else std::cout << "not found" << target << std::endl;
        std::this_thread::sleep_for( std::chrono::milliseconds(0) );
    }
}

void threadsafe_list_test(){
    // 使用vector容器初始化线程并执行回收
    std::vector< std::thread > threads; // thread对象容器
    for( int i=0; i<50 ;i++){
        threads.push_back( std::thread(f7) );
        threads.push_back( std::thread(f8) );
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join
}

#if TEST_THREAD_SAFE_DATE_STRUCTURE
int main()
{
    // threadsafe_stack_test(); 
    // threadsafe_queue_test();
    threadsafe_map_test();
    // threadsafe_list_test();
    
    // 对比互斥元实现和无锁并发实现：
    // threadsafe_stack_test()  2.20s sys 2.882 user 4.333 real 1.845
    // lock_free_stack_test()   2.26s sys 0.830 user 6.354 real 1.898 sys用时更少，但user用时更多
    return 0;
}
#endif
