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

#ifndef SQ_PTR_ARRAY_H
#define SQ_PTR_ARRAY_H

#ifdef __cplusplus
#include <functional>
#endif

#include <SqArray.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqPtrArray       SqPtrArray;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* macro for accessing variable of array */

#define sq_ptr_array_data                (void**)sq_array_data

#define sq_ptr_array_length              sq_array_length

#define sq_ptr_array_header              sq_array_header

#define sq_ptr_array_capacity            sq_array_capacity

#define sq_ptr_array_clear_func          sq_array_clear_func

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_ptr_array_new(unsigned int capacity, SqClearFunc clear_func);
#define sq_ptr_array_new(capacity, clear_func)     \
		(SqPtrArray*)sq_ptr_array_init(malloc(sizeof(SqPtrArray)), capacity, clear_func)

//void  sq_ptr_array_free(void *array);
#define sq_ptr_array_free(array)         free(sq_ptr_array_final(array))

//bool  sq_ptr_array_is_inited(void *array);
#define sq_ptr_array_is_inited           sq_array_is_inited

//bool  sq_ptr_array_not_inited(void *array);
#define sq_ptr_array_not_inited          sq_array_not_inited

//bool  sq_ptr_array_empty(void *array);
#define sq_ptr_array_empty               sq_array_empty

// Get address of element
//void **sq_ptr_array_addr(void *array, unsigned int index);
#define sq_ptr_array_addr(array, index)  \
		( ((SqPtrArray*)(array))->data + (index) )

//void *sq_ptr_array_at(void *array, unsigned int index);
#define sq_ptr_array_at(array, index)    \
		((SqPtrArray*)(array))->data[index]

//void *sq_ptr_array_begin(void *array);
#define sq_ptr_array_begin(array)        \
		(((SqPtrArray*)(array))->data)

//void *sq_ptr_array_end(void *array);
#define sq_ptr_array_end(array)          \
		( (((SqPtrArray*)(array))->data) + ((SqPtrArray*)(array))->length )

//void **sq_ptr_array_alloc(void *array, unsigned int count);
#define sq_ptr_array_alloc(array, count)                \
		SQ_ARRAY_ALLOC(array, void*, count)

//void **sq_ptr_array_alloc_at(void *array, unsigned int index, unsigned int count);
#define sq_ptr_array_alloc_at(array, index, count)      \
		SQ_ARRAY_ALLOC_AT(array, void*, index, count)

// void sq_ptr_array_push(void *array, void *value);
#define sq_ptr_array_push(array, value)                 \
		sq_array_push(array, void*, value)

// void sq_ptr_array_push_in(void *array, unsigned int index, void *value);
#define sq_ptr_array_push_in(array, index, value)       \
		sq_array_push_in(array, void*, index, value)

// Quick sort
// void sq_ptr_array_sort(void *array, SqCompareFunc compareFunc);
#define sq_ptr_array_sort(array, compareFunc)           \
		sq_array_sort(array, void*, compareFunc)

// Binary search for sorted array
//void **sq_ptr_array_search(void *array, const void *key, SqCompareFunc compareFunc);
#define sq_ptr_array_search(array, key, compareFunc)    \
		sq_array_search(array, void*, key, compareFunc)

//void **sq_ptr_array_find(void *array, const void *key, SqCompareFunc compareFunc);
#define sq_ptr_array_find(array, key, compareFunc)      \
		SQ_ARRAY_FIND(array, void*, key, compareFunc)

//void **sq_ptr_array_find_sorted(void *array, const void *key, SqCompareFunc compareFunc, unsigned int *insertingIndex);
#define sq_ptr_array_find_sorted(array, key, compareFunc, insertingIndex)    \
		SQ_ARRAY_FIND_SORTED(array, void*, key, compareFunc, insertingIndex)

// void sq_ptr_array_erase_addr(void *array, void **elementAddr, unsigned int count);
#define sq_ptr_array_erase_addr(array, elementAddr, count)       \
		sq_ptr_array_erase(array, (void**)(elementAddr) - sq_ptr_array_data(array), count)

// alias of sq_ptr_array_erase()
// void sq_ptr_array_remove(void *array, unsigned int index, unsigned int count);
#define sq_ptr_array_remove              sq_ptr_array_erase

// alias of sq_ptr_array_erase_addr()
// void sq_ptr_array_remove_addr(void *array, void **elementAddr, unsigned int count);
#define sq_ptr_array_remove_addr         sq_ptr_array_erase_addr

/* macro functions - Macros use parameters multiple times. Do not use the ++ operator in parameters. */

//void **sq_ptr_array_append(void *array, const void *values, unsigned int count);
#define sq_ptr_array_append(array, values, count)                \
		sq_array_append(array, void*, values, count)

