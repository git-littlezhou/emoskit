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

#ifndef _SERVICE_METHOD_H_
#define _SERVICE_METHOD_H_

#include <memory>
#include "method_handler.h"
#include "method.h"

namespace emoskit {
	
	class ServiceMethod : public Method {
	public:
		ServiceMethod(const char* method_name, MethodType method_type, MethodHandler* handler)
			: Method(method_name, method_type), handler_(handler){}

		MethodHandler* handler() const { return handler_.get(); }
		void SetHandler(MethodHandler* handler) { handler_.reset(handler); }		

	private:
		std::unique_ptr<MethodHandler> handler_;
	};

}
#endif