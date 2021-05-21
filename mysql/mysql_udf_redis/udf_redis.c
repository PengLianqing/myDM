#include "udf_redis.h"sendv


#define safe_free(x) if(x){free(x);x=NULL;}

struct credis
{
	int                fd;
	struct sockaddr_in addr;
};

struct credis *res = NULL;

my_bool redis_connect_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = INT_RESULT;
	initid->ptr       = NULL;
	
	return 0;
}


char *redis_connect(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	uint16_t port;
	
//	FILE *fp;
	if(res != NULL)
	{
/*		fp = fopen("/usr/local/mysql/debug.log", "a");
		fprintf(fp, "res->fd = %d\r\n", res->fd);
		fclose(fp);
*/		credis_close(res);
	}
	port  = *((ushort*) args->args[1]);
	res = credis_connect(args->args[0], port);
	if (res == NULL)
	{
		*length = 60;
		return "Error connecting to Redis server. Please start server to run";
	}

	*length = 2;
	return "OK";
}


my_bool redis_close_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	initid->ptr       = NULL;	
	return 0;
}

char *redis_close(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	credis_close(res);

	*length = 2;
	return "OK";
}

my_bool redis_set_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_set(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	

	*length = 2;
	rc = credis_set(res, args->args[0], args->args[1]);

	return (rc) ? "OK" : "NO";
}

my_bool redis_delete_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 1)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_delete(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	

	*length = 2;
	rc = credis_delete(res, args->args[0]);

	return (rc) ? "OK" : "NO";
}

my_bool redis_incr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 1)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_incr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	

	*length = 2;
	rc = credis_incr(res, args->args[0]);

	return (rc) ? "OK" : "NO";
}


my_bool redis_incrBy_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = INT_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_incrBy(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	int  val;
	
	val  = *((int*) args->args[1]);

	*length = 2;
	rc = credis_incrBy(res, args->args[0], val);

	return (rc) ? "OK" : "NO";
}


my_bool redis_decrBy_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = INT_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_decrBy(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	int  val;
	
	val  = *((int*) args->args[1]);

	*length = 2;
	rc = credis_decrBy(res, args->args[0], val);

	return (rc) ? "OK" : "NO";
}


my_bool redis_decr_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 1)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_decr(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	
	if(res == NULL)
	{
		return NULL;
	}

	*length = 2;
	rc = credis_decr(res, args->args[0]);

	return (rc) ? "OK" : "NO";
}


my_bool redis_lPush_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_lPush(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	

	*length = 2;
	rc = credis_lPush(res, args->args[0], args->args[1]);

	return (rc) ? "OK" : "NO";
}


my_bool redis_rPush_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 2)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

char *redis_rPush(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	bool rc;
	

	*length = 2;
	rc = credis_rPush(res, args->args[0], args->args[1]);

	return (rc) ? "OK" : "NO";
}



my_bool redis_pipe_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count != 1)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;

	initid->ptr       = NULL;	
	return 0;
}

void redis_pipe_deinit(UDF_INIT *initid)
{
	safe_free(initid->ptr);
	return;
}

char *redis_pipe(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
	char *str;

	FILE *fp;

	fp = fopen("/usr/local/mysql/debug.log", "a");
	fprintf(fp, "redis_pipe = %s\r\n", args->args[0]);
	fclose(fp);
	
	str = credis_pipe(res, (char *)args->args[0]);
	if(str == NULL)
	{
		*length = 3;
		return "err";
	}
	
	initid->ptr = str;

	*length = strlen(str);
	return str;
}


my_bool json_object_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	if (args->arg_count < 1)
	{
		strcpy(message,"Wrong arguments to metaphon;  Use the source");
		return 1;
	}
	
	initid->ptr       = NULL;	
	return 0;
}


void json_object_deinit(UDF_INIT *initid)
{
	safe_free(initid->ptr);
}


