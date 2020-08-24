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

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqBuffer.h>
#include <SqUtil.h>
#include <SqSchema.h>


#define DEFAULT_STRING_LENGTH    191

SqSchema*  sq_schema_new(const char* name)
{
	static int cur_version = 0;
	SqSchema* schema;
	SqType* typeinfo;

	schema = malloc(sizeof(SqSchema));
	typeinfo = sq_type_new(8, (SqDestroyFunc)sq_table_free);

	sq_entry_init((SqEntry*)schema, typeinfo);
	schema->name = name ? strdup(name) : NULL;
	// table_types
	sq_ptr_array_init(&schema->table_types, 8, NULL);
	schema->table_types_sorted = false;
	//
	schema->version = cur_version++;
	return schema;
}

void  sq_schema_free(SqSchema* schema)
{
	sq_entry_final((SqEntry*)schema);
	sq_ptr_array_final(&schema->table_types);
	free(schema);
}

SqTable* sq_schema_create_full(SqSchema* schema,
                               const char* table_name,
                               const SqType* type_info,
                               const char* type_name,
                               size_t instance_size)
{
	SqTable*  table;

	table = sq_table_new(table_name, type_info);
	// if type_info == NULL,
	// table->type is dynamic type and table->bit_field has SQB_TYPE_DYNAMIC
	if (type_info == NULL) {
		table->type->size = instance_size;
		if (type_name)
			table->type->name = strdup(type_name);
	}

#ifdef SQ_HAVE_NAMING_CONVENTION
	if (table->name == NULL)
		table->name = sq_name2table(table->type->name);
	if (table->type->name == NULL)
		table->type->name = sq_name2type(table->name);
#endif

	// add table in table_types
	sq_ptr_array_append(&schema->table_types, table);
	schema->table_types_sorted = false;
	// add table in schema->type
	sq_type_insert_entry(schema->type, (SqEntry*)table);
	schema->bit_field |= SQB_CHANGE;
	return table;
}

SqTable* sq_schema_alter(SqSchema* schema, const char* name, const SqType* type_info)
{
	SqTable*  table;

	table = sq_table_new(name, type_info);
	table->bit_field |= SQB_CHANGE;

	sq_type_insert_entry(schema->type, (SqEntry*)table);
	schema->bit_field |= SQB_CHANGE;
	return table;
}

void sq_schema_drop(SqSchema* schema, const char* name)
{
	SqTable* table;

	table = calloc(1, sizeof(SqTable));
	table->old_name = strdup(name);
	table->name = NULL;
	table->bit_field = SQB_DYNAMIC;

	sq_type_insert_entry(schema->type, (SqEntry*)table);
	schema->bit_field |= SQB_CHANGE;

#if 0
	// remove table in table_types
	void** addr = sq_ptr_array_find(&schema->table_types, name, (SqCompareFunc)sq_entry_cmp_str__type_name);
	if (addr)
		sq_ptr_array_erase_addr(&schema->table_types, addr, 1);
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

	sq_type_insert_entry(schema->type, (SqEntry*)table);
	schema->bit_field |= SQB_CHANGE;

#if 0
//	table = (SqTable*)sq_type_find_entry(schema->type, from,
//	                                     (SqCompareFunc)sq_entry_cmp_str__name);
	table = (SqTable*)sq_type_find_entry(schema->type, from, NULL);
	if (table) {
		table = *(SqTable**)table;
		free(table->name);
		table->name = strdup(to);
	}
#endif
}

SqTable* sq_schema_find(SqSchema* schema, const char* name)
{
	SqCompareFunc cmp_func;
	void** addr;

	// if cmp_func == NULL, sq_type_find_entry() will sort entry before finding.
	if (schema->bit_field & SQB_CHANGE)
		cmp_func = (SqCompareFunc)sq_reentry_cmp_str__name;
	else
		cmp_func = NULL;

	addr = sq_type_find_entry(schema->type, name, cmp_func);
	if (addr)
		return *addr;
	else
		return NULL;
}

SqTable* sq_schema_find_type(SqSchema* schema, const char* name)
{
	SqPtrArray* array = &schema->table_types;
	SqTable**   table;

	if (schema->table_types.data == NULL)
		return NULL;
	// sort
	if (schema->table_types_sorted == false) {
		schema->table_types_sorted = true;
		sq_ptr_array_sort(array, sq_entry_cmp_type_name);
	}
	// search
	table = sq_ptr_array_search(array, name,
			(SqCompareFunc)sq_entry_cmp_str__type_name);
	if (table)
		return *table;
	return NULL;
}

