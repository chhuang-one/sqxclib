/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

#include "migrations.h"
#include <SqApp.h>
#include <SqApp-config.h>

// ----------------------------------------------------------------------------
// Common setting

// No migration mode
#if defined(DB_NO_MIGRATION) && DB_NO_MIGRATION
#undef  DB_NO_MIGRATION
#define DB_NO_MIGRATION    SQDB_CONFIG_NO_MIGRATION
#endif

#define DB_BIT_FIELD    DB_NO_MIGRATION

// ----------------------------------------------------------------------------
// Database products

/* ------ SQLite ------ */
#if   DB_SQLITE && SQ_CONFIG_HAVE_SQLITE

#include <SqdbSqlite.h>
#define DB_CONNECTION    SQDB_INFO_SQLITE
#define DB_CONFIG        ((SqdbConfig*) &db_config_sqlite)

static const SqdbConfigSqlite  db_config_sqlite = {
	.bit_field = DB_BIT_FIELD,
	.folder    = DB_FOLDER,
	.extension = DB_EXTENSION,
};

#elif DB_SQLITE
#define DB_PRODUCT_ERROR    "sqxclib does not enable SQLite support when compiling."
#endif  // DB_SQLITE

/* ------ MySQL ------ */
#if   DB_MYSQL && SQ_CONFIG_HAVE_MYSQL

#include <SqdbMysql.h>
#define DB_CONNECTION    SQDB_INFO_MYSQL
#define DB_CONFIG        ((SqdbConfig*) &db_config_mysql)

static const SqdbConfigMysql  db_config_mysql = {
	.bit_field = DB_BIT_FIELD,
	.host      = DB_HOST,
	.port      = DB_PORT,
	.user      = DB_USERNAME,
	.password  = DB_PASSWORD,
};

#elif DB_MYSQL
#define DB_PRODUCT_ERROR    "sqxclib does not enable MySQL support when compiling."
#endif  // DB_MYSQL

/* ------ PostgreSQL ------ */
#if   DB_POSTGRE && SQ_CONFIG_HAVE_POSTGRESQL

#include <SqdbPostgre.h>
#define DB_CONNECTION    SQDB_INFO_POSTGRE
#define DB_CONFIG        ((SqdbConfig*) &db_config_postgre)

static const SqdbConfigPostgre  db_config_postgre = {
	.bit_field = DB_BIT_FIELD,
	.host      = DB_HOST,
	.port      = DB_PORT,
	.user      = DB_USERNAME,
	.password  = DB_PASSWORD,
};

#elif DB_POSTGRE
#define DB_PRODUCT_ERROR    "sqxclib does not enable PostgreSQL support when compiling."
#endif  // DB_POSTGRE

/* ------ error ------ */
#ifdef DB_PRODUCT_ERROR
#define DB_CONNECTION    NULL
#define DB_CONFIG        NULL
#endif

// ----------------------------------------------------------------------------
// default setting

const struct SqAppSetting sqAppSetting_default = {
    .db_info      =  DB_CONNECTION,
    .db_config    =  DB_CONFIG,
    .db_database  =  DB_DATABASE,
    .migrations   =  sqApp_migrations_default,
    .n_migrations = &sqApp_n_migrations_default,

#ifdef DB_PRODUCT_ERROR
    .error        = DB_PRODUCT_ERROR,
#endif
};
