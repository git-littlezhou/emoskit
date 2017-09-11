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

#include <cctype>

#include "config_parser.h"
#include "util.h"

namespace emoskit {

	ConfigParser::ConfigParser()
	{
	}

	ConfigParser::~ConfigParser()
	{
	}

	bool 
	ConfigParser::Init(const char* file_path) {
		bool ret = util::ReadFile(file_path, &contents_);
		if (ret)
			ret = ParseConfig();
		return ret;
	}

	bool
	ConfigParser::GetItem(const std::string& section, const std::string& key, std::string* value, const std::string default_value) {
		bool ret = GetItem(section, key, value);
		if (!ret) {
			*value = default_value;
		}
		return ret;
	}

	bool
	ConfigParser::GetItem(const std::string& section, const std::string& key, int* value, const int& default_value) {
		bool ret = GetItem(section, key, value);
		if (!ret) {
			*value = default_value;
		}
		return ret;
	}

	bool
	ConfigParser::GetItem(const std::string& section, const std::string& key, int* value) {
		std::string temp_value;
		bool ret = GetItem(section, key, &temp_value);
		if (ret) {
			*value = std::atoi(temp_value.c_str());
		}
		return ret;
	}

	bool 
	ConfigParser::GetItem(const std::string& section, const std::string& key, std::string* value) {
		auto sec = config_kv_.find(section);

		if (sec != config_kv_.end()) {

			auto k = sec->second.find(key);

			if (k != sec->second.end()) {
				*value = k->second;
				return true;
			}
		}

		return false;
	}

	void 
	ConfigParser::set_contents(const std::string contents) {
		this->contents_ = contents;
	}

	bool
	ConfigParser::ParseConfig() {
		std::string section = "";
		std::string key = "";
		int pos = 0, section_name_end_pos, key_name_end_pos, value_end_pos, temp_pos;

		for (;;) {
			while (std::isspace(contents_[pos]))
				++pos;

			if (contents_[pos] == '\0')
				return true;

			/* ignore annotation */
			if (contents_[pos] == '#') {
				while (contents_[pos] != '\n' && contents_[pos] != '\0')
					++pos;
				continue;
			}

			/* handle section */
			if (contents_[pos] == '[') {
				++pos;

				/* leading spaces */
				while (contents_[pos] == ' ')
					++pos;

				/* section name can only contains alpha or digit or '_' */
				section_name_end_pos = ParseSectionName(pos);		

				if (section_name_end_pos == pos) {
					emoskit_log(ERROR_LOG, "unrecognized section name, section name can only contains alpha or digit or '_', line %d", CountLine(section_name_end_pos));
					break;
				}

				section = contents_.substr(pos, section_name_end_pos - pos);
				if (config_kv_.find(section) != config_kv_.end()) {
					emoskit_log(ERROR_LOG, "duplicated section: %s, line: %d", section.c_str(), CountLine(pos));
					break;
				}

				std::unordered_map<std::string, std::string > map;
				config_kv_[section] = map;

				pos = SkipSpace(section_name_end_pos);

				if(contents_[pos] != ']'){
					emoskit_log(ERROR_LOG, "parse ConfigParser file error, expect ']' after section name and section name can only contains alpha or digit or '_', line %d", CountLine(pos));
					break;
				}else ++pos;
			}

			/* handle ConfigParser key and value
			 * key can only contains digit or alpha or '_'
			 */
			else if (std::isalpha(contents_[pos]) || std::isdigit(contents_[pos]) || contents_[pos] == '_') {

				if (section == "") {
					emoskit_log(ERROR_LOG, "ConfigParser key value has no section, line %d", CountLine(pos));
					break;
				}

				key_name_end_pos = ParseKeyName(pos);

				if(key_name_end_pos == pos){
					emoskit_log(ERROR_LOG, "unrecognized key name, key name can only contains digit or alpha or '_', line %d", CountLine(key_name_end_pos));
					break;
				}

				key = contents_.substr(pos, key_name_end_pos - pos);

				temp_pos = pos;
				pos = SkipSpace(key_name_end_pos);

				if (contents_[pos] != '=') {
					emoskit_log(ERROR_LOG, "parse ConfigParser file error, expect '=' after key name, line %d", CountLine(temp_pos));
					break;
				}
				else ++pos;

				temp_pos = pos - 1;
				pos = SkipSpace(pos);

				value_end_pos = pos;
				/* get value */
				while (!std::isspace(contents_[value_end_pos]) && contents_[value_end_pos] != '\0')
					++value_end_pos;

				if (value_end_pos == pos) {
					emoskit_log(ERROR_LOG, "parse ConfigParser file error, expect value after '=', line %d", CountLine(temp_pos));
					break;
				}

				config_kv_[section][key] = contents_.substr(pos, value_end_pos - pos);

				pos = value_end_pos;

			}
			else {
				/* unrecognized characters*/
				emoskit_log(ERROR_LOG, "unrecognized characters: %c (key not start with digit or alpha or '_' or section not start with '['), line: %d", contents_[pos], CountLine(pos));
				break;
			}
		}
		return false;
	}

	int 
	ConfigParser::CountLine(int end_pos) {
		int line = 1, pos = 0;
		while (pos <= end_pos) {
			if (contents_[pos] == '\n')
				++line;
			++pos;
		}
		return line;
	}

	int 
	ConfigParser::ParseSectionName(int pos) {
		while (std::isalpha(contents_[pos]) || std::isdigit(contents_[pos]) || contents_[pos] == '_')
			++pos;

		return pos;
	}

	int
	ConfigParser::ParseKeyName(int pos) {
		while (std::isalpha(contents_[pos]) || std::isdigit(contents_[pos]) || contents_[pos] == '_')
			++pos;

		return pos;
	}

	int 
	ConfigParser::SkipSpace(int pos) {
		while (contents_[pos] == ' ' || contents_[pos] == '\t')
			++pos;
		return pos;
	}
}