int   sq_schema_accumulate(SqSchema* schema, SqSchema* schema_src)
{	//      *schema, *schema_src
	SqTable *table,  *table_src;
	SqType  *type,   *type_src;
	// other variable
	int      index;
	void   **addr;

	type = schema->type;
	type_src = schema_src->type;

	// run sq_schema_accumulate() first time for sq_schema_trace_foreign()
	if (schema->offset == 0) {
		for (index = 0;  index < type->n_entry;  index++) {
			table = (SqTable*)type->entry[index];
			if (table->tempcols.data == NULL)
				sq_ptr_array_init(&table->tempcols, 4, NULL);
			sq_table_get_foreigns(table, &table->tempcols);
		}
	}

	for (index = 0;  index < type_src->n_entry;  index++) {
		table_src = (SqTable*)type_src->entry[index];
		if (table_src->bit_field & SQB_CHANGE) {
			// === ALTER TABLE ===
			// find table if table->name == table_src->name
			addr = sq_reentries_find_name(&type->entry, table_src->name);
			if (addr) {
				table = *(SqTable**)addr;
#ifdef SQ_SUPPORT_STATIC_TABLE
				if ((table->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic table to replace static one
					table = sq_table_copy_static(table);
					// replace table pointer in schema->table_types
					sq_reentries_replace(&schema->table_types, *addr, table);
					schema->table_types_sorted = false;
					// replace table pointer in schema->type->entry
					*addr = table;
				}
#endif   // SQ_SUPPORT_STATIC_TABLE
				if (sq_table_accumulate(table, table_src) != SQCODE_OK)
					return SQCODE_STATIC_DATA;
				// It doesn't need to steal 'table_src' from 'schema_src'
				continue;
			}
			else {
				// If original table not found, add it.
				sq_reentries_add(&type->entry, table_src);
				// table_types
				sq_ptr_array_append(&schema->table_types, table_src);
				schema->table_types_sorted = false;
			}
		}
		else if (table_src->name == NULL) {
			// === DROP TABLE ===
			// erase original table if table->name == table_src->old_name
			sq_reentries_erase_name(&type->entry, table_src->old_name);
			// append "dropped record"
			sq_reentries_add(&type->entry, table_src);
		}
		else if (table_src->old_name) {
			// === RENAME TABLE ===
			// find existing if table->name == table_src->old_name
			addr = sq_reentries_find_name(&type->entry, table_src->old_name);
			if (addr) {
				// rename existing table->name to table_src->name
				table = *(SqTable**)addr;
#ifdef SQ_SUPPORT_STATIC_TABLE
				if ((table->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic table to replace static table
					table = sq_table_copy_static(table);
					// replace table pointer in schema->table_types
					sq_reentries_replace(&schema->table_types, *addr, table);
//					schema->table_types_sorted = false;
					// replace table pointer in schema->type->entry
					*addr = table;
				}
#endif   // SQ_SUPPORT_STATIC_TABLE
				free(table->name);
				table->name = strdup(table_src->name);
			}
			// insert 'table_src' to table
			sq_reentries_add(&type->entry, table_src);
		}
		else {
			// === ADD TABLE ===
			// insert 'table_src' to schema
			sq_reentries_add(&type->entry, table_src);
			// table_types
			sq_ptr_array_append(&schema->table_types, table_src);
			schema->table_types_sorted = false;
		}

		// steal 'table_src' if 'type_src' is not static.
		if (type_src->bit_field & SQB_TYPE_DYNAMIC)
			type_src->entry[index] = NULL;
	}

	// trace dropped or renamed table/column
	sq_schema_trace_foreign(schema);

	// erase changes and remove NULL table in schema (schema->type->entry)
	sq_reentries_erase_changes(&type->entry);
	sq_reentries_remove_null(&type->entry);
	schema->offset = type->n_entry;    // update 'offset' for sq_schema_trace_foreign()
	// update other data in SqSchema
	schema->version = schema_src->version;
	type->bit_field &= ~SQB_TYPE_SORTED;

	// erase changes and remove NULL column in tables (table->type->entry)
	for (index = 0;  index < type->n_entry;  index++) {
		table = (SqTable*)type->entry[index];
		sq_reentries_erase_changes(&table->type->entry);
		sq_reentries_remove_null(&table->type->entry);
		table->offset = table->type->n_entry;	// update 'offset' for sq_schema_trace_foreign()
		// update other data in SqTable
		table->bit_field &= ~SQB_CHANGE;        // table has completed changes
	}

	return SQCODE_OK;
}

int     sq_schema_trace_foreign(SqSchema* schema)
{
	SqType     *schema_type = schema->type;
	SqTable    *table, *table_tmp;
	SqColumn   *column;
	const char *name;
	int         result = SQCODE_OK;

	sq_ptr_array_foreach(&schema_type->entry, element) {
		table = (SqTable*)element;
		if (table == NULL || table->tempcols.data == NULL)
			continue;

		sq_ptr_array_foreach(&table->tempcols, element) {
			column = (SqColumn*)element;
			// trace renamed table
			name = sq_reentries_trace_renamed(&schema_type->entry,
					column->foreign->table, schema->offset);
			if (name == NULL) {
				// table dropped.
				result = SQCODE_REENTRY_DROPPED;
				continue;   // error...
			}
			else if (name != column->foreign->table) {
				// table renamed. name = the newest table name
				free(column->foreign->table);
				column->foreign->table = strdup(name);
			}
			// find referenced table
			table_tmp = (SqTable*)sq_reentries_find_name(&schema_type->entry,
					column->foreign->table);
			if (table_tmp == NULL) {
				// table not found. dropped?
				result = SQCODE_ENTRY_NOT_FOUND;
				continue;  // error...
			}
			// trace renamed column
			name = sq_reentries_trace_renamed(&table_tmp->type->entry,
					column->foreign->column, table_tmp->offset);
			if (name == NULL) {
				// column dropped.
				result = SQCODE_REENTRY_DROPPED;
				continue;   // error...
			}
			else if (name != column->foreign->column) {
				// column renamed. name = newest column name
				free(column->foreign->column);
				column->foreign->column = strdup(name);
			}
		}
	}
	return result;
}
