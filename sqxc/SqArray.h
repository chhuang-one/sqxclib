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

#ifndef SQ_ARRAY_H
#define SQ_ARRAY_H

#ifdef __cplusplus
#include <type_traits>
#endif

#include <stddef.h>     // NULL
#include <stdint.h>     // uint8_t
#include <stdlib.h>     // qsort(), bsearch(), malloc(), free()
#include <string.h>     // memcpy(), memmove()

#include <SqDefine.h>   // SqClearFunc, SqCompareFunc

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqArrayHeader    SqArrayHeader;
typedef struct SqArray          SqArray;
typedef struct SqIntArray       SqIntArray;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* macro for accessing variable of SqArray */

#define sq_array_data(array)           \
		((SqArray*)(array))->data

#define sq_array_length(array)         \
		((SqArray*)(array))->length

#define sq_array_size(array)           \
		((SqArray*)(array))->length

#define sq_array_header(array)         \
		( (SqArrayHeader*) (((SqArray*)(array))->data - sizeof(SqArrayHeader)) )

#define sq_array_header_offsetof(HeaderField)    \
		( sizeof(SqArrayHeader) - offsetof(SqArrayHeader, HeaderField) )

#define sq_array_element_size(array)   \
		*(int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(element_size) )

#define sq_array_capacity(array)       \
		*(int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(capacity) )

#define sq_array_ref_count(array)      \
		*(int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(ref_count) )

#define sq_array_clear_func(array)     \
		*(SqClearFunc*)( ((SqArray*)(array))->data - sq_array_header_offsetof(clear_func) )

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_array_new(int element_size, int capacity);
#define sq_array_new(element_size, capacity)         \
		(SqArray*)sq_array_init(malloc(sizeof(SqArray)), element_size, capacity)

//void  sq_array_free(void *array);
#define sq_array_free(array)         free(sq_array_final(array))

//bool  sq_array_is_inited(void *array);
#define sq_array_is_inited(array)    (sq_array_data(array) != NULL)

//bool  sq_array_not_inited(void *array);
#define sq_array_not_inited(array)   (sq_array_data(array) == NULL)

//bool  sq_array_empty(void *array);
#define sq_array_empty(array)        (sq_array_length(array) == 0)

// Get address of element
//ElementType *sq_array_addr(void *array, ElementType, int index);
#define sq_array_addr(array, ElementType, index)  \
		( ((ElementType*)((SqArray*)(array))->data) + (index) )

//ElementType  sq_array_at(void *array, ElementType, int index);
#define sq_array_at(array, ElementType, index)    \
		*sq_array_addr(array, ElementType, index)

//void *sq_array_begin(void *array, ElementType);
#define sq_array_begin(array, ElementType)        \
		((ElementType*)((SqArray*)(array))->data)

//void *sq_array_end(void *array, ElementType);
#define sq_array_end(array, ElementType)          \
		( sq_array_begin(array, ElementType) + ((SqArray*)(array))->length )

/* macro for maintaining C/C++ inline functions easily */

//void *SQ_ARRAY_APPEND(void *array, ElementType, const void *values, int count);
#define SQ_ARRAY_APPEND(array, ElementType, values, count)           \
		memcpy(sq_array_alloc(array, count), values,                 \
		       sizeof(ElementType) * (count))

//void *SQ_ARRAY_INSERT(void *array, ElementType, int index, const void *values, int count);
#define SQ_ARRAY_INSERT(array, ElementType, index, values, count)    \
		memcpy(sq_array_alloc_at(array, index, count), values,       \
		       sizeof(ElementType) * (count))

// Removes a value from array without calling the destroy function.
// void SQ_ARRAY_STEAL(void *array, ElementType, int index, int count);
#define SQ_ARRAY_STEAL(array, ElementType, index, count)             \
		{                                                            \
		memmove(sq_array_data(array) + sizeof(ElementType) * (index),              \
		        sq_array_data(array) + sizeof(ElementType) * ((index)+(count)),    \
		        sizeof(ElementType) * (sq_array_length(array)-(index)-(count)) );  \
		((SqArray*)(array))->length -= (count);                      \
		}

