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


#if HAVE_THREAD == 1
/* SqStorage.c */
#define SQ_CONFIG_HAVE_THREAD
#endif
#undef HAVE_THREAD

#if HAVE_JSONC == 1
/* SqxcJsonc.c, SqxcJsonc.h, SqStorage.c */
#define SQ_CONFIG_HAVE_JSONC
#endif
#undef HAVE_JSONC

#if HAVE_SQLITE == 1
/* SqdbSqlite.h */
#define SQ_CONFIG_HAVE_SQLITE
#endif
#undef HAVE_SQLITE

#if HAVE_MYSQL == 1
/* SqdbMysql.h */
#define SQ_CONFIG_HAVE_MYSQL
#endif
#undef HAVE_MYSQL

/* SqEntry.c */
// #define SQ_CONFIG_SQL_CASE_SENSITIVE

/* Sqxc can process (skip) unknown array & object */
// #define SQ_CONFIG_SQXC_UNKNOWN_SKIP

/* Enable "SQL_table_name" <-> "C struct type_name" converting. (SqSchema.h, SqUtil.h)
   When calling sq_schema_create_xxx():
     user only specify "SQL_table_name", program generate "C struct type_name".
     user only specify "C struct type_name", program generate "SQL_table_name".
 */
// #define SQ_CONFIG_NAMING_CONVENTION

// ----------------------------------------------------------------------------
// Default length (size)

/* Sqdb.c, SqSchema.c - SQL_STRING_LENGTH_DEFAULT */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191

/* SqEntry.c, SqType.c, SqSchema.c - SQ_TYPE_N_ENTRY_DEFAULT */
#define SQ_CONFIG_TYPE_N_ENTRY_DEFAULT            16

/* SqBuffer.c - SQ_BUFFER_SIZE_DEFAULT */
#define SQ_CONFIG_BUFFER_SIZE_DEAULT             128

/* SqxcSql.c */
#define SQ_CONFIG_SQXC_SQL_BUFFER_SIZE_DEAULT    256

/* SqType-PtrArray.c - SQ_TYPE_PTR_ARRAY_SIZE_DEFAULT */
#define SQ_CONFIG_TYPE_PTR_ARRAY_SIZE_DEFAULT     16

/* SqTable-relation.c */
#define SQ_CONFIG_TABLE_RELATION_SIZE             16    //  8

/* SqSchema-relation.c */
#define SQ_CONFIG_SCHEMA_RELATION_POOL_SIZE      128    // 64
#define SQ_CONFIG_SCHEMA_RELATION_SIZE            16    //  8

#endif  // SQ_CONFIG_H
