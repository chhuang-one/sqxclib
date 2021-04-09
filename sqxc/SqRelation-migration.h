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

#ifndef SQ_RELATION_MIGRATION_H
#define SQ_RELATION_MIGRATION_H

#include <SqType.h>
#include <SqRelation.h>
#include <SqSchema.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

// These types are for internal use only
#define SQ_TYPE_TRACING    SQ_TYPE_FAKE3    // column/table has foreign/composite key
#define SQ_TYPE_RESERVE    SQ_TYPE_FAKE4    // not yet synchronize to database
#define SQ_TYPE_REENTRY    SQ_TYPE_FAKE5

/*
Relation:
  SQ_TYPE_REENTRY: It contain renamed and dropped records that used by sq_relation_trace_reentry()
    renamed record <-----> renamed column/table
    renamed record <-----> dropped record
  SQ_TYPE_RESERVE: It contain renamed and dropped records that have not synchronize to database.
 */

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- SqRelation functions --- */

// This function remove objects from related list of 'from_object' if there are the same objects in 'where_object_in'.
void  sq_relation_exclude(SqRelation *relation, const void *from_object, const void *where_object_in);

// This function trace related list of SQ_TYPE_REENTRY by SqReentry.old_name
void *sq_relation_trace_reentry(SqRelation *relation, const char *old_name);

// This function free not synced records in SQ_TYPE_RESERVE.
void  sq_relation_erase_reserve(SqRelation *relation, SqDestroyFunc destroy_func);

/* --- SqTable functions --- */

void      sq_table_create_relation(SqTable *table, SqRelationPool *pool);
SqColumn *sq_table_replace_column(SqTable* table, SqColumn *old_column, SqColumn *new_column);

/* --- SqSchema functions --- */

void  sq_schema_create_relation(SqSchema *schema);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_RELATION_MIGRATION_H
