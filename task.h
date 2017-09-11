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

#ifndef _TASK_H_
#define _TASK_H_

#include "server.h"
#include "call.h"
#include "util.h"

namespace emoskit {

	typedef struct Task Task;

	struct Task{
		util_fd_t fd = -1;

		Call call;
		protobuf::Request request_content;
		std::string store_response;

		EventLoop* event_loop;

		void InitCall(ServerContext* server_context, ServiceMethod* service_method, const ::std::string* params){
			call.set_server_context(server_context);
			call.set_service_method(service_method);
			call.set_params(params);
		}

		void operator()(){
			if (fd == -1)
				return;

			store_response.clear();

			//Status result = event_loop->server->service_->CallMethod(
			//	&(event_loop->server->server_context_), request.method_id(), request.contents(), store_response);
			Status result = call(&store_response);

			llong seq;
			common::MsgQueue<Response>* response_queues = &event_loop->response_queue;
			Response& response = response_queues->TakeObjectAndSeq(seq);

			response.response_content.Clear();

			response.response_content.set_contents(store_response);

			if (expect_true(result.is_ok())){
				response.response_content.set_response_code(protobuf::OK);
				//response.response_content.set_contents(request.contents().c_str());
			}
			else{
				response.response_content.set_response_code(protobuf::ERROR);
				response.response_content.set_info(result.message().c_str());
			}

			//response = new Response;
			//memcpy(response.buf, buf, sizeof(buf));
			response.fd = fd;

			response_queues->Publish(seq);
			response_queues->set_flag_and_notify();

			//write(fd, buf, sizeof(buf));
		}
	};

}
#endif