/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#ifndef SQDB_SQLITE_H
#define SQDB_SQLITE_H

#include <sqlite3.h>

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ macro and type/structue declaration

typedef struct SqdbSqlite          SqdbSqlite;
typedef struct SqdbConfigSqlite    SqdbConfigSqlite;

// ----------------------------------------------------------------------------
// C data and functions declaration

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo    *SQDB_INFO_SQLITE;

#define sqdb_sqlite_new(sqdb_config)    sqdb_new(SQDB_INFO_SQLITE, sqdb_config)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ structue definition

/*
    SqdbSqlite - Sqdb for SQLite

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
struct SqdbSqlite : Sq::DbMethod           // <-- 1. inherit member function(method)
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

/*
    SqdbConfigSqlite - SqdbSqlite use this configure

    SqdbConfig
    |
    `--- SqdbConfigSqlite

    Note: use 'const char*' to declare string here, C++ user can initialize static struct easily.
 */
struct SqdbConfigSqlite
{
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;
 */

	// ------ SqdbConfigSqlite members ------
	const char     *folder;
	const char     *extension;   // optional
};

// ----------------------------------------------------------------------------
// C++ data, methods, and functions declaration

#ifdef __cplusplus

namespace Sq {

typedef struct SqdbConfigSqlite    DbConfigSqlite;

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct DbSqlite : SqdbSqlite
{
	DbSqlite(SqdbConfigSqlite *config = NULL) {
		this->info = SQDB_INFO_SQLITE;  SQDB_INFO_SQLITE->init((Sqdb*)this, (SqdbConfig*)config);
	}
	~DbSqlite() {
		SQDB_INFO_SQLITE->final((Sqdb*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_SQLITE_H
