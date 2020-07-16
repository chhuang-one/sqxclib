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

#ifdef __cplusplus
extern "C" {
#endif

// default length of header
#define SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH     3

// ----------------------------------------------------------------------------
// macro for accessing variable of array

#define sq_ptr_array_data(array)    \
		((SqPtrArray*)(array))->data

#define sq_ptr_array_length(array)    \
		((SqPtrArray*)(array))->length

#define sq_ptr_array_header(array)    \
		(((SqPtrArray*)(array))->data - (intptr_t)((SqPtrArray*)(array))->data[-1])

#define sq_ptr_array_header_length(array)  \
		((intptr_t*)((SqPtrArray*)(array))->data)[-1]

#define sq_ptr_array_allocated(array)  \
		((intptr_t*)((SqPtrArray*)(array))->data)[-2]

#define sq_ptr_array_destroy_func(array)  \
		((SqDestroyFunc*)((SqPtrArray*)(array))->data)[-3]

// ----------------------------------------------------------------------------
// macro functions - parameter used only once in macro (except parameter 'array')

//void  sq_ptr_array_init(void* array, int allocated_length, SqDestroyFunc func);
#define sq_ptr_array_init(array, allocated_length, func)  \
		sq_ptr_array_init_full(array, allocated_length, SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH, func)

//void* sq_ptr_array_new(int allocated_length, SqDestroyFunc func);
#define sq_ptr_array_new(allocated_length, func)  \
		sq_ptr_array_init_full(NULL, allocated_length, SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH, func)

//void  sq_ptr_array_free(void* array);
#define sq_ptr_array_free(array)    free(sq_ptr_array_final(array))

//void** sq_ptr_array_alloc(void* array, int count)
#define sq_ptr_array_alloc(array, count)               \
		sq_ptr_array_alloc_at(array, sq_ptr_array_length(array), count)

// void sq_ptr_array_insert(void* array, int index, void* value);
#define sq_ptr_array_insert(array, index, value)  \
		*sq_ptr_array_alloc_at(array, index, 1) = (value)

// void sq_ptr_array_append(void* array, void* value);
#define sq_ptr_array_append(array, value)  \
		*sq_ptr_array_alloc_at(array, sq_ptr_array_length(array), 1) = (value)

// bool sq_ptr_array_empty(void* array)
#define sq_ptr_array_empty(array)  \
		(((SqPtrArray*)(array))->data == NULL)

// Get address of element
//void** sq_ptr_array_addr(void* array, int index);
#define sq_ptr_array_addr(array, index)  \
		( ((SqPtrArray*)(array))->data + (index) )

//void* sq_ptr_array_at(void* array, int index);
#define sq_ptr_array_at(array, index)    \
		*( ((SqPtrArray*)(array))->data + (index) )

//void* sq_ptr_array_begin(void* array);
#define sq_ptr_array_begin(array)  \
		(((SqPtrArray*)(array))->data)

//void* sq_ptr_array_end(void* array);
#define sq_ptr_array_end(array)    \
		( (((SqPtrArray*)(array))->data) + ((SqPtrArray*)(array))->length )

// Quick sort
// void sq_ptr_array_sort(void* array, SqCompareFunc compare_func);
#define sq_ptr_array_sort(array, compare_func)   \
		qsort( ((SqPtrArray*)(array))->data, ((SqPtrArray*)(array))->length,  \
		       sizeof(void*), (SqCompareFunc)compare_func)

// Binary search for sorted array
//void* sq_ptr_array_search(void* array, const void* key, SqCompareFunc compare_func);
#define sq_ptr_array_search(array, key, compare_func)   \
		bsearch( (void*)(key),                          \
		         ((SqPtrArray*)(array))->data, ((SqPtrArray*)(array))->length,  \
		         sizeof(void*), (SqCompareFunc)compare_func)

// void sq_ptr_array_foreach(void* array, void* element)
#define sq_ptr_array_foreach(array, element)                    \
		for (void **element##_end  = sq_ptr_array_end(array),   \
		          **element##_addr = sq_ptr_array_begin(array), \
		           *element = *element##_addr;                  \
		     element##_addr < element##_end;                    \
		     element##_addr++, element = *element##_addr)

// void sq_ptr_array_foreach_addr(void* array, void** element_addr)
#define sq_ptr_array_foreach_addr(array, element_addr)              \
		for (void **element_addr##_end = sq_ptr_array_end(array),   \
		          **element_addr       = sq_ptr_array_begin(array); \
		     element_addr < element_addr##_end;                     \
		     element_addr++)

// void sq_intptr_array_foreach(void* array, intptr_t* element_addr)
#define sq_intptr_array_foreach(array, element)                \
		for (intptr_t *element##_end  = (intptr_t*)sq_ptr_array_end(array),   \
		              *element##_addr = (intptr_t*)sq_ptr_array_begin(array), \
		               element        = *element##_addr;       \
		     element##_addr < element_addr##_end;              \
		     element##_addr++, element = *element##_addr)

// void sq_uintptr_array_foreach(void* array, uintptr_t* element_addr)
#define sq_uintptr_array_foreach(array, element)                \
		for (uintptr_t *element##_end  = (uintptr_t*)sq_ptr_array_end(array),   \
		               *element##_addr = (uintptr_t*)sq_ptr_array_begin(array), \
		                element        = *element##_addr;       \
		     element##_addr < element_addr##_end;              \
		     element##_addr++, element = *element##_addr)

// ----------------------------------------------------------------------------
// macro for maintaining C/C++ inline functions easily

//void* sq_ptr_array_insert_n(void* array, int index, const void* values, int count);
#define SQ_PTR_ARRAY_INSERT_N(array, index, values, count)    \
		memcpy(sq_ptr_array_alloc_at(array, index, count),    \
		       values, sizeof(void*) * (count))

//void* sq_ptr_array_append_n(void* array, const void* values, int count);
#define SQ_PTR_ARRAY_APPEND_N(array, values, count)   \
		memcpy(sq_ptr_array_alloc(array, count),      \
		       values, sizeof(void*) * (count))

// void sq_ptr_array_erase(void* array, int index, int count);
#define SQ_PTR_ARRAY_ERASE(array, index, count)                \
		{                                                      \
		SqDestroyFunc  destroy_ = sq_ptr_array_destroy_func(array);                 \
		if (destroy_)  destroy_(sq_ptr_array_at(array, index));                     \
		memmove(sq_ptr_array_addr(array, (index)),             \
		        sq_ptr_array_addr(array, (index) + (count)),   \
		        sizeof(void*) * (sq_ptr_array_length(array) -(count) -(index)) );  \
		((SqPtrArray*)(array))->length -= (count);             \
		}

// Removes a value from array without calling the destroy function.
// void sq_ptr_array_steal(void* array, int index, int count);
#define SQ_PTR_ARRAY_STEAL(array, index, count)                \
		{                                                      \
		memmove(sq_ptr_array_addr(array, (index)),             \
		        sq_ptr_array_addr(array, (index) + (count)),   \
		        sizeof(void*) * (sq_ptr_array_length(array) -(count) -(index)) );  \
		((SqPtrArray*)(array))->length -= (count);             \
		}

// ----------------------------------------------------------------------------
// C functions

void*  sq_ptr_array_init_full(void* array,
                              int allocated_length, int header_length,
                              SqDestroyFunc  destroy_func);

void*  sq_ptr_array_final(void* array);

void** sq_ptr_array_alloc_at(void* array, int index, int count);

// find element in unsorted array
void** sq_ptr_array_find(void* array, const void* key, SqCompareFunc cmpfunc);

// remove NULL pointer in array
void   sq_ptr_array_remove_null(void* array);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// PtrArrayMethod : a template C++ struct is used by SqPtrArray and it's children.

#ifdef __cplusplus

namespace Sq
{

// This one is for derived use only, no data members here.
// This one is NOT for directly use, it must has SqPtrArray data members.
// Your derived struct/class must be C++11 standard-layout.
template<class Type>
struct PtrArrayMethod
{
	// access variable of PtrArray
	int    allocated();
	Type*  addr(int index);

	// member functions
	void   init(int allocated_length = 0, SqDestroyFunc func = NULL);
	void   final(void);
	Type** alloc(int length);
	Type** alloc(int index, int length);

	void   insert(int index, Type* values, int length = 1);
	void   insert(int index, Type  value);
	void   append(Type* values, int length = 1);
	void   append(Type  value);
	void   erase(int index, int length = 1);

	// quick sort
	void   sort(SqCompareFunc func);
	// binary search
	Type*  search(Type key, SqCompareFunc func);
	// find unsorted
	Type*  find(Type key, SqCompareFunc func);

	// foreach
	void   foreach(std::function<void(Type  element)> func);
	void   foreach(std::function<void(Type* address)> func);

	// ----------------------------------------------------
	// C++

	typedef Type*        iterator;
	typedef const Type*  const_iterator;

	int    size();
	Type*  begin();
	Type*  end();
    Type&  at(int index);
    Type&  operator[](int index);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// SqPtrArray - each size of members in PtrArray must be the same

#define SQ_PTR_ARRAY_MEMBERS(Type, data_name, length_name)   \
		Type*     data_name;    \
		int       length_name

#ifdef __cplusplus

// C++ template works with C macro
template<class Type>
struct SqPtrArrayTemplate : Sq::PtrArrayMethod<Type>
{
	SQ_PTR_ARRAY_MEMBERS(Type, data, length);
/*	// ------ SqPtrArray members ------
	void**         data;
	int            length;
 */
};
#define SQ_PTR_ARRAY(Type)     struct SqPtrArrayTemplate<Type>

#else  // __cplusplus

// implement template by C macro
#define SQ_PTR_ARRAY(Type)     struct { SQ_PTR_ARRAY_MEMBERS(Type, data, length); }

#endif  // __cplusplus

typedef SQ_PTR_ARRAY(void*)      SqPtrArray;
typedef SQ_PTR_ARRAY(intptr_t)   SqIntptrArray;

// ----------------------------------------------------------------------------
// C/C++ inline functions

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// C99 or C++ inline functions

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_erase(void* array, int index, int count)
{
	SQ_PTR_ARRAY_ERASE(array, index, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_steal(void* array, int index, int count)
{
	SQ_PTR_ARRAY_STEAL(array, index, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_insert_n(void* array, int index, const void* values, int count)
{
	SQ_PTR_ARRAY_INSERT_N(array, index, values, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_ptr_array_append_n(void* array, const void* values, int count)
{
	SQ_PTR_ARRAY_APPEND_N(array, values, count);
}

#else  // __STDC_VERSION__ || __cplusplus

// C functions
void  sq_ptr_array_erase(void* array, int index, int count);
void  sq_ptr_array_steal(void* array, int index, int count);
void  sq_ptr_array_insert_n(void* array, int index, const void* values, int count);
void  sq_ptr_array_append_n(void* array, const void* values, int count);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// Sq::Array and it's C++ template 

#ifdef __cplusplus

namespace Sq
{

// ----------------------------------------------------------------------------
// PtrArrayMethod template functions

template<class Type>
inline Type* PtrArrayMethod<Type>::addr(int index)
	{ return (Type*)sq_ptr_array_addr(this, index); }

template<class Type>
inline void  PtrArrayMethod<Type>::init(int allocated_length, SqDestroyFunc func)
	{ sq_ptr_array_init_full(this, allocated_length, SQ_PTR_ARRAY_HEADER_DEFAULT_LENGTH, func); }
template<class Type>
inline void  PtrArrayMethod<Type>::final(void)
	{ sq_ptr_array_final(this); }
template<class Type>
inline Type** PtrArrayMethod<Type>::alloc(int length)
	{ return (Type**) sq_ptr_array_alloc(this, length); }
template<class Type>
inline Type** PtrArrayMethod<Type>::alloc(int index, int length)
	{ return (Type**) sq_ptr_array_alloc_at(this, index, length); }

template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type* values, int length)
	{ SQ_PTR_ARRAY_INSERT_N(this, index, values, length); }
template<class Type>
inline void  PtrArrayMethod<Type>::insert(int index, Type  value)
	{ sq_ptr_array_insert(this, index, value); }

template<class Type>
inline void  PtrArrayMethod<Type>::append(Type* values, int length)
	{ SQ_PTR_ARRAY_APPEND_N(this, values, length); }
template<class Type>
inline void  PtrArrayMethod<Type>::append(Type  value)
	{ sq_ptr_array_append(this, value); }
template<class Type>
inline void  PtrArrayMethod<Type>::erase(int index, int length)
	{ sq_ptr_array_erase(this, index, length); }

template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type  element)> func)
	{ sq_ptr_array_foreach(this, element) { func(element); } }
template<class Type>
inline void  PtrArrayMethod<Type>::foreach(std::function<void(Type* address)> func)
	{ sq_ptr_array_foreach_addr(this, address) { func(address); } }

template<class Type>
inline void  PtrArrayMethod<Type>::sort(SqCompareFunc func)
	{ sq_ptr_array_sort(this, func); }
template<class Type>
inline Type* PtrArrayMethod<Type>::search(Type key, SqCompareFunc func)
	{ return (Type*)sq_ptr_array_search(this, &key, func); }
template<class Type>
inline Type* PtrArrayMethod<Type>::find(Type key, SqCompareFunc func)
	{ return (Type*)sq_ptr_array_find(this, &key, func); }

// --------------------------------------------------------
// PtrArrayMethod iterator

template<class Type>
inline int   PtrArrayMethod<Type>::size()
	{ return sq_ptr_array_length(this); }
template<class Type>
inline Type* PtrArrayMethod<Type>::begin()
	{ return (Type*)sq_ptr_array_begin(this); }
template<class Type>
inline Type* PtrArrayMethod<Type>::end()
	{ return (Type*)sq_ptr_array_end(this); }
template<class Type>
inline Type& PtrArrayMethod<Type>::at(int index)
	{ return (Type)sq_ptr_array_at(this, index); }
template<class Type>
inline Type& PtrArrayMethod<Type>::operator[](int index)
	{ return (Type)sq_ptr_array_at(this, index); }

// ----------------------------------------------------------------------------
// PtrArray is C++11 standard-layout

// This one is for directly use only. You can NOT derived it.
template<class Type> struct PtrArray : SqPtrArrayTemplate<Type>
{
	PtrArray<Type>(int allocated_length = 0, SqDestroyFunc func = NULL)
		{ sq_ptr_array_init(this, allocated_length, func); }
	~PtrArray<Type>(void)
		{ sq_ptr_array_final(this); }
};

struct IntptrArray : SqPtrArrayTemplate<intptr_t>
{
	IntptrArray(int allocated_length = 0, SqDestroyFunc func = NULL)
		{ sq_ptr_array_init(this, allocated_length, func); }
	~IntptrArray(void)
		{ sq_ptr_array_final(this); }
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_PTR_ARRAY_H
