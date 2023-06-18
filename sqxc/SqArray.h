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

//void *sq_array_new(int elementSize, int capacity);
#define sq_array_new(elementSize, capacity)       \
		(SqArray*)sq_array_init(malloc(sizeof(SqArray)), elementSize, capacity)

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

// void sq_array_push(void *array, ElementType, ElementType value);
#define sq_array_push(array, ElementType, value)              \
		*(ElementType*)sq_array_alloc(array, 1) = (ElementType)(value)

// void sq_array_push_to(void *array, ElementType, int index, ElementType value);
#define sq_array_push_to(array, ElementType, index, value)    \
		*(ElementType*)sq_array_alloc_at(array, index, 1) = (ElementType)(value)

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
		memmove(sq_array_data(array) + sizeof(ElementType) * (index),              \
		        sq_array_data(array) + sizeof(ElementType) * ((index)+(count)),    \
		        sizeof(ElementType) * ((sq_array_length(array) -= (count)) - (index)) )

// void SQ_ARRAY_STEAL_ADDR(void *array, ElementType, ElementType *elementAddr, int count);
#define SQ_ARRAY_STEAL_ADDR(array, ElementType, elementAddr, count)  \
		memmove(elementAddr,                                         \
		        (ElementType*)(elementAddr) + (count),               \
		        sizeof(ElementType) * (sq_array_length(array) -= (count)) - ((uint8_t*)(elementAddr) - sq_array_data(array)) )

// Quick sort
// void SQ_ARRAY_SORT(void *array, ElementType, SqCompareFunc compareFunc);
#define SQ_ARRAY_SORT(array, ElementType, compareFunc)               \
		qsort(sq_array_data(array), ((SqArray*)(array))->length,     \
		      sizeof(ElementType), (SqCompareFunc)compareFunc)

// Binary search for sorted array
//void *SQ_ARRAY_SEARCH(void *array, ElementType, const void *key, SqCompareFunc compareFunc);
#define SQ_ARRAY_SEARCH(array, ElementType, key, compareFunc)        \
		bsearch((void*)(key),                                        \
		        sq_array_data(array), ((SqArray*)(array))->length,   \
		        sizeof(ElementType), (SqCompareFunc)compareFunc)

// find element in unsorted array
//void *SQ_ARRAY_FIND(void *array, ElementType, const void *key, SqCompareFunc compareFunc);
#define SQ_ARRAY_FIND(array, ElementType, key, compareFunc)          \
		sq_array_find(array, sizeof(ElementType), key, (SqCompareFunc)compareFunc)

// find element in sorted array and get index of element
//void *SQ_ARRAY_FIND_SORTED(void *array, ElementType, const void *key, SqCompareFunc compareFunc, int *insertingIndex);
#define SQ_ARRAY_FIND_SORTED(array, ElementType, key, compareFunc, insertingIndex)    \
		sq_array_find_sorted(array, sizeof(ElementType), key, (SqCompareFunc)compareFunc, insertingIndex)

/* C functions */

void *sq_array_init(void *array,
                    int   elementSize,
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
                    int   elementSize,
                    const void *key,
                    SqCompareFunc compareFunc);

