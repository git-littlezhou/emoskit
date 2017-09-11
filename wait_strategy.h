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

#ifndef _WAIT_STRATEGY_H_
#define _WAIT_STRATEGY_H_

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "util.h"
#include "logger.h"

namespace emoskit {

	namespace common {

		template <class T>
		class wait_strategy
		{
		public:
			virtual ~wait_strategy() {}
			virtual llong wait_for(T&, int data = -1) = 0;
			virtual void signal_when_blocking() = 0;
		};

		template <class T>
		class yield_sleep_wait_strategy : public wait_strategy<T>
		{
		private:
			const int spin_tries = 200;
		public:
			virtual llong
				wait_for(T& circ_buf, int data)
			{
				int counter = spin_tries;

				llong current_out = circ_buf.out_.load(std::memory_order_relaxed);
				llong next = current_out + 1;

				while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
					//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
					circ_buf.update_cache_in();
					apply_wait_method(counter);
					current_out = circ_buf.out_.load(std::memory_order_relaxed);
					next = current_out + 1;
				}

				while (!circ_buf.out_.compare_exchange_weak(current_out, next, std::memory_order_release, std::memory_order_relaxed)) {
					next = current_out + 1;

					while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
						//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
						circ_buf.update_cache_in();
						apply_wait_method(counter);
						current_out = circ_buf.out_.load(std::memory_order_relaxed);
						next = current_out + 1;
					}
				}

				EMOSKIT_MEMORY_FENCE_ACQUIRE;
				//assert(circ_buf.buffer[next & circ_buf.mask].isFree == false);
				return next;
			}

			void
			apply_wait_method(int& counter)
			{
				if (counter > 100) {
					--counter;
				}
				else if(counter > 0) {
					--counter;
					std::this_thread::yield();
				}
				else {
					std::this_thread::sleep_for(std::chrono::nanoseconds(1));
					//std::this_thread::sleep_for(std::chrono::duration<long int, std::ratio<1,100000000000>>(1));
				}
			}

			virtual void
				signal_when_blocking() {}

