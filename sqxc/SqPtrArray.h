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

#ifndef SQ_PTR_ARRAY_H
#define SQ_PTR_ARRAY_H

#ifdef __cplusplus
#include <functional>
#endif

#include <SqArray.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqPtrArray       SqPtrArray;

// deprecated
typedef struct SqIntptrArray    SqIntptrArray;
typedef struct SqUintptrArray   SqUintptrArray;

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

// deprecated
#define sq_ptr_array_destroy_func        sq_array_clear_func

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_ptr_array_new(int capacity, SqClearFunc clear_func);
#define sq_ptr_array_new(capacity, clear_func)  \
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
//void **sq_ptr_array_addr(void *array, int index);
#define sq_ptr_array_addr(array, index)  \
		( ((SqPtrArray*)(array))->data + (index) )

//void *sq_ptr_array_at(void *array, int index);
#define sq_ptr_array_at(array, index)    \
		((SqPtrArray*)(array))->data[index]
//		*( ((SqPtrArray*)(array))->data + (index) )

//void *sq_ptr_array_begin(void *array);
#define sq_ptr_array_begin(array)  \
		(((SqPtrArray*)(array))->data)

//void *sq_ptr_array_end(void *array);
#define sq_ptr_array_end(array)    \
		( (((SqPtrArray*)(array))->data) + ((SqPtrArray*)(array))->length )

//void **sq_ptr_array_alloc(void *array, int count);
#define sq_ptr_array_alloc               (void**)sq_array_alloc

//void **sq_ptr_array_alloc_at(void *array, int index, int count);
#define sq_ptr_array_alloc_at            (void**)sq_array_alloc_at

// void sq_ptr_array_push(void *array, void *value);
#define sq_ptr_array_push(array, value)            \
		*(void**)sq_array_alloc_at(array, sq_array_length(array), 1) = (void*)(value)

// void sq_ptr_array_push_to(void *array, int index, void *value);
#define sq_ptr_array_push_to(array, index, value)  \
		*(void**)sq_array_alloc_at(array, index, 1) = (void*)(value)

// void sq_ptr_array_remove(void *array, int index, int count);
#define sq_ptr_array_remove              sq_ptr_array_erase

// void sq_ptr_array_erase_addr(void *array, void **element_addr, int count);
#define sq_ptr_array_erase_addr(array, element_addr, count)    \
		sq_ptr_array_erase(array, (int)((void**)(element_addr) - sq_ptr_array_data(array)), count)

// void sq_ptr_array_remove_addr(void *array, void **element_addr, int count);
#define sq_ptr_array_remove_addr         sq_ptr_array_erase_addr

// Quick sort
// void sq_ptr_array_sort(void *array, SqCompareFunc compareFunc);
#define sq_ptr_array_sort(array, compareFunc)           \
		SQ_ARRAY_SORT(array, void*, compareFunc)

// Binary search for sorted array
//void **sq_ptr_array_search(void *array, const void *key, SqCompareFunc compareFunc);
#define sq_ptr_array_search(array, key, compareFunc)    \
		(void**)SQ_ARRAY_SEARCH(array, void*, key, compareFunc)

//void **sq_ptr_array_find(void *array, const void *key, SqCompareFunc compareFunc);
#define sq_ptr_array_find(array, key, compareFunc)      \
		(void**)SQ_ARRAY_FIND(array, void*, key, compareFunc)

//void **sq_array_find_sorted(void *array, const void *key, SqCompareFunc compareFunc, int *insertingIndex);
#define sq_ptr_array_find_sorted(array, key, compareFunc, insertingIndex)    \
		(void**)SQ_ARRAY_FIND_SORTED(array, void*, key, compareFunc, insertingIndex)

