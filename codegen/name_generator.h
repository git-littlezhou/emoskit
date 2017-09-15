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

#ifndef _NAME_GENERATOR_H_
#define _NAME_GENERATOR_H_

#include <stddef.h>

namespace emoskit {

	class NameGenerator
	{
	public:
		NameGenerator();
		~NameGenerator();

		virtual const char* GetIDLClassName(const char* type, char* dest, size_t size);
		virtual const char* GetIDLFileName(const char* name, char* dest, size_t size);

		virtual const char* GetStubFileName(const char* name, char* dest, size_t size);
		virtual const char* GetStubClassName(const char* name, char* dest, size_t size);

		virtual const char* GetClientFileName(const char* name, char* dest, size_t size);
		virtual const char* GetClientClassName(const char* name, char* dest, size_t size);

		virtual const char* GetServiceFileName(const char* name, char* dest, size_t size);
		virtual const char* GetServiceClassName(const char* name, char* dest, size_t size);

		virtual const char* GetServiceImplFileName(const char* name, char* dest, size_t size);
		virtual const char* GetServiceImplClassName(const char* name, char* dest, size_t size);

		virtual const char* GetServerMainFileName(const char* name, char* dest, size_t size);
		virtual const char* GetClientMainFileName(const char* name, char* dest, size_t size);

		virtual const char* GetMethodNamesFileName(const char* name, char* dest, size_t size);
		
		virtual const char* GetClientConfigFileName(const char* name, char* dest, size_t size);
		virtual const char* GetServerConfigFileName(const char * name, char * dest, size_t size);

		virtual const char* GetClientExceFileName(const char* name, char* dest, size_t size);
		virtual const char* GetServerExceFileName(const char * name, char * dest, size_t size);

		virtual const char* GetDefineHeader(const char* hpp_file_name, char* dest, size_t size);

	private:

	};

}
#endif // !_NAME_GENERATOR_H_

