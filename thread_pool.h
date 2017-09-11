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

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <functional>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include "logger.h"
#include "circbuf.h"

namespace emoskit {

	namespace common {
		template <class T>
		struct wrapper {
			wrapper() : isFinish_(false), task_() {}
			T task_;
			bool isFinish_;
		};

		template<class T>
		class ThreadPool
		{
		public:

			typedef long long llong;

			ThreadPool(int corePoolSize, int maxPoolSize, int keepAliveTime, ull circbufSize, ProductionStrategy productionStrategy = ProductionStrategy::MUTIL_THREADS);
			~ThreadPool();

			ThreadPool& operator=(const ThreadPool&) = delete;
			ThreadPool(const ThreadPool&) = delete;
			ThreadPool(ThreadPool&&) = delete;
			ThreadPool& operator = (ThreadPool&&) = delete;

			inline_ int getPoolSize() { return queue_.size(); }
			inline_ void shutdown();
			inline_ void start();

			inline_ void addTask(const T& task);
			inline_ void addTaskMove(T& task);

			inline_ T& getTaskObjectAndSeq(llong& seq);
			inline_ void publish_task(const llong& idx) { queue_[idx].isFinish_ = false; queue_.publish(idx); }

			inline_ void addIfUnderMaxPoolSize();

		private:
			void addThread(int i);
			void rmThread(int i);

		private:
			int							corePoolSize_;
			int							maxPoolSize_;
			int							keepAliveTime_;
			bool						isStop_;
			int							poolSize_;
			common::CircBuf<wrapper<T>>	queue_;
			std::vector<std::thread>	workers_;
			std::vector<bool>			isTerminate_;
			std::mutex					mlock_;
		};

		template<class T>
		ThreadPool<T>::ThreadPool(int corePoolSize, int maxPoolSize, int keepAliveTime, ull circbufSize, ProductionStrategy productionStrategy)
			: queue_(circbufSize, productionStrategy), workers_(), isTerminate_(), mlock_()
		{
			if (corePoolSize <= 0) {
				emoskit_log(WARN_LOG,
					"corePollSize is less than 1, set to 1/2 * hardware_concurrency(if valid) or 1 by default.");
				corePoolSize = std::thread::hardware_concurrency();
				corePoolSize = corePoolSize > 1 ? (corePoolSize / 1): 1;
			}

			if (maxPoolSize <= 0 || maxPoolSize < corePoolSize) {
				emoskit_log(WARN_LOG,
					"maxPoolSize must be equal or greater than corePollSize, set to corePollSize by default.");
				maxPoolSize = corePoolSize;
			}

			this->corePoolSize_ = corePoolSize;
			this->maxPoolSize_ = maxPoolSize;
			this->keepAliveTime_ = keepAliveTime;
			this->isStop_ = false;
			this->poolSize_ = 0;

		}

		template<class T> inline_
			ThreadPool<T>::~ThreadPool()
		{
			shutdown();
			while (poolSize_ > 0);
		}

		template<class T> inline_ void
			ThreadPool<T>::shutdown()
		{
			llong idx;
			std::lock_guard<std::mutex> lock(mlock_);
			if (isStop_)
				return;
			isStop_ = true;
			for (int i = 0; i < poolSize_; ++i) {
				idx = queue_.take();
				queue_[idx].isFinish_ = true;
				queue_.publish(idx);
			}
		}

		template<class T> inline_ void 
		ThreadPool<T>::start()
		{
			std::lock_guard<std::mutex> lock(mlock_);
			for (int i = 0; i < this->corePoolSize_; ++i) {
				addThread(i);
			}
		}

		template<class T> inline_ void
			ThreadPool<T>::addTask(const T & task)
		{
			llong index;
			while ((index = queue_.poll()) < 0) {
				addIfUnderMaxPoolSize();
				std::this_thread::yield();
			}
			queue_[index].task_ = task;
			queue_[index].isFinish_ = false;
			queue_.publish(index);
		}

		template<class T> inline_ void
			ThreadPool<T>::addTaskMove(T & task)
		{
			llong index;
			while ((index = queue_.poll()) < 0) {
				addIfUnderMaxPoolSize();
				std::this_thread::yield();
			}
			queue_[index].task_ = std::move(task);
			queue_[index].isFinish_ = false;
			queue_.publish(index);
		}

		template<class T> inline_ void
		common::ThreadPool<T>::addIfUnderMaxPoolSize()
		{
			std::lock_guard<std::mutex> lock(mlock_);
			if (poolSize_ < maxPoolSize_ && isStop_ == false) {
				emoskit_log(INFO_LOG, "adding new thread.");
				addThread(workers_.size() - 1);
			}
		}

		template<class T> inline_ T &
		ThreadPool<T>::getTaskObjectAndSeq(llong & seq)
		{
			llong index;
			while ((index = queue_.poll()) < 0) {
				addIfUnderMaxPoolSize();
				std::this_thread::yield();
			}
			seq = index;
			return queue_[seq].task_;
		}

		template<class T> inline_ void
		ThreadPool<T>::addThread(int i)
		{
			workers_.emplace_back(
				[this, i] {
				llong idx;
				for (;;) {
					// core pool threads handle
					if (i < this->corePoolSize_) {
						idx = this->queue_.get_locality();
						if (expect_false(idx < 0)) {
							continue;
						}
					}
					// non core pool threads handle
					else {
						idx = this->queue_.get_locality_with_timeout(this->keepAliveTime_);

						// if timeout, stop the threads
						if (idx < 0) {
							emoskit_log(INFO_LOG, "non core pool thread, stopping.");
							std::lock_guard<std::mutex> lock(this->mlock_);
							this->rmThread(i);
							return;
						}
					}

					if (expect_false(this->queue_[idx].isFinish_)) {
						std::lock_guard<std::mutex> lock(this->mlock_);
						this->rmThread(i);
						return;
					}

					this->queue_[idx].task_();
					this->queue_.consumer_done(idx);
				}
			}
			);
			workers_.back().detach();
			++poolSize_;
		}

		template<class T> inline_ void
		common::ThreadPool<T>::rmThread(int i)
		{
			workers_.erase(workers_.begin() + i);
			--poolSize_;
			emoskit_log(INFO_LOG, "Thread %d finishing.", i);
		}
	}
}
#endif // !_THREAD_POOL_H_

