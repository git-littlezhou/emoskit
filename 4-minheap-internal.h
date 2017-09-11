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

#ifndef _4_MINHEAP_INTERNAL_H_
#define _4_MINHEAP_INTERNAL_H_

#include "event.h"
#include "array.h"

typedef double ev_tstamp;

#define inline_ inline

namespace emoskit {

	namespace common {

		typedef struct {
			ev_watcher_time* wt;
			ev_tstamp cache_at;
			#define HEAP_AT(mht) mht.cache_at
			#define HEAP_CACHE_AT(mht) mht.cache_at = mht.wt->at
			#define HEAP_WATCHER_ACTIVE(mht) mht.wt->active
			#define HEAP_WT(mht) mht.wt
		}min_heap_t;

		#define DHEAD 4
		#define PARENT(p) ((p) - 1) / DHEAD
		#define LCHILD(p) DHEAD * p + 1
		#define IS_ROOT(p) (!(p))

		class quad_minheap {
		public:
			quad_minheap() :size(0) {};
			inline_ void				shift_up(int pos);
			inline_ void				shift_down(int pos);
			inline_ void				adjust(int pos);
			inline_ int					push(ev_watcher_time* wt);
			inline_ ev_watcher_time*	pop();
			inline_ min_heap_t&			top();
			inline_ int					remove(int pos);
			inline_ bool				isempty();
			inline_ min_heap_t&			get(int idx);

		private:
			array<min_heap_t> heap;
			int size;
		};

		inline_ void
			common::quad_minheap::shift_up(int pos)
		{
			min_heap_t bak = heap[pos];
			int parent = PARENT(pos);

			while (!IS_ROOT(pos)) {
				if (HEAP_AT(heap[parent]) < HEAP_AT(bak))
					break;
				heap[pos] = heap[parent];
				HEAP_WATCHER_ACTIVE(heap[pos]) = pos + 1;
				pos = parent;
				parent = PARENT(pos);
			}
			heap[pos] = bak;
			HEAP_WATCHER_ACTIVE(heap[pos]) = pos + 1;
		}

		inline_ void
			common::quad_minheap::shift_down(int pos)
		{
			min_heap_t bak = heap[pos];
			int lchild = LCHILD(pos);
			int min;

			for (;;) {
				/* find the minimun child */
				/* if has four childs */
				if (lchild + DHEAD - 1 < size) {
					min = lchild;
					if (HEAP_AT(heap[lchild + 1]) < HEAP_AT(heap[min])) min = lchild + 1;
					if (HEAP_AT(heap[lchild + 2]) < HEAP_AT(heap[min])) min = lchild + 2;
					if (HEAP_AT(heap[lchild + 3]) < HEAP_AT(heap[min])) min = lchild + 3;
				}
				/* less than four child */
				else if (lchild < size) {
					min = lchild;
					if (lchild + 1 < size && HEAP_AT(heap[lchild + 1]) < HEAP_AT(heap[min])) min = lchild + 1;
					if (lchild + 2 < size && HEAP_AT(heap[lchild + 2]) < HEAP_AT(heap[min])) min = lchild + 2;
				}
				/* has no childs */
				else
					break;

				/* if the element is smaller than all the childs, break*/
				if (HEAP_AT(bak) < HEAP_AT(heap[min]))
					break;

				heap[pos] = heap[min];
				HEAP_WATCHER_ACTIVE(heap[pos]) = pos + 1;
				lchild = LCHILD(min);
				pos = min;
			}
			heap[pos] = bak;
			HEAP_WATCHER_ACTIVE(heap[pos]) = pos + 1;
		}

		inline_ void
			common::quad_minheap::adjust(int pos)
		{
			if (!IS_ROOT(pos) && HEAP_AT(heap[pos]) < HEAP_AT(heap[PARENT(pos)]))
				shift_up(pos);
			else
				shift_down(pos);
		}

		inline_ int
			common::quad_minheap::push(ev_watcher_time * wt)
		{
			HEAP_WT(heap[size]) = wt;
			shift_up(size);
			++size;
			return 0;
		}

		inline_ ev_watcher_time *
			common::quad_minheap::pop()
		{
			if (expect_false(0 == size))
				return nullptr;

			HEAP_WATCHER_ACTIVE(heap[0]) = 0;

			ev_watcher_time * wt = HEAP_WT(heap[0]);

			HEAP_WT(heap[0]) = HEAP_WT(heap[size - 1]);
			HEAP_WT(heap[size - 1]) = nullptr;

			--size;

			shift_down(0);

			return wt;
		}

		inline_ min_heap_t &
			common::quad_minheap::top()
		{
			return heap[0];
		}

		inline_ int
			common::quad_minheap::remove(int pos)
		{
			if (expect_false(pos < 0 || pos >= size))
				return -1;

			HEAP_WATCHER_ACTIVE(heap[pos]) = 0;

			HEAP_WT(heap[pos]) = HEAP_WT(heap[size - 1]);
			HEAP_WT(heap[size - 1]) = nullptr;

			--size;

			adjust(pos);

			return  0;
		}

		inline_ bool
			common::quad_minheap::isempty()
		{
			return 0 == size;
		}

		inline_ min_heap_t&
			common::quad_minheap::get(int idx)
		{
			return heap[idx];
		}
	}
}
#endif // ! _4_MINHEAP_INTERNAL_H_

