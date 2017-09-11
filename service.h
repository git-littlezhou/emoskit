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

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include "service_method.h"
#include "status.h"
#include "murmur_hash.h"
#include "logger.h"

namespace emoskit {
	
	class Service
	{
	public:
		Service(){}
		~Service(){}
		
		Status CallMethod(ServerContext* server_context, uint32_t method_id, const std::string* param, std::string* response) {
			auto service_method = methods_.find(method_id);
			if (expect_false(service_method == methods_.end()))
				return Status(StatusCode::METHOD_NOT_EXIST, "method not exist.");
			return service_method->second->handler()->RunHandler(server_context, param, response);
		}

		ServiceMethod* GetMethod(uint32_t method_id){
			auto service_method = methods_.find(method_id);
			if (expect_true(service_method != methods_.end())){
				return service_method->second.get();
			}
			emoskit_log(WARN_LOG, "method does not exist, method id: %u", method_id);
			return nullptr;
		}
		
	protected:
		void AddMethod(ServiceMethod* method) { 
			//methods_.emplace_back(method);
			const char* name = method->method_name();
			if (!methods_.emplace(
				std::piecewise_construct, 
				std::forward_as_tuple(method->hash_code()), 
				std::forward_as_tuple(method)
				).second){
				emoskit_log(ERROR_LOG,
					"Method %s has already been registered or has the same hash code with the existing method.", name);
				exit(0);
			}
		}
		
	private:
		//::std::vector<std::unique_ptr<ServiceMethod>> methods_;
		::std::unordered_map<uint32_t, std::unique_ptr<ServiceMethod>> methods_;
	};

}
#endif