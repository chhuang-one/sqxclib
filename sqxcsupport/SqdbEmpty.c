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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>        // fprintf(), stderr

#include <SqError.h>
#include <Sqdb-migration.h>
#include <SqdbEmpty.h>

#include <SqxcValue.h>
#include <SqxcSql.h>

static void sqdb_empty_init(SqdbEmpty *sqdb, const SqdbConfigEmpty *config);
static void sqdb_empty_final(SqdbEmpty *sqdb);
static int  sqdb_empty_open(SqdbEmpty *sqdb, const char *database_name);
static int  sqdb_empty_close(SqdbEmpty *sqdb);
static int  sqdb_empty_exec(SqdbEmpty *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_empty_migrate(SqdbEmpty *sqdb, SqSchema *schema, SqSchema *schema_next);

const SqdbInfo sqdbInfo_Empty = {
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

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_empty_init(SqdbEmpty *sqdb, const SqdbConfigEmpty *config_src)
{
	if (config_src == NULL) {
		// use default value to replace 'config_src'.
	}

	// initialize SqdbEmpty
}

static void sqdb_empty_final(SqdbEmpty *sqdb)
{
	// finalize SqdbEmpty
}

static int  sqdb_empty_open(SqdbEmpty *sqdb, const char *database_name)
{
	// connect to Database product and open database
	return SQCODE_OK;
}

static int  sqdb_empty_close(SqdbEmpty *sqdb)
{
	// close database and disconnect
	return SQCODE_OK;
}

static int  sqdb_empty_migrate(SqdbEmpty *db, SqSchema *schema, SqSchema *schema_next)
{
	SqTable    *table;
	SqPtrArray *reentries;

	// If 'schema_next' is NULL, synchronize schema to database. This is mainly used by SQLite.
	if (schema_next == NULL) {
		return SQCODE_OK;
	}

	if (db->version < schema_next->version) {
		// do migrations by 'schema_next'
		reentries = sq_type_entry_array(schema_next->type);
		for (int index = 0;  index < reentries->length;  index++) {
			table = (SqTable*)reentries->data[index];

			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
				// sqdb_exec_alter_table() execute SQL statements to alter table.
			}
			else if (table->name == NULL) {
				// DROP TABLE
				// sqdb_sql_drop_table() write SQL statements to buffer. user must execute statement in buffer.
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
				// sqdb_sql_rename_table() write SQL statements to buffer. user must execute statement in buffer.
			}
			else {
				// CREATE TABLE
				// sqdb_sql_create_table() write SQL statements to buffer. user must execute statement in buffer.
				// sqdb_exec_create_index() execute SQL statements to create indexes.
			}
		}

		// update database version
		db->version = schema_next->version;
	}

	// include and apply changes from 'schema_next'
	sq_schema_update(schema, schema_next);
	schema->version = schema_next->version;

	return SQCODE_OK;
}

static int  sqdb_empty_exec(SqdbEmpty *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
#ifndef NDEBUG
	fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL) {
		// execute SQL statement directly
		// return SQCODE_EXEC_ERROR if error occur
		return SQCODE_OK;
	}

	// Determines command based on the first character in SQL statement.
	switch (sql[0]) {
	case 'S':    // SELECT
	case 's':    // select
#ifndef NDEBUG
		if (xc == NULL || xc->info != SQXC_INFO_VALUE)
			return SQCODE_EXEC_ERROR;
#endif
/*
		// if Sqxc element prepare for multiple row
		if (sqxc_value_container(xc)) {
			xc->type = SQXC_TYPE_ARRAY;
			xc->name = NULL;
			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}

		// get rows from Database product and send them to Sqxc element 'xc'

		// if Sqxc element prepare for multiple row
		if (sqxc_value_container(xc)) {
			xc->type = SQXC_TYPE_ARRAY_END;
			xc->name = NULL;
//			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}
 */
		break;

	case 'I':    // INSERT
	case 'i':    // insert
#ifndef NDEBUG
		if (xc == NULL || xc->info != SQXC_INFO_SQL)
			return SQCODE_EXEC_ERROR;
#endif
		// execute SQL statement...

		// set the last inserted row id
		((SqxcSql*)xc)->id = 0;
		break;

	case 'U':    // UPDATE
	case 'u':    // update
#ifndef NDEBUG
		if (xc == NULL || xc->info != SQXC_INFO_SQL)
			return SQCODE_EXEC_ERROR;
#endif
		// execute SQL statement...

		// set number of rows changed
		((SqxcSql*)xc)->changes = 0;
		break;

	default:
		break;
	}

	return SQCODE_OK;
}
