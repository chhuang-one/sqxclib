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

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqBuffer.h>
#include <SqUtil.h>
#include <SqSchema.h>
#include <SqRelation-migration.h>  // migration

#define SCHEMA_INITIAL_VERSION       0
#define SQL_STRING_LENGTH_DEFAULT    SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT
#define SQ_TYPE_N_ENTRY_DEFAULT      SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

void  sq_schema_init(SqSchema* schema, const char* name)
{
	static int cur_version = SCHEMA_INITIAL_VERSION;
	SqType* typeinfo;

	typeinfo = sq_type_new(SQ_TYPE_N_ENTRY_DEFAULT, (SqDestroyFunc)sq_table_free);
	typeinfo->parse = NULL;
	typeinfo->write = NULL;

	sq_entry_init((SqEntry*)schema, typeinfo);
	schema->name = name ? strdup(name) : NULL;
	// version count
	schema->version = cur_version++;
	// new relation
	schema->relation = NULL;
	schema->relation_pool = NULL;
}

void  sq_schema_final(SqSchema* schema)
{
	// reduce the stack frame:
	// sq_type_unref() will not be called by below sq_entry_final()
	sq_type_unref((SqType*)schema->type);
	schema->type = NULL;
	// finalize parent struct - SqEntry
	sq_entry_final((SqEntry*)schema);
}

SqSchema*  sq_schema_new(const char* name)
{
	SqSchema* schema;

	schema = malloc(sizeof(SqSchema));
	sq_schema_init(schema, name);
	return schema;
}

void  sq_schema_free(SqSchema* schema)
{
	sq_schema_final(schema);
	free(schema);
}

SqTable* sq_schema_create_full(SqSchema* schema,
                               const char* table_name,
                               const char* type_name,
                               const SqType* type_info,
                               size_t instance_size)
{
	SqTable*  table;

	if (schema->version == 0)
		schema->version++;
	table = sq_table_new(table_name, type_info);
	// if type_info == NULL,
	// table->type is dynamic type and table->bit_field has SQB_TYPE_DYNAMIC
	if (type_info == NULL) {
		((SqType*)table->type)->size = instance_size;
		if (type_name)
			((SqType*)table->type)->name = strdup(type_name);
	}

#ifdef SQ_CONFIG_NAMING_CONVENTION
	if (table->name == NULL)
		table->name = sq_name2table(table->type->name);
	if (table->type->name == NULL)
		table->type->name = sq_name2type(table->name);
#endif

	// add table in schema->type
	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_CHANGED;
	return table;
}

SqTable* sq_schema_alter(SqSchema* schema, const char* name, const SqType* type_info)
{
	SqTable*  table;

	table = sq_table_new(name, type_info);
	table->bit_field |= SQB_CHANGED;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_CHANGED;
	return table;
}

void sq_schema_drop(SqSchema* schema, const char* name)
{
	SqTable* table;

	table = calloc(1, sizeof(SqTable));
	table->old_name = strdup(name);
	table->name = NULL;
	table->bit_field = SQB_DYNAMIC;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_CHANGED;

#if 0
	// remove table in schema->type
	addr = sq_type_find_entry(schema->type, name, NULL);
	if (addr)
		sq_type_erase_entry_addr(schema->type, addr, 1);
#endif
}

void sq_schema_rename(SqSchema* schema, const char* from, const char* to)
{
	SqTable* table;

	table = calloc(1, sizeof(SqTable));
	table->old_name = strdup(from);
	table->name = strdup(to);
	table->bit_field = SQB_DYNAMIC;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_CHANGED;

#if 0
//	table = (SqTable*)sq_type_find_entry(schema->type, from,
//	                                     (SqCompareFunc)sq_entry_cmp_str__name);
	table = (SqTable*)sq_type_find_entry(schema->type, from, NULL);
	if (table) {
		table = *(SqTable**)table;
#if 0
		if (table->old_name == NULL)
			table->old_name = table->name;
		else
#endif
			free(table->name);
		table->name = strdup(to);
		table->bit_field |= SQB_RENAMED;
		schema->type->bit_field &= ~SQB_TYPE_SORTED;
	}
#endif
}

