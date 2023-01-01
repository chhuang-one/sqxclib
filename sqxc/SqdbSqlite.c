/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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
#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqdbSqlite.h>
#include <SqxcValue.h>
#include <SqxcSql.h>
#include <SqRelation-migration.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#define strdup       _strdup
#endif

#define NEW_TABLE_PREFIX_NAME          "new__table__"
#define SQLITE_VERSION_NUMBER_3_20     3020000           // 3.20.0

static void sqdb_sqlite_init(SqdbSqlite *sqdb, const SqdbConfigSqlite *config);
static void sqdb_sqlite_final(SqdbSqlite *sqdb);
static int  sqdb_sqlite_open(SqdbSqlite *sqdb, const char *database_name);
static int  sqdb_sqlite_close(SqdbSqlite *sqdb);
static int  sqdb_sqlite_exec(SqdbSqlite *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_sqlite_migrate(SqdbSqlite *sqdb, SqSchema *schema, SqSchema *schema_next);

const SqdbInfo SqdbInfo_SQLite_ = {
	.size    = sizeof(SqdbSqlite),
	.product = SQDB_PRODUCT_SQLITE,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}
	},

	.init    = (void*)sqdb_sqlite_init,
	.final   = (void*)sqdb_sqlite_final,
	.open    = (void*)sqdb_sqlite_open,
	.close   = (void*)sqdb_sqlite_close,
	.exec    = (void*)sqdb_sqlite_exec,
	.migrate = (void*)sqdb_sqlite_migrate,
};

// ----------------------------------------------------------------------------
// SqdbInfo functions

static void sqdb_sqlite_recreate_table(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table);
static void sqdb_sqlite_create_dependent(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table);
static void sqdb_sqlite_create_trigger(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column);
static bool sqdb_sqlite_alter_table(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table);
#ifndef NDEBUG
static int  debug_callback(void *user_data, int argc, char **argv, char **columnName);
#endif

