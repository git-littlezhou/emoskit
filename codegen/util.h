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

#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>

namespace emoskit {
	
	namespace util {
	
		void StrReplaceAll(std::string& str, const std::string& from, const std::string& to);

		void StrAppendFormat(std::string* str, const char* fmt, ...);

		std::string& StrTrim(std::string& str, const char* trim = " \t\r\n");

		bool StrEndsWith(const std::string& value, const std::string& ending);

		char* ToUpper(char* str);

		char* ToLower(char* str);

	}
}
#endif // !_UTIL_H_

