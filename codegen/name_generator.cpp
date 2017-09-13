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

#include <string>
#include <stdio.h>
#include <string.h>

#include "name_generator.h"
#include "util.h"

namespace emoskit {

	NameGenerator::NameGenerator()
	{
	}

	NameGenerator::~NameGenerator()
	{
	}

	const char * 
	NameGenerator::GetIDLClassName(const char * type, char * dest, size_t size){
		std::string str_type = type;

		util::StrReplaceAll(str_type, ".", "::");

		snprintf(dest, size, "::%s", str_type.c_str());

		return dest;
	}

	const char * 
	NameGenerator::GetIDLFileName(const char * name, char * dest, size_t size){	
		snprintf(dest, size, "%s", name);

		const char* proto_suffix = ".proto";

		if (util::StrEndsWith(dest, proto_suffix)) {
			dest[strlen(dest) - strlen(proto_suffix)] = '\0';
		}

		strncat(dest, ".pb", size - strlen(dest) - 1);

		return dest;
	}

	const char * 
	NameGenerator::GetStubFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_stub", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetStubClassName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%c%sStub", toupper(*name), name + 1);

		return dest;
	}

	const char * 
	NameGenerator::GetClientFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_client", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetClientClassName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%c%sClient", toupper(*name), name + 1);

		return dest;
	}

	const char * 
	NameGenerator::GetServiceFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_service", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetServiceClassName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%c%sService", toupper(*name), name + 1);

		return dest;
	}

	const char * 
	NameGenerator::GetServiceImplFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_service_impl", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetServiceImplClassName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%c%sServiceImpl", toupper(*name), name + 1);

		return dest;
	}

	const char * 
	NameGenerator::GetServerMainFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_server_main", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetClientMainFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_client_main", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetMethodNamesFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_method_names", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetClientConfigFileName(const char * name, char * dest, size_t size){
		snprintf(dest, size, "%s_client", name);

		return util::ToLower(dest);
	}

	const char *
	NameGenerator::GetServerConfigFileName(const char * name, char * dest, size_t size) {
		snprintf(dest, size, "%s_server", name);

		return util::ToLower(dest);
	}

	const char * 
	NameGenerator::GetDefineHeader(const char * hpp_file_name, char * dest, size_t size){
		snprintf(dest, size, "_%s_H_", hpp_file_name);
		return util::ToUpper(dest);
	}

}