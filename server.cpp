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

#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <sys/eventfd.h>
#include "logger.h"
#include "server.h"
#include "status.h"
#include "task.h"
#include "thread_pool.h"

emoskit::Server::Server()
	//:host_("222.201.145.219"), port_("50060")
	: host_("127.0.0.1"), port_("50060")
	//: host_("hadoop-0C"), port_("50060")
{
	listen_fd_								= -1;
	event_loop_count_						= 1;
	thread_pool_core_pool_size_				= std::thread::hardware_concurrency() / 2;
	thread_pool_max_pool_size_				= thread_pool_core_pool_size_;
	thread_pool_keep_alive_time_seconds_	= 60;
	circbuf_size_							= 1024;
	thread_pool_							= nullptr;
	event_loop_								= nullptr;
}

emoskit::Server::~Server()
{
	delete	  thread_pool_; 
	delete [] event_loop_;
}

void noinline_
emoskit::Server::Start()
{
	if (listen_fd_ != -1)
		return;

	listen_fd_ = util::CreateSocketAndListen(host_.c_str(), port_.c_str(), nullptr);

	if (listen_fd_ < 0) {
		emoskit_log(ERROR_LOG, "create socket error for %s:%s, exiting.", host_.c_str(), port_.c_str());
		exit(1);
	}

	/*create thread pool*/
	thread_pool_ = new common::ThreadPool<Task>(
		thread_pool_core_pool_size_ , 
		thread_pool_max_pool_size_ , 
		thread_pool_keep_alive_time_seconds_ , 
		circbuf_size_ , 
		event_loop_count_ > 1 ? common::ProductionStrategy::MUTIL_THREADS : common::ProductionStrategy::SINGLE_THREAD);

	thread_pool_->start();

	event_loop_ = new EventLoop[event_loop_count_];

	for (int i = 0; i < event_loop_count_; ++i) {

		event_loop_[i].event_loop.init(0);

		/*init accept_fd_watchers_*/
		event_loop_[i].server = this;
		event_loop_[i].event_loop.io_watcher_init(&event_loop_[i].accept_fd_watcher,
			event_loop_[i].fd_queue.get_event_fd(), EV_READ, AcceptFdCb, static_cast<void*>(&event_loop_[i]), 0);
		event_loop_[i].event_loop.io_watcher_start(&event_loop_[i].accept_fd_watcher);

		/*init response_watchers_*/
		event_loop_[i].event_loop.io_watcher_init(&event_loop_[i].response_watcher,
			event_loop_[i].response_queue.get_event_fd(), EV_READ, ResponseCb, static_cast<void*>(&event_loop_[i]));
		event_loop_[i].event_loop.io_watcher_start(&event_loop_[i].response_watcher);

		event_loop_[i].epoll_thread = std::thread([this, i]() { this->event_loop_[i].event_loop.dispatch(0); });
		event_loop_[i].epoll_thread.detach();
	}

}

void noinline_
emoskit::Server::Wait()
{
	util_fd_t fd;
	int idx = 0, count = 0;

	for (;;) {
		if (expect_false((fd = accept(listen_fd_, NULL, NULL)) < 0)) {
			if (errno == EINTR || errno == ECONNABORTED)
				continue;
			else {
				emoskit_log(ERROR_LOG, "emoskit accept error: %s", strerror(errno));
				continue;
			}
		}

		idx %= event_loop_count_;
		while (expect_false(event_loop_[idx].fd_queue.Put(fd) != 0)) {
			++idx;
			idx %= event_loop_count_;
			if (++count == event_loop_count_) {
				std::this_thread::yield();
				count = 0;
			}
		}

		event_loop_[idx].fd_queue.set_flag_and_notify();

		++idx;
		count = 0;
	}
}

//void 
//emoskit::Task::operator()()
//{
//	if (fd == -1)
//		return;
//
//	store_response.clear();
//
//	Status result = event_loop->server->service_->CallMethod(
//		&(event_loop->server->server_context_), request.method_id(), request.contents(), store_response);
//
//	llong seq;
//	common::MsgQueue<Response>* response_queues = &event_loop->response_queue;
//	Response& response = response_queues->TakeObjectAndSeq(seq);
//	
//	response.response_content.Clear();
//
//	response.response_content.set_contents(store_response);
//
//	if (expect_true(result.is_ok())){
//		response.response_content.set_response_code(protobuf::OK);
//		//response.response_content.set_contents(request.contents().c_str());
//	}
//	else{
//		response.response_content.set_response_code(protobuf::ERROR);
//		response.response_content.set_info(result.message().c_str());
//	}
//
//	//response = new Response;
//	//memcpy(response.buf, buf, sizeof(buf));
//	response.fd = fd;
//
//	response_queues->Publish(seq);
//	response_queues->set_flag_and_notify();
//
//	//write(fd, buf, sizeof(buf));
//}
