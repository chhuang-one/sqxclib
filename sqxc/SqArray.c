/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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
                    int   element_size,
                    int   capacity)
{
	((SqArray*)array)->data = (uint8_t*)malloc(sizeof(SqArrayHeader) + capacity * element_size)
	                          + sizeof(SqArrayHeader);
	((SqArray*)array)->length = 0;
	sq_array_element_size(array)  = element_size;
	sq_array_capacity(array)      = capacity;

	return array;
}

void *sq_array_final(void *array)
{
/*
	// it doesn't initialize.
	if (sq_array_not_inited(array))
		return array;
*/

	free(sq_array_header(array));

	((SqArray*)array)->data = NULL;
	((SqArray*)array)->length = 0;
	return array;
}

void *sq_array_alloc_at(void *array, int index, int count)
{
	int   new_length;
	int   capacity;
	int   length;
	int   element_size;

	// it doesn't initialize.
//	if (sq_array_not_inited(array))
//		return NULL;

	element_size = sq_array_element_size(array);
	length     = sq_array_length(array);
	capacity   = sq_array_capacity(array);
	new_length = length + count;

	if (capacity < new_length) {
		if ( (capacity*=2) < new_length)
			capacity = new_length * 2;
		sq_array_capacity(array) = capacity;
		sq_array_data(array) = (uint8_t*)realloc(sq_array_data(array) - sizeof(SqArrayHeader),
		                                         sizeof(SqArrayHeader) + capacity * element_size)
		                       + sizeof(SqArrayHeader);
	}

	if (index < length) {
		memmove(sq_array_data(array) + (index + count) * element_size,
		        sq_array_data(array) +  index * element_size,
		        (length - index) * element_size);
	}
	else
		index = length;

	((SqArray*)array)->length = new_length;
	return ((SqArray*)array)->data + index * element_size;
}

void *sq_array_find(void *array,
                    const void *key,
                    SqCompareFunc cmpfunc)
{
	int      element_size = sq_array_element_size(array);
	uint8_t *cur = sq_array_data(array);
	uint8_t *end = cur + sq_array_length(array) * element_size;

	for (;  cur < end;  cur += element_size) {
		if (cmpfunc(key, cur) == 0)
			return cur;
	}
	return NULL;
}

void  *sq_array_find_sorted(void *array,
                            const void *key,
                            SqCompareFunc compare,
                            int  *inserted_index)
{
	int      element_size = sq_array_element_size(array);
	int      low;
	int      cur;
	int      high;
	int      diff;
	uint8_t *cur_addr;

	low  = 0;
	cur  = 0;
	high = sq_array_length(array);

	while (low < high) {
//		cur = low + ((high - low) /  2);
		cur = low + ((high - low) >> 1);
		// compare current element
		cur_addr = sq_array_data(array) + cur * element_size;
		diff = compare(key,  cur_addr);

		if (diff == 0) {
			if (inserted_index)
				*inserted_index = cur;
			// return current element
			return  cur_addr;
		}
		else if (diff < 0)
			high = cur;
		else    // if (diff > 0)
			low = cur + 1;
	}

	if (inserted_index) {
		if (cur < low)
			cur++;
		inserted_index[0] = cur;
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
