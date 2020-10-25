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

#ifndef SQDB_EMPTY_H
#define SQDB_EMPTY_H

#include <Sqdb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqdbEmpty          SqdbEmpty;
typedef struct SqdbConfigEmpty    SqdbConfigEmpty;

extern const SqdbInfo*    SQDB_INFO_EMPTY;

#ifdef __cplusplus
}  // extern "C"
#endif

/* ----------------------------------------------------------------------------
    SqdbEmpty - Sqdb for debug/sample

    Sqdb
    |
    `--- SqdbEmpty
 */

#ifdef __cplusplus
struct SqdbEmpty : Sq::DbMethod
#else
struct SqdbEmpty
#endif
{
	SQDB_MEMBERS;
/*	// ------ Sqdb members ------
	const SqdbInfo *info;
 */

	// ------ SqdbEmpty members ------
	int    version;     // schema version in SQL database

	void*  instance;
};

/* ----------------------------------------------------------------------------
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


#endif  // SQDB_EMPTY_H
