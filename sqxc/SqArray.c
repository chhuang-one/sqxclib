/*
 *   Copyright (C) 2020-2024 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <SqArray.h>

void *sq_array_init(void *array,
                    unsigned int  elementSize,
                    unsigned int  capacity)
{
	((SqArray*)array)->data = (uint8_t*)malloc(sizeof(SqArrayHeader) + capacity * elementSize)
	                          + sizeof(SqArrayHeader);
	((SqArray*)array)->length = 0;
	sq_array_element_size(array)  = elementSize;
	sq_array_capacity(array)      = capacity;

	return array;
}

void *sq_array_final(void *array)
{
	// it doesn't initialize.
	if (sq_array_not_inited(array))
		return array;

	free(sq_array_header(array));

	((SqArray*)array)->data = NULL;
	((SqArray*)array)->length = 0;
	return array;
}

void *sq_array_alloc_at(void *array, unsigned int index, unsigned int count)
{
	unsigned int  new_length;
	unsigned int  capacity;
	unsigned int  length;
	unsigned int  elementSize;

	// it doesn't initialize.
//	if (sq_array_not_inited(array))
//		return NULL;

	elementSize = sq_array_element_size(array);
	length      = sq_array_length(array);
	capacity    = sq_array_capacity(array);
	new_length  = length + count;

	if (capacity < new_length) {
		if ( (capacity*=2) < new_length)
			capacity = new_length * 2;
		sq_array_capacity(array) = capacity;
		sq_array_data(array) = (uint8_t*)realloc(sq_array_data(array) - sizeof(SqArrayHeader),
		                                         sizeof(SqArrayHeader) + capacity * elementSize)
		                       + sizeof(SqArrayHeader);
	}

	if (index < length) {
		memmove(sq_array_data(array) + (index + count) * elementSize,
		        sq_array_data(array) +  index * elementSize,
		        (length - index) * elementSize);
	}
	else
		index = length;

	((SqArray*)array)->length = new_length;
	return ((SqArray*)array)->data + index * elementSize;
}

void *sq_array_find(const void   *array,
                    unsigned int  elementSize,
                    const void   *key,
                    SqCompareFunc compareFunc)
{
	uint8_t *cur = sq_array_data(array);
	uint8_t *end = cur + sq_array_length(array) * elementSize;

	for (;  cur < end;  cur += elementSize) {
		if (compareFunc(key, cur) == 0)
			return cur;
	}
	return NULL;
}

void  *sq_array_find_sorted(const void   *array,
                            unsigned int  elementSize,
                            const void   *key,
                            SqCompareFunc compareFunc,
                            unsigned int *insertingIndex)
{
	unsigned int  low;
	unsigned int  cur;
	unsigned int  high;
	int      diff;
	uint8_t *cur_addr;

	low  = 0;
	cur  = 0;
	high = sq_array_length(array);

	while (low < high) {
//		cur = low + ((high - low) /  2);
		cur = low + ((high - low) >> 1);
		// compare current element
		cur_addr = sq_array_data(array) + cur * elementSize;
		diff = compareFunc(key,  cur_addr);

		if (diff == 0) {
			if (insertingIndex)
				*insertingIndex = cur;
			// return current element
			return  cur_addr;
		}
		else if (diff < 0)
			high = cur;
		else    // if (diff > 0)
			low = cur + 1;
	}

	if (insertingIndex) {
		if (cur < low)
			cur++;
		insertingIndex[0] = cur;
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqArray.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__