static void sqdb_sqlite_init(SqdbSqlite *sqdb, const SqdbConfigSqlite *config_src)
{
	if (config_src) {
		sqdb->extension = config_src->extension ? strdup(config_src->extension) : NULL;
		sqdb->folder    = config_src->folder    ? strdup(config_src->folder)    : NULL;
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
		return SQCODE_OPEN_FAILED;
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
		if (schema->relation) {
			// sq_schema_erase_records(schema, '=') was called by sqdb_sqlite_migrate()
			// when database schema version = (equal) current schema version

			// sort table and column by name and free temporary data after migration.
			sq_schema_complete(schema, false);
		}
		return SQCODE_OK;
	}
	type = (SqType*)schema->type;
	// buffer for SQL statement
	sq_buffer_init(&sql_buf);

	// trace renamed (or dropped) table and column that was referenced by others
	if (schema->relation)
		sq_schema_trace_name(schema);

#ifndef NDEBUG
	fprintf(stderr, "SQLite: BEGIN TRANSACTION ------\n");
#endif
	rc = sqlite3_exec(sqdb->self, "PRAGMA foreign_keys=off; BEGIN TRANSACTION", NULL, NULL, &errorMsg);
	if (rc != SQLITE_OK)    // error occurred
		goto atExit;

	// --------- rename and drop table ---------
	if (schema->relation)
		node = sq_relation_find(schema->relation, SQ_TYPE_UNSYNCED, NULL);
	else
		node = NULL;

	if (node) {
		for (;  node->next;  node = node->next) {
			SqTable *table = node->next->object;
			// clear sql_buf
			sql_buf.writed = 0;
			// === DROP TABLE ===
			if (table->name == NULL)
				sqdb_sql_drop_table((Sqdb*)sqdb, &sql_buf, table, true);
			// === RENAME TABLE ===
			else if (table->old_name)
				sqdb_sql_rename_table((Sqdb*)sqdb, &sql_buf, table->old_name, table->name);
			// === CREATE TABLE ===  Do it in next loop
			else
				continue;

			// exec SQL statement
#ifndef NDEBUG
			fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
			sqlite3_exec(sqdb->self, sql_buf.mem, NULL, NULL, NULL);
		}
	}

	// --------- create and recreate table ---------
	for (int index = 0;  index < type->n_entry;  index++) {
		table = (SqTable*)type->entry[index];
		// clear sql_buf
		sql_buf.writed = 0;
		// === CREATE TABLE ===
		if ((table->bit_field & SQB_TABLE_SQL_CREATED) == 0) {
			if (sqdb_sql_create_table((Sqdb*)sqdb, &sql_buf, table, NULL, true) > 0)
				sq_buffer_write_c(&sql_buf, ';');
			else
				sql_buf.writed = 0;
			sqdb_sqlite_create_dependent(sqdb, &sql_buf, table);
		}
		// === ALTER TABLE ===
		else if (table->bit_field & SQB_TABLE_COL_CHANGED) {
			if (sqdb_sqlite_alter_table(sqdb, &sql_buf, table) == false) {
				// === RECREATE TABLE ===
				sqdb_sqlite_recreate_table(sqdb, &sql_buf, table);
				sqdb_sqlite_create_dependent(sqdb, &sql_buf, table);
			}
		}

		// exec SQL statement
		if (sql_buf.writed > 0) {
			// 'sql_buf' must become null-terminated string before executing
			sq_buffer_write_c(&sql_buf, 0);
#ifndef NDEBUG
			fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
			rc = sqlite3_exec(sqdb->self, sql_buf.mem, NULL, NULL, &errorMsg);
			if (rc != SQLITE_OK) {
				// error occurred
#ifndef NDEBUG
				fprintf(stderr, "SQLite: ROLLBACK TRANSACTION ------\n");
#endif
				sqlite3_exec(sqdb->self, "ROLLBACK", NULL, NULL, NULL);
				goto atExit;
			}
		}
	}
#ifndef NDEBUG
	fprintf(stderr, "SQLite: END TRANSACTION ------\n");
#endif
	rc = sqlite3_exec(sqdb->self, "COMMIT; PRAGMA foreign_keys=on", NULL, NULL, &errorMsg);
	if (rc != SQLITE_OK)    // error occurred
		goto atExit;

	if (schema->relation) {
		// clear all renamed and dropped records
		// database schema version = (equal) current schema version
		sq_schema_erase_records(schema, '=');
		// sort table and column by name and free temporary data after migration.
		sq_schema_complete(schema, false);
	}

	// update SQLite.user_version
	sqdb->version = schema->version;
	sql_buf.writed = 0;
	sq_buffer_alloc(&sql_buf, snprintf(NULL, 0, "PRAGMA user_version = %d", sqdb->version));
	sprintf(sql_buf.mem, "PRAGMA user_version = %d", sqdb->version);
//	sq_buffer_write_c(&sql_buf, 0);  // null-terminated
#ifndef NDEBUG
	fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
	rc = sqlite3_exec(sqdb->self, sql_buf.mem, NULL, NULL, &errorMsg);

atExit:
	// free buffer for SQL statement
	sq_buffer_final(&sql_buf);

	if (rc != SQLITE_OK) {
		// error occurred
#ifndef NDEBUG
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
		// trace renamed (or dropped) table (and column) that was referenced by others
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

	// built-in types are not object
	if (SQ_TYPE_NOT_BUILTIN(sqxc_value_element(xc))) {
		xc->type = SQXC_TYPE_OBJECT;
		xc->name = NULL;
		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
		// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
		if (xc->code != SQCODE_OK)
			return 1;
	}

	for (index = 0;  index < argc;  index++) {
		xc->type = SQXC_TYPE_STRING;
		xc->name = columnName[index];
		xc->value.string = argv[index];
		xc = sqxc_send(xc);

#ifndef NDEBUG
		switch (xc->code) {
		case SQCODE_OK:
			break;

		case SQCODE_ENTRY_NOT_FOUND:
			fprintf(stderr, "sqdb_sqlite_exec(): column '%s' not found.\n", columnName[index]);
			break;

		default:
			fprintf(stderr, "sqdb_sqlite_exec(): error occurred during parsing column '%s'.\n", columnName[index]);
			// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
//			return 1;
			break;
		}
#endif  // NDEBUG
	}

	// built-in types are not object
	if (SQ_TYPE_NOT_BUILTIN(sqxc_value_element(xc))) {
		xc->type = SQXC_TYPE_OBJECT_END;
		xc->name = NULL;
		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
#ifndef NDEBUG
		// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
		if (xc->code != SQCODE_OK)
			return 1;
#endif  // NDEBUG
	}

	// xc may be changed by sqxc_send()
	*(Sqxc**)user_data = xc;

	return 0;
}

#ifndef NDEBUG
static int  debug_callback(void *user_data, int argc, char **argv, char **columnName)
{
	int i;

	fprintf(stderr, "SQLite callback: ");
	for (i=0; i<argc; i++)
		fprintf(stderr, "%s = %s\n", columnName[i], argv[i] ? argv[i] : "NULL");
	return 0;
}
#endif  // NDEBUG

static int  sqdb_sqlite_exec(SqdbSqlite *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
	int   rc;
	int   code = SQCODE_OK;
	char *errorMsg = NULL;

#ifndef NDEBUG
	fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL) {
#ifndef NDEBUG
		rc = sqlite3_exec(sqdb->self, sql, debug_callback, NULL, &errorMsg);
#else
		rc = sqlite3_exec(sqdb->self, sql, NULL, NULL, &errorMsg);
#endif
	}
	else {
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_VALUE) {
				fprintf(stderr, "sqdb_sqlite_exec(): SELECT command must use with SqxcValue.\n");
				return SQCODE_EXEC_ERROR;
			}
#endif
			// if Sqxc element prepare for multiple row
			if (sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}

			// set xc->code and call sqlite3_exec()
			xc->code = SQCODE_NO_DATA;
			rc = sqlite3_exec(sqdb->self, sql, query_callback, &xc, &errorMsg);
			// if the result set is empty.
			if(xc->code == SQCODE_NO_DATA)
				code = SQCODE_NO_DATA;

			// if Sqxc element prepare for multiple row
			if (sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY_END;
				xc->name = NULL;
//				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}
			break;

		case 'I':    // INSERT
		case 'i':    // insert
		case 'U':    // UPDATE
		case 'u':    // update
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_SQL) {
				fprintf(stderr, "sqdb_sqlite_exec(): INSERT and UPDATE command must use with SqxcSql.\n");
				return SQCODE_EXEC_ERROR;
			}
#endif
			// Don't break here
//			break;
		default:
			rc = sqlite3_exec(sqdb->self, sql, NULL, NULL, &errorMsg);
			// set the last inserted row id
			((SqxcSql*)xc)->id = sqlite3_last_insert_rowid(sqdb->self);
			// set number of rows changed
			((SqxcSql*)xc)->changes = sqlite3_changes(sqdb->self);
			break;
		}
	}

	// check return value of sqlite3_exec()
	if (rc != SQLITE_OK) {
#ifndef NDEBUG
		fprintf(stderr, "SQLite: %s\n", errorMsg);
#endif
		sqlite3_free(errorMsg);
		return SQCODE_EXEC_ERROR;
	}
	return code;
}

