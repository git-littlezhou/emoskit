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

#ifndef  _COMMON_H_
#define  _COMMON_H_

#include <functional>

#define svr_inline static inline

namespace emoskit {

	namespace common {

		/**********************4-heap implement**********************************/

		/* 4-heap has a shallow depth and better cpu cache efficient than 2-heap
		* for n-heap, heap[x]'s childs are heap[nx + 1] to heap[nx + n], parent is heap[(x-1)/n]
		* so for 4-heap, heap[x]'s childs are heap[4x + 1] to heap[4x + 4], parent is heap[(x-1)/4]
		* if for 2-heap, if we let the heap begin at heap[1] rather than heap[0], heap[x]'s childs is heap[(2(x-1) + 1) + 1]
		* and heap[[(2(x-1) + 2) + 1]], that is, heap[2x] and heap[2x+1], and parent is heap[((x-1)-1) / 2 + 1], that is heap[x/2]
		* 2x and x/2 is easier to calculate, 2x = x << 1, x/2 = x >> 1
		*/
		#define DHEAD 4
		#define PARENT(p) ((p) - 1) / DHEAD
		#define LCHILD(p) DHEAD * p + 1
		#define IS_ROOT(p) (!(p))

		template<class T, class F = std::less<T>> svr_inline void
			downheap(T *heap, int size, int pos, F comparator = F())
		{
			T bak = heap[pos];
			int lchild = LCHILD(pos);
			int min;

			for (;;) {
				/* find the minimun child */
				/* if has four childs */
				if (lchild + DHEAD - 1 < size) {
					min = lchild;
					if (comparator(heap[lchild + 1], heap[min])) min = lchild + 1;
					if (comparator(heap[lchild + 2], heap[min])) min = lchild + 2;
					if (comparator(heap[lchild + 3], heap[min])) min = lchild + 3;
				}
				/* less than four child */
				else if (lchild < size) {
					min = lchild;
					if (lchild + 1 < size && comparator(heap[lchild + 1], heap[min])) min = lchild + 1;
					if (lchild + 2 < size && comparator(heap[lchild + 2], heap[min])) min = lchild + 2;
					//if (lchild + 3 < size && comparator(heap[lchild + 3], heap[min])) min = lchild + 3;
				}
				/* has no childs */
				else
					break;

				/* if the element is smaller than all the childs, break*/
				if (comparator(bak, heap[min]))
					break;

				heap[pos] = heap[min];
				lchild = LCHILD(min);
				pos = min;
			}
			heap[pos] = bak;
		}

		template<class T, class F = std::less<T>> svr_inline void
			upheap(T *heap, int pos, F comparator = F())
		{
			T bak = heap[pos];
			int parent = PARENT(pos);

			while (!IS_ROOT(pos)) {
				if (comparator(heap[parent], bak))
					break;
				heap[pos] = heap[parent];
				pos = parent;
				parent = PARENT(pos);
			}
			heap[pos] = bak;
		}

		template<class T, class F = std::less<T>> svr_inline void
			adjustheap(T *heap, int size, int pos, F comparator = F())
		{
			if (!IS_ROOT(pos) && comparator(heap[pos], heap[PARENT(pos)]))
				upheap(heap, pos, comparator);
			else
				downheap(heap, size, pos, comparator);
		}

		template<class T, class F = std::less<T>> svr_inline void
			buildheap(T *heap, int size, F comparator = F()) {
			for (int i = 0; i < size; ++i)
				upheap(heap, i, comparator);
		}
		/**********************4-heap implement end*************************************/
	}
}
#endif
