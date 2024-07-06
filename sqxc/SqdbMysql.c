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
#include <stdio.h>        // snprintf(), fprintf(), stderr
#include <stdbool.h>      // bool, true, false

#include <SqError.h>
#include <Sqdb-migration.h>
#include <SqdbMysql.h>
#include <SqxcValue.h>
#include <SqxcSql.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

#define MYSQL_DEFAULT_HOST      "localhost"
#define MYSQL_DEFAULT_PORT      3306
#define MYSQL_DEFAULT_USER      "root"
#define MYSQL_DEFAULT_PASSWORD  ""

static void sqdb_mysql_init(SqdbMysql *sqdb, const SqdbConfigMysql *config);
static void sqdb_mysql_final(SqdbMysql *sqdb);
static int  sqdb_mysql_open(SqdbMysql *sqdb, const char *database_name);
static int  sqdb_mysql_close(SqdbMysql *sqdb);
static int  sqdb_mysql_exec(SqdbMysql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_mysql_migrate(SqdbMysql *sqdb, SqSchema *schema, SqSchema *schema_next);

static int  sqdb_mysql_schema_get_version(SqdbMysql *sqdb);
static void sqdb_mysql_schema_set_version(SqdbMysql *sqdb, int version);

static const SqdbConfigMysql db_default = {
	.host     = MYSQL_DEFAULT_HOST,
	.port     = MYSQL_DEFAULT_PORT,
	.user     = MYSQL_DEFAULT_USER,
	.password = MYSQL_DEFAULT_PASSWORD,
};

const SqdbInfo sqdbInfo_MySQL = {
	.size    = sizeof(SqdbMysql),
	.product = SQDB_PRODUCT_MYSQL,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 0,
		.use_modify = 1,
	},
	.quote = {
		.identifier = {'`', '`'}
	},

	.init    = (void*)sqdb_mysql_init,
	.final   = (void*)sqdb_mysql_final,
	.open    = (void*)sqdb_mysql_open,
	.close   = (void*)sqdb_mysql_close,
	.exec    = (void*)sqdb_mysql_exec,
	.migrate = (void*)sqdb_mysql_migrate,
};

// ----------------------------------------------------------------------------
// SqdbInfo functions

static void sqdb_mysql_init(SqdbMysql *sqdb, const SqdbConfigMysql *config_src)
{
	// Memory allocated by mysql_init() must be freed with mysql_close().
	sqdb->self = mysql_init(NULL);
	sqdb->config = config_src;
	sqdb->version = 0;
}

static void sqdb_mysql_final(SqdbMysql *sqdb)
{
	if (sqdb->self)
		mysql_close(sqdb->self);
}

static int  sqdb_mysql_open(SqdbMysql *sqdb, const char *database_name)
{
	MYSQL *conres;
	const SqdbConfigMysql *config = (sqdb->config) ? sqdb->config : &db_default;

	if (database_name == NULL) {
		database_name = config->db;
		if (database_name == NULL)
			return SQCODE_OPEN_FAILED;
	}
	if (sqdb->self == NULL)
		sqdb->self = mysql_init(NULL);

	conres = mysql_real_connect(sqdb->self, config->host,
	                            config->user, config->password,
	                            database_name, config->port,
	                            NULL, 0);
	if (conres == NULL) {
		mysql_close(sqdb->self);
		sqdb->self = NULL;
		return SQCODE_OPEN_FAILED;
	}
	sqdb->version = sqdb_mysql_schema_get_version(sqdb);
	return SQCODE_OK;
}

static int  sqdb_mysql_close(SqdbMysql *sqdb)
{
	mysql_close(sqdb->self);
	sqdb->self = NULL;
	return SQCODE_OK;
}

