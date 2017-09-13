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

#include "server_generator.h"
#include "server_template.h"
#include "util.h"

namespace emoskit {

	#define BUFFER_SIZE 256

	ServerGenerator::ServerGenerator(NameGenerator* name_gen) : name_gen_(name_gen) {}
	ServerGenerator::~ServerGenerator() {}

	void 
	ServerGenerator::GenerateServiceHpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetServiceFileName(syntax_tree->service_name(), filename, sizeof(filename));

		fprintf(stream, "#ifndef %s\n", name_gen_->GetDefineHeader(filename, buf, sizeof(buf)));
		fprintf(stream, "#define %s\n", buf);

		fprintf(stream, "\n");

		fprintf(stream, "#include <string>\n");
		fprintf(stream, "#include <functional>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include <google/protobuf/wrappers.pb.h>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include <%s/service.h>\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetIDLFileName(syntax_tree->proto_file_name(), buf, sizeof(buf)));
		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetMethodNamesFileName(syntax_tree->service_name(), buf, sizeof(buf)));

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		fprintf(stream, "	class %s : public ::%s::Service {\n", name_gen_->GetServiceClassName(syntax_tree->service_name(), class_name, sizeof(class_name)), PROJECT_NAME);
		fprintf(stream, "	public:\n");
		fprintf(stream, "		%s();\n", class_name);
		fprintf(stream, "		virtual ~%s();\n", class_name);

		fprintf(stream, "\n");

		FuncVector* func_vec = syntax_tree->func_list();
		
		for (FuncVector::iterator iter = func_vec->begin(); iter != func_vec->end(); ++iter) {
			std::string declaration;
			GetServiceFuncDeclaration(syntax_tree, &(*iter), 1, 0, 3, &declaration);
			fprintf(stream, "		virtual %s;\n", declaration.c_str());
			fprintf(stream, "\n");
		}

		fprintf(stream, "	};\n");

		fprintf(stream, "\n");

