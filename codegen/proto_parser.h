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

#ifndef _PROTO_PARSER_H_
#define _PROTO_PARSER_H_

#include <string>
#include <vector>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/command_line_interface.h>

#include "IDLParser.h"
#include "syntax_tree.h"

namespace emoskit {

	class ProtoParser : public IDLParser
	{
	public:
		ProtoParser() {}
		virtual ~ProtoParser() {}

		int Parse(const char* proto_file_name, const std::vector<std::string>& include_path_list, SyntaxTree* syntax_tree) override;

	private:
		int ParseProtoFile(const char* proto_file_name, SyntaxTree* syntax_tree, google::protobuf::compiler::DiskSourceTree & tree);
		int AddEcho(SyntaxTree* syntax_tree);
	};

	class ProtoErrorPrinter : public google::protobuf::compiler::MultiFileErrorCollector {
	public:
		ProtoErrorPrinter() {}
		~ProtoErrorPrinter() {}

		void AddError(const std::string& filename, int line, int column, const std::string& message) override{

			fprintf(stderr, "%s", filename.c_str());

			if (line != -1)
				fprintf(stderr, ", line: %d, column: %d", line + 1, column + 1);

			fprintf(stderr, ": %s\n", message.c_str());
		}
	};
}
#endif // !_PROTO_PARSER_H_

