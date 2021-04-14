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
#include <SqdbSqlite.h>
#include <SqxcValue.h>
#include <SqxcSql.h>
#include <SqRelation-migration.h>

#define NEW_TABLE_PREFIX_NAME          "new__table__"
#define SQLITE_VERSION_NUMBER_3_20     3020000           // 3.20.0

static void sqdb_sqlite_init(SqdbSqlite *sqdb, SqdbConfigSqlite *config);
static void sqdb_sqlite_final(SqdbSqlite *sqdb);
static int  sqdb_sqlite_open(SqdbSqlite *sqdb, const char *database_name);
static int  sqdb_sqlite_close(SqdbSqlite *sqdb);
static int  sqdb_sqlite_exec(SqdbSqlite *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_sqlite_migrate(SqdbSqlite *sqdb, SqSchema *schema, SqSchema *schema_next);

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbSqlite),
	.product = SQDB_PRODUCT_SQLITE,
	.column  = {
		.has_boolean = 1,
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

const SqdbInfo *SQDB_INFO_SQLITE = &dbinfo;

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_sqlite_recreate_table(SqdbSqlite *db, SqBuffer *sql_buf,
                                       SqTable *table, int n_existed_columns);
static bool sqdb_sqlite_alter_table(SqdbSqlite *db, SqBuffer *sql_buf,
                                    SqTable *table, int n_existed_columns);
#if DEBUG
static int debug_callback(void *user_data, int argc, char **argv, char **columnName);
#endif

static void sqdb_sqlite_init(SqdbSqlite *sqdb, SqdbConfigSqlite *config_src)
{
	if (config_src) {
		sqdb->extension = strdup(config_src->extension);
		sqdb->folder = strdup(config_src->folder);
	}
	else {
		sqdb->extension = NULL;
		sqdb->folder = NULL;
	}
	sqdb->version = 0;
}

static void sqdb_sqlite_final(SqdbSqlite *sqdb)
{
	free(sqdb->extension);
	free(sqdb->folder);
}

static int int_callback(void *user_data, int argc, char **argv, char **columnName)
{
	*(int*)user_data = strtol(argv[0], NULL, 10);
	return 0;
}

static int  sqdb_sqlite_open(SqdbSqlite *sqdb, const char *database_name)
{
	char *ext = sqdb->extension;
	char *folder = sqdb->folder;
	char *buf;
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

static int  sqdb_sqlite_close(SqdbSqlite *sqdb)
{
	sqlite3_close(sqdb->self);
	return SQCODE_OK;
}

// synchronize schema to database
static int  sqdb_sqlite_migrate_sync(SqdbSqlite *sqdb, SqSchema *schema)
{
	SqRelationNode *node;
	SqBuffer    sql_buf;
	SqTable    *table;
	SqType     *type;
	char       *errorMsg;
	int   rc;

	// Don't synchronize if database schema version greater than or equal to the latest schema version
	// --- database schema version > the latest schema version
	if (sqdb->version > schema->version)
		return SQCODE_CAN_NOT_SYNC;
	// --- database schema version == the latest schema version
	if (sqdb->version == schema->version) {
		sq_schema_complete(schema, false);
		return SQCODE_OK;
	}
	type = (SqType*)schema->type;
	// buffer for SQL statement
	sq_buffer_init(&sql_buf);

	// remove NULL tables in schema
	schema->offset = sq_reentries_remove_null(sq_type_get_ptr_array(schema->type), schema->offset);

	// trace renamed (or dropped) table/column that was referenced by others
	sq_schema_trace_name(schema);

#if DEBUG
	fprintf(stderr, "SQLite: BEGIN TRANSACTION ------\n");
#endif
	rc = sqlite3_exec(sqdb->self, "PRAGMA foreign_keys=off; BEGIN TRANSACTION", NULL, NULL, &errorMsg);
	if (rc != SQLITE_OK)    // error occurred
		goto atExit;

	// --------- rename and drop table ---------
	node = sq_relation_find(schema->relation, SQ_TYPE_RESERVE, NULL);
	if (node) {
		for (;  node->next;  node = node->next) {
			SqTable *table = node->next->object;
			// clear sql_buf
			sql_buf.writed = 0;
			// === DROP TABLE ===
			if (table->name == NULL)
				sqdb_sql_drop_table((Sqdb*)sqdb, &sql_buf, table, true);
			// === RENAME TABLE ===
			else
				sqdb_sql_rename_table((Sqdb*)sqdb, &sql_buf, table->old_name, table->name);

			// exec SQL statement
#if DEBUG
			fprintf(stderr, "SQL: %s\n", sql_buf.buf);
#endif
			sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, NULL);
		}
	}

	// --------- create and recreate table ---------
	for (int index = 0;  index < type->n_entry;  index++) {
		table = (SqTable*)type->entry[index];
		// remove NULL columns in table
		table->offset = sq_reentries_remove_null(sq_type_get_ptr_array(table->type), table->offset);
		// clear sql_buf
		sql_buf.writed = 0;
		// === CREATE TABLE ===
		if ((table->bit_field & SQB_TABLE_SQL_CREATED) == 0) {
			if (sqdb_sql_create_table((Sqdb*)sqdb, &sql_buf, table, NULL) > 0)
				sq_buffer_write_c(&sql_buf, ';');
			else
				sql_buf.writed = 0;
			sqdb_exec_create_index((Sqdb*)sqdb, &sql_buf, table, NULL);
		}
		// === ALTER TABLE ===
		else if (table->bit_field & SQB_TABLE_COL_CHANGED) {
			if (sqdb_sqlite_alter_table(sqdb, &sql_buf, table, table->offset) == false) {
				// === RECREATE TABLE ===
				sqdb_sqlite_recreate_table(sqdb, &sql_buf, table, table->offset);
			}
		}

		// exec SQL statement
		if (sql_buf.writed > 0) {
			sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#ifdef DEBUG
			fprintf(stderr, "SQL: %s\n", sql_buf.buf);
#endif
			rc = sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, &errorMsg);
			if (rc != SQLITE_OK) {
				// error occurred
#ifdef DEBUG
				fprintf(stderr, "SQLite: ROLLBACK TRANSACTION ------\n");
#endif
				sqlite3_exec(sqdb->self, "ROLLBACK", NULL, NULL, NULL);
				goto atExit;
			}
		}
	}
#ifdef DEBUG
	fprintf(stderr, "SQLite: END TRANSACTION ------\n");
#endif
	rc = sqlite3_exec(sqdb->self, "COMMIT; PRAGMA foreign_keys=on", NULL, NULL, &errorMsg);
	if (rc != SQLITE_OK)    // error occurred
		goto atExit;

	// clear all renamed and dropped records
	// database schema version = (equal) current schema version
	sq_schema_erase_records(schema, '=');
	// sort table/column by name and free temporary data after migration.
	sq_schema_complete(schema, false);

	// update SQLite.user_version
	sqdb->version = schema->version;
	sql_buf.writed = 0;
	sq_buffer_alloc(&sql_buf, snprintf(NULL, 0, "PRAGMA user_version = %d", sqdb->version));
	sprintf(sql_buf.buf, "PRAGMA user_version = %d", sqdb->version);
//	sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#ifdef DEBUG
	fprintf(stderr, "SQL: %s\n", sql_buf.buf);
#endif
	rc = sqlite3_exec(sqdb->self, sql_buf.buf, NULL, NULL, &errorMsg);

atExit:
	// free buffer for SQL statement
	sq_buffer_final(&sql_buf);

	if (rc != SQLITE_OK) {
		// error occurred
#ifdef DEBUG
		fprintf(stderr, "SQLite: %s\n", errorMsg);
#endif
		sqlite3_free(errorMsg);
		return SQCODE_ERROR;
	}
	return SQCODE_OK;
}

