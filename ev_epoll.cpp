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

#include "ev_epoll.h"
#include "event.h"
#include "ev_loop.h"
#include "stdio.h"

emoskit::Epoll::Epoll() : epoll_events(NULL), epoll_eventmax(0)
{

}

emoskit::Epoll::~Epoll()
{
	if (epoll_events != NULL)
		mm::mm_free(epoll_events);
}

void 
emoskit::Epoll::backend_init()
{
	backend_fd = epoll_create(1024);

	if (backend_fd < 0) {
		emoskit_log(ERROR_LOG, "epoll_create error, aborting.");
		abort();
	}

	fcntl(backend_fd, F_SETFD, FD_CLOEXEC);

	epoll_eventmax = INIT_EVENT;
	epoll_events = (struct epoll_event*) mm::mm_malloc(sizeof(struct epoll_event) * epoll_eventmax);
}

void 
emoskit::Epoll::backend_modify(ev_loop* const loop, int fd, int oldev, int newev)
{
	/* ignoring EPOLL_CTL_DEL
	* if file handle referenced by the fd is closed, epoll will remove it automatically,
	* if not, we handle the spurious event in epoll_poll.
	* if the fd is added again and the first try to add it,
	* if it is failed, we test if the previous eventmask is the same as new eventmask, if not, we mod it.
	*/
	if (!newev)
		return;

	struct epoll_event ev;
	unsigned char old_previous_evs;

	/* record the old eventmask*/
	old_previous_evs = loop->get_ev_fd(fd).previous_evs;
	loop->get_ev_fd(fd).previous_evs = newev;

	/* store the tag in the upper 32 bit, if we get the event of a fd from epoll and the tag is not equal to
	* the tag we set here, it means it is a spurious notification. this is a problem of epoll.
	*/
	ev.data.u64 = (uint64_t)fd | ((uint64_t)++(loop->get_ev_fd(fd).tag) << 32);
	ev.events = (newev & EV_READ ? EPOLLIN : 0) | (newev & EV_WRITE ? EPOLLOUT : 0);

	/* if oldev is null*/
	if (expect_true(!epoll_ctl(backend_fd, oldev && newev != oldev ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, fd, &ev))) {
		return;
	}

	if (expect_true(errno == ENOENT)) {
		/* we did EPOLL_CTL_MOD, and fd in not registered with this epoll instance, try to add*/
		if (!epoll_ctl(backend_fd, EPOLL_CTL_ADD, fd, &ev))
			return;
	}
	else if (expect_true(errno == EEXIST)) {
		/* we did EPOLL_CTL_ADD, and the fd is already registered with this epoll instance.
		* this is because we ignored a previous del, if the olded is the same as the newev,
		* we assume it hasn't changed
		*/
		if (old_previous_evs == newev) {
			/* don't need to call epoll_ctl, decrement the tag */
			--(loop->get_ev_fd(fd).tag);
			return;
		}

		if (!epoll_ctl(backend_fd, EPOLL_CTL_MOD, fd, &ev))
			return;
	}

	/* the fd is ivalid, remove it*/
	loop->kill_fd(fd);

	/* didn't call epoll_ctl successfully, decrement the tag */
	--(loop->get_ev_fd(fd).tag);
}

inline void 
emoskit::Epoll::backend_poll(ev_loop* const loop, ev_tstamp timeout)
{
	int eventcnt;

	if (expect_false(timeout > EPOLL_MAX_TIMEOUT_MS))
		timeout = EPOLL_MAX_TIMEOUT_MS;

	eventcnt = epoll_wait(backend_fd, epoll_events, epoll_eventmax, timeout * 1e3);
	

	if (expect_false(eventcnt < 0)) {
		if (errno != EINTR)
			emoskit_log(ERROR_LOG, "emoskit epoll_wait error");
		return;
	}

	int i;
	for (i = 0; i < eventcnt; ++i) {
		struct epoll_event* ev = epoll_events + i;

		int fd = (uint32_t)ev->data.u64;

		int want = loop->get_ev_fd(fd).events;
		int got = (ev->events & (EPOLLOUT | EPOLLERR | EPOLLHUP) ? EV_WRITE : 0)
			| (ev->events & (EPOLLIN | EPOLLERR | EPOLLHUP) ? EV_READ : 0);

		/* check spurious notification*/
		if (expect_false(loop->get_ev_fd(fd).tag != (uint32_t)(ev->data.u64 >> 32))) {
			continue;
		}

		/* we received an event but we are not interested in it, try del or mod
		* this is because we ignored del in order to get some performance improvements
		*/
		if (expect_false(got & ~want)) {
			loop->get_ev_fd(fd).previous_evs = want;

			ev->events = (want & EV_WRITE ? EPOLLIN : 0)
				| (want & EV_READ ? EPOLLOUT : 0);

			if (epoll_ctl(backend_fd, want ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, fd, ev)) {
				continue;
			}
		}

		loop->feed_fd_event(fd, got);
	}
}

inline void 
emoskit::Epoll::backend_destroy()
{
	if (epoll_events != NULL)
		mm::mm_free(epoll_events);
	if (backend_fd > 0)
		close(backend_fd);
	backend_fd = -1;
	epoll_events = NULL;
}