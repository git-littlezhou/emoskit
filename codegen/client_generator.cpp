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

#include "client_generator.h"
#include "client_template.h"
#include "util.h"

namespace emoskit {

	#define BUFFER_SIZE 256

	ClientGenerator::ClientGenerator(NameGenerator* name_gen) : name_gen_(name_gen) {}
	ClientGenerator::~ClientGenerator(){}

	void 
	ClientGenerator::GenerateClientHpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };
		char stub_class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetClientFileName(syntax_tree->service_name(), filename, sizeof(filename));

		fprintf(stream, "#ifndef %s\n", name_gen_->GetDefineHeader(filename, buf, sizeof(buf)));
		fprintf(stream, "#define %s\n", buf);

		fprintf(stream, "\n");

		fprintf(stream, "#include <memory>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include <%s/client_config.h>\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetStubFileName(syntax_tree->service_name(), buf, sizeof(buf)));

		fprintf(stream, "\n");

		fprintf(stream, "using %s::common::StreamBase;\n", PROJECT_NAME);
		fprintf(stream, "using %s::ClientConfig;\n", PROJECT_NAME);

		fprintf(stream, "\n");
		
		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		fprintf(stream, "	class %s {\n", name_gen_->GetClientClassName(syntax_tree->service_name(), class_name, sizeof(class_name)));
		fprintf(stream, "	public:\n");
		fprintf(stream, "		%s(const std::shared_ptr<StreamBase>& stream, ClientConfig* client_config)\n", class_name);
		fprintf(stream, "			: stub_(new %s(stream)), client_config_(client_config) {}\n", name_gen_->GetStubClassName(syntax_tree->service_name(), stub_class_name, sizeof(stub_class_name)));
		fprintf(stream, "		~%s() {}\n", class_name);

		fprintf(stream, "\n");

		fprintf(stream, "		void set_client_config(ClientConfig* client_config) { client_config_ = client_config; }\n");
		fprintf(stream, "		ClientConfig* client_config() { return client_config_; }\n");

		fprintf(stream, "\n");

		FuncVector* func_vec = syntax_tree->func_list();
		FuncVector::iterator iter = func_vec->begin();

		// generate Echo only
		fprintf(stream, "		std::string %s(const std::string& msg);\n", iter->func_name());

		fprintf(stream, "\n");

		fprintf(stream, "		// You can declare more methods here and implement them in .cpp file\n");

		fprintf(stream, "\n");

		fprintf(stream, "	private:\n");
		fprintf(stream, "		ClientConfig* client_config_;\n");
		fprintf(stream, "		std::unique_ptr<%s> stub_;\n", stub_class_name);
		fprintf(stream, "	};\n");

		fprintf(stream, "\n");

		fprintf(stream, "}\n");

		fprintf(stream, "#endif\n");

		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GenerateClientCpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetClientFileName(syntax_tree->service_name(), filename, sizeof(filename));

		fprintf(stream, "#include <%s/client_context.h>\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", filename);

		fprintf(stream, "\n");

		fprintf(stream, "using %s::ClientContext;\n", PROJECT_NAME);
		fprintf(stream, "using %s::Status;\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());
		
		fprintf(stream, "\n");

		FuncVector* func_vec = syntax_tree->func_list();
		FuncVector::iterator iter = func_vec->begin();

		fprintf(stream, "	std::string\n");
		fprintf(stream, "	%s::%s(const std::string& msg) {\n", name_gen_->GetClientClassName(syntax_tree->service_name(), class_name, sizeof(class_name)), iter->func_name());
		fprintf(stream, "		%s request;\n", name_gen_->GetIDLClassName(iter->req()->param_type(), buf, sizeof(buf)));
		fprintf(stream, "		%s response;\n", name_gen_->GetIDLClassName(iter->rsp()->param_type(), buf, sizeof(buf)));

		fprintf(stream, "\n");

		fprintf(stream, "		request.set_value(msg);\n");

		fprintf(stream, "\n");

		fprintf(stream, "		ClientContext client_context;\n");

		fprintf(stream, "\n");

		fprintf(stream, "		Status status = stub_->%s(&client_context, request, &response);\n", iter->func_name());

		fprintf(stream, "\n");

		fprintf(stream, "		if (status.is_ok()) {\n");
		fprintf(stream, "			return response.value();\n");
		fprintf(stream, "		}\n");
		fprintf(stream, "		else {\n");
		fprintf(stream, "			emoskit_log(ERROR_LOG, \"RPC failed, code: %s, message : %s\", status.code(), status.message().c_str());\n", "%d", "%s");
		fprintf(stream, "			return \"RPC failed\";\n");
		fprintf(stream, "		}\n");

		fprintf(stream, "\n");

		fprintf(stream, "	}\n");
		fprintf(stream, "}\n");

		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GenerateStubHpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetStubFileName(syntax_tree->service_name(), filename, sizeof(filename));

		fprintf(stream, "#ifndef %s\n", name_gen_->GetDefineHeader(filename, buf, sizeof(buf)));
		fprintf(stream, "#define %s\n", buf);

		fprintf(stream, "\n");

		fprintf(stream, "#include <memory>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include <google/protobuf/wrappers.pb.h>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include <%s/client_context.h>\n", PROJECT_NAME);
		fprintf(stream, "#include <%s/status.h>\n", PROJECT_NAME);
		fprintf(stream, "#include <%s/stream_base.h>\n", PROJECT_NAME);
		fprintf(stream, "#include <%s/method.h>\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetIDLFileName(syntax_tree->proto_file_name(), buf, sizeof(buf)));

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		fprintf(stream, "	class %s {\n", name_gen_->GetStubClassName(syntax_tree->service_name(), class_name, sizeof(class_name)));
		fprintf(stream, "	public:\n");
		fprintf(stream, "		%s(const std::shared_ptr<::%s::common::StreamBase>& stream);\n", class_name, PROJECT_NAME);
		fprintf(stream, "		~%s();\n", class_name);

		fprintf(stream, "\n");

		FuncVector* func_vec = syntax_tree->func_list();
		FuncVector::iterator iter = func_vec->begin();

		for (; iter != func_vec->end(); ++iter) {
			std::string declaration;
			GetStubFuncDeclaration(syntax_tree, &(*iter), 1, 3, &declaration);
			fprintf(stream, "		%s;\n", declaration.c_str());
			fprintf(stream, "\n");
		}

		fprintf(stream, "	private:\n");
		fprintf(stream, "		std::shared_ptr<::%s::common::StreamBase> stream_;\n", PROJECT_NAME);

		for (iter = func_vec->begin(); iter != func_vec->end(); ++iter) {
			fprintf(stream, "		const ::%s::Method method_%s_;\n", PROJECT_NAME, iter->func_name());
		}

		fprintf(stream, "	};\n");

		fprintf(stream, "\n");

		fprintf(stream, "}\n");

		fprintf(stream, "#endif\n");

		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GenerateStubCpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };
		char method_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetStubFileName(syntax_tree->service_name(), filename, sizeof(filename));
		name_gen_->GetStubClassName(syntax_tree->service_name(), class_name, sizeof(class_name));
		name_gen_->GetMethodNamesFileName(syntax_tree->service_name(), method_name, sizeof(method_name));

		fprintf(stream, "#include <%s/client_blocking_call.h>\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", filename);
		fprintf(stream, "#include \"%s.h\"\n", method_name);

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		fprintf(stream, "	%s::%s(const std::shared_ptr<::%s::common::StreamBase>& stream) : stream_(stream),\n", class_name, class_name, PROJECT_NAME);

		FuncVector* func_vec = syntax_tree->func_list();
		FuncVector::iterator iter = func_vec->begin();
		int i;
		for (iter = func_vec->begin(), i = 0; iter != func_vec->end(); ++i) {
			fprintf(stream, "		method_%s_(%s[%d], ::%s::Method::MethodType::NORMAL_METHOD)", iter->func_name(), method_name, i, PROJECT_NAME);
			++iter;
			if (iter != func_vec->end()) {
				fprintf(stream, ",\n");
			}
			else {
				fprintf(stream, "\n");
			}
		}

		fprintf(stream, "	{\n");
		fprintf(stream, "	}\n");

		fprintf(stream, "\n");

		fprintf(stream, "	%s::~%s()\n", class_name, class_name);
		fprintf(stream, "	{\n");
		fprintf(stream, "	}\n");

		fprintf(stream, "\n");

		for (iter = func_vec->begin(); iter != func_vec->end(); ++iter) {
			std::string declaration;
			GetStubFuncDeclaration(syntax_tree, &(*iter), 0, 2, &declaration);
			fprintf(stream, "	%s {\n", declaration.c_str());
			fprintf(stream, "		return %s::ClientBlockingCall(*stream_.get(), method_%s_, request, response, context);\n",
				PROJECT_NAME, iter->func_name());
			fprintf(stream, "	}\n");
			fprintf(stream, "\n");
		}

		fprintf(stream, "}\n");

		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GenerateClientMainCpp(SyntaxTree * syntax_tree, FILE * stream){
		char client_file[BUFFER_SIZE] = { 0 };
		char client_class[BUFFER_SIZE] = { 0 };
		char client_conf[BUFFER_SIZE] = { 0 };

		name_gen_->GetClientFileName(syntax_tree->service_name(), client_file, sizeof(client_file));
		name_gen_->GetClientClassName(syntax_tree->service_name(), client_class, sizeof(client_class));
		name_gen_->GetClientConfigFileName(syntax_tree->service_name(), client_conf, sizeof(client_conf));

		std::string contents = EMOSKIT_CLIENT_MAIN_TEMPLATE;

		util::StrTrim(contents);

		util::StrReplaceAll(contents, "$client_file$", client_file);
		util::StrReplaceAll(contents, "$package_name$", syntax_tree->package_name());
		util::StrReplaceAll(contents, "$client_class$", client_class);
		util::StrReplaceAll(contents, "$client_conf_file$", client_conf);

		fprintf(stream, "%s", contents.c_str());

		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GenerateClientConfigFile(SyntaxTree * syntax_tree, FILE * stream){
		std::string contents = EMOSKIT_CLIENT_CONFIG_TEMPLATE;
		util::StrTrim(contents);
		fprintf(stream, "%s", contents.c_str());
		fprintf(stream, "\n");
	}

	void 
	ClientGenerator::GetStubFuncDeclaration(SyntaxTree * syntax_tree, SyntaxFunc * func, bool is_hpp, int how_much_tab, std::string * buf){
		char class_name[BUFFER_SIZE] = { 0 };
		char param_type_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetStubClassName(syntax_tree->service_name(), class_name, sizeof(class_name));

		if (is_hpp) {
			util::StrAppendFormat(buf, "::%s::Status %s(::%s::ClientContext* context,\n", PROJECT_NAME, func->func_name(), PROJECT_NAME);
		}
		else {
			util::StrAppendFormat(buf, "::%s::Status\n", PROJECT_NAME);
			int count = how_much_tab - 1;
			while (count--)
				util::StrAppendFormat(buf, "	");
			util::StrAppendFormat(buf, "%s::%s(::%s::ClientContext* context,\n", class_name, func->func_name(), PROJECT_NAME);
		}

		while (how_much_tab--)
			util::StrAppendFormat(buf, "	");

		util::StrAppendFormat(buf, "const %s& %s, ", name_gen_->GetIDLClassName(func->req()->param_type(), param_type_name, sizeof(param_type_name)),
			func->req()->param_name());

		util::StrAppendFormat(buf, "%s* %s)", name_gen_->GetIDLClassName(func->rsp()->param_type(), param_type_name, sizeof(param_type_name)),
			func->rsp()->param_name());
	}
}