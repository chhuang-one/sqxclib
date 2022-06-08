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

#ifndef SQDB_SQLITE_H
#define SQDB_SQLITE_H

#include <sqlite3.h>

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqdbSqlite          SqdbSqlite;
typedef struct SqdbConfigSqlite    SqdbConfigSqlite;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo         SqdbInfo_SQLite_;
#define SQDB_INFO_SQLITE    (&SqdbInfo_SQLite_)

#define sqdb_sqlite_new(sqdb_config)    sqdb_new(SQDB_INFO_SQLITE, sqdb_config)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

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
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

typedef struct SqdbConfigSqlite    DbConfigSqlite;

struct DbSqlite : SqdbSqlite
{
	DbSqlite(const SqdbConfigSqlite *config = NULL) {
		init(SQDB_INFO_SQLITE, (const SqdbConfig*)config);
	}
	~DbSqlite() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_SQLITE_H
