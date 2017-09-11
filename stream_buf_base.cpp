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

#include "stream_buf_base.h"
#include "util.h"
#include "mm.h"

namespace emoskit {
	
	namespace common {
		
		StreamBufBase::StreamBufBase(size_t buf_size) : kBufSize(util::roundup_pow_of_two(buf_size))
		{

			char* rb = (char*)mm::mm_malloc(kBufSize);
			char* wb = (char*)mm::mm_malloc(kBufSize);

			/*
			 * void setg( char_type* gbeg, char_type* gcurr, char_type* gend );
			 * Sets the values of the pointers defining the get area. Specifically, after the call eback() == gbeg, gptr() == gcurr, egptr() == gend
			 */
			setg(rb, rb, rb);
			/*
			 * void setp( char_type* pbeg, char_type* pend );
			 * Sets the values of the pointers defining the put area. Specifically, after the call pbase() == pbeg, pptr() == pbeg, epptr() == pend
			 */
			setp(wb, wb + kBufSize);
		}

		StreamBufBase::~StreamBufBase()
		{
			mm::mm_free(eback());
			mm::mm_free(pbase());
		}

		/* Synchronizes the controlled character sequence (the buffers) with the associated character sequence. */
		int 
		StreamBufBase::sync(){
			size_t need_write_len = pptr() - pbase();
			size_t already_write_len = 0;
			/* write all data */
			while (already_write_len < need_write_len) {
				int ret = BufWriteToTheControlledSequence(pbase() + already_write_len, need_write_len - already_write_len, 0);
				if (expect_true(ret > 0))
					already_write_len += ret;
				else return -1;
				
			}
			/* reset write buf */
			setp(pbase(), pbase() + kBufSize);

			return 0;
		}

		std::streambuf::int_type
		StreamBufBase::overflow(int_type ch) {
			if (-1 == sync())
				return traits_type::eof();
			else {
				if (expect_true(!traits_type::eq_int_type(ch, traits_type::eof()))) {
					/* int_type sputc( char_type ch );
					 * Return value
					 * The written character, converted to int_type with Traits::to_int_type(ch) on success.
					 * Traits::eof() (as returned by overflow()) on failure.
					 */
					ch = sputc(traits_type::to_char_type(ch));
				}

				return traits_type::not_eof(ch);
			}
		}

		std::streambuf::int_type
		StreamBufBase::underflow() {
			int ret = BufReadFromTheControlledSequence(eback(), kBufSize, 0);
			if (expect_true(ret > 0)) {
				setg(eback(), eback(), eback() + ret);
				return traits_type::to_int_type(*gptr());
			}
			else {
				return traits_type::eof();
			}
		}
	}
}