// ----------------------------------------------------------------------------

// write exist columns
void  sqdb_write_exist_column_list(Sqdb *db, SqBuffer *sql_buf, SqTable* table, bool old_name)
{
	SqColumn *column;
	SqType   *tabletype = (SqType*)table->type;
	char *allocated;
	int   count = 0;

	for (int index = 0;  index < tabletype->n_entry;  index++) {
		column = (SqColumn*)tabletype->entry[index];
		// skip SQ_TYPE_CONSTRAINT and SQ_TYPE_INDEX. They are fake types.
		if (SQ_TYPE_IS_FAKE(column->type))
			continue;
		// skip if column is newly added one (not exist in old table)
		if (sq_relation_find(table->relation, SQ_TYPE_UNSYNCED, column))
			continue;

		// write comma between two columns
		if (count > 0) {
			allocated = sq_buffer_alloc(sql_buf, 2);
			allocated[0] = ',';
			allocated[1] = ' ';
		}
		count++;
		// write column->old_name or column->name
		sq_buffer_write_c(sql_buf, db->info->quote.identifier[0]);
		if (old_name && column->old_name)
			sq_buffer_write(sql_buf, column->old_name);
		else
			sq_buffer_write(sql_buf, column->name);
		sq_buffer_write_c(sql_buf, db->info->quote.identifier[1]);
	}
}

