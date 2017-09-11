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

#include <random>

#include "client_config.h"
#include "config_parser.h"
#include "logger.h"

namespace emoskit {
	
	ClientConfig::ClientConfig()
	{
	}

	ClientConfig::~ClientConfig()
	{
	}

	bool 
	ClientConfig::ReadConfigFile(const char* file_path) {
		ConfigParser config_parser;
		bool ret = config_parser.Init(file_path);

		const std::string kServerSection = "Server";
		const std::string kServerCountKey = "ServerCount";
		const std::string kIP = "IP";
		const std::string kPort = "Port";

		std::string ip;
		int port;

		if (ret) {
			bool succeed = config_parser.GetItem(kServerSection, kServerCountKey, &server_count_);

			if (!succeed) {
				emoskit_log(ERROR_LOG, "not found key '%s' under section '%s'.", kServerCountKey.c_str(), kServerSection.c_str());
				return false;
			}

			if (server_count_ <= 0) {
				emoskit_log(ERROR_LOG, "'ServerCount' must greater than 0.");
				return false;
			}

			std::string section;
			for (int i = 0; i < server_count_; ++i) {

				section = kServerSection + "_" + std::to_string(i);

				if (!config_parser.GetItem(section, kIP, &ip)) {
					emoskit_log(WARN_LOG, "not found key '%s' under section '%s'.", kIP.c_str(), section.c_str());
					continue;
				}

				if (!config_parser.GetItem(section, kPort, &port)){
					emoskit_log(WARN_LOG, "not found key '%s' under section '%s'.", kPort.c_str(), section.c_str());
					continue;
				}

				endpoints_.emplace_back(ip.c_str(), port);
			}

			if (endpoints_.size() == 0){
				emoskit_log(ERROR_LOG, "not found any server endpoint");
				return false;
			}
		}
		return ret;
	}

	Endpoint* 
	ClientConfig::GetEndpointRandom() {

		Endpoint* endpoint_ptr = nullptr;

		if (endpoints_.size() > 0){
			/* a uniformly-distributed integer random number generator that produces non-deterministic random numbers.
			 * used to obtain a seed for the random number engine
			 */
			std::random_device rd;
			/* produces high quality unsigned integer random numbers of type UIntType */
			std::mt19937 generator(rd());

			std::uniform_int_distribution<unsigned int> dis(0, endpoints_.size() - 1);

			endpoint_ptr = &endpoints_[dis(generator)];
		}

		if (!endpoint_ptr) {
			emoskit_log(ERROR_LOG, "get endpoint randomly error, there may be no endpoint.");
		}

		return endpoint_ptr;
	}

	Endpoint* 
	ClientConfig::GetEndpointAt(unsigned int idx) {

		Endpoint* endpoint_ptr = nullptr;

		if (idx >= 0 && idx < endpoints_.size()) {
			endpoint_ptr = &endpoints_[idx];
		}

		if (!endpoint_ptr) {
			emoskit_log(ERROR_LOG, "get endpoint by index error, the index may be out of range.");
		}

		return endpoint_ptr;
	}
}