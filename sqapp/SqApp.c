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

/* ------ common ------ */
#define DB_CONFIG    ((SqdbConfig*) &db_config)

/* ------ SQLite ------ */
#ifdef SQLITE
#undef SQLITE
	#if !defined(SQ_CONFIG_HAVE_SQLITE)
	#error sqxclib does not enable SQLite support when compiling.
	#endif

#include <SqdbSqlite.h>
#define DB_CONNECTION    SQDB_INFO_SQLITE

static const SqdbConfigSqlite  db_config = {
	.folder    = DB_FOLDER,
	.extension = DB_EXTENSION,
};
#endif  // SQLITE

/* ------ MySQL ------ */
#ifdef MYSQL
#undef MYSQL
	#if !defined(SQ_CONFIG_HAVE_MYSQL)
	#error sqxclib does not enable MySQL support when compiling.
	#endif

#include <SqdbMysql.h>
#define DB_CONNECTION    SQDB_INFO_MYSQL

static const SqdbConfigMysql  db_config = {
	.host     = DB_HOST,
	.port     = DB_PORT,
	.user     = DB_USERNAME,
	.password = DB_PASSWORD,
};
#endif  // MYSQL


void  sq_app_init(SqApp *app)
{
	app->db = sqdb_new(DB_CONNECTION, DB_CONFIG);
	app->db_config = DB_CONFIG;
	app->db_database  = DB_DATABASE;
	app->migrations   = migrations;
	app->n_migrations = n_migrations;

	app->storage = sq_storage_new(app->db);
}

void  sq_app_final(SqApp *app)
{

}
