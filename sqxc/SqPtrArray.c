/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#include <SqPtrArray.h>

void *sq_ptr_array_init_full(void *array,
                             int allocated_length, int header_length,
                             SqDestroyFunc  destroy_func)
{
	int   size;

	if (header_length < SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT)
		header_length = SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT;
	if (allocated_length == 0)
		allocated_length  = 8;
	size = (header_length + allocated_length) * sizeof(void*);
	// allocate memory
	if (array == NULL)
		array = malloc(sizeof(SqPtrArray));
	((SqPtrArray*)array)->data = (void**)malloc(size) + header_length;
	((SqPtrArray*)array)->length = 0;
	sq_ptr_array_header_length(array) = header_length;
	sq_ptr_array_allocated(array) = allocated_length;
	sq_ptr_array_destroy_func(array) = destroy_func;

	return array;
}

void *sq_ptr_array_final(void *array)
{
	SqDestroyFunc  destroy;

	// it doesn't initialize.
	if (sq_ptr_array_not_inited(array))
		return array;

	destroy = sq_ptr_array_destroy_func(array);
	if (destroy) {
		sq_ptr_array_foreach(array, element) {
			if (element)
				destroy(element);
		}
	}
	free(sq_ptr_array_header(array));

	((SqPtrArray*)array)->data = NULL;
	((SqPtrArray*)array)->length = 0;
	return array;
}

void **sq_ptr_array_alloc_at(void *array, int index, int count)
{
	void *header;
	int   header_length;
	int   new_length;
	int   allocated;
	int   length;

	// it doesn't initialize.
//	if (sq_ptr_array_not_inited(array))
//		sq_ptr_array_init_full(array, count*2, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, NULL);

	length     = sq_ptr_array_length(array);
	allocated  = (int)sq_ptr_array_allocated(array);
	new_length = length + count;
	if (allocated < new_length) {
		header_length = (int)sq_ptr_array_header_length(array);
		header = ((SqPtrArray*)array)->data - header_length;
		if ( (allocated*=2) < new_length)
			allocated = new_length * 2;
		sq_ptr_array_allocated(array) = allocated;
		header = realloc(header, (header_length + allocated) * sizeof(void*));
		((SqPtrArray*)array)->data = (void**)header + header_length;
	}

	if (index < length) {
		memmove(sq_ptr_array_addr(array, index + count),
		        sq_ptr_array_addr(array, index),
		        sizeof(void*) * (length -index));
	}
	else
		index = length;

	((SqPtrArray*)array)->length = new_length;
	return ((SqPtrArray*)array)->data + index;
}

void **sq_ptr_array_find(void *array, const void *key, SqCompareFunc cmpfunc)
{
	sq_ptr_array_foreach_addr(array, element_addr) {
		if (cmpfunc(key, element_addr) == 0)
			return element_addr;
	}
	return NULL;
}

void sq_ptr_array_erase(void *array, int index, int count)
{
	SqDestroyFunc  destroy_func;

	// it doesn't initialize.
//	if (sq_ptr_array_not_inited(array))
//		return;

	// destroy elements
	destroy_func = sq_ptr_array_destroy_func(array);
	if (destroy_func) {
		for (int n = 0;  n < count;  n++)
			destroy_func(sq_ptr_array_at(array, index +n));
	}
	// move data
	memmove(sq_ptr_array_addr(array, index),
	        sq_ptr_array_addr(array, index + count),
	        sizeof(void*) * (sq_ptr_array_length(array) -count -index) );
	((SqPtrArray*)(array))->length -= (count);
}

void **sq_ptr_array_find_sorted(void *array, const void *key,
                                SqCompareFunc compare, int *inserted_index)
{
	int      low;
	int      cur;
	int      high;
	int      diff;
	void    *cur_key;

	low  = 0;
	cur  = 0;
	high = ((SqPtrArray*)array)->length;
	while (low < high) {
//		cur = low + ((high - low) / 2);
		cur = low + ((high - low) >> 1);
		cur_key = ((SqPtrArray*)array)->data + cur;

		diff = compare(key, cur_key);
		if (diff == 0) {
			if (inserted_index)
				inserted_index[0] = cur;
			return ((SqPtrArray*)array)->data + cur;
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
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqPtrArray.h

#else   // __STDC_VERSION__
// declare functions here if compiler does NOT support inline function.

void  sq_ptr_array_steal(void *array, int index, int count)
{
	SQ_PTR_ARRAY_STEAL(array, index, count);
}

void  sq_ptr_array_steal_addr(void *array, void **element_addr, int count)
{
	SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count);
}

void  sq_ptr_array_insert_n(void *array, int index, const void *values, int count)
{
	SQ_PTR_ARRAY_INSERT_N(array, index, values, count);
}

void  sq_ptr_array_append_n(void *array, const void *values, int count)
{
	SQ_PTR_ARRAY_APPEND_N(array, values, count);
}

#endif  // __STDC_VERSION__
