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

#include "network_event_handler.h"
#include "server.h"
#include "task.h"
#include "thread_pool.h"

namespace emoskit{

	void
	ResponseCb(ev_loop* loop, ev_watcher* watcher, short event) {
		EventLoop* event_loop = static_cast<EventLoop*>(watcher->get_data());
		common::MsgQueue<Response>* msg_queue = &(event_loop->response_queue);

		msg_queue->unset_flag_and_clear_notify();

		util_fd_t fd;
		SocketState* socket_state;
		Response response;
		uint32_t packet_length, size;

		for (;;) {
			if (msg_queue->GetConsumerObject(response) == -1)
				break;

			fd = response.fd;
			socket_state = &(event_loop->server->socket_state_[fd]);

			packet_length = response.response_content.ByteSize();
			*(uint32_t*)(socket_state->output_buffer.get_empty_buffer(sizeof(uint32_t))) = htonl(packet_length);
			response.response_content.SerializeToArray(socket_state->output_buffer.get_empty_buffer(packet_length), packet_length);
			/*Response& response = msg_queue->GetConsumerObjectAndseq(seq);
			if (seq == -1)
			break;*/

			//write(response->fd, response->buf, sizeof(response->buf));
			if ((size = write(fd, socket_state->output_buffer.GetBufferContents(), socket_state->output_buffer.BufferContentsSize())) < 0) {
				// TODO: error handle
			}
			socket_state->output_buffer.Consume(size);
			socket_state->output_buffer.AdjustBuffer();
			//delete response;
			/*if (write(response.fd, response.buf, sizeof(response.buf)) == -1) {
			perror("response error.");
			}*/
			//response.response_content.SerializeToFileDescriptor(response.fd);
			//msg_queue->ConsumerDone(seq);
		}
	}

	void
	ReadCb(ev_loop* loop, ev_watcher* watcher, short event) {
		EventLoop* event_loop = static_cast<EventLoop*>(watcher->get_data());
		Server* server = event_loop->server;

		int fd;

		if (event & EV_READ) {
			int size;
			llong seq;
			//Task task;
			fd = watcher->get_fd();
			SocketState& socket_state = server->socket_state_[fd];
			if (expect_true(socket_state.packet_length == 0)) {
				//get the packet length
				if ((size = read(fd, &socket_state.packet_length, sizeof(uint32_t))) <= 0) {
					loop->io_watcher_stop(static_cast<ev_io*>(watcher));
					if (socket_state.task_count == 0) {
						emoskit_log(INFO_LOG, "Socket closed by client, close fd: %d", fd);
						close(fd);
					}
					socket_state.is_finish = true;
					return;
				}
				socket_state.packet_length = ntohl(socket_state.packet_length);
				socket_state.unread_length = socket_state.packet_length;

				if (expect_false(socket_state.packet_length > socket_state.receive_buf_size)) {
					// if packet size > 8k, the emoskit treats it as protocol error, close socket
					if (socket_state.packet_length > (2 << 13)) {
						emoskit_log(ERROR_LOG, "Protocol error: packet length too long. Try to close socket, fd: %d", fd);
						socket_state.packet_length = 0;
						server->StopWatcherAndCloseSocket(loop, watcher, socket_state, fd);
						return;
					}
					socket_state.receive_buf_size = util::rounddown_pow_of_two(socket_state.packet_length);
					socket_state.receive_buf = (char*)mm::mm_realloc(socket_state.receive_buf, socket_state.receive_buf_size);
					socket_state.receive_buf_offset = socket_state.receive_buf;
				}
			}

			if (expect_false((size = read(fd, socket_state.receive_buf_offset, socket_state.unread_length)) <= 0)) {
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					return;
				emoskit_log(ERROR_LOG, "Read packet error. Try to close socket, fd: %d", fd);
				server->StopWatcherAndCloseSocket(loop, watcher, socket_state, fd);
				return;
			}
			else if (size < socket_state.packet_length){
				emoskit_log(INFO_LOG, "Read part of the packet. fd: %d", fd);
				socket_state.receive_buf_offset = socket_state.receive_buf + size;
				socket_state.unread_length = socket_state.packet_length - size;
				return;
			}

			Task& task = server->thread_pool_->getTaskObjectAndSeq(seq);
			if (expect_false(!task.request_content.ParseFromArray(socket_state.receive_buf, socket_state.packet_length))) {
				emoskit_log(ERROR_LOG, "Protocolerror: packet parse error. Try to close socket, fd: %d", fd);
				task.fd = -1;
				server->thread_pool_->publish_task(seq);
				
				server->StopWatcherAndCloseSocket(loop, watcher, socket_state, fd);
				socket_state.ResetReceiveBufState();
				return;
			}
			socket_state.ResetReceiveBufState();

			task.InitCall(&server->server_context_, 
				server->service_->GetMethod(task.request_content.method_id()), &task.request_content.contents());
				
			task.fd = fd;
			task.event_loop = event_loop;
			//write(fd, task.buf, sizeof(task.buf));
			server->thread_pool_->publish_task(seq);
			return;
		}
		else if (event & EV_ERROR) {
			emoskit_log(ERROR_LOG, "invalid fd=%d, and has been closed.", fd);
		}
	}

	void
	AcceptFdCb(ev_loop* loop, ev_watcher* watcher, short event){
		EventLoop* event_loop = static_cast<EventLoop*>(watcher->get_data());
		common::SimpleMsgQueue<int>* msg_queue = &(event_loop->fd_queue);

		msg_queue->unset_flag_and_clear_notify();

		int fd;

		for (;;) {
			if (msg_queue->Get(fd) < 0)
				break;
			emoskit_log(DEBUG_LOG, "accept fd: %d", fd);
			util::MakeSocketNonblocking(fd);

			event_loop->server->socket_state_[fd].task_count = 0;
			event_loop->server->socket_state_[fd].packet_length = 0;
			event_loop->server->socket_state_[fd].unread_length = 0;
			event_loop->server->socket_state_[fd].is_finish = false;

			ev_io* iow = new ev_io;
			loop->io_watcher_init(iow, fd, EV_READ, ReadCb, watcher->get_data());
			loop->io_watcher_start(iow);
		}
	}

}