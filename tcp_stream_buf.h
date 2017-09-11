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

#ifndef _TCP_STREAM_BUF_H_
#define _TCP_STREAM_BUF_H_

#include <sys/types.h>
#include <sys/socket.h>

#include "util.h"
#include "stream_buf_base.h"

namespace emoskit {

	namespace common {

		class TcpStreamBuf : public StreamBufBase
		{
		public:
			TcpStreamBuf(int socket_fd, size_t buf_size) : StreamBufBase(buf_size), socket_fd_(socket_fd) {}
			virtual ~TcpStreamBuf() {}

		private:
			ssize_t BufReadFromTheControlledSequence(void* buf, size_t len, int flag) override {
				int ret;
				while (true) {
					ret = recv(socket_fd_, buf, len, flag);
					if (expect_true(ret > 0))
						return ret;
					if (ret == -1 && errno == EINTR)
						continue;
					else break;
				}
				return ret;
			}

			ssize_t BufWriteToTheControlledSequence(void* buf, size_t len, int flag) override {
				int ret;
				while (true) {
					ret = send(socket_fd_, buf, len, flag);
					if (expect_true(ret > 0))
						return ret;
					if (ret == -1 && errno == EINTR)
						continue;
					else break;
				}
				return ret;
			}

		private:
			int socket_fd_;
		};

	}
}
#endif
