/*
 *   Copyright (C) 2020 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <SqPtrArray.h>

void* sq_ptr_array_init_full(void* array,
                             int allocated_length, int header_length,
                             SqDestroyFunc  destroy_func)
{
	int   size;

	// it has been initialized
//	if (sq_ptr_array_data(array))
//		return array;

	if (header_length < SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH)
		header_length = SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH;
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

void* sq_ptr_array_final(void* array)
{
	SqDestroyFunc  destroy;

	// it doesn't initialize.
	if (sq_ptr_array_data(array) == NULL)
		return array;

	destroy = sq_ptr_array_destroy_func(array);
	if (destroy) {
		sq_ptr_array_foreach(array, element) {
			destroy(element);
		}
	}
	free(sq_ptr_array_header(array));

//	((SqPtrArray*)array)->data = NULL;
//	((SqPtrArray*)array)->length = 0;
	return array;
}

void** sq_ptr_array_alloc_at(void* array, int index, int count)
{
	void* header;
	int   header_length;
	int   new_length;
	int   allocated;

	allocated  = sq_ptr_array_allocated(array);
	new_length = sq_ptr_array_length(array) + count;
	if (allocated < new_length) {
		header_length = sq_ptr_array_header_length(array);
		header = ((SqPtrArray*)array)->data - header_length;
		if ( (allocated*=2) < new_length)
			allocated = new_length * 2;
		sq_ptr_array_allocated(array) = allocated;
		header = realloc(header, (header_length + allocated) * sizeof(void*));
		((SqPtrArray*)array)->data = (void**)header + header_length;
	}
	if (index < sq_ptr_array_length(array)) {
		memmove(sq_ptr_array_addr(array, index + count),
		        sq_ptr_array_addr(array, index),
		        sizeof(void*) * (sq_ptr_array_length(array) -index));
	}
	((SqPtrArray*)array)->length = new_length;
	return ((SqPtrArray*)array)->data + index;
}

void** sq_ptr_array_find(void* array, const void* key, SqCompareFunc cmpfunc)
{
	sq_ptr_array_foreach_addr(array, element_addr) {
		if (cmpfunc(key, element_addr) == 0)
			return element_addr;
	}
	return NULL;
}

void   sq_ptr_array_remove_null(void* parray)
{
	SqPtrArray* array = (SqPtrArray*)parray;
	int  index_src, index_dest;

	for (index_dest = 0;  index_dest < array->length;  index_dest++) {
		if (array->data[index_dest] == NULL)
			break;
	}
	for (index_src = index_dest +1;  index_src < array->length;  index_src++) {
		if (array->data[index_src])
			array->data[index_dest++] = array->data[index_src];
	}

	array->length = index_dest;
}

// ----------------------------------------------------------------------------
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqArray.h
#else

void  sq_ptr_array_erase(void* array, int index, int count)
{
	SQ_PTR_ARRAY_ERASE(array, index, count);
}

void  sq_ptr_array_steal(void* array, int index, int count)
{
	SQ_PTR_ARRAY_STEAL(array, index, count);
}

void  sq_ptr_array_insert_n(void* array, int index, const void* values, int count)
{
	SQ_PTR_ARRAY_INSERT_N(array, index, values, count);
}

void  sq_ptr_array_append_n(void* array, const void* values, int count)
{
	SQ_PTR_ARRAY_APPEND_N(array, values, count);
}

#endif  // __STDC_VERSION__
