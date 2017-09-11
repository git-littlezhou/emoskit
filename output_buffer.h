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

#ifndef  _OUTPUT_BUFFER_H_
#define _OUTPUT_BUFFER_H_

#include <stdint.h>
#include "mm.h"
#include "util.h"

namespace emoskit {

	namespace common {
		
		#define DEFAULT_BUFFER_SIZE 128
		#define MAX_CONSUME_OFFSET 1024

		class OutputBuffer
		{
		public:
			OutputBuffer(uint32_t buffer_size = DEFAULT_BUFFER_SIZE);
			~OutputBuffer();
			void Resize(uint32_t buffer_size);
			void AdjustBuffer();
			void Consume(uint32_t size);
			char* GetBufferContents();
			char* get_empty_buffer(uint32_t size);
			uint32_t surplus_size() { return buffer_size_ - used_buffer_size_; }
			uint32_t BufferContentsSize() { return used_buffer_size_ - consume_offset_; }
		private:
			char* buffer_;
			uint32_t buffer_size_;
			uint32_t used_buffer_size_;
			uint32_t consume_offset_;	
		};

		inline_ 
		OutputBuffer::OutputBuffer(uint32_t buffer_size)
		{
			buffer_size_ = buffer_size;
			used_buffer_size_ = 0;
			consume_offset_ = 0;
			buffer_ = (char*)mm::mm_malloc(buffer_size_);
		}

		inline_ 
		OutputBuffer::~OutputBuffer()
		{
			mm::mm_free(buffer_);
		}

		inline_ void 
		OutputBuffer::Resize(uint32_t buffer_size)
		{
			buffer_size_ = util::roundup_pow_of_two(buffer_size);
			buffer_ = (char*)mm::mm_realloc(buffer_, buffer_size_);
		}

		inline_ void 
		OutputBuffer::AdjustBuffer()
		{
			if (used_buffer_size_ == consume_offset_) {
				used_buffer_size_ = consume_offset_ = 0;
			}
			else if (consume_offset_ > MAX_CONSUME_OFFSET) {
				emoskit_log(INFO_LOG, "Meet max consume offset, move the buffer left.");
				if (used_buffer_size_ - consume_offset_ > consume_offset_) {
					memmove(buffer_, (buffer_ + consume_offset_), used_buffer_size_ - consume_offset_);
				}
				else {
					memcpy(buffer_, (buffer_ + consume_offset_), used_buffer_size_ - consume_offset_);
				}
				used_buffer_size_ -= consume_offset_;
				consume_offset_ = 0;
			}
		}

		inline_ void 
		OutputBuffer::Consume(uint32_t size)
		{
			consume_offset_ += size;
		}

		inline_ char * 
		OutputBuffer::GetBufferContents()
		{
			return buffer_ + consume_offset_;
		}

		inline_ char * 
		OutputBuffer::get_empty_buffer(uint32_t size)
		{
			if (size > surplus_size()) {
				Resize(buffer_size_ + (size - surplus_size()));
			}

			char* buffer_empty = buffer_ + used_buffer_size_;
			used_buffer_size_ += size;

			return buffer_empty;
		}
	}
}
#endif // ! _OUTPUT_BUFFER_H_

