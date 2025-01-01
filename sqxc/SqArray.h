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

#ifndef SQ_ARRAY_H
#define SQ_ARRAY_H

#ifdef __cplusplus
#include <type_traits>
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stddef.h>       // NULL
#include <stdint.h>       // uint8_t
#include <stdlib.h>       // qsort(), bsearch(), malloc(), free()
#include <string.h>       // memcpy(), memmove()

#include <SqDefine.h>     // SqClearFunc, SqCompareFunc

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

#define sq_array_header_offsetof(HeaderField)     \
		( sizeof(SqArrayHeader) - offsetof(SqArrayHeader, HeaderField) )

#define sq_array_element_size(array)   \
		*(unsigned int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(element_size) )

#define sq_array_capacity(array)       \
		*(unsigned int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(capacity) )

#define sq_array_ref_count(array)      \
		*(int*)( ((SqArray*)(array))->data - sq_array_header_offsetof(ref_count) )

#define sq_array_clear_func(array)     \
		*(SqClearFunc*)( ((SqArray*)(array))->data - sq_array_header_offsetof(clear_func) )

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_array_new(unsigned int elementSize, unsigned int capacity);
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
//ElementType *sq_array_addr(void *array, ElementType, unsigned int index);
#define sq_array_addr(array, ElementType, index)  \
		( ((ElementType*)((SqArray*)(array))->data) + (index) )

//ElementType  sq_array_at(void *array, ElementType, unsigned int index);
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

// void sq_array_push_in(void *array, ElementType, unsigned int index, ElementType value);
#define sq_array_push_in(array, ElementType, index, value)    \
		*(ElementType*)sq_array_alloc_at(array, index, 1) = (ElementType)(value)

// deprecated
// void sq_array_push_to(void *array, ElementType, unsigned int index, ElementType value);
#define sq_array_push_to(array, ElementType, index, value)    \
		*(ElementType*)sq_array_alloc_at(array, index, 1) = (ElementType)(value)

// Quick sort
// void sq_array_sort(void *array, ElementType, SqCompareFunc compareFunc);
#define sq_array_sort(array, ElementType, compareFunc)               \
		qsort(sq_array_data(array), ((SqArray*)(array))->length,     \
		      sizeof(ElementType), (SqCompareFunc)compareFunc)

// Binary search for sorted array
//ElementType *sq_array_search(void *array, ElementType, const void *key, SqCompareFunc compareFunc);
#define sq_array_search(array, ElementType, key, compareFunc)                      \
		(ElementType*)bsearch((void*)(key),                                        \
		                      sq_array_data(array), ((SqArray*)(array))->length,   \
		                      sizeof(ElementType), (SqCompareFunc)compareFunc)

/* macro functions - Macros use parameters multiple times. Do not use the ++ operator in parameters. */

//ElementType *sq_array_append(void *array, ElementType, const void *values, unsigned int count);
#define sq_array_append(array, ElementType, values, count)                         \
		(ElementType*)memcpy(sq_array_alloc(array, count), values,                 \
		                     sizeof(ElementType) * (count))

//ElementType *sq_array_insert(void *array, ElementType, unsigned int index, const void *values, unsigned int count);
#define sq_array_insert(array, ElementType, index, values, count)                  \
		(ElementType*)memcpy(sq_array_alloc_at(array, index, count), values,       \
		                     sizeof(ElementType) * (count))

// Removes a value from array without calling the destroy function.
// void sq_array_steal(void *array, ElementType, unsigned int index, unsigned int count);
#define sq_array_steal(array, ElementType, index, count)                           \
		memmove((ElementType*)sq_array_data(array) +  (index),                     \
		        (ElementType*)sq_array_data(array) + ((index)+(count)),            \
		        sizeof(ElementType) * ((sq_array_length(array) -= (count)) - (index)) )

// void sq_array_steal_addr(void *array, ElementType, ElementType *elementAddr, unsigned int count);
#define sq_array_steal_addr(array, ElementType, elementAddr, count)  \
		memmove(elementAddr,                                         \
		        (ElementType*)(elementAddr) + (count),               \
		        sizeof(ElementType) * (sq_array_length(array) -= (count)) - ((uint8_t*)(elementAddr) - sq_array_data(array)) )

/* Alias */

// alias of sq_array_append()
//void *SQ_ARRAY_APPEND(void *array, ElementType, const void *values, unsigned int count);
#define SQ_ARRAY_APPEND        sq_array_append

// alias of sq_array_insert()
//void *SQ_ARRAY_INSERT(void *array, ElementType, unsigned int index, const void *values, unsigned int count);
#define SQ_ARRAY_INSERT        sq_array_insert

