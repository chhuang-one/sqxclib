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

/* ----------------------------------------------------------------------------
	Sqdb - Database interface for SQLite, MySQL...etc
 */

#ifndef SQDB_H
#define SQDB_H

#include <SqBuffer.h>
#include <SqSchema.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct Sqdb             Sqdb;
typedef struct SqdbInfo         SqdbInfo;
typedef struct SqdbConfig       SqdbConfig;

typedef struct Sqxc             Sqxc;        // define in Sqxc.h

typedef enum SqdbProduct {
	SQDB_PRODUCT_UNKNOWN,
	SQDB_PRODUCT_SQLITE,
	SQDB_PRODUCT_MYSQL,
} SqdbProduct;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- macro functions --- parameter used only once in macro (except parameter 'db') */

// int  sqdb_open(Sqdb *db, const char *database_name);
#define sqdb_open(db, database_name)    (db)->info->open(db, database_name)

// int  sqdb_close(Sqdb *db);
#define sqdb_close(db)                  (db)->info->close(db)

// int  sqdb_migrate(Sqdb *db, SqSchema *schema_cur, SqSchema *schema_next);
#define sqdb_migrate(db, schema_cur, schema_next)    \
		(db)->info->migrate(db, schema_cur, schema_next)

// int  sqdb_exec(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
#define sqdb_exec(db, sql, xc, reserve)    \
		(db)->info->exec(db, sql, xc, reserve)

/* --- C Functions --- */

// if 'config' is NULL, program must set configure later
Sqdb   *sqdb_new(const SqdbInfo *info, SqdbConfig *config);
void    sqdb_free(Sqdb *db);

/* --- execute SQL statement --- */

int  sqdb_exec_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns);
int  sqdb_exec_alter_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns, SqTable *table_data);

/* --- write SQL statement to 'sql_buf' --- */

// return number of columns in 'table'
int  sqdb_sql_create_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns, bool primary_first);
int  sqdb_sql_create_table_params(Sqdb *db, SqBuffer *sql_buf, SqPtrArray *arranged_columns, bool primary_first);
void sqdb_sql_rename_table(Sqdb *db, SqBuffer *sql_buf, const char *old_name, const char *new_name);
void sqdb_sql_drop_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, bool if_exist);

void sqdb_sql_from(Sqdb *db, SqBuffer *sql_buf, SqTable *table, bool is_delete);

void sqdb_sql_rename_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column, SqColumn *column_data);
void sqdb_sql_add_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
void sqdb_sql_alter_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
void sqdb_sql_drop_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);

void sqdb_sql_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
void sqdb_sql_drop_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);

/* --- write parameter,arguments to 'sql_buf' --- */

void sqdb_sql_write_column(Sqdb *db, SqBuffer *sql_buf, SqColumn *column, const char *column_name);
void sqdb_sql_write_constraint(Sqdb *db, SqBuffer *sql_buf, SqColumn *column);
void sqdb_sql_write_composite_columns(Sqdb *db, SqBuffer *sql_buf, SqColumn *column);
void sqdb_sql_write_foreign_ref(Sqdb *db, SqBuffer *sql_buf, SqColumn *column);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq
{

/* DbMethod : a C++ struct is used by Sqdb and it's children. */

// This one is for derived use only, it must has Sqdb data members.
// Your derived struct/class must be C++11 standard-layout.
struct DbMethod
{
	int  open(const char *name);
	int  close(void);
	int  exec(const char *sql, Sqxc *xc, void *reserve);
	int  migrate(SqSchema *schema_cur, SqSchema *schema_next);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/* SqdbInfo - It is data and function interface for database product.
 */

struct SqdbInfo
{
	uintptr_t      size;       // Sqdb instance size
	SqdbProduct    product;    // SqdbProduct product = SQLite, MySQL...etc

	struct {
		unsigned int has_boolean:1;      // has Boolean Data Type
		unsigned int use_alter:1;        // ALTER COLUMN
		unsigned int use_modify:1;       // MODIFY COLUMN
	} column;

	// for  Database column/table identifiers
	struct {
		char         identifier[2];      // SQLite is "", MySQL is ``, SQL Server is []
	} quote;

	// initialize derived structure of Sqdb
	void (*init)(Sqdb *db, SqdbConfig *config);
	// finalize derived structure of Sqdb
	void (*final)(Sqdb *db);

	// open a database file or establish a connection to a database server
	int  (*open)(Sqdb *db, const char *name);
	// close a previously opened file or connection.
	int  (*close)(Sqdb *db);
	// executes the SQL statement
	int  (*exec)(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
	// migrate schema from 'schema_next' to 'schema_cur'
	int  (*migrate)(Sqdb *db, SqSchema *schema_cur, SqSchema *schema_next);
};

/*
   Sqdb - It is a base structure for database product (SQLite, MySQL...etc).

   TODO: Sqdb should be thread safe...

   The correct way to derive Sqdb:  (conforming C++11 standard-layout)
   1. Use Sq::DbMethod to inherit member function(method).
   2. Use SQDB_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

#define SQDB_MEMBERS           \
	const SqdbInfo *info;      \
	int             version

#ifdef __cplusplus
struct Sqdb : Sq::DbMethod           // <-- 1. inherit member function(method)
#else
struct Sqdb
#endif
{
	SQDB_MEMBERS;                    // <-- 2. inherit member variable
/*	// ------ Sqdb members ------
	const SqdbInfo *info;

	// schema version in SQL database
	int             version;
 */

	/* Add variable and function */  // <-- 3. Add variable and non-virtual function in derived struct.
};

/* SqdbConfig - setting of SQL product */

#define SQDB_CONFIG_MEMBERS    \
	unsigned int    product;   \
	unsigned int    bit_field

struct SqdbConfig
{
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	unsigned int    product;
	unsigned int    bit_field;    // reserve. constant or dynamic config data
 */
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- define methods for Sq::Db --- */

inline int  DbMethod::open(const char *name) {
	return sqdb_open((Sqdb*)this, name);
}
inline int  DbMethod::close(void) {
	return sqdb_close((Sqdb*)this);
}
inline int  DbMethod::exec(const char *sql, Sqxc *xc, void *reserve) {
	return sqdb_exec((Sqdb*)this, sql, xc, reserve);
}
inline int  DbMethod::migrate(SqSchema *schema_cur, SqSchema *schema_next) {
	return sqdb_migrate((Sqdb*)this, schema_cur, schema_next);
}

/* --- define C++11 standard-layout structures --- */

// This one is for directly use only. You can NOT derived it.
typedef struct Sqdb        Db;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQDB_H