//void **sq_ptr_array_insert(void *array, unsigned int index, const void *values, unsigned int count);
#define sq_ptr_array_insert(array, index, values, count)         \
		sq_array_insert(array, void*, index, values, count)

// Removes a value from array without calling the destroy function.
// void sq_ptr_array_steal(void *array, unsigned int index, unsigned int count);
#define sq_ptr_array_steal(array, index, count)                  \
		sq_array_steal(array, void*, index, count)

// void sq_ptr_array_steal_addr(void *array, void **elementAddr, unsigned int count);
#define sq_ptr_array_steal_addr(array, elementAddr, count)       \
		sq_array_steal_addr(array, void*, elementAddr, count)

/* C functions */

void  *sq_ptr_array_init(void *array, unsigned int capacity, SqClearFunc clear_func);

void  *sq_ptr_array_final(void *array);

void   sq_ptr_array_erase(void *array, unsigned int index, unsigned int count);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	PtrArrayMethod is used by SqPtrArray and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqPtrArray members.

	ArrayMethod
	|
	+--- PtrArrayMethod
 */

template<class Type>
struct PtrArrayMethod : ArrayMethod<Type>
{
	// access variable of PtrArray
	SqClearFunc clearFunc();
	void        clearFunc(SqClearFunc func);

	// member functions
	void   init(unsigned int capacity = 8, SqClearFunc func = NULL);
	void   final(void);

	// removes elements from array with calling the clear function.
	void   erase(unsigned int index, unsigned int count = 1);
	void   erase(Type *addr, unsigned int count = 1);

	// alias of erase().
	void   remove(unsigned int index, unsigned int count = 1);
	void   remove(Type *addr, unsigned int count = 1);

	// foreach
	void   foreach(std::function<void(Type  element)> func);
	void   foreach(std::function<void(Type *address)> func);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqPtrArray is array of pointer */

#ifdef __cplusplus
struct SqPtrArray : Sq::PtrArrayMethod<void*>        // <-- 1. inherit C++ member function(method)
#else
struct SqPtrArray
#endif
{
	SQ_ARRAY_MEMBERS(void*, data, length);           // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void         **data;
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

/* define PtrArrayMethod template functions */

template<class Type>
inline SqClearFunc  PtrArrayMethod<Type>::clearFunc() {
	return sq_ptr_array_clear_func(this);
}
template<class Type>
inline void  PtrArrayMethod<Type>::clearFunc(SqClearFunc func) {
	sq_ptr_array_clear_func(this) = func;
}

template<class Type>
inline void  PtrArrayMethod<Type>::init(unsigned int capacity, SqClearFunc func) {
	sq_ptr_array_init(this, capacity, func);
}
template<class Type>
inline void  PtrArrayMethod<Type>::final(void) {
	sq_ptr_array_final(this);
}

// removes elements from array with calling the clear function.
template<class Type>
inline void  PtrArrayMethod<Type>::erase(unsigned int index, unsigned int count) {
	sq_ptr_array_erase(this, index, count);
}
template<class Type>
inline void  PtrArrayMethod<Type>::erase(Type *addr, unsigned int count) {
	sq_ptr_array_erase_addr(this, (void**)addr, count);
}

// These are aliases of erase().
template<class Type>
inline void  PtrArrayMethod<Type>::remove(unsigned int index, unsigned int count) {
	sq_ptr_array_erase(this, index, count);
}
template<class Type>
inline void  PtrArrayMethod<Type>::remove(Type *addr, unsigned int count) {
	sq_ptr_array_erase_addr(this, (void**)addr, count);
}

template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type  element)> func) {
	void **beg = ((SqPtrArray*)this)->data;
	void **end = beg + ((SqPtrArray*)this)->length;
	for (void **cur = beg;  cur < end;  cur++)
		func((Type)*cur);
}
template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type *address)> func) {
	void **beg = ((SqPtrArray*)this)->data;
	void **end = beg + ((SqPtrArray*)this)->length;
	for (void **cur = beg;  cur < end;  cur++)
		func((Type*)cur);
}

/* All derived struct/class must be C++11 standard-layout. */

struct PtrArray : SqPtrArray
{
	// constructor
	PtrArray(unsigned int capacity = 8, SqClearFunc func = NULL) {
		sq_ptr_array_init(this, capacity, func);
	}
	// destructor
	~PtrArray(void) {
		sq_ptr_array_final(this);
	}
	// copy constructor
	PtrArray(const PtrArray &src) {
		sq_ptr_array_init(this, sq_ptr_array_capacity(&src), sq_ptr_array_clear_func(&src));
		sq_ptr_array_append(this, src.data, src.length);
	}
	// move constructor
	PtrArray(PtrArray &&src) {
		this->data   = src.data;
		this->length = src.length;
		src.data   = NULL;
		src.length = 0;
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_PTR_ARRAY_H
