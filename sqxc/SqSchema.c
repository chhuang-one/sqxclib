/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <SqConfig.h>
#include <SqError.h>
#include <SqBuffer.h>
#include <SqConvert.h>
#include <SqType.h>
#include <SqSchema.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

#define SCHEMA_INITIAL_VERSION       1
#define SQL_STRING_LENGTH_DEFAULT    SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT
#define SQ_TYPE_N_ENTRY_DEFAULT      SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

void  sq_schema_init(SqSchema *schema, const char *name)
{
	static int cur_version = SCHEMA_INITIAL_VERSION;

	// generate version number by incrementing 'cur_version'
	sq_schema_init_ver(schema, cur_version++, name);
}

void  sq_schema_init_ver(SqSchema *schema, int version, const char *name)
{
	SqType    *table_type;

	table_type = sq_type_new(SQ_TYPE_N_ENTRY_DEFAULT, (SqDestroyFunc)sq_table_free);
	table_type->parse = NULL;
	table_type->write = NULL;

	sq_entry_init((SqEntry*)schema, table_type);
	schema->name = name ? strdup(name) : NULL;
	schema->version = version;
	// callback for derived Sqdb
	schema->on_destory = NULL;
	// for (SQLite) migration.
	schema->relation = NULL;
	schema->relation_pool = NULL;
}

void  sq_schema_final(SqSchema *schema)
{
	sq_type_free((SqType*)schema->type);
	// finalize parent struct - SqEntry
	sq_entry_final((SqEntry*)schema);

	// call on_destory() callback to free 'relation' and 'relation-pool' for SQLite
	if (schema->on_destory)
		schema->on_destory(schema);
}

SqSchema *sq_schema_new(const char *name)
{
	SqSchema *schema;

	schema = malloc(sizeof(SqSchema));
	sq_schema_init(schema, name);
	return schema;
}

SqSchema *sq_schema_new_ver(int version, const char *name)
{
	SqSchema *schema;

	schema = malloc(sizeof(SqSchema));
	sq_schema_init_ver(schema, version, name);
	return schema;
}

void  sq_schema_free(SqSchema *schema)
{
	sq_schema_final(schema);
	free(schema);
}

SqTable *sq_schema_create_full(SqSchema     *schema,
                               const char   *table_name,
                               const char   *type_name,
                               const SqType *table_type,
                               size_t        instance_size)
{
	SqTable  *table;

	table = sq_table_new(table_name, table_type);
	// if 'table_type' is NULL,
	// table->type is dynamic type and table->bit_field has SQB_TYPE_DYNAMIC
	if (table_type == NULL) {
		((SqType*)table->type)->size = (unsigned int)instance_size;
		if (type_name)
			((SqType*)table->type)->name = strdup(type_name);
	}

	// add table in schema->type
	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_SCHEMA_CHANGED;
	return table;
}

SqTable *sq_schema_alter(SqSchema *schema, const char *name, const SqType *table_type)
{
	SqTable  *table;

	table = sq_table_new(name, table_type);
	table->bit_field  |= SQB_TABLE_CHANGED;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_SCHEMA_CHANGED;
	return table;
}

void sq_schema_drop(SqSchema *schema, const char *name)
{
	SqTable *table;

	table = calloc(1, sizeof(SqTable));
	table->old_name = strdup(name);
	table->name = NULL;
	table->bit_field = SQB_DYNAMIC;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_SCHEMA_CHANGED;

#if 0
	// remove table in schema->type
	addr = sq_type_find_entry(schema->type, name, NULL);
	if (addr)
		sq_type_erase_entry_addr(schema->type, addr, 1);
#endif
}

void sq_schema_rename(SqSchema *schema, const char *from, const char *to)
{
	SqTable *table;

	table = calloc(1, sizeof(SqTable));
	table->old_name = strdup(from);
	table->name = strdup(to);
	table->bit_field = SQB_DYNAMIC;

	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_SCHEMA_CHANGED;

#if 0
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

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqSchema.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

void  sq_schema_add(SqSchema *schema, SqTable *table)
{
	sq_type_add_entry((SqType*)schema->type, (SqEntry*)table, 1, 0);
	schema->bit_field |= SQB_SCHEMA_CHANGED;
}

SqTable *sq_schema_find(SqSchema *schema, const char *table_name)
{
	return (SqTable*)sq_entry_find((SqEntry*)schema, table_name, NULL);
}

#endif  // __STDC_VERSION
