/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H


#if 1    // defined(HAVE_CONFIG_H)
#include "config.h"
#else
#define HAVE_THREAD   1
#define HAVE_JSONC    1
#define HAVE_SQLITE   1
#define HAVE_MYSQL    1
#endif


/* SqStorage.c */
#if HAVE_THREAD == 1
#define SQ_CONFIG_HAVE_THREAD        1
#else
#define SQ_CONFIG_HAVE_THREAD        0
#endif
#undef HAVE_THREAD

/* SqxcJsonc.c, SqxcJsonc.h, SqStorage.c */
#if HAVE_JSONC == 1
#define SQ_CONFIG_HAVE_JSONC         1
#else
#define SQ_CONFIG_HAVE_JSONC         0
#endif
#undef HAVE_JSONC

/* SqdbSqlite.h */
#if HAVE_SQLITE == 1
#define SQ_CONFIG_HAVE_SQLITE        1
#else
#define SQ_CONFIG_HAVE_SQLITE        0
#endif
#undef HAVE_SQLITE

/* SqdbMysql.h */
#if HAVE_MYSQL == 1
#define SQ_CONFIG_HAVE_MYSQL         1
#else
#define SQ_CONFIG_HAVE_MYSQL         0
#endif
#undef HAVE_MYSQL

/* SqdbPostgre.h */
#if HAVE_POSTGRESQL == 1
#define SQ_CONFIG_HAVE_POSTGRESQL    1
#else
#define SQ_CONFIG_HAVE_POSTGRESQL    0
#endif
#undef HAVE_POSTGRESQL

/* Enable SqxcNested.data3 to do fast type match.
   Sqxc can run a bit faster when doing object and array type match.
   Disable it to reduce code size.
   Affected source : SqJoint, SqType, SqCommand
 */
#define SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH      1

/* Enable it will remove columns that have fake type when synchronize schema to database.
   Sqxc can run hardly faster when parsing input data. It is no effect if you use SQLite.
   Disable it to reduce code size.
   Affected source : Sqdb-migration, SqdbMysql
 */
#define SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB     0

/* Enable it will add sq_storage_update_field() and C++ updateField() to sqxclib.
   Disable it if you don't use these and want to reduce binary size.
 */
#define SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD         1

/* If you enable this, sq_time_to_string() will use gmtime() to convert calendar time */
#define SQ_CONFIG_CONVERT_TIME_TO_GMT              0

/* sqxclib is case-insensitive by default when parsing command from console.
   User can enable SQ_CONFIG_COMMAND_CASE_SENSITIVE to change it.
   Affected source : SqConsole
 */
#define SQ_CONFIG_COMMAND_CASE_SENSITIVE           0

/* sqxclib is case-sensitive when searching and sorting SQL column name and JSON field name by default.
   You may disable this for some old SQL product.
   Affected source : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE        1

// ----------------------------------------------------------------------------
// Default length (size)

/* If user doesn't specify SQL string length, program will use it by default.
   Affected source : Sqdb, SqSchema
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191

/* SqEntry.c, SqType.c, SqSchema.c - SQ_TYPE_N_ENTRY_DEFAULT */
#define SQ_CONFIG_TYPE_N_ENTRY_DEFAULT            16

/* SqBuffer.c - SQ_BUFFER_SIZE_DEFAULT */
#define SQ_CONFIG_BUFFER_SIZE_DEAULT             128

/* SqxcSql.c */
#define SQ_CONFIG_SQXC_SQL_BUFFER_SIZE_DEAULT    256

/* SqType-array.c - SQ_TYPE_ARRAY_SIZE_DEFAULT */
#define SQ_CONFIG_TYPE_ARRAY_SIZE_DEFAULT         16

/* SqTable-relation.c */
#define SQ_CONFIG_TABLE_RELATION_SIZE             16    //  8

/* SqSchema-relation.c */
#define SQ_CONFIG_SCHEMA_RELATION_POOL_SIZE      128    // 64
#define SQ_CONFIG_SCHEMA_RELATION_SIZE            16    //  8

#endif  // SQ_CONFIG_H
