/*
 *   Copyright (C) 2020 by C.H. Huang
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

#ifndef SQDB_SQLITE_H
#define SQDB_SQLITE_H

#include <sqlite3.h>

#include <Sqdb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqdbConfigSqlite    SqdbConfigSqlite;

extern const SqdbInfo*    SQDB_INFO_SQLITE;

/* ----------------------------------------------------------------------------
    SqdbConfigSqlite

    SqdbConfig
    |
    `--- SqdbConfigSqlite
 */
struct SqdbConfigSqlite {
	SQDB_CONFIG_MEMBERS;
/*	// ------ SqdbConfig members ------
	SqdbInfo*      info;
	unsigned int   bit_field;    // reserve
 */

	// ------ SqdbConfigSqlite members ------
	char*          folder;
	char*          extension;    // optional
};



#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQDB_SQLITE_H
