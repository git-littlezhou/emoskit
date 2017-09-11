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

#ifndef _MSG_QUEUE_INTERFACE_
#define _MSG_QUEUE_INTERFACE_

#include "notifier.h"

namespace emoskit {
	
	namespace common {
	
		template<class T>
		class MsgQueueInterface
		{
		public:
			MsgQueueInterface() {};
			~MsgQueueInterface() {};
			//virtual inline_ int Put(const T& data) = 0;
			//virtual inline_ int Get(T& data) = 0;
			//inline_ int Put(const T& data) {};
			//inline_ int Get(T& data) {};
			inline_ void set_flag_and_notify();
			inline_ void unset_flag_and_clear_notify();
			int get_event_fd() { return notifier.get_event_fd(); }

		private:
			Notifier notifier;
		};

		template<class T> inline_ void
		MsgQueueInterface<T>::set_flag_and_notify()
		{
			notifier.set_flag_and_notify();
		}

		template<class T> inline_ void
		MsgQueueInterface<T>::unset_flag_and_clear_notify()
		{
			notifier.unset_flag_and_clear_notify();
		}

	}
}
#endif // !_MSG_QUEUE_INTERFACE_
