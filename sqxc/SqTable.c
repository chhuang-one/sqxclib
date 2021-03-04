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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <SqTable.h>

SqTable* sq_table_new(const char* name, const SqType* typeinfo)
{
	SqTable* table;

	table = malloc(sizeof(SqTable));
	// create dynamic SqType
	if (typeinfo == NULL)
		typeinfo = sq_type_new(8, (SqDestroyFunc)sq_column_free);
	sq_entry_init((SqEntry*)table, typeinfo);
	table->name = (name) ? strdup(name) : NULL;
	table->bit_field |= SQB_POINTER;
	table->old_name = NULL;
	table->foreigns = (SqPtrArray){NULL, 0};

	return (SqTable*)table;
}

void  sq_table_free(SqTable* table)
{
	if (table->bit_field & SQB_DYNAMIC) {
		// reduce the stack frame:
		// sq_type_unref() will not be called by below sq_entry_final()
		if (table->type) {
			sq_type_unref((SqType*)table->type);
			table->type = NULL;
		}
		// finalize parent struct - SqEntry
		sq_entry_final((SqEntry*)table);
		free((char*)table->old_name);
		// free temporary data
		sq_ptr_array_final(&table->foreigns);
		// free SqTable
		free(table);
	}
}

bool  sq_table_has_column(SqTable* table, const char* column_name)
{
	SqCompareFunc cmp_func;

	if (table->bit_field & SQB_CHANGED)
		cmp_func = (SqCompareFunc)sq_reentry_cmp_str__name;
	else
		cmp_func = NULL;

	if (sq_type_find_entry(table->type, column_name, cmp_func))
		return true;
	return false;
}

void  sq_table_add_column(SqTable* table, const SqColumn* column, int n_column)
{
	SqType* type = (SqType*)table->type;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		type = sq_type_copy_static(type, (SqDestroyFunc)sq_column_free);
		table->type = type;
	}
	sq_type_add_entry(type, (SqEntry*)column, n_column, sizeof(SqColumn));
}

void  sq_table_add_column_ptrs(SqTable* table, const SqColumn** column_ptrs, int n_column_ptrs)
{
	SqType*  type = (SqType*)table->type;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		type = sq_type_copy_static(type, (SqDestroyFunc)sq_column_free);
		table->type = type;
	}
	sq_type_add_entry_ptrs(type, (const SqEntry**)column_ptrs, n_column_ptrs);
}

void  sq_table_drop_column(SqTable* table, const char* column_name)
{
	SqColumn*  column;

	column = calloc(1, sizeof(SqColumn));
	column->old_name = strdup(column_name);

	sq_table_add_column(table, column, 1);
	table->bit_field |= SQB_CHANGED;

#if 0
	column = (SqColumn*)sq_type_find_entry(table->type, column_name, NULL);
	if (column) {
		sq_table_replace_column(table, (SqColumn**)column, NULL, NULL);
		sq_type_steal_entry_addr(table->type, (void**)column, 1);
	}
#endif
}

void  sq_table_rename_column(SqTable* table, const char* from, const char* to)
{
	SqColumn*  column;

	column = calloc(1, sizeof(SqColumn));
	column->old_name = strdup(from);
	column->name = strdup(to);
	column->bit_field = SQB_DYNAMIC;

	sq_table_add_column(table, column, 1);
	table->bit_field |= SQB_CHANGED;

#if 0
	column = (SqColumn*)sq_type_find_entry(table->type, from, NULL);
	if (column) {
		column = *(SqColumn**)column;
		if (column->old_name == NULL)
			column->old_name = column->name;
		else
			free(column->name);
		column->name = strdup(to);
		column->bit_field |= SQB_RENAMED;
		table->type->bit_field &= ~SQB_TYPE_SORTED;
	}
#endif
}

