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

#ifndef  _MM_H_
#define  _MM_H_

#include <stdlib.h>
#include "logger.h"

// can remove
#define expect_true(cond) __builtin_expect(!!(cond), 1)
#define expect_false(cond) __builtin_expect(!!(cond), 0)

#define attr_cold	__attribute__ ((__cold__))
#define attr_hot	__attribute__ ((__hot__))
//end can remove

#define inline_speed static inline

namespace emoskit {
	namespace mm {

		static void* (*allocator)(void*, size_t) = realloc;

		static void attr_cold
			set_allocator(void* (*alloc)(void*, size_t)) {
			allocator = alloc;
		}

		inline_speed void*
			mm_realloc(void* ptr, size_t size) {
			ptr = allocator(ptr, size);
			if (expect_false(!ptr && size)) {
				emoskit_log(ERROR_LOG, "emoskit memory allocation failed.");
				return nullptr;
			}
			return ptr;
		}

		inline_speed void*
			mm_malloc(size_t size) {
			return mm_realloc(0, size);
		}

		inline_speed void
			mm_free(void* ptr) {
			mm_realloc(ptr, 0);
		}
	}
}
#endif
