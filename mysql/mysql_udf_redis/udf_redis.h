#ifndef UDF_REDIS__h
#define UDF_REDIS__h

/*

gcc -I/usr/include/mysql -shared -fPIC -o udf_redis.c udf_redis.h cJSON.c -o udf_redis.so

connect:连接redis
redis_connect_init,redis_connect
调用：credis_connect

close:关闭连接
redis_close_init,redis_close
调用：credis_close

set: redis set命令
redis_set_init,redis_set

delete: redis删除key
redis_delete_init,redis_delete

incr：redis key中储存的值自增
redis_incr_init,redis_incr
redis_incrBy_init,redis_incrBy
redis_decrBy_init,redis_decrBy

lpush,rpush
redis_lPush_init,redis_lPush
redis_rPush_init,redis_rPush

pipe 高效地向redis插入大量数据
redis_pipe_init,redis_pipe

创建json
json_object_init,json_object_deinit,json_object

safe_write（write实现） 、 sendv（writev实现） 发送命令
char *cmds;
safe_write(redis->fd, cmds, strlen(cmds)); 
struct iovec   iov[5];
rc = sendv(redis->fd, iov, 5);	
// asprintf()可以说是一个增强版的sprintf(),在不确定字符串的长度时，能够根据格式化的字符串长度，申请足够的内存空间。
// sendv/readv 实现一次读写多个缓冲区。

*/

/*

redis 语句:

set key value
get key
incr key
incrby key increasement
decrby key increasement

lpush key value1 value2 ...
rpush key value1 value2 ...

pipe

*/

#include <mysql.h>
#include <mysql_com.h>

#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "cJSON.h"

void credis_close(struct credis  *redis);

struct credis *credis_connect(char *host, uint16_t port);

bool credis_delete(struct credis  *redis, char *key);

bool credis_set(struct credis  *redis, char *key, char *value);

bool credis_incrBy(struct credis  *redis, char *key, int val);

bool credis_incr(struct credis  *redis, char *key);

bool credis_decr(struct credis  *redis, char *key);

bool credis_decrBy(struct credis  *redis, char *key, int val);

int credis_lPush(struct credis  *redis, char *key, char *value);

int credis_rPush(struct credis  *redis, char *key, char *value);

char *credis_pipe(struct credis  *redis, char *cmds);

bool redis_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_close_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_close(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_set(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_delete(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_incr_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_incr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_incrBy_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_incrBy(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_decrBy_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_decrBy(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_decr_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_decr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_lPush_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_lPush(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_rPush_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

char *redis_rPush(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool redis_pipe_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

void redis_pipe_deinit(UDF_INIT *initid);

char *redis_pipe(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool json_object_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

void json_object_deinit(UDF_INIT *initid);

char *json_object(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);

bool safe_write(int sockfd, const char *buf, const int len);

bool sendv(int fd, struct iovec *vec , size_t c);

#endif