#pragma once

#include<stdio.h>

#define HL_RED          "\033[1;31m"				//高亮红色
#define HL_GREEN		"\033[1;32m"				//高亮绿色
#define HL_YEL          "\033[1;33m"				//高亮黄色
#define BROWN           "\033[0;33m"				//灰色

enum LOG_LEVEL
{
    LOG_LEVEL_OFF = 0,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DBG,
    LOG_LEVEL_ALL,
};

static const int log_level = LOG_LEVEL_DBG;

#define LOGE(fmt, args ...) \
do \
{ \
	if(log_level >= LOG_LEVEL_ERR) \
	printf(HL_RED "WARN  (%s|%d): " \
	                    fmt, __func__, __LINE__, ## args); \
} \
while(0)

#define LOGW(fmt, args ...) \
do \
{ \
	if(log_level >= LOG_LEVEL_WARN) \
	printf(HL_YEL "WARN  (%s|%d): " \
	                    fmt, __func__, __LINE__, ## args); \
} \
while(0)

#define LOGI(fmt, args ...) \
do \
{ \
	if(log_level >= LOG_LEVEL_INFO) \
	printf(HL_GREEN "WARN  (%s|%d): " \
	                    fmt, __func__, __LINE__, ## args); \
} \
while(0)

#define LOGD(fmt, args ...) \
do \
{ \
	if(log_level >= LOG_LEVEL_DBG) \
	printf(BROWN "WARN  (%s|%d): " \
	                    fmt, __func__, __LINE__, ## args); \
} \
while(0)

