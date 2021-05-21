/* udf 使用

*1.使用
sudo apt-get install libmysqld-dev // 安装依赖
g++ -I/usr/include/mysql -shared -fPIC -o mudf_test.so mudf_test.cpp // 编译为动态库
sudo cp mudf_test.so /usr/lib/mysql/plugin/ // 拷贝到mysql扩展目录

*2.mysql操作：
# 如果存在自定义函数,删除函数str_reverse
DROP FUNCTION IF EXISTS str_reverse; 
DROP FUNCTION IF EXISTS mysum;

# CREATE FUNCTION xxxx(function) RETURNS xxxx(return type) SONAME 'xxxx.so';
CREATE FUNCTION str_reverse RETURNS STRING SONAME 'mudf_test.so';
CREATE AGGREGATE FUNCTION mysum RETURNS INTEGER SONAME 'mudf_test.so';

# 测试
create database test
use test;
CREATE TABLE salary( name varchar(64) NOT NULL DEFAULT '' COMMENT 'name', salary int(11) NOT NULL DEFAULT 0 COMMENT 'salary', primary key(name) )ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'test';
insert into salary values ('zhangsan', 11380), ('lisi', 12000), ('wangwu', 8789);

select mysum(name) from salary;  // 使用错误的操作,返回error ERROR 1123 (HY000): Can't initialize function 'mysum'; wrong argument type of arguments: mysum() requires int
select mysum(salary) from salary; // 使用正确的操作,返回结果

+---------------+
| mysum(salary) |
+---------------+
|         32169 |
+---------------+
1 row in set (0.01 sec)

*3.函数编写
xxx();
xxx_init();
xxx_deinit();
xxx_clear();
xxx_add();

# 变量类型
SQL 类型    C/C++ 类型
STRING      char *
INTEGER     long long
REAL        double

https://www.cnblogs.com/linuxbug/p/4950626.html

# MySQL按下列操作来处理集合UDF：
1.调用 xxx_init() 让集合函数分配它需要用来存储结果的内存。
2.按照GROUP BY表达式来排序表。
3.为每个新组中的第一行调用xxx_clear()函数。
4.为属于同组的每一个新行调用xxx_add()函数。
5.当组改变时或每组的最后一行被处理完之后，调用xxx()来获取集合结果。
6.重复，以上3步直到所有行被处理完。
7.调用xxx_deinit() 函数去释放UDF分配的内存。

typedef struct st_udf_init
{
  my_bool maybe_null;              // 1 if function can return NULL xxx()函数可能返回NULL
  unsigned int decimals;           // for real function 小数位数,默认值是传到主函数的参量里小数的最大位数.
                                   // 如果函数传递 1.34, 1.345, 和1.3, 那么默认值为3，因为1.345 有3位小数
  unsigned long max_length;        // For string functions 结果的最大长度
                                   // 对字符串函数，默认值是结果的最大长度。对整型函数，默认是21位。对实型函数，默认是13再加上initid->decimals指示的小数位数。（对数字函数，长度包含正负号或者小数点符）
  char *ptr;                       // free pointer for function data 用户指针,自定义用途，用户自主分配、释放内存 initid->ptr = allocated_memory;
  my_bool const_item;              // 0 if result is independent of arguments 
} UDF_INIT;

enum Item_result // 返回结果类型
{
    STRING_RESULT=0,
    REAL_RESULT,
    INT_RESULT,
    ROW_RESULT,
    DECIMAL_RESULT
};

typedef struct st_udf_args
{
  unsigned int arg_count;            // Number of arguments  参数个数
  enum Item_result *arg_type;        // Pointer to item_results 参数类型列表
                                     // args->arg_type[0] = STRING_RESULT;
  char **args;                       // Pointer to argument 参数列表,根据参数类型转换
                                     // long long int_val; int_val = *((long long*) args->args[i]);
  unsigned long *lengths;            // Length of string arguments 参量的最大字符串长度
  char *maybe_null;                  // Set to 1 for all maybe_null args
  char **attributes;                 // Pointer to attribute name
  unsigned long *attribute_lengths;  // Length of attribute arguments
} UDF_ARGS;

*/

// mysql udf 自定义函数

// mysql
#include <mysql.h>
#include <mysql_com.h>

// c/cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

// 字符函数返回一个指向结果的指针，并且设置 *result 和 *length  为返回值的内容和长度.
// 如果返回长度大于255,需要自己分配内存，并在xx_init()和xx_deinit()做好分配和释放.
// 如果返回长度小于255,使用result 和 length完成返回
char* func0(UDF_INIT *initid, UDF_ARGS *args,
          char *result, unsigned long *length,
          char *is_null, char *error){
    
    if (args->arg_type[0] == STRING_RESULT) // 判断参数类型是否为 STRING_RESULT
    {   
        if (strlen(args->args[0]) > 256) // 判断参数长度
        {
            // strncpy(StrData, args->args[0], 4096);
            // StrData[4096-1] = 0;

            /* function */

            // return StrData;
        }
        else
        {
            strncpy(result, args->args[0], 256);
            result[256-1] = 0;
            
            std::reverse(result, result + strlen(result));
            *length = (unsigned long)strlen(result);
            return result;
        }
    }

    return NULL;
}

// 对于long long 和 double 类型的函数，主函数 xxx()的返回值是函数值.
long long func1(UDF_INIT *initid, UDF_ARGS *args,
              char *is_null, char *error){
    
    if (args->arg_type[0] == INT_RESULT) // 判断参数类型是否为 INT_RESULT
    {  
        long long int_val; 
        int_val = *((long long*) args->args[0]);
        
        /* function */
        
    }

    return 0 ;
}

double func2(UDF_INIT *initid, UDF_ARGS *args,
              char *is_null, char *error){
    
    if (args->arg_type[0] == REAL_RESULT) // 判断参数类型是否为 REAL_RESULT
    {  
        double long_val; 
        long_val = *((double*) args->args[0]);
        
        /* function */
        
    }

    return 0.0;
}

bool func_init(UDF_INIT *initid, UDF_ARGS *args, char *message){
    if (args->arg_count != 1)
    {    
        strcpy(message,"wrong number of arguments: str_reverse() requires one argument");    
        return 1;  
    }
    
    if (args->arg_type[0] != STRING_RESULT)  
    {    
        strcpy(message,"str_reverse() requires a string as parameter");    
        return 1;  
    }

    /* function */
    // init
    // malloc initid->ptr = data;
    // ...

    return 0;
}

void func_deinit(UDF_INIT *initid){
    // free(data) initid->ptr = nullptr;
}

// 对于集合函数 AGGREGATE FUNCTION
void func_reset(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    // 同func_clear().
}

void func_clear(UDF_INIT *initid, char *is_null, char *error)
{
    // 为每个新组中的第一行调用xxx_clear()函数.
}

void func_add(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *error)
{
    // 为属于同组的每一个新行调用xxx_add()函数.
    // *(int*)(args->args[0]);
}
