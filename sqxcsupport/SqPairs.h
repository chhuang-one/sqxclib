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

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

void    sq_pairs_init(SqPairs *pairs, SqCompareFunc key_compare_func);
void    sq_pairs_final(SqPairs *pairs);

void    sq_pairs_add(SqPairs *pairs, void *key, void *value);
void    sq_pairs_erase(SqPairs *pairs, void *key);
void    sq_pairs_steal(SqPairs *pairs, void *key);
void   *sq_pairs_find(SqPairs *pairs, void *key);

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
	void    add(Key *key, Value *value);
	void    remove(Key *key);
	void    erase(Key *key);
	void    steal(Key *key);
	Value  *find(Key *key);
	void    sort();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqPairs is array of key-value pairs.
 */

#define SQ_PAIRS_MEMBERS                            \
	SQ_ARRAY_MEMBERS(void*, data, length);          \
	int            sorted;                          \
	SqCompareFunc  key_compare_func;                \
	SqDestroyFunc  key_destroy_func;                \
	SqDestroyFunc  value_destroy_func

#ifdef __cplusplus
struct SqPairs : Sq::PairsMethod<void, void>     // <-- 1. inherit C++ member function(method)
#else
struct SqPairs
#endif
{
	SQ_PAIRS_MEMBERS;                            // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void         **data;
	int            length;

	// ------ SqPairs members ------
	int            sorted;
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

template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::add(Key *key, Value *value) {
	sq_pairs_add((SqPairs*)this, (void*)key, (void*)value);
}
template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::remove(Key *key) {
	sq_pairs_erase((SqPairs*)this, (void*)key);
}
template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::erase(Key *key) {
	sq_pairs_erase((SqPairs*)this, (void*)key);
}
template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::steal(Key *key) {
	sq_pairs_steal((SqPairs*)this, (void*)key);
}
template<typename Key, typename Value>
inline Value *PairsMethod<Key, Value>::find(Key *key) {
	return (Value*)sq_pairs_find((SqPairs*)this, (void*)key);
}
template<typename Key, typename Value>
inline void   PairsMethod<Key, Value>::sort() {
	sq_pairs_sort((SqPairs*)this);
}

/* All derived struct/class must be C++11 standard-layout. */

template<typename Key = char, typename Value = Key>
struct Pairs : Sq::PairsMethod<Key, Value>
{
	SQ_PAIRS_MEMBERS;                            // <-- 2. inherit member variable
/*	// ------ SqArray members ------
	void         **data;
	int            length;

	// ------ SqPairs members ------
	int            sorted;
	SqCompareFunc  key_compare_func;
	SqDestroyFunc  key_destroy_func;
	SqDestroyFunc  value_destroy_func;
 */

	Pairs(SqCompareFunc keyCompareFunc = (SqCompareFunc)sq_pairs_cmp_string) {
		sq_pairs_init((SqPairs*)this, keyCompareFunc);
	}
	~Pairs() {
		sq_pairs_final((SqPairs*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_PAIRS_H