static int  sqdb_mysql_migrate(SqdbMysql *db, SqSchema *schema, SqSchema *schema_next)
{
	SqBuffer    sql_buf;
	SqTable    *table, *table_data;
	SqPtrArray *reentries;
	bool        has_error = false;

	if (db->self == NULL)
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

	if (db->version < schema_next->version) {
		// do migrations by 'schema_next'
#ifndef NDEBUG
		fprintf(stderr, "MySQL: start of migration ------\n");
#endif
		reentries = sq_type_entry_array(schema_next->type);
		for (unsigned int index = 0;  index < reentries->length;  index++) {
			table = (SqTable*)reentries->data[index];

			// clear sql_buf
			sql_buf.writed = 0;

			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
				table_data = sq_schema_find(schema, table->name);
				sqdb_exec_alter_table((Sqdb*)db, &sql_buf, table, NULL, table_data);
			}
			else if (table->name == NULL) {
				// DROP TABLE
				sqdb_sql_drop_table((Sqdb*)db, &sql_buf, table, false);
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
				sqdb_sql_rename_table((Sqdb*)db, &sql_buf, table->old_name, table->name);
			}
			else {
				// CREATE TABLE
				if (sqdb_sql_create_table((Sqdb*)db, &sql_buf, table, NULL, false) > 0) {
#ifndef NDEBUG
					fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
					if (mysql_query(db->self, sql_buf.mem)) {
						has_error = true;
#ifndef NDEBUG
						fprintf(stderr, "MySQL: %s\n", mysql_error(db->self));
#endif
						break;
					}
				}
				sql_buf.writed = 0;
				sqdb_exec_create_index((Sqdb*)db, &sql_buf, table, NULL);
			}

			if (sql_buf.writed > 0) {
#ifndef NDEBUG
				fprintf(stderr, "SQL: %s\n", sql_buf.mem);
#endif
				if (mysql_query(db->self, sql_buf.mem)) {
					has_error = true;
#ifndef NDEBUG
					fprintf(stderr, "MySQL: %s\n", mysql_error(db->self));
#endif
					break;
				}
			}
		}
#ifndef NDEBUG
		fprintf(stderr, "MySQL: end of migration ------\n");
#endif
		// update database version
		db->version = schema_next->version;
		sqdb_mysql_schema_set_version(db, db->version);
	}

	// include and apply changes from 'schema_next'
	sq_schema_update(schema, schema_next);
	schema->version = schema_next->version;

	sq_buffer_final(&sql_buf);

	if (has_error)
		return SQCODE_EXEC_ERROR;
	return SQCODE_OK;
}

static int  sqdb_mysql_exec(SqdbMysql *sqdb, const char *sql, Sqxc *xc, void *reserve)
{
	MYSQL_RES   *result;
	MYSQL_ROW    row;
	MYSQL_FIELD *field;
	unsigned int n_fields;
	char **names;
	int    rc = 0;
	int    code = SQCODE_OK;

#ifndef NDEBUG
	fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL)
		rc = mysql_query(sqdb->self, sql);
	else {
		// Determines command based on the first character in SQL statement.
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_VALUE) {
				fprintf(stderr, "%s: SELECT command must use with SqxcValue.\n",
				        "sqdb_mysql_exec()");
				return SQCODE_EXEC_ERROR;
			}