// void SQ_ARRAY_STEAL_ADDR(void *array, ElementType, void **element_addr, int count);
#define SQ_ARRAY_STEAL_ADDR(array, ElementType, element_addr, count) \
		{                                                            \
		memmove(element_addr,                                        \
		        (ElementType*)(element_addr) + (count),              \
		        sizeof(ElementType) * (sq_array_length(array) -(count) -((ElementType*)(element_addr) - (ElementType*)sq_array_data(array))) );  \
		((SqArray*)(array))->length -= (count);                      \
		}

// Quick sort
// void SQ_ARRAY_SORT(void *array, ElementType, SqCompareFunc compare_func);
#define SQ_ARRAY_SORT(array, ElementType, compare_func)              \
		qsort( sq_array_data(array), ((SqArray*)(array))->length,    \
		       sizeof(ElementType), (SqCompareFunc)compare_func)

// Binary search for sorted array
//void *SQ_ARRAY_SEARCH(void *array, ElementType, const void *key, SqCompareFunc compare_func);
#define SQ_ARRAY_SEARCH(array, ElementType, key, compare_func)       \
		bsearch( (void*)(key),                                       \
		         sq_array_data(array), ((SqArray*)(array))->length,  \
		         sizeof(ElementType), (SqCompareFunc)compare_func)

/* C functions */

void *sq_array_init(void *array,
                    int   element_size,
                    int   capacity);

void *sq_array_final(void *array);

// Allocate space at the specified array location.
// It returns allocated address in the array.
void *sq_array_alloc_at(void *array, int index, int count);

//void *sq_array_alloc(void *array, int count)
#define sq_array_alloc(array, count)              \
		sq_array_alloc_at(array, sq_array_length(array), count)

// find element in unsorted array
void *sq_array_find(void *array,
                    const void *key,
                    SqCompareFunc cmpfunc);

// find element in sorted array and get index of element
void *sq_array_find_sorted(void *array,
                           const void *key,
                           SqCompareFunc compare,
                           int  *inserted_index);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	ArrayMethod is used by SqArray and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqArray members.
 */
template<class Type>
struct ArrayMethod
{
	// access variable
	int    elementSize();

	Type  *alloc(int length = 1);
	Type  *alloc(int index, int length);
	Type  *allocAt(int index, int length = 1);

	void   append(Type *values, int length = 1);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	void   append(ValueType  value);
	template <typename ValueType = Type,
	          typename std::enable_if<!std::is_arithmetic<ValueType>::value>::type * = nullptr>
	void   append(ValueType &value);

	void   insert(int index, Type *values, int length = 1);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	void   insert(int index, ValueType  value);
	template <typename ValueType = Type,
	          typename std::enable_if<!std::is_arithmetic<ValueType>::value>::type * = nullptr>
	void   insert(int index, ValueType &value);

	void   steal(int index, int length = 1);
	void   steal(Type *addr, int length = 1);

	// quick sort
	void   sort(SqCompareFunc func);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	void   sort();

	// binary search
	Type  *search(void *key, SqCompareFunc func);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	Type  *search(Type  key);

	// find element in unsorted array
	Type  *find(void *key, SqCompareFunc cmpfunc);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	Type  *find(Type  key);

	// find element in sorted array and get index of element
	Type  *findSorted(void *key, SqCompareFunc cmpfunc, int *inserted_index = NULL);
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	Type  *findSorted(Type  key, int *inserted_index = NULL);

	// get element address
	Type  *addr(int index);

	// SqCompareFunc - static function for sort(), search(), find(), findSorted()
	template <typename ValueType = Type,
	          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type * = nullptr>
	static int   compare(ValueType* a, ValueType* b);

	/* ArrayMethod iterator (uncompleted) */
	typedef Type        *iterator;
	typedef const Type  *const_iterator;

	int    size();
	int    capacity();
	void   reserve(int n);
	Type  *begin();
	Type  *end();
	Type  &at(int index);
	Type  &operator[](int index);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqArrayHeader is hidden in front of the array */

struct SqArrayHeader
{
	int   element_size;
	int   capacity;

	// reserve: ref_count, bit_field
	int   ref_count;
	int   int2;

