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

#ifndef SQ_DEFINE_H
#define SQ_DEFINE_H

#include <stdint.h>     // int64_t, intptr_t...etc
#include <stdbool.h>    // bool
#include <string.h>     // strcmp() for Sq::compare<char*>
#include <time.h>       // time_t

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef union SqValue        SqValue;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef void  (*SqInitFunc)(void *value);
typedef void  (*SqFinalFunc)(void *value);
typedef void  (*SqClearFunc)(void *value);
typedef void  (*SqDestroyFunc)(void *value);
typedef int   (*SqCompareFunc)(const void *a, const void *b);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

union SqValue
{
	bool          bool_;
	bool          boolean;
	int           integer;
	int           int_;
	unsigned int  uinteger;
	unsigned int  uint;
	int64_t       int64;
	uint64_t      uint64;
	time_t        time;
	time_t        rawtime;
	double        fraction;
	double        double_;
	const char   *str;
	const char   *string;
	const char   *stream;      // Text stream must be null-terminated string
	void         *pointer;
	void         *ptr;
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* Sq::compare template function for SqCompareFunc */

// actual parameter type:
// int  compare(Type *a, Type *b);
template<typename Type>
static int  compare(const void *a, const void *b);

/* template specialization for Sq::compare */

// actual parameter type:
// int  compare<int>(int *a, int *b)
template<>
inline int  compare<int>(const void *a, const void *b) {
	return *(int*)a - *(int*)b;
}

// actual parameter type:
// int  compare<double>(double *a, double *b)
template<>
inline int  compare<double>(const void *a, const void *b) {
	return (*(double*)a > *(double*)b) - (*(double*)a < *(double*)b);
}

// actual parameter type:
// int  compare<float>(float *a, float *b)
template<>
inline int  compare<float>(const void *a, const void *b) {
	return (*(float*)a > *(float*)b) - (*(float*)a < *(float*)b);
}

// actual parameter type:
// int  compare<char*>(char **a, char **b)
template<>
inline int  compare<char*>(const void *a, const void *b) {
	return strcmp(*(char**)a, *(char**)b);
}

// actual parameter type:
// int  compare<const char*>(const char **a, const char **b)
template<>
inline int  compare<const char*>(const void *a, const void *b) {
	return strcmp(*(char**)a, *(char**)b);
}

// default Sq::compare template function
// actual parameter type:
// int  compare(Type *a, Type *b) {
template<typename Type>
inline int  compare(const void *a, const void *b) {
	if (*(Type*)a < *(Type*)b) return -1;
	if (*(Type*)a > *(Type*)b) return 1;
	return 0;
}

/* All derived struct/class must be C++11 standard-layout. */

typedef union SqValue    Value;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_DEFINE_H
