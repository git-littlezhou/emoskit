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

#include "ev_loop.h"

emoskit::ev_loop::~ev_loop()
{
	if (backend != NULL)
		delete backend;
}

void noinline_ 
emoskit::ev_loop::init(int flag)
{
	dummy_watcher.cb	= dummycb;
	timeout_blocktime	= 0;
	io_blocktime		= 0;
	activecnt			= 0;
	fdchangescnt		= 0;

	for (int i = 0; i < NUMPRI; ++i) {
		pendingcnt[i] = 0;
	}

#if EV_USE_EPOLL
	backend = new Epoll;
#endif // EV_USE_EPOLL

	backend->backend_init();

	mn_time_now = util::get_time();

}

int noinline_ 
emoskit::ev_loop::dispatch(int flag)
{
	loop_done = EV_RUN;
	ev_tstamp timeout;
	ev_tstamp min_time;

	do {

		update_fd();

		time_update();

		timeout = 0.;

		if (expect_true(!(!activecnt || flag & EVRUN_NOWAIT))) {

			timeout = MAXTIME_BLOCK;
			if (!timer_heap.isempty()) {
				min_time = HEAP_AT(timer_heap.top()) - mn_time_now;
				if (timeout > min_time)
					timeout = min_time;
			}

			if (expect_false(timeout < timeout_blocktime))
				timeout = timeout_blocktime;
		}
		backend->backend_poll(this, timeout);

		time_update();

		timer_notify();
		invoke_pendings();

	} while (expect_true(activecnt && !loop_done && !(flag & (EVRUN_ONCE | EVRUN_NOWAIT))));

	return activecnt;
}

/* loop through the pendings array according to priority
* and invoke cb function
*/
void noinline_ 
emoskit::ev_loop::invoke_pendings()
{
	for (int pri = 0; pri < NUMPRI; ++pri) {
		while (pendingcnt[pri]) {
			ev_pending *pendingev = &pendings[pri][--pendingcnt[pri]];

			ev_watcher *pendingw = pendingev->w;

			pendingw->pending = 0;
			//pendingw->cb(this, pendingw->get_fd(), pendingev->events, pendingw->data);
			pendingw->cb(this, pendingw, pendingev->events);
		}
	}
}

void noinline_
emoskit::ev_loop::io_watcher_init(ev_io * iow, int fd, int ev, CB_DECLARE, void * data, int pri)
{
	if (expect_false(iow->active))
		return;

	assert(("io_watcher_init called with negative fd", fd >= 0));
	assert(("io_watcher_init call with illgeal event mask", !(ev & ~(EV_READ | EV_WRITE))));

	/* adjust pri*/
	pri = pri < 0 ? 0 : pri;
	pri = pri > NUMPRI - 1 ? NUMPRI - 1 : pri;

	watcher_init(iow, data, cb, pri);

	iow->events	  = ev;
	iow->fd		  = fd;
	iow->next	  = nullptr;
}

void noinline_
emoskit::ev_loop::io_watcher_start(ev_io * iow)
{
	if (expect_false(iow->active))
		return;

	watcher_enable(iow, 1);

	int fd = iow->fd;
	watcher_list_add(&get_ev_fd(fd).head, iow);

	fd_change(fd, EV_IS_CHANGED);
}

void noinline_
emoskit::ev_loop::io_watcher_stop(ev_io * iow)
{
	clear_pending(iow);

	if (expect_false(!(iow->active)))
		return;

	int fd = iow->fd;
	watcher_list_del(&get_ev_fd(fd).head, iow);
	watcher_disable(iow);

	fd_change(fd, EV_IS_CHANGED);
}

void noinline_ 
emoskit::ev_loop::timer_watcher_init(ev_timer * timerw, ev_tstamp after, ev_tstamp repeat, CB_DECLARE, void*data)
{
	if (expect_false(timerw->active))
		return;

	watcher_init(timerw, data, cb);

	timerw->at		= after;
	timerw->repeat	= repeat;
}

void noinline_ 
emoskit::ev_loop::timer_watcher_start(ev_timer * timerw)
{
	if (expect_false(timerw->active))
		return;

	WATCHER_AT(timerw) += mn_time_now;

	watcher_enable(timerw, 0);

	timer_heap.push(timerw);
	HEAP_CACHE_AT(timer_heap.get(timerw->active - 1));
}

void noinline_ 
emoskit::ev_loop::timer_watcher_stop(ev_timer * timerw)
{
	clear_pending(timerw);

	if (expect_false(!(timerw->active)))
		return;

	timer_heap.remove(timerw->active - 1);
	timerw->at -= mn_time_now;

	watcher_disable(timerw);
}