// alias of sq_array_steal()
//void  SQ_ARRAY_STEAL(void *array, ElementType, unsigned int index, unsigned int count);
#define SQ_ARRAY_STEAL         sq_array_steal

// alias of sq_array_steal_addr()
//void  SQ_ARRAY_STEAL_ADDR(void *array, ElementType, ElementType *elementAddr, unsigned int count);
#define SQ_ARRAY_STEAL_ADDR    sq_array_steal_addr

// alias of sq_array_sort()
//void  SQ_ARRAY_SORT(void *array, ElementType, SqCompareFunc compareFunc);
#define SQ_ARRAY_SORT          sq_array_sort

// alias of sq_array_search()
//ElementType *SQ_ARRAY_SEARCH(void *array, ElementType, const void *key, SqCompareFunc compareFunc);
#define SQ_ARRAY_SEARCH        sq_array_search

/* macro for maintaining C/C++ inline functions easily */

//void *SQ_ARRAY_NEW(ElementType, unsigned int capacity);
#define SQ_ARRAY_NEW(ElementType, capacity)                          \
		sq_array_new(sizeof(ElementType), capacity)

//void *SQ_ARRAY_INIT(void *array, ElementType, unsigned int capacity);
#define SQ_ARRAY_INIT(array, ElementType, capacity)                  \
		sq_array_init(array, sizeof(ElementType), capacity)

//ElementType *SQ_ARRAY_ALLOC(void *array, ElementType, unsigned int count);
#define SQ_ARRAY_ALLOC(array, ElementType, count)                    \
		(ElementType*)sq_array_alloc_at(array, sq_array_length(array), count)

//ElementType *SQ_ARRAY_ALLOC_AT(void *array, ElementType, unsigned int index, unsigned int count);
#define SQ_ARRAY_ALLOC_AT(array, ElementType, index, count)          \
		(ElementType*)sq_array_alloc_at(array, index, count)

// find element in unsorted array
//ElementType *SQ_ARRAY_FIND(void *array, ElementType, const void *key, SqCompareFunc compareFunc);
#define SQ_ARRAY_FIND(array, ElementType, key, compareFunc)          \
		(ElementType*)sq_array_find(array, sizeof(ElementType), key, (SqCompareFunc)compareFunc)

// find element in sorted array and get index of element
//ElementType *SQ_ARRAY_FIND_SORTED(void *array, ElementType, const void *key, SqCompareFunc compareFunc, unsigned int *insertingIndex);
#define SQ_ARRAY_FIND_SORTED(array, ElementType, key, compareFunc, insertingIndex)    \
		(ElementType*)sq_array_find_sorted(array, sizeof(ElementType), key, (SqCompareFunc)compareFunc, insertingIndex)

/* C functions */

void *sq_array_init(void *array,
                    unsigned int  elementSize,
                    unsigned int  capacity);

void *sq_array_final(void *array);

// Allocate space at the specified array location.
// It returns allocated address in the array.
void *sq_array_alloc_at(void *array, unsigned int index, unsigned int count);

//void *sq_array_alloc(void *array, unsigned int count)
#define sq_array_alloc(array, count)              \
		sq_array_alloc_at(array, sq_array_length(array), count)

// find element in unsorted array
void *sq_array_find(const void   *array,
                    unsigned int  elementSize,
                    const void   *key,
                    SqCompareFunc compareFunc);

