### mydb

提供set、get接口，可以选择使用线程安全的map、redis、mysql等进行数据的存储与查询。

## some idea

### 实现功能

提供直接储存与获取 线程安全的哈希表；
提供Redis储存；
提供Mysql储存；
提供Redis作为mysql的缓存的存取。

### 实现Redis作为Mysql的缓存

使用过期键储存在Redis中，
查询时，如果存在数据，则直接返回，否则，请求mysql。
需要布隆过滤器，防止数据不存在。
数据写入时，写入到mysql和redis中，且由线程安全的队列存放缓冲数据。

### 提供接口

set();
get();
多线程安全。

### 杂

*if redis list length >=1000
set key newlist
copy list
remove list

redis将数据汇总保存到mysql / s

mysql将数据传输到redisredis，用于读取

管道怎么用，数据怎么转换为统一格式。

*时间序列数据存储：
使用无锁队列/线程安全的队列实现缓冲，
对于非临时数据，在线程中将数据传输到mysql保存；
对于临时数据，传输到Redis的list中，设置超时和队长限制。

*key-value数据
发布订阅对应关系，使用Mysql储存，
* 发布端 发布的数据Topic类型
* 订阅端 订阅的数据Topic类型
* TOPICs -> 订阅端
此处加锁，使用Redis键加锁、储存数据。
Redis使用链表储存订阅关系，Mysql更新到Redis
now-> last ->llast信息流，推送到订阅端。

*定时任务更新redis数据、mysql数据、储存部分配置

*脚本实现初始化

*Matlab绘图实现