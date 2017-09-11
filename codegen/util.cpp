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

#include <algorithm>
#include <stdarg.h>
#include <stdio.h>

#include "util.h"

namespace emoskit {

	namespace util {
		
		void 
		StrReplaceAll(std::string & str, const std::string & from, const std::string & to){
			size_t pos = 0;
			while ((pos = str.find(from, pos)) != std::string::npos) {
				str.replace(pos, from.length(), to);
				pos += to.length();
			}
		}

		void 
		StrAppendFormat(std::string * str, const char * fmt, ...){
			if (nullptr == fmt)
				return;

			size_t append_str_len = 0;

			va_list va;
			va_start(va, fmt);
			append_str_len = vsnprintf(NULL, 0, fmt, va);
			va_end(va);
			
			size_t str_len = str->length();
			
			str->resize(str_len + append_str_len);

			va_start(va, fmt);
			vsnprintf((char*)str->data() + str_len, append_str_len + 1, fmt, va);
			va_end(va);
		}

		std::string&
		StrTrim(std::string & str, const char * trim){
			size_t first = str.find_first_not_of(trim);

			if (first == std::string::npos)
				str = "";
			else {
				size_t last = str.find_last_not_of(trim);
				str = str.substr(first, (last - first + 1));
			}
			return str;
		}

		bool 
		StrEndsWith(const std::string& value, const std::string& ending){
			if (ending.size() > value.size()) return false;
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}

		char * 
		ToUpper(char * str){
			char *ret = str;

			for (; *str != '\0'; ++str)
				*str = toupper(*str);

			return ret;
		}

		char * 
		ToLower(char * str){
			char *ret = str;

			for (; *str != '\0'; ++str)
				*str = tolower(*str);

			return ret;
		}

	}

}