char *json_object(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{

	cJSON *root;
	char *out;
	int i = 0;

	root=cJSON_CreateObject();
	for(i = 0;i<args->arg_count;i++)
	{
		if(args->arg_type[i] == STRING_RESULT)
		{
		    cJSON_AddStringToObject(root, args->attributes[i], args->args[i]);
		}
		else if(args->arg_type[i] == INT_RESULT)
		{
			cJSON_AddNumberToObject(root, args->attributes[i], *((int*)args->args[i]));
		}
	}
	out=cJSON_Print(root);
    cJSON_Delete(root);
	*length = strlen(out);

	initid->ptr       = out;	
	return out;
	
}


//------华丽的分割线--------

bool safe_write(int sockfd, const char *buf, const int len)
{
	ssize_t sent;
	size_t  remaining, pos;

	pos       = 0;
	remaining = len;

	do {

		sent = write(sockfd, buf+pos, remaining); //Linux specific
		if (sent <= 0)
			return false; // write error or whatever

		pos       += sent;
		remaining -= sent;
	}
	while (remaining > 0);

	return true;
}

bool sendv(int fd, struct iovec *vec , size_t c)
{ 
	int wrote, left; 
	struct iovec *iov; 

	iov = vec; 
	while (c > 0)
	{ 
		wrote = writev(fd, iov, c); 
		if(wrote < 0) 
		{ 
			if(errno == EAGAIN) continue; 
			//printf("Unexpected writev error %d\n", errno); 
			return false;
		}

		for ( ; c; iov++,c--) 
		{ 
			left = wrote - iov->iov_len; 
			if(left >= 0)
			{ 
					wrote -= iov->iov_len; 
					continue; 
			} 
			iov->iov_len -= wrote; 
			iov->iov_base += wrote; 
			break; 
		}
	}

	return true;
}

struct credis *credis_connect(char *host, uint16_t port)
{
	struct timeval timeout;
	struct credis  *redis;

	redis = (struct credis *)malloc(sizeof(struct credis));
	if(redis == NULL) return NULL;
	
	redis->fd                   = socket(AF_INET, SOCK_STREAM, 0);
	redis->addr.sin_family      = AF_INET;         
	redis->addr.sin_port        = htons(port);         
	redis->addr.sin_addr.s_addr = inet_addr(host);
	
	timeout.tv_sec  = 5;  
	timeout.tv_usec = 0;

	setsockopt(redis->fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
	setsockopt(redis->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	if(connect(redis->fd, (struct sockaddr *)&redis->addr, sizeof(struct sockaddr)) == -1)        
	{
		safe_free(redis);
		return NULL;
	}

	return redis;
}

void credis_close(struct credis  *redis)
{
	if(redis == NULL) return ;

	close(redis->fd);
	
	safe_free(redis);
}


bool credis_set(struct credis  *redis, char *key, char *value)
{     
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[5];
	
	if(redis == NULL) return false;

	iov[0].iov_base = "SET ";
	iov[0].iov_len  = 4;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = " ";
	iov[2].iov_len  = 1;
	iov[3].iov_base = value;
	iov[3].iov_len  = strlen(value);
	iov[4].iov_base = "\r\n";
	iov[4].iov_len  = 2;
	
	
	rc = sendv(redis->fd, iov, 5);	
	if(!rc) return false;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 49, 0);
	
	return (strncmp(buf, "+OK", 3) == 0) ? true : false;
}

bool credis_delete(struct credis  *redis, char *key)
{
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[3];
	
	memset(buf, 0, sizeof(buf));
	
	if(redis == NULL) return false;

	iov[0].iov_base = "DEL ";
	iov[0].iov_len  = 4;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = "\r\n";
	iov[2].iov_len  = 2;

	rc = sendv(redis->fd, iov, 3);
	if(!rc) return false;

	nbytes = recv(redis->fd, buf, 49, 0);
	if(nbytes == -1) return false;
	
	return (strncmp(buf, ":1", 2) == 0) ? true : false;
}


bool credis_incr(struct credis  *redis, char *key)
{
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[3];
	
	memset(buf, 0, sizeof(buf));
	
	if(redis == NULL) return NULL;

	iov[0].iov_base = "INCR ";
	iov[0].iov_len  = 5;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = "\r\n";
	iov[2].iov_len  = 2;

	rc = sendv(redis->fd, iov, 3);
	if(!rc) return false;
	
	nbytes = recv(redis->fd, buf, 49, 0);
	if(nbytes < 1) return false;

	return true;
}

bool credis_incrBy(struct credis  *redis, char *key, int val)
{
	int     len;
	bool    rc;
	char    *str;
	char    buf[50];
	ssize_t nbytes;
	
	if(redis == NULL) return false;

	len = asprintf(&str, "INCRBY %s %d\r\n", key, val);
	
	rc = safe_write(redis->fd, str, len);
	safe_free(str);

	if(!rc) return false;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 49, 0);
	if(nbytes < 1) return false;
	
	return true;
}

bool credis_decrBy(struct credis  *redis, char *key, int val)
{
	int     len;
	bool    rc;
	char    *str;
	char    buf[50];
	ssize_t nbytes;
	
	if(redis == NULL) return false;

	len = asprintf(&str, "DECRBY %s %d\r\n", key, val);
	
	rc = safe_write(redis->fd, str, len);
	safe_free(str);

	if(!rc) return false;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 49, 0);
	if(nbytes < 1) return false;
	
	return true;
}

