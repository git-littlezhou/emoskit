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

#include "server_template.h"

namespace emoskit {

const char * EMOSKIT_SERVER_MAIN_TEMPLATE =
	R"(
#include <iostream>
#include <signal.h>
#include <assert.h>

#include <emoskit/server.h>
#include <emoskit/server_config.h>

#include "$service_impl_file$.h"

using namespace std;
using namespace emoskit;

void PrintHelp(const char* program) {
	printf("Usage: %s [-c <config file>] [-l <log level>] [-d]\n", program);
	printf("	-f <config file>             # server config file\n");
	printf("	-l <log level>               # DEBUG:1, INFO:2, WARN:3, ERROR:4\n");
	printf("	-d                           # daemonize\n");
	printf("\n");

	exit(0);
}

int main(int argc, char *argv[]){

	const char * config_file = nullptr;
	bool daemonize = false;
	int log_level = -1;

	int c;
	while ((c = getopt(argc, argv, "c:hl:d")) != EOF) {
		switch (c) {
		case 'c':
			config_file = optarg;
			break;
		case 'd':
			daemonize = true;
			break;
		case 'l':
			log_level = atoi(optarg);
			break;
		case  'h':
		default:
			PrintHelp(argv[0]);
			break;
		}
	}

	if (daemonize) util::Daemonize();

	assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);

	if (nullptr == config_file) PrintHelp(argv[0]);

	ServerConfig server_config;
	if (!server_config.ReadConfigFile(config_file))
		return 0;

	if (log_level > 0)
		server_config.set_log_level(log_level);

	Server server(&server_config);

	$service_impl_class$ service;

	server.RegisterService(&service);

	server.Start();
	server.Wait();

	return 0;
}
)";

const char * EMOSKIT_MAKEFILE_TEMPLATE =
	R"(
$protobuf_home$
EMOSKIT_HOME=$emoskit_home$

ifeq ($(debug),y)
	OPT = -g2
else
	OPT = -O2
endif


CC = g++
RM = rm -f

CXXFLAGS = -std=c++11 -ffunction-sections $(OPT)
LDFLAGS = -Wl,-gc-sections $-Lprotobuf_lib$ -L$(EMOSKIT_HOME)/lib/
LIBS = -lpthread -lprotobuf -lemoskit
CPPFLAGS = $-Iprtobuf_include$ -I $(EMOSKIT_HOME)/include/


SVR_OBJS = $pb_file$.o $server_main_file$.o $service_file$.o $service_impl_file$.o

CLI_OBJS = $pb_file$.o $client_main_file$.o $client_file$.o $stub_file$.o

%.o : %.cc
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
%.o : %.cc
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
%.o : %.cpp
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

TARGETS = $server_exec_file$ $client_exec_file$

all: $(TARGETS)

$server_exec_file$: $(SVR_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$client_exec_file$: $(CLI_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$server_main_file$.cpp: $service_impl_file$.h
$service_impl_file$.cpp: $service_impl_file$.h
$service_impl_file$.h: $service_file$.h
$service_file$.cpp: $service_file$.h
$service_file$.h: $pb_file$.h

$client_main_file$.cpp: $client_file$.h
$client_file$.cpp: $client_file$.h
$client_file$.h: $stub_file$.h
$stub_file$.cpp: $stub_file$.h
$stub_file$.h: $pb_file$.h

$pb_file$.cc: $pb_file$.h
$pb_file$.h: $proto_file_name$
	$protobuf_bin$protoc $-Iprtobuf_include$ --cpp_out=. -I $(EMOSKIT_HOME) -I. $^

clean:
	@$(RM) $(TARGETS)
	@$(RM) *.o
	@$(RM) *.pb.*

)";

const char * EMOSKIT_SERVER_CONFIG_TEMPLATE =
	R"(
[Server]
IP = 127.0.0.1
Port = 50060
EventLoopCnt = 3
WorkerPoolCorePoolSize = 6
WorkerPoolMaxPoolSize = 9
WorkerPoolKeepAliveTimeSeconds = 60
CircbufSize = 1024
 
[Log]
LogDir = ./log
LogLevel = 3	# DEBUG:1, INFO:2, WARN:3, ERROR:4
)";
}