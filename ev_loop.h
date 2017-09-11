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

#ifndef _EV_LOOP_H_
#define _EV_LOOP_H_

// can remove
#define EV_USE_EPOLL 1

#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stack>
#include "array.h"
#include "event.h"
#include "util.h"
#include "4-minheap-internal.h"
#if EV_USE_EPOLL
#include "ev_epoll.h"
#endif // EV_USE_EPOLL


#define EV_IS_CHANGED 1

#define NUMPRI 5
#define MAXTIME_BLOCK 60
typedef unsigned char uchar;

namespace emoskit {

	enum {
		EVRUN_ONCE = 1,
		EVRUN_NOWAIT = 2
	};
	enum {
		EV_RUN = 0,
		EV_BREAK = 1
	};

	/* dummy callback for dummy watcher */
	static void
		dummycb(ev_loop*, ev_watcher*, short revents)
	{
	}

	typedef struct {
		ev_watcher* w;
		short events;
	}ev_pending;

	typedef struct {
		ev_watcher_list* head;
		uchar events;
		uchar ischanged;
		uchar previous_evs;
#if  EV_USE_EPOLL
		uint32_t tag;
#endif //  EV_USE_EPOLL
	} ev_fd;

	class ev_loop {
	public:
		~ev_loop();

		void noinline_ init(int flag);
		int noinline_ dispatch(int flag);

		ev_fd& get_ev_fd(int fd) { return fds[fd]; }

		inline_ void kill_fd(int fd);
		inline_ void feed_fd_event(int fd, int ev);

		void set_io_collect_interval(ev_tstamp interval) { io_blocktime = interval; }
		void set_timeout_collect_interval(ev_tstamp interval) { timeout_blocktime = interval; }

		void loop_break() { loop_done = EV_BREAK; }

		void noinline_ io_watcher_init(ev_io* iow, int fd, int ev, CB_DECLARE, void* data = nullptr, int pri = NUMPRI / 2);
		void noinline_ io_watcher_start(ev_io* iow);
		void noinline_ io_watcher_stop(ev_io* iow);

		void noinline_ timer_watcher_init(ev_timer* timerw, ev_tstamp after, ev_tstamp repeat, CB_DECLARE, void*data = nullptr);
		void noinline_ timer_watcher_start(ev_timer* timerw);
		void noinline_ timer_watcher_stop(ev_timer* timerw);

	private:

		void watcher_enable(ev_watcher* w, int active) { w->active = active; ++activecnt; }
		void watcher_disable(ev_watcher* w) { w->active = 0; --activecnt; }

		inline_ void watcher_list_add(ev_watcher_list** head, ev_watcher_list* elem);
		inline_ void watcher_list_del(ev_watcher_list** head, ev_watcher_list* elem);

		inline_ void feed_event(ev_watcher* w, int ev);

		inline_ void invert_feed(ev_watcher* w);
		inline_ void invert_feed_done(int ev);

		inline_ void fd_change(int fd, int flag);

		inline_ void time_update();
		inline_ void timer_notify();

		inline_ void clear_pending(ev_watcher* w);
		inline_ void update_fd();

		void noinline_ invoke_pendings();

		inline_ void watcher_init(ev_watcher* w, void*data, CB_DECLARE, int pri = NUMPRI / 2, int active = 0, int pending = 0);

		ev_tstamp mn_time_now;

		common::array<ev_pending> pendings[NUMPRI];
		int pendingcnt[NUMPRI];

		ev_tstamp io_blocktime;
		ev_tstamp timeout_blocktime;

		common::array<int> fdchanges;
		int fdchangescnt;

		int activecnt;
		int loop_done;

		common::array<ev_fd> fds;

		/* for time-related watcher, ensure that trigger in chronological order*/
		std::stack<ev_watcher*> stack;

		/**/
		ev_watcher dummy_watcher;

		Backend* backend;

		common::quad_minheap timer_heap;
	};

	inline_ void
	ev_loop::watcher_list_add(ev_watcher_list ** head, ev_watcher_list * elem)
	{
		elem->next = *head;
		*head = elem;
	}

	inline_ void
	ev_loop::watcher_list_del(ev_watcher_list ** head, ev_watcher_list * elem)
	{
		while (*head) {
			if (*head == elem) {
				*head = elem->next;
				break;
			}
			head = &((*head)->next);
		}
	}

	/* the fd is invalid, remove all the watcher*/
	inline_ void
	ev_loop::kill_fd(int fd)
	{
		ev_io* iow;
		while ((iow = static_cast<ev_io*>(get_ev_fd(fd).head)))
		{
			io_watcher_stop(iow);
			feed_event(iow, EV_ERROR);
		}
		close(fd);
	}

