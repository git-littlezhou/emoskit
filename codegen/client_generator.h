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

#ifndef _CLIENT_GENERATOR_H_
#define _CLIENT_GENERATOR_H_

#include <stdio.h>
#include <string>

#include "syntax_tree.h"
#include "name_generator.h"

namespace emoskit {
	
	class ClientGenerator{
	public:
		ClientGenerator(NameGenerator* name_gen);
		~ClientGenerator();

		void GenerateClientHpp(SyntaxTree* syntax_tree, FILE* stream);
		void GenerateClientCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateStubHpp(SyntaxTree* syntax_tree, FILE* stream);
		void GenerateStubCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateClientMainCpp(SyntaxTree* syntax_tree, FILE* stream);

		void GenerateClientConfigFile(SyntaxTree* syntax_tree, FILE* stream);

	private:
		void GetStubFuncDeclaration(SyntaxTree* syntax_tree, SyntaxFunc* func, bool is_hpp, int how_much_tab, std::string* buf);
		NameGenerator* name_gen_;
		const char* PROJECT_NAME = "emoskit";
	};

}
#endif // !_CLIENT_GENERATOR_H_

