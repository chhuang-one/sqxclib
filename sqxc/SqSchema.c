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

#define SCHEMA_INITIAL_VERSION       1
#define SQL_STRING_LENGTH_DEFAULT    SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT
#define SQ_TYPE_N_ENTRY_DEFAULT      SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

SqSchema*  sq_schema_new(const char* name)
{
	static int cur_version = SCHEMA_INITIAL_VERSION;
	SqSchema* schema;
	SqType* typeinfo;

	schema = malloc(sizeof(SqSchema));
	typeinfo = sq_type_new(SQ_TYPE_N_ENTRY_DEFAULT, (SqDestroyFunc)sq_table_free);
	typeinfo->parse = NULL;
	typeinfo->write = NULL;

	sq_entry_init((SqEntry*)schema, typeinfo);
	schema->name = name ? strdup(name) : NULL;
	// version count
	schema->version = cur_version++;
	return schema;
}

void  sq_schema_free(SqSchema* schema)
{
	sq_entry_final((SqEntry*)schema);
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
		table->type->size = instance_size;
		if (type_name)
			table->type->name = strdup(type_name);
	}

#ifdef SQ_CONFIG_NAMING_CONVENTION
	if (table->name == NULL)
		table->name = sq_name2table(table->type->name);
	if (table->type->name == NULL)
		table->type->name = sq_name2type(table->name);
#endif

	// add table in schema->type
	sq_type_insert_entry(schema->type, (SqEntry*)table);
	schema->bit_field |= SQB_CHANGED;
	return table;
}

SqTable* sq_schema_create_by_columns(SqSchema* schema,
                                     const char* table_name,
                                     const char* type_name,
                                     const SqColumn **columns,
                                     int n_columns)
{
	SqTable*    table;

	table = sq_schema_create_full(schema, table_name, type_name, NULL, 0);
	sq_ptr_array_append_n(sq_type_get_ptr_array(table->type), columns, n_columns);
	sq_type_decide_size(table->type, NULL);
	return table;
}

SqTable* sq_schema_alter(SqSchema* schema, const char* name, const SqType* type_info)
{
	SqTable*  table;

	table = sq_table_new(name, type_info);
	table->bit_field |= SQB_CHANGED;

	sq_type_insert_entry(schema->type, (SqEntry*)table);
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

	sq_type_insert_entry(schema->type, (SqEntry*)table);
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

	sq_type_insert_entry(schema->type, (SqEntry*)table);
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
	if ((schema->bit_field & SQB_SCHEMA_INCLUDED) == 0) {
		// get foreign keys before calling sq_schema_trace_foreign()
		for (index = 0;  index < reentries->length;  index++) {
			table = (SqTable*)reentries->data[index];
			if (table->foreigns.data == NULL)
				sq_ptr_array_init(&table->foreigns, 4, NULL);
			sq_table_get_foreigns(table, &table->foreigns);
		}
	}

	for (index = 0;  index < reentries_src->length;  index++) {
		table_src = (SqTable*)reentries_src->data[index];
		if (table_src->bit_field & SQB_CHANGED) {
			// === ALTER TABLE ===
			// find table if table->name == table_src->name
			addr = sq_reentries_find_name(reentries, table_src->name);
			if (addr) {
				table = *(SqTable**)addr;
				sq_table_include(table, table_src);
				// append changed table to tail
				*addr = NULL;
				sq_reentries_add(reentries, table);
				// It doesn't need to steal 'table_src' from 'schema_src'
				continue;
			}
			else {
				// It must steal 'table_src' from 'schema_src' if table not found.
				if (table_src->foreigns.data == NULL)
					sq_ptr_array_init(&table_src->foreigns, 4, NULL);
				sq_table_get_foreigns(table_src, &table_src->foreigns);
			}
		}
		else if (table_src->name == NULL) {
			// === DROP TABLE ===
			// erase original table if table->name == table_src->old_name
			addr = sq_reentries_find_name(reentries, table_src->old_name);
			if (addr) {
				table = *(SqTable**)addr;
				sq_table_free(table);
				*addr = NULL;
			}
		}
		else if (table_src->old_name) {
			// === RENAME TABLE ===
			// find existing if table->name == table_src->old_name
			addr = sq_reentries_find_name(reentries, table_src->old_name);
			if (addr) {
				// rename existing table->name to table_src->name
				table = *(SqTable**)addr;
#if 0
				if (table->old_name == NULL)
					table->old_name = table->name;
				else
#endif
					free(table->name);
				table->name = strdup(table_src->name);
				table->bit_field |= SQB_RENAMED;
			}
		}
		else {
			// === ADD TABLE ===
			if (table_src->foreigns.data == NULL)
				sq_ptr_array_init(&table_src->foreigns, 4, NULL);			
			sq_table_get_foreigns(table_src, &table_src->foreigns);
		}

		// steal 'table_src' from 'schema_src->type->entry'.
		reentries_src->data[index] = NULL;
		// add 'table_src' to schema->type->entry.
		sq_reentries_add(reentries, table_src);
	}

	// update other data in SqSchema
	schema->version = schema_src->version;
	schema->bit_field |= SQB_SCHEMA_INCLUDED;
	schema->type->bit_field &= ~SQB_TYPE_SORTED;

	return SQCODE_OK;
}

