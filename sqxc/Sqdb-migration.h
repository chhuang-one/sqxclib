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

#ifndef SQDB_MIGRATION_H
#define SQDB_MIGRATION_H

#include <SqSchema.h>

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* This used by migration: include and apply changes from 'schema_src'.
   It may move/steal tables and column from 'schema_src'.
   sq_schema_update() doesn't create and reserve data for SQLite table recreating.
 */
int  sq_schema_update(SqSchema *schema, SqSchema *schema_src);

#ifdef __cplusplus
}  // extern "C"
#endif



#endif  // SQDB_MIGRATION_H