static void  sqdb_sqlite_recreate_table(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table)
{
	SqPtrArray   *columns;

	columns = sq_type_get_ptr_array(table->type);

	sq_buffer_write(sql_buf, "CREATE TABLE IF NOT EXISTS \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write_c(sql_buf, '\"');
	sqdb_sql_create_table_params((Sqdb*)db, sql_buf, columns, true);
	sq_buffer_write_c(sql_buf, ';');

	// -- copy data from the table to the new_tables
	sq_buffer_write(sql_buf, " INSERT INTO \"" NEW_TABLE_PREFIX_NAME);
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "\" (");
	sqdb_write_exist_column_list((Sqdb*)db, sql_buf, table, false);
	sq_buffer_write(sql_buf, ") SELECT ");
	sqdb_write_exist_column_list((Sqdb*)db, sql_buf, table, true);
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

static void sqdb_sqlite_create_dependent(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table)
{
	SqColumn  *column;
	SqEntry  **cur, **end;

	end = table->type->entry + table->type->n_entry;
	for (cur = table->type->entry;  cur < end;  cur++) {
		column = *(SqColumn**)cur;
		// CREATE INDEX
		if (column->type == SQ_TYPE_INDEX) {
			sqdb_sql_create_index((Sqdb*)db, sql_buf, table, column);
			sq_buffer_write_c(sql_buf, ';');
		}
		// CREATE TRIGGER
		else if (column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE)
			sqdb_sqlite_create_trigger(db, sql_buf, table, column);
	}
}

static void sqdb_sqlite_create_trigger(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table, SqColumn *column)
{
	sq_buffer_write(sql_buf, "CREATE TRIGGER");

	// write "[trigger name]"
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ' ';
	sq_buffer_r_at(sql_buf, 0) = '[';
	sq_buffer_write(sql_buf, "sqxc_trig_");
	sq_buffer_write(sql_buf, table->name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = ']';
	sq_buffer_r_at(sql_buf, 0) = ' ';

	sq_buffer_write(sql_buf, "AFTER UPDATE ");

	sq_buffer_write(sql_buf, "ON");
	sqdb_sql_write_identifier((Sqdb*)db, sql_buf, table->name, false);

	// FOR EACH ROW
	// WHEN NEW.updated_at <= OLD.updated_at
	sq_buffer_write(sql_buf, "FOR EACH ROW " "WHEN NEW.");
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_alloc(sql_buf, 2);
	sq_buffer_r_at(sql_buf, 1) = '<';
	sq_buffer_r_at(sql_buf, 0) = '=';
	sq_buffer_write(sql_buf, "OLD.");
	sq_buffer_write(sql_buf, column->name);

	// BEGIN
	//   UPDATE table_name SET updated_at=CURRENT_TIMESTAMP WHERE id=OLD.id;
	// END;
	sq_buffer_write(sql_buf, " BEGIN " "UPDATE");
	sqdb_sql_write_identifier((Sqdb*)db, sql_buf, table->name, false);
	sq_buffer_write(sql_buf, "SET ");
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_write(sql_buf, "=CURRENT_TIMESTAMP WHERE ");

	column = sq_table_get_primary(table, NULL);
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_write(sql_buf, "=OLD.");
	sq_buffer_write(sql_buf, column->name);
	sq_buffer_write(sql_buf, "; END;");
}

static bool sqdb_sqlite_alter_table(SqdbSqlite *db, SqBuffer *sql_buf, SqTable *table)
{
	SqRelationNode *node;
	SqColumn     *column;
	unsigned int  unsupported = SQB_TABLE_COL_ADDED_CURRENT_TIME |
	                            SQB_TABLE_COL_ADDED_EXPRESSION |
	                            SQB_TABLE_COL_ADDED_CONSTRAINT |
	                            SQB_TABLE_COL_ADDED_UNIQUE |
	                            SQB_TABLE_COL_DROPPED |
	                            SQB_TABLE_COL_ALTERED;

	// SQlite >= 3.20.0 support RENAME COLUMN statement.
	if (sqlite3_libversion_number() < SQLITE_VERSION_NUMBER_3_20)
		unsupported |= SQB_TABLE_COL_RENAMED;

	// check unsupported
	if (table->bit_field & unsupported)
		return false;

	// do unsynced
	node = sq_relation_find(table->relation, SQ_TYPE_UNSYNCED, NULL);
	if (node) {
		// reverse node order because unsynced changes store in SqRelation is reverse order.
		node->next = sq_relation_node_reverse(node->next);
		// processing columns by original order
		for (node = node->next;  node;  node = node->next) {
			column = node->object;
			// INDEX
			if (column->type == SQ_TYPE_INDEX) {
				// DROP INDEX
				if (column->name == NULL)
					sqdb_sql_drop_column((Sqdb*)db, sql_buf, table, column);
				// CREATE INDEX
				else
					sqdb_sql_create_index((Sqdb*)db, sql_buf, table, column);
				sq_buffer_write_c(sql_buf, ';');
			}
			// RENAME COLUMN
			else if (column->old_name) {
				sqdb_sql_rename_column((Sqdb*)db, sql_buf, table, column, NULL);
				sq_buffer_write_c(sql_buf, ';');
				// clear renamed data
				free((char*)column->old_name);
				column->old_name = NULL;
				column->bit_field &= ~SQB_RENAMED;
			}
			// ADD COLUMN / UNIQUE / FOREIGN KEY
			else {
				sqdb_sql_add_column((Sqdb*)db, sql_buf, table, column);
				sq_buffer_write_c(sql_buf, ';');

				// ON UPDATE CURRENT_TIMESTAMP
				if (column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE)
					sqdb_sqlite_create_trigger(db, sql_buf, table, column);
			}
		}
	}

	return true;
}
