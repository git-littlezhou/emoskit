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

#ifndef  _EV_EPOLL_H_
#define  _EV_EPOLL_H_

#include <sys/epoll.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include "mm.h"
#include "logger.h"
#include "backend.h"

// can remove
typedef double ev_tstamp;
//#define expect_true(cond) __builtin_expect(!!(cond), 1)
//#define expect_false(cond) __builtin_expect(!!(cond), 0)

#define INIT_EVENT 64

/* In kernels before 2.6.37, a timeout value larger than 
* approximately LONG_MAX / HZ milliseconds is treated as -1.
* HZ can be as big as 1000, and LONG_MAX can be as small as (1<<31)-1, 
* so the largest number of msec we can support here is 
* ((1<<31)-1 - 999ULL) / 1000 = 2147482.  Let's
* round that down by 47 seconds.(ULL is error 误差)
*/
#define EPOLL_MAX_TIMEOUT_MS (35*60*1000)

namespace emoskit {

	class Epoll :public Backend {
	public:
		Epoll();
		~Epoll();
		void backend_init();
		void backend_modify(ev_loop* const loop, int fd, int oldev, int newev);
		void backend_poll(ev_loop* const loop, ev_tstamp timeout);
		void backend_destroy();
	private:
		struct epoll_event* epoll_events;
		int epoll_eventmax;
	};
}
#endif