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

#ifndef _STATUS_H_
#define _STATUS_H_

#include <string>
#include "status_code.h"

namespace emoskit {
	
	class Status{
	public:
		Status() : code_(StatusCode::OK){}
		Status(StatusCode code, const std::string& message)
			: code_(code), message_(message){}
		StatusCode code() const { return code_; }
		std::string message() const { return message_; }
		bool is_ok() const { return code_ == StatusCode::OK; }

		const static Status& OK;
	private:
		StatusCode code_;
		std::string message_;
	};

}
#endif // !_STATUS_H_
