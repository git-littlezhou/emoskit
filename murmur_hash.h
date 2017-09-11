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

#ifndef _MURMUR_HASH_H_
#define _MURMUR_HASH_H_

#include <stddef.h>
#include <cstdint>

/* compute the hash of key */
uint32_t murmur_hash3(const void *key, size_t len, uint32_t seed);

#endif // !_MURMUR_HASH_H_
