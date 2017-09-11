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

#ifndef _NOTIFIER_H_
#define _NOTIFIER_H_

#include "util.h"

namespace emoskit {
	
	namespace common {
		
		class Notifier
		{
		public:
			Notifier();
			~Notifier();
			inline_ void set_flag_and_notify();
			inline_ void unset_flag_and_clear_notify();
			int get_event_fd() { return event_fd_; }

		private:
			//volatile int flag_;
			//std::atomic_flag flag_;
			std::atomic<bool> flag_;
			util_fd_t event_fd_;
			uint64_t bytes_for_eventfd_notify;
		};

		inline_ 
		//Notifier::Notifier():flag_(ATOMIC_FLAG_INIT){
		Notifier::Notifier() :flag_(ATOMIC_FLAG_INIT) {
			//flag_ = 0;
			flag_ = false;
			bytes_for_eventfd_notify = 1;
			event_fd_ = util::CreateEventFd();
			if (event_fd_ < 0) {
				emoskit_log(ERROR_LOG, "create eventfd error, exiting...");
				exit(1);
			}
			util::MakeSocketNonblocking(event_fd_);
		}

		inline_
		Notifier::~Notifier(){
			close(event_fd_);
		}

		inline_ void 
		Notifier::set_flag_and_notify(){
			//EMOSKIT_MEMORY_FENCE;
			//if (expect_false(!flag_)) {
			//	flag_ = 1;
			//	EMOSKIT_MEMORY_FENCE_RELEASE;
			//if(expect_false(!flag_.test_and_set())){
			if (expect_false(!flag_.exchange(true))) {
				if (expect_false(write(event_fd_, &bytes_for_eventfd_notify, sizeof(bytes_for_eventfd_notify)) == -1)) {
					if (errno != EINTR){
						emoskit_log(ERROR_LOG, "write event fd error, notify failed.");
						flag_.exchange(false);
					}
				}
			}
		}

		inline_ void 
		Notifier::unset_flag_and_clear_notify()
		{
			if (expect_false(read(event_fd_, &bytes_for_eventfd_notify, sizeof(bytes_for_eventfd_notify)) == -1)) {
				if (errno != EINTR){
					emoskit_log(ERROR_LOG, "read event fd error, clear notify failed ");
				}
			}
			//EMOSKIT_MEMORY_FENCE;
			//flag_.clear();
			flag_ = false;
			//EMOSKIT_MEMORY_FENCE;
		}
	}
}

#endif // 

