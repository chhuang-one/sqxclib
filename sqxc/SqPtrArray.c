/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

void *sq_ptr_array_init(void *array, unsigned int capacity, SqClearFunc clear_func)
{
	if (capacity == 0)
		capacity  = 8;
	sq_array_init(array, sizeof(void*), capacity);
	sq_ptr_array_clear_func(array) = clear_func;
	return array;
}

void *sq_ptr_array_final(void *array)
{
	// it doesn't initialize.
	if (sq_ptr_array_not_inited(array))
		return array;

	sq_ptr_array_erase(array, 0, sq_array_length(array));
	return sq_array_final(array);
}

void sq_ptr_array_erase(void *array, unsigned int index, unsigned int count)
{
	SqClearFunc  clear_func;
	void **beg;
	void **end;

	// it doesn't initialize.
//	if (sq_ptr_array_not_inited(array))
//		return;

	clear_func = sq_ptr_array_clear_func(array);
	beg = sq_ptr_array_data(array) + index;
	end = beg + count;
	// clear element pointers
	if (clear_func) {
		for (void **cur = beg;  cur < end;  cur++)
			if (*cur)
				clear_func(*cur);
	}

	// move data
	memmove(beg, end,
	        (sq_array_length(array) -index -count) * sizeof(void*));
	((SqPtrArray*)(array))->length -= count;
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqPtrArray.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__
