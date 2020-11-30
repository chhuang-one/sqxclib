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
#include <SqxcValue.h>
#include <SqxcSql.h>

#define NEW_TABLE_PREFIX_NAME    "new__table__"

static void sqdb_sqlite_recreate_table(SqdbSqlite* db, SqBuffer* sql_buf, SqTable* table);

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

#if DEBUG
static int debug_callback(void *user_data, int argc, char **argv, char **columnName);
#endif

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

static int int_callback(void *user_data, int argc, char **argv, char **columnName)
{
	*(int*)user_data = strtol(argv[0], NULL, 10);
	return 0;
}

static int  sqdb_sqlite_open(SqdbSqlite* sqdb, const char* database_name)
{
	char* ext = sqdb->extension;
	char* folder = sqdb->folder;
	char* buf;
	int   len;
	int   rc;

	if (ext == NULL)
		ext = "db";
	if (folder == NULL)
		folder = ".";
	len = snprintf(NULL, 0, "%s/%s.%s", folder, database_name, ext) + 1;
	buf = malloc(len);
	snprintf(buf, len, "%s/%s.%s", folder, database_name, ext);

	rc = sqlite3_open(buf, &sqdb->self);
	free(buf);

	if (rc != SQLITE_OK)
		return SQCODE_OPEN_FAIL;
	rc = sqlite3_exec(sqdb->self, "PRAGMA user_version;", int_callback, &sqdb->version, NULL);
	return SQCODE_OK;
}

static int  sqdb_sqlite_close(SqdbSqlite* sqdb)
{
	sqlite3_close(sqdb->self);
	return SQCODE_OK;
}

static int  sqdb_sqlite_migrate(SqdbSqlite* sqdb, SqSchema* schema, SqSchema* schema_next)
{
	SqBuffer    sql_buf;
	SqPtrArray  reentries;
	SqTable**   table_addr;
	SqTable*    table;
	SqType*     type;
	char*       errorMsg = NULL;
	int   rc;

	if (sqdb->self == NULL)
		return SQCODE_ERROR;

	// End of migration
	if (schema_next == NULL) {
		// Don't migrate if database schema version equal the latest schema version
		if (sqdb->version == schema->version)
			return SQCODE_OK;

		// trace renamed (or dropped) table/column that was referenced by others
		sq_schema_trace_foreign(schema);

		type = schema->type;
		sq_ptr_array_init(&reentries, type->n_entry - schema->offset, (SqDestroyFunc)sq_table_free);
		// move renamed and dropped (table's)record from schema->type->entry to another array
		for (int index = schema->offset;  index < type->n_entry;  index++) {
			table = (SqTable*)type->entry[index];
			if (table && table->old_name) {    // (table->bit_field & SQB_RENAMED) == 0
				sq_ptr_array_append(&reentries, table);
				type->entry[index] = NULL;
			}
		}
		// clear all renamed and dropped records
		// database schema version < (less than) current schema version
		sq_schema_clear_records(schema, '<');
		// sort schema->type->entry
		sq_type_sort_entry(type);

		// buffer for SQL statement
		sq_buffer_init(&sql_buf);

		// run SQL statement: rename and drop table
		for (int index = 0;  index < reentries.length;  index++) {
			table = reentries.data[index];
			if (table->name == NULL) {
				// this is dropped record
				table_addr = (SqTable**)reentries.data + index;
			}
			else {
				// this is renamed record
				table_addr = (SqTable**)sq_reentries_trace_renamed(&reentries, table->name, index+1, true);
				// if table doesn't rename again.
				if (table_addr == NULL)
					table_addr = (SqTable**)reentries.data + index;
			}

			sql_buf.writed = 0;
			// === DROP TABLE ===
			// if table has been dropped or dropped after renaming
			if (table_addr[0]->name == NULL)
				sqdb_sql_drop_table((Sqdb*)sqdb, &sql_buf, table);
			// === RENAME TABLE ===
			else {
				sq_buffer_write(&sql_buf, "ALTER TABLE \"");
				sq_buffer_write(&sql_buf, table->old_name);
				sq_buffer_write_c(&sql_buf, '"');

				sq_buffer_write(&sql_buf, " RENAME \"");
				sq_buffer_write(&sql_buf, table_addr[0]->name);
				sq_buffer_alloc(&sql_buf, 2);
				sql_buf.buf[sql_buf.writed -2] = '"';
				sql_buf.buf[sql_buf.writed -1] = ';';
				// check table existence
				table = (SqTable*)sq_type_find_entry(type, table_addr[0]->name, NULL);
				if (table) {
					table = *(SqTable**)table;
					// don't rename if table doesn't exist in database
					if ((table->bit_field & SQB_TABLE_SQL_CREATED) == 0) {
						sq_table_free(table_addr[0]);
						table_addr[0] = NULL;
						continue;
					}
				}
			}
			sq_table_free(table_addr[0]);
			table_addr[0] = NULL;

			// exec SQL statement
			sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#if DEBUG
			puts(sql_buf.buf);
			rc = sqlite3_exec(sqdb->self, sql_buf.buf, debug_callback, NULL, &errorMsg);
#else
			rc = sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, &errorMsg);
#endif
			if (rc != SQLITE_OK) {
				fprintf(stderr, "SQL error: %s\n", errorMsg);
				sqlite3_free(errorMsg);
			}
		}
		// free RENAME and DROP records
		sq_ptr_array_final(&reentries);

		sql_buf.writed = 0;
		sq_buffer_write(&sql_buf, "PRAGMA foreign_keys=off; BEGIN TRANSACTION; ");
		// run SQL statement: create/recreate table
		type = schema->type;
		for (int index = 0;  index < type->n_entry;  index++) {
			table = (SqTable*)type->entry[index];
			if (table == NULL)
				continue;
			// === CREATE TABLE ===
			if ((table->bit_field & SQB_TABLE_SQL_CREATED) == 0)
				sqdb_sql_create_table((Sqdb*)sqdb, &sql_buf, table, NULL);
			// === RECREATE TABLE ===
			else if (table->bit_field & SQB_TABLE_COL_CHANGED)
				sqdb_sqlite_recreate_table(sqdb, &sql_buf, table);
			table->bit_field |=  SQB_TABLE_SQL_CREATED;
			table->bit_field &= ~SQB_TABLE_COL_CHANGED;
		}
		sq_buffer_write(&sql_buf, " COMMIT; PRAGMA foreign_keys=on;");
		sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#ifdef DEBUG
		puts(sql_buf.buf);
		rc = sqlite3_exec(sqdb->self, sql_buf.buf, debug_callback, NULL, &errorMsg);
#else
		rc = sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, &errorMsg);
