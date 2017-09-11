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

#ifndef  _BACKEND_H_
#define  _BACKEND_H_

typedef double ev_tstamp;

namespace emoskit {
	/* forward declaration */
	class ev_loop;

	class Backend
	{
	public:
		Backend() : backend_fd(-1) {}
		virtual ~Backend() { if (backend_fd > 0) close(backend_fd); }
		virtual void backend_init() = 0;
		virtual void backend_modify(ev_loop* const loop, int fd, int oldev, int newev) = 0;
		virtual void backend_poll(ev_loop* const loop, ev_tstamp timeout) = 0;
		virtual void backend_destroy() = 0;
	protected:
		int backend_fd;
	};
}
#endif
