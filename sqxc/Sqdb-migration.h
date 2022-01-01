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


int   sq_entry_update(SqEntry *entry, SqEntry *entry_src, SqDestroyFunc destroy_func);

#define sq_schema_update(schema, schema_src)    \
        sq_entry_update((SqEntry*)schema, (SqEntry*)schema_src, (SqDestroyFunc)sq_table_free)

#ifdef __cplusplus
}  // extern "C"
#endif



#endif  // SQDB_MIGRATION_H
