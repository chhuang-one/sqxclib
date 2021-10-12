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
#include <SqApp.h>
#include <SqAppConfig.h>
#include <migrations.h>

// ----------------------------------------------------------------------------
// SQL products

/* ------ SQLite ------ */
#if defined(SQLITE) && defined(SQ_CONFIG_HAVE_SQLITE)

#include <SqdbSqlite.h>
#define DB_CONNECTION    SQDB_INFO_SQLITE
#define DB_CONFIG        ((SqdbConfig*) &db_config_sqlite)

static const SqdbConfigSqlite  db_config_sqlite = {
	.folder    = DB_FOLDER,
	.extension = DB_EXTENSION,
};

#elif defined(SQLITE)
#define DB_PRODUCT_ERROR    "sqxclib does not enable SQLite support when compiling."
#endif  // SQLITE
#undef SQLITE

/* ------ MySQL ------ */
#if defined(MYSQL) && defined(SQ_CONFIG_HAVE_MYSQL)

#include <SqdbMysql.h>
#define DB_CONNECTION    SQDB_INFO_MYSQL
#define DB_CONFIG        ((SqdbConfig*) &db_config_mysql)

static const SqdbConfigMysql  db_config_mysql = {
	.host     = DB_HOST,
	.port     = DB_PORT,
	.user     = DB_USERNAME,
	.password = DB_PASSWORD,
};

#elif defined(MYSQL)
#define DB_PRODUCT_ERROR    "sqxclib does not enable MySQL support when compiling."
#endif  // MYSQL
#undef MYSQL

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
	app->db_config = DB_CONFIG;
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
