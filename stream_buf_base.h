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

#ifndef _STREAM_BUF_BASE_H_
#define _STREAM_BUF_BASE_H_

#include <streambuf>

namespace emoskit {
	
	namespace common {
	
		class StreamBufBase : public std::streambuf
		{
		public:
			StreamBufBase(size_t buf_size);
			virtual ~StreamBufBase();

		protected:
			/*
			 * overide base class streambuf's funtion
			 * see http://en.cppreference.com/w/cpp/io/basic_streambuf for detail.
			 */
			int sync() override;
			int_type overflow(int_type ch = traits_type::eof()) override;
			int_type underflow() override;

		private:
			/*
			 * how to read from or write to the controlled sequence, implemented by derive class
			 */
			virtual ssize_t BufReadFromTheControlledSequence(void* buf, size_t len, int flag) = 0;
			virtual ssize_t BufWriteToTheControlledSequence(void* buf, size_t len, int flag) = 0;

		private:
			const size_t kBufSize;
		};

	}
}
#endif
