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

#include <thread>

#include "server_config.h"
#include "config_parser.h"
#include "logger.h"

namespace emoskit{

	ServerConfig::ServerConfig(){}

	ServerConfig::~ServerConfig(){}

	bool 
	ServerConfig::ReadConfigFile(const char* file_path){
		ConfigParser config_parser;
		bool ret = config_parser.Init(file_path);

		if (!ret)
			return ret;

		const std::string kstrServerSection = "Server";
		const std::string kstrIP = "IP";
		const std::string kstrPort = "Port";
		const std::string kstrEventLoopCnt = "EventLoopCnt";
		const std::string kstrWorkerPoolCorePoolSize = "WorkerPoolCorePoolSize";
		const std::string kstrWorkerPoolMaxPoolSize = "WorkerPoolMaxPoolSize";
		const std::string kstrWorkerPoolKeepAliveTimeSeconds = "WorkerPoolKeepAliveTimeSeconds";
		const std::string kstrCircbufSize = "CircbufSize";

		const std::string kstrLogSection = "Log";
		const std::string kstrLogDir = "LogDir";
		const std::string kstrLogLevel = "LogLevel";

		// Server Section
		if (!config_parser.GetItem(kstrServerSection, kstrIP, &server_ip_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrIP.c_str(), kstrServerSection.c_str());
			return false;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrPort, &server_port_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrPort.c_str(), kstrServerSection.c_str());
			return false;
		}

		if(server_port_ <=0 ){
			emoskit_log(ERROR_LOG, "config error: %s = %d, but it must greater than 0.", kstrPort.c_str(), server_port_);
			return false;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrEventLoopCnt, &event_loop_count_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrEventLoopCnt.c_str(), kstrServerSection.c_str());
			return false;
		}

		if(event_loop_count_ <= 0){
			emoskit_log(ERROR_LOG, "config error: %s = %d, but it must greater than 0.", kstrEventLoopCnt.c_str(), event_loop_count_);
			return false;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrWorkerPoolCorePoolSize, &thread_pool_core_pool_size_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrWorkerPoolCorePoolSize.c_str(), kstrServerSection.c_str());
			return false;
		}

		if (thread_pool_core_pool_size_ <= 0) {
			emoskit_log(WARN_LOG, "%s = %d, but it must greater than 0, set to hardware_concurrency / 2 by default.", kstrWorkerPoolCorePoolSize.c_str(), thread_pool_core_pool_size_);
			thread_pool_core_pool_size_ = std::thread::hardware_concurrency() / 2;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrWorkerPoolMaxPoolSize, &thread_pool_max_pool_size_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrWorkerPoolMaxPoolSize.c_str(), kstrServerSection.c_str());
			return false;
		}

		if (thread_pool_max_pool_size_ < thread_pool_core_pool_size_) {
			emoskit_log(WARN_LOG, "%s = %d, but it must greater or equal to %s, set to %s by default.", kstrWorkerPoolMaxPoolSize.c_str(), thread_pool_max_pool_size_,
				kstrWorkerPoolCorePoolSize.c_str(), kstrWorkerPoolCorePoolSize.c_str());
			thread_pool_max_pool_size_ = thread_pool_core_pool_size_;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrWorkerPoolKeepAliveTimeSeconds, &thread_pool_keep_alive_time_seconds_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrWorkerPoolKeepAliveTimeSeconds.c_str(), kstrServerSection.c_str());
			return false;
		}

		if (!config_parser.GetItem(kstrServerSection, kstrCircbufSize, &circbuf_size_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrCircbufSize.c_str(), kstrServerSection.c_str());
			return false;
		}

		if (circbuf_size_ < 2) {
			emoskit_log(ERROR_LOG, "config error: %s = %d, but it must greater or equal than 2.", kstrCircbufSize.c_str(), circbuf_size_);
			return false;
		}
		
		// Log Section
		if (!config_parser.GetItem(kstrLogSection, kstrLogDir, &log_dir_)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrLogDir.c_str(), kstrLogSection.c_str());
			return false;
		}

		int log_level;
		if (!config_parser.GetItem(kstrLogSection, kstrLogLevel, &log_level)) {
			emoskit_log(ERROR_LOG, "config error: not found key '%s' under section '%s'.", kstrLogLevel.c_str(), kstrLogSection.c_str());
			return false;
		}

		if(log_level < 1 || log_level > 4){
			emoskit_log(ERROR_LOG, "config error: %s = %d, but it must between 1-4 (1:DEBUG, 2:INFO, 3:WARN, 4:ERROR).", kstrLogLevel.c_str(), log_level);
			return false;
		}

		set_log_level(log_level);
		
		return ret;
	}

}



