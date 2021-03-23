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

//      SQ_TYPE_INDEX    SQ_TYPE_FAKE1
#define SQ_TYPE_FOREIGN  SQ_TYPE_FAKE2    // for internal use only
#define SQ_TYPE_REENTRY  SQ_TYPE_FAKE3    // for internal use only
//#define SQ_TYPE_RESERVE  SQ_TYPE_FAKE4    // for internal use only

#ifdef __cplusplus
extern "C" {
#endif

// This function trace related list of SQ_TYPE_REENTRY by SqReentry.old_name.
// if 'erase_traced' == true, it will erase node that has been traced in related list of SQ_TYPE_REENTRY.
const char *sq_relation_trace_reentry(SqRelation *relation, const char *old_name, bool erase_traced);

void  sq_table_create_relation(SqTable *table, SqRelationPool *pool);

void  sq_schema_create_relation(SqSchema *schema);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_RELATION_MIGRATION_H
