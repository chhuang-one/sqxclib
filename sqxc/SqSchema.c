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
	sq_field_init((SqField*)schema, NULL);
	schema->name = name ? strdup(name) : NULL;
	// table_type_map
	sq_ptr_array_init(&schema->table_type_map, 8, NULL);
	schema->table_type_sorted = false;
	return schema;
}

SqSchema*  sq_schema_final(SqSchema* schema)
{
	sq_field_final((SqField*)schema);
	sq_ptr_array_final(&schema->table_type_map);
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
	sq_ptr_array_append(&schema->table_type_map, table);
	schema->table_type_sorted = false;
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
	// table_type_map
	addr = sq_ptr_array_find(&schema->table_type_map, name,
			                 (SqCompareFunc)sq_field_cmp_str__name);
	sq_ptr_array_erase(&schema->table_type_map, addr - schema->table_type_map.data, 1);
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
	SqPtrArray* array = &schema->table_type_map;
	SqTable**   table;

	if (schema->table_type_map.data == NULL)
		return NULL;
	if (schema->table_type_sorted == false) {
		schema->table_type_sorted = true;
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

	// schema->table_type_map
	array = &schema->table_type_map;
	for (index = 0;  index < array->length;  index++) {
		if (array->data[index] == table_old) {
			array->data[index]  = table_new;
			break;
		}
	}
	if (index == array->length) {
		sq_ptr_array_append(array, table_new);
		schema->table_type_sorted = false;
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

int   sq_schema_accumulate(SqSchema* schema, SqSchema* schema_src)
{	//      *schema, *schema_src
	SqTable *table,  *table_src, *table_new;
	SqType  *type,   *type_src;
	int      index,   index_src,  count;

	type = schema->type;
	type_src = schema_src->type;

	for (index_src = 0;  index_src < type_src->map_length;  index_src++) {
		table_src = (SqTable*)type_src->map[index_src];
		if (table_src->bit_field & SQB_CHANGE) {
			// ALTER TABLE
			// accumulate if table->name == table_src->name
			table = (SqTable*)sq_type_find_field(type, table_src->name,
			                           (SqCompareFunc)sq_field_cmp_str__name);
			if (table == NULL) {
				sq_type_insert_field(type, (SqField*)table_src);
				// table_type_map
				sq_ptr_array_append(&schema->table_type_map, table_src);
				schema->table_type_sorted = false;
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
		}
		else if (table_src->name == NULL) {
			// DROP TABLE
			// remove if table->old_name == table_src->old_name
			sq_type_erase_field(type, table_src->old_name,
			                    (SqCompareFunc)sq_table_cmp_str__old_name);
			// remove if table->name == table_src->old_name
			sq_type_erase_field(type, table_src->old_name, NULL);
			// steal table_src if table_src is not static.
			if (type_src->bit_field & SQB_DYNAMIC)
				type_src->map[index_src] = NULL;
			// insert table_src to schema
			sq_type_insert_field(type, (SqField*)table_src);
		}
		else if (table_src->old_name) {
			// RENAME TABLE
			count = 0;
			// find existing if table->name == table_src->old_name
			for (index = 0;  index < type->map_length;  index++) {
				table = (SqTable*)type->map[index];
				// skip "drop only" table
				if (table->name == NULL)
					continue;
				// skip if table->name != table_src->old_name
				if (strcasecmp(table->name, table_src->old_name) != 0)
					continue;
				// rename existing table->name to table_src->name
				if (table->old_name)   // this is "rename only" table
					count++;
				else if ((table->bit_field & SQB_DYNAMIC) == 0) {
					table_new = sq_table_copy_static(table);
					type->map[index] = (SqField*)table_new;
					sq_schema_replace_table_type(schema, table, table_new);
					table = table_new;
				}
				free(table->name);
				table->name = strdup(table_src->name);
			}

			if (table_src->bit_field & SQB_DYNAMIC) {
				// steal table_src if table_src is not static.
				if (type_src->bit_field & SQB_DYNAMIC)
					type_src->map[index_src] = NULL;
				// insert table_src to table
				sq_type_insert_field(type, (SqField*)table_src);
			}
			else {
				// create dynamic "rename only" table to replace static one
				table_new = calloc(1, sizeof(SqColumn));
				table_new->name = strdup(table_src->name);
				table_new->old_name = strdup(table->old_name);
				table_new->bit_field = table->bit_field | SQB_DYNAMIC;
				if (count > 0)
					table_new->bit_field |= SQB_IGNORE;
				sq_type_insert_field(type, (SqField*)table_new);
			}
		}
		else {
			// ADD TABLE
			// steal table_src if table_src is not static.
			if (type_src->bit_field & SQB_DYNAMIC)
				type_src->map[index_src] = NULL;
			// insert table_src to schema
			sq_type_insert_field(type, (SqField*)table_src);
		}
	}

	// remove NULL table (it was stolen) if table_src is not static.
	sq_ptr_array_remove_null(sq_type_get_array(type_src));
	return SQCODE_OK;
}

