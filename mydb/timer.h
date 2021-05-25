/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       timer.cpp/.h
  * @brief      red-black-tree
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-1-2021      Peng            1. 完成
  *
  @verbatim
  ==============================================================================
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Peng****************************
  */
#include "rbTree.hpp"
#include "debug.h"
#include "mstime.h"
#include <functional>

#include <unistd.h>
#include <vector>
#include <algorithm>

#include <thread>

namespace peng{

class timerEvent{

    public:

    std::function<void()> func;
    int64_t time;

    timerEvent(char *){ // timerEvent(NULL)
        func = std::move( []{} );
        time = INT64_MAX;
        LOGD( "NULL Timer.\r\n" );
    }

    timerEvent( std::function<void()>&& _func , int64_t _runAfter ){
        func = std::move( _func );
        time = peng::Time::nowuSec().getTimeVal() + _runAfter;
        LOGD( "Timer run at time %ld.\r\n" , time );
    }

    timerEvent( std::function<void()>& _func , int64_t _runAfter ){
        func = _func;
        time = peng::Time::nowuSec().getTimeVal() + _runAfter;
        LOGD( "Timer run at time %ld.\r\n" , time );
    }

    friend bool operator<( timerEvent &a , timerEvent &b ) {
        return a.time < b.time;
    }

    friend bool operator==( timerEvent &a , timerEvent &b ){
        return a.time==b.time;
    }
    
    friend std::ostream  &operator<<( std::ostream &out, timerEvent &a ) // 重载<<
    {
        std::cout << a.time ;
        return out;
    }

    // 测试 operator<
    static void test(){
        std::vector<timerEvent> v;
        for(int i=0;i<10;++i) {
            v.emplace_back( []{} , rand()%10240 );
        }
        std::cout << "timerEvents :" << std::endl;
        for(auto elem:v) std::cout << elem.time << std::endl;
        std::cout << std::endl;
        std::sort( v.begin() , v.end() );
        std::cout << "sort :" << std::endl;
        for(auto elem:v) std::cout << elem.time << std::endl;
        std::cout << std::endl;
    }
};

class Timer{
    public:

    Timer(){
        
    }

    void addTimer( timerEvent* te , int msec ){
        
    }

    void delTimer( timerEvent* te ){
        
    }

    void updateTimer( timerEvent* te , int msec ) {
        
    }
};


class rbtTimer:public Timer{
    // 线程池处理Timer任务，需要rbt实现线程安全
    public:

    rbtTimer(){
        rbTree = new RBTree<timerEvent>;
        pTimerHandlers = new std::thread(
        [ this ]
        {
            // 获取tid
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            std::string stid = oss.str();
            unsigned long long _tid = std::stoull(stid);
            // LOGD( "Thread %lluld running Timer functions.\r\n" , _tid );

            int nums=0;
            // 待添加timerfd,线程池,null判断等
            while( true ) {
                if( rbTree->size()<=0 ) {
                    ::usleep( 100 );
                    continue;
                }

                int64_t time = peng::Time::nowuSec().getTimeVal();
                timerEvent ev = rbTree->minimum();
                if( ev.time < time ) {
                    delTimer(&ev); // 删除Timer时间
                    ev.func(); // 运行 ev.func
                    LOGI( "Thread %lluld running %d functions.\r\n" , _tid , ++nums );
                }
                ::usleep( 100 );
            }
        });
    }

    ~rbtTimer() {
        pTimerHandlers->join();
        delete rbTree;
    }

    void addTimer( timerEvent* te ){
        rbTree->insert( *te );
        LOGD( "Num of Timers %d.\r\n" , rbTree->size() );
    }

    void addTimer( timerEvent &&te ){
        rbTree->insert( std::move(te) );
        LOGD( "Num of Timers %d.\r\n" , rbTree->size() );
    }

    timerEvent *addTimer( std::function<void()> _func , int msec ){
        timerEvent *te = new timerEvent(_func,msec);
        rbTree->insert( *te );
        LOGD( "Num of Timers %d.\r\n" , rbTree->size() );
        return te;
    }

    void delTimer( timerEvent* te ){
        rbTree->remove(*te);
        LOGD( "Num of Timers %d.\r\n" , rbTree->size() );
    }

    void updateTimer( timerEvent* te , int msec ) {

    }

    private:
    RBTree<timerEvent> *rbTree; // RBTree<timerEvent> rbTree会导致丢失一个节点的情况
    std::thread *pTimerHandlers;
};

}