	// reserve: callback functions
	void *clear_func;        // element clear function
	void *ptr;
};

/* SqArray is array of arbitrary elements */

#define SQ_ARRAY_MEMBERS(Type, data_name, length_name)   \
		Type     *data_name;    \
		int       length_name

#define SQ_ARRAY_STRUCT(Type)           \
		struct { SQ_ARRAY_MEMBERS(Type, data, length); }

#define SQ_ARRAY_TYPEDEF(Type, Name)    \
		typedef struct { SQ_ARRAY_MEMBERS(Type, data, length); } Name

struct SqArray
{
	SQ_ARRAY_MEMBERS(uint8_t, data, length);
/*	// ------ SqArray members ------
	uint8_t  *data;
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

/* define ArrayMethod template functions */

template<class Type>
inline int   ArrayMethod<Type>::elementSize() {
	return sq_array_element_size(this);
}

template<class Type>
inline Type *ArrayMethod<Type>::alloc(int length) {
	return (Type*) sq_array_alloc(this, length);
}
template<class Type>
inline Type *ArrayMethod<Type>::alloc(int index, int length) {
	return (Type*) sq_array_alloc_at(this, index, length);
}
template<class Type>
inline Type *ArrayMethod<Type>::allocAt(int index, int length) {
	return (Type*) sq_array_alloc_at(this, index, length);
}

template<class Type>
inline void  ArrayMethod<Type>::append(Type *values, int length) {
	SQ_ARRAY_APPEND(this, Type, values, length);
}
template <typename Type>
template <typename ValueType, typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::append(ValueType  value) {
	*(Type*)sq_array_alloc_at(this, sq_array_length(this), 1) = value;
}
template <typename Type>
template <typename ValueType, typename std::enable_if<!std::is_arithmetic<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::append(ValueType &value) {
	append(&value, 1);
}

template<class Type>
inline void  ArrayMethod<Type>::insert(int index, Type *values, int length) {
	SQ_ARRAY_INSERT(this, Type, index, values, length);
}
template <typename Type>
template <typename ValueType, typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::insert(int index, ValueType  value) {
	*(Type*)sq_array_alloc_at(this, index, 1) = value;
}
template <typename Type>
template <typename ValueType, typename std::enable_if<!std::is_arithmetic<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::insert(int index, ValueType &value) {
	insert(index, &value, 1);
}

template<class Type>
inline void  ArrayMethod<Type>::steal(int index, int length) {
	SQ_ARRAY_STEAL(this, Type, index, length);
}
template<class Type>
inline void  ArrayMethod<Type>::steal(Type *addr, int length) {
	SQ_ARRAY_STEAL_ADDR(this, Type, (void**)addr, length);
}

template<class Type>
inline void  ArrayMethod<Type>::sort(SqCompareFunc func) {
	SQ_ARRAY_SORT(this, Type, func);
}
template <typename Type>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::sort() {
	SQ_ARRAY_SORT(this, Type, (SqCompareFunc)ArrayMethod<Type>::compare<Type>);
}

template<class Type>
inline Type *ArrayMethod<Type>::search(void *key, SqCompareFunc func) {
	return (Type*) SQ_ARRAY_SEARCH(this, Type, key, func);
}
template <typename Type>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline Type *ArrayMethod<Type>::search(Type  key) {
	return (Type*) SQ_ARRAY_SEARCH(this, Type, &key, (SqCompareFunc)ArrayMethod<Type>::compare<Type>);
}

template<class Type>
inline Type  *ArrayMethod<Type>::find(void *key, SqCompareFunc cmpfunc) {
	return (Type*) sq_array_find(this, key, cmpfunc);
}
template <typename Type>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::find(Type  key) {
	return (Type*) sq_array_find(this, &key, (SqCompareFunc)ArrayMethod<Type>::compare<Type>);
}

template<class Type>
inline Type  *ArrayMethod<Type>::findSorted(void *key, SqCompareFunc cmpfunc, int *inserted_index) {
	return (Type*) sq_array_find_sorted(this, key, cmpfunc, inserted_index);
}
template <typename Type>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::findSorted(Type  key, int *inserted_index) {
	return (Type*) sq_array_find_sorted(this, &key, (SqCompareFunc)ArrayMethod<Type>::compare<Type>, inserted_index);
}

template<class Type>
inline Type *ArrayMethod<Type>::addr(int index) {
	return (Type*)sq_array_addr(this, Type, index);
}

// template specialization for ArrayMethod::compare
template <>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
int  ArrayMethod<double>::compare(ValueType* a, ValueType* b) {
	return (*a > *b) - (*a < *b);
}
template <>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
int  ArrayMethod<float>::compare(ValueType* a, ValueType* b) {
	return (*a > *b) - (*a < *b);
}
template <typename Type>
template <typename ValueType,
          typename std::enable_if<std::is_arithmetic<ValueType>::value>::type *>
int  ArrayMethod<Type>::compare(ValueType* a, ValueType* b) {
	return *a - *b;
}

/* ArrayMethod iterator (uncompleted) */

template<class Type>
inline int   ArrayMethod<Type>::size() {
	return sq_array_length(this);
}
template<class Type>
inline int   ArrayMethod<Type>::capacity() {
	return sq_array_capacity(this);
}
template<class Type>
inline void  ArrayMethod<Type>::reserve(int  n) {
	int  length = sq_array_length(this);
	if (n > length) {
		sq_array_alloc_at(this, length, n - length);
		sq_array_length(this) = length;
	}
}
template<class Type>
inline Type *ArrayMethod<Type>::begin() {
	return (Type*)sq_array_begin(this, Type);
}
template<class Type>
inline Type *ArrayMethod<Type>::end() {
	return (Type*)sq_array_end(this, Type);
}
template<class Type>
inline Type &ArrayMethod<Type>::at(int index) {
	return sq_array_at(this, Type, index);
}
template<class Type>
inline Type &ArrayMethod<Type>::operator[](int index) {
	return sq_array_at(this, Type, index);
}

/* All derived struct/class must be C++11 standard-layout. */

template<class Type>
struct Array : ArrayMethod<Type>                 // <-- 1. inherit C++ method
{
	SQ_ARRAY_MEMBERS(Type, data, length);        // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	Type     *data;
	int       length;
 */

