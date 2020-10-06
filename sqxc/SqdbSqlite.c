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

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqdbSqlite.h>

/* ----------------------------------------------------------------------------
    SqdbSqlite

    Sqdb
    |
    `--- SqdbSqlite
 */
typedef struct SqdbSqlite    SqdbSqlite;

struct SqdbSqlite
{
	SQDB_MEMBERS;
/*	// ------ Sqdb members ------
	SqdbInfo*      info;
 */

	// ------ SqdbSqlite members ------
    sqlite3*       self;
	int            version;      // schema version in SQL database
	char*          folder;
	char*          extension;    // optional
};

static void sqdb_sqlite_init(SqdbSqlite* db, SqdbConfigSqlite* config);
static void sqdb_sqlite_final(SqdbSqlite* db);
static int  sqdb_sqlite_open(SqdbSqlite* db, const char* database_name);
static int  sqdb_sqlite_close(SqdbSqlite* db);
static int  sqdb_sqlite_exec(SqdbSqlite* db, const char* sql, Sqxc* xc);
static int  sqdb_sqlite_migrate(SqdbSqlite* db, SqSchema* schema, SqSchema* schema_next);

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbSqlite),
	.product = SQDB_PRODUCT_SQLITE,
    .xcinfo  = NULL,
	.column  = {
	 	.use_alter  = 1,
		.use_modify = 0,
	},

	.init    = (void*)sqdb_sqlite_init,
	.final   = (void*)sqdb_sqlite_final,
    .open    = (void*)sqdb_sqlite_open,
    .close   = (void*)sqdb_sqlite_close,
    .exec    = (void*)sqdb_sqlite_exec,
    .migrate = (void*)sqdb_sqlite_migrate,
};

const SqdbInfo* SQDB_INFO_SQLITE = &dbinfo;

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_sqlite_init(SqdbSqlite* db, SqdbConfigSqlite* config_src)
{
	db->extension = strdup(config_src->extension);
	db->folder = strdup(config_src->folder);
}

static void sqdb_sqlite_final(SqdbSqlite* db)
{
	free(db->extension);
	free(db->folder);
}

static int  sqdb_sqlite_open(SqdbSqlite* db, const char* database_name)
{
	char* ext = db->extension;
	char* buf;
	int   len;
	int   rc;

	if (ext == NULL)
		ext = "db";
	len = snprintf(NULL, 0, "%s/%s.%s", db->folder, database_name, ext) + 1;
	buf = malloc(len);
	snprintf(buf, len, "%s/%s.%s", db->folder, database_name, ext);

	rc = sqlite3_open(buf, &db->self);
	free(buf);

	if (rc != SQLITE_OK)
		return SQCODE_OPEN_FAIL;
	return SQCODE_OK;
}

static int  sqdb_sqlite_close(SqdbSqlite* db)
{
	sqlite3_close(db->self);
	return SQCODE_OK;
}

// support RENAME COLUMN statement if SQlite >= 3.20.0
static int  sqdb_sqlite_migrate(SqdbSqlite* db, SqSchema* schema, SqSchema* schema_next)
{
//	SqBuffer* buffer;

	if (schema_next == NULL) {
		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_foreign(schema);

		// SQL command...
		// drop, rename, alter, or recreate table

		// erase changed records and remove NULL records in schema
		sq_schema_clear_records(schema, 1, 0);
		return SQCODE_OK;
	}

	// include and apply changes
	sq_schema_include(schema, schema_next);
	if (db->version == schema->version) {
		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_foreign(schema);
		// erase changed records and remove NULL records in schema
		sq_schema_clear_records(schema, 0, SQB_TABLE_SQL_CREATED);
	}

	return SQCODE_OK;
}

static int callback(void *user_data, int argc, char **argv, char **columnName)
{
	return 0;
}

static int  sqdb_sqlite_exec(SqdbSqlite* db, const char* sql, Sqxc* xc)
{
	int  rc;
#if 0
	char* errorMsg;

	rc = sqlite3_exec(db->dbobj, sql, callback, xc, &errorMsg);
	sqlite3_free(errorMsg);
#else
	rc = sqlite3_exec(db->self, sql, callback, xc, NULL);
#endif
	if (rc != SQLITE_OK)
		return SQCODE_EXEC_ERROR;
	return SQCODE_OK;
}