SqTable* sq_schema_find(SqSchema* schema, const char* name)
{
	SqCompareFunc cmp_func;
	void** addr;

	// if cmp_func == NULL, sq_type_find_entry() will sort entry before finding.
	if (schema->bit_field & SQB_CHANGED)
		cmp_func = (SqCompareFunc)sq_reentry_cmp_str__name;
	else
		cmp_func = NULL;

	addr = sq_type_find_entry(schema->type, name, cmp_func);
	if (addr)
		return *addr;
	else
		return NULL;
}

int   sq_schema_include(SqSchema* schema, SqSchema* schema_src)
{	//         *schema,    *schema_src
	SqTable    *table,     *table_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	int      index;
	void   **addr;

	reentries = sq_type_get_ptr_array(schema->type);
	reentries_src = sq_type_get_ptr_array(schema_src->type);

	// run sq_schema_include() first time.
	if (schema->relation == NULL)
		sq_schema_create_relation(schema);

	for (index = 0;  index < reentries_src->length;  index++) {
		table_src = (SqTable*)reentries_src->data[index];
		if (table_src->bit_field & SQB_CHANGED) {
			// === ALTER TABLE ===
			// find table if table->name == table_src->name
			addr = sq_reentries_find_name(reentries, table_src->name);
			if (addr) {
				table = *(SqTable**)addr;
				sq_table_include(table, table_src);    // TODO: NO_STEAL
				// relation
				if (sq_relation_find(table->relation, SQ_TYPE_TRACING, NULL))
					sq_relation_add(schema->relation, SQ_TYPE_TRACING, table, 0);
				else
					sq_relation_erase(schema->relation, SQ_TYPE_TRACING, table, 0, NULL);
			}
#ifdef DEBUG
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
			addr = sq_reentries_find_name(reentries, table_src->old_name);
			if (addr) {
				table = *(SqTable**)addr;
				// trace foreign or composite
				sq_relation_erase(schema->relation, SQ_TYPE_TRACING, table, 0, NULL);
				// if 'table' is not renamed table
				if ((table->bit_field & SQB_RENAMED) == 0)
					sq_relation_add(schema->relation, SQ_TYPE_REENTRY, table, 0);
				// reserve record for existing table in database
				if (table->bit_field & SQB_TABLE_SQL_CREATED) {
					if (sq_relation_find(schema->relation, SQ_TYPE_RESERVE, table) == NULL)
						sq_relation_add(schema->relation, SQ_TYPE_RESERVE, table, 0);
				}
				// 'table' become to dropped record
				if (table->old_name == NULL)
					table->old_name = table->name;
				else
					free((char*)table->name);
				table->name = NULL;
				table->bit_field &= ~SQB_RENAMED;
				// dropped record must remove from schema->type->entry
				*addr = NULL;
			}
#if DEBUG
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
			addr = sq_reentries_find_name(reentries, table_src->old_name);
			if (addr) {
				if (sq_relation_trace_reentry(schema->relation, table_src->name)) {
					// There is a column that has the same name was erased/renamed
					sq_schema_trace_name(schema);
					sq_schema_erase_records(schema, '?');
				}
				// rename existing table->name to table_src->name
				table = *(SqTable**)addr;
				// rename 'table'
				if (table->old_name == NULL)
					table->old_name = table->name;    // strdup(table->name)
				else
					free((char*)table->name);
				table->name = strdup(table_src->name);
				table->bit_field |= SQB_RENAMED;
				// trace_name
				sq_relation_add(schema->relation, table_src, table, 0);
				sq_relation_add(schema->relation, SQ_TYPE_REENTRY, table_src, 0);
				// reserve record for existing table in database
				if (table->bit_field & SQB_TABLE_SQL_CREATED) {
					if (sq_relation_find(schema->relation, SQ_TYPE_RESERVE, table) == NULL)
						sq_relation_add(schema->relation, SQ_TYPE_RESERVE, table, 0);
				}
			}
#if DEBUG
			else {
				fprintf(stderr, "SqSchema: Can't rename table %s. It is not found.\n", table_src->old_name);
			}
#endif
		}
		else {
			// === ADD TABLE ===
			if (sq_relation_trace_reentry(schema->relation, table_src->name)) {
				// There is a column that has the same name was erased/renamed
				sq_schema_trace_name(schema);
				sq_schema_erase_records(schema, '?');
			}

			if (table_src->relation == NULL)
				sq_table_create_relation(table_src, schema->relation_pool);
			if (sq_relation_find(table_src->relation, SQ_TYPE_TRACING, NULL))
				sq_relation_add(schema->relation, SQ_TYPE_TRACING, table_src, 0);
			// add 'table_src' to schema->type->entry.
			sq_reentries_add(reentries, table_src);
		}

		// TODO: NO_STEAL
		// steal 'table_src' from 'schema_src->type->entry'.
		reentries_src->data[index] = NULL;
	}

	// update other data in SqSchema
	schema->version = schema_src->version;
	((SqType*)schema->type)->bit_field &= ~SQB_TYPE_SORTED;

	return SQCODE_OK;
}

