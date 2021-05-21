#pragma once

// c/cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

// peng
#include "socket.h"
#include "debug.h"

namespace peng {

#define Redis_Server_ip "127.0.0.1"
#define Redis_Server_port 6379
#define safe_free(x) if(x){free(x);x=NULL;}

class Redis_client{

public:

    bool credis_set(std::string key, std::string value);
    bool credis_get(std::string key, std::string &value);

    Redis_client(){

        rSocket.connect( Redis_Server_ip , Redis_Server_port );
        
        std::cout << std::endl << "Connect to Redis server :" << std::endl << 
        "socket fd:   " << rSocket.fd() << std::endl << \
        "IP:   " << rSocket.ip() << std::endl << \
        "Port: " <<rSocket.port() << std::endl << std::endl;
        
        // std::cout << rSocket.getSocketOptString() << std::endl ;
        
        // ping-pong 测试连接状况
        rSocket.send( "PING\r\n" , strlen("PING\r\n") );
        char buf[128];
        rSocket.readline( buf , 128 );
        printf( "%s" , buf );
        
    }

    ~Redis_client(){
        ::close(rSocket.fd());
    }

    char *redisCMD( char* cmd , char** args , int num ){
        char    buf[128];
        ssize_t nbytes;

        std::vector< iovec > vec( num+2 );
        vec[0].iov_base = (void *)cmd;
        vec[0].iov_len  = strlen(cmd);
        for( int i=0;i<= num;++i ){
            vec[ i+1 ].iov_base = (void *)args[ i ];
            vec[ i+1 ].iov_len = strlen( args[ i ] );
        }
        vec[num+1].iov_base = (void *)"\r\n";
        vec[num+1].iov_len  = 2;

        rSocket.sendv( &vec[0] , vec.size() );

        nbytes = rSocket.readline(buf, sizeof(buf)); // 读第一行
        LOGI( "%s" , buf );

        // 接收完
        char temp[4096];
        do{
            memset( temp , 0 , sizeof(temp) );
            nbytes = rSocket.read(temp, sizeof(temp));
            LOGD( "%s" , temp );
        }while( nbytes == sizeof(temp) );

        return strdup(buf);
    }

    bool credis_set(char *key, char *value)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[5];
        
        iov[0].iov_base = (void *)"SET ";
        iov[0].iov_len  = 4;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)" ";
        iov[2].iov_len  = 1;
        iov[3].iov_base = value;
        iov[3].iov_len  = strlen(value);
        iov[4].iov_base = (void *)"\r\n";
        iov[4].iov_len  = 2;
           
        rc = rSocket.sendv( iov, 5 );	
        if(!rc) return false;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        LOGI( "%s" , buf );
        
