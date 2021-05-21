#include "mudf_socket.hpp"
#include "src/dataStructure/threadsafe_queue.hpp"

#include <atomic>
#include <thread>
#include <iostream>
#include <sstream> // std::ostringstream

#include "mydb.hpp"

std::atomic<int> times;

void mydb_Redis_test(){

    peng::mydb_Redis mydb;
    std::vector<std::thread*> threads;
    for(int i=0;i<10;++i) {
        threads.push_back(new std::thread(
        [ & ]
        {
            std::cout << "thread " << i << std::endl;
            for(int i=0;i<100;++i) {
                std::string key = "test" + std::to_string( rand()%128 );
                std::string value = "value" + std::to_string( rand()%128 );
                mydb.set(key, value);
                std::cout << "set " << key << " , " << value << std::endl;
                ::usleep(1000*10); // 10ms
            }
        }));
    }
    for(int i=0;i<1;++i) {
        threads.push_back(new std::thread(
        [ & ]
        {
            std::cout << "thread " << i << std::endl;
            for(int i=0;i<100;++i) {
                std::string key = "test" + std::to_string( rand()%128 );
                std::string value;
                bool ret = mydb.get(key, value);
                if( ret ) std::cout << "get " << key << " , " << value << std::endl;
                else std::cout << "get " << key << " , " << "Not Fount" << std::endl;
                ::usleep(1000*10); // 10ms

                // 使用原子变量监测执行情况
                // ::sleep(1);
                // times.fetch_add(1);
                // std::cout << "Test times : " << times.load() << std::endl;
            }
        }));
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join

    std::cout << "end." << std::endl;
    
}
void mydb_Pedis_test(){
    peng::mydb_Pedis mydb;
    std::vector<std::thread*> threads;
    for(int i=0;i<10;++i) {
        threads.push_back(new std::thread(
        [ & ]
        {
            std::cout << "thread " << i << std::endl;
            for(int i=0;i<100;++i) {
                std::string key = "test" + std::to_string( rand()%128 );
                std::string value = "value" + std::to_string( rand()%128 );
                mydb.set(key, value);
                std::cout << "set " << key << " , " << value << std::endl;
                ::usleep(1000*10); // 10ms
            }
        }));
    }
    for(int i=0;i<10;++i) {
        threads.push_back(new std::thread(
        [ & ]
        {
            std::cout << "thread " << i << std::endl;
            for(int i=0;i<100;++i) {
                std::string key = "test" + std::to_string( rand()%128 );
                std::string value;
                bool ret = mydb.get(key, value);
                std::cout << "get " << key << " , " << value << std::endl;
                ::usleep(1000*10); // 10ms

                // 使用原子变量监测执行情况
                // ::sleep(1);
                // times.fetch_add(1);
                // std::cout << "Test times : " << times.load() << std::endl;
            }
        }));
    }
    std::for_each( threads.begin() ,threads.end() ,std::mem_fn( &std::thread::join ) ); // 轮流在每个线程上调用join

    std::cout << "end." << std::endl;

}

int main(){

    srand( time(NULL) );
    // mydb_Redis_test();
    mydb_Pedis_test();

}