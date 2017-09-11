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

#ifndef _CIRCBUF_H_
#define _CIRCBUF_H_

#include <iostream>
#include <atomic>
#include <thread>
#include <semaphore.h>
#include <assert.h>
#include "mm.h"
#include "util.h"
#include "logger.h"
#include "wait_strategy.h"

namespace emoskit {

	namespace common {

		enum ProductionStrategy { SINGLE_THREAD, MUTIL_THREADS };
		enum WaitStrategy { YIELD_SLEEP_WAIT_STRATEGY, BLOCK_WAIT_STRATEGY, TIMEOUT_WAIT_STRATEGY, NOBLOCK_WAIT_STRATEGY };

		template <class T>
		class TaskWrapper {
		public:
			TaskWrapper() :isFree_(true), task_() {}
			T task_;
			volatile bool isFree_;
		};

		template <class T>
		class CircBuf
		{
		public:
			inline_ CircBuf(ull size, ProductionStrategy productionStrategy, WaitStrategy wait_strategy = YIELD_SLEEP_WAIT_STRATEGY, int timeout_seconds = 600);
			virtual ~CircBuf();

			CircBuf& operator=(const CircBuf&) = delete;
			CircBuf(const CircBuf&) = delete;

			inline_ llong take();
			inline_ llong poll();
			inline_ void publish(llong seq);
			inline_ llong get_locality();
			inline_ int get_move(T& ob);
			inline_ llong get_locality_with_timeout(int timeout_seconds);
			inline_ int get_move_with_timeout(T& ob, int timeout_seconds);
			inline_ void consumer_done(llong seq);
			inline_ bool is_empty();
			T& operator[](llong idx);

		private:
			inline_ llong single_producer_next(bool is_block);
			inline_ llong mutil_producer_next(bool is_block);
			inline_ llong get_in();
			inline_ llong get_cache_out() { return cache_out_; EMOSKIT_MEMORY_FENCE_ACQUIRE; }
			inline_ void update_cache_out() { cache_out_ = out_.load(std::memory_order_relaxed); }
			inline_ void update_cache_in();
			inline_ bool is_free(const llong &seq);

		private:
			TaskWrapper<T>* buffer_;
			ull size_;
			ull mask_;
			const ProductionStrategy productionStrategy_;

			wait_strategy<CircBuf<T>>* waitStrategy_;
			wait_strategy<CircBuf<T>>* timeoutWaitStrategy_;

			ull p11, p12, p13, p14, p15, p16, p17;
			std::atomic<llong> in_;
			ull p21, p22, p23, p24, p25, p26, p27;
			std::atomic<llong> out_;
			ull p31, p32, p33, p34, p35, p36, p37;
			//for single producer
			volatile llong single_in_;
			ull p41, p42, p43, p44, p45, p46, p47;
			volatile llong cache_out_;
			ull p51, p52, p53, p54, p55, p56, p57;
			volatile llong cache_in_;
			ull p61, p62, p63, p64, p65, p66, p67;

			friend class yield_sleep_wait_strategy<CircBuf<T>>;
			friend class block_wait_strategy<CircBuf<T>>;
			friend class timeout_wait_strategy<CircBuf<T>>;
			friend class noblock_wait_strategy<CircBuf<T>>;
		};

		template<class T> inline_
			CircBuf<T>::CircBuf(ull size, ProductionStrategy productionStrategy, WaitStrategy wait_strategy, int timeout_seconds) : productionStrategy_(productionStrategy)
		{
			if (!util::is_power_of_2(size))
				size = util::roundup_pow_of_two(size);

			if (0ull == size) {
				emoskit_log(WARN_LOG, "CircBuf size is 0, set to 1024 by default.");
				size = 1024;
			}
			else if (size < 2) {
				emoskit_log(WARN_LOG, "CircBuf size is less than 2, set to 2 by default.");
				size = 2;
			}

			this->size_ = size;
			this->mask_ = this->size_ - 1;
			this->in_ = -1;
			this->out_ = -1;
			this->single_in_ = -1;
			this->cache_out_ = -1;
			this->cache_in_ = -1;

			if(wait_strategy == YIELD_SLEEP_WAIT_STRATEGY)
				this->waitStrategy_ = new yield_sleep_wait_strategy<CircBuf<T>>();
			else if (wait_strategy == BLOCK_WAIT_STRATEGY)
				this->waitStrategy_ = new block_wait_strategy<CircBuf<T>>();
			else if(wait_strategy == TIMEOUT_WAIT_STRATEGY)
				this->waitStrategy_ = new timeout_wait_strategy<CircBuf<T>>(timeout_seconds);
			else if(wait_strategy == NOBLOCK_WAIT_STRATEGY)
				this->waitStrategy_ = new noblock_wait_strategy<CircBuf<T>>();
			else {
				emoskit_log(ERROR_LOG, "unknow wait strategy, exiting.");
				exit(1);
			}
			this->timeoutWaitStrategy_ = new timeout_wait_strategy<CircBuf<T>>(10);

			/*buffer_ = (TaskWrapper<T>*)mm::mm_malloc(this->size_ * sizeof(TaskWrapper<T>));
			for (int i = 0; i < this->size_; ++i) {
				UTIL::Construct(&buffer_[i]);
				buffer_[i].isFree_ = true;
			}*/
			buffer_ = new TaskWrapper<T>[this->size_];
		}