	// constructor
	Array(int capacity = 8) {
		sq_array_init(this, sizeof(Type), capacity);
	}
	// destructor
	~Array(void) {
		sq_array_final(this);
	}
	// copy constructor
	Array(const Array &src) {
		sq_array_init(this, sizeof(Type), sq_array_capacity(&src));
		SQ_ARRAY_APPEND(this, Type, src.data, src.length);
	}
	// move constructor
	Array(Array &&src) {
		this->data   = src.data;
		this->length = src.length;
		src.data   = NULL;
		src.length = 0;
	}
};

/* Sq::IntArray */
typedef Array<int>    IntArray;

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// SqIntArray ( equal Sq::Array<int> )

#define sq_int_array_capacity            sq_array_capacity

//void *sq_int_array_init(void *array, int capacity);
#define sq_int_array_init(array, capacity)    \
		sq_array_init(array, sizeof(int), capacity)

//void *sq_int_array_final(void *array);
#define sq_int_array_final               sq_array_final

// int  sq_int_array_at(void *array, int index);
#define sq_int_array_at(array, index)    sq_array_at(array, int, index)

// int *sq_int_array_begin(void *array);
#define sq_int_array_begin(array)        sq_array_begin(array, int)

// int *sq_int_array_end(void *array);
#define sq_int_array_end(array)          sq_array_end(array, int)

// int *sq_int_array_alloc(void *array, int count);
#define sq_int_array_alloc               (int*)sq_array_alloc

// int *sq_int_array_alloc_at(void *array, int index, int count);
#define sq_int_array_alloc_at            (int*)sq_array_alloc_at

// int *sq_int_array_append(void *array, const int *values, int count);
#define sq_int_array_append(array, values, count)    \
		(int*)SQ_ARRAY_APPEND(array, int, values, count)

// int *sq_int_array_insert(void *array, int index, const int *values, int count);
#define sq_int_array_insert(array, index, values, count)    \
		(int*)SQ_ARRAY_INSERT(array, int, index, values, count)

/*
	SqArray
	|
	+--- SqIntArray  ( equal Sq::Array<int> )
 */

#ifdef __cplusplus
struct SqIntArray : Sq::ArrayMethod<int>         // <-- 1. inherit C++ method
#else
struct SqIntArray
#endif
{
	SQ_ARRAY_MEMBERS(int, data, length);         // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	int      *data;
	int       length;
 */
};


#endif  // SQ_ARRAY_H
