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
#include <SqdbMysql.h>
#include <SqxcValue.h>
#include <SqxcSql.h>
#include <Sqdb-migration.h>

#define MYSQL_DEFAULT_HOST      "localhost"
#define MYSQL_DEFAULT_PORT      3306
#define MYSQL_DEFAULT_USER      "root"
#define MYSQL_DEFAULT_PASSWORD  ""

static void sqdb_mysql_init(SqdbMysql *sqdb, SqdbConfigMysql *config);
static void sqdb_mysql_final(SqdbMysql *sqdb);
static int  sqdb_mysql_open(SqdbMysql *sqdb, const char *database_name);
static int  sqdb_mysql_close(SqdbMysql *sqdb);
static int  sqdb_mysql_exec(SqdbMysql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_mysql_migrate(SqdbMysql *sqdb, SqSchema *schema, SqSchema *schema_next);

static int  sqdb_mysql_schema_get_version(SqdbMysql *sqdb);
static void sqdb_mysql_schema_set_version(SqdbMysql *sqdb, int version);

static const SqdbConfigMysql db_default = {
	.host     = MYSQL_DEFAULT_HOST,
	.user     = MYSQL_DEFAULT_USER,
	.password = MYSQL_DEFAULT_PASSWORD,
	.port     = MYSQL_DEFAULT_PORT,
};

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbMysql),
	.product = SQDB_PRODUCT_MYSQL,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
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

const SqdbInfo *SQDB_INFO_MYSQL = &dbinfo;

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_mysql_init(SqdbMysql *sqdb, SqdbConfigMysql *config_src)
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
			return SQCODE_OPEN_FAIL;
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
		return SQCODE_OPEN_FAIL;
	}
	sqdb->version = sqdb_mysql_schema_get_version(sqdb);
	return SQCODE_OK;
}

static int  sqdb_mysql_close(SqdbMysql *sqdb)
{
	mysql_close(sqdb->self);
	return SQCODE_OK;
}

static int  sqdb_mysql_migrate(SqdbMysql *db, SqSchema *schema, SqSchema *schema_next)
{
	SqBuffer    sql_buf;
	SqTable    *table, *table_data;
	SqPtrArray *reentries;
	int    rc = 0;

	if (db->self == NULL)
		return SQCODE_ERROR;
	if (schema_next == NULL)
		return SQCODE_OK;

	sq_buffer_init(&sql_buf);    // buffer for SQL statement
	reentries = sq_type_get_ptr_array(schema_next->type);

	if (db->version < schema_next->version) {
#if DEBUG
		fprintf(stderr, "MySQL: start of migration ------\n");
#endif
		for (int index = 0;  index < reentries->length;  index++) {
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
#if DEBUG
					fprintf(stderr, "SQL: %s\n", sql_buf.buf);
#endif
					rc = mysql_query(db->self, sql_buf.buf);
					if (rc)
						goto atExit;
				}
				sql_buf.writed = 0;
				sqdb_exec_create_index((Sqdb*)db, &sql_buf, table, NULL);
			}

			if (sql_buf.writed > 0) {
#if DEBUG
				fprintf(stderr, "SQL: %s\n", sql_buf.buf);
#endif
				rc = mysql_query(db->self, sql_buf.buf);
				if (rc)
					goto atExit;
			}
		}
#if DEBUG
		fprintf(stderr, "MySQL: end of migration ------\n");
#endif
		// update database version
		db->version = schema_next->version;
		sqdb_mysql_schema_set_version(db, db->version);
	}
	sq_schema_update(schema, schema_next);
	schema->version = schema_next->version;

atExit:
	sq_buffer_final(&sql_buf);
	if (rc) {
		fprintf(stderr, "MySQL: %s\n", mysql_error(db->self));
		return SQCODE_EXEC_ERROR;
	}
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

#ifdef DEBUG
		fprintf(stderr, "SQL: %s\n", sql);
#endif

	if (xc == NULL)
		rc = mysql_query(sqdb->self, sql);
	else {
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
#ifdef DEBUG
			if (xc->info != SQXC_INFO_VALUE) {
				fprintf(stderr, "sqdb_mysql_exec(): SELECT command must use with SqxcValue.\n");
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
			if (sqxc_value_current(xc) == sqxc_value_container(xc)) {
				xc->type = SQXC_TYPE_ARRAY;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
			}

			while ((row = mysql_fetch_row(result))) {
				xc->type = SQXC_TYPE_OBJECT;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);

				xc->type = SQXC_TYPE_STRING;
				for (unsigned int i = 0;  i < n_fields;  i++) {
					xc->name = names[i];
					xc->value.string = row[i];
					xc = sqxc_send(xc);
#ifdef DEBUG
					switch (xc->code) {
					case SQCODE_OK:
						break;

					case SQCODE_ENTRY_NOT_FOUND:
						fprintf(stderr, "sqdb_mysql_exec(): column '%s' not found.\n", names[i]);
						break;

					default:
						fprintf(stderr, "sqdb_mysql_exec(): error occurred during parsing column '%s'.\n", names[i]);
						break;
					}
#endif  // DEBUG
				}

				xc->type = SQXC_TYPE_OBJECT_END;
				xc->name = NULL;
				xc->value.pointer = NULL;
				xc = sqxc_send(xc);
//				if (xc->code != SQCODE_OK)
//					break;
			}

			// if Sqxc element prepare for multiple row
			if (sqxc_value_current(xc) == sqxc_value_container(xc)) {
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
#ifdef DEBUG
			if (xc->info != SQXC_INFO_SQL) {
				fprintf(stderr, "sqdb_mysql_exec(): INSERT command must use with SqxcSql.\n");
				return SQCODE_EXEC_ERROR;
			}
#endif
		default:
			rc = mysql_query(sqdb->self, sql);
			break;
		}
	}

	if (rc) {
		fprintf(stderr, "MySQL: %s\n", mysql_error(sqdb->self));
		return SQCODE_EXEC_ERROR;
	}
	return SQCODE_OK;
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
		mysql_query(sqdb->self, "SELECT version from " SQDB_MIGRATIONS_TABLE " WHERE id = 0");
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