			virtual ~yield_sleep_wait_strategy() {}
		};

		template <class T>
		class block_wait_strategy : public wait_strategy<T> {
		public:
			virtual llong
				wait_for(T& circ_buf, int data)
			{
				llong current_out = circ_buf.out_.load(std::memory_order_relaxed);
				llong next = current_out + 1;

				while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
					//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
					circ_buf.update_cache_in();
					apply_wait_method(circ_buf);
					current_out = circ_buf.out_.load(std::memory_order_relaxed);
					next = current_out + 1;
				}

				while (!circ_buf.out_.compare_exchange_weak(current_out, next, std::memory_order_release, std::memory_order_relaxed))
				{
					next = current_out + 1;
					while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
						//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
						circ_buf.update_cache_in();
						apply_wait_method(circ_buf);
						current_out = circ_buf.out_.load(std::memory_order_relaxed);
						next = current_out + 1;
					}
				}
				EMOSKIT_MEMORY_FENCE_ACQUIRE;
				return next;
			}

			void
				apply_wait_method(T& circ_buf)
			{
				if (circ_buf.is_empty()) {
					std::unique_lock <std::mutex> lck(mtx);
					while (circ_buf.is_empty()) {
						need_notify = true;
						cv.wait(lck);
					}
				}
			}

			virtual void
				signal_when_blocking()
			{
				if (need_notify.exchange(false)) {
					std::unique_lock <std::mutex> lck(mtx);
					cv.notify_all();
				}
			}

			virtual ~block_wait_strategy() {}
			block_wait_strategy() { need_notify = false; }
			//block_wait_strategy(ull mask) :wait_strategy<T>(mask) {}
		private:
			std::mutex mtx;
			std::condition_variable cv;
			std::atomic<bool> need_notify;
		};

		template <class T>
		class timeout_wait_strategy : public wait_strategy<T> {
		public:
			virtual llong
				//wait_for(T* buffer, std::atomic<llong>& out)
				wait_for(T& circ_buf, int timeout)
			{
				if (timeout == -1)
					timeout = timeout_seconds;

				int counter = spin_tries;
				ev_tstamp start_time = util::get_time();

				llong current_out = circ_buf.out_.load(std::memory_order_relaxed);
				llong next = current_out + 1;
				while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
					//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
					circ_buf.update_cache_in();
					apply_wait_method(circ_buf, counter);
					if (counter == 0 && (util::get_time() - start_time) >= timeout)
						return -1;
					current_out = circ_buf.out_.load(std::memory_order_relaxed);
					next = current_out + 1;
					//EMOSKIT_MEMORY_FENCE;
				}

				while (!circ_buf.out_.compare_exchange_weak(current_out, next, std::memory_order_release, std::memory_order_relaxed))
				{
					next = current_out + 1;
					while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
						//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
						circ_buf.update_cache_in();
						apply_wait_method(circ_buf, counter);
						if (counter == 0 && (util::get_time() - start_time) >= timeout)
							return -1;
						current_out = circ_buf.out_.load(std::memory_order_relaxed);
						next = current_out + 1;
						//EMOSKIT_MEMORY_FENCE;
					}
				}

				EMOSKIT_MEMORY_FENCE_ACQUIRE;
				return next;
			}

			void
			apply_wait_method(T& circ_buf, int& counter)
			{
				if (counter > 0) {
					--counter;
				}
				else {
					std::this_thread::sleep_for(std::chrono::nanoseconds(1));
				}
			}

			virtual void
			signal_when_blocking() {}

			timeout_wait_strategy(int timeout_seconds) { this->timeout_seconds = timeout_seconds; }
			virtual ~timeout_wait_strategy() {}
		private:
			int timeout_seconds;
			const int spin_tries = 200;
		};

		template <class T>
		class noblock_wait_strategy : public wait_strategy<T>{
		public:
			virtual llong
				wait_for(T& circ_buf, int data)
			{
				llong current_out = circ_buf.out_.load(std::memory_order_relaxed);
				llong next = current_out + 1;

				while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
					//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
					circ_buf.update_cache_in();
					if (circ_buf.is_empty())
						return -1;
					current_out = circ_buf.out_.load(std::memory_order_relaxed);
					next = current_out + 1;
				}

				while (!circ_buf.out_.compare_exchange_weak(current_out, next, std::memory_order_release, std::memory_order_relaxed)) {
					next = current_out + 1;

					while (next > circ_buf.get_in() || circ_buf.is_free(next)) {
						//while (next > circ_buf.get_in() || !circ_buf.is_available(next)) {
						circ_buf.update_cache_in();
						if (circ_buf.is_empty())
							return -1;
						current_out = circ_buf.out_.load(std::memory_order_relaxed);
						next = current_out + 1;
					}
				}

				EMOSKIT_MEMORY_FENCE_ACQUIRE;
				//assert(circ_buf.buffer[next & circ_buf.mask].isFree == false);
				return next;
			}

			void
				apply_wait_method(int& counter){}

			virtual void
				signal_when_blocking() {}

			virtual ~noblock_wait_strategy() {}
		};
		//class wait_strategy 
		//{
		////protected:
		////	const ull mask;
		//public:
		////	wait_strategy(ull mask) :mask(mask) {}
		//	virtual ~wait_strategy() {}
		//	virtual void wait_for(int&) = 0;
		//	virtual void signal_when_blocking() = 0;
		//};

		//
		//class yield_wait_strategy : public wait_strategy
		//{
		////private:
		////	const int spin_tries = 100;
		//public:
		//	virtual void 
		//	wait_for(int& counter)
		//	{
		//		//int counter = spin_tries;

		//		if (counter == 0) {
		//			std::this_thread::yield();
		//		}
		//		else {
		//			--counter;
		//		}
		//		return;
		//		/*llong current_out = out.load(std::memory_order_relaxed);
		//		llong next = current_out + 1;

		//		while (buffer[next & this->mask].isFree) {
		//			apply_wait_method(counter);
		//			current_out = out.load(std::memory_order_relaxed);
		//			next = current_out + 1;
		//		}

		//		while (!out.compare_exchange_weak(current_out, next, std::memory_order_relaxed, std::memory_order_relaxed))
		//		{
		//			next = current_out + 1;
		//			while (buffer[next & this->mask].isFree) {
		//				apply_wait_method(counter);
		//				current_out = out.load(std::memory_order_relaxed);
		//				next = current_out + 1;
		//			}
		//		}
		//		
		//		EMOSKIT_MEMORY_FENCE_ACQUIRE;*/
		//	}
		//	/*
		//	void 
		//	apply_wait_method(int& counter) 
		//	{
		//		if (counter == 0) {
		//			std::this_thread::yield();
		//		}
		//		else {
		//			--counter;
		//		}
		//	}*/

		//	virtual void 
		//	signal_when_blocking() {}

		//	virtual ~yield_wait_strategy() {}
		//	//yield_wait_strategy(ull mask) :wait_strategy<T>(mask) {}
		//};

		//class block_wait_strategy : public wait_strategy {
		//public:
		//	virtual void
		//	wait_for(int& counter) 
		//	{
		//		if (counter == 0) {
		//			std::unique_lock <std::mutex> lck(mtx);
		//			cv.wait(lck);
		//		}
		//		else {
		//			--counter;
		//		}
		//	}
		//	virtual void
		//	signal_when_blocking() 
		//	{
		//		std::unique_lock <std::mutex> lck(mtx);
		//		cv.notify_all();
		//	}

		//private:
		//	std::mutex mtx;
		//	std::condition_variable cv;
		//};
	}
}
#endif // ! _WAIT_STRATEGY_H_