// find element in sorted array and get index of element
void *sq_array_find_sorted(void *array,
                           int   elementSize,
                           const void *key,
                           SqCompareFunc compareFunc,
                           int  *insertingIndex);

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
	Type  *alloc(int count = 1);
	Type  *alloc(int index, int count);
	Type  *allocAt(int index, int count = 1);

	// append
	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   append(ValueType *values, int count = 1);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic<ValueType>::value ||
	                                      std::is_pointer<ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   append(ValueType  value);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	void   append(ValueType &value);

	// insert
	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   insert(int index, ValueType *values, int count = 1);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic<ValueType>::value ||
	                                      std::is_pointer<ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   insert(int index, ValueType  value);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	void   insert(int index, ValueType &value);

	// steal
	void   steal(int index, int count = 1);
	void   steal(Type *addr, int count = 1);

	// quick sort
	void   sort(SqCompareFunc compareFunc);

	template<typename ValueType = Type,
	         typename std::enable_if<std::is_arithmetic< ValueType>::value ||
	                                 std::is_same<char*, ValueType>::value ||
	                                 std::is_same<const char*, ValueType>::value>::type * = nullptr>
	void   sort();

	// binary search
	Type  *search(const void *key, SqCompareFunc compareFunc);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *search(ValueType  key, SqCompareFunc compareFunc = Sq::compare<Type>);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *search(ValueType &key, SqCompareFunc compareFunc);

	// find element in unsorted array
	Type  *find(const void *key, SqCompareFunc compareFunc);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *find(ValueType  key, SqCompareFunc compareFunc = Sq::compare<Type>);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *find(ValueType &key, SqCompareFunc compareFunc);

	// find element in sorted array and get index of element
	Type  *findSorted(const void *key, SqCompareFunc compareFunc, int *insertingIndex = NULL);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *findSorted(ValueType  key, SqCompareFunc compareFunc, int *insertingIndex = NULL);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *findSorted(ValueType &key, SqCompareFunc compareFunc, int *insertingIndex = NULL);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *findSorted(ValueType  key, int *insertingIndex = NULL);

	// access member variable
	int    elementSize();

	// get address of element
	Type  *addr(int index);

	/* ArrayMethod C++ definition (uncompleted) */
	typedef Type         value_type;

	typedef Type        &reference;
	typedef const Type  &const_reference;

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
inline Type *ArrayMethod<Type>::alloc(int count) {
	return (Type*) sq_array_alloc(this, count);
}
template<class Type>
inline Type *ArrayMethod<Type>::alloc(int index, int count) {
	return (Type*) sq_array_alloc_at(this, index, count);
}
template<class Type>
inline Type *ArrayMethod<Type>::allocAt(int index, int count) {
	return (Type*) sq_array_alloc_at(this, index, count);
}