#endif
			rc = mysql_query(sqdb->self, sql);
			if (rc)
				break;
			result = mysql_use_result(sqdb->self);
			n_fields = mysql_num_fields(result);

			names = calloc(1, sizeof(char*) * n_fields);
			for (unsigned int i = 0;  (field = mysql_fetch_field(result));  i++)
				names[i] = field->name;

			// if Sqxc element prepare for multiple row
			if (sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}

			// get result set
			xc->code = SQCODE_NO_DATA;
			while ((row = mysql_fetch_row(result))) {
				// built-in types are not object
				if (SQ_TYPE_NOT_BUILTIN(sqxc_value_element(xc))) {
					xc->type = SQXC_TYPE_OBJECT;
					xc->name = NULL;
					xc->value.pointer = NULL;
					xc = sqxc_send(xc);
//					if (xc->code != SQCODE_OK)
//						break;
				}

				xc->type = SQXC_TYPE_STR;
				for (unsigned int i = 0;  i < n_fields;  i++) {
					xc->name = names[i];
					xc->value.str = row[i];
					xc = sqxc_send(xc);
#ifndef NDEBUG
					switch (xc->code) {
					case SQCODE_OK:
						break;

					case SQCODE_ENTRY_NOT_FOUND:
						// warning
						fprintf(stderr, "%s: column '%s' not found.\n",
						        "sqdb_mysql_exec()", names[i]);
						break;

					default:
						fprintf(stderr, "%s: error occurred during parsing column '%s'.\n",
						        "sqdb_mysql_exec()", names[i]);
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
//					if (xc->code != SQCODE_OK)
//						break;
				}
			}
			// if the result set is empty.
			if (xc->code == SQCODE_NO_DATA)
				code = SQCODE_NO_DATA;

			// if Sqxc element prepare for multiple row
			if (sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY_END;
				xc->name = NULL;
//				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}

			free(names);
			mysql_free_result(result);
			break;

		case 'I':    // INSERT
		case 'i':    // insert
		case 'U':    // UPDATE
		case 'u':    // update
#ifndef NDEBUG
			if (xc->info != SQXC_INFO_SQL) {
				fprintf(stderr, "%s: INSERT and UPDATE command must use with SqxcSql.\n",
				        "sqdb_mysql_exec()");
				return SQCODE_EXEC_ERROR;
			}
#endif
			// Don't break here
//			break;
		default:
			rc = mysql_query(sqdb->self, sql);
			// set the last inserted row id
			((SqxcSql*)xc)->id = mysql_insert_id(sqdb->self);
			// set number of rows changed
			((SqxcSql*)xc)->changes = mysql_affected_rows(sqdb->self);
			break;
		}
	}

	if (rc) {
#ifndef NDEBUG
		fprintf(stderr, "MySQL: %s\n", mysql_error(sqdb->self));
#endif
		return SQCODE_EXEC_ERROR;
	}
	return code;
}

// ----------------------------------------------------------------------------
// other static functions

#define SQDB_MIGRATIONS_TABLE    "sqdb_migrations"

static int  sqdb_mysql_schema_get_version(SqdbMysql *sqdb)
{
	MYSQL_RES   *result;
	MYSQL_ROW    row;
	int  version = 0;
	int  rc;

	rc = mysql_query(sqdb->self, "CREATE TABLE IF NOT EXISTS " SQDB_MIGRATIONS_TABLE " ("
	                 "id INT NOT NULL, version INT NOT NULL DEFAULT 0, PRIMARY KEY (id)"
	                 ")");
	if (rc == 0) {
		// try to get or set version
		mysql_query(sqdb->self, "SELECT version FROM " SQDB_MIGRATIONS_TABLE " WHERE id = 0");
		result = mysql_store_result(sqdb->self);
		if ((row = mysql_fetch_row(result)))
			version = strtol(row[0], NULL, 10);
		else
			mysql_query(sqdb->self, "INSERT INTO " SQDB_MIGRATIONS_TABLE " (id) VALUES (0)");
		mysql_free_result(result);
	}

	return version;
}

static void sqdb_mysql_schema_set_version(SqdbMysql *sqdb, int version)
{
	char *buf;
	int   len;

	len = snprintf(NULL, 0, "UPDATE " SQDB_MIGRATIONS_TABLE " SET version=%d WHERE id = 0", version) + 1;
	buf = malloc(len);
	snprintf(buf, len, "UPDATE " SQDB_MIGRATIONS_TABLE " SET version=%d WHERE id = 0", version);
	mysql_query(sqdb->self, buf);
	free(buf);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline function.

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqEntry.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

Sqdb *sqdb_mysql_new(const SqdbConfigMysql *config) {
	return sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*)config);
}

#endif  // __STDC_VERSION__
