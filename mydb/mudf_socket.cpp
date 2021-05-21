/**
  ****************************(C) COPYRIGHT 2021 Peng****************************
  * @file       mudf_socket.cpp
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
#include "mudf_socket.hpp"
#include <string>

namespace peng {

/**
  * @brief          
  * @retval         redis_test
  */
void redis_test(){
    
    peng::Redis_client rc;

    while(1){
        
        // // redisCMD测试，存在读取数据量未知的问题
        // char cmd[] = "LRANGE ";
        // char *para[] = {"testl ", "0 " , "100 "};
        // std::string ret = rc.redisCMD( cmd , &para[0] , 3 );
        // std::cout << ret ;

        // set测试
        rc.credis_set( "id " , "1 " );
        rc.credis_incr( "id " );
        rc.credis_incrBy( "id " , 1);
        rc.credis_decr( "id " );
        rc.credis_decrBy( "id " , 1);
        
        // lPush/rPop/lLen测试
        char buf[128];
        sprintf( buf , "%d" , rand()%128 );
        rc.credis_lPush( "testl " , buf );
        rc.credis_rPop( "testl " );

        rc.credis_lLen( "testl " );
        ::sleep(1);
    }
    return;
}

bool Redis_client::credis_set( std::string key, std::string value ){
    bool    rc;
    char    buf[128];
    ssize_t nbytes;

    struct iovec   iov[5];
    
    iov[0].iov_base = (void *)"SET ";
    iov[0].iov_len  = 4;
    iov[1].iov_base = (void *)&key[0];
    iov[1].iov_len  = key.length();
    iov[2].iov_base = (void *)" ";
    iov[2].iov_len  = 1;
    iov[3].iov_base = (void *)&value[0];
    iov[3].iov_len  = value.length();
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

bool Redis_client::credis_get( std::string key, std::string &value ){
    bool    rc;
    char    buf[128];
    ssize_t nbytes;

    struct iovec   iov[3];
    
    iov[0].iov_base = (void *)"GET ";
    iov[0].iov_len  = 4;
    iov[1].iov_base = (void *)&key[0];
    iov[1].iov_len  = key.length();
    iov[2].iov_base = (void *)"\r\n";
    iov[2].iov_len  = 2;
        
    rc = rSocket.sendv( iov, 3 );	
    if(!rc) return false;
    /* 
        memset(buf, 0, sizeof(buf));
        nbytes = rSocket.read( buf, sizeof(buf) );
        LOGI( "%s" , buf );
        Redis消息格式：
        1.查询到数据时
        $7
        value38
        2.未查询到数据时
        $-1
    */
    
    nbytes = rSocket.readline( buf, sizeof(buf) );
    LOGI( "%s" , buf );
    int ret = std::stoi( &buf[1] ); // 7 或 -1
    if( ret == -1 ) return false;
    else {
        // 实际需要接受ret+2字节的数据(/r/n)
        value.resize( ret ); 
        nbytes = rSocket.read( &value[0], ret );
        rSocket.read( buf , 2 );
        return true;
    }
}

}