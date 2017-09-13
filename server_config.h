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

#ifndef _SERVER_CONFIG_H_
#define _SERVER_CONFIG_H_

#include "logger.h"

namespace emoskit {

	class ServerConfig
	{
	public:
		ServerConfig();
		~ServerConfig();

		bool ReadConfigFile(const char* file_path);


		std::string server_ip() const { return server_ip_; }
		void set_server_ip(std::string val) { server_ip_ = val; }

		int server_port() const { return server_port_; }
		void set_server_port(int val) { server_port_ = val; }

		int event_loop_count() const { return event_loop_count_; }
		void set_event_loop_count(int val) { event_loop_count_ = val; }

		int thread_pool_core_pool_size() const { return thread_pool_core_pool_size_; }
		void set_thread_pool_core_pool_size(int val) { thread_pool_core_pool_size_ = val; }

		int thread_pool_max_pool_size() const { return thread_pool_max_pool_size_; }
		void set_thread_pool_max_pool_size(int val) { thread_pool_max_pool_size_ = val; }

		int thread_pool_keep_alive_time_seconds() const { return thread_pool_keep_alive_time_seconds_; }
		void set_thread_pool_keep_alive_time_seconds(int val) { thread_pool_keep_alive_time_seconds_ = val; }

		int circbuf_size() const { return circbuf_size_; }
		void set_circbuf_size(int val) { circbuf_size_ = val; }

		std::string log_dir() const { return log_dir_; }
		void set_log_dir(std::string val) { log_dir_ = val; }

		LoggerLevel log_level() const { return log_level_; }
		void set_log_level(int level) {
			switch (level)
			{
			case 1: log_level_ = LOG_LEVEL_DEBUG; break;
			case 2: log_level_ = LOG_LEVEL_INFO; break;
			case 3: log_level_ = LOG_LEVEL_WARN; break;
			case 4: log_level_ = LOG_LEVEL_ERROR; break;
			default:
				emoskit_log(WARN_LOG, "%s = %d, but it must between 1-4 (DEBUG:1, INFO:2, WARN:3, ERROR:4), set to ERROR by default.", "LogLevel", level);
				log_level_ = LOG_LEVEL_ERROR;
				break;
			}
		}

	private:
		std::string server_ip_;
		int server_port_;

		int event_loop_count_;

		int thread_pool_core_pool_size_;
		int thread_pool_max_pool_size_;
		int thread_pool_keep_alive_time_seconds_;

		int circbuf_size_;

		std::string log_dir_;
		LoggerLevel log_level_;
		/* may be more properties */
	};

}
#endif // !_SERVER_CONFIG_H_

