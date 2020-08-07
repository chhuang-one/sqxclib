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

SqSchema*  sq_schema_init(SqSchema* schema, const char* name)
{
	static int cur_ver = 0;

	sq_field_init((SqField*)schema, NULL);
	schema->name = name ? strdup(name) : NULL;
	// table_types
	sq_ptr_array_init(&schema->table_types, 8, NULL);
	schema->table_types_sorted = false;
	schema->version = cur_ver++;
	return schema;
}

SqSchema*  sq_schema_final(SqSchema* schema)
{
	sq_field_final((SqField*)schema);
	sq_ptr_array_final(&schema->table_types);
	return schema;
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
	// table->type is dynamic type and table->bit_field has SQB_DYNAMIC_TYPE
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

	sq_type_insert_field(schema->type, (SqField*)table);
	sq_ptr_array_append(&schema->table_types, table);
	schema->table_types_sorted = false;
	return table;
}

SqTable* sq_schema_alter(SqSchema* schema, const char* name, const SqType* type_info)
{
	SqTable*  table;

	table = sq_table_new(name, type_info);
	table->bit_field |= SQB_CHANGE;
	sq_type_insert_field(schema->type, (SqField*)table);
	return table;
}

void sq_schema_drop(SqSchema* schema, const char* name)
{
	SqTable* table;
	void**   addr;

	if (schema->bit_field & SQB_CHANGE) {
		table = calloc(1, sizeof(SqTable));
		table->old_name = strdup(name);
		table->name = NULL;
		table->bit_field = SQB_DYNAMIC;
		sq_type_insert_field(schema->type, (SqField*)table);
		return;
	}
	sq_type_erase_field(schema->type, name, NULL);
	// table_types
	addr = sq_ptr_array_find(&schema->table_types, name,
			                 (SqCompareFunc)sq_field_cmp_str__name);
	sq_ptr_array_erase(&schema->table_types, addr - schema->table_types.data, 1);
}

void sq_schema_rename(SqSchema* schema, const char* from, const char* to)
{
	SqTable* table;

	if (schema->bit_field & SQB_CHANGE) {
		table = calloc(1, sizeof(SqTable));
		table->old_name = strdup(from);
		table->name = strdup(to);
		table->bit_field = SQB_DYNAMIC;
		sq_type_insert_field(schema->type, (SqField*)table);
		return;
	}

//	table = (SqTable*)sq_type_find_field(schema->type, from,
//	                                     (SqCompareFunc)sq_field_cmp_str__name);
	table = (SqTable*)sq_type_find_field(schema->type, from, NULL);
	if (table) {
		free(table->name);
		table->name = strdup(to);
	}
}

SqTable* sq_schema_find(SqSchema* schema, const char* name)
{
	return (SqTable*)sq_type_find_field(schema->type, name, NULL);
}
 
SqTable* sq_schema_find_type(SqSchema* schema, const char* name)
{
	SqPtrArray* array = &schema->table_types;
	SqTable**   table;

	if (schema->table_types.data == NULL)
		return NULL;
	if (schema->table_types_sorted == false) {
		schema->table_types_sorted = true;
		sq_ptr_array_sort(array, sq_field_cmp_type_name);
	}
	table = sq_ptr_array_search(array, name,
			(SqCompareFunc)sq_field_cmp_str__type_name);
	if (table)
		return *table;
	return NULL;
}

static SqTable* sq_schema_replace_table_type(SqSchema* schema, SqTable* table_old, SqTable* table_new)
{
	SqPtrArray* array;
	int  index;

	// schema->table_types
	array = &schema->table_types;
	for (index = 0;  index < array->length;  index++) {
		if (array->data[index] == table_old) {
			array->data[index]  = table_new;
			break;
		}
	}
	if (index == array->length) {
		sq_ptr_array_append(array, table_new);
		schema->table_types_sorted = false;
	}
	return table_new;
}

