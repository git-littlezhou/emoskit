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

#ifndef _STATUS_CODE_H_
#define _STATUS_CODE_H_

namespace emoskit {

	enum StatusCode {
		OK = 0,
		ERROR = 1,
		UNIMPLEMENTED = 2,
		INVALID_ARGUMENT = 3,
		METHOD_NOT_EXIST = 4,
		PROTOCOL_ERROR = 5,
		PERMISSION_DENIED = 6,
		UNAUTHENTICATED = 7,
		CONNECT_CLOSED = 8
	};

}
#endif // !_STATUS_CODE_H_
