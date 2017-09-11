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

#ifndef  _SIMPLE_MSG_QUEUE_H_
#define	_SIMPLE_MSG_QUEUE_H_

#include <memory>
#include "simple_circbuf.h"
#include "util.h"
#include "msg_queue_interface.h"

namespace emoskit {

	namespace common {
		
		template<class T>
		class SimpleMsgQueue : public MsgQueueInterface<T> {
		public:
			SimpleMsgQueue(ull queue_size = 1024);
			void set_queue_size(ull queue_size);
			int Put(const T& data);
			int Get(T& data);

		private:
			SimpleCircBuf<T> queue_;
		};

		template<class T> inline_
		SimpleMsgQueue<T>::SimpleMsgQueue(ull queue_size) : queue_(queue_size)
		{
		}

		template<class T> inline_ void 
		SimpleMsgQueue<T>::set_queue_size(ull queue_size)
		{
			queue_.ReInit(queue_size);
		}

		template<class T> inline_ int
		SimpleMsgQueue<T>::Put(const T & data)
		{
			return queue_.Put(data);
		}

		template<class T> inline_ int 
		SimpleMsgQueue<T>::Get(T & data)
		{
			return queue_.Get(data);
		}

	}
}
#endif

