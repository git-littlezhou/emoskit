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

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/command_line_interface.h>

#include "proto_parser.h"

using google::protobuf::compiler::DiskSourceTree;
using google::protobuf::compiler::Importer;
using google::protobuf::FileDescriptor;
using google::protobuf::ServiceDescriptor;
using google::protobuf::MethodDescriptor;
using google::protobuf::Descriptor;

namespace emoskit {

	int 
	ProtoParser::Parse(const char * proto_file_name, const std::vector<std::string>& include_path_list, SyntaxTree * syntax_tree){
		
		DiskSourceTree tree;
		tree.MapPath("", "./");
		for (auto & include_path : include_path_list) {
			tree.MapPath("", include_path);
		}

		int ret = ParseProtoFile(proto_file_name, syntax_tree, tree);

		if (ret == 0)
			AddEcho(syntax_tree);

		return ret;
	}

	int 
	ProtoParser::ParseProtoFile(const char * proto_file_name, SyntaxTree * syntax_tree, DiskSourceTree & tree){

		ProtoErrorPrinter error;

		Importer importer(&tree, &error);

		const FileDescriptor * fd = importer.Import(proto_file_name);

		if (fd == nullptr)
			return -1;

		if (fd->service_count() < 1) {
			fprintf(stderr, "service not found, exit.\n");
			return -1;
		}

		syntax_tree->set_proto_file_name(proto_file_name);
		syntax_tree->set_package_name(fd->package().c_str());

		if(fd->service_count() > 1)
			fprintf(stderr, "WARN: only support one service.\n");

		
		const ServiceDescriptor * service = fd->service(0);
		syntax_tree->set_service_name(service->name().c_str());

		for (int i = 0; i < service->method_count(); ++i) {
			const MethodDescriptor * method = service->method(i);

			SyntaxFunc func;

			func.set_func_name(method->name().c_str());

			const Descriptor * input_type = method->input_type();
			const Descriptor * output_type = method->output_type();

			func.req()->set_param_name("request");
			func.req()->set_param_type(input_type->full_name().c_str());

			func.rsp()->set_param_name("response");
			func.rsp()->set_param_type(output_type->full_name().c_str());

			syntax_tree->func_list()->push_back(func);

		}
		return 0;
	}

	int
	ProtoParser::AddEcho(SyntaxTree * syntax_tree){
	
		const char* type_name = "google.protobuf.StringValue";

		SyntaxFunc func;
		func.set_func_name("Echo");
		func.req()->set_param_type(type_name);
		func.req()->set_param_name("request");
		func.rsp()->set_param_type(type_name);
		func.rsp()->set_param_name("response");

		syntax_tree->func_list()->insert(syntax_tree->func_list()->begin(), func);

		return 0;
	}

}