int     sq_schema_trace_foreign(SqSchema* schema)
{
	SqPtrArray *reentries = sq_type_get_ptr_array(schema->type);
	SqTable    *table, *table_fore;
	SqColumn   *column;
	SqReentry  *reentry;
	int         result = SQCODE_OK;

	for (int index = 0;  index < reentries->length;  index++) {
		table = (SqTable*)reentries->data[index];
		if (table == NULL || table->foreigns.data == NULL)
			continue;

		for (int i = 0;  i < table->foreigns.length;  i++) {
			column = (SqColumn*)table->foreigns.data[i];
			if (column == NULL)
				continue;

			// --------------------------------------------
			// trace renamed table
			reentry = (SqReentry*)sq_reentries_trace_renamed(reentries,
					column->foreign->table, schema->offset, false);
			if (reentry) {
				reentry = *(SqReentry**)reentry;
				if (reentry->name == NULL) {
					// table dropped.
					result = SQCODE_REENTRY_DROPPED;
					continue;   // error...
				}
				// reference table was renamed.
				if ((column->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic column to replace static one
					column = sq_column_copy_static(column);
					sq_table_replace_column(table, NULL,
							(SqColumn**)&table->foreigns.data[i], column);
				}
				free(column->foreign->table);
				column->foreign->table = strdup(reentry->name);    // name = the newest table name
			}

			// --------------------------------------------
			// find referenced table
			table_fore = (SqTable*)sq_reentries_find_name(reentries,
					column->foreign->table);
			if (table_fore)
				table_fore = *(SqTable**)table_fore;
			else {
				// (renamed) table not found. dropped?
				result = SQCODE_REFERENCE_NOT_FOUND;
				continue;  // error...
			}

			// --------------------------------------------
			// trace renamed column
			reentry = (SqReentry*)sq_reentries_trace_renamed(&table_fore->type->entry,
					column->foreign->column, table_fore->offset, false);
			if (reentry) {
				reentry = *(SqReentry**)reentry;
				if (reentry->name == NULL) {
					// column dropped.
					result = SQCODE_REENTRY_DROPPED;
					continue;   // error...
				}
				// reference column renamed.
				if ((column->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic column to replace static one
					column = sq_column_copy_static(column);
					sq_table_replace_column(table, NULL,
							(SqColumn**)&table->foreigns.data[i], column);
				}
				free(column->foreign->column);
				column->foreign->column = strdup(reentry->name);    // name = newest column name
			}
		}
	}
	return result;
}

void  sq_schema_clear_records(SqSchema* schema, char ver_comparison)
{
	SqPtrArray* reentries;
	SqTable* table;
	int      index;

	// erase changed records and remove NULL records in schema
	reentries = sq_type_get_ptr_array(schema->type);    // schema->type->entry
	sq_reentries_clear_records(reentries, ver_comparison);
	sq_reentries_remove_null(reentries);
	// set schema->offset for sq_schema_trace_foreign() or sq_schema_arrange()
	// if database schema version < current schema version, reset schema->offset for sq_schema_arrange()
	schema->offset = (ver_comparison == '<') ? 0 : reentries->length;
	// clear SQB_CHANGED and SQB_RENAMED
	schema->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);

	// erase changed records and remove NULL records in tables
	for (index = 0;  index < reentries->length;  index++) {
		table = (SqTable*)reentries->data[index];
		reentries = sq_type_get_ptr_array(table->type);    // table->type->entry
		//  table->type maybe static
		if (table->type->bit_field & SQB_TYPE_DYNAMIC) {
			sq_reentries_clear_records(reentries, ver_comparison);
			sq_reentries_remove_null(reentries);
		}
		// if database schema version < current schema version, reset table->offset for sq_schema_arrange()
		table->offset = (ver_comparison == '<') ? 0 : reentries->length;
		// clear SQB_CHANGED and SQB_RENAMED
		table->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);
		// if database schema version == current schema version
		if (ver_comparison == '=') {
			table->bit_field |= SQB_TABLE_SQL_CREATED;
			table->bit_field &= ~(SQB_TABLE_COL_CHANGED);
		}
	}
}

// used by sq_schema_arrange()
// trace column reference and count order
// before calling this function:
// 1. sort schema->type->entry by table->name
// 2. sort table->extra->foreigns by column->foreign->table
// 3. reset all table->offset = 0
static int  count_table_order(SqSchema* schema, SqTable* table, int* is_reo)
{
	SqPtrArray* foreigns;
	SqColumn*   column;
	SqTable*    fore_table;

	if (table->offset > 0)
		return table->offset;
	table->offset = 1;
	table->bit_field |= SQB_TABLE_REO_CHECKING;
	foreigns = &table->foreigns;

	for (int index = 0;  index < foreigns->length;  index++) {
 		column = (SqColumn*)foreigns->data[index];
		if (column == NULL)
			continue;

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
			if (column->constraint)
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
				if (column->constraint)
					table->bit_field |= SQB_TABLE_REO_CONSTRAINT;
				continue;
			}
			// if tables doesn't reference each other, remove current column from 'foreigns'.
			// these NULL will be removed by sq_reentries_remove_null()
			foreigns->data[index] = NULL;
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
	schema->type->bit_field |= SQB_TYPE_SORTED;

	// reset all table->offset before calling count_table_order()
//	for (int index = 0;  index < tables->length;  index++)
//		((SqTable*)tables->data[index])->offset = 0;

	// count order number and set it in table->offset
	for (int index = 0;  index < tables->length;  index++) {
		table_one = (SqTable*)tables->data[index];
		count_table_order(schema, table_one, NULL);
		sq_reentries_remove_null(&table_one->foreigns);
	}

	// sort entries by table->offset
	sq_ptr_array_sort(entries, (SqCompareFunc)sq_entry_cmp_offset);
}

void    sq_schema_complete(SqSchema* schema)
{
	SqPtrArray* entries = sq_type_get_ptr_array(schema->type);
	SqTable*    table;

	for (int index = 0;  index < entries->length;  index++) {
		table = entries->data[index];
//		table->offset = 0;
		sq_ptr_array_final(&table->foreigns);
	}

	schema->bit_field &= ~SQB_SCHEMA_INCLUDED;
}
