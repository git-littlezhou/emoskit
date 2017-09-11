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

#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <vector>

namespace emoskit{

	#define NAME_LEN 256
	#define TYPE_LEN 256
	
	class SyntaxParam {
	public:
		SyntaxParam();
		~SyntaxParam();
		
		void set_param_name(const char* param_name);
		const char* param_name()const;
		
		void set_param_type(const char* param_type);
		const char* param_type()const;
		
	private:
		char param_name_[NAME_LEN];
		char param_type_[TYPE_LEN];
	};
	
	class SyntaxFunc{
	public:
		SyntaxFunc();
		~SyntaxFunc();
		
		SyntaxParam* req();
		SyntaxParam* rsp();
		
		void set_func_name(const char* func_name);
		const char* func_name()const;
		
	private:
		SyntaxParam req_;
		SyntaxParam rsp_;
		
		char func_name_[NAME_LEN];
	};
	
	typedef std::vector<SyntaxFunc> FuncVector;

	class SyntaxTree {
	public:
		SyntaxTree();
		~SyntaxTree();
		
		void set_proto_file_name(const char* proto_file_name);
		const char* proto_file_name()const;

		void set_package_name(const char* package_name);
		const char* package_name()const;

		void set_service_name(const char* package_name);
		const char* service_name()const;

		const FuncVector* func_list()const;
		FuncVector* func_list();

	private:
		char proto_file_name_[NAME_LEN];
		char package_name_[NAME_LEN];
		char service_name_[NAME_LEN];

		FuncVector func_list_;
	};
}
#endif // !_SYNTAX_H_

