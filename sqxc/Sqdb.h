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

/* ----------------------------------------------------------------------------
	Sqdb - Database interface for SQLite, MySQL...etc
 */

#ifndef SQDB_H
#define SQDB_H

#include <SqBuffer.h>
#include <SqSchema.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Sqdb             Sqdb;
typedef struct SqdbInfo         SqdbInfo;
typedef struct SqdbConfig       SqdbConfig;

typedef struct Sqxc             Sqxc;        // declare in Sqxc.h

typedef enum SqdbProduct {
	SQDB_PRODUCT_UNKNOWN,
	SQDB_PRODUCT_SQLITE,
	SQDB_PRODUCT_MYSQL,
} SqdbProduct;

// ----------------------------------------------------------------------------
// macro functions - parameter used only once in macro (except parameter 'db')

// int  sqdb_open(Sqdb* db, const char* database_name);
#define sqdb_open(db, database_name)    (db)->info->open(db, database_name)

// int  sqdb_close(Sqdb* db);
#define sqdb_close(db)                  (db)->info->close(db)

// int  sqdb_migrate(Sqdb* db, SqSchema* schema_cur, SqSchema* schema_next);
#define sqdb_migrate(db, schema_cur, schema_next)    \
		(db)->info->migrate(db, schema_cur, schema_next)

// int  sqdb_exec(Sqdb* db, const char* sql, Sqxc* xc, void* reserve);
#define sqdb_exec(db, sql, xc, reserve)    \
		(db)->info->exec(db, sql, xc, reserve)

// ----------------------------------------------------------------------------
// C Functions

// if 'config' is NULL, program must set configure later
Sqdb*   sqdb_new(const SqdbInfo *info, SqdbConfig* config);
void    sqdb_free(Sqdb* db);

// for testing
void sqdb_schema_to_sql(Sqdb* db, SqBuffer* buffer, SqSchema* schema, SqPtrArray* arranged_tables);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// DbMethod : a C++ struct is used by SqDb and it's children.

#ifdef __cplusplus

namespace Sq
{

// This one is for derived use only, it must has Sqdb data members.
// Your derived struct/class must be C++11 standard-layout.
struct DbMethod
{
	int  open(const char* name);
	int  close(void);
	int  exec(const char* sql, Sqxc* xc, void* reserve);
	int  migrate(SqSchema* schema_cur, SqSchema* schema_next);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// SqdbInfo

struct SqdbInfo
{
	uintptr_t      size;       // instance size
	SqdbProduct    product;    // SqdbProduct product = SQLite, MySQL...etc

	struct {
		unsigned int use_alter:1;        // ALTER COLUMN
		unsigned int use_modify:1;       // MODIFY COLUMN
	} column;

	void (*init)(Sqdb* db, SqdbConfig* config);
	void (*final)(Sqdb* db);

	int  (*open)(Sqdb* db, const char* name);
	int  (*close)(Sqdb* db);
	int  (*exec)(Sqdb* db, const char* sql, Sqxc* xc, void* reserve);
	int  (*migrate)(Sqdb* db, SqSchema* schema_cur, SqSchema* schema_next);
};

/* ----------------------------------------------------------------------------
   Sqdb - database interface.

   TODO: Sqdb should be thread safe...
 */

#define SQDB_MEMBERS           \
	const SqdbInfo *info

#ifdef __cplusplus
struct Sqdb : Sq::DbMethod
#else
struct Sqdb
#endif
{
	SQDB_MEMBERS;
/*	// ------ Sqdb members ------
	const SqdbInfo *info;
 */
};

// ----------------------------------------------------------------------------
// SqdbConfig - setting of SQL product

#define SQDB_CONFIG_MEMBERS    \
	const SqdbInfo *info;      \
	unsigned int    bit_field

struct SqdbConfig
{
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	const SqdbInfo *info;
	unsigned int    bit_field;    // reserve
 */
};

// ----------------------------------------------------------------------------
// Sq::Db C++ functions

#ifdef __cplusplus

namespace Sq
{

int  DbMethod::open(const char* name)
	{ return sqdb_open((Sqdb*)this, name); }
int  DbMethod::close(void)
	{ return sqdb_close((Sqdb*)this); }
int  DbMethod::exec(const char* sql, Sqxc* xc, void* reserve)
	{ return sqdb_exec((Sqdb*)this, sql, xc, reserve); }
int  DbMethod::migrate(SqSchema* schema_cur, SqSchema* schema_next)
	{ return sqdb_migrate((Sqdb*)this, schema_cur, schema_next); }

// ----------------------------------------------------------------------------
// Db is C++11 standard-layout

// This one is for directly use only. You can NOT derived it.
typedef struct Sqdb        Db;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQDB_H
