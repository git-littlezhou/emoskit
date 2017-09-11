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

#ifndef _TCP_STREAM_H_
#define _TCP_STREAM_H_

#include <unistd.h>

#include "tcp_stream_buf.h"
#include "stream_base.h"

namespace emoskit {

	namespace common {
	
		class TcpStream : public StreamBase
		{
		public:
			TcpStream(size_t buf_size) : StreamBase(buf_size), socket_fd_(-1) {}

			~TcpStream() {
				if (socket_fd_ > 0)
					close(socket_fd_);
			}

			void Attach(int socket_fd) {
				ReplaceRdBuf(new TcpStreamBuf(socket_fd, kBufSize));
				socket_fd_ = socket_fd;
			}

		private:
			int socket_fd_;
		};

	}
}


#endif // !

