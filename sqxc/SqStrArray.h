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

#ifndef SQ_STR_ARRAY_H
#define SQ_STR_ARRAY_H

#include <SqPtrArray.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqStrArray    SqStrArray;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/*	macro for accessing variable of array

	Header field of SqStrArray
	strArray->data[-1]: (intptr_t) header length
	strArray->data[-2]: (intptr_t) allocated length
	strArray->data[-3]: (SqDestroyFunc*) destroy function
 */

/* macro for SqStrArray */

#define sq_str_array_length(array)     \
		((SqStrArray*)(array))->length

#define sq_str_array_allocated(array)  \
		*(intptr_t*)(((SqStrArray*)(array))->data -2)

#define sq_str_array_capacity(array)   \
		*(intptr_t*)(((SqStrArray*)(array))->data -2)

#define sq_str_array_destroy_func(array)  \
		*(SqDestroyFunc*)(((SqStrArray*)(array))->data -3)

//void *sq_str_array_final(SqStrArray *array);
#define sq_str_array_final       sq_ptr_array_final

//char **sq_str_array_alloc(SqStrArray *array, int count)
#define sq_str_array_alloc       sq_ptr_array_alloc

//char **sq_str_array_alloc_at(SqStrArray *array, int index, int count)
#define sq_str_array_alloc_at    sq_ptr_array_alloc_at

// void sq_str_array_erase(SqStrArray *array, int index, int count);
#define sq_str_array_erase       sq_ptr_array_erase

// void sq_str_array_steal(SqStrArray *array, int index, int count);
#define sq_str_array_steal       sq_ptr_array_steal

// void sq_str_array_foreach(SqStrArray *array, char *element)
#define sq_str_array_foreach(array, element)                    \
		for (char **element##_end  = (char**)sq_ptr_array_end(array),   \
		          **element##_addr = (char**)sq_ptr_array_begin(array), \
		           *element = *element##_addr;                  \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

// void sq_str_array_foreach_addr(SqStrArray *array, char **element_addr)
#define sq_str_array_foreach_addr(array, element_addr)              \
		for (void **element_addr##_end = sq_ptr_array_end(array),   \
		          **element_addr       = sq_ptr_array_begin(array); \
		     element_addr < element_addr##_end;                     \
		     element_addr++)

/* C functions */
void  *sq_str_array_init(SqStrArray *array, int allocated_length);

void   sq_str_array_insert(SqStrArray *array, int index, const char *str);
void   sq_str_array_insert_n(SqStrArray *array, int index, const char **strs, int count);

void   sq_str_array_append(SqStrArray *array, const char *str);
void   sq_str_array_append_n(SqStrArray *array, const char **strs, int count);

// for internal use only
void   sq_str_array_dup_n(SqStrArray *array, int index, int length);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {


};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

#ifdef __cplusplus
struct SqStrArray : Sq::PtrArrayMethod<char*>        // <-- 1. inherit C++ member function(method)
#else
struct SqStrArray
#endif
{
	SQ_PTR_ARRAY_MEMBERS(char*, data, length);       // <-- 2. inherit member variable
/*	// ------ SqPtrArray members ------
	char    **data;
	int       length;
 */
};


// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define PtrArrayMethod template functions */


/* All derived struct/class must be C++11 standard-layout. */

struct StrArray : SqStrArray
{
	// constructor
	StrArray(int allocated_length = 0) {
		sq_str_array_init(this, allocated_length);
	}
	// destructor
	~StrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	StrArray(StrArray &src) {
		sq_str_array_append_n(this, (const char**)src.data, src.length);
		sq_str_array_destroy_func(this) = sq_str_array_destroy_func(&src);
		if (sq_str_array_destroy_func(this) == NULL)
			sq_str_array_destroy_func(this) = free;
	}
	// move constructor
	StrArray(StrArray &&src) {
		this->data = src.data;
		this->length = src.length;
		src.data = NULL;
		src.length = 0;
	}

	// ------ StrArray method ------
	void  insert(int index, const char **strs, int length) {
		sq_str_array_insert_n(this, index, strs, length);
	}
	void  insert(int index, const char *str) {
		sq_str_array_insert(this, index, str);
	}

	void  append(const char **strs, int length) {
		sq_str_array_append_n(this, strs, length);
	}
	void  append(const char *str) {
		sq_str_array_append(this, str);
	}
};

};  // namespace Sq

#endif  // __cplusplus


// ============================================================================
// SqStringArray is defined for compatibility with older versions

typedef struct SqStrArray    SqStringArray;

#define sq_string_array_length          sq_str_array_length

#define sq_string_array_allocated       sq_str_array_allocated

#define sq_string_array_capacity        sq_str_array_capacity

#define sq_string_array_destroy_func    sq_str_array_destroy_func

//void  sq_string_array_init(void *array, int allocated_length, SqDestroyFunc destroy_func);
#define sq_string_array_init(array, allocated_length, destroy_func)    \
		sq_ptr_array_init_full(array, allocated_length, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, destroy_func)

//void *sq_string_array_final(void *array);
#define sq_string_array_final           sq_ptr_array_final

// void sq_string_array_insert(void *array, int index, void *str);
#define sq_string_array_insert(array, index, str)  \
		*sq_ptr_array_alloc_at(array, index, 1) = (void*)(str)

// void sq_string_array_append(void *array, void *str);
#define sq_string_array_append(array, str)  \
		*sq_ptr_array_alloc_at(array, sq_string_array_length(array), 1) = (void*)(str)

// void sq_string_array_erase(void *array, int index, int count);
#define sq_string_array_erase           sq_ptr_array_erase

// void sq_string_array_steal(void *array, int index, int count);
#define sq_string_array_steal           sq_ptr_array_steal

// void sq_string_array_foreach(void *array, char *element)
#define sq_string_array_foreach(array, element)                 \
		for (char **element##_end  = (char**)sq_ptr_array_end(array),   \
		          **element##_addr = (char**)sq_ptr_array_begin(array), \
		           *element = *element##_addr;                  \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

// void sq_string_array_foreach_addr(void *array, char **element_addr)
#define sq_string_array_foreach_addr(array, element_addr)           \
		for (void **element_addr##_end = sq_ptr_array_end(array),   \
		          **element_addr       = sq_ptr_array_begin(array); \
		     element_addr < element_addr##_end;                     \
		     element_addr++)


#ifdef __cplusplus
namespace Sq {
typedef struct StrArray    StringArray;
};
#endif  // __cplusplus


#endif  // SQ_STR_ARRAY_H
