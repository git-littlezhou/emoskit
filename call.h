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

#ifndef _CALL_H_
#define _CALL_H_

#include <string>
#include "server_context.h"
#include "service_method.h"
#include "network_proto.pb.h"
#include "network_event_handler.h"

namespace emoskit {

	class Call {
	public:

		Call() : server_context_(nullptr), service_method_(nullptr){}

		void set_server_context(ServerContext* server_context){ server_context_ = server_context; }
		void set_service_method(ServiceMethod* service_method){ service_method_ = service_method; }
		void set_params(const ::std::string* params){ params_ = params; }

		Status operator()(::std::string* result){
			if(expect_false(service_method_ == nullptr)){
				return Status(StatusCode::METHOD_NOT_EXIST, "method not exist.");
			}
			return service_method_->handler()->RunHandler(server_context_, params_, result);
		}

	private:
		ServerContext* server_context_;
		ServiceMethod* service_method_;
		const ::std::string* params_;

		friend void ReadCb(ev_loop* loop, ev_watcher* watcher, short event);
	};
	
}
#endif // !_CALL_H_
