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

#ifndef _CREATE_STREAM_H_
#define _CREATE_STREAM_H_

#include <memory>

#include "stream_base.h"
#include "tcp_stream.h"
#include "client_config.h"
#include "util.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


namespace emoskit {

	std::shared_ptr<::emoskit::common::StreamBase> CreateStreamOrDie(ClientConfig* client_config) {

		const Endpoint* endpoint = client_config->GetEndpointRandom();

		util_fd_t connect_fd = -1;

		if (endpoint != nullptr) {
			connect_fd = util::CreateSocketAndConnect(endpoint->ip, endpoint->port, -1);
		}

		if (connect_fd < 0) {
			exit(0);
		}

		std::shared_ptr<::emoskit::common::TcpStream> stream(new ::emoskit::common::TcpStream(4096));

		stream->Attach(connect_fd);

		return stream;
	}

}
#endif // !_CREATE_STREAM_H_

