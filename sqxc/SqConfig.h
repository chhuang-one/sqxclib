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

#ifndef SQ_CONFIG_H
#define SQ_CONFIG_H


/* SqEntry.c */
// #define SQ_CONFIG_SQL_CASE_SENSITIVE    1

/* Sqdb.c, SqSchema.c - SQL_STRING_LENGTH_DEFAULT */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT    191

/* Enable "SQL_table_name" <-> "C struct type_name" converting. (SqSchema.h, SqUtil.h)
   When calling sq_schema_create_xxx():
     user only specify "SQL_table_name", program generate "C struct type_name".
     user only specify "C struct type_name", program generate "SQL_table_name".
 */
// #define SQ_CONFIG_HAVE_NAMING_CONVENTION    1


// ----------------------------------------------------------------------------
// Default size

/* SqEntry.c, SqType.c - SQ_TYPE_N_ENTRY_DEFAULT */
#define SQ_CONFIG_TYPE_N_ENTRY_DEFAULT    16

/* SqBuffer.c - SQ_BUFFER_SIZE_DEFAULT */
#define SQ_CONFIG_BUFFER_SIZE_DEAULT     128


#endif  // SQ_CONFIG_H
