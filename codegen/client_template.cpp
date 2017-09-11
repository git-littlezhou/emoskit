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

#include "client_template.h"

namespace emoskit {

const char * EMOSKIT_CLIENT_CONFIG_TEMPLATE =
	R"(
[Server]
ServerCount = 2

[Server_0]
IP = 127.0.0.1
Port = 50060

[Server_1]
IP = 127.0.0.1
Port = 50060
)";

const char * EMOSKIT_CLIENT_MAIN_TEMPLATE =
	R"(
#include <iostream>

#include <emoskit/create_stream.h>

#include "$client_file$.h"

using namespace std;

using namespace $package_name$;
using namespace emoskit;

int main() {

	emoskit_set_log_level(LOG_LEVEL_DEBUG);

	ClientConfig client_config;
	client_config.ReadConfigFile("$client_conf_file$.conf");

	$client_class$ client(CreateStreamOrDie(&client_config), &client_config);

	string msg = "hello world!";
	
	cout << client.Echo(msg) << endl;

	return 0;
}
)";
}