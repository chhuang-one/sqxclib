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

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqdbEmpty.h>

#include <SqxcValue.h>  // TODO: SqdbHelper functions
#include <SqxcSql.h>

static void sqdb_empty_init(SqdbEmpty *sqdb, SqdbConfigEmpty *config);
static void sqdb_empty_final(SqdbEmpty *sqdb);
static int  sqdb_empty_open(SqdbEmpty *sqdb, const char *database_name);
static int  sqdb_empty_close(SqdbEmpty *sqdb);
static int  sqdb_empty_exec(SqdbEmpty *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_empty_migrate(SqdbEmpty *sqdb, SqSchema *schema, SqSchema *schema_next);

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbEmpty),
	.product = SQDB_PRODUCT_UNKNOWN,
	.column  = {
	 	.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}
	},

	.init    = (void*)sqdb_empty_init,
	.final   = (void*)sqdb_empty_final,
    .open    = (void*)sqdb_empty_open,
    .close   = (void*)sqdb_empty_close,
    .exec    = (void*)sqdb_empty_exec,
    .migrate = (void*)sqdb_empty_migrate,
};

const SqdbInfo *SQDB_INFO_EMPTY = &dbinfo;

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_empty_init(SqdbEmpty *sqdb, SqdbConfigEmpty *config_src)
{
	if (config_src) {
		// setup Sqdb
	}
	else {
		// apply default value
	}
}

static void sqdb_empty_final(SqdbEmpty *sqdb)
{
}

static int  sqdb_empty_open(SqdbEmpty *sqdb, const char *database_name)
{
	return SQCODE_OK;
}

static int  sqdb_empty_close(SqdbEmpty *sqdb)
{
	return SQCODE_OK;
}

static int  sqdb_empty_migrate(SqdbEmpty *sqdb, SqSchema *schema, SqSchema *schema_next)
{
//	SqBuffer *buffer;

	// synchronize schema to database.
	if (schema_next == NULL) {
		// Don't migrate if database schema equal the latest schema
		if (sqdb->version == schema->version)
			return SQCODE_OK;

		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_name(schema);

		// erase renamed & dropped records in schema
		// database schema version < (less than) current schema version
		sq_schema_erase_records(schema, '<');

		// free temporary data after migration.
		sq_schema_complete(schema, true);
		return SQCODE_OK;
	}

	if (sqdb->version < schema_next->version) {
		// Do migration
	}

	// include and apply changes
	sq_schema_include(schema, schema_next);

	// current database schema
	if (sqdb->version == schema->version) {
		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_name(schema);
		// clear changed records and remove NULL records in schema
		// database schema version = (equal) current schema version
		sq_schema_erase_records(schema, '=');
	}

	return SQCODE_OK;
}

static int  sqdb_empty_exec(SqdbEmpty *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
	switch (sql[0]) {
	case 'S':    // SELECT
	case 's':    // select
#ifdef DEBUG
		if (xc == NULL || xc->info != SQXC_INFO_VALUE)
			return SQCODE_EXEC_ERROR;
#endif
		break;

	case 'I':    // INSERT
	case 'i':    // insert
#ifdef DEBUG
		if (xc == NULL || xc->info != SQXC_INFO_SQL)
			return SQCODE_EXEC_ERROR;
#endif
	default:
		break;
	}

	return SQCODE_OK;
}