// find element in sorted array and get index of element
void *sq_array_find_sorted(const void   *array,
                           unsigned int  elementSize,
                           const void   *key,
                           SqCompareFunc compareFunc,
                           unsigned int *insertingIndex);

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
	Type  *alloc(unsigned int count = 1);
	Type  *alloc(unsigned int index, unsigned int count);
	Type  *allocAt(unsigned int index, unsigned int count = 1);

	// append
	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   append(ValueType *values, unsigned int count = 1);

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
	void   insert(unsigned int index, ValueType *values, unsigned int count = 1);

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic<ValueType>::value ||
	                                      std::is_pointer<ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)  ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	void   insert(unsigned int index, ValueType  value);

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	void   insert(unsigned int index, ValueType &value);

	// steal
	void   steal(unsigned int index, unsigned int count = 1);
	void   steal(Type *addr, unsigned int count = 1);

	// quick sort
	void   sort(SqCompareFunc compareFunc);

	template<typename ValueType = Type,
	         typename std::enable_if<std::is_arithmetic< ValueType>::value ||
	                                 std::is_same<char*, ValueType>::value ||
	                                 std::is_same<const char*, ValueType>::value>::type * = nullptr>
	void   sort();

	// binary search
	Type  *search(const void *key, SqCompareFunc compareFunc) const;

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *search(ValueType  key, SqCompareFunc compareFunc = Sq::compare<Type>) const;

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *search(ValueType &key, SqCompareFunc compareFunc) const;

	// find element in unsorted array
	Type  *find(const void *key, SqCompareFunc compareFunc) const;

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *find(ValueType  key, SqCompareFunc compareFunc = Sq::compare<Type>) const;

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *find(ValueType &key, SqCompareFunc compareFunc) const;

	// find element in sorted array and get index of element
	Type  *findSorted(const void *key, SqCompareFunc compareFunc, unsigned int *insertingIndex = NULL) const;

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *findSorted(ValueType  key, SqCompareFunc compareFunc, unsigned int *insertingIndex = NULL) const;

	template<typename ValueType = Type,
	         typename std::enable_if< std::is_same<Type, ValueType>::value &&
	                                 !std::is_arithmetic<ValueType>::value &&
	                                 !std::is_pointer<ValueType>::value>::type * = nullptr>
	Type  *findSorted(ValueType &key, SqCompareFunc compareFunc, unsigned int *insertingIndex = NULL) const;

	template<typename ValueType = Type,
	         typename std::enable_if<(std::is_same<Type, ValueType>::value &&
	                                     (std::is_arithmetic< ValueType>::value ||
	                                      std::is_same<char*, ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value)) ||
	                                 (std::is_same<const char*, Type>::value &&
	                                  std::is_same<char*, ValueType>::value)            ||
	                                 (std::is_same<char*, Type>::value &&
	                                  std::is_same<const char*, ValueType>::value)>::type * = nullptr>
	Type  *findSorted(ValueType  key, unsigned int *insertingIndex = NULL) const;

	// access member variable
	unsigned int  elementSize();

	// get address of element
	Type  *addr(unsigned int index);

	/* ArrayMethod C++ definition (uncompleted) */
	typedef Type         value_type;

	typedef Type        &reference;
	typedef const Type  &const_reference;

	typedef Type        *iterator;
	typedef const Type  *const_iterator;

	unsigned int  size() const;
	unsigned int  capacity();
	void   reserve(unsigned int n);
	Type  *begin();
	Type  *end();
	Type  &at(unsigned int index);
	Type  &operator[](unsigned int index);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqArrayHeader is hidden in front of the array */

struct SqArrayHeader
{
	unsigned int  element_size;
	unsigned int  capacity;

	// reserve: ref_count, bit_field
	int   ref_count;
	int   int2;

	// reserve: callback functions
	void *clear_func;        // element clear function
	void *ptr;
};

/* SqArray is array of arbitrary elements */

#define SQ_ARRAY_MEMBERS(Type, data_name, length_name)   \
		Type         *data_name;    \
		unsigned int  length_name

#define SQ_ARRAY_STRUCT(Type)           \
		struct { SQ_ARRAY_MEMBERS(Type, data, length); }

#define SQ_ARRAY_TYPEDEF(Type, Name)    \
		typedef struct { SQ_ARRAY_MEMBERS(Type, data, length); } Name