static SqTable* sq_schema_replace_table(SqSchema* schema, SqTable* table_old, SqTable* table_new)
{
	SqPtrArray* array;
	int  index;

	// schema->type
	array = sq_type_get_array(schema->type);
	for (index = 0;  index < array->length;  index++) {
		if (array->data[index] == table_old) {
			array->data[index]  = table_new;
			break;
		}
	}
	if (index == array->length)
		sq_type_insert_field(schema->type, (SqField*)table_new);

	return table_new;
}

static int sq_schema_find_or_replace(SqSchema* schema, const char* table_name, SqTable* table_to_replace)
{
	int       index;
	SqType*   type = schema->type;
	SqTable*  table;

	for (index = 0;  index < type->n_entry;  index++) {
		table = (SqTable*)type->entry[index];
		// skip "dropped record" or "renamed record"
		if (table->old_name)
			continue;
		// erase if table->name == table_name
		if (strcasecmp(table->name, table_name) == 0) {
			if (table_to_replace)
				sq_ptr_array_erase(sq_type_get_array(type), index, 1);
			break;
		}
	}
	// insert column to table
	if (table_to_replace)
		sq_type_insert_field(type, (SqField*)table_to_replace);

	if (index < type->n_entry)
		return index;
	else
		return -1;
}

int   sq_schema_accumulate(SqSchema* schema, SqSchema* schema_src)
{	//      *schema, *schema_src
	SqTable *table,  *table_src, *table_new;
	SqType  *type,   *type_src;
	int      index,   index_src;

	type = schema->type;
	type_src = schema_src->type;

	for (index_src = 0;  index_src < type_src->n_entry;  index_src++) {
		table_src = (SqTable*)type_src->entry[index_src];
		if (table_src->bit_field & SQB_CHANGE) {
			// === ALTER TABLE ===
			// find table if table->name == table_src->name
			index = sq_schema_find_or_replace(schema, table_src->name, NULL);
			// old table not found
			if (index == -1) {
				sq_type_insert_field(type, (SqField*)table_src);
				// table_types
				sq_ptr_array_append(&schema->table_types, table_src);
				schema->table_types_sorted = false;
				continue;
			}

			if ((table->bit_field & SQB_DYNAMIC) == 0) {
				// create dynamic table to replace static one
				table_new = sq_table_copy_static(table);
				sq_schema_replace_table(schema, table, table_new);
				sq_schema_replace_table_type(schema, table, table_new);
				sq_table_free(table);
				table = table_new;
			}
			if (sq_table_accumulate(table, table_src) != SQCODE_OK)
				return SQCODE_STATIC_DATA;
			// "altered record" doesn't need to steal table_src from schema_src
			continue;
		}
		else if (table_src->name == NULL) {
			// === DROP TABLE ===
			// replace table if table->name == table_src->old_name
			// delete original table and append "dropped record"
			sq_schema_find_or_replace(schema, table_src->old_name, table_src);
		}
		else if (table_src->old_name) {
			// === RENAME TABLE ===
			// find existing if table->name == table_src->old_name
			index = sq_schema_find_or_replace(schema, table_src->old_name, NULL);
			if (index != -1) {
				// rename existing table->name to table_src->name
				table = (SqTable*)type->entry[index];
				if ((table->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic table to replace static table
					table_new = sq_table_copy_static(table);
					type->entry[index] = (SqField*)table_new;
					sq_schema_replace_table_type(schema, table, table_new);
					table = table_new;
				}
				free(table->name);
				table->name = strdup(table_src->name);
			}
			// insert table_src to table
			sq_type_insert_field(type, (SqField*)table_src);
		}
		else {
			// === ADD TABLE ===
			// insert table_src to schema
			sq_type_insert_field(type, (SqField*)table_src);
		}

		// steal table_src if type_src is not static.
		if (type_src->bit_field & SQB_DYNAMIC)
			type_src->entry[index_src] = NULL;
	}

	// remove NULL table (it was stolen) if table_src is not static.
	sq_ptr_array_remove_null(sq_type_get_array(type_src));
	return SQCODE_OK;
}
