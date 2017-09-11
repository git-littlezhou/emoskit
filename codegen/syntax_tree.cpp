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

#include <string.h>

#include "syntax_tree.h"

namespace emoskit {

	/* start of implement of SyntaxParam */

	SyntaxParam::SyntaxParam(){
		memset(param_name_, 0, sizeof(param_name_));
		memset(param_type_, 0, sizeof(param_type_));
	}

	SyntaxParam::~SyntaxParam()
	{
	}

	void 
	SyntaxParam::set_param_name(const char * param_name){
		strncpy(param_name_, param_name, sizeof(param_name_) - 1);
	}

	const char * 
	SyntaxParam::param_name() const
	{
		return param_name_;
	}

	void 
	SyntaxParam::set_param_type(const char * param_type){
		strncpy(param_type_, param_type, sizeof(param_type_) - 1);
	}

	const char *
	SyntaxParam::param_type() const
	{
		return param_type_;
	}

	/* end of implement of SyntaxParam */

	/* start of implement of SyntaxFunc */

	SyntaxFunc::SyntaxFunc(){
		memset(func_name_, 0, sizeof(func_name_));
	}

	SyntaxFunc::~SyntaxFunc()
	{
	}

	SyntaxParam * 
	SyntaxFunc::req(){
		return &req_;
	}

	SyntaxParam * 
	SyntaxFunc::rsp(){
		return &rsp_;
	}

	void 
	SyntaxFunc::set_func_name(const char * func_name){
		strncpy(func_name_, func_name, sizeof(func_name_) - 1);
	}

	const char *
	SyntaxFunc::func_name() const{
		return func_name_;
	}
	/* end of implement of SyntaxFunc */

	/* start of implement of SyntaxTree */

	SyntaxTree::SyntaxTree(){
		memset(proto_file_name_, 0 , sizeof(proto_file_name_));
		memset(package_name_, 0, sizeof(package_name_));
		memset(service_name_, 0, sizeof(service_name_));
	}

	SyntaxTree::~SyntaxTree()
	{
	}

	void 
	SyntaxTree::set_proto_file_name(const char * proto_file_name){
		strncpy(proto_file_name_, proto_file_name, sizeof(proto_file_name_) - 1);
	}

	const char * 
	SyntaxTree::proto_file_name() const{
		return proto_file_name_;
	}

	void 
	SyntaxTree::set_package_name(const char * package_name){
		strncpy(package_name_, package_name, sizeof(package_name_) - 1);
	}

	const char * 
	SyntaxTree::package_name() const{
		return package_name_;
	}

	void 
	SyntaxTree::set_service_name(const char * service_name){
		strncpy(service_name_, service_name, sizeof(service_name_) - 1);
	}

	const char *  
	SyntaxTree::service_name() const{
		return service_name_;
	}

	const FuncVector * 
	SyntaxTree::func_list() const
	{
		return &func_list_;
	}

	FuncVector * 
	SyntaxTree::func_list()
	{
		return &func_list_;
	}
	/* end of implement of SyntaxTree */

}
