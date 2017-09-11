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

#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include <string>
#include <unordered_map>

namespace emoskit {
	
	class ConfigParser
	{
	public:
		ConfigParser();
		~ConfigParser();

		bool Init(const char* file_path);
		bool ParseConfig();

		bool GetItem(const std::string& section, const std::string& key, std::string* value);
		bool GetItem(const std::string& section, const std::string& key, int* value);

		bool GetItem(const std::string& section, const std::string& key, std::string* value, const std::string default_value);
		bool GetItem(const std::string& section, const std::string& key, int* value, const int& default_value);

		void set_contents(const std::string contents);

	private:
		int CountLine(int end_pos);
		int ParseSectionName(int pos);
		int ParseKeyName(int pos);
		int SkipSpace(int pos);

		std::string contents_;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config_kv_;
	};

}

#endif // !

