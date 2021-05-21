DROP FUNCTION IF EXISTS `redis_connect`;
DROP FUNCTION IF EXISTS `redis_close`;
DROP FUNCTION IF EXISTS `redis_put`;
DROP FUNCTION IF EXISTS `redis_set`;
DROP FUNCTION IF EXISTS `redis_delete`;
DROP FUNCTION IF EXISTS `redis_incrBy`;
DROP FUNCTION IF EXISTS `redis_incr`;
DROP FUNCTION IF EXISTS `redis_decrBy`;
DROP FUNCTION IF EXISTS `redis_decr`;
DROP FUNCTION IF EXISTS `redis_decr`;
DROP FUNCTION IF EXISTS `redis_lPush`;
DROP FUNCTION IF EXISTS `redis_rPush`;
DROP FUNCTION IF EXISTS `redis_pipe`;
DROP FUNCTION IF EXISTS `json_object`;


create function json_object returns string soname 'udf_redis.so';
create function redis_pipe returns string soname 'udf_redis.so';
create function redis_rPush returns string soname 'udf_redis.so';
create function redis_lPush returns string soname 'udf_redis.so';
create function redis_decr returns string soname 'udf_redis.so';
create function redis_decrBy returns string soname 'udf_redis.so';
create function redis_incr returns string soname 'udf_redis.so';
create function redis_incrBy returns string soname 'udf_redis.so';
create function redis_delete returns string soname 'udf_redis.so';
create function redis_set returns string soname 'udf_redis.so';
create function redis_close returns string soname 'udf_redis.so';
create function redis_connect returns string soname 'udf_redis.so';


use phpwind;

# hits 点击数
# lastpost 最后回复时间
# lastposter 最后回复用户

DELIMITER |

DROP TRIGGER IF EXISTS sync_redis_insert;
CREATE TRIGGER sync_redis_insert
AFTER INSERT ON pw_threads
FOR EACH ROW BEGIN
    SET @tt_json = (SELECT json_object(tid,fid,icon,author,authorid,subject,toolinfo,toolfield,ifcheck,type,postdate,replies,favors,modelid,shares,topped,topreplays,locked,digest,special,state,ifupload,ifmail,ifmark,ifshield,anonymous,dig,fight,ptable,ifmagic,ifhide,inspect,tpcstatus) FROM pw_threads WHERE tid = NEW.tid LIMIT 1); 
    SET @tt_con  = (SELECT redis_connect("127.0.0.1",6378));
    SET @tt_resu = (SELECT redis_pipe(CONCAT("SET pw_threads_hits_",NEW.tid," ",NEW.hits,"\r\n","SET pw_threads_lastpost_",NEW.tid," ",NEW.lastpost,"\r\n","SET pw_threads_lastposter_",NEW.tid," ",NEW.lastposter,"\r\n","LPUSH pw_threads_",NEW.fid," ",NEW.tid,"\r\n","SET pw_threads_",NEW.tid," ",@tt_json,"\r\n")));

END |


DELIMITER ;


# UPDATE更新操作的触发器 titlefont   
/*
DELIMITER |   
DROP TRIGGER IF EXISTS sync_redis_update;   
CREATE TRIGGER sync_redis_update   
AFTER UPDATE ON pw_threads   
FOR EACH ROW BEGIN   
    SET @tt_json = (SELECT json_object(tid,fid,icon,author,authorid,subject,toolinfo,toolfield,ifcheck,type,postdate,replies,favors,modelid,shares,topped,topreplays,locked,digest,special,state,ifupload,ifmail,ifmark,ifshield,anonymous,dig,fight,ptable,ifmagic,ifhide,inspect,tpcstatus) FROM pw_threads WHERE tid = NEW.tid LIMIT 1); 
    SET @tt_con  = (SELECT redis_connect("127.0.0.1",6378));
    SET @tt_resu = (SELECT redis_pipe(CONCAT("SET pw_threads_hits_",NEW.tid," ",NEW.hits,"\r\n","SET pw_threads_lastpost_",NEW.tid," ",NEW.lastpost,"\r\n","SET pw_threads_lastposter_",NEW.tid," ",NEW.lastposter,"\r\n","SET pw_threads_",NEW.tid," ",@tt_json,"\r\n")));
 
END |   
DELIMITER ;  
*/
/* DELETE删除操作的触发器 */   
/*
DELIMITER |   
DROP TRIGGER IF EXISTS sync_redis_delete;   
CREATE TRIGGER sync_redis_delete   
AFTER DELETE ON pw_threads   
FOR EACH ROW BEGIN   
    SET @tt_con  = (SELECT redis_connect("127.0.0.1",6378));
    SET @tt_resu = (SELECT redis_pipe(CONCAT("DEL pw_threads_hits_",OLD.tid,"\r\n","DEL pw_threads_lastpost_",OLD.tid,"\r\n","DEL pw_threads_lastposter_",OLD.tid,"\r\n","RPUSH pw_threads_",NEW.fid," ",NEW.tid,"\r\n","DEL pw_threads_",OLD.tid,"\r\n")));
#    SET @tt_resu = (SELECT http_delete(CONCAT('http://192.168.8.34:1978/', OLD.id)));   
END |   
DELIMITER ;  

*/
