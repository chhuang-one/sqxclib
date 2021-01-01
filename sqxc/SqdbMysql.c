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


static void sqdb_mysql_init(SqdbMysql* sqdb, SqdbConfigMysql* config);
static void sqdb_mysql_final(SqdbMysql* sqdb);
static int  sqdb_mysql_open(SqdbMysql* sqdb, const char* database_name);
static int  sqdb_mysql_close(SqdbMysql* sqdb);
static int  sqdb_mysql_exec(SqdbMysql* sqdb, const char* sql, Sqxc* xc, void* reserve);
static int  sqdb_mysql_migrate(SqdbMysql* sqdb, SqSchema* schema, SqSchema* schema_next);

static const SqdbInfo dbinfo = {
	.size    = sizeof(SqdbMysql),
	.product = SQDB_PRODUCT_MYSQL,
	.column  = {
		.has_boolean = 1,
	 	.use_alter  = 1,
		.use_modify = 0,
	},

	.init    = (void*)sqdb_mysql_init,
	.final   = (void*)sqdb_mysql_final,
    .open    = (void*)sqdb_mysql_open,
    .close   = (void*)sqdb_mysql_close,
    .exec    = (void*)sqdb_mysql_exec,
    .migrate = (void*)sqdb_mysql_migrate,
};

const SqdbInfo* SQDB_INFO_MYSQL = &dbinfo;

// ----------------------------------------------------------------------------
// SqdbInfo

static void sqdb_mysql_init(SqdbMysql* sqdb, SqdbConfigMysql* config_src)
{
}

static void sqdb_mysql_final(SqdbMysql* sqdb)
{
}

static int  sqdb_mysql_open(SqdbMysql* sqdb, const char* database_name)
{
	return SQCODE_OK;
}

static int  sqdb_mysql_close(SqdbMysql* sqdb)
{
	return SQCODE_OK;
}

static int  sqdb_mysql_migrate(SqdbMysql* sqdb, SqSchema* schema, SqSchema* schema_next)
{
	return SQCODE_OK;
}

static int  sqdb_mysql_exec(SqdbMysql* sqdb, const char* sql, Sqxc* xc, void* reserve)
{
	return SQCODE_OK;
}
