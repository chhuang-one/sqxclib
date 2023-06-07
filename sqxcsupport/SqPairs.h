/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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

#ifndef SQ_PAIRS_H
#define SQ_PAIRS_H

#include <SqPtrArray.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqPairs       SqPairs;    // array of SqPair

#define SQB_PAIRS_SORTED    (1 << 0)
#define SQB_PAIRS_FOUND     (1 << 1)

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* macro for accessing variable of SqPairs */

#define sq_pairs_is_found(pairs)     (((pairs)->bit_field & SQB_PAIRS_FOUND)  > 0)
#define sq_pairs_not_found(pairs)    (((pairs)->bit_field & SQB_PAIRS_FOUND) == 0)

/* C functions */

void    sq_pairs_init(SqPairs *pairs, SqCompareFunc key_compare_func);
void    sq_pairs_final(SqPairs *pairs);

void    sq_pairs_add(SqPairs *pairs, void *key, void *value);
void    sq_pairs_erase(SqPairs *pairs, void *key);
void    sq_pairs_steal(SqPairs *pairs, void *key);
void   *sq_pairs_find(SqPairs *pairs, void *key);

// alias of sq_pairs_erase()
// void sq_pairs_remove(SqPairs *pairs, void *key);
#define sq_pairs_remove        sq_pairs_erase

void    sq_pairs_sort(SqPairs *pairs);

// SqPair SqCompareFunc
int     sq_pairs_cmp_string(const char **key1, const char **key2);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	PairsMethod is used by SqPairs and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqPairs members.
 */
template<typename Key, typename Value = Key>
struct PairsMethod
{
	// status of find()
	bool    isFound(void);
	bool    notFound(void);

	// add
	template<typename KeyType   = Key,
	         typename ValueType = Value,
	         typename std::enable_if<(std::is_pointer<Key>::value &&
	                                     (std::is_pointer<KeyType>::value ||
	                                      std::is_same<const char*, KeyType>::value) ) &&
	                                 (std::is_pointer<Value>::value &&
	                                     (std::is_pointer<ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value) )>::type * = nullptr>
	void    add(KeyType  key, ValueType  value);

	template<typename KeyType   = Key,
	         typename ValueType = Value,
	         typename std::enable_if<(std::is_pointer<Key>::value &&
	                                     (std::is_pointer<KeyType>::value ||
	                                      std::is_same<const char*, KeyType>::value) ) &&
	                                 (std::is_integral<Value>::value &&
	                                  std::is_integral<ValueType>::value)>::type * = nullptr>
	void    add(KeyType  key, ValueType  value);

	template<typename KeyType   = Key,
	         typename ValueType = Value,
	         typename std::enable_if<(std::is_integral<Key>::value &&
	                                  std::is_integral<KeyType>::value) &&
	                                 (std::is_pointer<Value>::value &&
	                                     (std::is_pointer<ValueType>::value ||
	                                      std::is_same<const char*, ValueType>::value) )>::type * = nullptr>
	void    add(KeyType  key, ValueType  value);

	template<typename KeyType   = Key,
	         typename ValueType = Value,
	         typename std::enable_if<(std::is_integral<Key>::value &&
	                                  std::is_integral<KeyType>::value) &&
	                                 (std::is_integral<Value>::value &&
	                                  std::is_integral<ValueType>::value)>::type * = nullptr>
	void    add(KeyType  key, ValueType  value);

	// alias of erase()
	// remove
	template<typename KeyType   = Key,
	         typename std::enable_if< std::is_pointer<Key>::value &&
	                                 (std::is_pointer<KeyType>::value ||
	                                  std::is_same<const char*, KeyType>::value)>::type * = nullptr>
	void    remove(KeyType  key);

	template<typename KeyType   = Key,
	         typename std::enable_if<std::is_integral<Key>::value &&
	                                 std::is_integral<KeyType>::value>::type * = nullptr>
	void    remove(KeyType  key);

	// erase
	template<typename KeyType   = Key,
	         typename std::enable_if< std::is_pointer<Key>::value &&
	                                 (std::is_pointer<KeyType>::value ||
	                                  std::is_same<const char*, KeyType>::value)>::type * = nullptr>
	void    erase(KeyType  key);

	template<typename KeyType   = Key,
	         typename std::enable_if<std::is_integral<Key>::value &&
	                                 std::is_integral<KeyType>::value>::type * = nullptr>
	void    erase(KeyType  key);

