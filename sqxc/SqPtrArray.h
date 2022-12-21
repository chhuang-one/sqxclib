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

#ifndef SQ_PTR_ARRAY_H
#define SQ_PTR_ARRAY_H

#ifdef __cplusplus
#include <functional>
#endif

#include <stddef.h>     // NULL
#include <stdint.h>
#include <stdlib.h>     // qsort(), bsearch(), malloc(), free()
#include <string.h>

#include <SqDefine.h>

// default
#define SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT     3

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqPtrArray       SqPtrArray;
typedef struct SqIntptrArray    SqIntptrArray;
typedef struct SqUintptrArray   SqUintptrArray;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/*	macro for accessing variable of array

	Header field of SqPtrArray
	ptr_array->data[-1]: (intptr_t) header length
	ptr_array->data[-2]: (intptr_t) allocated length
	ptr_array->data[-3]: (SqDestroyFunc*) destroy function
 */

#define sq_ptr_array_data(array)       \
		((SqPtrArray*)(array))->data

#define sq_ptr_array_length(array)     \
		((SqPtrArray*)(array))->length

#define sq_ptr_array_header(array)     \
		(((SqPtrArray*)(array))->data - (intptr_t)((SqPtrArray*)(array))->data[-1])

#define sq_ptr_array_header_length(array)  \
		*(intptr_t*)(((SqPtrArray*)(array))->data -1)

#define sq_ptr_array_allocated(array)  \
		*(intptr_t*)(((SqPtrArray*)(array))->data -2)

#define sq_ptr_array_capacity(array)   \
		*(intptr_t*)(((SqPtrArray*)(array))->data -2) 

#define sq_ptr_array_destroy_func(array)   \
		*(SqDestroyFunc*)(((SqPtrArray*)(array))->data -3)

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void  sq_ptr_array_init(void *array, int allocated_length, SqDestroyFunc destroy_func);
#define sq_ptr_array_init(array, allocated_length, destroy_func)  \
		sq_ptr_array_init_full(array, allocated_length, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, destroy_func)

//void *sq_ptr_array_new(int allocated_length, SqDestroyFunc destroy_func);
#define sq_ptr_array_new(allocated_length, destroy_func)  \
		sq_ptr_array_init_full(NULL, allocated_length, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, destroy_func)

//void  sq_ptr_array_free(void *array);
#define sq_ptr_array_free(array)    free(sq_ptr_array_final(array))

//bool  sq_ptr_array_is_inited(void *array);
#define sq_ptr_array_is_inited(array)    (sq_ptr_array_data(array) != NULL)

//bool  sq_ptr_array_not_inited(void *array);
#define sq_ptr_array_not_inited(array)   (sq_ptr_array_data(array) == NULL)

//void **sq_ptr_array_alloc(void *array, int count)
#define sq_ptr_array_alloc(array, count)               \
		sq_ptr_array_alloc_at(array, sq_ptr_array_length(array), count)

// void sq_ptr_array_insert(void *array, int index, void *value);
#define sq_ptr_array_insert(array, index, value)  \
		*sq_ptr_array_alloc_at(array, index, 1) = (void*)(value)

// void sq_ptr_array_append(void *array, void *value);
#define sq_ptr_array_append(array, value)  \
		*sq_ptr_array_alloc_at(array, sq_ptr_array_length(array), 1) = (void*)(value)

// void sq_ptr_array_erase_addr(void *array, void **element_addr, int count);
#define sq_ptr_array_erase_addr(array, element_addr, count)    \
		sq_ptr_array_erase(array, (int)((void**)(element_addr) - sq_ptr_array_data(array)), count)

// bool sq_ptr_array_empty(void *array)
#define sq_ptr_array_empty(array)  \
		(((SqPtrArray*)(array))->data == NULL)

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

// Quick sort
// void sq_ptr_array_sort(void *array, SqCompareFunc compare_func);
#define sq_ptr_array_sort(array, compare_func)   \
		qsort( ((SqPtrArray*)(array))->data, ((SqPtrArray*)(array))->length,  \
		       sizeof(void*), (SqCompareFunc)compare_func)

// Binary search for sorted array
//void *sq_ptr_array_search(void *array, const void *key, SqCompareFunc compare_func);
#define sq_ptr_array_search(array, key, compare_func)   \
		bsearch( (void*)(key),                          \
		         ((SqPtrArray*)(array))->data, ((SqPtrArray*)(array))->length,  \
		         sizeof(void*), (SqCompareFunc)compare_func)

