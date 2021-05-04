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
	// free relation after sq_entry_final()
	if (schema->relation) {
		sq_relation_free(schema->relation);
		sq_relation_pool_destroy(schema->relation_pool);
	}
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
		cmp_func = (SqCompareFunc)sq_entry_cmp_str__name;
	else
		cmp_func = NULL;

	addr = sq_type_find_entry(schema->type, name, cmp_func);
	if (addr)
		return *addr;
	else
		return NULL;
}
