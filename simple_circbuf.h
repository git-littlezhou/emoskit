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

#ifndef _LF_OROW_CIRCBUF_H_
#define _LF_OROW_CIRCBUF_H_

#include "util.h"

namespace emoskit {

	namespace common {

		template<class T>
		class SimpleCircBuf {

		public:
			SimpleCircBuf(ull buffer_size = 1024);
			void ReInit(ull buffer_size);
			int Put(const T& data);
			int Get(T&);

		private:
			void set_buffer_size(ull buffer_size);

		private:
			T* buffer_;
			ull buffer_size_;
			ull mask_;
			ull p11, p12, p13, p14, p15, p16, p17;
			volatile ull in_;
			ull p21, p22, p23, p24, p25, p26, p27;
			volatile ull out_;
			ull p31, p32, p33, p34, p35, p36, p37;

		};

		template<class T>
		SimpleCircBuf<T>::SimpleCircBuf(ull buffer_size)
		{
			buffer_ = nullptr;
			ReInit(buffer_size);
		}

		template<class T> inline_ void 
		SimpleCircBuf<T>::ReInit(ull buffer_size)
		{
			set_buffer_size(buffer_size);
			mask_ = buffer_size_ - 1;
			in_ = out_ = 0;

			buffer_ = (T*)mm::mm_realloc(buffer_, buffer_size_ * sizeof(T));
		}

		template<class T> inline_ void
		SimpleCircBuf<T>::set_buffer_size(ull buffer_size)
		{
			if (!util::is_power_of_2(buffer_size))
				buffer_size = util::roundup_pow_of_two(buffer_size);

			if (0ull == buffer_size) {
				emoskit_log(WARN_LOG, "CircBuf size is 0, set to 1024 by default.");
				buffer_size = 1024;
			}
			else if (buffer_size < 2) {
				emoskit_log(WARN_LOG, "CircBuf size is less than 2, set to 2 by default.");
				buffer_size = 2;
			}

			buffer_size_ = buffer_size;
		}

		template<class T> int
		SimpleCircBuf<T>::Get(T& data)
		{
			if (in_ == out_)
				return -1;

			/*
			 * Ensure that we sample the in_ index before we
			 * start removing bytes from the circbuf.
			 */
			EMOSKIT_MEMORY_FENCE_ACQUIRE;

			data = std::move(buffer_[out_ & mask_]);

			/*
			 * Ensure that we remove the bytes from the circbuf before
			 * we update the out_ index.
			 */
			EMOSKIT_MEMORY_FENCE_RELEASE;

			++out_;

			return 0;
		}

		template<class T> int 
		SimpleCircBuf<T>::Put(const T& data)
		{
			if ((in_ - out_) == buffer_size_)
				return -1;

			/*
			* Ensure that we sample the out_ index before we
			* start putting bytes into the circbuf.
			*/
			EMOSKIT_MEMORY_FENCE_ACQUIRE;

			buffer_[in_ & mask_] = data;

			/*
			* Ensure that we add the bytes to the circbuf before
			* we update the in_ index.
			*/
			EMOSKIT_MEMORY_FENCE_RELEASE;

			++in_;

			return 0;
		}

	}
}
#endif // !_LF_OROW_CIRCBUF_H_

