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

#ifndef _STREAM_BASE_H_
#define _STREAM_BASE_H_

#include <iostream>

#include "stream_buf_base.h"
#include "mm.h"

namespace emoskit {
	
	namespace common {
		
		class StreamBase : public std::iostream
		{
		public:
			StreamBase(size_t buf_size) : kBufSize(buf_size) {}

			virtual ~StreamBase() {
				delete rdbuf();
				rdbuf(nullptr);
			}

			void ReplaceRdBuf(StreamBufBase* stream_buf) {
				std::streambuf* old_stream_buf = rdbuf(stream_buf);
				delete old_stream_buf;
			}

		protected:
			const size_t kBufSize;
		};

	}
}
#endif // !_STREAM_BASE_H_

