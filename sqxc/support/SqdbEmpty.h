/*
 *   Copyright (C) 2020-2026 by C.H. Huang
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

#ifndef SQDB_EMPTY_H
#define SQDB_EMPTY_H

#include <sqxc/Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqdbEmpty          SqdbEmpty;
typedef struct SqdbConfigEmpty    SqdbConfigEmpty;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo        sqdbInfo_Empty;
#define SQDB_INFO_EMPTY    (&sqdbInfo_Empty)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqdbEmpty - Sqdb for debug/sample

	Sqdb
	|
	`--- SqdbEmpty
 */

#ifdef __cplusplus
struct SqdbEmpty : Sq::DbMethod            // <-- 1. inherit C++ member function(method)
#else
struct SqdbEmpty
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ Sqdb members ------
	const SqdbInfo *info;

	// schema version of the currently opened database
	int             version;
 */

	// ------ SqdbEmpty members ------     // <-- 3. Add variable and non-virtual function in derived struct.
	void  *instance;
};

/*	SqdbConfigEmpty - SqdbEmpty use this to configure database connection

	SqdbConfig
	|
	`--- SqdbConfigEmpty

	SqdbConfigEmpty must have no base struct because I need use aggregate initialization with it.
 */
struct SqdbConfigEmpty
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;   // reserve
 */

	// ------ SqdbConfigEmpty members ------
	int   debug;
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
Sqdb *sqdb_empty_new(const SqdbConfigEmpty *config) {
	return sqdb_new(SQDB_INFO_EMPTY, (SqdbConfig*)config);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

Sqdb *sqdb_empty_new(const SqdbConfigEmpty *config);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

typedef struct SqdbConfigEmpty    DbConfigEmpty;

struct DbEmpty : SqdbEmpty
{
	// constructor
	DbEmpty(const SqdbConfigEmpty *config = NULL) {
		init(SQDB_INFO_EMPTY, (const SqdbConfig*)config);
	}
	// destructor
	~DbEmpty() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_EMPTY_H
