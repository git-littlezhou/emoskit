/*
*
* Copyright 2017 emsokit authors.
* All rights reserved.
*
* Licensed under the BSD 3-Clause License (the "License"); you may
* not use this file except in compliance with the License. You may
* obtain a copy of the License at
*
*	https://opensource.org/licenses/BSD-3-Clause
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
* implied. See the License for the specific language governing
* permissions and limitations under the License.
*
*/

#ifndef  _LOGGER_H_
#define  _LOGGER_H_

#include <stdarg.h>

#include "gcc_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LoggerLevel{
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR
}LoggerLevel;

#define DEBUG_LOG LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__
#define INFO_LOG LOG_LEVEL_INFO, __FILE__, __LINE__, __func__
#define WARN_LOG LOG_LEVEL_WARN, __FILE__, __LINE__, __func__
#define ERROR_LOG LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__

typedef void(*log_print_function)(LoggerLevel log_level, const char *file, int line, const char* func, const char* log_message);

void emoskit_log(LoggerLevel log_level, const char *file, int line, const char* func, const char* format, ...)__attribute__((format(printf, 5, 6)));
void emoskit_set_log_level(LoggerLevel log_level);
void emoskit_set_log_func(log_print_function func);

inline const char* 
emoskit_log_level_string(LoggerLevel log_level){
	switch (log_level)
	{
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	default:
		return "UNKNOW LOG LEVEL";
	}
}

#ifdef __cplusplus
}
#endif

#endif
