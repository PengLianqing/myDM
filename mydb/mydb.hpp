/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       mydb.cpp/h
  * @brief      
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
#pragma once

#include "mudf_socket.hpp"
#include "src/dataStructure/threadsafe_queue.hpp"
#include "src/dataStructure/threadsafe_map.hpp"

#include <thread>
#include <iostream>
#include <sstream> // std::ostringstream

#include "mstime.h"

namespace peng {

class mKV{ 
    // 键值对key-value对象
    public:
    std::string key;
    std::string value;
    mKV() {
    }
    mKV( std::string _key , std::string _value ):key(_key),value(_value) {
    }
};

class mydb{
    public:
    bool set( std::string key , std::string value ) {
        std::cout << "set: " << key << "," << value << std::endl;
        return true;
    }
    bool get( std::string key ) {
        std::cout << "get: " << key << std::endl;
        return true;
    }
};

class mydb_Redis:public mydb{
    public:

    mydb_Redis( int _nread=10 ):nread(_nread){

        // 创建用于读的连接池
        for(int i=0;i<nread;++i){
            client_read.push( new peng::Redis_client() );
        }

        // 创建用于写Redis的线程
        pRedisClientUpdate = new std::thread(
        [ this ]
        {
            // 获取tid
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            std::string stid = oss.str();
            _tid = std::stoull(stid);
            LOGD( "Thread %lluld running Redis Client.\r\n" , _tid );
            
            // 创建连接
            client_write = new peng::Redis_client; 

            mKV cur;
            while(1) {

            // #define TEST_REDIS_WR
            
            #ifdef TEST_REDIS_WR
                // 测试与Redis交互
                cur.key = "test";
                cur.value = "M20207xxxx";
                char key[16],value[16];
                ::sprintf( key , &cur.key[0] , cur.key.length() );
                ::sprintf( value , &cur.value[0] , cur.value.length() );
                // std::cout << key << "," << value << std::endl;
                rc->credis_lPush( key , value );
                rc->credis_rPop( "test" );
                ::sleep(1);

            #else

                tq.wait_and_pop( cur );
                client_write->credis_set( cur.key , cur.value );

            #endif

            }
        });
    }

    void set( std::string key , std::string value ) {
        tq.push( mKV(key,value) );
    }

    bool get( std::string key , std::string &value ) {
        // 从连接池获取Redis Client连接并处理get请求
        peng::Redis_client *client;
        client_read.wait_and_pop( client );
        bool ret = client->credis_get(key, value);
        client_read.push( client );
        return ret;
    }
    
    ~mydb_Redis(){
        for(int i=0;i<nread;++i){
            client_read.wait_and_pop();
        }
        pRedisClientUpdate->join();
    }

    private:
        
        threadsafe_queue<mKV> tq; // 线程安全的队列 用于缓冲

        std::thread* pRedisClientUpdate; // 用于将数据更新到Redis的线程
        
        unsigned long long _tid; // 线程id

        peng::Redis_client *client_write; // Redis client连接,用于写入

        int nread;
        threadsafe_queue<peng::Redis_client *> client_read; // Redis client连接,用于读取
};

class mydb_Pedis:public mydb{
    public:

    mydb_Pedis(){

    }

    void set( std::string key , std::string value ) {
        tm.add_or_update_mapping(key, value);
    }

    bool get( std::string key , std::string &value ) {
       value = tm.value_for(key);
       return true;
    }
    
    ~mydb_Pedis(){
        
    }

    private:
    threadsafe_map< std::string , std::string > tm;
};


}

