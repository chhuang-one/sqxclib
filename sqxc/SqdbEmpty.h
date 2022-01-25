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

#ifndef SQDB_EMPTY_H
#define SQDB_EMPTY_H

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqdbEmpty          SqdbEmpty;
typedef struct SqdbConfigEmpty    SqdbConfigEmpty;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo    *SQDB_INFO_EMPTY;

#define sqdb_empty_new(sqdb_config)    sqdb_new(SQDB_INFO_EMPTY, sqdb_config)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
    SqdbEmpty - Sqdb for debug/sample

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

	// schema version in SQL database
	int             version;
 */

	// ------ SqdbEmpty members ------     // <-- 3. Add variable and non-virtual function in derived struct.
	void  *instance;
};

/*
    SqdbConfigEmpty - SqdbEmpty use this configure

    SqdbConfig
    |
    `--- SqdbConfigEmpty
 */
struct SqdbConfigEmpty
{
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;   // reserve
 */

	// ------ SqdbConfigEmpty members ------
	int   debug;
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

typedef struct SqdbConfigEmpty    DbConfigEmpty;

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct DbEmpty : SqdbEmpty
{
	DbEmpty(const SqdbConfigEmpty *config = NULL) {
		init(SQDB_INFO_EMPTY, (const SqdbConfig*)config);
	}
	~DbEmpty() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_EMPTY_H