        // 返回+OK说明操作成功
        return (strncmp(buf, "+OK", 3) == 0) ? true : false;
    }

    bool credis_get(char *key, char *value)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];
        
        iov[0].iov_base = (void *)"GET ";
        iov[0].iov_len  = 4;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;
           
        rc = rSocket.sendv( iov, 3 );	
        if(!rc) return false;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        LOGI( "%s" , buf );
        memcpy( value , buf , strlen(buf) );
        
        // 返回+OK说明操作成功
        return (strncmp(buf, "+OK", 3) == 0) ? true : false;
    }

    bool credis_delete(char *key)
    {
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];
        
        memset(buf, 0, sizeof(buf));
        
        iov[0].iov_base = (void *)"DEL ";
        iov[0].iov_len  = 4;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;

        rc = rSocket.sendv( iov, 3 );
        if(!rc) return false;

        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes == -1) return false;
        LOGI( "%s" , buf );
        
        // 返回:1说明操作成功
        return (strncmp(buf, ":1", 2) == 0) ? true : false;
    }


    bool credis_incr(char *key)
    {
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];
        
        memset(buf, 0, sizeof(buf));

        iov[0].iov_base = (void *)"INCR ";
        iov[0].iov_len  = 5;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;

        rc = rSocket.sendv( iov, 3 );
        if(!rc) return false;

        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes <1 ) return false;
        LOGI( "%s" , buf );

        return true;
    }

    bool credis_incrBy(char *key, int val)
    {
        int     len;
        bool    rc;
        char    *str;
        char    buf[128];
        ssize_t nbytes;
        
        len = asprintf(&str, "INCRBY %s %d\r\n", key, val);
        
        rc = rSocket.send( str, len );
        safe_free(str);

        if(!rc) return false;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return false;
        LOGI( "%s" , buf );

        return true;
    }

    bool credis_decrBy(char *key, int val)
    {
        int     len;
        bool    rc;
        char    *str;
        char    buf[128];
        ssize_t nbytes;
        
        len = asprintf(&str, "DECRBY %s %d\r\n", key, val);
        
        rc = rSocket.send( str, len );
        safe_free(str);

        if(!rc) return false;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return false;
        LOGI( "%s" , buf );

        return true;
    }

    bool credis_decr(char *key)
    {
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];
        
        memset(buf, 0, sizeof(buf));

        iov[0].iov_base = (void *)"DECR ";
        iov[0].iov_len  = 5;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;

        rc = rSocket.sendv( iov, 3 );
        if(!rc) return false;
        
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return false;
        LOGI( "%s" , buf );

        return true;
    }

    int credis_lPush(char *key, char *value)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[5];
        
        iov[0].iov_base = (void *)"LPUSH ";
        iov[0].iov_len  = 6;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)" ";
        iov[2].iov_len  = 1;
        iov[3].iov_base = value;
        iov[3].iov_len  = strlen(value);
        iov[4].iov_base = (void *)"\r\n";
        iov[4].iov_len  = 2;
        
        rc = rSocket.sendv( iov, 5 );
        if(!rc) return 0;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return 0;
        LOGI( "%s" , buf );

        return atoi(buf+1);
    }


    int credis_rPush(char *key, char *value)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[5];

        iov[0].iov_base = (void *)"RPUSH ";
        iov[0].iov_len  = 6;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)" ";
        iov[2].iov_len  = 1;
        iov[3].iov_base = value;
        iov[3].iov_len  = strlen(value);
        iov[4].iov_base = (void *)"\r\n";
        iov[4].iov_len  = 2;
        
        rc = rSocket.sendv( iov, 5 );
        if(!rc) return 0;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return 0;
        LOGI( "%s" , buf );

        return atoi(buf+1);
    }

    int credis_lPop(char *key)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];

        iov[0].iov_base = (void *)"LPOP ";
        iov[0].iov_len  = 5;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;
        
        rc = rSocket.sendv( iov, 3 );
        if(!rc) return 0;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return 0;
        LOGI( "%s" , buf );

        return atoi(buf+1);
    }

    int credis_rPop(char *key)
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];

        iov[0].iov_base = (void *)"RPOP ";
        iov[0].iov_len  = 5;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;
        
        rc = rSocket.sendv( iov, 3 );
        if(!rc) return 0;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return 0;
        LOGI( "%s" , buf );

        return atoi(buf+1);
    }

    int credis_lLen(char *key )
    {     
        bool    rc;
        char    buf[128];
        ssize_t nbytes;

        struct iovec   iov[3];

        iov[0].iov_base = (void *)"LLEN ";
        iov[0].iov_len  = 5;
        iov[1].iov_base = key;
        iov[1].iov_len  = strlen(key);
        iov[2].iov_base = (void *)"\r\n";
        iov[2].iov_len  = 2;
        
        rc = rSocket.sendv( iov, 3 );
        if(!rc) return 0;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return 0;
        LOGI( "%s" , buf );

        return atoi(buf+1);
    }

    char *credis_pipe(char *cmds)
    {
        char    buf[128];
        bool    rc;

        ssize_t nbytes;

        rc = rSocket.send( cmds, strlen(cmds) );
        if(!rc) return NULL;

        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        if(nbytes < 1) return NULL;
        LOGI( "%s" , buf );

        return strdup(buf);
    }


private:

    peng::Socket rSocket;

};
    void redis_test();
}