static int  sqdb_sqlite_migrate(SqdbSqlite *sqdb, SqSchema *schema, SqSchema *schema_next)
{
	if (sqdb->self == NULL)
		return SQCODE_ERROR;

	// If 'schema_next' is NULL, synchronize schema to database
	if (schema_next == NULL)
		return sqdb_sqlite_migrate_sync(sqdb, schema);

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

static int query_callback(void *user_data, int argc, char **argv, char **columnName)
{
	Sqxc *xc = *(Sqxc**)user_data;
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
	Sqxc *xc = (Sqxc*)user_data;

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

	fprintf(stderr, "SQLite callback: ");
	for (i=0; i<argc; i++)
		fprintf(stderr, "%s = %s\n", columnName[i], argv[i] ? argv[i] : "NULL");
	return 0;
}
#endif  // DEBUG

static int  sqdb_sqlite_exec(SqdbSqlite *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
	int   rc;
	char *errorMsg;

#ifdef DEBUG
	fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL) {
#ifdef DEBUG
		rc = sqlite3_exec(sqdb->self, sql, debug_callback, NULL, &errorMsg);
#else
		rc = sqlite3_exec(sqdb->self, sql, NULL, NULL, &errorMsg);
#endif
	}
	else {
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifdef DEBUG
			if (xc->info != SQXC_INFO_VALUE) {
				fprintf(stderr, "sqdb_sqlite_exec(): SELECT command must use with SqxcValue.\n");
				return SQCODE_EXEC_ERROR;
			}
#endif
			// if Sqxc element prepare for multiple row
			if (sqxc_value_current(xc) == sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}
			rc = sqlite3_exec(sqdb->self, sql, query_callback, &xc, &errorMsg);
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
			if (xc->info != SQXC_INFO_SQL) {
				fprintf(stderr, "sqdb_sqlite_exec(): INSERT command must use with SqxcSql.\n");
				return SQCODE_EXEC_ERROR;
			}
#endif
		default:
			rc = sqlite3_exec(sqdb->self, sql, insert_callback, xc, &errorMsg);
			break;
		}
	}

	// check return value of sqlite3_exec()
	if (rc != SQLITE_OK) {
#if DEBUG
		fprintf(stderr, "SQLite: %s\n", errorMsg);
		sqlite3_free(errorMsg);
#endif
		return SQCODE_EXEC_ERROR;
	}
	return SQCODE_OK;
}