#endif
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", errorMsg);
			sqlite3_free(errorMsg);
		}
		// update SQLite.user_version
		sqdb->version = schema->version;
		sql_buf.writed = snprintf(NULL, 0, "PRAGMA user_version = %d;", sqdb->version) + 1;
		sprintf(sql_buf.buf, "PRAGMA user_version = %d;", sqdb->version);
//		sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#ifdef DEBUG
		puts(sql_buf.buf);
		rc = sqlite3_exec(sqdb->self, sql_buf.buf, debug_callback, NULL, &errorMsg);
#else
		rc = sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, &errorMsg);
#endif
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", errorMsg);
			sqlite3_free(errorMsg);
		}
		// free buffer for SQL statement
		sq_buffer_final(&sql_buf);

		// free temporary data after migration.
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

#ifdef DEBUG
static int debug_callback(void *user_data, int argc, char **argv, char **columnName)
{
	int i;

	for (i=0; i<argc; i++) {
		printf("%s = %s\n", columnName[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}
#endif  // DEBUG

static int  sqdb_sqlite_exec(SqdbSqlite* sqdb, const char* sql, Sqxc* xc, void* reserve)
{
	int   rc;
//	char* errorMsg;

	if (xc == NULL) {
#ifdef DEBUG
		rc = sqlite3_exec(sqdb->self, sql, debug_callback, NULL, NULL);
#else
		// no callback if xc is NULL
		rc = sqlite3_exec(sqdb->self, sql, NULL, NULL, NULL);
#endif
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

// ----------------------------------------------------------------------------

static void  sqdb_sqlite_recreate_table(SqdbSqlite* db, SqBuffer* sql_buf, SqTable* table)
{
	unsigned int  col_changed;
	SqPtrArray*   columns;

	col_changed = table->bit_field & SQB_TABLE_COL_CHANGED;
	columns = sq_type_get_ptr_array(table->type);

#if 0
	// if SQLite version > 3.20
	if (col_changed == SQB_TABLE_COL_RENAMED) {
		sqdb_sql_rename_column(db, sql_buf, table, column);
		return;
	}
#endif

	sq_buffer_write(sql_buf, "CREATE TABLE IF NOT EXISTS \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write_c(sql_buf, '\"');
	sqdb_sql_create_table_params((Sqdb*)db, sql_buf, columns);
//	sq_buffer_write_c(sql_buf, ';');

	// -- copy data from the table to the new_tables
	sq_buffer_write(sql_buf, " INSERT INTO \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\" (");
	sqdb_sql_write_column_list((Sqdb*)db, sql_buf, columns, false);
	sq_buffer_write(sql_buf, ") SELECT ");
	sqdb_sql_write_column_list((Sqdb*)db, sql_buf, columns, true);
	sq_buffer_write(sql_buf, " FROM \"");
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\"; ");

	// -- drop the table
	sq_buffer_write(sql_buf, "DROP TABLE \"");
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\"; ");

	// -- rename the new_table to the table
	sq_buffer_write(sql_buf, "ALTER TABLE \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\" ");
	sq_buffer_write(sql_buf, "RENAME TO \"");
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\"; ");
}
