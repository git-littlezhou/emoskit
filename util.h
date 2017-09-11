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

#ifndef  _UTIL_H_
#define  _UTIL_H_

#include <time.h>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <netdb.h> 
#include <errno.h>
#include <string.h>
#include <string>
#include <new>

#include "logger.h"

#ifndef EMOSKIT_MEMORY_FENCE
/* g++ enerates mfence for memory_order_seq_cst, but nothing for others
* Because, on strongly-ordered systems (x86, SPARC TSO, IBM mainframe), release-acquire ordering is automatic for the majority of operations.
* No additional CPU instructions are issued for this synchronization mode, only certain compiler optimizations are affected (e.g. the compiler
* is prohibited from moving non-atomic stores past the atomic store-release or perform non-atomic loads earlier than the atomic load-acquire).
* On weakly-ordered systems (ARM, Itanium, PowerPC), special CPU load or memory fence instructions have to be used.
* see http://en.cppreference.com/w/cpp/atomic/memory_order
* (x86 only do "Stores reordered after loads"
* so if we want to prevent reordering by x86 CPU during runtime, we can only use mfence or lock instruction,
* ACQUIRE or RELEASE can not avoid "Stores reordered after loads"
* In other cases, we only need to prevent compiler from reordering instructions during compile time in x86.
* see https://en.wikipedia.org/wiki/Memory_ordering#cite_note-vtune-sfence-10,)
* atomic_signal_fence is equivalent to std::atomic_thread_fence, except no CPU instructions for memory ordering are issued.
* Only reordering of the instructions by the compiler is suppressed as order instructs
*/
#if (__i386 || __i386__) || (__amd64 || __amd64__ || __x86_64 || __x86_64__)
#define EMOSKIT_MEMORY_FENCE					std::atomic_thread_fence (std::memory_order_seq_cst)
#define EMOSKIT_MEMORY_FENCE_ACQUIRE			std::atomic_signal_fence (std::memory_order_acquire)
#define EMOSKIT_MEMORY_FENCE_RELEASE			std::atomic_signal_fence (std::memory_order_release)
//#define EMOSKIT_MEMORY_FENCE         __asm__ __volatile__ ("lock; addl $0,0(%%esp)" : : : "memory")	// for x86-32
//#define EMOSKIT_MEMORY_FENCE         __asm__ __volatile__ ("mfence"				   : : : "memory")  // for x86-64
//#define EMOSKIT_MEMORY_FENCE_ACQUIRE __asm__ __volatile__ (""					   : : : "memory")
//#define EMOSKIT_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("")
//#define EMOSKIT_MEMORY_FENCE_ACQUIRE __asm__ __volatile__ ("lfence"				   : : : "memory")
//#define EMOSKIT_MEMORY_FENCE_RELEASE __asm__ __volatile__ ("sfence"                 : : : "memory")
//#elif __powerpc__ || __ppc__ || __powerpc64__ || __ppc64__
//#define EMOSKIT_MEMORY_FENCE         __asm__ __volatile__ ("sync"				   : : : "memory")
#else
#define EMOSKIT_MEMORY_FENCE					std::atomic_thread_fence (std::memory_order_seq_cst)
#define EMOSKIT_MEMORY_FENCE_ACQUIRE			std::atomic_thread_fence (std::memory_order_acquire)
#define EMOSKIT_MEMORY_FENCE_RELEASE			std::atomic_thread_fence (std::memory_order_release)
#endif
#endif

#ifndef EMOSKIT_MEMORY_FENCE_ACQUIRE
#define EMOSKIT_MEMORY_FENCE_ACQUIRE		EMOSKIT_MEMORY_FENCE
#endif // !EMOSKIT_MEMORY_FENCE_ACQUIRE

#ifndef EMOSKIT_MEMORY_FENCE_RELEASE
#define EMOSKIT_MEMORY_FENCE_RELEASE		EMOSKIT_MEMORY_FENCE
#endif // ! EMOSKIT_MEMORY_FENCE_RELEASE1

typedef double ev_tstamp;
typedef unsigned long long ull;
typedef long long llong;

#define svr_inline inline

#define expect_true(cond) __builtin_expect(!!(cond), 1)
#define expect_false(cond) __builtin_expect(!!(cond), 0)

#define attr_cold	__attribute__ ((__cold__))
#define attr_hot	__attribute__ ((__hot__))

#define noinline_ __attribute__ ((noinline))
#define inline_	inline

#define util_fd_t int

namespace emoskit {

	namespace util{
		svr_inline ev_tstamp
		get_time()
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			return ts.tv_sec + ts.tv_nsec * 1e-9;
		}

		svr_inline bool
		is_power_of_2(ull x)
		{
			return (x != 0 && ((x & (x - 1)) == 0));
		}

		svr_inline int
		find_last_set_bit(ull x)
		{
			int cnt;
			if (expect_true(0 != x)) {
				x >>= 1;
				for (cnt = 0; x != 0; ++cnt) {
					x >>= 1;
				}
			}
			else {
				cnt = -1;
			}
			return cnt + 1;
		}

		/*
		* The result of a shift operation is undefined if the right parameter is negative
		* or if the right parameter is greater than or equal to the number of bits in the (promoted) shift-expression
		*/
		svr_inline ull
		roundup_pow_of_two(ull x)
		{
			int cnt = find_last_set_bit(x - 1);
			if (cnt < sizeof(x) * 8)
				return 1ull << cnt;

			return 0ull;
		}

		svr_inline ull
		rounddown_pow_of_two(ull x)
		{
			if (0ull == x)
				return 0ull;

			return 1ull << (find_last_set_bit(x) - 1);
		}

		svr_inline int 
		CreateEventFd()
		{
			int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

			if (event_fd < 0 && errno == EINVAL) {
				event_fd = eventfd(0, 0);
			}

			return event_fd;
		}

		svr_inline int
		MakeSocketNonblocking(int fd)
		{
			int flags;
			if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
				emoskit_log(WARN_LOG, "fcntl(%d, F_GETFL) error.", fd);
				return -1;
			}
			if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
				emoskit_log(WARN_LOG, "fcntl(%d, F_SETFL) error.", fd);
				return -1;
			}
			return 0;
		}

		svr_inline int
		MakeSocketCloseonexec(int fd)
		{
			int flags;
			if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
				emoskit_log(WARN_LOG, "fcntl(%d, F_GETFL) error.", fd);
				return -1;
			}
			if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
				emoskit_log(WARN_LOG, "fcntl(%d, F_SETFL) error.", fd);
				return -1;
			}
			return 0;
		}

		template<class T1, class T2> svr_inline void
		Construct(T1 * p, const T2& value) {
			new (p) T1(value);
		}

		template<class T> svr_inline void
		Construct(T * p) {
			new (p) T();
		}

		template<class T> svr_inline void
		Destroy(T* pointer) {
			pointer->~T();
		}

		int noinline_ CreateSocketAndListen(const char * host, const char * serv, socklen_t * addrlen);
		int noinline_ CreateSocketAndConnect(const char* ip, unsigned port, int timeout_ms, const char* bind_ip = nullptr, int bind_port = -1);
		bool noinline_ ReadFile(const char* file_path, std::string* contents);
	}
}
#endif // ! _UTIL_H_
