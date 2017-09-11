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

#ifndef _GCC_ATOMIC_H_
#define _GCC_ATOMIC_H_

#include <stdint.h>

/*atomic operator, see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html for detail*/

#define EMOSKIT_ATOMIC_RELAXED __ATOMIC_RELAXED
#define EMOSKIT_ATOMIC_CONSUME __ATOMIC_CONSUME
#define EMOSKIT_ATOMIC_ACQUIRE __ATOMIC_ACQUIRE
#define EMOSKIT_ATOMIC_RELEASE __ATOMIC_RELEASE
#define EMOSKIT_ATOMIC_ACQ_REL __ATOMIC_ACQ_REL
#define EMOSKIT_ATOMIC_SEQ_CST __ATOMIC_SEQ_CST

#define emoskit_atomic_load(ptr, memory_order) (__atomic_load_n((ptr), (memory_order)))
#define emoskit_atomic_acquire_load(ptr) (__atomic_load_n((ptr), __ATOMIC_ACQUIRE))
#define emoskit_atomic_relaxed_load(ptr) (__atomic_load_n((ptr), __ATOMIC_RELAXED))

#define emoskit_atomic_store(ptr, value, memory_order) \
	(__atomic_store_n((ptr), (intptr_t)(value), (memory_order)))
#define emoskit_atomic_seq_store(ptr, value) \
	(__atomic_store_n((ptr), (intptr_t)(value), __ATOMIC_SEQ_CST))
#define emoskit_atomic_release_store(ptr, value) \
	(__atomic_store_n((ptr), (intptr_t)(value), __ATOMIC_RELEASE))
#define emoskit_atomic_relaxed_store(ptr, value) \
	(__atomic_store_n((ptr), (intptr_t)(value), __ATOMIC_RELAXED))
	
#define emoskit_fetch_add(ptr, delta, memory_order) \
	(__atomic_fetch_add((ptr), (intptr_t)(delta), (memory_order)))
#define emoskit_fetch_seq_add(ptr, delta) \
	(__atomic_fetch_add((ptr), (intptr_t)(delta), __ATOMIC_SEQ_CST))	
#define emoskit_fetch_release_add(ptr, delta) \
	(__atomic_fetch_add((ptr), (intptr_t)(delta), __ATOMIC_RELEASE))
#define emoskit_fetch_relaxed_add(ptr, delta) \
	(__atomic_fetch_add((ptr), (intptr_t)(delta), __ATOMIC_RELAXED))
	
#define emoskit_fetch_sub(ptr, delta, memory_order) \
	(__atomic_fetch_sub((ptr), (intptr_t)(delta), (memory_order)))
#define emoskit_fetch_seq_sub(ptr, delta) \
	(__atomic_fetch_sub((ptr), (intptr_t)(delta), __ATOMIC_SEQ_CST))	
#define emoskit_fetch_release_sub(ptr, delta) \
	(__atomic_fetch_sub((ptr), (intptr_t)(delta), __ATOMIC_RELEASE))
#define emoskit_fetch_relaxed_sub(ptr, delta) \
	(__atomic_fetch_sub((ptr), (intptr_t)(delta), __ATOMIC_RELAXED))
	
#define emoskit_cas_weak(ptr, expected_ptr, desired, succ_memory_order, fail_memory_order) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 1, (succ_memory_order), (fail_memory_order)))
#define emoskit_relaxed_cas_weak(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
#define emoskit_release_cas_weak(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
#define emoskit_acquire_cas_weak(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 1, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))

#define emoskit_cas_strong(ptr, expected_ptr, desired, succ_memory_order, fail_memory_order) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 0, (succ_memory_order), (fail_memory_order)))
#define emoskit_relaxed_cas_strong(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
#define emoskit_release_cas_strong(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
#define emoskit_acquire_cas_strong(ptr, expected_ptr, desired) \
	(__atomic_compare_exchange_n((ptr), (expected_ptr), (desired), 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))

#define emoskit_exchange(ptr, value, memory_order) \
	(__atomic_exchange_n((ptr), (value), (memory_order)))
#define emoskit_release_exchange(ptr, value) \
	(__atomic_exchange_n((ptr), (value), __ATOMIC_RELEASE))
#define emoskit_seq_exchange(ptr, value) \
	(__atomic_exchange_n((ptr), (value), __ATOMIC_SEQ_CST))

#endif // !_GCC_ATOMIC_H_