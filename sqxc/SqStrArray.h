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

/* macro for accessing variable of SqStrArray */

#define sq_str_array_length          sq_array_length

#define sq_str_array_capacity        sq_array_capacity

#define sq_str_array_clear_func      sq_array_clear_func

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_str_array_final(SqStrArray *array);
#define sq_str_array_final           sq_ptr_array_final

//char **sq_str_array_alloc(SqStrArray *array, unsigned int count);
#define sq_str_array_alloc(array, count)                 \
		SQ_ARRAY_ALLOC(array, char*, count)

//char **sq_str_array_alloc_at(SqStrArray *array, unsigned int index, unsigned int count);
#define sq_str_array_alloc_at(array, index, count)       \
		SQ_ARRAY_ALLOC_AT(array, char*, index, count)

// void sq_str_array_erase(SqStrArray *array, unsigned int index, unsigned int count);
#define sq_str_array_erase           sq_ptr_array_erase

// void sq_str_array_erase_addr(SqStrArray *array, char **elementAddr, unsigned int count);
#define sq_str_array_erase_addr      sq_ptr_array_erase_addr

// alias of sq_str_array_erase()
// void sq_str_array_remove(SqStrArray *array, unsigned int index, unsigned int count);
#define sq_str_array_remove          sq_ptr_array_erase

// alias of sq_str_array_erase_addr()
// void sq_str_array_remove_addr(SqStrArray *array, char **elementAddr, unsigned int count);
#define sq_str_array_remove_addr     sq_ptr_array_erase_addr

// void sq_str_array_sort(void *array, SqCompareFunc compareFunc);
#define sq_str_array_sort            sq_ptr_array_sort

//char **sq_str_array_search(void *array, const char *key, SqCompareFunc compareFunc);
#define sq_str_array_search(array, key, compareFunc)    \
		sq_array_search(array, char*, key, compareFunc)

//char **sq_str_array_find(void *array, const char *key, SqCompareFunc compareFunc);
#define sq_str_array_find(array, key, compareFunc)      \
		SQ_ARRAY_FIND(array, char*, key, compareFunc)

//char **sq_array_find_sorted(void *array, const char *key, SqCompareFunc compareFunc, unsigned int *insertingIndex);
#define sq_str_array_find_sorted(array, key, compareFunc, insertingIndex)   \
		SQ_ARRAY_FIND_SORTED(array, char*, key, compareFunc, insertingIndex)

/* macro functions - Macros use parameters multiple times. Do not use the ++ operator in parameters. */

// void sq_str_array_steal(SqStrArray *array, unsigned int index, unsigned int count);
#define sq_str_array_steal           sq_ptr_array_steal

// void sq_str_array_steal_addr(SqStrArray *array, char **elementAddr, unsigned int count);
#define sq_str_array_steal_addr      sq_ptr_array_steal_addr

/* C functions */
void  *sq_str_array_init(SqStrArray *array, unsigned int capacity);

void   sq_str_array_push_in(SqStrArray *array, unsigned int index, const char *str);

// deprecated
void   sq_str_array_push_to(SqStrArray *array, unsigned int index, const char *str);

char **sq_str_array_insert(SqStrArray *array, unsigned int index, const char **strs, unsigned int count);

void   sq_str_array_push(SqStrArray *array, const char *str);
char **sq_str_array_append(SqStrArray *array, const char **strs, unsigned int count);

// for internal use only
void   sq_str_array_strdup(SqStrArray *array, unsigned int index, unsigned int count);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	StrArrayMethod is used by SqStrArray and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqStrArray members.

	ArrayMethod
	|
	+--- PtrArrayMethod
	     |
	     +--- StrArrayMethod
 */

struct StrArrayMethod : PtrArrayMethod<char*>
{
	void   append(const char **strs, unsigned int count);
	void   append(const char  *str);

	void   insert(unsigned int index, const char **strs, unsigned int count);
	void   insert(unsigned int index, const char  *str);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

#ifdef __cplusplus
struct SqStrArray : Sq::StrArrayMethod               // <-- 1. inherit C++ member function(method)
#else
struct SqStrArray
#endif
{
	SQ_ARRAY_MEMBERS(char*, data, length);           // <-- 2. inherit member variable
/*	// ------ SqPtrArray members ------
	char         **data;
	unsigned int   length;
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

/* define StrArrayMethod template functions */

inline void  StrArrayMethod::append(const char **strs, unsigned int count) {
	sq_str_array_append((SqStrArray*)this, strs, count);
}
inline void  StrArrayMethod::append(const char  *str) {
	sq_str_array_push((SqStrArray*)this, str);
}

inline void  StrArrayMethod::insert(unsigned int index, const char **strs, unsigned int count) {
	sq_str_array_insert((SqStrArray*)this, index, strs, count);
}
inline void  StrArrayMethod::insert(unsigned int index, const char  *str) {
	sq_str_array_push_in((SqStrArray*)this, index, str);
}

/* All derived struct/class must be C++11 standard-layout. */

struct StrArray : SqStrArray
{
	// constructor
	StrArray(unsigned int capacity = 8) {
		sq_ptr_array_init(this, capacity, free);
	}
	// destructor
	~StrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	StrArray(const StrArray &src) {
		sq_ptr_array_init(this, sq_str_array_capacity(&src), sq_str_array_clear_func(&src));
		sq_str_array_append(this, (const char**)src.data, src.length);
	}
	// move constructor
	StrArray(StrArray &&src) {
		this->data   = src.data;
		this->length = src.length;
		src.data   = NULL;
		src.length = 0;
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STR_ARRAY_H
