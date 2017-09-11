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

#ifndef _METHOD_HANDLER_H_
#define _METHOD_HANDLER_H_

#include <string>
#include <functional>
#include "server_context.h"
#include "status.h"
#include "util.h"

namespace emoskit {
	
	class MethodHandler {
	public:
		virtual ~MethodHandler() {}
		virtual Status RunHandler(ServerContext* server_context, const std::string* param, std::string* result) = 0;
	};

	template <class ServiceType, class RequestType, class ResponseType>
	class MethodHandlerImpl : public MethodHandler {
	public:

		typedef std::function<Status(ServiceType*, ServerContext*, const RequestType&, ResponseType*)> ServiceFunc;

		MethodHandlerImpl(ServiceFunc func,
			ServiceType* service)
			: func_(func), service_(service) {}
		~MethodHandlerImpl(){}

		Status RunHandler(ServerContext* server_context, const std::string* param, std::string* result) final {
			RequestType request;
			if (expect_false(!request.ParseFromString(*param))){
				return Status(StatusCode::PROTOCOL_ERROR, "");
			}
			ResponseType reply;
			Status status = func_(service_, server_context, request, &reply);
			reply.SerializeToString(result);
			return status;
		}
	private:
		// Application provided handler function.
		ServiceFunc func_;
		// The class the above 'func_' function lives in.
		ServiceType* service_;
	};
}
#endif