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

#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include <thread>
#include <stdio.h>
#include <chrono>
#include "util.h"
#include "ev_loop.h"
#include "simple_msg_queue.h"
#include "msg_queue.h"
#include "network_proto.pb.h"
#include "output_buffer.h"
#include "service.h"
#include "server_context.h"
#include "network_event_handler.h"

namespace emoskit {

	namespace common{
		template<class T> class ThreadPool;
	}

	struct Task;

	#define RECEIVE_BUF_SIZE 128
	#define OUTPUT_BUF_SIZE 128
	
	typedef struct Response Response;
	typedef struct EventLoop EventLoop;
	typedef struct SocketState SocketState;

	class Server
	{
	public:
		Server();
		virtual ~Server();

		Server& operator=(const Server&) = delete;
		Server(const Server&) = delete;
		Server(Server&&) = delete;
		Server& operator = (Server&&) = delete;

		void set_host_and_port(const std::string& host, const std::string& port) { host_ = host; port_ = port; }
		void set_event_loop_count(int event_loop_count);
		void set_thread_pool_core_pool_size(int thread_pool_core_pool_size) { thread_pool_core_pool_size_ = thread_pool_core_pool_size; }
		void set_thread_pool_max_pool_size(int thread_pool_max_pool_size) { thread_pool_max_pool_size_ = thread_pool_max_pool_size; }
		void set_thread_pool_keep_alive_time_seconds(int thread_pool_keep_alive_time_seconds) { thread_pool_keep_alive_time_seconds_ = thread_pool_keep_alive_time_seconds; }
		void set_circbuf_size(ull circbuf_size) { circbuf_size_ = circbuf_size; }

		void RegisterService(Service* service){ service_ = service; }
		void noinline_ Start();
		void noinline_ Wait();
	private:
		inline_ void StopWatcherAndCloseSocket(ev_loop* loop, ev_watcher* watcher, SocketState& socket_state, util_fd_t fd);

	private:
		
		std::string host_;
		std::string port_;

		util_fd_t listen_fd_;

		int event_loop_count_;

		/* for thread pool*/
		int thread_pool_core_pool_size_;
		int thread_pool_max_pool_size_;
		int thread_pool_keep_alive_time_seconds_;

		/*for circbuf*/
		ull circbuf_size_;

		common::ThreadPool<Task>* thread_pool_;

		common::array<SocketState> socket_state_;

		EventLoop* event_loop_;

		Service* service_;

		ServerContext server_context_;

		//protobuf::Request request;

		friend void AcceptFdCb(ev_loop* loop, ev_watcher* watcher, short event);
		friend void ReadCb(ev_loop* loop, ev_watcher* watcher, short event);
		friend void ResponseCb(ev_loop* loop, ev_watcher* watcher, short event);
		friend Task;
	};

	struct Response{
		util_fd_t fd;
		protobuf::Response response_content;
		//char buf[64];
	};

	struct SocketState{
		SocketState() { 
			packet_length = 0;
			unread_length = 0;
			receive_buf_size = RECEIVE_BUF_SIZE;
			receive_buf = (char*)mm::mm_malloc(RECEIVE_BUF_SIZE); 
			receive_buf_offset = receive_buf;
		}
		~SocketState()
		{
			mm::mm_free(receive_buf);
		}

		void ResetReceiveBufState() { packet_length = unread_length = 0; receive_buf_offset = receive_buf; }
		unsigned int task_count;
		bool is_finish;

		uint32_t packet_length;
		uint32_t unread_length;
		char* receive_buf;
		char* receive_buf_offset;
		uint32_t receive_buf_size;

		common::OutputBuffer output_buffer;
	};

	struct EventLoop{
		ev_loop event_loop;
		std::thread epoll_thread;

		ev_io accept_fd_watcher;
		ev_io response_watcher;

		common::SimpleMsgQueue<util_fd_t> fd_queue;
		common::MsgQueue<Response> response_queue;

		Server* server;
	};

	inline_ void 
	Server::StopWatcherAndCloseSocket(ev_loop * loop, ev_watcher * watcher, SocketState &socket_state, util_fd_t fd)
	{
		loop->io_watcher_stop(static_cast<ev_io*>(watcher));
		if (socket_state.task_count == 0) {
			emoskit_log(INFO_LOG, "close fd: %d", fd);
			close(fd);
		}
		socket_state.is_finish = true;
	}
	
	inline_ void 
	Server::set_event_loop_count(int event_loop_count) {
		if (event_loop_count_ < 1) {
			emoskit_log(WARN_LOG, "event loop count is less than 1, set to 1 by default");
			event_loop_count_ = 1;
		}
		else event_loop_count_ = event_loop_count; 
	}
}
#endif 

