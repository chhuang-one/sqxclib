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

#ifndef SQDB_H
#define SQDB_H

#include <sqlite3.h>

//#include <SqPtrArray.h>
#include <SqSchema.h>
#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Sqdb             Sqdb;
typedef struct SqdbInfo         SqdbInfo;
typedef struct SqdbConfig       SqdbConfig;
typedef enum   SqdbProduct      SqdbProduct;

enum SqdbProduct {
	SQDB_PRODUCT_UNKNOWN,
	SQDB_PRODUCT_SQLITE,
	SQDB_PRODUCT_MYSQL,
};

// ----------------------------------------------------------------------------
// SqdbInfo

struct SqdbInfo {
	uintptr_t      size;       // instance size
	SqdbProduct    product;    // SqdbProduct product = SQLite, MySQL...etc
	SqxcInfo*      xcinfo;

	struct {
		unsigned int use_alter:1;        // ALTER COLUMN
		unsigned int use_modify:1;       // MODIFY COLUMN
	} column;

	void (*init)(Sqdb* db, SqdbConfig* config);
	void (*final)(Sqdb* db);

	int  (*open)(Sqdb* db, const char* name);
	int  (*close)(Sqdb* db);
	int  (*exec)(Sqdb* db, const char* sql, Sqxc* xc);
	int  (*migrate)(Sqdb* db, SqSchema* schema_cur, SqSchema* schema_next);
};

// ----------------------------------------------------------------------------
// Sqdb

#define SQDB_MEMBERS           \
	SqdbInfo*      info

struct Sqdb {
	SQDB_MEMBERS;
/*	// ------ Sqdb members ------
	SqdbInfo*      info;
 */
};

// ----------------------------------------------------------------------------
// SqdbConfig - setting of SQL product

#define SQDB_CONFIG_MEMBERS    \
	SqdbInfo*      info;       \
	unsigned int   bit_field

struct SqdbConfig {
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	SqdbInfo*      info;
	unsigned int   bit_field;    // reserve
 */
};

// ----------------------------------------------------------------------------
// C Functions

Sqdb*   sqdb_new(SqdbInfo* info, SqdbConfig* config);
void    sqdb_free(Sqdb* db);

// int  sqdb_open(Sqdb* db, const char* database_name);
#define sqdb_open(db, database_name)    (db)->info->open(db, database_name)

// int  sqdb_close(Sqdb* db);
#define sqdb_close(db)                  (db)->info->close(db)

// int  sqdb_migrate(Sqdb* db, SqSchema* schema_cur, SqSchema* schema_next);
#define sqdb_migrate(db, schema_cur, schema_next)    \
		(db)->info->migrate(db, schema_cur, schema_next)

// int  sqdb_exec(Sqdb* db, const char* sql);
#define sqdb_exec(db, sql)              (db)->info->exec(db, sql)

// for testing
void sqdb_schema_to_sql(Sqdb* db, SqBuffer* buffer, SqSchema* schema, SqPtrArray* arranged_tables);


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQDB_H