		template<class T> inline_
			CircBuf<T>::~CircBuf()
		{
			if (buffer_ != nullptr) {
				/*for (int i = 0; i < this->size_; ++i) {
					UTIL::Destroy(&buffer_[i]);
				}
				mm::mm_free(buffer_);*/
				delete[] buffer_;
			}

			if (waitStrategy_ != nullptr)
				delete waitStrategy_;

			if (timeoutWaitStrategy_ != nullptr)
				delete timeoutWaitStrategy_;
		}

		template<class T> inline_ llong
			CircBuf<T>::take()
		{
			if (productionStrategy_ == ProductionStrategy::SINGLE_THREAD)
				return single_producer_next(true);
			return mutil_producer_next(true);
		}

		template<class T> inline_ llong
			CircBuf<T>::poll()
		{
			if (productionStrategy_ == ProductionStrategy::SINGLE_THREAD)
				return single_producer_next(false);
			return mutil_producer_next(false);
		}

		template<class T> inline_ llong
			CircBuf<T>::mutil_producer_next(bool is_block)
		{
			llong current, next, rollback_round;
			do {
				current = in_.load(std::memory_order_relaxed);
				next = current + 1;
				rollback_round = next - size_;
				//if (rollback_round > out.load(std::memory_order_acquire) || !is_free(next)) {
				if (rollback_round > get_cache_out() || !is_free(next)) {
					update_cache_out();
					if (is_block)
						std::this_thread::yield();
					else return -1;
				}
				else if (in_.compare_exchange_weak(current, next, std::memory_order_seq_cst, std::memory_order_seq_cst)) {
					break;
				}
			} while (true);

			assert(buffer_[next & mask_].isFree_ == true);
			return next;
		}

		template<class T> inline_ llong
			CircBuf<T>::single_producer_next(bool is_block)
		{
			llong current, next, rollback_round;
			current = single_in_;
			next = current + 1;
			rollback_round = next - size_;

			//if (rollback_round <= get_cache_out() && is_free(next)) {
			//	single_in_ = next;
			//	return next;
			//}

			//while (rollback_round > out.load(std::memory_order_acquire) || !is_free(next)) {
			while (rollback_round > get_cache_out() || !is_free(next)) {
				update_cache_out();
				//waitpolicy	
				if (is_block)
					std::this_thread::yield();
				else return -1;
			}

			//update_cache_out();
			single_in_ = next;

			return next;
		}

		template<class T> inline_ llong
			CircBuf<T>::get_in()
		{
			return cache_in_;
			EMOSKIT_MEMORY_FENCE_ACQUIRE;
		}

		template<class T> inline_ void
			CircBuf<T>::update_cache_in()
		{
			if (productionStrategy_ == ProductionStrategy::SINGLE_THREAD)
				cache_in_ = single_in_;

			else cache_in_ = in_.load(std::memory_order_relaxed);
		}

		template<class T> inline_ bool
			CircBuf<T>::is_free(const llong &seq)
		{
			return this->buffer_[seq & this->mask_].isFree_;
		}

		template<class T> inline_ void
			CircBuf<T>::publish(llong seq)
		{
			EMOSKIT_MEMORY_FENCE_RELEASE;
			assert(buffer_[seq & mask_].isFree_ == true);
			buffer_[seq & mask_].isFree_ = false;
			waitStrategy_->signal_when_blocking();
		}

		template<class T> inline_ void
			CircBuf<T>::consumer_done(llong seq)
		{
			EMOSKIT_MEMORY_FENCE;
			assert(buffer_[seq & mask_].isFree_ == false);
			buffer_[seq & mask_].isFree_ = true;
		}

		template<class T> inline_ bool
			CircBuf<T>::is_empty()
		{
			if (productionStrategy_ == ProductionStrategy::MUTIL_THREADS)
				return in_ == out_;
			return single_in_ == out_;
		}

		template<class T> inline_ llong
			CircBuf<T>::get_locality()
		{
			return waitStrategy_->wait_for(*this);
		}

		template<class T> inline_ int
			CircBuf<T>::get_move(T & ob)
		{
			llong seq;
			if ((seq = get_locality()) == -1)
				return -1;

			ob = std::move(buffer_[seq & mask_].task_);
			EMOSKIT_MEMORY_FENCE_RELEASE;
			consumer_done(seq);
			return 0;
		}

		template<class T> inline_ llong
			CircBuf<T>::get_locality_with_timeout(int timeout_seconds)
		{
			return timeoutWaitStrategy_->wait_for(*this, timeout_seconds);
		}

		template<class T> inline_ int
			CircBuf<T>::get_move_with_timeout(T & ob, int timeout_seconds)
		{
			llong seq;
			if ((seq = get_locality_with_timeout(timeout_seconds)) == -1)
				return -1;

			ob = std::move(buffer_[seq & mask_].task_);
			EMOSKIT_MEMORY_FENCE_RELEASE;
			consumer_done(seq);
			return 0;
		}

		template<class T> inline_ T &
			CircBuf<T>::operator[](llong idx)
		{
			return buffer_[idx & mask_].task_;
		}

	}
}
#endif // ! _CIRCBUF_H_

