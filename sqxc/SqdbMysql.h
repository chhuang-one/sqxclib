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

#ifndef SQDB_MYSQL_H
#define SQDB_MYSQL_H

/* --- set 0 or 1 to choose one of include path for mysql.h --- */
#if 1
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqdbMysql           SqdbMysql;
typedef struct SqdbConfigMysql     SqdbConfigMysql;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqdbInfo        SqdbInfo_MySQL_;
#define SQDB_INFO_MYSQL    (&SqdbInfo_MySQL_)

#define sqdb_mysql_new(sqdb_config)    sqdb_new(SQDB_INFO_MYSQL, sqdb_config)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqdbMysql - Sqdb for MySQL

	Sqdb
	|
	`--- SqdbMysql

	The correct way to derive Sqdb:  (conforming C++11 standard-layout)
	1. Use Sq::DbMethod to inherit member function(method).
	2. Use SQDB_MEMBERS to inherit member variable.
	3. Add variable and non-virtual function in derived struct.
	** This can keep std::is_standard_layout<>::value == true
 */

#ifdef __cplusplus
struct SqdbMysql : Sq::DbMethod            // <-- 1. inherit C++ member function(method)
#else
struct SqdbMysql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ Sqdb members ------
	const SqdbInfo *info;

	// schema version in SQL database
	int             version;
 */

	// ------ SqdbMysql members ------     // <-- 3. Add variable and non-virtual function in derived struct.
	MYSQL *self;
	const SqdbConfigMysql *config;
};

/*	SqdbConfigMysql - SqdbMysql use this to configure database connection

	SqdbConfig
	|
	`--- SqdbConfigMysql

	SqdbConfigMysql must have no base struct because I need use aggregate initialization with it.

	Note: use 'const char*' to declare string here, C++ user can initialize static struct easily.
 */
struct SqdbConfigMysql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;
 */

	// ------ SqdbConfigMysql members ------
	const char   *host;
	unsigned int  port;
	const char   *user;
	const char   *password;
	const char   *db;
};


// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

typedef struct SqdbConfigMysql    DbConfigMysql;

struct DbMysql : SqdbMysql
{
	DbMysql(const SqdbConfigMysql *config = NULL) {
		init(SQDB_INFO_MYSQL, (const SqdbConfig*)config);
	}
	~DbMysql() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQDB_MYSQL_H