		fprintf(stream, "}\n");
		fprintf(stream, "#endif\n");
		fprintf(stream, "\n");
	}

	void 
	ServerGenerator::GenerateServiceCpp(SyntaxTree * syntax_tree, FILE * stream){
		char filename[BUFFER_SIZE] = { 0 };
		char buf1[BUFFER_SIZE] = { 0 };
		char buf2[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetServiceFileName(syntax_tree->service_name(), filename, sizeof(filename));

		fprintf(stream, "#include \"%s.h\"\n", filename);

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		name_gen_->GetServiceClassName(syntax_tree->service_name(), class_name, sizeof(class_name));
		fprintf(stream, "	%s::%s() {\n", class_name, class_name);

		FuncVector* func_vec = syntax_tree->func_list();

		int i = 0;
		for (FuncVector::iterator iter = func_vec->begin(); iter != func_vec->end(); ++i) {
			fprintf(stream, "		AddMethod(new ::%s::ServiceMethod(\n", PROJECT_NAME);
			fprintf(stream, "			%s[%d],\n", name_gen_->GetMethodNamesFileName(syntax_tree->service_name(), buf1, sizeof(buf1)), i);
			fprintf(stream, "			::%s::Method::NORMAL_METHOD,\n", PROJECT_NAME);
			fprintf(stream, "			new ::%s::MethodHandlerImpl<%s, %s, %s>(\n", PROJECT_NAME, class_name, 
				name_gen_->GetIDLClassName(iter->req()->param_type(), buf1, sizeof(buf1)),
				name_gen_->GetIDLClassName(iter->rsp()->param_type(), buf2, sizeof(buf2)));
			fprintf(stream, "				std::mem_fn(&%s::%s), this)));\n", class_name, iter->func_name());

			++iter;

			if(iter != func_vec->end())
				fprintf(stream, "\n");
		}
		fprintf(stream, "	}\n");

		fprintf(stream, "\n");

		fprintf(stream, "	%s::~%s() {}\n", class_name, class_name);

		fprintf(stream, "\n");

		for (FuncVector::iterator iter = func_vec->begin(); iter != func_vec->end(); ++iter) {
			std::string declaration;
			GetServiceFuncDeclaration(syntax_tree, &(*iter), 0, 0, 2, &declaration);
			fprintf(stream, "	%s {\n", declaration.c_str());
			fprintf(stream, "		return ::%s::Status(::%s::StatusCode::UNIMPLEMENTED, \"method unimplement\");\n", PROJECT_NAME, PROJECT_NAME);
			fprintf(stream, "	}\n");
			fprintf(stream, "\n");
		}

		fprintf(stream, "}\n");
		fprintf(stream, "\n");
	}

	void
	ServerGenerator::GenerateServiceImplHpp(SyntaxTree * syntax_tree, FILE * stream){
		char file_name[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetServiceImplFileName(syntax_tree->service_name(), file_name, sizeof(file_name));

		fprintf(stream, "#ifndef %s\n", name_gen_->GetDefineHeader(file_name, buf, sizeof(buf)));
		fprintf(stream, "#define %s\n", buf);

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetServiceFileName(syntax_tree->service_name(), buf, sizeof(buf)));
		fprintf(stream, "#include \"%s.h\"\n", name_gen_->GetIDLFileName(syntax_tree->proto_file_name(), buf, sizeof(buf)));

		fprintf(stream, "\n");

		fprintf(stream, "using namespace %s;\n", PROJECT_NAME);

		fprintf(stream, "\n");

		fprintf(stream, "class %s : public ::%s::%s {\n", name_gen_->GetServiceImplClassName(syntax_tree->service_name(), class_name, sizeof(class_name)),
			syntax_tree->package_name(), name_gen_->GetServiceClassName(syntax_tree->service_name(), buf, sizeof(buf)));
		fprintf(stream, "public:\n");

		FuncVector* func_vec = syntax_tree->func_list();

		FuncVector::iterator iter = func_vec->begin();
		// generate Echo function
		std::string declaration;
		GetServiceFuncDeclaration(syntax_tree, &(*iter), 1, 1, 2, &declaration);
		fprintf(stream, "	%s override;\n", declaration.c_str());
		fprintf(stream, "\n");

		++iter;

		if (iter != func_vec->end()) {
			fprintf(stream, "/*	You can uncomment the functions declaration and implement it in .cpp file	*/\n");
			fprintf(stream, "\n");
		}

		for (; iter != func_vec->end(); ++iter) {
			fprintf(stream, "/*\n");
			std::string declaration;
			GetServiceFuncDeclaration(syntax_tree, &(*iter), 1, 1, 2, &declaration);
			fprintf(stream, "	%s override;\n", declaration.c_str());
			fprintf(stream, "*/\n");
			fprintf(stream, "\n");
		}

		fprintf(stream, "};\n");
		fprintf(stream, "#endif\n");

		fprintf(stream, "\n");
	}

	void 
	ServerGenerator::GenerateServiceImplCpp(SyntaxTree * syntax_tree, FILE * stream){
		char file_name[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };
		char class_name[BUFFER_SIZE] = { 0 };

		name_gen_->GetServiceImplFileName(syntax_tree->service_name(), file_name, sizeof(file_name));

		fprintf(stream, "#include <string>\n");

		fprintf(stream, "\n");

		fprintf(stream, "#include \"%s.h\"\n", file_name);

		fprintf(stream, "\n");

		FuncVector* func_vec = syntax_tree->func_list();

		FuncVector::iterator iter = func_vec->begin();
		// generate Echo function
		std::string declaration;
		GetServiceFuncDeclaration(syntax_tree, &(*iter), 0, 1, 1, &declaration);
		fprintf(stream, "%s {\n", declaration.c_str());
		fprintf(stream, "	%s->set_value(%s.value());\n", iter->rsp()->param_name(), iter->req()->param_name());
		fprintf(stream, "	return Status::OK;\n");
		fprintf(stream, "}\n");
		fprintf(stream, "\n");

		++iter;

		if (iter != func_vec->end()) {
			fprintf(stream, "/*	You can uncomment and implement the functions	*/\n");
			fprintf(stream, "\n");
		}

		for (; iter != func_vec->end(); ++iter) {
			fprintf(stream, "/*\n");
			std::string declaration;
			GetServiceFuncDeclaration(syntax_tree, &(*iter), 0, 1, 1, &declaration);
			fprintf(stream, "%s {}\n", declaration.c_str());
			fprintf(stream, "*/\n");
			fprintf(stream, "\n");
		}
		
	}

	void 
	ServerGenerator::GenerateServerMainCpp(SyntaxTree * syntax_tree, FILE * stream){
		char service_impl_file[BUFFER_SIZE] = { 0 };
		char service_impl_class[BUFFER_SIZE] = { 0 };

		name_gen_->GetServiceImplFileName(syntax_tree->service_name(), service_impl_file, sizeof(service_impl_file));
		name_gen_->GetServiceImplClassName(syntax_tree->service_name(), service_impl_class, sizeof(service_impl_class));

		std::string contents = EMOSKIT_SERVER_MAIN_TEMPLATE;

		util::StrTrim(contents);

		util::StrReplaceAll(contents, "$service_impl_file$", service_impl_file);
		util::StrReplaceAll(contents, "$service_impl_class$", service_impl_class);

		fprintf(stream, "%s", contents.c_str());

		fprintf(stream, "\n");

	}

	void 
	ServerGenerator::GenerateMethodNameHpp(SyntaxTree * syntax_tree, FILE * stream){
		char file_name[BUFFER_SIZE] = { 0 };
		char buf[BUFFER_SIZE] = { 0 };

		name_gen_->GetMethodNamesFileName(syntax_tree->service_name(), file_name, sizeof(file_name));

		fprintf(stream, "#ifndef %s\n", name_gen_->GetDefineHeader(file_name, buf, sizeof(buf)));
		fprintf(stream, "#define %s\n", buf);

		fprintf(stream, "\n");

		fprintf(stream, "namespace %s {\n", syntax_tree->package_name());

		fprintf(stream, "\n");

		fprintf(stream, "	static const char* %s[] = {\n", file_name);

		FuncVector* func_vec = syntax_tree->func_list();
		for (FuncVector::iterator iter = func_vec->begin(); iter != func_vec->end();) {
			fprintf(stream, "		\"%s/%s/%s\"", syntax_tree->package_name(), syntax_tree->service_name(), iter->func_name());

			++iter;

			if (iter != func_vec->end()) {
				fprintf(stream, ",\n");
			}
			else {
				fprintf(stream, "\n");
			}
		}

		fprintf(stream, "	};\n");

		fprintf(stream, "\n");

		fprintf(stream, "}\n");

		fprintf(stream, "#endif\n");

		fprintf(stream, "\n");
	}

	void 
	ServerGenerator::GenerateServerConfigFile(SyntaxTree * syntax_tree, FILE * stream){
		std::string contents = EMOSKIT_SERVER_CONFIG_TEMPLATE;
		util::StrTrim(contents);
		fprintf(stream, "%s", contents.c_str());
		fprintf(stream, "\n");
	}

	void
	ServerGenerator::GenerateMakefile(SyntaxTree * syntax_tree, FILE * stream, const char * protobuf_home, const char * emoskit_home){
		char server_main_file[BUFFER_SIZE] = { 0 }, service_file[BUFFER_SIZE] = { 0 }, service_impl_file[BUFFER_SIZE] = { 0 };
		char client_main_file[BUFFER_SIZE] = { 0 }, client_file[BUFFER_SIZE] = { 0 }, stub_file[BUFFER_SIZE] = { 0 };
		char pb_file[BUFFER_SIZE] = { 0 };

		name_gen_->GetServerMainFileName(syntax_tree->service_name(), server_main_file, sizeof(server_main_file));
		name_gen_->GetServiceFileName(syntax_tree->service_name(), service_file, sizeof(service_file));
		name_gen_->GetServiceImplFileName(syntax_tree->service_name(), service_impl_file, sizeof(service_impl_file));

		name_gen_->GetClientMainFileName(syntax_tree->service_name(), client_main_file, sizeof(client_main_file));
		name_gen_->GetClientFileName(syntax_tree->service_name(), client_file, sizeof(client_file));
		name_gen_->GetStubFileName(syntax_tree->service_name(), stub_file, sizeof(stub_file));

		name_gen_->GetIDLFileName(syntax_tree->proto_file_name(), pb_file, sizeof(pb_file));
		

		std::string contents = EMOSKIT_MAKEFILE_TEMPLATE;

		std::string pb_home = protobuf_home;
		if (pb_home == "system") {
			util::StrReplaceAll(contents, "$protobuf_home$", "");
			util::StrReplaceAll(contents, "$-Lprotobuf_lib$", "");
			util::StrReplaceAll(contents, "$-Iprtobuf_include$", "");
			util::StrReplaceAll(contents, "$protobuf_bin$", "");
		}
		else {
			char protobuf_home_temp[BUFFER_SIZE] = { 0 };
			strncpy(protobuf_home_temp, protobuf_home, sizeof(protobuf_home_temp));
			if ('/' == protobuf_home_temp[strlen(protobuf_home_temp) - 1]) {
				protobuf_home_temp[strlen(protobuf_home_temp) - 1] = '\0';
			}

			pb_home = protobuf_home_temp;

			util::StrReplaceAll(contents, "$protobuf_home$", std::string("PROTOBUF_HOME=") + pb_home);
			util::StrReplaceAll(contents, "$-Lprotobuf_lib$", "-L$(PROTOBUF_HOME)/lib/");
			util::StrReplaceAll(contents, "$-Iprtobuf_include$", "-I $(PROTOBUF_HOME)/include/");
			util::StrReplaceAll(contents, "$protobuf_bin$", "$(PROTOBUF_HOME)/bin/");
		}

		char emoskit_home_temp[BUFFER_SIZE] = { 0 };
		strncpy(emoskit_home_temp, emoskit_home, sizeof(emoskit_home_temp));
		if ('/' == emoskit_home_temp[strlen(emoskit_home_temp) - 1]) {
			emoskit_home_temp[strlen(emoskit_home_temp) - 1] = '\0';
		}
		util::StrReplaceAll(contents, "$emoskit_home$", emoskit_home_temp);

		util::StrReplaceAll(contents, "$server_main_file$", server_main_file);
		util::StrReplaceAll(contents, "$service_file$", service_file);
		util::StrReplaceAll(contents, "$service_impl_file$", service_impl_file);

		util::StrReplaceAll(contents, "$client_main_file$", client_main_file);
		util::StrReplaceAll(contents, "$client_file$", client_file);
		util::StrReplaceAll(contents, "$stub_file$", stub_file);

		util::StrReplaceAll(contents, "$pb_file$", pb_file);

		util::StrReplaceAll(contents, "$proto_file_name$", syntax_tree->proto_file_name());

		util::StrTrim(contents);

		fprintf(stream, "%s", contents.c_str());

		fprintf(stream, "\n");
	}

	void 
	ServerGenerator::GetServiceFuncDeclaration(SyntaxTree * syntax_tree, SyntaxFunc* func, bool is_hpp, bool is_impl, int how_much_tab, std::string * buf){
		char class_name[BUFFER_SIZE] = { 0 };
		char param_type_name[BUFFER_SIZE] = { 0 };

		if (is_impl) {
			name_gen_->GetServiceImplClassName(syntax_tree->service_name(), class_name, sizeof(class_name));
		}
		else {
			name_gen_->GetServiceClassName(syntax_tree->service_name(), class_name, sizeof(class_name));
		}

		if (is_hpp) {
			util::StrAppendFormat(buf, "::%s::Status %s(::%s::ServerContext* context,\n", PROJECT_NAME, func->func_name(), PROJECT_NAME);
		}
		else{
			util::StrAppendFormat(buf, "::%s::Status\n", PROJECT_NAME);
			int count = how_much_tab - 1;
			while (count--)
				util::StrAppendFormat(buf, "	");
			util::StrAppendFormat(buf, "%s::%s(::%s::ServerContext* context,\n", class_name, func->func_name(), PROJECT_NAME);
			//util::StrAppendFormat(buf, "::%s::Status %s::%s(::%s::ServerContext* context,\n", PROJECT_NAME, class_name, func->func_name(), PROJECT_NAME);
		}

		while (how_much_tab--)
			util::StrAppendFormat(buf, "	");

		util::StrAppendFormat(buf, "const %s& %s, ", name_gen_->GetIDLClassName(func->req()->param_type(), param_type_name, sizeof(param_type_name)),
			func->req()->param_name());

		util::StrAppendFormat(buf, "%s* %s)", name_gen_->GetIDLClassName(func->rsp()->param_type(), param_type_name, sizeof(param_type_name)),
			func->rsp()->param_name());
	}

}