// append
template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)  ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline void  ArrayMethod<Type>::append(ValueType *values, int count) {
	SQ_ARRAY_APPEND(this, Type, values, count);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic<ValueType>::value ||
                                      std::is_pointer<ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)  ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline void  ArrayMethod<Type>::append(ValueType  value) {
	*(Type*)sq_array_alloc_at(this, sq_array_length(this), 1) = (Type)value;
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::append(ValueType &value) {
	SQ_ARRAY_APPEND(this, Type, &value, 1);
}

// insert
template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)  ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline void  ArrayMethod<Type>::insert(int index, ValueType *values, int count) {
	SQ_ARRAY_INSERT(this, Type, index, values, count);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic<ValueType>::value ||
                                      std::is_pointer<ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)  ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline void  ArrayMethod<Type>::insert(int index, ValueType  value) {
	*(Type*)sq_array_alloc_at(this, index, 1) = (Type)value;
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::insert(int index, ValueType &value) {
	SQ_ARRAY_INSERT(this, Type, index, &value, 1);
}

// steal
template<class Type>
inline void  ArrayMethod<Type>::steal(int index, int count) {
	SQ_ARRAY_STEAL(this, Type, index, count);
}
template<class Type>
inline void  ArrayMethod<Type>::steal(Type *addr, int count) {
	SQ_ARRAY_STEAL_ADDR(this, Type, addr, count);
}

// quick sort
template<class Type>
inline void  ArrayMethod<Type>::sort(SqCompareFunc compareFunc) {
	SQ_ARRAY_SORT(this, Type, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<std::is_arithmetic< ValueType>::value ||
                                 std::is_same<char*, ValueType>::value ||
                                 std::is_same<const char*, ValueType>::value>::type *>
inline void  ArrayMethod<Type>::sort() {
	SQ_ARRAY_SORT(this, Type, Sq::compare<Type>);
}

// binary search
template<class Type>
inline Type *ArrayMethod<Type>::search(const void *key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_SEARCH(this, Type, key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic< ValueType>::value ||
                                      std::is_same<char*, ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)            ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline Type *ArrayMethod<Type>::search(ValueType  key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_SEARCH(this, Type, &key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type *ArrayMethod<Type>::search(ValueType &key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_SEARCH(this, Type, &key, compareFunc);
}

// find element in unsorted array
template<class Type>
inline Type  *ArrayMethod<Type>::find(const void *key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_FIND(this, Type, key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic< ValueType>::value ||
                                      std::is_same<char*, ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)            ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline Type  *ArrayMethod<Type>::find(ValueType  key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_FIND(this, Type, &key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::find(ValueType &key, SqCompareFunc compareFunc) {
	return (Type*) SQ_ARRAY_FIND(this, Type, &key, compareFunc);
}

// find element in sorted array and get index of element
template<class Type>
inline Type  *ArrayMethod<Type>::findSorted(const void *key, SqCompareFunc compareFunc, int *insertingIndex) {
	return (Type*) SQ_ARRAY_FIND_SORTED(this, Type, key, compareFunc, insertingIndex);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic< ValueType>::value ||
                                      std::is_same<char*, ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)            ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline Type  *ArrayMethod<Type>::findSorted(ValueType  key, SqCompareFunc compareFunc, int *insertingIndex) {
	return (Type*) SQ_ARRAY_FIND_SORTED(this, Type, &key, compareFunc, insertingIndex);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::findSorted(ValueType &key, SqCompareFunc compareFunc, int *insertingIndex) {
	return (Type*) SQ_ARRAY_FIND_SORTED(this, Type, &key, compareFunc, insertingIndex);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
                                     (std::is_arithmetic< ValueType>::value ||
                                      std::is_same<char*, ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value)) ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)            ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline Type  *ArrayMethod<Type>::findSorted(ValueType  key, int *insertingIndex) {
	return (Type*) SQ_ARRAY_FIND_SORTED(this, Type, &key, Sq::compare<Type>, insertingIndex);
}

// access member variable
template<class Type>
inline int   ArrayMethod<Type>::elementSize() {
	return sq_array_element_size(this);
}

// get address of element
template<class Type>
inline Type *ArrayMethod<Type>::addr(int index) {
	return (Type*)sq_array_addr(this, Type, index);
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
#define sq_int_array_init(array, capacity)           \
		sq_array_init(array, sizeof(int), capacity)

//void *sq_int_array_final(void *array);
#define sq_int_array_final               sq_array_final

// int *sq_int_array_addr(void *array, int index);
#define sq_int_array_addr(array, index)  sq_array_addr(array, int, index)

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

// void sq_int_array_push(void *array, int value);
#define sq_int_array_push(array, value)              \
		*sq_int_array_alloc(array, 1) = (int)(value);

// void sq_int_array_push_to(void *array, int index, int value);
#define sq_int_array_push_to(array, index, value)    \
		*sq_int_array_alloc_at(array, index, 1) = (int)(value);

// int *sq_int_array_append(void *array, const int *values, int count);
#define sq_int_array_append(array, values, count)    \
		(int*)SQ_ARRAY_APPEND(array, int, values, count)

// int *sq_int_array_insert(void *array, int index, const int *values, int count);
#define sq_int_array_insert(array, index, values, count)    \
		(int*)SQ_ARRAY_INSERT(array, int, index, values, count)

// void sq_int_array_steal(void *array, int index, int count);
#define sq_int_array_steal(array, index, count)             \
		SQ_ARRAY_STEAL(array, int, index, count)

// void sq_int_array_steal_addr(void *array, int *elementAddr, int count);
#define sq_int_array_steal_addr(array, elementAddr, count)  \
		SQ_ARRAY_STEAL_ADDR(array, int, elementAddr, count)

// Quick sort
// void sq_int_array_sort(void *array, SqCompareFunc compareFunc);
#define sq_int_array_sort(array, compareFunc)               \
		SQ_ARRAY_SORT(array, int, compareFunc)

// Binary search for sorted array
// int *sq_int_array_search(void *array, const int *key, SqCompareFunc compareFunc);
#define sq_int_array_search(array, key, compareFunc)        \
		(int*)SQ_ARRAY_SEARCH(array, int, key, compareFunc)

// find element in unsorted array
// int *sq_int_array_find(void *array, const int *key, SqCompareFunc compareFunc);
#define sq_int_array_find(array, key, compareFunc)          \
		(int*)SQ_ARRAY_FIND(array, int, key, compareFunc)

// find element in sorted array and get index of element
// int *sq_int_array_find_sorted(void *array, const int *key, SqCompareFunc compareFunc, int *insertingIndex);
#define sq_int_array_find_sorted(array, key, compareFunc, insertingIndex)    \
		(int*)SQ_ARRAY_FIND_SORTED(array, int, key, compareFunc, insertingIndex)

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
