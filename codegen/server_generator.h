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

#ifndef _SERVER_GENERATOR_H_
#define _SERVER_GENERATOR_H_

#include <stdio.h>
#include <string>

#include "syntax_tree.h"
#include "name_generator.h"

namespace emoskit {

	class ServerGenerator {
	public:
		ServerGenerator(NameGenerator* name_gen);
		~ServerGenerator();

		void GenerateServiceHpp(SyntaxTree* syntax_tree, FILE* stream);
		void GenerateServiceCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateServiceImplHpp(SyntaxTree* syntax_tree, FILE* stream);
		void GenerateServiceImplCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateServerMainCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateMethodNameHpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateMakefile(SyntaxTree* syntax_tree, FILE* stream, const char* protobuf_home, const char* emoskit_home);

	private:
		void GetServiceFuncDeclaration(SyntaxTree* syntax_tree, SyntaxFunc* func, bool is_hpp, bool is_impl, int how_much_tab, std::string* buf);
		NameGenerator* name_gen_;
		const char* PROJECT_NAME = "emoskit";
	};

}
#endif // !_SERVER_GENERATOR_H_

