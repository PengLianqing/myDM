redis集群实现：

1.安装redis:部分组件ubuntu直接安装是缺失的
wget https://download.redis.io/releases/redis-6.2.3.tar.gz
tar xzf redis-6.2.3.tar.gz
cd redis-6.2.3
make
make install

2.创建redis-cluster目录 、 9000~9003文件夹 、 bin文件夹
sudo mkdir -p 9000/date 9001/date 9002/date 9003/date
sudo mkdir bin
将src目录下的二进制文件拷贝到bin文件夹下,redis-benchmark  redis-check-aof  redis-check-rdb  redis-cli  redis-sentinel  redis-server  redis-trib.rb

3.将redis-6.2.3目录下的文件拷贝到9000等目录下(不需要-R)：
peng@ubuntu:~/code/OpsourcePrj/thirdparty/redis-6.2.3$ sudo cp * ../../redis-cluster/9000/
cp: 未指定 -r；略过目录'deps'
cp: 未指定 -r；略过目录'src'
cp: 未指定 -r；略过目录'tests'
cp: 未指定 -r；略过目录'utils'

4.逐个修改conf文件：
包括ip地址、端口、daemonize、pidfile、dir、cluster-enabled、appendonly、cluster-config-file等。
bind 211.67.19.174
port 9000
daemonize yes
pidfile /var/run/redis_9000.pid
dir /home/peng/code/redis-cluster/9000/date
cluster-enabled yes
appendonly yes // 开启AOF数据持久化
cluster-config-file nodes-9000.conf

5.启动redis(可以使用脚本文件,sudo)
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9000/redis.conf
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9001/redis.conf
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9002/redis.conf
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9003/redis.conf
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9004/redis.conf
sudo /home/peng/code/redis-cluster/bin/redis-server /home/peng/code/redis-cluster/9005/redis.conf

6.查看运行情况与停止
ps -ef|grep redis-server
sudo killall redis-server

7.此时集群还未建立，只是创建了redis-server端，提示"(error) CLUSTERDOWN Hash slot not served"
peng@ubuntu:~/code/redis-cluster$ sudo redis-cli -c -h 211.67.19.174 -p 9000
211.67.19.174:9000> keys *
(empty array)
211.67.19.174:9000> set name test
(error) CLUSTERDOWN Hash slot not served

8.创建集群
--cluster-replicas 1 说明每个主节点1个从节点。
至少需要3个master，每个master一个从节点，所以至少需要6个节点。
redis-cli --cluster create 211.67.19.174:9000 211.67.19.174:9001 211.67.19.174:9002 211.67.19.174:9003 211.67.19.174:9004 211.67.19.174:9005 --cluster-replicas 1

9.测试
sudo redis-cli -c -h 211.67.19.174 -p 9000 // -c 指定为集群模式
set name test // 返回Redirected to slot [5798] located at 211.67.19.174:9001

使用 cluster info命令查看集群情况；
使用 cluster nodes命令查看集群节点；

10.故障转移
kill掉主节点后，noded显示master fail状态，重启节点后变为slave节点。

11.AOF持久化
redis-check-aof --fix appendonly.aof

故障转移机制：
1.集群中的节点会向其它节点发送PING消息（该PING消息会带着当前集群和节点的信息），如果在规定时间内，没有收到对应的PONG消息，就把此节点标记为疑似下线。
2.当被分配了slot槽位的主节点中有超过一半的节点都认为此节点疑似下线，那么该节点就真的下线。
3.其它节点收到某节点已经下线的广播后，把自己内部的集群维护信息也修改为该节点已事实下线。
4.节点资格审查：然后对从节点进行资格审查，每个从节点检查最后与主节点的断线时间，如果该值超过配置文件的设置，那么取消该从节点的资格。
5.准备选举时间：这里使用了延迟触发机制，主要是给那些延迟低的更高的优先级，延迟低的让它提前参与被选举，延迟高的让它靠后参与被选举。（延迟的高低是依据之前与主节点的最后断线时间确定的）
6.选举投票：当从节点获取选举资格后，会向其他带有slot槽位的主节点发起选举请求，由它们进行投票，优先级越高的从节点就越有可能成为主节点，当从节点获取的票数到达一定数值时（如集群内有N个主节点，那么只要有一个从节点获得了N/2+1的选票即认为胜出），就会替换成为主节点。
7.替换主节点：被选举出来的从节点会执行slaveof no one把自己的状态从slave变成master，然后执行clusterDelSlot操作撤销故障主节点负责的槽，并执行 clusterAddSlot把这些槽分配给自己，之后向集群广播自己的pong消息，通知集群内所有的节点，当前从节点已变为主节点。
8.接管相关操作：新的主节点接管了之前故障的主节点的槽信息，接收和处理与自己槽位相关的命令请求。

https://baijiahao.baidu.com/s?id=1660566302486460631&wfr=spider&for=pc


mysql高可用方案：
1.主从复制
使用双节点数据库，搭建单向或者双向的半同步复制。
和proxy、keepalived等第三方软件同时使用，即可以用来监控数据库的健康，又可以执行一系列管理命令。

2.双通道复制
半同步复制在发生超时会变为异步复制，不能保证数据的一致性。
优化：发生超时时建立两条通道，半同步通道从当前位置复制主机，异步复制通道追补从机落后的数据。

3.多节点数据库集群
构建一主多从或多主多从的集群。
MHA+多节点集群、zookeeper+proxy代理（分布式+代理，避免了脑裂现象）

4.共享存储
通过高速连接实现数据的集中存储，主库与备库挂载相同的文件系统。

5.分布式协议
MySQL cluster（使用NDB引擎实时备份冗余数据，实现数据库的高可用性和数据一致性）、**Galera（多主数据同步，支持Innodb）**、**POAXS**

https://zhuanlan.zhihu.com/p/25960208

mysql udf 用户自定义函数，
