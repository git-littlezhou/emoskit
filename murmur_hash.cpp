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
* MurmurHash3 was written by Austin Appleby, and is placed in the public domain.
*/

#include "murmur_hash.h"

#define ROTL32(x, r) ((x) << (r)) | ((x) >> (32 - (r)))

uint32_t murmur_hash3(const void *key, size_t len, uint32_t seed){
	const uint32_t* key_x4 = (const uint32_t*) key;
	const size_t nblocks = len >> 2;
	
	uint32_t hash = seed;
	uint32_t k;
	
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;
	const uint32_t r1 = 15;
	const uint32_t r2 = 13;
	
	const uint8_t *remaining_bytes = (const uint8_t *)(key_x4 + nblocks);
	
	// for each fourByteChunk of key
	for (int i = 0; i < nblocks; i++) {
		k = *(key_x4 + i);

		k *= c1;
		k = ROTL32(k, r1);
		k *= c2;

		hash ^= k;
		hash = ROTL32(hash, r2);
		hash = hash * 5 + 0xe6546b64;
	}
	
	k = 0;
	
	//for any remaining bytes in key
	switch (len & 3){
	case 3:
		k ^= ((uint32_t)remaining_bytes[2]) << 16;
	case 2:
		k ^= ((uint32_t)remaining_bytes[1]) << 8;
	case 1:
		k ^= remaining_bytes[0];
		k *= c1; 
		k = ROTL32(k, r1);
		k *= c2;
		hash ^= k;
	}
	
	hash ^= (uint32_t)len;
	
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;
	
	return hash;
}
