/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <SqConfig.h>
#include <SqError.h>
#include <migrations.h>
#include <SqApp.h>
#include <SqApp-config.h>    // include SqApp-config.h at last

// ----------------------------------------------------------------------------
// SQL products

/* ------ SQLite ------ */
#if defined(DB_SQLITE) && defined(SQ_CONFIG_HAVE_SQLITE)

#include <SqdbSqlite.h>
#define DB_CONNECTION    SQDB_INFO_SQLITE
#define DB_CONFIG        ((SqdbConfig*) &db_config_sqlite)

static const SqdbConfigSqlite  db_config_sqlite = {
	.folder    = DB_FOLDER,
	.extension = DB_EXTENSION,
};

#elif defined(DB_SQLITE)
#define DB_PRODUCT_ERROR    "sqxclib does not enable SQLite support when compiling."
#endif  // DB_SQLITE

/* ------ MySQL ------ */
#if defined(DB_MYSQL) && defined(SQ_CONFIG_HAVE_MYSQL)

#include <SqdbMysql.h>
#define DB_CONNECTION    SQDB_INFO_MYSQL
#define DB_CONFIG        ((SqdbConfig*) &db_config_mysql)

static const SqdbConfigMysql  db_config_mysql = {
	.host     = DB_HOST,
	.port     = DB_PORT,
	.user     = DB_USERNAME,
	.password = DB_PASSWORD,
};

#elif defined(DB_MYSQL)
#define DB_PRODUCT_ERROR    "sqxclib does not enable MySQL support when compiling."
#endif  // DB_MYSQL

/* ------ error ------ */
#ifdef DB_PRODUCT_ERROR
#include <stdio.h>    // puts
#define DB_CONNECTION    NULL
#define DB_CONFIG        NULL
#endif

// ----------------------------------------------------------------------------
// SqApp functions

void  sq_app_init(SqApp *app)
{
#ifdef DB_PRODUCT_ERROR
	puts(DB_PRODUCT_ERROR);
	exit(EXIT_FAILURE);
#endif

	app->db = sqdb_new(DB_CONNECTION, DB_CONFIG);
	app->db_config    = DB_CONFIG;
	app->db_database  = DB_DATABASE;
	app->migrations   = migrations;
	app->n_migrations = n_migrations;

	app->storage = sq_storage_new(app->db);
}

void  sq_app_final(SqApp *app)
{
	sq_storage_free(app->storage);
	sqdb_free(app->db);
}

int   sq_app_open_database(SqApp *app, const char *db_database)
{
	if (db_database == NULL)
		db_database = DB_DATABASE;
	return sq_storage_open(app->storage, db_database);
}

void  sq_app_close_database(SqApp *app)
{
	sq_storage_close(app->storage);
}

int   sq_app_make_schema(SqApp *app, int cur)
{
	const SqMigration *migration;
	SqSchema *schema;
	int  code = SQCODE_OK;

	if (cur == 0)
		cur = app->db->version;
	// if the database vesion is 0 (no migrations have been done), return SQCODE_DB_VERSION_0
	if (cur == 0)
		return SQCODE_DB_VERSION_0;
	// if these migrations are not for this database, return SQCODE_DB_VERSION_MISMATCH
	if (cur >= app->n_migrations)
		return SQCODE_DB_VERSION_MISMATCH;

	schema = malloc(sizeof(SqSchema));
	for (int i = 1;  i <= cur;  i++) {
		migration = app->migrations[i];
		if (migration == NULL)
			continue;
		sq_schema_init(schema, NULL);
		schema->version = i;    // specify version number
		migration->up(schema, app->storage);
		code = sq_storage_migrate(app->storage, schema);
		sq_schema_final(schema);
		if (code != SQCODE_OK)
			break;
	}
	free(schema);

	// synchronize schema to database and update schema/table status
	// This mainly used by SQLite
	sq_storage_migrate(app->storage, NULL);

	return code;
}

int   sq_app_migrate(SqApp *app, int step)
{
	const SqMigration *migration = NULL;
	SqSchema    *schema;
	bool schema_changed = false;
	int  end, cur;
	int  code;
	int  batch;

	if (sq_migration_get_last(app->storage, &batch) == -1)
		sq_migration_install(app->db);

	code = SQCODE_OK;
	end = app->n_migrations;
	cur = app->db->version;
	if (cur >= end)
		return SQCODE_DB_VERSION_MISMATCH;
	if (step > 0) {
		end = cur + step + 1;
		if (end > app->n_migrations)
			end = app->n_migrations;
	}

	schema = malloc(sizeof(SqSchema));
	for (cur = cur+1;  cur < end;  cur++) {
		migration = app->migrations[cur];
		schema_changed = true;
		if (migration == NULL)
			continue;
		sq_schema_init(schema, NULL);
		schema->version = cur;    // specify version number
		migration->up(schema, app->storage);
		code = sq_storage_migrate(app->storage, schema);
		sq_schema_final(schema);
		if (code != SQCODE_OK)
			break;
/*
		code = sq_storage_migrate(app->storage, NULL);
		if (code != SQCODE_OK)
			break;
 */
		sq_migration_insert(app->storage, (SqMigration**)app->migrations,
		                    cur, 1, batch +1);
	}
	free(schema);

	if (schema_changed)
		code = sq_storage_migrate(app->storage, NULL);
	return code;
}

int   sq_app_rollback(SqApp *app, int step)
{
	const SqMigration *migration;
	SqSchema    *schema;
	bool schema_changed = false;
	int  cur;
	int  code;
	int  batch = 0;

	code = SQCODE_OK;
	cur = app->db->version;
	if (cur >= app->n_migrations)
		return SQCODE_DB_VERSION_MISMATCH;
	if (step == 0) {
		if (sq_migration_get_last(app->storage, &batch) == 0)
			return SQCODE_DB_NO_MIGRATIONS;
		step = sq_migration_count_batch(app->storage, batch);
	}

	schema = malloc(sizeof(SqSchema));
	for (;  cur > 0 && step > 0;  step--, cur--) {
		migration = app->migrations[cur];
		schema_changed = true;
		if (migration == NULL)
			continue;
		sq_schema_init(schema, NULL);
		schema->version  = cur - 1;    // specify version number
		migration->down(schema, app->storage);
		app->db->version = cur - 2;
		code = sq_storage_migrate(app->storage, schema);
		sq_schema_final(schema);
		if (code != SQCODE_OK)
			break;
/*
		code = sq_storage_migrate(app->storage, NULL);
		if (code != SQCODE_OK)
			break;
 */
		sq_migration_remove(app->storage, cur);
	}
	free(schema);

	if (schema_changed)
		code = sq_storage_migrate(app->storage, NULL);
	return SQCODE_OK;
}
