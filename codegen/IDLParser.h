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

#ifndef _IDL_PARSER_H_
#define _IDL_PARSER_H_

#include "syntax_tree.h"

namespace emoskit {

	class IDLParser
	{
	public:
		IDLParser() {}
		virtual ~IDLParser() {}

		virtual int Parse(const char* proto_file, const std::vector<std::string>& include_path_list, SyntaxTree* syntax_tree) = 0;

	};
}
#endif // !_IDL_PARSER_H_

