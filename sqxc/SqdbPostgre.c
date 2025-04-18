/*
 *   Copyright (C) 2022-2025 by C.H. Huang
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
#include <stdio.h>        // snprintf(), fprintf(), stderr
#include <stdbool.h>      // bool, true, false

#include <SqError.h>
#include <Sqdb-migration.h>
#include <SqdbPostgre.h>
#include <SqxcValue.h>
#include <SqxcSql.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#define strtoll      _strtoi64
#define strcasecmp   _stricmp
#endif

#define POSTGRE_DEFAULT_HOST      "localhost"
#define POSTGRE_DEFAULT_PORT      5432
#define POSTGRE_DEFAULT_USER      "postgres"
#define POSTGRE_DEFAULT_PASSWORD  ""

static void sqdb_postgre_init(SqdbPostgre *sqdb, const SqdbConfigPostgre *config);
static void sqdb_postgre_final(SqdbPostgre *sqdb);
static int  sqdb_postgre_open(SqdbPostgre *sqdb, const char *database_name);
static int  sqdb_postgre_close(SqdbPostgre *sqdb);
static int  sqdb_postgre_exec(SqdbPostgre *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_postgre_migrate(SqdbPostgre *sqdb, SqSchema *schema, SqSchema *schema_next);

static void sqdb_postgre_create_table_dep(SqdbPostgre *db, SqBuffer *sql_buf, SqTable *table);
static void sqdb_postgre_create_trigger(SqdbPostgre *db, SqBuffer *sql_buf, const char *table_name, const char *column_name);
static void sqdb_postgre_drop_table_dep(SqdbPostgre *db, SqBuffer *sql_buf, SqTable *table);
static void sqdb_postgre_drop_trigger(SqdbPostgre *sqdb, SqBuffer *sql_buf, const char *table_name, const char *column_name);
static void sqdb_postgre_alter_table(SqdbPostgre *sqdb, SqBuffer *sql_buf, SqTable *table, SqTable *old_table);
static int  sqdb_postgre_schema_get_version(SqdbPostgre *sqdb);
static void sqdb_postgre_schema_set_version(SqdbPostgre *sqdb, int version);

static const SqdbConfigPostgre db_default = {
	.host     = POSTGRE_DEFAULT_HOST,
	.port     = POSTGRE_DEFAULT_PORT,
	.user     = POSTGRE_DEFAULT_USER,
	.password = POSTGRE_DEFAULT_PASSWORD,
};

const SqdbInfo sqdbInfo_Postgres = {
	.size    = sizeof(SqdbPostgre),
	.product = SQDB_PRODUCT_POSTGRE,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}
	},

	.init    = (void*)sqdb_postgre_init,
	.final   = (void*)sqdb_postgre_final,
	.open    = (void*)sqdb_postgre_open,
	.close   = (void*)sqdb_postgre_close,
	.exec    = (void*)sqdb_postgre_exec,
	.migrate = (void*)sqdb_postgre_migrate,
};

// ----------------------------------------------------------------------------
// SqdbInfo functions

static void sqdb_postgre_init(SqdbPostgre *sqdb, const SqdbConfigPostgre *config)
{
	sqdb->conn = NULL;
	sqdb->config = config;
	sqdb->version = 0;
	if (config == NULL)
		sqdb->config = &db_default;
}

static void sqdb_postgre_final(SqdbPostgre *sqdb)
{
	if (sqdb->conn)
		PQfinish(sqdb->conn);
}

static int  sqdb_postgre_open(SqdbPostgre *sqdb, const char *database_name)
{
	const SqdbConfigPostgre *config = sqdb->config;
	char *port_str;
	int   len;

	if (database_name == NULL) {
		database_name = config->db;
		if (database_name == NULL)
			return SQCODE_OPEN_FAILED;
	}
	len = snprintf(NULL, 0, "%d", config->port) +1;
	port_str = malloc(len);
	sprintf(port_str, "%d", config->port);

	sqdb->conn = PQsetdbLogin(config->host, port_str, NULL, NULL,
			database_name, config->user, config->password);
	free(port_str);

	if (PQstatus(sqdb->conn) != CONNECTION_OK) {
		fprintf(stderr, "PostgreSQL: %s\n", PQerrorMessage(sqdb->conn));
		return SQCODE_OPEN_FAILED;
	}
	sqdb->version = sqdb_postgre_schema_get_version(sqdb);
	return SQCODE_OK;
}

static int  sqdb_postgre_close(SqdbPostgre *sqdb)
{
	if (sqdb->conn)
		PQfinish(sqdb->conn);
	sqdb->conn = NULL;
	return SQCODE_OK;
}

static int  sqdb_postgre_exec(SqdbPostgre *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
	PGresult  *results;
	int        n_fields;
	int        n_tuples;
	int        code = SQCODE_OK;
	// used by INSERT
	char *sql_new = NULL;

#ifndef NDEBUG
	fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL)
		results = PQexec(sqdb->conn, sql);
	else {
		// Determines command based on the first character in SQL statement.
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_VALUE) {
				fprintf(stderr, "%s: SELECT command must use with SqxcValue.\n",
				        "sqdb_postgre_exec()");
				return SQCODE_EXEC_ERROR;
			}
#endif
			results = PQexec(sqdb->conn, sql);
			if (PQresultStatus(results) != PGRES_TUPLES_OK)
				break;

			n_fields = PQnfields(results);
			n_tuples = PQntuples(results);
			// if the result set is empty.
			if (n_fields == 0 || n_tuples == 0) {
				xc->code = SQCODE_NO_DATA;
				code = SQCODE_NO_DATA;
				break;
			}

			// If SqxcValue is prepared to receive multiple rows
			if (sqxc_value_container(xc)) {
				// SQL multiple rows corresponds to SQXC_TYPE_ARRAY
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}

			for (int i = 0;  i < n_tuples;  i++) {
				// Special case:
				// Don't send object if user selects only one column and the column type is built-in types (not object).
				if (SQ_TYPE_NOT_BUILTIN(sqxc_value_element(xc))) {
					// SQL row corresponds to SQXC_TYPE_OBJECT
					xc->type = SQXC_TYPE_OBJECT;
					xc->name = NULL;
					xc->value.pointer = NULL;
					xc = sqxc_send(xc);
//					if (xc->code != SQCODE_OK)
//						break;
				}

				for (int j = 0;  j < n_fields;  j++) {
					xc->type = SQXC_TYPE_STR;
					xc->name = PQfname(results, j);
					xc->value.str = PQgetvalue(results, i, j);
					xc = sqxc_send(xc);
#ifndef NDEBUG
					switch (xc->code) {
					case SQCODE_OK:
						break;

					case SQCODE_ENTRY_NOT_FOUND:
						// warning
						fprintf(stderr, "%s: column '%s' not found.\n",
						        "sqdb_postgre_exec()", xc->name);
						break;

					default:
						fprintf(stderr, "%s: error occurred during parsing column '%s'.\n",
						        "sqdb_postgre_exec()", xc->name);
						break;
					}
#endif  // NDEBUG
				}

				// Special case:
				// Don't send object if user selects only one column and the column type is built-in types (not object).
				if (SQ_TYPE_NOT_BUILTIN(sqxc_value_element(xc))) {
					// SQL row corresponds to SQXC_TYPE_OBJECT
					xc->type = SQXC_TYPE_OBJECT_END;
					xc->name = NULL;
					xc->value.pointer = NULL;
					xc = sqxc_send(xc);
//					if (xc->code != SQCODE_OK)
//						break;
				}
			}

			// If SqxcValue is prepared to receive multiple rows
			if (sqxc_value_container(xc)) {
				// SQL multiple rows corresponds to SQXC_TYPE_ARRAY
				xc->type = SQXC_TYPE_ARRAY_END;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// for last inserted row id
			// The new SQL statment is sql  + " RETURNING id"         + "\0"
			// sql_new = malloc(strlen(sql) + strlen(" RETURNING id") + 1)
			sql_new = malloc(strlen(sql) + 14);    // + " RETURNING id" "\0"
			strcpy(sql_new, sql);
			strcat(sql_new, " RETURNING id");
			sql = sql_new;
			// Don't break here
//			break;
		case 'U':    // UPDATE
		case 'u':    // update
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_SQL) {
				fprintf(stderr, "%s: INSERT and UPDATE command must use with SqxcSql.\n",
				        "sqdb_postgre_exec()");
				free(sql_new);
				return SQCODE_EXEC_ERROR;
			}
#endif
			// Don't break here
//			break;
		default:
			results = PQexec(sqdb->conn, sql);
			// set the last inserted row id
			if (sql_new) {
				if (PQntuples(results) > 0)
					((SqxcSql*)xc)->id = (int64_t)strtoll(PQgetvalue(results, 0, 0), NULL, 10);
				else
					((SqxcSql*)xc)->id = 0;
				free(sql_new);
			}
			// set number of rows changed
			((SqxcSql*)xc)->changes = (int64_t)strtoll(PQcmdTuples(results), NULL, 10);
			break;
		}
	}

	if (PQresultStatus(results) != PGRES_COMMAND_OK && PQresultStatus(results) != PGRES_TUPLES_OK) {
#ifndef NDEBUG
		fprintf(stderr, "PostgreSQL: %s\n", PQerrorMessage(sqdb->conn));
//		psql_error("%s", PQerrorMessage(sqdb->conn));
#endif
		code = SQCODE_EXEC_ERROR;
	}
	PQclear(results);

	return code;
}

// used by sqdb_postgre_migrate()
static bool sqdb_postgre_has_error(SqdbPostgre *sqdb, PGresult *result)
{
	bool has_error;

	if (PQresultStatus(result) == PGRES_COMMAND_OK)
		has_error = false;
	else {
		has_error = true;
#ifndef NDEBUG
		fprintf(stderr, "PostgreSQL: %s\n", PQerrorMessage(sqdb->conn));
#endif
	}

	PQclear(result);
	return has_error;
}

static int  sqdb_postgre_migrate(SqdbPostgre *sqdb, SqSchema *schema, SqSchema *schema_next)
{
	SqBuffer    sql_buf;
	SqTable    *table, *old_table;
	SqPtrArray *reentries;
	PGresult   *results;
	bool        has_error = false;

	if (sqdb->conn == NULL)
		return SQCODE_ERROR;

	// If 'schema_next' is NULL, update and sort 'schema' and
	// synchronize 'schema' to database (mainly for SQLite).
	if (schema_next == NULL) {
#if SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB
		sq_schema_erase_fake_type(schema);
#endif
		// sort tables and columns by their name
		sq_schema_sort_table_column(schema);
		return SQCODE_OK;
	}

	// buffer for SQL statement
	sq_buffer_init(&sql_buf);

	if (sqdb->version < schema_next->version) {
		// do migrations by 'schema_next'
#ifndef NDEBUG
		fprintf(stderr, "PostgreSQL: start of migration ------\n");
#endif
		reentries = sq_type_entry_array(schema_next->type);
		for (unsigned int index = 0;  index < reentries->length;  index++) {
			table = (SqTable*)reentries->data[index];

			// clear sql_buf
			sql_buf.writed = 0;

			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
				old_table = sq_schema_find(schema, table->name);
				sqdb_postgre_alter_table(sqdb, &sql_buf, table, old_table);
			}
			else if (table->name == NULL) {
				// DROP TRIGGER in current table before DROP TABLE
				old_table = sq_schema_find(schema, table->old_name);
				sqdb_postgre_drop_table_dep(sqdb, &sql_buf, old_table);
				// DROP TABLE
				sqdb_sql_drop_table((Sqdb*)sqdb, &sql_buf, table, false);
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
				sqdb_sql_rename_table((Sqdb*)sqdb, &sql_buf, table->old_name, table->name);
			}
			else {
				// CREATE TABLE
				if (sqdb_sql_create_table((Sqdb*)sqdb, &sql_buf, table, NULL, false) > 0) {
#ifndef NDEBUG
					fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
					results = PQexec(sqdb->conn, sql_buf.mem);
					has_error = sqdb_postgre_has_error(sqdb, results);
					if (has_error)
						break;
				}
				sql_buf.writed = 0;    // write to start of buffer
				sqdb_postgre_create_table_dep(sqdb, &sql_buf, table);
			}

			if (sql_buf.writed > 0) {
				// 'sql_buf' must become null-terminated string before executing
				sq_buffer_write_c(&sql_buf, 0);  
#ifndef NDEBUG
				fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
				results = PQexec(sqdb->conn, sql_buf.mem);
				has_error = sqdb_postgre_has_error(sqdb, results);
				if (has_error)
					break;
			}
		}
#ifndef NDEBUG
		fprintf(stderr, "PostgreSQL: end of migration ------\n");
#endif
		// update database version
		sqdb->version = schema_next->version;
		sqdb_postgre_schema_set_version(sqdb, sqdb->version);
	}

	// include and apply changes from 'schema_next'
	sq_schema_update(schema, schema_next);
	schema->version = schema_next->version;

	sq_buffer_final(&sql_buf);

	if (has_error)
		return SQCODE_EXEC_ERROR;
	return SQCODE_OK;
}

// ----------------------------------------------------------------------------
// other static functions

#define SQDB_MIGRATIONS_TABLE    "sqdb_migrations"

static void sqdb_postgre_create_table_dep(SqdbPostgre *sqdb, SqBuffer *sql_buf, SqTable *table)
{
	SqColumn  *column;
	SqEntry  **cur, **end;

	end = table->type->entry + table->type->n_entry;
	for (cur = table->type->entry;  cur < end;  cur++) {
		column = *(SqColumn**)cur;
		// CREATE INDEX
		if (column->type == SQ_TYPE_INDEX) {
			sqdb_sql_create_index((Sqdb*)sqdb, sql_buf, table, column);
			sq_buffer_write_c(sql_buf, ';');
		}
		// CREATE TRIGGER
		else if (column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE)
			sqdb_postgre_create_trigger(sqdb, sql_buf, table->name, column->name);
	}
}

static void sqdb_postgre_create_trigger(SqdbPostgre *sqdb, SqBuffer *sql_buf, const char *table_name, const char *column_name)
{
	// CREATE function
	sq_buffer_write(sql_buf, "create or replace function ");
	sq_buffer_write(sql_buf, "sqxc_upd_");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write(sql_buf, "() returns trigger as "
	                         "$$ "
	                         "begin "
	                         "new.");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write(sql_buf, " = current_timestamp; "
	                         "return new; "
	                         "end "
	                         "$$ "
	                         "language plpgsql; ");

	// CREATE trigger
	sq_buffer_write(sql_buf, "create trigger ");
//	sq_buffer_write(sql_buf, "create or replace trigger ");    // PostgresSQL 14
	sq_buffer_write(sql_buf, "sqxc_trig_");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write(sql_buf, " before update on ");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, " for each row execute procedure ");
	sq_buffer_write(sql_buf, "sqxc_upd_");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write(sql_buf, "();");
}

static void sqdb_postgre_drop_table_dep(SqdbPostgre *sqdb, SqBuffer *sql_buf, SqTable *table)
{
	SqColumn  *column;
	SqEntry  **cur, **end;

	end = table->type->entry + table->type->n_entry;
	for (cur = table->type->entry;  cur < end;  cur++) {
		column = *(SqColumn**)cur;
		// DROP TRIGGER
		if (column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE)
			sqdb_postgre_drop_trigger(sqdb, sql_buf, table->name, column->name);
	}
}

static void sqdb_postgre_drop_trigger(SqdbPostgre *sqdb, SqBuffer *sql_buf, const char *table_name, const char *column_name)
{
	// DROP TRIGGER [IF EXISTS] trigger_name ON table_name
	sq_buffer_write(sql_buf, "DROP TRIGGER ");
//	sq_buffer_write(sql_buf, "IF EXISTS ");
	sq_buffer_write(sql_buf, "sqxc_trig_");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write(sql_buf, " ON ");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write_c(sql_buf, ';');

	// DROP FUNCTION [ IF EXISTS ] name
	sq_buffer_write(sql_buf, "DROP FUNCTION ");
//	sq_buffer_write(sql_buf, "IF EXISTS ");
	sq_buffer_write(sql_buf, "sqxc_upd_");
	sq_buffer_write(sql_buf, table_name);
	sq_buffer_write(sql_buf, "__");
	sq_buffer_write(sql_buf, column_name);
	sq_buffer_write_c(sql_buf, ';');
}

static void sqdb_postgre_alter_table(SqdbPostgre *db, SqBuffer *buffer, SqTable *table, SqTable *old_table)
{
	const SqType *table_type = table->type;
	SqColumn *old_column, *column;
	unsigned int  index;
	int       temp;
	PGresult *results;

	// ALTER TABLE
	for (index = 0;  index < table_type->n_entry;  index++) {
		column = (SqColumn*)table_type->entry[index];

		if (column->bit_field & SQB_COLUMN_CHANGED) {
			// ALTER COLUMN
			// check
			if (old_table == NULL) {
#ifndef NDEBUG
				fprintf(stderr, "%s: table '%s' not found in current schema.\n",
				        "sqdb_postgre_alter_table()", table->name);
#endif
			}

			old_column = sq_table_find_column(old_table, column->name);
			if (old_column == NULL) {
#ifndef NDEBUG
				fprintf(stderr, "%s: altering column '%s' not found.\n",
				        "sqdb_postgre_alter_table()", column->name);
#endif
				continue;
			}

			temp = 0;
			// write ALTER TABLE "table name"
			sq_buffer_write(buffer, "ALTER TABLE");
			sqdb_sql_write_identifier((Sqdb*)db, buffer, table->name, false);

			if (column->type   != old_column->type ||
			    column->size   != old_column->size ||
			    column->digits != old_column->digits)
			{
				// write ALTER COLUMN "column name"
				sq_buffer_write(buffer, "ALTER COLUMN");
				sqdb_sql_write_identifier((Sqdb*)db, buffer, column->name, false);
				// TYPE new_data_type
				sq_buffer_write(buffer, "TYPE ");
				sqdb_sql_write_column_type((Sqdb*)db, buffer, column);
				temp++;
			}

			if ((column->bit_field & SQB_COLUMN_NULLABLE) != (old_column->bit_field & SQB_COLUMN_NULLABLE)) {
				if (temp)
					sq_buffer_write_c(buffer, ',');
				// write ALTER COLUMN "column name"
				sq_buffer_write(buffer, "ALTER COLUMN");
				sqdb_sql_write_identifier((Sqdb*)db, buffer, column->name, false);
				// DROP NOT NULL or SET NOT NULL
				if (column->bit_field & SQB_COLUMN_NULLABLE)
					sq_buffer_write(buffer, "DROP NOT NULL");
				else
					sq_buffer_write(buffer, "SET NOT NULL");
				temp++;
			}

			if (column->default_value != old_column->default_value) {
				if (temp)
					sq_buffer_write_c(buffer, ',');
				// write ALTER COLUMN "column name"
				sq_buffer_write(buffer, "ALTER COLUMN");
				sqdb_sql_write_identifier((Sqdb*)db, buffer, column->name, false);
				if (column->default_value) {
					sq_buffer_write(buffer, "SET DEFAULT ");
					sq_buffer_write(buffer, column->default_value);
				}
				else if (column->bit_field & SQB_COLUMN_CURRENT)
					sq_buffer_write(buffer, "SET DEFAULT CURRENT_TIMESTAMP");
				else
					sq_buffer_write(buffer, "DROP DEFAULT");
				temp++;
			}

			// raw SQL
			if (((column->raw && old_column->raw) && strcasecmp(column->raw, old_column->raw) != 0) ||
			    ((column->raw != old_column->raw) && (column->raw == NULL || old_column->raw == NULL)) )
			{
				if (temp)
					sq_buffer_write_c(buffer, ';');
				// write ALTER TABLE "table name"
				sq_buffer_write(buffer, "ALTER TABLE");
				sqdb_sql_write_identifier((Sqdb*)db, buffer, table->name, false);
				// write ALTER COLUMN "column name"
				sq_buffer_write(buffer, "ALTER COLUMN");
				sqdb_sql_write_identifier((Sqdb*)db, buffer, column->name, false);
				if (column->raw == NULL) {
					sq_buffer_write(buffer, "DROP ");
					sq_buffer_write(buffer, old_column->raw);
				}
				else {
					sq_buffer_write(buffer, "SET ");
					sq_buffer_write(buffer, column->raw);
				}
				sq_buffer_write_c(buffer, ';');
				temp = 0;
			}

			buffer->mem[buffer->writed] = 0;    // NULL-termainated is not counted in length
		}
		else if (column->name == NULL) {
			// DROP COLUMN / CONSTRAINT / INDEX / KEY
			old_column = sq_table_find_column(old_table, column->old_name);
			if (old_column == NULL) {
#ifndef NDEBUG
				fprintf(stderr, "%s: dropping column '%s' not found.\n",
				        "sqdb_postgre_alter_table()", column->old_name);
#endif
			}
			else if (old_column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE)
				sqdb_postgre_drop_trigger(db, buffer, table->name, column->old_name);

			sqdb_sql_drop_column((Sqdb*)db, buffer, table, column);
		}
		else if (column->old_name && (column->bit_field & SQB_COLUMN_RENAMED) == 0) {
			// RENAME COLUMN
			sqdb_sql_rename_column((Sqdb*)db, buffer, table, column, NULL);
		}
		else {
			// ADD COLUMN / CONSTRAINT / INDEX / KEY
			sqdb_sql_add_column((Sqdb*)db, buffer, table, column);
			// ON UPDATE CURRENT_TIMESTAMP
			if (column->bit_field & SQB_COLUMN_CURRENT_ON_UPDATE) {
				sq_buffer_write_c(buffer, ';');
				sqdb_postgre_create_trigger(db, buffer, table->name, column->name);
			}
		}

		buffer->writed = 0;
#ifndef NDEBUG
		fprintf(stderr, "SQL: %s\n", buffer->mem);
#endif
		results = PQexec(db->conn, buffer->mem);
/*
		if (PQresultStatus(results) == PGRES_COMMAND_OK)
			temp = SQCODE_OK;
		else
			temp = SQCODE_ERROR;
 */
		PQclear(results);
