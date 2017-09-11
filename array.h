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

#ifndef  _ARRAY_H_
#define  _ARRAY_H_

#include <cstdlib>
#include <cstring>
#include "util.h"
#include "mm.h"

// can remove
typedef double ev_tstamp;
#define expect_true(cond) __builtin_expect(!!(cond), 1)
#define expect_false(cond) __builtin_expect(!!(cond), 0)

#define MALLOC_ROUND 4096
namespace emoskit {
	namespace common
	{
		template <class T>
		class array
		{
		public:
			array();
			array(std::size_t);
			~array();
			T& operator[](std::size_t index);
		private:
			void array_resize(std::size_t size);
			std::size_t calculatesize(std::size_t newsize);
			T *elements;
			std::size_t maxsize;
		};

		template<class T>
		inline array<T>::array() : elements(nullptr), maxsize(0)
		{

		}

		template<class T>
		inline array<T>::array(std::size_t init_size)
		{
			elements = nullptr;
			maxsize = 0;
			array_resize(init_size);
		}

		template<class T>
		inline array<T>::~array()
		{
			if (elements != nullptr) {
				for (std::size_t i = 0; i < maxsize; ++i) {
					emoskit::util::Destroy(&elements[i]);
				}
				free(elements);
			}
		}

		template<class T>
		inline T & array<T>::operator[](std::size_t index)
		{
			if (expect_false(index >= maxsize)) {
				array_resize(index + 1);
			}
			return elements[index];
		}

		template<class T>
		inline void array<T>::array_resize(std::size_t size)
		{
			std::size_t newsize = calculatesize(size);
			elements = (T*)mm::mm_realloc(elements, sizeof(T) * newsize);

			// init zero
			for (std::size_t i = maxsize; i < newsize; ++i) {
				emoskit::util::Construct(&elements[i]);
			}
			//memset(elements + maxsize, 0, sizeof(T) * (newsize - maxsize));
			maxsize = newsize;
		}

		template<class T>
		inline std::size_t array<T>::calculatesize(std::size_t size)
		{
			unsigned int type_size = sizeof(T);
			unsigned int malloc_overhead = sizeof(void*) * 4;
			std::size_t newsize = size + 1;

			do
				newsize <<= 1;
			while (size > newsize);

			if (type_size * newsize > MALLOC_ROUND - malloc_overhead) {
				newsize *= newsize * type_size;
				newsize = (newsize + malloc_overhead + (MALLOC_ROUND - 1)) & ~(MALLOC_ROUND - 1);
				newsize -= malloc_overhead;
				newsize /= type_size;
			}
			return newsize;
		}
	}
}
#endif