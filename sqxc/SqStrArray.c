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

#include <SqStrArray.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

void   sq_str_array_insert(SqStrArray *array, int index, const char *str)
{
	sq_ptr_array_push_to(array, index, strdup(str));
}

void   sq_str_array_insert_n(SqStrArray *array, int index, const char **strs, int count)
{
	SQ_PTR_ARRAY_INSERT(array, index, strs, count);
	sq_str_array_dup_n(array, index, count);
}

void   sq_str_array_append(SqStrArray *array, const char *str)
{
	sq_ptr_array_push(array, strdup(str));
}

void   sq_str_array_append_n(SqStrArray *array, const char **strs, int count)
{
	SQ_PTR_ARRAY_APPEND(array, strs, count);
	sq_str_array_dup_n(array, array->length -count, count);
}

void   sq_str_array_dup_n(SqStrArray *array, int index, int length)
{
	char **cur = array->data + index;
	char **end = cur + length;

	for (;  cur < end;  cur++)
		*cur =  strdup(*cur);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqPtrArray.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__