	// steal
	template<typename KeyType   = Key,
	         typename std::enable_if< std::is_pointer<Key>::value &&
	                                 (std::is_pointer<KeyType>::value ||
	                                  std::is_same<const char*, KeyType>::value)>::type * = nullptr>
	void    steal(KeyType  key);

	template<typename KeyType   = Key,
	         typename std::enable_if<std::is_integral<Key>::value &&
	                                 std::is_integral<KeyType>::value>::type * = nullptr>
	void    steal(KeyType  key);

	// find
	template<typename KeyType   = Key,
	         typename std::enable_if< std::is_pointer<Key>::value &&
	                                 (std::is_pointer<KeyType>::value ||
	                                  std::is_same<const char*, KeyType>::value)>::type * = nullptr>
	Value   find(KeyType  key);

	template<typename KeyType   = Key,
	         typename std::enable_if<std::is_integral<Key>::value &&
	                                 std::is_integral<KeyType>::value>::type * = nullptr>
	Value   find(KeyType  key);

	// sort
	void    sort();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqPairs is array of key-value pairs.

	It only supports pointer type, intptr_t, and uintptr_t.
 */

#define SQ_PAIRS_MEMBERS                            \
	SQ_ARRAY_MEMBERS(void*, data, length);          \
	unsigned int   bit_field;                       \
	SqCompareFunc  key_compare_func;                \
	SqDestroyFunc  key_destroy_func;                \
	SqDestroyFunc  value_destroy_func

#ifdef __cplusplus
struct SqPairs : Sq::PairsMethod<void*, void*>   // <-- 1. inherit C++ member function(method)
#else
struct SqPairs
#endif
{
	SQ_PAIRS_MEMBERS;                            // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void         **data;
	int            length;

	// ------ SqPairs members ------
	unsigned int   bit_field;
	SqCompareFunc  key_compare_func;
	SqDestroyFunc  key_destroy_func;
	SqDestroyFunc  value_destroy_func;
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

/* define PairsMethod functions. */

// status of find()
template<typename Key, typename Value>
inline bool   PairsMethod<Key, Value>::isFound(void) {
	return sq_pairs_is_found((SqPairs*)this);
}
template<typename Key, typename Value>
inline bool   PairsMethod<Key, Value>::notFound(void) {
	return sq_pairs_not_found((SqPairs*)this);
}

// add
template<typename Key, typename Value>
template<typename KeyType, typename ValueType,
         typename std::enable_if<(std::is_pointer<Key>::value &&
                                     (std::is_pointer<KeyType>::value ||
                                      std::is_same<const char*, KeyType>::value) ) &&
                                 (std::is_pointer<Value>::value &&
                                     (std::is_pointer<ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value) )>::type *>
inline void   PairsMethod<Key, Value>::add(KeyType  key, ValueType  value) {
	sq_pairs_add((SqPairs*)this, (void*)key, (void*)value);
}

template<typename Key, typename Value>
template<typename KeyType, typename ValueType,
         typename std::enable_if<(std::is_pointer<Key>::value &&
                                     (std::is_pointer<KeyType>::value ||
                                      std::is_same<const char*, KeyType>::value) ) &&
                                 (std::is_integral<Value>::value &&
                                  std::is_integral<ValueType>::value)>::type *>
inline void   PairsMethod<Key, Value>::add(KeyType  key, ValueType  value) {
	sq_pairs_add((SqPairs*)this, (void*)key, (void*)(intptr_t)value);
}

template<typename Key, typename Value>
template<typename KeyType, typename ValueType,
         typename std::enable_if<(std::is_integral<Key>::value &&
	                              std::is_integral<KeyType>::value) &&
                                 (std::is_pointer<Value>::value &&
                                     (std::is_pointer<ValueType>::value ||
                                      std::is_same<const char*, ValueType>::value) )>::type *>
inline void   PairsMethod<Key, Value>::add(KeyType  key, ValueType  value) {
	sq_pairs_add((SqPairs*)this, (void*)(intptr_t)key, (void*)value);
}

template<typename Key, typename Value>
template<typename KeyType, typename ValueType,
         typename std::enable_if<(std::is_integral<Key>::value &&
                                  std::is_integral<KeyType>::value) &&
                                 (std::is_integral<Value>::value &&
                                  std::is_integral<ValueType>::value)>::type *>
inline void   PairsMethod<Key, Value>::add(KeyType  key, ValueType  value) {
	sq_pairs_add((SqPairs*)this, (void*)(intptr_t)key, (void*)(intptr_t)value);
}

// alias of erase()
// remove
template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if< std::is_pointer<Key>::value &&
                                 (std::is_pointer<KeyType>::value ||
                                  std::is_same<const char*, KeyType>::value)>::type *>
inline void   PairsMethod<Key, Value>::remove(KeyType  key) {
	sq_pairs_erase((SqPairs*)this, (void*)key);
}

template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if<std::is_integral<Key>::value &&
                                 std::is_integral<KeyType>::value>::type *>
inline void   PairsMethod<Key, Value>::remove(KeyType  key) {
	sq_pairs_erase((SqPairs*)this, (void*)(intptr_t)key);
}

// erase
template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if< std::is_pointer<Key>::value &&
                                 (std::is_pointer<KeyType>::value ||
                                  std::is_same<const char*, KeyType>::value)>::type *>
inline void   PairsMethod<Key, Value>::erase(KeyType  key) {
	sq_pairs_erase((SqPairs*)this, (void*)key);
}

template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if<std::is_integral<Key>::value &&
                                 std::is_integral<KeyType>::value>::type *>
inline void   PairsMethod<Key, Value>::erase(KeyType  key) {
	sq_pairs_erase((SqPairs*)this, (void*)(intptr_t)key);
}

// steal
template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if< std::is_pointer<Key>::value &&
                                 (std::is_pointer<KeyType>::value ||
                                  std::is_same<const char*, KeyType>::value)>::type *>
inline void   PairsMethod<Key, Value>::steal(KeyType  key) {
	sq_pairs_steal((SqPairs*)this, (void*)key);
}

template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if<std::is_integral<Key>::value &&
                                 std::is_integral<KeyType>::value>::type *>
inline void   PairsMethod<Key, Value>::steal(KeyType  key) {
	sq_pairs_steal((SqPairs*)this, (void*)(intptr_t)key);
}

// find
template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if< std::is_pointer<Key>::value &&
                                 (std::is_pointer<KeyType>::value ||
                                  std::is_same<const char*, KeyType>::value)>::type *>
inline Value  PairsMethod<Key, Value>::find(KeyType  key) {
	return (Value)sq_pairs_find((SqPairs*)this, (void*)key);
}

template<typename Key, typename Value>
template<typename KeyType,
         typename std::enable_if<std::is_integral<Key>::value &&
                                 std::is_integral<KeyType>::value>::type *>
inline Value  PairsMethod<Key, Value>::find(KeyType  key) {
	return (Value)sq_pairs_find((SqPairs*)this, (void*)(intptr_t)key);
}

// sort
template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::sort() {
	sq_pairs_sort((SqPairs*)this);
}

/* All derived struct/class must be C++11 standard-layout. */

template<typename Key = char*, typename Value = Key>
struct Pairs : Sq::PairsMethod<Key, Value>       // <-- 1. inherit C++ member function(method)
{
	SQ_PAIRS_MEMBERS;                            // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void         **data;
	int            length;

	// ------ SqPairs members ------
	unsigned int   bit_field;
	SqCompareFunc  key_compare_func;
	SqDestroyFunc  key_destroy_func;
	SqDestroyFunc  value_destroy_func;
 */

	Pairs(SqCompareFunc keyCompareFunc   = (SqCompareFunc)Sq::compare<Key>,
	      SqDestroyFunc keyDestroyFunc   = NULL,
	      SqDestroyFunc valueDestroyFunc = NULL)
	{
		static_assert( (std::is_pointer<Key>::value        ||
		                std::is_same<intptr_t, Key>::value ||
		                std::is_same<uintptr_t, Key>::value)   &&
		               (std::is_pointer<Value>::value        ||
		                std::is_same<intptr_t, Value>::value ||
		                std::is_same<uintptr_t, Value>::value),
		              "Sq::Pairs only supports pointer type, intptr_t, and uintptr_t.");

		sq_pairs_init((SqPairs*)this, keyCompareFunc);
		this->key_destroy_func   = keyDestroyFunc;
		this->value_destroy_func = valueDestroyFunc;
	}
	~Pairs() {
		sq_pairs_final((SqPairs*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_PAIRS_H