	/* if reify is set, it means the event that fd interest in have changed
	* while we were waiting for new events
	*/
	inline_ void
	ev_loop::feed_fd_event(int fd, int ev)
	{
		if (expect_false(get_ev_fd(fd).ischanged))
			return;

		ev_fd &evfd = get_ev_fd(fd);
		ev_io* ptrWatcher;
		int evhit;

		for ((ptrWatcher = static_cast<ev_io*>(evfd.head)); ptrWatcher; ptrWatcher = static_cast<ev_io*>(ptrWatcher->next)) {
			evhit = ptrWatcher->events & ev;
			if (evhit) {
				feed_event(ptrWatcher, evhit);
			}
		}
	}

	/* put pending watcher into pending array*/
	inline_ void
	ev_loop::feed_event(ev_watcher * w, int ev)
	{
		int pri = w->priority;

		if (expect_false(w->pending)) {
			pendings[pri][w->pending - 1].events |= ev;
		}
		else {
			w->pending = ++pendingcnt[pri];
			pendings[pri][w->pending - 1].w = w;
			pendings[pri][w->pending - 1].events = ev;
		}
	}

	inline_ void
	ev_loop::invert_feed(ev_watcher * w)
	{
		stack.push(w);
	}

	inline_ void
	ev_loop::invert_feed_done(int ev)
	{
		while (!stack.empty()) {
			feed_event(stack.top(), ev);
			stack.pop();
		}
	}

	inline_ void
	ev_loop::fd_change(int fd, int flag)
	{
		uchar ischanged = get_ev_fd(fd).ischanged;
		get_ev_fd(fd).ischanged |= flag;

		if (expect_true(!ischanged)) {
			++fdchangescnt;
			fdchanges[fdchangescnt - 1] = fd;
		}
	}

	inline_ void
	ev_loop::time_update()
	{
		mn_time_now = util::get_time();
	}

	inline_ void
	ev_loop::clear_pending(ev_watcher * w)
	{
		if (expect_true(w->pending)) {
			pendings[w->priority][w->pending - 1].w = &dummy_watcher;
			w->pending = 0;
		}
	}

	/* loop through the fdchanges array, check if fd's event set has changed
	* if yes, call backend_modify to modify it
	* if not, do nothing
	* finally empty the fdchanges array
	*/
	inline_ void
	ev_loop::update_fd()
	{
		for (int i = 0; i < fdchangescnt; ++i) {
			int fd = fdchanges[i];
			ev_fd& evfd = get_ev_fd(fd);

			uchar changed = evfd.ischanged;
			uchar o_event = evfd.events;

			evfd.ischanged = 0;
			evfd.events = 0;

			ev_io* iow;
			for (iow = static_cast<ev_io*>(evfd.head); iow; iow = static_cast<ev_io*>(iow->next)) {
				evfd.events |= iow->events;
			}

			if (o_event != evfd.events)
				changed = EV_IS_CHANGED;

			if (changed & EV_IS_CHANGED)
				backend->backend_modify(this, fd, o_event, evfd.events);
		}
		fdchangescnt = 0;
	}

	inline_ void
	ev_loop::watcher_init(ev_watcher * w, void * data, CB_DECLARE, int pri, int active, int pending)
	{

		/* adjust pri*/
		pri = pri < 0 ? 0 : pri;
		pri = pri > NUMPRI - 1 ? NUMPRI - 1 : pri;

		w->active = active;
		w->pending = pending;
		w->priority = pri;
		w->data = data;
		w->cb = cb;
	}

	inline_ void
	ev_loop::timer_notify()
	{
		if (!timer_heap.isempty() && HEAP_AT((timer_heap.top())) < mn_time_now) {
			do {
				common::min_heap_t& wt = timer_heap.top();
				ev_timer* et = static_cast<ev_timer*>(HEAP_WT(wt));
				if (et->repeat) {
					WATCHER_AT(et) += et->repeat;
					if (WATCHER_AT(et) < mn_time_now)
						WATCHER_AT(et) = mn_time_now;

					HEAP_CACHE_AT(wt);
					timer_heap.shift_down(0);
				}
				else {
					timer_watcher_stop(et);
				}

				invert_feed(et);
			} while (!timer_heap.isempty() && HEAP_AT((timer_heap.top())) < mn_time_now);
			invert_feed_done(EV_TIMER);
		}
	}
}
#endif // ! EV_LOOP_H