int  sq_table_get_columns(SqTable* table, SqPtrArray* ptrarray,
                          const SqType* type, unsigned int bit_field)
{
	SqPtrArray*  colarray;
	SqColumn*    column;
	int          count = 0;
	int          matched_count;
	unsigned int bit_field_orig = bit_field;

	if (ptrarray && ptrarray->data == NULL)
		sq_ptr_array_init(ptrarray, 4, NULL);
	colarray = sq_type_get_ptr_array(table->type);
	// for each columns
	for (int index = 0;  index < colarray->length;  index++) {
		column = colarray->data[index];
		// reset variable
		matched_count = 0;
		bit_field = bit_field_orig;
		// check special case of foreign key
		if (column->foreign  &&  bit_field & SQB_FOREIGN)
			bit_field &= ~SQB_FOREIGN;
		// check 2 conditions
		if ((column->bit_field & bit_field) == bit_field)
			matched_count++;
		if (column->type == type || type == NULL)
			matched_count++;
		// if matched 2 conditions
		if (matched_count == 2) {
			if (ptrarray)
				sq_ptr_array_append(ptrarray, column);
			count++;
		}
	}
	return count;
}

SqColumn* sq_table_get_primary(SqTable* table)
{
	SqPtrArray* array;
	SqColumn*   column;

	array = sq_type_get_ptr_array(table->type);
	for (int index = 0;  index < array->length;  index++) {
		column = array->data[index];
		if (column->bit_field & SQB_PRIMARY && SQ_TYPE_IS_INT(column->type))
			return column;
	}
	return NULL;
}

SqColumn* sq_table_add_bool(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_BOOL);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_int(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_INT);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_uint(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_UINT);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_int64(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_INT64);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_uint64(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_UINT64);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_double(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_DOUBLE);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_timestamp(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_TIME);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_string(SqTable* table, const char* name, size_t offset, int length)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_STRING);
	column->offset = offset;
	column->size = length;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn* sq_table_add_custom(SqTable* table, const char* name,
                              size_t offset, const SqType* sqtype,
                              int  length)
{
	SqColumn* column;

	column = sq_column_new(name, sqtype);
	column->offset = offset;
	column->size = length;

	sq_table_add_column(table, column, 1);
	return column;
}

// --------------------------------------------------------
// SqTable C functions for CONSTRAINT

SqColumn* sq_table_add_composite(SqTable* table,
                                 SqType*  column_type,
                                 unsigned int bit_field,
                                 const char*  name)
{
	SqColumn* column;

	column = calloc(1, sizeof(SqColumn));
	column->type = column_type;
	column->bit_field = SQB_DYNAMIC | bit_field;
#if 1
	if (name)
		column->name = strdup(name);
#else
	if (name)
		column->name = strdup(name);
	else {
		const char* post_string;
		int  name_length;

		switch(bit_field) {
		case SQB_INDEX:
			post_string = "_index";
			name_length = 5;
			break;
		case SQB_UNIQUE:
			post_string = "_unique";
			name_length = 7;
			break;
		case SQB_PRIMARY:
			post_string = "_primary";
			name_length = 8;
			break;
		case SQB_FOREIGN:
			post_string = "_foreign";
			name_length = 8;
			break;
		default:
			post_string = "";
			name_length = 0;
			break;
		}
		column->name = malloc(strlen(table->name) + strlen(column1_name) + name_length +2);  // + '_' + '\0'
		column->name[0] = 0;
		strcat(column->name, table->name);
		strcat(column->name, "_");
		strcat(column->name, column1_name);
		strcat(column->name, post_string);
	}
#endif
	sq_table_add_column(table, column, 1);
	return column;
}

void      sq_table_drop_composite(SqTable* table,
                                  SqType*  column_type,
                                  unsigned int bit_field,
                                  const char*  name)
{
	SqColumn* column;

	column = calloc(1, sizeof(SqColumn));
	column->type = column_type;
	column->bit_field = SQB_DYNAMIC | bit_field;
	column->old_name = strdup(name);
	sq_table_add_column(table, column, 1);

#if 0
	void** addr = sq_type_find_entry(table->type, name, NULL);
	if (addr)
		sq_type_erase_entry_addr(table->type, addr, 1);
#endif
}

