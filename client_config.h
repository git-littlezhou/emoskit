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

#ifndef _CLIENT_CONFIG_H_
#define _CLIENT_CONFIG_H_

#include <string>
#include <vector>

namespace emoskit {
	
	typedef struct Endpoint{
		const char* ip;
		int port;

		Endpoint(const char* ip = nullptr, int port = -1) : ip(ip), port(port){}
	}Endpoint;

	class ClientConfig
	{
	public:
		ClientConfig();
		~ClientConfig();

		bool ReadConfigFile(const char* file_path);

		Endpoint* GetEndpointRandom();
		Endpoint* GetEndpointAt(unsigned int idx);

		int EndpointCount() { return endpoints_.size(); }

	private:
		std::vector<Endpoint> endpoints_;
		int server_count_;
		/* may be more properties */
	};

}
#endif // !_CLIENT_CONFIG_H_

