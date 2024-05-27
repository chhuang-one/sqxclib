/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#ifndef SQDB_SQLITE_H
#define SQDB_SQLITE_H

#include <sqlite3.h>

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqdbSqlite          SqdbSqlite;
typedef struct SqdbConfigSqlite    SqdbConfigSqlite;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo         sqdbInfo_SQLite;
#define SQDB_INFO_SQLITE    (&sqdbInfo_SQLite)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqdbSqlite - Sqdb for SQLite

	Sqdb
	|
	`--- SqdbSqlite

	The correct way to derive Sqdb:  (conforming C++11 standard-layout)
	1. Use Sq::DbMethod to inherit member function(method).
	2. Use SQDB_MEMBERS to inherit member variable.
	3. Add variable and non-virtual function in derived struct.
	** This can keep std::is_standard_layout<>::value == true
 */

#ifdef __cplusplus
struct SqdbSqlite : Sq::DbMethod           // <-- 1. inherit C++ member function(method)
#else
struct SqdbSqlite
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ Sqdb members ------
	const SqdbInfo *info;

	// schema version in SQL database
	int             version;
 */

	// ------ SqdbSqlite members ------    // <-- 3. Add variable and non-virtual function in derived struct.
	sqlite3        *self;
	char           *folder;
	char           *extension;   // optional
};

/*	SqdbConfigSqlite - SqdbSqlite use this to configure database connection

	SqdbConfig
	|
	`--- SqdbConfigSqlite

	SqdbConfigSqlite must have no base struct because I need use aggregate initialization with it.

	Note: use 'const char*' to declare string here, C++ user can initialize static struct easily.
 */
struct SqdbConfigSqlite
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;
 */

	// ------ SqdbConfigSqlite members ------
	const char     *folder;
	const char     *extension;   // optional
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
Sqdb *sqdb_sqlite_new(const SqdbConfigSqlite *config) {
	return sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*)config);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

Sqdb *sqdb_sqlite_new(const SqdbConfigSqlite *config);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

typedef struct SqdbConfigSqlite    DbConfigSqlite;

struct DbSqlite : SqdbSqlite
{
	// constructor
	DbSqlite(const SqdbConfigSqlite *config = NULL) {
		init(SQDB_INFO_SQLITE, (const SqdbConfig*)config);
	}
	DbSqlite(const SqdbConfigSqlite &config) {
		init(SQDB_INFO_SQLITE, (const SqdbConfig&)config);
	}
	// destructor
	~DbSqlite() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_SQLITE_H