/*
		if (temp == SQCODE_ERROR)
			break;
 */
	}

//	return temp;
}

static int  sqdb_postgre_schema_get_version(SqdbPostgre *sqdb)
{
	PGresult    *results;
	int  version = 0;

	results = PQexec(sqdb->conn, "CREATE TABLE IF NOT EXISTS " SQDB_MIGRATIONS_TABLE " ("
	                 "id INT NOT NULL, version INT NOT NULL DEFAULT 0, PRIMARY KEY (id)"
	                 ")");
	if (PQresultStatus(results) == PGRES_COMMAND_OK) {
		PQclear(results);
		// try to get or set version
		results = PQexec(sqdb->conn, "SELECT version FROM " SQDB_MIGRATIONS_TABLE " WHERE id = 0");
		if (PQresultStatus(results) == PGRES_TUPLES_OK && PQntuples(results) > 0)
			version = strtol(PQgetvalue(results, 0, 0), NULL, 10);
		else {
			PQclear(results);
			results = PQexec(sqdb->conn, "INSERT INTO " SQDB_MIGRATIONS_TABLE " (id) VALUES (0)");
		}
	}
	PQclear(results);
	return version;
}

static void sqdb_postgre_schema_set_version(SqdbPostgre *sqdb, int version)
{
	PGresult    *results;
	char *buf;
	int   len;

	len = snprintf(NULL, 0, "UPDATE " SQDB_MIGRATIONS_TABLE " SET version=%d WHERE id = 0", version) + 1;
	buf = malloc(len);
	snprintf(buf, len, "UPDATE " SQDB_MIGRATIONS_TABLE " SET version=%d WHERE id = 0", version);
	results = PQexec(sqdb->conn, buf);
	PQclear(results);
	free(buf);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline function.

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqEntry.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

Sqdb *sqdb_postgre_new(const SqdbConfigPostgre *config) {
	return sqdb_new(SQDB_INFO_POSTGRE, (SqdbConfig*)config);
}

#endif  // __STDC_VERSION__
