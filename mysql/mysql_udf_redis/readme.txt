rm -rf /usr/local/mysql/lib/plugin/udf_redis.so 
gcc -fPIC -Wall -I/usr/local/mysql/include -I. -shared udf_redis.c cJSON.c -o udf_redis.so
cp udf_redis.so /usr/local/mysql/lib/plugin/
/usr/local/mysql/bin/mysql -u root -p < redis.sql

https://blog.csdn.net/socho/article/details/52292064