SqColumn* sq_table_add_index(SqTable* table,
                             const char* index_name,
                             const char* column1_name, ...)
{
	SqColumn* column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_INDEX, 0, index_name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_index(SqTable* table, const char* index_name)
{
	sq_table_drop_composite(table, SQ_TYPE_INDEX, 0, index_name);
}

SqColumn* sq_table_add_unique(SqTable* table,
                              const char* unique_name,
                              const char* column1_name, ...)
{
	SqColumn* column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, unique_name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_unique(SqTable* table, const char* unique_name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, unique_name);
}

SqColumn* sq_table_add_primary(SqTable* table,
                               const char* primary_name,
                               const char* column1_name, ...)
{
	SqColumn* column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, primary_name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_primary(SqTable* table, const char* primary_name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, primary_name);
}

SqColumn* sq_table_add_foreign(SqTable* table, const char* name, const char* column_name)
{
	SqColumn* column;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
	sq_column_set_composite(column, column_name, NULL);
	return column;
}

void   sq_table_drop_foreign(SqTable* table, const char* name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
}

// --------------------------------------------------------
// migration functions

void  sq_table_replace_column(SqTable*   table,
                              SqColumn** old_in_type,
                              SqColumn** old_in_foreigns,
                              SqColumn*  new_one)
{
	SqPtrArray* reentries = NULL;
	SqColumn*   column;

	if ((table->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		table->type = sq_type_copy_static(table->type, (SqDestroyFunc)sq_column_free);
	// table->type->entry
	if (old_in_type == NULL)
		reentries = sq_type_get_ptr_array(table->type);
	else {
		column = *old_in_type;
		*old_in_type = new_one;
	}
	// table->foreigns
	if (old_in_foreigns == NULL) {
		if (column->foreign) {
			reentries = &table->foreigns;
			if (new_one && new_one->foreign == NULL)
				new_one = NULL;
		}
	}
	else {
		column = *old_in_foreigns;
		if (new_one && new_one->foreign == NULL)
			*old_in_foreigns = NULL;
		else
			*old_in_foreigns = new_one;
	}
	// replace old column by new_one
	if (reentries) {
		for (int index = 0;  index < reentries->length;  index++)
			if (reentries->data[index] == column)
				reentries->data[index]  = new_one;
	}
	// free column
	sq_column_free(column);
}

int   sq_table_include(SqTable* table, SqTable* table_src)
{	//         *table,     *table_src
	SqColumn   *column,    *column_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	int       index;
	void    **addr;

	if ((table->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		table->type = sq_type_copy_static(table->type, (SqDestroyFunc)sq_column_free);

	reentries = sq_type_get_ptr_array(table->type);
	reentries_src = sq_type_get_ptr_array(table_src->type);

	// if table is empty table
	if (reentries->length == 0) {
		// set SQB_CHANGED if it is "ALTER TABLE"
		if (table_src->bit_field & SQB_CHANGED)
			table->bit_field |= SQB_CHANGED;
	}

	for (index = 0;  index < reentries_src->length;  index++) {
		column_src = (SqColumn*)reentries_src->data[index];
		if (column_src->bit_field & SQB_CHANGED) {
			// === ALTER COLUMN ===
			// free column if column->name == column_src->name
			addr = sq_reentries_find_name(reentries, column_src->name);
			if (addr)
				sq_table_replace_column(table, (SqColumn**)addr, NULL, NULL);
			// set bit_field: column altered
			table->bit_field |= SQB_TABLE_COL_ALTERED;
		}
		else if (column_src->name == NULL) {
			// === DROP COLUMN / CONSTRAINT / KEY ===
			// free column if column->name == column_src->old_name
			addr = sq_reentries_find_name(reentries, column_src->old_name);
			if (addr)
				sq_table_replace_column(table, (SqColumn**)addr, NULL, NULL);
			// set bit_field: column dropped
			table->bit_field |= SQB_TABLE_COL_DROPPED;
		}
		else if (column_src->old_name) {
			// === RENAME COLUMN ===
			// find column if column->name == column_src->old_name
			addr = sq_reentries_find_name(reentries, column_src->old_name);
			// rename existing column->name to column_src->name
			if (addr) {
				column = *(SqColumn**)addr;
				if ((column->bit_field & SQB_DYNAMIC) == 0) {
					// create dynamic column to replace static one
					column = sq_column_copy_static(column);
					sq_table_replace_column(table, (SqColumn**)addr, NULL, column);
				}
				// store old_name temporary, program use it when SQLite recreate
				if (column->old_name == NULL)
					column->old_name = column->name;
				else
					free((char*)column->name);
				column->name = strdup(column_src->name);
				column->bit_field |= SQB_RENAMED;
			}
			// set bit_field: column renamed
			table->bit_field |= SQB_TABLE_COL_RENAMED;
		}
		else {
			// === ADD COLUMN / CONSTRAINT / KEY ===
			// set bit_field: column added
			if (column_src->type == SQ_TYPE_CONSTRAINT)
				table->bit_field |= SQB_TABLE_COL_ADDED_CONSTRAINT;
			else if (column_src->bit_field & (SQB_UNIQUE | SQB_PRIMARY))
				table->bit_field |= SQB_TABLE_COL_ADDED_UNIQUE;
			else if (column_src->default_value) {
				// DEFAULT (expression)
				if (column_src->default_value[0] == '(')
					table->bit_field |= SQB_TABLE_COL_ADDED_EXPRESSION;
				// DEFAULT CURRENT_TIME or CURRENT_DATE...etc
				else if (column_src->default_value[0] == 'C' && column_src->default_value[7] == '_')
					table->bit_field |= SQB_TABLE_COL_ADDED_CURRENT_TIME;
				else
					table->bit_field |= SQB_TABLE_COL_ADDED;
			}
			else
				table->bit_field |= SQB_TABLE_COL_ADDED;
		}

		// steal 'column_src' if 'table_src->type' is not static.
		if (table_src->type->bit_field & SQB_TYPE_DYNAMIC)
			reentries_src->data[index] = NULL;
		// append 'column_src' to table->type->entry and calculate new size
		sq_reentries_add(reentries, column_src);
		if (column_src->type)
			sq_type_decide_size((SqType*)table->type, (SqEntry*)column_src);

		// ADD or ALTER COLUMN that having foreign reference
		if (column_src->foreign && column_src->old_name == NULL) {
			if (table->foreigns.data == NULL)
				sq_ptr_array_init(&table->foreigns, 4, NULL);
			sq_ptr_array_append(&table->foreigns, column_src);
		}
	}

	// SqTable.type.entry must sort again
	((SqType*)table->type)->bit_field &= ~SQB_TYPE_SORTED;

	return SQCODE_OK;
}

void  sq_table_exclude(SqTable* table, SqPtrArray* excluded_columns, SqPtrArray* result)
{
	SqPtrArray* array = sq_type_get_ptr_array(table->type);
	SqColumn*   column;
	int         index_r = 0;    // index of result

	// allocate enough space
	if (result->length < array->length)
		sq_ptr_array_alloc(result, array->length - result->length);

	for (int index = 0;  index < array->length;  index++) {
		column = (SqColumn*)array->data[index];
		// don't copy column to 'result' if it in excluded list
		for (int index_x = 0;  ;  index_x++) {
			if (index_x == excluded_columns->length)
				result->data[index_r++] = (SqEntry*)column;
			if (column == (SqColumn*)excluded_columns->data[index_x])
				break;
		}
	}
	result->length = index_r;
}

int  sq_table_erase_records(SqTable* table, char version_comparison)
{
	SqPtrArray* columns;
	int  n_old_columns;

	// copy table->type if it is static SqType.
	if ((table->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		table->type = sq_type_copy_static(table->type, (SqDestroyFunc)sq_column_free);
	// It can get columns from table->type after calling sq_type_copy_static()
	columns = sq_type_get_ptr_array(table->type);
	// clear records
	sq_reentries_clear_records(columns, version_comparison);
	n_old_columns = sq_reentries_remove_null(columns, table->offset);
	// if database schema version < current schema version, reset table->offset for sq_schema_arrange()
	table->offset = (version_comparison == '<') ? 0 : columns->length;
	// clear SQB_CHANGED and SQB_RENAMED
	table->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);
	// if database schema version == current schema version
	if (version_comparison == '=') {
		table->bit_field |= SQB_TABLE_SQL_CREATED;
		table->bit_field &= ~(SQB_TABLE_COL_CHANGED);
	}

	return n_old_columns;
}

void   sq_table_complete(SqTable* table, bool no_unused_column)
{
	SqPtrArray* reentries;
	SqColumn*   column;
	bool        has_null = false;

	sq_ptr_array_final(&table->foreigns);

	if (table->type->bit_field & SQB_DYNAMIC) {
		reentries = sq_type_get_ptr_array(table->type);
		for (int index = 0;  index < reentries->length;  index++) {
			column = reentries->data[index];
			if (no_unused_column && (column->type == SQ_TYPE_INDEX || column->type == SQ_TYPE_CONSTRAINT)) {
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
//

static void  sq_foreign_free(SqForeign* foreign)
{
	free((char*)foreign->table);
	free((char*)foreign->column);
	free((char*)foreign->on_delete);
	free((char*)foreign->on_update);
	free((char*)foreign);
}

static SqForeign* sq_foreign_copy(SqForeign* src)
{
	SqForeign*  foreign;

	foreign = malloc(sizeof(SqForeign));
	foreign->table  = strdup(src->table);
	foreign->column = strdup(src->column);
	foreign->on_delete = strdup(src->on_delete);
	foreign->on_update = strdup(src->on_update);
	return foreign;
}

// ----------------------------------------------------------------------------
// SqColumn C functions

#define SQ_COMPOSITE_LENGTH_DEFAULT    4

#define sq_composite_allocated(constraint)    *( (intptr_t*) ((constraint)-1) )
#define sq_composite_alloc(n)                 ( (char**)malloc(sizeof(char*) *(n+1)) +1)
#define sq_composite_realloc(constraint, n)   ( (char**)realloc((constraint)-1, sizeof(char*) *(n+1)) +1)
#define sq_composite_free(constraint)         free((constraint)-1)

SqColumn*  sq_column_new(const char* name, const SqType* typeinfo)
{
	SqColumn*  column;

	column = calloc(1, sizeof(SqColumn));
	// init SqEntry members
	sq_entry_init((SqEntry*)column, typeinfo);
	column->name = strdup(name);

	// init SqColumn members
	return column;
}

void  sq_column_free(SqColumn* column)
{
	if (column->bit_field & SQB_DYNAMIC) {
		// reduce the stack frame:
		// sq_type_unref() will not be called by below sq_entry_final()
		if (column->type) {
			sq_type_unref((SqType*)column->type);
			column->type = NULL;
		}
		// finalize parent struct - SqEntry
		sq_entry_final((SqEntry*)column);
		// free SqColumn
		free((char*)column->default_value);
		free((char*)column->check);
		free((char*)column->raw);
		free((char*)column->old_name);
		if (column->foreign)
			sq_foreign_free(column->foreign);
		if (column->composite) {
			for (char** cur = column->composite;  *cur;  cur++)
				free(*cur);
			sq_composite_free(column->composite);
		}
		free(column);
	}
}

SqColumn* sq_column_copy_static(const SqColumn* column_src)
{
	SqColumn* column;
	int       index, length;

	column = malloc(sizeof(SqColumn));
	column->type      = column_src->type;
	column->offset    = column_src->offset;
	column->bit_field = column_src->bit_field | SQB_DYNAMIC;
	column->size      = column_src->size;
	column->digits    = column_src->digits;

	column->name          = column_src->name ? strdup(column_src->name) : NULL;
	column->default_value = column_src->default_value ? strdup(column_src->default_value) : NULL;
	column->check         = column_src->check ? strdup(column_src->check) : NULL;
	column->raw           = column_src->raw ? strdup(column_src->raw) : NULL;
	column->old_name      = column_src->old_name ? strdup(column_src->old_name) : NULL;

	column->foreign = NULL;
	if (column_src->foreign) {
		column->foreign = sq_foreign_copy(column_src->foreign);
		column->bit_field |= SQB_FOREIGN;
	}

	column->composite = NULL;
	if (column_src->composite) {
		for (index = 0;  column_src->composite[index];  index++)
			;
		if (index > 0) {
			length = index + 1;
			column->composite = sq_composite_alloc(length);
			column->composite[index] = NULL;
			for (index = 0;  index < length;  index++)
				column->composite[index] = strdup(column_src->composite[index]);
		}
	}
	return column;
}

// foreign key references
void  sq_column_reference(SqColumn* column,
                          const char* foreign_table_name,
                          const char* foreign_column_name)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	// remove foreign key if foreign_table_name == NULL
	if (foreign_table_name == NULL) {
		if (column)
			sq_foreign_free(column->foreign);
		column->foreign = NULL;
		return;
	}

	if (column->foreign == NULL) {
		column->foreign = malloc(sizeof(SqForeign));
		column->foreign->on_delete = NULL;
		column->foreign->on_update = NULL;
	}
	else {
		free((char*)column->foreign->table);
		free((char*)column->foreign->column);
	}

	column->foreign->table = strdup(foreign_table_name);
	column->foreign->column = strdup(foreign_column_name);
}

// foreign key on delete
void  sq_column_on_delete(SqColumn* column, const char* act)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;
	if (column->foreign)
		column->foreign->on_delete = strdup(act);
}

// foreign key on update
void  sq_column_on_update(SqColumn* column, const char* act)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;
	if (column->foreign)
		column->foreign->on_update = strdup(act);
}

void  sq_column_set_composite(SqColumn* column, ...)
{
	const char* first;
	va_list  arg_list;

	va_start(arg_list, column);
	first = va_arg(arg_list, const char*);
	sq_column_set_composite_va(column, first, arg_list);
	va_end(arg_list);
}

void  sq_column_set_composite_va(SqColumn* column, const char *name, va_list arg_list)
{
	int   index, allocated;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	if (column->composite == NULL) {
		column->composite = sq_composite_alloc(SQ_COMPOSITE_LENGTH_DEFAULT);
		allocated = SQ_COMPOSITE_LENGTH_DEFAULT;
	}
	else {
		allocated = sq_composite_allocated(column->composite);
		for (index = 0;  column->composite[index];  index++)
			free(column->composite[index]);
	}

	for (index = 0;  ;  index++) {
		// add string to null terminated string array 
		if (index == allocated) {
			allocated *= 2;
			column->composite = sq_composite_realloc(column->composite, allocated);
			sq_composite_allocated(column->composite) = allocated;
		}
		column->composite[index] = name ? strdup(name) : NULL;
		// break if name is NULL
		if (name)
			name = va_arg(arg_list, const char*);
		else
			break;
	}
}

// used by sq_table_arrange()
// primary key = 0
// foreign key = 1
// normal      = 2
// constraint  = 3
int  sq_column_cmp_attrib(SqColumn** column1, SqColumn** column2)
{
	int  var1 = 0, var2 = 0;

	if ((*column1)->type == SQ_TYPE_CONSTRAINT || (*column1)->type == SQ_TYPE_INDEX)
		var1 += 3;
	if ((*column1)->foreign || (*column1)->bit_field & SQB_FOREIGN)
		var1 += 1;
	else if (((*column1)->bit_field & SQB_PRIMARY) == 0)
		var1 += 2;

	if ((*column2)->type == SQ_TYPE_CONSTRAINT || (*column2)->type == SQ_TYPE_INDEX)
		var2 += 3;
	if ((*column2)->foreign || (*column2)->bit_field & SQB_FOREIGN)
		var2 += 1;
	else if (((*column2)->bit_field & SQB_PRIMARY) == 0)
		var2 += 2;

	return var1 - var2;
}