// ----------------------------------------------------------------------------

static void  sqdb_sqlite_recreate_table(SqdbSqlite *db, SqBuffer *sql_buf,
                                        SqTable *table, int n_existed_columns)
{
	SqPtrArray   *columns;

	columns = sq_type_get_ptr_array(table->type);

	sq_buffer_write(sql_buf, "CREATE TABLE IF NOT EXISTS \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write_c(sql_buf, '\"');
	sqdb_sql_create_table_params((Sqdb*)db, sql_buf, columns, n_existed_columns);
	sq_buffer_write_c(sql_buf, ';');

	// -- copy data from the table to the new_tables
	sq_buffer_write(sql_buf, " INSERT INTO \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\" (");
	sqdb_sql_write_column_list((Sqdb*)db, sql_buf, columns, n_existed_columns, false);
	sq_buffer_write(sql_buf, ") SELECT ");
	sqdb_sql_write_column_list((Sqdb*)db, sql_buf, columns, n_existed_columns, true);
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

static bool sqdb_sqlite_alter_table(SqdbSqlite *db, SqBuffer *sql_buf,
                                    SqTable *table, int n_existed_columns)
{
	SqRelationNode *node;
	SqColumn     *column;
	SqPtrArray   *array = sq_type_get_ptr_array(table->type);
	unsigned int  unsupported = SQB_TABLE_COL_ADDED_CURRENT_TIME |
	                            SQB_TABLE_COL_ADDED_EXPRESSION |
		                        SQB_TABLE_COL_ADDED_CONSTRAINT |
	                            SQB_TABLE_COL_ADDED_UNIQUE |
	                            SQB_TABLE_COL_DROPPED |
	                            SQB_TABLE_COL_ALTERED;

	// SQlite >= 3.20.0 support RENAME COLUMN statement.
	if (sqlite3_libversion_number() < SQLITE_VERSION_NUMBER_3_20)
		unsupported |= SQB_TABLE_COL_RENAMED;
	// check
	if (table->bit_field & unsupported)
		return false;

	// RENAME COLUMN
	node = sq_relation_find(table->relation, SQ_TYPE_RESERVE, NULL);
	if (node) {
		for (node = node->next;  node;  node = node->next) {
			column = node->object;
			sqdb_sql_rename_column((Sqdb*)db, sql_buf, table, column);
			sq_buffer_write_c(sql_buf, ';');
		}
	}

	// ADD COLUMN / INDEX / FOREIGN KEY
	for (int index = n_existed_columns;  index < array->length;  index++) {
		column = array->data[index];
		sqdb_sql_add_column((Sqdb*)db, sql_buf, table, column);
#ifdef SQ_CONFIG_SQL_COLUMN_NOT_NULL_WITHOUT_DEFAULT
		// Program can add default value if newly added column has "NOT NULL" without "DEFAULT".
		if ((column->bit_field & SQB_NULLABLE)==0 && column->default_value == NULL) {
			sq_buffer_write(sql_buf, " DEFAULT ");
			if (SQ_TYPE_IS_ARITHMETIC(column->type))
				sq_buffer_write(sql_buf, "0");
			else
				sq_buffer_write(sql_buf, "''");
		}
#endif  // SQ_CONFIG_SQL_COLUMN_NOT_NULL_WITHOUT_DEFAULT
		sq_buffer_write_c(sql_buf, ';');
	}

	return true;
}
