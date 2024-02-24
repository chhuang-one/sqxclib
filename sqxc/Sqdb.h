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

/* ----------------------------------------------------------------------------
	Sqdb - Database interface for SQLite, MySQL...etc
 */

#ifndef SQDB_H
#define SQDB_H

#include <SqBuffer.h>
#include <SqSchema.h>
#include <Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct Sqdb             Sqdb;
typedef struct SqdbInfo         SqdbInfo;
typedef struct SqdbConfig       SqdbConfig;

typedef struct Sqxc             Sqxc;        // define in Sqxc.h

typedef enum SqdbProduct {
	SQDB_PRODUCT_UNKNOWN,
	SQDB_PRODUCT_SQLITE,
	SQDB_PRODUCT_MYSQL,
	SQDB_PRODUCT_POSTGRE,

	SQDB_PRODUCT_CUSTOM   = 10,
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
#define sqdb_exec(db, sql, xc, reserve)              \
		(db)->info->exec(db, sql, xc, reserve)

/* --- C Functions --- */

// if 'config' is NULL, program must set configure later
Sqdb   *sqdb_new(const SqdbInfo *info, const SqdbConfig *config);
void    sqdb_free(Sqdb *db);

void    sqdb_init(Sqdb *db, const SqdbInfo *info, const SqdbConfig *config);
void    sqdb_final(Sqdb *db);

/* --- execute SQL statement --- */

int  sqdb_exec_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns);
int  sqdb_exec_alter_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns, SqTable *old_table);

/* --- write SQL statement to 'sql_buf' --- */

// return number of columns in 'table'
int  sqdb_sql_create_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqPtrArray *arranged_columns, bool primary_first);
int  sqdb_sql_create_table_params(Sqdb *db, SqBuffer *sql_buf, SqPtrArray *arranged_columns, bool primary_first);
void sqdb_sql_rename_table(Sqdb *db, SqBuffer *sql_buf, const char *old_name, const char *new_name);
void sqdb_sql_drop_table(Sqdb *db, SqBuffer *sql_buf, SqTable *table, bool if_exist);

// if 'table_type' is NULL, it writes "SELECT * FROM table_name" to sql_buf.
//
// if SQ_CONFIG_QUERY_ONLY_COLUMN is 0, return NULL.
// otherwise return primary column if 'table_type->bit_field' has SQB_TYPE_QUERY_FIRST.
SqColumn *sqdb_sql_select(Sqdb *db, SqBuffer *sql_buf, const char *table_name, const SqType *table_type);

void sqdb_sql_delete(Sqdb *db, SqBuffer *sql_buf, const char *table_name);

void sqdb_sql_rename_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column, SqColumn *old_column);
void sqdb_sql_add_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
void sqdb_sql_alter_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
void sqdb_sql_drop_column(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);

void sqdb_sql_create_index(Sqdb *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);

/* --- write parameter,arguments to 'sql_buf' --- */

void sqdb_sql_write_identifier(Sqdb *db, SqBuffer *sql_buf, const char *identifier, bool with_brackets);
void sqdb_sql_write_column_type(Sqdb *db, SqBuffer *sql_buf, SqColumn *column);
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

namespace Sq {

/*	DbMethod is used by Sqdb and it's children.

	It's derived struct/class must be C++11 standard-layout and has Sqdb members.
 */
struct DbMethod
{
	void init(const SqdbInfo *info, const SqdbConfig *config);
	void init(const SqdbInfo *info, const SqdbConfig &config);
	void final(void);

	int  open(const char *name);
	int  close(void);
	int  exec(const char *sql, Sqxc *xc, void *reserve = NULL);
	int  exec(const char *sql, Sq::XcMethod *xc, void *reserve = NULL);
	int  migrate(SqSchema *schema_cur, SqSchema *schema_next);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqdbInfo - It is interface for database product.
 */

struct SqdbInfo
{
	uintptr_t      size;       // Sqdb instance size
	SqdbProduct    product;    // SQL  product code

	struct {
		unsigned int has_boolean:1;      // has Boolean Data Type
		unsigned int use_alter:1;        // use "ALTER COLUMN" to change column
		unsigned int use_modify:1;       // use "MODIFY COLUMN" to change column
	} column;

	// for  Database column and table identifiers
	struct {
		char         identifier[2];      // SQLite is "", MySQL is ``, SQL Server is []
	} quote;

	// initialize derived structure of Sqdb
	void (*init)(Sqdb *db, const SqdbConfig *config);
	// finalize derived structure of Sqdb
	void (*final)(Sqdb *db);

	// open a database file or establish a connection to a database server
	int  (*open)(Sqdb *db, const char *name);
	// close a previously opened file or connection.
	int  (*close)(Sqdb *db);
	// executes the SQL statement
	int  (*exec)(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
	// migrate schema. It apply changes of 'schema_next' to 'schema_current'
	int  (*migrate)(Sqdb *db, SqSchema *schema_current, SqSchema *schema_next);
};

/*	Sqdb - It is a base structure for database product (SQLite, MySQL...etc).

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
struct Sqdb : Sq::DbMethod                 // <-- 1. inherit member function(method)
#else
struct Sqdb
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ Sqdb members ------
	const SqdbInfo *info;

	// schema version in SQL database
	int             version;
 */
};

/*	SqdbConfig - setting of SQL product

	SqdbConfig must have no base struct because I need use aggregate initialization with it.
 */

#define SQDB_CONFIG_MEMBERS    \
	unsigned int    product;   \
	unsigned int    bit_field

struct SqdbConfig
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable
/*	// ------ SqdbConfig members ------
	unsigned int    product;      // reserve. value of enum SqdbProduct
	unsigned int    bit_field;    // reserve. Is the instance of config constant or dynamic?
 */
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define DbMethod functions. */

inline void DbMethod::init(const SqdbInfo *info, const SqdbConfig *config) {
	sqdb_init((Sqdb*)this, info, config);
}
inline void DbMethod::init(const SqdbInfo *info, const SqdbConfig &config) {
	sqdb_init((Sqdb*)this, info, &config);
}
inline void DbMethod::final(void) {
	sqdb_final((Sqdb*)this);
}

inline int  DbMethod::open(const char *name) {
	return sqdb_open((Sqdb*)this, name);
}
inline int  DbMethod::close(void) {
	return sqdb_close((Sqdb*)this);
}
inline int  DbMethod::exec(const char *sql, Sqxc *xc, void *reserve) {
	return sqdb_exec((Sqdb*)this, sql, xc, reserve);
}
inline int  DbMethod::exec(const char *sql, Sq::XcMethod *xc, void *reserve) {
	return sqdb_exec((Sqdb*)this, sql, (Sqxc*)xc, reserve);
}
inline int  DbMethod::migrate(SqSchema *schema_cur, SqSchema *schema_next) {
	return sqdb_migrate((Sqdb*)this, schema_cur, schema_next);
}

/* All derived struct/class must be C++11 standard-layout. */

struct Db : Sqdb
{
	// destructor
	~Db(void) {
		sqdb_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQDB_H
