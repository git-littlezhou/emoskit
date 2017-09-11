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

#include "$service_impl_file$.h"

using namespace std;
using namespace emoskit;

int main(int argc, char *argv[]){

	emoskit_set_log_level(LOG_LEVEL_WARN);

	assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);

	Server server;
	server.set_event_loop_count(4);

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

TARGETS = greeter_server greeter_client

all: $(TARGETS)

greeter_server: $(SVR_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

greeter_client: $(CLI_OBJS)
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
}