int     sq_schema_trace_name(SqSchema* schema)
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
					reentry = sq_relation_trace_reentry(schema->relation, column->composite[index]);
					if (reentry == NULL)
						continue;
					// column dropped.
					if (reentry->name == NULL) {
						result = SQCODE_REENTRY_DROPPED;
#ifdef DEBUG
						fprintf(stderr, "SqSchema: column %s has been dropped.\n", column->composite[index]);
#endif
						continue;   // error...
					}
					// column has been renamed
					if ((column->bit_field & SQB_DYNAMIC) == 0)
						column = sq_table_replace_column(table, column, sq_column_copy_static(column));
					free(column->composite[index]);
					column->composite[index] = strdup(column->composite[index]);
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
#ifdef DEBUG
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
			table_fore = (SqTable*)sq_reentries_find_name(sq_type_get_ptr_array(schema->type),
			                                              column->foreign->table);
			if (table_fore)
				table_fore = *(SqTable**)table_fore;
			else {
				// (renamed) table not found. dropped?
				result = SQCODE_REFERENCE_NOT_FOUND;
#ifdef DEBUG
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
#ifdef DEBUG
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

void  sq_schema_erase_records(SqSchema* schema, char version_comparison)
{
	SqType *type;

	// erase relation for renamed & dropped records in schema
	sq_relation_exclude(schema->relation, SQ_TYPE_REENTRY, SQ_TYPE_RESERVE);
	sq_relation_erase(schema->relation, SQ_TYPE_REENTRY, NULL, -1, (SqDestroyFunc)sq_table_free);

	// if database schema version == current schema version
	if (version_comparison == '=') {
		// free not synced record
		sq_relation_erase_reserve(schema->relation, (SqDestroyFunc)sq_table_free);
		// reset data in schema
		schema->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);
		schema->offset = schema->type->n_entry;    // schema->offset is number of synced tables
	}
	sq_relation_remove_empty(schema->relation);

	type = (SqType*)schema->type;
	// erase renamed & dropped records of column in tables
	for (int index = 0;  index < type->n_entry;  index++)
		sq_table_erase_records((SqTable*)type->entry[index], version_comparison);
}

