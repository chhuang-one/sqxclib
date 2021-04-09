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

#include <stdio.h>
#include <string.h>

#include <SqError.h>
#include <SqConfig.h>
#include <SqSchema.h>
#include <SqRelation-migration.h>

#define SQ_TABLE_RELATION_SIZE          SQ_CONFIG_TABLE_RELATION_SIZE
#define SQ_SCHEMA_RELATION_SIZE         SQ_CONFIG_SCHEMA_RELATION_SIZE
#define SQ_SCHEMA_RELATION_POOL_SIZE    SQ_CONFIG_SCHEMA_RELATION_POOL_SIZE

// ----------------------------------------------------------------------------
// SqRelation functions for migration

void  sq_relation_exclude(SqRelation *relation, const void *from_object, const void *where_object_in) {
	SqRelationNode *rnode, *rnode_in, *rnode_pool;

	rnode = sq_relation_find(relation, from_object, NULL);
	if (rnode == NULL)
		return;
	rnode_in = sq_relation_find(relation, where_object_in, NULL);
	if (rnode_in == NULL)
		return;

	for (rnode_pool = rnode->next;  rnode_pool;  rnode_pool = rnode->next) {
		if (sq_relation_node_find(rnode_in, rnode_pool->object, NULL)) {
			rnode->next = rnode_pool->next;
			sq_relation_pool_free(relation->pool, rnode_pool);
		}
		else
			rnode = rnode->next;
	}
}

void *sq_relation_trace_reentry(SqRelation *relation, const char *old_name) {
	SqRelationNode *rnode;
	SqReentry    *reentry;

	rnode = sq_relation_find(relation, SQ_TYPE_REENTRY, NULL);
	if (rnode == NULL)
		return NULL;

	for (rnode = rnode->next;  ;  rnode = rnode->next) {
		if (rnode == NULL)
			return NULL;
		reentry = rnode->object;
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
		if (strcmp(reentry->old_name, old_name) == 0)
#else
		if (strcasecmp(reentry->old_name, old_name) == 0)
#endif
			break;
	}
	// get final renamed/dropped record
	rnode = sq_relation_find(relation, reentry, NULL);
	for (rnode = rnode->next;  rnode;  rnode = rnode->next) {
		if (SQ_TYPE_NOT_FAKE(rnode->object)) {    // rnode->object != SQ_TYPE_REENTRY
			reentry = rnode->object;
/*
			if (erase_traced) {
				rnode = sq_relation_find(relation, reentry, NULL);
				for (;  rnode;  rnode = rnode->next)
					sq_relation_erase(relation, SQ_TYPE_REENTRY, rnode->object, 0, NULL);
			}
 */
			return reentry;
		}
	}
	return reentry;
}

void  sq_relation_erase_reserve(SqRelation *relation, SqDestroyFunc destroy_func)
{
	SqRelationNode *rnode, *rnode_next;
	SqReentry    *reentry;

	rnode = sq_relation_find(relation, SQ_TYPE_RESERVE, NULL);
	if (rnode == NULL)
		return;
	rnode_next = rnode->next;
	rnode->next = NULL;

	for (rnode = rnode_next;  rnode;  rnode = rnode_next) {
		rnode_next = rnode->next;
		reentry = rnode->object;
		// free dropped record
		if (reentry->name == NULL && destroy_func)
			destroy_func(reentry);
		// free renamed record
		else if (reentry->bit_field & SQB_DYNAMIC) {
			free((char*)reentry->old_name);
			reentry->old_name = NULL;
			reentry->bit_field &= ~SQB_RENAMED;
		}
		sq_relation_pool_free(relation->pool, rnode);
	}
}

// ----------------------------------------------------------------------------
// SqTable functions for migration that using SqRelation

void  sq_table_create_relation(SqTable *table, SqRelationPool *pool) {
	SqType   *type;
	SqColumn *column;
//	int       n_foreign = 0;

	table->relation = sq_relation_new(pool, SQ_TABLE_RELATION_SIZE);
	type = (SqType*)table->type;
	for (int i = 0;  i < type->n_entry;  i++) {
		column = (SqColumn*)type->entry[i];
		if (column->foreign) {
			sq_relation_add(table->relation, SQ_TYPE_TRACING, column, 0);
//			n_foreign++;
			continue;
		}
		if (column->type == SQ_TYPE_INDEX || column->type == SQ_TYPE_CONSTRAINT)
			sq_relation_add(table->relation, SQ_TYPE_TRACING, column, 0);
	}

//	return n_foreign;
}

SqColumn *sq_table_replace_column(SqTable* table, SqColumn *old_column, SqColumn *new_column) {
	SqType *type = (SqType*)table->type;
	SqEntry **end, **cur;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		type = sq_type_copy_static(type, (SqDestroyFunc)sq_column_free);
		table->type = type;
	}
	if (table->relation)
		sq_relation_replace(table->relation, old_column, new_column, 0);

	for (end = type->entry + type->n_entry, cur = type->entry;  cur < end;  cur++) {
		if (*(SqColumn**)cur == old_column) {
			*(SqColumn**)cur  = new_column;
			sq_column_free(old_column);
			break;
		}
	}
	return new_column;
}

// ----------------------------------------------------------------------------
// SqSchema functions for migration that using SqRelation

void  sq_schema_create_relation(SqSchema *schema) {
	SqType  *type = (SqType*)schema->type;
	SqTable *table;

	schema->relation_pool = sq_relation_pool_create(SQ_SCHEMA_RELATION_POOL_SIZE);
	schema->relation = sq_relation_new(schema->relation_pool, SQ_SCHEMA_RELATION_SIZE);

	for (int i = 0;  i < type->n_entry;  i++) {
		table = (SqTable*)type->entry[i];
		// skip ALTER TABLE
		if (table->bit_field & SQB_CHANGED)
			continue;
		// skip DROP TABLE and RENAME TABLE
		if (table->old_name && (table->bit_field & SQB_RENAMED) == 0)
			continue;
		if (table->relation == NULL)
			sq_table_create_relation(table, schema->relation_pool);
	}
}