struct SqArray
{
	SQ_ARRAY_MEMBERS(uint8_t, data, length);
/*	// ------ SqArray members ------
	uint8_t      *data;
	unsigned int  length;
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
inline Type *ArrayMethod<Type>::alloc(unsigned int count) {
	return (Type*) sq_array_alloc(this, count);
}
template<class Type>
inline Type *ArrayMethod<Type>::alloc(unsigned int index, unsigned int count) {
	return (Type*) sq_array_alloc_at(this, index, count);
}
template<class Type>
inline Type *ArrayMethod<Type>::allocAt(unsigned int index, unsigned int count) {
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
inline void  ArrayMethod<Type>::append(ValueType *values, unsigned int count) {
	sq_array_append(this, Type, values, count);
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
	sq_array_append(this, Type, &value, 1);
}

// insert
template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value    ||
                                 (std::is_same<const char*, Type>::value &&
                                  std::is_same<char*, ValueType>::value)  ||
                                 (std::is_same<char*, Type>::value &&
                                  std::is_same<const char*, ValueType>::value)>::type *>
inline void  ArrayMethod<Type>::insert(unsigned int index, ValueType *values, unsigned int count) {
	sq_array_insert(this, Type, index, values, count);
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
inline void  ArrayMethod<Type>::insert(unsigned int index, ValueType  value) {
	*(Type*)sq_array_alloc_at(this, index, 1) = (Type)value;
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline void  ArrayMethod<Type>::insert(unsigned int index, ValueType &value) {
	sq_array_insert(this, Type, index, &value, 1);
}

// steal
template<class Type>
inline void  ArrayMethod<Type>::steal(unsigned int index, unsigned int count) {
	sq_array_steal(this, Type, index, count);
}
template<class Type>
inline void  ArrayMethod<Type>::steal(Type *addr, unsigned int count) {
	sq_array_steal_addr(this, Type, addr, count);
}

// quick sort
template<class Type>
inline void  ArrayMethod<Type>::sort(SqCompareFunc compareFunc) {
	sq_array_sort(this, Type, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if<std::is_arithmetic< ValueType>::value ||
                                 std::is_same<char*, ValueType>::value ||
                                 std::is_same<const char*, ValueType>::value>::type *>
inline void  ArrayMethod<Type>::sort() {
	sq_array_sort(this, Type, Sq::compare<Type>);
}

// binary search
template<class Type>
inline Type *ArrayMethod<Type>::search(const void *key, SqCompareFunc compareFunc) const {
	return sq_array_search(this, Type, key, compareFunc);
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
inline Type *ArrayMethod<Type>::search(ValueType  key, SqCompareFunc compareFunc) const {
	return sq_array_search(this, Type, &key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type *ArrayMethod<Type>::search(ValueType &key, SqCompareFunc compareFunc) const {
	return sq_array_search(this, Type, &key, compareFunc);
}

// find element in unsorted array
template<class Type>
inline Type  *ArrayMethod<Type>::find(const void *key, SqCompareFunc compareFunc) const {
	return SQ_ARRAY_FIND(this, Type, key, compareFunc);
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
inline Type  *ArrayMethod<Type>::find(ValueType  key, SqCompareFunc compareFunc) const {
	return SQ_ARRAY_FIND(this, Type, &key, compareFunc);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::find(ValueType &key, SqCompareFunc compareFunc) const {
	return SQ_ARRAY_FIND(this, Type, &key, compareFunc);
}

// find element in sorted array and get index of element
template<class Type>
inline Type  *ArrayMethod<Type>::findSorted(const void *key, SqCompareFunc compareFunc, unsigned int *insertingIndex) const {
	return SQ_ARRAY_FIND_SORTED(this, Type, key, compareFunc, insertingIndex);
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
inline Type  *ArrayMethod<Type>::findSorted(ValueType  key, SqCompareFunc compareFunc, unsigned int *insertingIndex) const {
	return SQ_ARRAY_FIND_SORTED(this, Type, &key, compareFunc, insertingIndex);
}

template<typename Type>
template<typename ValueType,
         typename std::enable_if< std::is_same<Type, ValueType>::value &&
                                 !std::is_arithmetic<ValueType>::value &&
                                 !std::is_pointer<ValueType>::value>::type *>
inline Type  *ArrayMethod<Type>::findSorted(ValueType &key, SqCompareFunc compareFunc, unsigned int *insertingIndex) const {
	return SQ_ARRAY_FIND_SORTED(this, Type, &key, compareFunc, insertingIndex);
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
inline Type  *ArrayMethod<Type>::findSorted(ValueType  key, unsigned int *insertingIndex) const {
	return SQ_ARRAY_FIND_SORTED(this, Type, &key, Sq::compare<Type>, insertingIndex);
}

// access member variable
template<class Type>
inline unsigned int   ArrayMethod<Type>::elementSize() {
	return sq_array_element_size(this);
}

// get address of element
template<class Type>
inline Type *ArrayMethod<Type>::addr(unsigned int index) {
	return sq_array_addr(this, Type, index);
}

/* ArrayMethod iterator (uncompleted) */

template<class Type>
inline unsigned int   ArrayMethod<Type>::size() const {
	return sq_array_length(this);
}
template<class Type>
inline unsigned int   ArrayMethod<Type>::capacity() {
	return sq_array_capacity(this);
}
template<class Type>
inline void  ArrayMethod<Type>::reserve(unsigned int  n) {
	unsigned int  length = sq_array_length(this);
	if (n > length) {
		sq_array_alloc_at(this, length, n - length);
		sq_array_length(this) = length;
	}
}
template<class Type>
inline Type *ArrayMethod<Type>::begin() {
	return sq_array_begin(this, Type);
}
template<class Type>
inline Type *ArrayMethod<Type>::end() {
	return sq_array_end(this, Type);
}
template<class Type>
inline Type &ArrayMethod<Type>::at(unsigned int index) {
	return sq_array_at(this, Type, index);
}
template<class Type>
inline Type &ArrayMethod<Type>::operator[](unsigned int index) {
	return sq_array_at(this, Type, index);
}

/* All derived struct/class must be C++11 standard-layout. */

template<class Type>
struct Array : ArrayMethod<Type>                 // <-- 1. inherit C++ method
{
	SQ_ARRAY_MEMBERS(Type, data, length);        // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	Type         *data;
	unsigned int  length;
 */

