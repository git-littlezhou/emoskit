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

#ifndef _METHOD_H_
#define _METHOD_H_

#include <stdint.h>
#include <string.h>
#include "murmur_hash.h"

namespace emoskit {

	class  Method {
	public:
		enum MethodType {
			NORMAL_METHOD = 0
			/*may be more type*/
		};

		Method(const char* method_name, MethodType method_type)
			: method_name_(method_name), method_type_(method_type){
			hash_code_ = murmur_hash3(method_name_, strlen(method_name_), 20161001);
		}

		const char* method_name()const{ return method_name_; }
		MethodType method_type()const { return method_type_; }
		uint32_t hash_code()const { return hash_code_; }

	private:
		const char* const method_name_;
		uint32_t hash_code_;
		MethodType method_type_;
	};
}
#endif
