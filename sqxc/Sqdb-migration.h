/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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

#include <SqConfig.h>
#include <SqSchema.h>

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* This is used by migration: include and apply changes from 'schema_src'.
   It may move/steal tables and column from 'schema_src'.
   sq_schema_update() doesn't create and reserve data for SQLite table recreating.
 */
int  sq_schema_update(SqSchema *schema, SqSchema *schema_src);


#if SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB

/* This is used by migration: remove columns that have fake type.
   call this function when program synchronize schema to database.
 */
void sq_schema_erase_fake_type(SqSchema *schema);

//void sq_schema_remove_fake_type(SqSchema *schema);
#define sq_schema_remove_fake_type        sq_schema_erase_fake_type

#endif  // SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB


#ifdef __cplusplus
}  // extern "C"
#endif



#endif  // SQDB_MIGRATION_H
