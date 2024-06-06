/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#ifndef SQ_RELATION_MIGRATION_H
#define SQ_RELATION_MIGRATION_H

#include <SqType.h>
#include <SqRelation.h>
#include <SqSchema.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

// These types are for internal use only
#define SQ_TYPE_TRACING    SQ_TYPE_FAKE4    // column (or table) has foreign/composite key
#define SQ_TYPE_UNSYNCED   SQ_TYPE_FAKE5    // records, columns, or tables that doesn't yet synchronize to database
#define SQ_TYPE_REENTRY    SQ_TYPE_FAKE6    // renamed and dropped records

/*
Relation:
    SQ_TYPE_REENTRY: It contain renamed and dropped records that used by sq_relation_trace_reentry()
        renamed record <-----> renamed column (or table)
        renamed record <-----> dropped record
        dropped record

    SQ_TYPE_UNSYNCED: It contain records, columns, or tables that have not synchronize to database.
                      sq_relation_erase_unsynced() can free dropped records and clear renaming data.
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

// This function free not synced records in SQ_TYPE_UNSYNCED.
void  sq_relation_erase_unsynced(SqRelation *relation, SqDestroyFunc destroy_func);

/* --- SqTable migration functions --- */

void      sq_table_create_relation(SqTable *table, SqRelationPool *pool);
SqColumn *sq_table_replace_column(SqTable *table, SqColumn *old_column, SqColumn *new_column);

/* This is used by migration: include and apply changes from 'table_src'.
   It may move/steal columns from 'table_src'.
   sq_table_include() will create and reserve data for SQLite table recreating.
 */
int       sq_table_include(SqTable *table, SqTable *table_src, SqSchema *schema);

/* erase ernamed & dropped records after calling sq_schema_include() and sq_schema_trace_name()
   if database schema version <  current schema version, pass 'version_comparison' = '<'
   if database schema version == current schema version, pass 'version_comparison' = '='

   return number of old columns after erasing.
 */
void      sq_table_erase_records(SqTable *table, char version_comparison);

void      sq_table_complete(SqTable *table, bool no_need_to_sync);


/* --- SqSchema functions --- */

void  sq_schema_create_relation(SqSchema *schema);

/* This is used by migration: include and apply changes from 'schema_src'.
   It may move/steal tables and column from 'schema_src'.
   sq_schema_include() will create and reserve data for SQLite table recreating.
 */
int   sq_schema_include(SqSchema *schema, SqSchema *schema_src);

// It trace renamed (or dropped) table (and column) that was referenced by others and update others references.
// use this function after calling sq_schema_include()
int   sq_schema_trace_name(SqSchema *schema);

/* erase renamed & dropped records after calling sq_schema_include() and sq_schema_trace_name()
   if database schema version <  current schema version, pass 'version_comparison' = '<'
   if database schema version == current schema version, pass 'version_comparison' = '='
 */
void  sq_schema_erase_records(SqSchema *schema, char version_comparison);

// call this function after synchronize schema to database (creating/altering database tables).
// It will free temporary data.
// If 'no_need_to_sync' == true, it will free unused index and composite constraint in memory.
// set 'no_need_to_sync' to false if your program needs to synchronize schema to the SQLite database at any time.
void  sq_schema_complete(SqSchema *schema, bool no_need_to_sync);


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_RELATION_MIGRATION_H
