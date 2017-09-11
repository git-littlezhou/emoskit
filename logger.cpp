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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "logger.h"

// for vasprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


void default_log_print_func(LoggerLevel log_level, const char *file, int line, const char* func, const char* log_message);

static LoggerLevel emoskit_min_log_level_to_print = LOG_LEVEL_ERROR;
static log_print_function log_print_func = default_log_print_func;

#define TIME_BUFFER_SIZE 32

void 
emoskit_log(LoggerLevel log_level, const char *file, int line, const char* func, const char* format, ...){
	if (log_level < emoskit_atomic_relaxed_load(&emoskit_min_log_level_to_print))
		return;

	char* log_message = NULL;
	va_list args;
	va_start(args, format);

	if (vasprintf(&log_message, format, args) == -1) {
		va_end(args);
		return;
	}
	va_end(args);

	log_print_func(log_level, file, line, func, log_message);

	free(log_message);
}

void 
emoskit_set_log_level(LoggerLevel log_level){
	emoskit_atomic_relaxed_store(&emoskit_min_log_level_to_print, log_level);
}

void 
emoskit_set_log_func(log_print_function func) {
	log_print_func = func;
}

void 
default_log_print_func(LoggerLevel log_level, const char *file, int line, const char* func, const char* log_message){
	const char* last_slash_pos;
	const char* filename;

	// Remove the path name, get the filename
	if (file){
		last_slash_pos = strrchr(file, '/');
		if (last_slash_pos){
			filename = last_slash_pos + 1;
		}
		else{
			filename = file;
		}
	}
	else{
		filename = "";
	}

	char time_buf[TIME_BUFFER_SIZE];

	struct tm tm_now;
	struct timespec ts_now;

	// get time now, and converted into string
	if (clock_gettime(CLOCK_REALTIME_COARSE, &ts_now) == -1)
		strcpy(time_buf, "clock_gettime error");
	else{
		localtime_r(&ts_now.tv_sec, &tm_now);
		strftime(time_buf, TIME_BUFFER_SIZE, "%m%d %H:%M:%S", &tm_now);
	}

	fprintf(stderr, "[%s.%09d][Thread %9tu][%s] %s:%d, in %s: %s\n",
		time_buf, (int)(ts_now.tv_nsec), pthread_self(), 
		emoskit_log_level_string(log_level), filename, line, func, log_message);

}