// void sq_ptr_array_foreach(void *array, void *element)
#define sq_ptr_array_foreach(array, element)                    \
		for (void **element##_end  = sq_ptr_array_end(array),   \
		          **element##_addr = sq_ptr_array_begin(array), \
		           *element = *element##_addr;                  \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

// void sq_ptr_array_foreach_addr(void *array, void **element_addr)
#define sq_ptr_array_foreach_addr(array, element_addr)              \
		for (void **element_addr##_end = sq_ptr_array_end(array),   \
		          **element_addr       = sq_ptr_array_begin(array); \
		     element_addr < element_addr##_end;                     \
		     element_addr++)

/* macro for maintaining C/C++ inline functions easily */

//void *sq_ptr_array_insert_n(void *array, int index, const void *values, int count);
#define SQ_PTR_ARRAY_INSERT_N(array, index, values, count)    \
		memcpy(sq_ptr_array_alloc_at(array, index, count),    \
		       values, sizeof(void*) * (count))

//void *sq_ptr_array_append_n(void *array, const void *values, int count);
#define SQ_PTR_ARRAY_APPEND_N(array, values, count)   \
		memcpy(sq_ptr_array_alloc(array, count),      \
		       values, sizeof(void*) * (count))

// Removes a value from array without calling the destroy function.
// void sq_ptr_array_steal(void *array, int index, int count);
#define SQ_PTR_ARRAY_STEAL(array, index, count)                \
		{                                                      \
		memmove(sq_ptr_array_addr(array, (index)),             \
		        sq_ptr_array_addr(array, (index) + (count)),   \
		        sizeof(void*) * (sq_ptr_array_length(array) -(count) -(index)) );  \
		((SqPtrArray*)(array))->length -= (count);             \
		}

// void sq_ptr_array_steal_addr(void *array, void **element_addr, int count);
#define SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count)      \
		{                                                        \
		memmove(element_addr, (void**)(element_addr) + (count),  \
		        sizeof(void*) * (sq_ptr_array_length(array) -(count) -((void**)(element_addr) - sq_ptr_array_data(array))) );  \
		((SqPtrArray*)(array))->length -= (count);               \
		}

/* macro for SqIntptrArray */

#define sq_intptr_array_length(array)    \
		((SqIntptrArray*)(array))->length

#define sq_intptr_array_allocated(array)  \
		*(intptr_t*)(((SqIntptrArray*)(array))->data -2)

#define sq_intptr_array_capacity(array)  \
		*(intptr_t*)(((SqIntptrArray*)(array))->data -2)

//void  sq_intptr_array_init(void *array, int allocated_length);
#define sq_intptr_array_init(array, allocated_length)    \
		sq_ptr_array_init_full(array, allocated_length, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, NULL)

//void *sq_intptr_array_final(void *array);
#define sq_intptr_array_final       sq_ptr_array_final

// intptr_t *sq_intptr_array_alloc(void *array, int count)
#define sq_intptr_array_alloc       sq_ptr_array_alloc

// intptr_t *sq_intptr_array_alloc_at(void *array, int index, int count)
#define sq_intptr_array_alloc_at    sq_ptr_array_alloc_at

// void sq_intptr_array_insert(void *array, int index, void *value);
#define sq_intptr_array_insert(array, index, value)  \
		*sq_ptr_array_alloc_at(array, index, 1) = (void*)(value)

// void sq_intptr_array_insert_n(void *array, int index, const intptr_t *values, int count);
#define sq_intptr_array_insert_n    sq_ptr_array_insert_n

// void sq_intptr_array_append(void *array, void *value);
#define sq_intptr_array_append(array, value)  \
		*sq_ptr_array_alloc_at(array, sq_intptr_array_length(array), 1) = (void*)(value)

// void sq_intptr_array_append_n(void *array, const intptr_t *values, int count);
#define sq_intptr_array_append_n    sq_ptr_array_append_n

// void sq_intptr_array_erase(void *array, int index, int count);
#define sq_intptr_array_erase       sq_ptr_array_erase

// void sq_intptr_array_foreach(void *array, intptr_t element)
#define sq_intptr_array_foreach(array, element)                \
		for (intptr_t *element##_end  = (intptr_t*)sq_ptr_array_end(array),   \
		              *element##_addr = (intptr_t*)sq_ptr_array_begin(array), \
		               element        = *element##_addr;       \
		     element##_addr < element##_end;                   \
		     element##_addr++, element = *element##_addr)

/* macro for SqUintptrArray */

// void sq_uintptr_array_foreach(void *array, uintptr_t element)
#define sq_uintptr_array_foreach(array, element)                \
		for (uintptr_t *element##_end  = (uintptr_t*)sq_ptr_array_end(array),   \
		               *element##_addr = (uintptr_t*)sq_ptr_array_begin(array), \
		                element        = *element##_addr;       \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

/* C functions */

void  *sq_ptr_array_init_full(void *array,
                              int allocated_length, int header_length,
                              SqDestroyFunc  destroy_func);

void  *sq_ptr_array_final(void *array);

void **sq_ptr_array_alloc_at(void *array, int index, int count);

void   sq_ptr_array_erase(void *array, int index, int count);

// find element in unsorted array
void **sq_ptr_array_find(void *array, const void *key, SqCompareFunc cmpfunc);

void **sq_ptr_array_find_sorted(void *array, const void *key,
                                SqCompareFunc compare, int *inserted_index);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	PtrArrayMethod is used by SqPtrArray and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqPtrArray members.
 */
template<class Type>
struct PtrArrayMethod
{
	// access variable of PtrArray
	int    headerLength();
	int    allocated();
	SqDestroyFunc destroyFunc();
	void          destroyFunc(SqDestroyFunc func);

	// member functions
	void   init(int allocated_length = 0, SqDestroyFunc func = NULL);
	void   final(void);

	bool   isInited(void);
	bool   notInited(void);

	Type **alloc(int length);
	Type **alloc(int index, int length);

	void   insert(int index, Type *values, int length = 1);
	void   insert(int index, Type  value);
	void   append(Type *values, int length = 1);
	void   append(Type  value);
	void   erase(int index, int length = 1);
	void   erase(Type *addr, int length = 1);
	void   steal(int index, int length = 1);
	void   steal(Type *addr, int length = 1);

	// quick sort
	void   sort(SqCompareFunc func);
	// binary search
	Type  *search(Type key, SqCompareFunc func);
	// find unsorted
	Type  *find(Type key, SqCompareFunc func);
	// find sorted (binary search)
	Type  *findSorted(Type key, SqCompareFunc func, int *insertedIndex = NULL);

	// foreach
	void   foreach(std::function<void(Type  element)> func);
	void   foreach(std::function<void(Type *address)> func);

	Type  *addr(int index);

	/* PtrArrayMethod iterator (uncompleted) */

	typedef Type        *iterator;
	typedef const Type  *const_iterator;

	int    size();
	int    capacity();
	void   reserve(int n);
	Type  *begin();
	Type  *end();
	Type   at(int index);
	Type   operator[](int index);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqPtrArray is array of pointer */

#define SQ_PTR_ARRAY_MEMBERS(Type, data_name, length_name)   \
		Type     *data_name;    \
		int       length_name

#ifdef __cplusplus
struct SqPtrArray : Sq::PtrArrayMethod<void*>        // <-- 1. inherit C++ member function(method)
#else
struct SqPtrArray
#endif
{
	SQ_PTR_ARRAY_MEMBERS(void*, data, length);       // <-- 2. inherit member variable
/*	// ------ SqPtrArray members ------
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
	SQ_PTR_ARRAY_MEMBERS(intptr_t, data, length);    // <-- 2. inherit member variable
/*	// ------ SqPtrArray members ------
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
	SQ_PTR_ARRAY_MEMBERS(uintptr_t, data, length);       // <-- 2. inherit member variable
/*	// ------ SqPtrArray members ------
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

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_insert_n(void *array, int index, const void *values, int count)
{
	SQ_PTR_ARRAY_INSERT_N(array, index, values, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_append_n(void *array, const void *values, int count)
{
	SQ_PTR_ARRAY_APPEND_N(array, values, count);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

void  sq_ptr_array_steal(void *array, int index, int count);
void  sq_ptr_array_steal_addr(void *array, void **element_addr, int count);
void  sq_ptr_array_insert_n(void *array, int index, const void *values, int count);
void  sq_ptr_array_append_n(void *array, const void *values, int count);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define PtrArrayMethod template functions */

template<class Type>
inline int   PtrArrayMethod<Type>::headerLength() {
	return sq_ptr_array_header_length(this);
}
template<class Type>
inline int   PtrArrayMethod<Type>::allocated() {
	return sq_ptr_array_allocated(this);
}
template<class Type>
inline SqDestroyFunc  PtrArrayMethod<Type>::destroyFunc() {
	return sq_ptr_array_destroy_func(this);
}
template<class Type>
inline void  PtrArrayMethod<Type>::destroyFunc(SqDestroyFunc func) {
	sq_ptr_array_destroy_func(this) = func;
}

template<class Type>
inline void  PtrArrayMethod<Type>::init(int allocated_length, SqDestroyFunc func) {
	sq_ptr_array_init_full(this, allocated_length, SQ_PTR_ARRAY_HEADER_LENGTH_DEFAULT, func);
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
inline Type **PtrArrayMethod<Type>::alloc(int length) {
	return (Type**) sq_ptr_array_alloc(this, length);
}
template<class Type>
inline Type **PtrArrayMethod<Type>::alloc(int index, int length) {
	return (Type**) sq_ptr_array_alloc_at(this, index, length);
}

template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type *values, int length) {
	SQ_PTR_ARRAY_INSERT_N(this, index, values, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type  value) {
	sq_ptr_array_insert(this, index, value);
}

template<class Type>
inline void  PtrArrayMethod<Type>::append(Type *values, int length) {
	SQ_PTR_ARRAY_APPEND_N(this, values, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::append(Type  value) {
	sq_ptr_array_append(this, value);
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
inline void  PtrArrayMethod<Type>::steal(int index, int length) {
	sq_ptr_array_steal(this, index, length);
}
template<class Type>
inline void  PtrArrayMethod<Type>::steal(Type *addr, int length) {
	sq_ptr_array_steal_addr(this, (void**)addr, length);
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

template<class Type>
inline void  PtrArrayMethod<Type>::sort(SqCompareFunc func) {
	sq_ptr_array_sort(this, func);
}
template<class Type>
inline Type *PtrArrayMethod<Type>::search(Type key, SqCompareFunc func) {
	return (Type*)sq_ptr_array_search(this, &key, func);
}
template<class Type>
inline Type *PtrArrayMethod<Type>::find(Type key, SqCompareFunc func) {
	return (Type*)sq_ptr_array_find(this, &key, func);
}
template<class Type>
inline Type *PtrArrayMethod<Type>::findSorted(Type key, SqCompareFunc func, int *insertedIndex) {
	return (Type*)sq_ptr_array_find_sorted(this, &key, func, insertedIndex);
}
template<class Type>
inline Type *PtrArrayMethod<Type>::addr(int index) {
	return (Type*)sq_ptr_array_addr(this, index);
}

/* PtrArrayMethod iterator (uncompleted) */

template<class Type>
inline int   PtrArrayMethod<Type>::size() {
	return sq_ptr_array_length(this);
}
template<class Type>
inline int   PtrArrayMethod<Type>::capacity() {
	return sq_ptr_array_capacity(this);
}
template<class Type>
inline void  PtrArrayMethod<Type>::reserve(int  n) {
	int  length = sq_ptr_array_length(this);
	if (n > length) {
		sq_ptr_array_alloc_at(this, length, n - length);
		sq_ptr_array_length(this) = length;
	}
}
template<class Type>
inline Type *PtrArrayMethod<Type>::begin() {
	return (Type*)sq_ptr_array_begin(this);
}
template<class Type>
inline Type *PtrArrayMethod<Type>::end() {
	return (Type*)sq_ptr_array_end(this);
}
template<class Type>
inline Type  PtrArrayMethod<Type>::at(int index) {
	return (Type)sq_ptr_array_at(this, index);
}
template<class Type>
inline Type  PtrArrayMethod<Type>::operator[](int index) {
	return (Type)sq_ptr_array_at(this, index);
}

/* All derived struct/class must be C++11 standard-layout. */

struct PtrArray : SqPtrArray
{
	// constructor
	PtrArray(int allocated_length = 0, SqDestroyFunc func = NULL) {
		sq_ptr_array_init(this, allocated_length, func);
	}
	// destructor
	~PtrArray(void) {
		sq_ptr_array_final(this);
	}
	// copy constructor
	PtrArray(SqPtrArray &src) {
		SQ_PTR_ARRAY_APPEND_N(this, src.data, src.length);
//		sq_ptr_array_destroy_func(this) = sq_ptr_array_destroy_func(&src);
	}
	// move constructor
	PtrArray(SqPtrArray &&src) {
		this->data = src.data;
		this->length = src.length;
		src.data = NULL;
		src.length = 0;
	}
};

struct IntptrArray : SqIntptrArray
{
	// constructor
	IntptrArray(int allocated_length = 0) {
		sq_ptr_array_init(this, allocated_length, NULL);
	}
	// destructor
	~IntptrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	IntptrArray(IntptrArray &src) {
		SQ_PTR_ARRAY_APPEND_N(this, src.data, src.length);
	}
	// move constructor
	IntptrArray(IntptrArray &&src) {
		data = src.data;
		length = src.length;
		src.data = NULL;
		src.length = 0;
	}
};

struct UintptrArray : SqUintptrArray
{
	// constructor
	UintptrArray(int allocated_length = 0) {
		sq_ptr_array_init(this, allocated_length, NULL);
	}
	// destructor
	~UintptrArray() {
		sq_ptr_array_final(this);
	}
	// copy constructor
	UintptrArray(UintptrArray &src) {
		SQ_PTR_ARRAY_APPEND_N(this, src.data, src.length);
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