	// constructor
	Array(unsigned int capacity = 8) {
		sq_array_init(this, sizeof(Type), capacity);
	}
	// destructor
	~Array(void) {
		sq_array_final(this);
	}
	// copy constructor
	Array(const Array &src) {
		sq_array_init(this, sizeof(Type), sq_array_capacity(&src));
		sq_array_append(this, Type, src.data, src.length);
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

/* macro functions - parameter used only once in macro (except parameter 'array') */

//void *sq_int_array_init(void *array, unsigned int capacity);
#define sq_int_array_init(array, capacity)           \
		sq_array_init(array, sizeof(int), capacity)

//void *sq_int_array_final(void *array);
#define sq_int_array_final               sq_array_final

// int *sq_int_array_addr(void *array, unsigned int index);
#define sq_int_array_addr(array, index)  sq_array_addr(array, int, index)

// int  sq_int_array_at(void *array, unsigned int index);
#define sq_int_array_at(array, index)    sq_array_at(array, int, index)

// int *sq_int_array_begin(void *array);
#define sq_int_array_begin(array)        sq_array_begin(array, int)

// int *sq_int_array_end(void *array);
#define sq_int_array_end(array)          sq_array_end(array, int)

// int *sq_int_array_alloc(void *array, unsigned int count);
#define sq_int_array_alloc(array, count)             \
		SQ_ARRAY_ALLOC(array, int, count)

// int *sq_int_array_alloc_at(void *array, unsigned int index, unsigned int count);
#define sq_int_array_alloc_at(array, index, count)   \
		SQ_ARRAY_ALLOC_AT(array, int, index, count)

// void sq_int_array_push(void *array, int value);
#define sq_int_array_push(array, value)              \
		sq_array_push(array, int, value)

// void sq_int_array_push_in(void *array, unsigned int index, int value);
#define sq_int_array_push_in(array, index, value)    \
		sq_array_push_in(array, int, index, value)

// deprecated
// void sq_int_array_push_to(void *array, unsigned int index, int value);
#define sq_int_array_push_to(array, index, value)    \
		sq_array_push_to(array, int, index, value)

// Quick sort
// void sq_int_array_sort(void *array, SqCompareFunc compareFunc);
#define sq_int_array_sort(array, compareFunc)               \
		sq_array_sort(array, int, compareFunc)

// Binary search for sorted array
// int *sq_int_array_search(void *array, const int *key, SqCompareFunc compareFunc);
#define sq_int_array_search(array, key, compareFunc)        \
		sq_array_search(array, int, key, compareFunc)

// find element in unsorted array
// int *sq_int_array_find(void *array, const int *key, SqCompareFunc compareFunc);
#define sq_int_array_find(array, key, compareFunc)          \
		SQ_ARRAY_FIND(array, int, key, compareFunc)

// find element in sorted array and get index of element
// int *sq_int_array_find_sorted(void *array, const int *key, SqCompareFunc compareFunc, unsigned int *insertingIndex);
#define sq_int_array_find_sorted(array, key, compareFunc, insertingIndex)    \
		SQ_ARRAY_FIND_SORTED(array, int, key, compareFunc, insertingIndex)

/* macro functions - Macros use parameters multiple times. Do not use the ++ operator in parameters. */

// int *sq_int_array_append(void *array, const int *values, unsigned int count);
#define sq_int_array_append(array, values, count)    \
		sq_array_append(array, int, values, count)

// int *sq_int_array_insert(void *array, unsigned int index, const int *values, unsigned int count);
#define sq_int_array_insert(array, index, values, count)    \
		sq_array_insert(array, int, index, values, count)

// void sq_int_array_steal(void *array, unsigned int index, unsigned int count);
#define sq_int_array_steal(array, index, count)             \
		sq_array_steal(array, int, index, count)

// void sq_int_array_steal_addr(void *array, int *elementAddr, unsigned int count);
#define sq_int_array_steal_addr(array, elementAddr, count)  \
		sq_array_steal_addr(array, int, elementAddr, count)

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
	int          *data;
	unsigned int  length;
 */
};


#endif  // SQ_ARRAY_H
