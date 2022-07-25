/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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

#include <assert.h>
#include <stdio.h>    // puts()

#include <SqConfig.h>
#include <SqError.h>
#include <migrations.h>
#include <SqApp.h>

// ----------------------------------------------------------------------------
// SqApp functions

SqApp *sq_app_new(const struct SqAppSetting *setting)
{
	SqApp *app;

	app = malloc(sizeof(SqApp));
	sq_app_init(app, setting);
	return app;
}

void   sq_app_free(SqApp *app)
{
	sq_app_final(app);
	free(app);
}

void  sq_app_init(SqApp *app, const struct SqAppSetting *setting)
{
	assert(setting != NULL);

	if (setting->error) {
		puts(setting->error);
		exit(EXIT_FAILURE);
	}

	app->db = sqdb_new(setting->db_info, setting->db_config);
	app->db_config    = setting->db_config;
	app->db_database  = setting->db_database;
	app->migrations   = setting->migrations;
	app->n_migrations = setting->n_migrations[0];

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
		db_database = app->db_database;
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
	// if the database vesion is 0 (no migrations have been done), return SQCODE_DB_SCHEMA_VERSION_0
	if (cur == 0)
		return SQCODE_DB_SCHEMA_VERSION_0;
	// if these migrations are not for this database, return SQCODE_DB_WRONG_MIGRATIONS
	if (cur >= app->n_migrations)
		return SQCODE_DB_WRONG_MIGRATIONS;

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

	// synchronize schema to database and update schema in 'storage'
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
		return SQCODE_DB_WRONG_MIGRATIONS;
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
		return SQCODE_DB_WRONG_MIGRATIONS;
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
