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

#include <SqxcValue.h>  // TODO: SqdbHelper functions
#include <SqxcSql.h>

static void sqdb_sqlite_init(SqdbSqlite* sqdb, SqdbConfigSqlite* config);
static void sqdb_sqlite_final(SqdbSqlite* sqdb);
static int  sqdb_sqlite_open(SqdbSqlite* sqdb, const char* database_name);
static int  sqdb_sqlite_close(SqdbSqlite* sqdb);
static int  sqdb_sqlite_exec(SqdbSqlite* sqdb, const char* sql, Sqxc* xc, void* reserve);
static int  sqdb_sqlite_migrate(SqdbSqlite* sqdb, SqSchema* schema, SqSchema* schema_next);

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbSqlite),
	.product = SQDB_PRODUCT_SQLITE,
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

static void sqdb_sqlite_init(SqdbSqlite* sqdb, SqdbConfigSqlite* config_src)
{
	if (config_src) {
		sqdb->extension = strdup(config_src->extension);
		sqdb->folder = strdup(config_src->folder);
	}
	else {
		sqdb->extension = NULL;
		sqdb->folder = NULL;
	}
}

static void sqdb_sqlite_final(SqdbSqlite* sqdb)
{
	free(sqdb->extension);
	free(sqdb->folder);
}

static int  sqdb_sqlite_open(SqdbSqlite* sqdb, const char* database_name)
{
	char* ext = sqdb->extension;
	char* buf;
	int   len;
	int   rc;

	if (ext == NULL)
		ext = "db";
	len = snprintf(NULL, 0, "%s/%s.%s", sqdb->folder, database_name, ext) + 1;
	buf = malloc(len);
	snprintf(buf, len, "%s/%s.%s", sqdb->folder, database_name, ext);

	rc = sqlite3_open(buf, &sqdb->self);
	free(buf);

	if (rc != SQLITE_OK)
		return SQCODE_OPEN_FAIL;
	return SQCODE_OK;
}

static int  sqdb_sqlite_close(SqdbSqlite* sqdb)
{
	sqlite3_close(sqdb->self);
	return SQCODE_OK;
}

// support RENAME COLUMN statement if SQlite >= 3.20.0
static int  sqdb_sqlite_migrate(SqdbSqlite* sqdb, SqSchema* schema, SqSchema* schema_next)
{
//	SqBuffer* buffer;

	// End of migration
	if (schema_next == NULL) {
		// Don't migrate if database schema version equal the latest schema version
		if (sqdb->version == schema->version)
			return SQCODE_OK;

		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_foreign(schema);

		// run SQL statement: rename/drop table

		// clear changed records and remove NULL records in schema
		// database schema version < (less than) current schema version
		sq_schema_clear_records(schema, '<');

		// run SQL statement: create/recreate table

		sq_schema_complete(schema);
		return SQCODE_OK;
	}

	// include and apply changes
	sq_schema_include(schema, schema_next);

	// current database schema
	if (sqdb->version == schema->version) {
		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_foreign(schema);
		// clear changed records and remove NULL records in schema
		// database schema version = (equal) current schema version
		sq_schema_clear_records(schema, '=');
	}

	return SQCODE_OK;
}

static int query_callback(void *user_data, int argc, char **argv, char **columnName)
{
	Sqxc* xc = *(Sqxc**)user_data;
	int   index;

	xc->type = SQXC_TYPE_OBJECT;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);
	// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
	if (xc->code != SQCODE_OK)
		return 1;

	for (index = 0;  index < argc;  index++) {
		xc->type = SQXC_TYPE_STRING;
		xc->name = columnName[index];
		xc->value.string = argv[index];
		xc = sqxc_send(xc);
#ifdef DEBUG
		// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
		if (xc->code != SQCODE_OK)
			return 1;
#endif  // DEBUG
	}

	xc->type = SQXC_TYPE_OBJECT_END;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);
#ifdef DEBUG
	// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
	if (xc->code != SQCODE_OK)
		return 1;
#endif  // DEBUG

	// xc may be changed by sqxc_send()
	*(Sqxc**)user_data = xc;

	return 0;
}

static int insert_callback(void *user_data, int argc, char **argv, char **columnName)
{
	Sqxc* xc = (Sqxc*)user_data;

#if 0
	if (((SqxcSql*)xc)->row_count > 0) {
		for (int index = 0;  index < argc;  index++)
			row[index] = strtol(argv[index], NULL, 10);
	}
#else
	sqxc_sql_id(xc) = strtol(argv[0], NULL, 10);
#endif

	return 0;
}

static int  sqdb_sqlite_exec(SqdbSqlite* sqdb, const char* sql, Sqxc* xc, void* reserve)
{
	int   rc;
//	char* errorMsg;

	if (xc == NULL) {
		// no callback if xc is NULL
		rc = sqlite3_exec(sqdb->self, sql, NULL, NULL, NULL);
	}
	else {
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifdef DEBUG
			if (xc == NULL || xc->info != SQXC_INFO_VALUE)
				return SQCODE_EXEC_ERROR;
#endif
			// if Sqxc element prepare for multiple row
			if (sqxc_value_current(xc) == sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}
			rc = sqlite3_exec(sqdb->self, sql, query_callback, &xc, NULL);
			// if Sqxc element prepare for multiple row
			if (sqxc_value_current(xc) == sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY_END;
				xc->name = NULL;
//				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}
			break;

		case 'I':    // INSERT
		case 'i':    // insert
#ifdef DEBUG
			if (xc == NULL || xc->info != SQXC_INFO_SQL)
				return SQCODE_EXEC_ERROR;
#endif
		default:
			rc = sqlite3_exec(sqdb->self, sql, insert_callback, xc, NULL);
			break;
		}
	}

//	sqlite3_free(errorMsg);
	if (rc != SQLITE_OK)
		return SQCODE_EXEC_ERROR;
	return SQCODE_OK;
}
