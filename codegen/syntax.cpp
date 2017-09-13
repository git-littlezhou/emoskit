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

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "proto_parser.h"
#include "util.h"
#include "name_generator.h"
#include "server_generator.h"
#include "client_generator.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

using namespace std;
using namespace emoskit;

#define BUFFER_SIZE 256

void PrintHelp(const char* program) {
	printf("Usage: %s <-f proto file> <-d destination dir>\n", program);
	printf("Parse PROTO_FILES and generate output based on the options given:\n");
	printf("	-f <proto file>             # Proto File\n");
	printf("	-d <dir>                    # destination output dir\n");
	printf("	-I <dir>                    # include path dir\n");
	printf("\n");

	exit(0);
}

int main(int argc, char * argv[]) {
	const char* pb_file = nullptr;
	const char* output_dir = nullptr;

	vector<string> include_paths;

	char real_path[BUFFER_SIZE];
	const char* rp = nullptr;

	int c;
	while ((c = getopt(argc, argv, "f:d:I:h")) != EOF) {
		switch (c) {
		case 'f':
			pb_file = optarg;
			break;
		case 'd':
			output_dir = optarg;
			break;
		case 'I':
			rp = realpath(optarg, real_path);
			if (rp != nullptr) {
				include_paths.push_back(rp);
			}
			break;
		case  'h':
		default:
			PrintHelp(argv[0]);
			break;
		}
	}

	if (nullptr == pb_file) {
		printf("Missing proto file...\n");
		PrintHelp(argv[0]);
	}
	if (nullptr == output_dir) {
		printf("Missing output dir...\n");
		PrintHelp(argv[0]);
	}

	if (0 != access(output_dir, R_OK | W_OK | X_OK)) {
		printf("Invalid dir: %s, error: %d, %s\n\n", output_dir, errno, strerror(errno));
		exit(0);
	}

	char out_path[BUFFER_SIZE] = { 0 };
	strncpy(out_path, output_dir, sizeof(out_path));
	if ('/' == out_path[strlen(out_path) - 1]) {
		out_path[strlen(out_path) - 1] = '\0';
	}

	ProtoParser proto_parser;
	SyntaxTree tree;

#ifdef HAVE_CONFIG_H
	std::string protobuf_home = PROTOBUF_HOME;
	std::string emoskit_home = EMOSKIT_HOME;
#else
	std::string protobuf_home = "system";
	std::string emoskit_home = "/usr/local";
#endif

	if (protobuf_home == "system") {
		include_paths.push_back("/usr");
		include_paths.push_back("/usr/local");
	}
	else { 
		include_paths.push_back(protobuf_home + "/include");
	}

	if (proto_parser.Parse(pb_file, include_paths, &tree) != 0)
		return 0;

	NameGenerator gen;

	NameGenerator name_gen;
	ServerGenerator svr_gen(&name_gen);
	ClientGenerator cli_gen(&name_gen);

	char filename[BUFFER_SIZE] = { 0 }, tmp[BUFFER_SIZE] = { 0 };

	const char* project = argv[0];

	// xx_server_main.cpp
	{
		name_gen.GetServerMainFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			svr_gen.GenerateServerMainCpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_service.h
	{
		name_gen.GetServiceFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.h", out_path, tmp);
		FILE * fp = fopen(filename, "w");
		assert(NULL != fp);
		svr_gen.GenerateServiceHpp(&tree, fp);
		fclose(fp);

		printf("%s: Generate %s file ... done\n", project, filename);
	}

	// xx_serivce.cpp
	{
		name_gen.GetServiceFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		FILE * fp = fopen(filename, "w");
		assert(NULL != fp);
		svr_gen.GenerateServiceCpp(&tree, fp);
		fclose(fp);

		printf("%s: Generate %s file ... done\n", project, filename);
	}

	// xx_service_impl.h
	{
		name_gen.GetServiceImplFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.h", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			svr_gen.GenerateServiceImplHpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_service_impl.cpp
	{
		name_gen.GetServiceImplFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			svr_gen.GenerateServiceImplCpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_server.conf
	{
		name_gen.GetServerConfigFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.conf", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			svr_gen.GenerateServerConfigFile(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_method_name.h
	{
		name_gen.GetMethodNamesFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.h", out_path, tmp);
		FILE * fp = fopen(filename, "w");
		assert(NULL != fp);
		svr_gen.GenerateMethodNameHpp(&tree, fp);
		fclose(fp);

		printf("%s: Generate %s file ... done\n", project, filename);
	}

	// xx_client_main.cpp
	{
		name_gen.GetClientMainFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			cli_gen.GenerateClientMainCpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_client.h
	{
		name_gen.GetClientFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.h", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			cli_gen.GenerateClientHpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_client.cpp
	{
		name_gen.GetClientFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			cli_gen.GenerateClientCpp(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_client.conf
	{
		name_gen.GetClientConfigFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.conf", out_path, tmp);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			cli_gen.GenerateClientConfigFile(&tree, fp);
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	// xx_stub.h
	{
		name_gen.GetStubFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.h", out_path, tmp);
		FILE * fp = fopen(filename, "w");
		assert(NULL != fp);
		cli_gen.GenerateStubHpp(&tree, fp);
		fclose(fp);

		printf("%s: Generate %s file ... done\n", project, filename);
	}

	// xx_stub.cpp
	{
		name_gen.GetStubFileName(tree.service_name(), tmp, sizeof(tmp));
		snprintf(filename, sizeof(filename), "%s/%s.cpp", out_path, tmp);
		FILE * fp = fopen(filename, "w");
		assert(NULL != fp);
		cli_gen.GenerateStubCpp(&tree, fp);
		fclose(fp);

		printf("%s: Generate %s file ... done\n", project, filename);
	}

	// Makefile
	{
		snprintf(filename, sizeof(filename), "%s/Makefile", out_path);
		if (0 != access(filename, F_OK)) {
			FILE * fp = fopen(filename, "w");
			assert(NULL != fp);
			svr_gen.GenerateMakefile(&tree, fp, protobuf_home.c_str(), emoskit_home.c_str());
			fclose(fp);

			printf("%s: Generate %s file ... done\n", project, filename);
		}
		else {
			printf("%s: %s is already exist... skip\n", project, filename);
		}
	}

	return 0;
}