// deprecated
// void sq_ptr_array_foreach(void *array, void *element)
#define sq_ptr_array_foreach(array, element)                    \
		for (void **element##_end  = sq_ptr_array_end(array),   \
		          **element##_addr = sq_ptr_array_begin(array), \
		           *element = *element##_addr;                  \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

// deprecated
// void sq_ptr_array_foreach_addr(void *array, void **element_addr)
#define sq_ptr_array_foreach_addr(array, element_addr)              \
		for (void **element_addr##_end = sq_ptr_array_end(array),   \
		          **element_addr       = sq_ptr_array_begin(array); \
		     element_addr < element_addr##_end;                     \
		     element_addr++)

/* macro for maintaining C/C++ inline functions easily */

//void **SQ_PTR_ARRAY_APPEND(void *array, const void *values, int count);
#define SQ_PTR_ARRAY_APPEND(array, values, count)                \
		(void**)SQ_ARRAY_APPEND(array, void*, values, count)

//void **SQ_PTR_ARRAY_INSERT(void *array, int index, const void *values, int count);
#define SQ_PTR_ARRAY_INSERT(array, index, values, count)         \
		(void**)SQ_ARRAY_INSERT(array, void*, index, values, count)

// Removes a value from array without calling the destroy function.
// void SQ_PTR_ARRAY_STEAL(void *array, int index, int count);
#define SQ_PTR_ARRAY_STEAL(array, index, count)                  \
		SQ_ARRAY_STEAL(array, void*, index, count)

// void SQ_PTR_ARRAY_STEAL_ADDR(void *array, void **element_addr, int count);
#define SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count)      \
		SQ_ARRAY_STEAL_ADDR(array, void*, element_addr, count)

/* macro for SqIntptrArray */

#define sq_intptr_array_length           sq_array_length

#define sq_intptr_array_capacity         sq_array_capacity

//void  sq_intptr_array_init(void *array, int capacity);
#define sq_intptr_array_init(array, capacity)    \
		sq_array_init(array, sizeof(intptr_t), capacity)

//void *sq_intptr_array_final(void *array);
#define sq_intptr_array_final            sq_array_final

// intptr_t *sq_intptr_array_alloc(void *array, int count);
#define sq_intptr_array_alloc            (intptr_t*)sq_array_alloc

// intptr_t *sq_intptr_array_alloc_at(void *array, int index, int count);
#define sq_intptr_array_alloc_at         (intptr_t*)sq_array_alloc_at

// void sq_intptr_array_append(void *array, void *value);
#define sq_intptr_array_append(array, value)  \
		*sq_ptr_array_alloc_at(array, sq_intptr_array_length(array), 1) = (void*)(value)

// void sq_intptr_array_append_n(void *array, const intptr_t *values, int count);
#define sq_intptr_array_append_n         sq_ptr_array_append

// void sq_intptr_array_insert(void *array, int index, void *value);
#define sq_intptr_array_insert(array, index, value)  \
		*sq_intptr_array_alloc_at(array, index, 1) = (intptr_t*)(value)

// void sq_intptr_array_insert_n(void *array, int index, const intptr_t *values, int count);
#define sq_intptr_array_insert_n         sq_ptr_array_insert

// void sq_intptr_array_remove(void *array, int index, int count);
#define sq_intptr_array_remove           sq_ptr_array_erase

// void sq_intptr_array_erase(void *array, int index, int count);
#define sq_intptr_array_erase            sq_ptr_array_erase

// deprecated
// void sq_intptr_array_foreach(void *array, intptr_t element)
#define sq_intptr_array_foreach(array, element)                \
		for (intptr_t *element##_end  = (intptr_t*)sq_ptr_array_end(array),   \
		              *element##_addr = (intptr_t*)sq_ptr_array_begin(array), \
		               element        = *element##_addr;       \
		     element##_addr < element##_end;                   \
		     element##_addr++, element = *element##_addr)

/* macro for SqUintptrArray */

#define sq_uintptr_array_length          sq_array_length

#define sq_uintptr_array_capacity        sq_array_capacity

//void  sq_uintptr_array_init(void *array, int capacity);
#define sq_uintptr_array_init(array, capacity)    \
		sq_array_init(array, sizeof(uintptr_t), capacity)

//void *sq_uintptr_array_final(void *array);
#define sq_uintptr_array_final           sq_array_final

// uintptr_t *sq_uintptr_array_alloc(void *array, int count);
#define sq_uintptr_array_alloc           (uintptr_t*)sq_array_alloc

// uintptr_t *sq_uintptr_array_alloc_at(void *array, int index, int count);
#define sq_uintptr_array_alloc_at        (uintptr_t*)sq_array_alloc_at

// void sq_uintptr_array_append(void *array, void *value);
#define sq_uintptr_array_append(array, value)  \
		*sq_ptr_array_alloc_at(array, sq_uintptr_array_length(array), 1) = (void*)(value)

// void sq_uintptr_array_append_n(void *array, const intptr_t *values, int count);
#define sq_uintptr_array_append_n        sq_ptr_array_append

// void sq_uintptr_array_insert(void *array, int index, uintptr_t *value);
#define sq_uintptr_array_insert(array, index, value)  \
		*sq_uintptr_array_alloc_at(array, index, 1) = (uintptr_t)(value)

// void sq_uintptr_array_insert_n(void *array, int index, const intptr_t *values, int count);
#define sq_uintptr_array_insert_n        sq_ptr_array_insert

// void sq_uintptr_array_erase(void *array, int index, int count);
#define sq_uintptr_array_erase           sq_ptr_array_erase

// void sq_uintptr_array_remove(void *array, int index, int count);
#define sq_uintptr_array_remove          sq_ptr_array_erase

// deprecated
// void sq_uintptr_array_foreach(void *array, uintptr_t element)
#define sq_uintptr_array_foreach(array, element)                \
		for (uintptr_t *element##_end  = (uintptr_t*)sq_ptr_array_end(array),   \
		               *element##_addr = (uintptr_t*)sq_ptr_array_begin(array), \
		                element        = *element##_addr;       \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

/* C functions */

void  *sq_ptr_array_init(void *array, int capacity, SqClearFunc clear_func);

void  *sq_ptr_array_final(void *array);

void   sq_ptr_array_erase(void *array, int index, int count);

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

	// deprecated
	SqDestroyFunc destroyFunc();
	// deprecated
	void          destroyFunc(SqDestroyFunc func);

	// member functions
	void   init(int capacity = 8, SqClearFunc func = NULL);
	void   final(void);

	bool   isInited(void);
	bool   notInited(void);

	void   append(Type *values, int length = 1);
	void   append(Type  value);
	void   insert(int index, Type *values, int length = 1);
	void   insert(int index, Type  value);

	void   remove(int index, int length = 1);
	void   remove(Type *addr, int length = 1);
	void   erase(int index, int length = 1);
	void   erase(Type *addr, int length = 1);

	// foreach
	void   foreach(std::function<void(Type  element)> func);
	void   foreach(std::function<void(Type *address)> func);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqPtrArray is array of pointer */

#define SQ_PTR_ARRAY_MEMBERS             SQ_ARRAY_MEMBERS

#ifdef __cplusplus
struct SqPtrArray : Sq::PtrArrayMethod<void*>        // <-- 1. inherit C++ member function(method)
#else
struct SqPtrArray
#endif
{
	SQ_ARRAY_MEMBERS(void*, data, length);           // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void    **data;
	int       length;
 */
};

#ifdef __cplusplus
struct SqIntptrArray : Sq::PtrArrayMethod<intptr_t>  // <-- 1. inherit C++ member function(method)
#else
struct SqIntptrArray
#endif
{
	SQ_ARRAY_MEMBERS(intptr_t, data, length);        // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	intptr_t  *data;
	int        length;
 */
};

#ifdef __cplusplus
struct SqUintptrArray : Sq::PtrArrayMethod<uintptr_t>    // <-- 1. inherit C++ member function(method)
#else
struct SqUintptrArray
#endif
{
	SQ_ARRAY_MEMBERS(uintptr_t, data, length);           // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	uintptr_t *data;
	int        length;
 */
};


// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void **sq_ptr_array_append(void *array, const void *values, int count)
{
	return SQ_PTR_ARRAY_APPEND(array, values, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void **sq_ptr_array_insert(void *array, int index, const void *values, int count)
{
	return SQ_PTR_ARRAY_INSERT(array, index, values, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_steal(void *array, int index, int count)
{
	SQ_PTR_ARRAY_STEAL(array, index, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_steal_addr(void *array, void **element_addr, int count)
{
	SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

void  sq_ptr_array_append(void *array, const void *values, int count);
void  sq_ptr_array_insert(void *array, int index, const void *values, int count);
void  sq_ptr_array_steal(void *array, int index, int count);
void  sq_ptr_array_steal_addr(void *array, void **element_addr, int count);

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

// deprecated
template<class Type>
inline SqDestroyFunc  PtrArrayMethod<Type>::destroyFunc() {
	return sq_ptr_array_destroy_func(this);
}
// deprecated
template<class Type>
inline void  PtrArrayMethod<Type>::destroyFunc(SqDestroyFunc func) {
	sq_ptr_array_destroy_func(this) = func;
}

template<class Type>
inline void  PtrArrayMethod<Type>::init(int capacity, SqClearFunc func) {
	sq_ptr_array_init(this, capacity, func);
}
template<class Type>
inline void  PtrArrayMethod<Type>::final(void) {
	sq_ptr_array_final(this);
}

template<class Type>
inline bool  PtrArrayMethod<Type>::isInited(void) {
	return sq_ptr_array_is_inited(this);
}
template<class Type>
inline bool  PtrArrayMethod<Type>::notInited(void) {
	return sq_ptr_array_not_inited(this);
}

template<class Type>
inline void  PtrArrayMethod<Type>::append(Type *values, int length) {
	SQ_ARRAY_APPEND(this, Type, values, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::append(Type  value) {
	*(Type*)sq_array_alloc_at(this, sq_array_length(this), 1) = value;
}
template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type *values, int length) {
	SQ_ARRAY_INSERT(this, Type, index, values, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type  value) {
	*(Type*)sq_array_alloc_at(this, index, 1) = value;
}

template<class Type>
inline void  PtrArrayMethod<Type>::remove(int index, int length) {
	sq_ptr_array_erase(this, index, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::remove(Type *addr, int length) {
	sq_ptr_array_erase_addr(this, (void**)addr, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::erase(int index, int length) {
	sq_ptr_array_erase(this, index, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::erase(Type *addr, int length) {
	sq_ptr_array_erase_addr(this, (void**)addr, length);
}

template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type  element)> func) {
	sq_ptr_array_foreach(this, element) {
		func((Type)element);
	}
}
template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type *address)> func) {
	sq_ptr_array_foreach_addr(this, address) {
		func((Type*)address);
	}
}

/* All derived struct/class must be C++11 standard-layout. */

struct PtrArray : SqPtrArray
{
	// constructor
	PtrArray(int capacity = 8, SqClearFunc func = NULL) {
		sq_ptr_array_init(this, capacity, func);
	}
	// destructor
	~PtrArray(void) {
		sq_ptr_array_final(this);
	}
	// copy constructor
	PtrArray(const PtrArray &src) {
		sq_ptr_array_init(this, sq_ptr_array_capacity(&src), sq_ptr_array_clear_func(&src));
		SQ_PTR_ARRAY_APPEND(this, src.data, src.length);
	}
	// move constructor
	PtrArray(PtrArray &&src) {
		this->data   = src.data;
		this->length = src.length;
		src.data   = NULL;
		src.length = 0;
	}
};

// deprecated
struct IntptrArray : SqIntptrArray
{
	// constructor
	IntptrArray(int capacity = 8) {
		sq_ptr_array_init(this, capacity, NULL);
	}
	// destructor
	~IntptrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	IntptrArray(const IntptrArray &src) {
		sq_ptr_array_init(this, sq_ptr_array_capacity(&src), sq_ptr_array_clear_func(&src));
		SQ_PTR_ARRAY_APPEND(this, src.data, src.length);
	}
	// move constructor
	IntptrArray(IntptrArray &&src) {
		data = src.data;
		length = src.length;
		src.data = NULL;
		src.length = 0;
	}
};

// deprecated
struct UintptrArray : SqUintptrArray
{
	// constructor
	UintptrArray(int capacity = 8) {
		sq_ptr_array_init(this, capacity, NULL);
	}
	// destructor
	~UintptrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	UintptrArray(const UintptrArray &src) {
		sq_ptr_array_init(this, sq_ptr_array_capacity(&src), sq_ptr_array_clear_func(&src));
		SQ_PTR_ARRAY_APPEND(this, src.data, src.length);
	}
	// move constructor
	UintptrArray(UintptrArray &&src) {
		data = src.data;
		length = src.length;
		src.data = NULL;
		src.length = 0;
	}
};

};  // namespace Sq

#endif  // __cplusplus

// ============================================================================
// SqStringArray is defined for compatibility with older versions
#include <SqStrArray.h>


#endif  // SQ_PTR_ARRAY_H
