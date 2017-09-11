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

#ifndef  _EVENT_H_
#define  _EVENT_H_

namespace emoskit {

	//forward declaration
	namespace common {
		class quad_minheap;
	}
	class ev_loop;

	#define CB_DECLARE void(*cb)(ev_loop*, ev_watcher*, short)
	#define SCHE_CB_DECLARE ev_tstamp(*reschedule_cb)(ev_periodic* w, ev_tstamp now)
	typedef double ev_tstamp;

	#define LOOP_FRIEND friend class ev_loop
	#define HEAP_FRIEND friend common::quad_minheap

	enum {
		EV_READ = 0x01,
		EV_WRITE = 0x02,
		EV_IO = EV_READ,
		EV_TIMER = 0x00000010,
		EV_PERIODIC = 0x00000020,
		EV_SIGNAL = 0x00000040,
		EV_ERROR = 0x80000000
	};

#define WATCHER_ACTIVE(w) w->active

	class ev_watcher
	{
	public:
		ev_watcher() { active = pending = priority = 0;  data = nullptr; cb = nullptr; }
		inline void set_data(void* data) { this->data = data; }
		inline void* get_data() { return data; }
		inline void set_cb(CB_DECLARE) { this->cb = cb; }

		virtual inline int get_fd() { return -1; }
		virtual ~ev_watcher() {}

	private:
		int active;
		int pending;
		int priority;
		void* data;
		CB_DECLARE;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	class ev_watcher_list : public ev_watcher
	{
	public:
		ev_watcher_list() { next = nullptr; }
		virtual ~ev_watcher_list() {}
		//void set_next(ev_watcher_list* next) { this->next = next; }
		//ev_watcher_list* get_next() { return this->next; }
	private:
		ev_watcher_list* next;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	class ev_io : public ev_watcher_list
	{
	public:
		ev_io() { fd = -1; events = 0; }
		virtual inline int get_fd() { return fd; }
		virtual ~ev_io() {}

	private:
		
		int fd;
		int events;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	class ev_signal : public ev_watcher_list
	{
	public:
		virtual inline int get_fd() { return signum; }
		virtual ~ev_signal() {}

	private:
		int signum; 
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	#define WATCHER_AT(w) w->at

	class ev_watcher_time : public ev_watcher
	{
	public:
		ev_tstamp get_at() { return at; }
		virtual ~ev_watcher_time() {}

	private:
		ev_tstamp at;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	class ev_periodic : public ev_watcher_time
	{
	public:
		inline void set_reschedule_cb(SCHE_CB_DECLARE) { this->reschedule_cb = reschedule_cb; }
		virtual ~ev_periodic() {}

	private:
		ev_tstamp offset;
		ev_tstamp interval;
		SCHE_CB_DECLARE;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};

	class ev_timer : public ev_watcher_time
	{
	public:
		virtual ~ev_timer() {}

	private:
		ev_tstamp repeat;
		LOOP_FRIEND;
		HEAP_FRIEND;
	};
}
#endif
/*
#define CB_DECLARE(x) void(*x)(ev_loop *loop, ev_watcher* w, int revents)
#define CB_VAR(x) CB_DECLARE(x);

class ev_watcher
{
public:
	ev_watcher() {}
	~ev_watcher() {}

	void set_active(int active) { this->active = active; }
	void set_pending(int pending) { this->pending = pending; }
	void set_priority(int priority) { this->priority = priority; }
	void set_data(void *data) { this->data = data; }
	void set_cb(CB_DECLARE(cb)) { this->cb = cb; }

	int get_active() { return this->active; }
	int get_pending() { return this->pending; }
	int get_priority() { return this->priority; }
	void * get_data() { return data; }
	CB_DECLARE(get_cb()) { return cb; }

private:
	int active;
	int pending;
	int priority;
	void* data;
	CB_VAR(cb)
};

class ev_watcher_list 
{
public:
	void set_next(ev_watcher_list* next) { this->next = next; }
	ev_watcher_list* get_next() { return this->next; }
private:
	ev_watcher_list* next;
};
*/
