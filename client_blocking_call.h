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

#ifndef _CLIENT_BLOCKING_CALL_H_
#define _CLIENT_BLOCKING_CALL_H_

#include <arpa/inet.h>
#include <cstring>

#include "client_context.h"
#include "stream_base.h"
#include "status.h"
#include "network_proto.pb.h"
#include "method.h"

namespace emoskit {

	template <class RequestType, class ResultType> Status 
	ClientBlockingCall(common::StreamBase& stream, const Method& method, const RequestType& request, ResultType* result, ClientContext* client_context) {
		static protobuf::Request req;
		static protobuf::Response rsp;
		static std::string contents;

		req.Clear();
		rsp.Clear();

		if (expect_false(!request.SerializeToString(&contents))) {
			emoskit_log(ERROR_LOG, "parse request params error when try to call rpc method: %s", method.method_name());
			return Status(INVALID_ARGUMENT, "protobuf parse request params error");
		}

		req.set_method_id(method.hash_code());
		req.set_contents(contents);

		req.SerializeToString(&contents);

		uint32_t len = htonl(contents.length());

		stream.write((char*)&len, sizeof(len));
		stream << contents;
		
		int is_good = stream.flush().good() ? 0 : -1;

		// send succeed
		if (expect_true(0 == is_good)) {
			uint32_t resp_len;
			is_good = stream.read((char*)&resp_len, sizeof(resp_len)).good();

			if (expect_true(is_good)) {
				resp_len = ntohl(resp_len);
				contents.resize(resp_len);
				is_good = stream.read((char*)contents.data(), resp_len).good();
			}
			
			// receive result and parse
			if (expect_true(is_good)) {
				if (expect_true(rsp.ParseFromString(contents))) {
					if (expect_true(rsp.response_code() == protobuf::ResponseType::OK)) {
						if (expect_true(result->ParseFromString(rsp.contents()))) {
							return Status::OK;
						}
					}
					else {
						return Status(ERROR, rsp.info());
					}
				}
			}
			emoskit_log(ERROR_LOG, "receive result error when call rpc method: %s", method.method_name());
			return Status(ERROR, "receive result error");
		}
		else if (0 == errno) {
			return Status(CONNECT_CLOSED, "connect has been closed");
		}
		else {
			emoskit_log(ERROR_LOG, "send request error when try to call rpc method: %s, error: %s", method.method_name(), std::strerror(errno));
			return Status(ERROR, "send request error");
		}

	}
}
#endif // !_CLIENT_BLOCKING_CALL_H_