bool credis_decr(struct credis  *redis, char *key)
{
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[3];
	
	memset(buf, 0, sizeof(buf));
	
	if(redis == NULL) return false;

	iov[0].iov_base = "DECR ";
	iov[0].iov_len  = 5;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = "\r\n";
	iov[2].iov_len  = 2;

	rc = sendv(redis->fd, iov, 3);
	if(!rc) return false;
	
	nbytes = recv(redis->fd, buf, 49, 0);
	if(nbytes < 1) return false;

	return true;
}

int credis_lPush(struct credis  *redis, char *key, char *value)
{     
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[5];
	
	if(redis == NULL) return false;

	iov[0].iov_base = "LPUSH ";
	iov[0].iov_len  = 6;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = " ";
	iov[2].iov_len  = 1;
	iov[3].iov_base = value;
	iov[3].iov_len  = strlen(value);
	iov[4].iov_base = "\r\n";
	iov[4].iov_len  = 2;
	
	rc = sendv(redis->fd, iov, 5);	
	if(!rc) return 0;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 49, 0);
	
	if(nbytes < 1) return 0;
	
	return atoi(buf+1);
}


int credis_rPush(struct credis  *redis, char *key, char *value)
{     
	bool    rc;
	char    buf[50];
	ssize_t nbytes;

	struct iovec   iov[5];
	
	if(redis == NULL) return false;


	iov[0].iov_base = "RPUSH ";
	iov[0].iov_len  = 6;
	iov[1].iov_base = key;
	iov[1].iov_len  = strlen(key);
	iov[2].iov_base = " ";
	iov[2].iov_len  = 1;
	iov[3].iov_base = value;
	iov[3].iov_len  = strlen(value);
	iov[4].iov_base = "\r\n";
	iov[4].iov_len  = 2;
	
	rc = sendv(redis->fd, iov, 5);	
	if(!rc) return 0;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 49, 0);
	
	if(nbytes < 1) return 0;
	
	return atoi(buf+1);
}

char *credis_pipe(struct credis  *redis, char *cmds)
{
	char    buf[100];
	bool    rc;

	ssize_t nbytes;
	
	if(redis == NULL) return NULL;

	rc = safe_write(redis->fd, cmds, strlen(cmds));
	if(!rc) return NULL;

	memset(buf, 0, sizeof(buf));
	nbytes = recv(redis->fd, buf, 99, 0);
	if(nbytes < 1) return NULL;
	
	return strdup(buf);
}