// used by sq_schema_arrange()
// trace column reference and count order
// before calling this function:
// 1. sort schema->type->entry by table->name
// 2. sort table->extra->foreigns by column->foreign->table
// 3. reset all table->offset = 0
static int  count_table_order(SqSchema *schema, SqTable *table, int *is_reo)
{
	SqRelationNode *node;
	SqColumn       *column;
	SqTable        *fore_table;

	if (table->offset > 0)
		return table->offset;
	table->offset = 1;
	table->bit_field |= SQB_TABLE_REO_CHECKING;
	node = sq_relation_find(table->relation, SQ_TYPE_TRACING, NULL);

	for (node = node->next;  node;  node = node->next) {
		column = node->object;
//		fore_table = sq_type_find_entry(schema->type, column->foreign->table, NULL);
		fore_table = sq_ptr_array_search(&schema->type->entry, column->foreign->table,
		                                 (SqCompareFunc)sq_entry_cmp_str__name);
		if (fore_table)
			fore_table = *(SqTable**)fore_table;
		else {
			// error...
			continue;
		}

		if (fore_table->bit_field & SQB_TABLE_REO_CHECKING) {
			// if tables reference each other, retain current column for future use.
			if (is_reo)
				*is_reo = 1;
			// for SQLite (constraint reference each other)
			if (column->type == SQ_TYPE_CONSTRAINT)
				table->bit_field |= SQB_TABLE_REO_CONSTRAINT;
			continue;
		}
		else {
			int  ref_each_other = 0;

			table->offset += count_table_order(schema, fore_table, &ref_each_other);
			// if tables reference each other, retain current column for future use.
			if (ref_each_other == 1) {
				if (is_reo)
					*is_reo = 1;
				// for SQLite (constraint reference each other)
				if (column->type == SQ_TYPE_CONSTRAINT)
					table->bit_field |= SQB_TABLE_REO_CONSTRAINT;
				continue;
			}
			// if tables doesn't reference each other, remove current column from relation.
			// if related list of object is empty, it will be removed by sq_relation_remove_empty()
			sq_relation_erase(table->relation, SQ_TYPE_TRACING, column, 0, NULL);
		}
	}

	table->bit_field &= ~SQB_TABLE_REO_CHECKING;
	return table->offset;
}

// used by sq_schema_arrange()
static int  sq_entry_cmp_offset(SqEntry** entry1, SqEntry** entry2)
{
	return (*entry1)->offset - (*entry2)->offset;
}

void    sq_schema_arrange(SqSchema* schema, SqPtrArray* entries)
{
	SqPtrArray* tables = sq_type_get_ptr_array(schema->type);
	SqTable*    table_one;

	// copy table pointers from schema->type->entry
	if (entries->length < tables->length)
		sq_ptr_array_alloc(entries, tables->length - entries->length);
	memcpy(entries->data, tables->data, sizeof(void*) * tables->length);
	entries->length = tables->length;

	// sort 'tables' (schema->type->entry) by table->name before calling count_table_order()
	sq_ptr_array_sort(tables, (SqCompareFunc)sq_entry_cmp_name);
	((SqType*)schema->type)->bit_field |= SQB_TYPE_SORTED;

	// reset all table->offset before calling count_table_order()
//	for (int index = 0;  index < tables->length;  index++)
//		((SqTable*)tables->data[index])->offset = 0;

	// count order number and set it in table->offset
	for (int index = 0;  index < tables->length;  index++) {
		table_one = (SqTable*)tables->data[index];
		count_table_order(schema, table_one, NULL);
		sq_relation_remove_empty(table_one->relation);
	}

	// sort entries by table->offset
	sq_ptr_array_sort(entries, (SqCompareFunc)sq_entry_cmp_offset);
}

void    sq_schema_complete(SqSchema* schema, bool no_need_to_sync)
{
	SqPtrArray* entries = sq_type_get_ptr_array(schema->type);
	SqTable*    table;

	for (int index = 0;  index < entries->length;  index++) {
		table = entries->data[index];
//		table->offset = 0;
		sq_table_complete(table, no_need_to_sync);
	}

	if (no_need_to_sync) {
		sq_relation_free(schema->relation);
		sq_relation_pool_destroy(schema->relation_pool);
	}
}
