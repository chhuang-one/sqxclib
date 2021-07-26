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

void  sq_relation_erase_unsynced(SqRelation *relation, SqDestroyFunc destroy_func)
{
	SqRelationNode *rnode, *rnode_next;
	SqReentry    *reentry;

	rnode = sq_relation_find(relation, SQ_TYPE_UNSYNCED, NULL);
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
		else if (reentry->old_name && reentry->bit_field & SQB_DYNAMIC) {
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

SqColumn *sq_table_replace_column(SqTable *table, SqColumn *old_column, SqColumn *new_column) {
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

int   sq_table_include(SqTable *table, SqTable *table_src, SqSchema *schema)
{	//         *table,     *table_src
	SqColumn   *column,    *column_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	int       index;
	void    **addr;
	union {
		int    index;
		void **addr;
	} temp;

	if ((table->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		table->type = sq_type_copy_static(table->type, (SqDestroyFunc)sq_column_free);
	if ((table->type->bit_field & SQB_TYPE_SORTED) == 0)
		sq_type_sort_entry((SqType*)table->type);

	reentries = sq_type_get_ptr_array(table->type);
	reentries_src = sq_type_get_ptr_array(table_src->type);

	// if table is empty table
	if (reentries->length == 0) {
		// set SQB_CHANGED if it is "ALTER TABLE"
		if (table_src->bit_field & SQB_CHANGED)
			table->bit_field |= SQB_CHANGED;
	}

	for (index = 0;  index < reentries_src->length;  index++) {
		// TODO: NO_STEAL
		column_src = (SqColumn*)reentries_src->data[index];
		// steal 'column_src' if 'table_src->type' is not static.
		if (table_src->type->bit_field & SQB_TYPE_DYNAMIC)
			reentries_src->data[index] = NULL;

		if (column_src->bit_field & SQB_CHANGED) {
			// === ALTER COLUMN ===
			// free column if column->name == column_src->name
			addr = sq_ptr_array_find_sorted(reentries, column_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr) {
				column = *(SqColumn**)addr;
				// If column has foreign/composite key, add it to SQ_TYPE_TRACING
				sq_relation_erase(table->relation, SQ_TYPE_TRACING, column, 0, NULL);
				if (column_src->foreign || column_src->composite)
					sq_relation_add(table->relation, SQ_TYPE_TRACING, column_src, 0);
				// replace 'column' by 'column_src'
				sq_relation_replace(table->relation, column, column_src, 0);
				*addr = column_src;
				// calculate size
				sq_type_decide_size((SqType*)table->type, (SqEntry*)column_src, false);
				// free removed column
				sq_column_free(column);
				// set bit_field: column altered
				table->bit_field |= SQB_TABLE_COL_ALTERED;
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqTable %s: Can't alter column %s. It is not found.\n",
				        table->name, column_src->name);
			}
#endif
		}
		else if (column_src->name == NULL) {
			// === DROP COLUMN / CONSTRAINT / KEY ===
			// free column if column->name == column_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, column_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr) {
				column = *(SqColumn**)addr;
				// erase relation in SQ_TYPE_TRACING and SQ_TYPE_UNSYNCED if exist
				sq_relation_erase(table->relation, SQ_TYPE_TRACING, column, 0, NULL);
				sq_relation_erase(table->relation, SQ_TYPE_UNSYNCED, column, 0, NULL);
				// sq_schema_trace_name()
				sq_relation_replace(table->relation, column, column_src, 0);
				if (column->old_name == NULL)
					sq_relation_add(table->relation, SQ_TYPE_REENTRY, column_src, 0);
				// remove dropped column from array
				sq_ptr_array_steal(reentries, temp.index, 1);
				// calculate size
				sq_type_decide_size((SqType*)table->type, (SqEntry*)column, true);
				// remove dropped column from table->type->entry
				sq_column_free(column);
				// special case: drop index
				if (column_src->type == SQ_TYPE_INDEX)
					sq_relation_add(table->relation, SQ_TYPE_UNSYNCED, column_src, 0);
				else {
					// set bit_field: column dropped
					table->bit_field |= SQB_TABLE_COL_DROPPED;
				}
			}
			else {
#ifndef NDEBUG
				fprintf(stderr, "SqTable %s: Can't drop column %s. It is not found.\n",
				        table->name, column_src->old_name);
#endif
				sq_column_free(column_src);
			}
		}
		else if (column_src->old_name) {
			// === RENAME COLUMN ===
			// find column if column->name == column_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, column_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, NULL);
			// rename existing column->name to column_src->name
			if (addr) {
				// if there is a column that has the same name was erased/renamed
				if (sq_relation_trace_reentry(table->relation, column_src->name)) {
					sq_schema_trace_name(schema);
					sq_schema_erase_records(schema, '?');
				}
				// create dynamic column to replace static one if column is static
				column = *(SqColumn**)addr;
				if ((column->bit_field & SQB_DYNAMIC) == 0) {
					column = sq_column_copy_static(column);
					sq_relation_replace(table->relation, *addr, column, 0);
					*addr = column;
				}
				// sq_schema_trace_name()
				sq_relation_add(table->relation, column_src, column, 0);
				sq_relation_add(table->relation, SQ_TYPE_REENTRY, column_src, 0);
				// get new index after renaming
				sq_ptr_array_find_sorted(reentries, column_src->name,
						(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
				// change column name
				if (table->bit_field & SQB_TABLE_SQL_CREATED) {
					// unsynced record (it doesn't yet synchronize to database)
					if (sq_relation_find(table->relation, SQ_TYPE_UNSYNCED, column) == NULL) {
						sq_relation_add(table->relation, SQ_TYPE_UNSYNCED, column, 0);
						// store old_name temporary, program use it when SQLite recreate table
						if (column->old_name == NULL) {
							column->old_name = column->name;
							column->name = NULL;
							column->bit_field |= SQB_RENAMED;
						}
					}
				}
				free((char*)column->name);
				column->name = strdup(column_src->name);
				// move
				temp.addr = reentries->data + temp.index;
				if (temp.addr < addr)          // insert after
					memmove(temp.addr +1, temp.addr, (char*)addr - (char*)temp.addr);
				else if (temp.addr != addr)    // insert before
					memmove(addr, addr +1, (char*)(--temp.addr) - (char*)addr);
				*temp.addr = column;
				// set bit_field: column renamed
				table->bit_field |= SQB_TABLE_COL_RENAMED;
			}
			else {
#ifndef NDEBUG
				fprintf(stderr, "SqTable %s: Can't rename column %s. It is not found.\n",
				        table->name, column_src->old_name);
#endif
				sq_column_free(column_src);
			}
		}
		else {
			// === ADD COLUMN / CONSTRAINT / KEY ===
			// get inserting position
			addr = sq_ptr_array_find_sorted(reentries, column_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr == NULL) {
				// if there is a column that has the same name was erased/renamed
				if (sq_relation_trace_reentry(table->relation, column_src->name)) {
					sq_schema_trace_name(schema);
					sq_schema_erase_records(schema, '?');
				}
				// sq_schema_trace_name()
				if (column_src->foreign || column_src->composite)
					sq_relation_add(table->relation, SQ_TYPE_TRACING, column_src, 0);
				if (table->bit_field & SQB_TABLE_SQL_CREATED || column_src->type == SQ_TYPE_INDEX)
					sq_relation_add(table->relation, SQ_TYPE_UNSYNCED, column_src, 0);
				// add 'reentry_src' to entry->type.
				sq_ptr_array_insert(reentries, temp.index, column_src);
				// steal 'reentry_src' from 'entry_src->type'. TODO: NO_STEAL
//				if (table_src->type->bit_field & SQB_DYNAMIC)
//					reentries_src->data[index] = NULL;
				// calculate size
				sq_type_decide_size((SqType*)table->type, (SqEntry*)column_src, false);
				// set bit_field: column added
				if (column_src->type == SQ_TYPE_CONSTRAINT)
					table->bit_field |= SQB_TABLE_COL_ADDED_CONSTRAINT;
				else if (column_src->bit_field & (SQB_UNIQUE | SQB_PRIMARY))
					table->bit_field |= SQB_TABLE_COL_ADDED_UNIQUE;
				else if (column_src->default_value) {
					// DEFAULT (expression)
					if (column_src->default_value[0] == '(')
						table->bit_field |= SQB_TABLE_COL_ADDED_EXPRESSION;
					// DEFAULT CURRENT_TIME, CURRENT_DATE, or CURRENT_TIMESTAMP...etc
					else if (column->bit_field & SQB_CURRENT || strncasecmp("CURRENT_", column_src->default_value, 8) == 0)
						table->bit_field |= SQB_TABLE_COL_ADDED_CURRENT_TIME;
					else
						table->bit_field |= SQB_TABLE_COL_ADDED;
				}
				else
					table->bit_field |= SQB_TABLE_COL_ADDED;
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqTable: column %s is exist.\n", column_src->name);
			}
#endif
		}
	}

	return SQCODE_OK;
}

void  sq_table_erase_records(SqTable *table, char version_comparison)
{
	// copy table->type if it is static SqType.
	if ((table->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		table->type = sq_type_copy_static(table->type, (SqDestroyFunc)sq_column_free);

	// erase relation for renamed & dropped records in table
	sq_relation_exclude(table->relation, SQ_TYPE_REENTRY, SQ_TYPE_UNSYNCED);
	sq_relation_erase(table->relation, SQ_TYPE_REENTRY, NULL, -1, (SqDestroyFunc)sq_column_free);
	// if database schema version == current schema version
	if (version_comparison == '=') {
		// free not synced record
		sq_relation_erase_unsynced(table->relation, (SqDestroyFunc)sq_column_free);
		// reset data in synced table
		table->bit_field |= SQB_TABLE_SQL_CREATED;
		table->bit_field &= ~(SQB_TABLE_COL_CHANGED | SQB_CHANGED | SQB_RENAMED);
	}
	sq_relation_remove_empty(table->relation);
}

void   sq_table_complete(SqTable *table, bool no_need_to_sync)
{
	SqPtrArray* reentries;
	SqColumn*   column;
	bool        has_null = false;

	if (no_need_to_sync && table->relation) {
		sq_relation_free(table->relation);
		table->relation = NULL;
	}

	if (table->type->bit_field & SQB_DYNAMIC) {
		reentries = sq_type_get_ptr_array(table->type);
		for (int index = 0;  index < reentries->length;  index++) {
			column = reentries->data[index];
			if (column == NULL)
				has_null = true;
			else if (no_need_to_sync && (column->type == SQ_TYPE_INDEX || column->type == SQ_TYPE_CONSTRAINT)) {
				sq_column_free(column);
				reentries->data[index] = NULL;
				has_null = true;
			}
			else if (column->bit_field & (SQB_RENAMED | SQB_DYNAMIC)) {
				free((char*)column->old_name);
				column->old_name = NULL;
			}
		}
		if (has_null)
			sq_reentries_remove_null(reentries, 0);
		// sort columns by name
		sq_type_sort_entry((SqType*)table->type);
	}
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

int   sq_schema_include(SqSchema *schema, SqSchema *schema_src)
{	//         *schema,    *schema_src
	SqTable    *table,     *table_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	int      index;
	void   **addr;
	union {
		int    index;
		void **addr;
	} temp;

	reentries = sq_type_get_ptr_array(schema->type);
	reentries_src = sq_type_get_ptr_array(schema_src->type);

	// run sq_schema_include() first time.
	if (schema->relation == NULL)
		sq_schema_create_relation(schema);
	if ((schema->type->bit_field & SQB_TYPE_SORTED) == 0)
		sq_type_sort_entry((SqType*)schema->type);

	for (index = 0;  index < reentries_src->length;  index++) {
		table_src = (SqTable*)reentries_src->data[index];
		if (table_src->bit_field & SQB_CHANGED) {
			// === ALTER TABLE ===
			// find table if table->name == table_src->name
			addr = sq_ptr_array_find_sorted(reentries, table_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, NULL);
			if (addr) {
				table = *(SqTable**)addr;
				sq_table_include(table, table_src, schema);    // TODO: NO_STEAL
				// If table has foreign/composite key, add it to SQ_TYPE_TRACING
				if (sq_relation_find(table->relation, SQ_TYPE_TRACING, NULL))
					sq_relation_add(schema->relation, SQ_TYPE_TRACING, table, 0);
				else
					sq_relation_erase(schema->relation, SQ_TYPE_TRACING, table, 0, NULL);
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqSchema: Can't alter table %s. It is not found.\n", table_src->name);
			}
#endif
			// It doesn't need to steal 'table_src' from 'schema_src'
			continue;
		}
		else if (table_src->name == NULL) {
			// === DROP TABLE ===
			// erase original table if table->name == table_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, table_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr) {
				table = *(SqTable**)addr;
				// trace foreign or composite
				sq_relation_erase(schema->relation, SQ_TYPE_TRACING, table, 0, NULL);
				// sq_schema_trace_name(): if 'table' is not renamed table
				if (table->old_name == NULL)
					sq_relation_add(schema->relation, SQ_TYPE_REENTRY, table, 0);
				// reserved record (it doesn't yet synchronize to database)
				if (table->bit_field & SQB_TABLE_SQL_CREATED) {
					if (sq_relation_find(schema->relation, SQ_TYPE_UNSYNCED, table) == NULL)
						sq_relation_add(schema->relation, SQ_TYPE_UNSYNCED, table, 0);
				}
				// 'table' become to dropped record
				if (table->old_name == NULL)
					table->old_name = table->name;
				else
					free((char*)table->name);
				table->name = NULL;
				table->bit_field &= ~SQB_RENAMED;
				// remove dropped table from schema->type->entry
				sq_ptr_array_steal(reentries, temp.index, 1);
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqSchema: Can't drop table %s. It is not found.\n", table_src->old_name);
			}
#endif
			// It doesn't need to steal 'table_src' from 'schema_src'
			continue;
		}
		else if (table_src->old_name) {
			// === RENAME TABLE ===
			// find existing if table->name == table_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, table_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, NULL);
			if (addr) {
				// if there is a table that has the same name was erased/renamed
				if (sq_relation_trace_reentry(schema->relation, table_src->name)) {
					sq_schema_trace_name(schema);
					sq_schema_erase_records(schema, '?');
				}
				table = *(SqTable**)addr;
				// sq_schema_trace_name()
				sq_relation_add(schema->relation, table_src, table, 0);
				sq_relation_add(schema->relation, SQ_TYPE_REENTRY, table_src, 0);
				// get new index after renaming
				sq_ptr_array_find_sorted(reentries, table_src->name,
						(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
				// change table name
				if (table->bit_field & SQB_TABLE_SQL_CREATED) {
					// unsynced record (it doesn't yet synchronize to database)
					if (sq_relation_find(schema->relation, SQ_TYPE_UNSYNCED, table) == NULL) {
						sq_relation_add(schema->relation, SQ_TYPE_UNSYNCED, table, 0);
						// store old_name temporary, if table's name doesn't yet synchronize to database
						if (table->old_name == NULL) {
							table->old_name = table->name;    // strdup(table->name)
							table->name = NULL;
							table->bit_field |= SQB_RENAMED;
						}
					}
				}
				free((char*)table->name);
				table->name = strdup(table_src->name);
				// move
				temp.addr = reentries->data + temp.index;
				if (temp.addr < addr)          // insert after
					memmove(temp.addr +1, temp.addr, (char*)addr - (char*)temp.addr);
				else if (temp.addr != addr)    // insert before
					memmove(addr, addr +1, (char*)(--temp.addr) - (char*)addr);
				*temp.addr = table;
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqSchema: Can't rename table %s. It is not found.\n", table_src->old_name);
			}
#endif
		}
		else {
			// === ADD TABLE ===
			// get inserting position
			addr = sq_ptr_array_find_sorted(reentries, table_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr == NULL) {
				// if there is a table that has the same name was erased/renamed
				if (sq_relation_trace_reentry(schema->relation, table_src->name)) {
					sq_schema_trace_name(schema);
					sq_schema_erase_records(schema, '?');
				}
				// sq_schema_trace_name()
				if (table_src->relation == NULL)
					sq_table_create_relation(table_src, schema->relation_pool);
				if (sq_relation_find(table_src->relation, SQ_TYPE_TRACING, NULL))
					sq_relation_add(schema->relation, SQ_TYPE_TRACING, table_src, 0);
				sq_relation_add(schema->relation, SQ_TYPE_UNSYNCED, table_src, 0);
				// add 'reentry_src' to entry->type.
				sq_ptr_array_insert(reentries, temp.index, table_src);
				// steal 'reentry_src' from 'entry_src->type'. TODO: NO_STEAL
//				if (table_src->type->bit_field & SQB_DYNAMIC)
//					reentries_src->data[index] = NULL;
			}
#ifndef NDEBUG
			else {
				fprintf(stderr, "SqSchema: table %s is exist.\n", table_src->name);
			}
#endif
		}

		// TODO: NO_STEAL
		// steal 'table_src' from 'schema_src->type->entry'.
		reentries_src->data[index] = NULL;
	}

	// update schema version
	schema->version = schema_src->version;

	return SQCODE_OK;
}

int     sq_schema_trace_name(SqSchema *schema)
{
	SqRelationNode *node_table, *node_column;
	SqTable    *table,  *table_fore;
	SqColumn   *column;
	SqReentry  *reentry;
	int         result = SQCODE_OK;

	// get tables that has foreign key
	node_table = sq_relation_find(schema->relation, SQ_TYPE_TRACING, NULL);
	if (node_table == NULL)
		return SQCODE_OK;
	// for each tables that has foreign key
	for (node_table = node_table->next;  node_table;  node_table = node_table->next) {
		table = node_table->object;
		// get columns that has foreign reference
		node_column = sq_relation_find(table->relation, SQ_TYPE_TRACING, NULL);
		if (node_column == NULL)
			continue;
		// for each columns that has foreign reference
		for (node_column = node_column->next;  node_column;  node_column = node_column->next) {
			column = node_column->object;

			// --------------------------------------------
			// trace renamed column in composite key
			if (column->composite) {
				for (int index = 0;  column->composite[index];  index++) {
					reentry = sq_relation_trace_reentry(table->relation, column->composite[index]);
					if (reentry == NULL)
						continue;
					// column dropped.
					if (reentry->name == NULL) {
						result = SQCODE_REENTRY_DROPPED;
#ifndef NDEBUG
						fprintf(stderr, "SqSchema: column %s has been dropped.\n", column->composite[index]);
#endif
						continue;   // error...
					}
					// column has been renamed
					if ((column->bit_field & SQB_DYNAMIC) == 0)
						column = sq_table_replace_column(table, column, sq_column_copy_static(column));
					free(column->composite[index]);
					column->composite[index] = strdup(reentry->name);
				}
			}

			// --------------------------------------------
			// trace renamed table
			if (column->foreign == NULL)
				continue;
			reentry = sq_relation_trace_reentry(schema->relation, column->foreign->table);
			if (reentry) {
				// table dropped.
				if (reentry->name == NULL) {
					result = SQCODE_REENTRY_DROPPED;
#ifndef NDEBUG
					fprintf(stderr, "SqSchema: foreign table %s has been dropped.\n", column->foreign->table);
#endif
					continue;   // error...
				}
				// table has been renamed
				else if ((column->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic column to replace static one
					column = sq_table_replace_column(table, column, sq_column_copy_static(column));
				}
				free((char*)column->foreign->table);
				column->foreign->table = strdup(reentry->name);
			}

			// --------------------------------------------
			// find referenced table
			table_fore = (SqTable*)sq_ptr_array_find_sorted(sq_type_get_ptr_array(schema->type),
					column->foreign->table, (SqCompareFunc) sq_entry_cmp_str__name, NULL);
			if (table_fore)
				table_fore = *(SqTable**)table_fore;
			else {
				// (renamed) table not found. dropped?
				result = SQCODE_REFERENCE_NOT_FOUND;
#ifndef NDEBUG
				fprintf(stderr, "SqSchema: foreign table %s not found.\n", column->foreign->table);
#endif
				continue;  // error...
			}

			// --------------------------------------------
			// trace renamed column
			reentry = sq_relation_trace_reentry(table_fore->relation, column->foreign->column);
			if (reentry) {
				// column dropped.
				if (reentry->name == NULL) {
					result = SQCODE_REENTRY_DROPPED;
#ifndef NDEBUG
					fprintf(stderr, "SqSchema: foreign column %s has been dropped.\n", column->foreign->column);
#endif
					continue;   // error...
				}
				// column has been renamed
				else if ((column->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic column to replace static one
					column = sq_table_replace_column(table, column, sq_column_copy_static(column));
				}
				free((char*)column->foreign->column);
				column->foreign->column = strdup(reentry->name);
			}
		}
	}
	return result;
}

void  sq_schema_erase_records(SqSchema *schema, char version_comparison)
{
	SqType *type;

	// erase relation for renamed & dropped records in schema
	sq_relation_exclude(schema->relation, SQ_TYPE_REENTRY, SQ_TYPE_UNSYNCED);
	sq_relation_erase(schema->relation, SQ_TYPE_REENTRY, NULL, -1, (SqDestroyFunc)sq_table_free);

	// if database schema version == current schema version
	if (version_comparison == '=') {
		// free not synced record
		sq_relation_erase_unsynced(schema->relation, (SqDestroyFunc)sq_table_free);
		// reset data in schema
		schema->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);
	}
	sq_relation_remove_empty(schema->relation);

	type = (SqType*)schema->type;
	// erase renamed & dropped records of column in tables
	for (int index = 0;  index < type->n_entry;  index++)
		sq_table_erase_records((SqTable*)type->entry[index], version_comparison);
}

void    sq_schema_complete(SqSchema *schema, bool no_need_to_sync)
{
	SqPtrArray *entries;
	SqTable    *table;
	bool        has_null;

	entries = sq_type_get_ptr_array(schema->type);
	sq_type_sort_entry((SqType*)schema->type);

	for (int index = 0;  index < entries->length;  index++) {
		table = entries->data[index];
		if (table == NULL)
			has_null = true;
//		table->offset = 0;
		sq_table_complete(table, no_need_to_sync);
	}
	if (has_null)
		sq_reentries_remove_null(entries, 0);

	if (no_need_to_sync) {
		sq_relation_free(schema->relation);
		sq_relation_pool_destroy(schema->relation_pool);
	}
}
