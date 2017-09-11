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

#ifndef  _MSG_QUEUE_H_
#define	_MSG_QUEUE_H_

#include "msg_queue_interface.h"
#include "circbuf.h"

namespace emoskit {
	
	namespace common {
	
		template<class T>
		class MsgQueue : public MsgQueueInterface<T> {
		public:
			MsgQueue(ull size = 1024);
			inline_ T& TakeObjectAndSeq(llong& seq);
			inline_ void Publish(const llong& seq);
			//inline_ void ConsumerDone(const llong& seq);
			inline_ int GetConsumerObject(T& data);
			//inline_ T& GetConsumerObjectAndseq(llong& seq);

		private:
			CircBuf<T> queue_;
		};

		template<class T>
		MsgQueue<T>::MsgQueue(ull size)
			:queue_(size, common::ProductionStrategy::MUTIL_THREADS, common::WaitStrategy::NOBLOCK_WAIT_STRATEGY)
		{
		}

		template<class T> inline_ T &
		MsgQueue<T>::TakeObjectAndSeq(llong & seq)
		{
			seq = queue_.take();
			return queue_[seq];
		}

		template<class T> inline_ void 
		MsgQueue<T>::Publish(const llong & seq)
		{
			queue_.publish(seq);
		}

		//template<class T> inline_ void 
		//MsgQueue<T>::ConsumerDone(const llong & seq)
		//{
		//	queue_.consumer_done(seq);
		//}

		template<class T> inline_ int 
		MsgQueue<T>::GetConsumerObject(T & data)
		{
			return queue_.get_move(data);
		}

		//template<class T> inline_ T & 
		//MsgQueue<T>::GetConsumerObjectAndseq(llong & seq)
		//{
		//	seq = queue_.get_locality();
		//	if (seq >= 0)
		//		return queue_[seq];
		//}
	}
}
#endif
