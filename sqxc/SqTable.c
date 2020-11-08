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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <SqTable.h>

#define N_CONSTRAINT_LEN    4

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
		// clear SqEntry
		sq_entry_final((SqEntry*)table);
		free(table->old_name);
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

static void  sq_table_append_column(SqTable* table, SqColumn* column)
{
	SqType* table_type = table->type;

	if ((table_type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		table_type  = sq_type_copy_static(table_type, (SqDestroyFunc)sq_column_free);
		table->type = table_type;
	}

	sq_type_insert_entry(table_type, (SqEntry*)column);
//	sq_ptr_array_append(&table_type->entry, column);
//	table_type->bit_field &= ~SQB_TYPE_SORTED;
}

void  sq_table_drop_column(SqTable* table, const char* column_name)
{
	SqColumn*  column;

	column = calloc(1, sizeof(SqColumn));
	column->old_name = strdup(column_name);

	sq_table_append_column(table, column);
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

	sq_table_append_column(table, column);
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

int  sq_table_get_columns(SqTable* table, SqPtrArray* ptrarray, unsigned int bit_field)
{
	SqPtrArray* colarray;
	SqColumn*   column;
	int         count = 0;

	if (ptrarray && ptrarray->data == NULL)
		sq_ptr_array_init(ptrarray, 4, NULL);

	colarray = sq_type_get_ptr_array(table->type);
	for (int index = 0;  index < colarray->length;  index++) {
		column = colarray->data[index];
		if ((column->bit_field & bit_field) == 0) {
			if ((bit_field & SQB_FOREIGN    && column->foreign) ||
			    (bit_field & SQB_CONSTRAINT && column->constraint) )
				;
			else
				continue;
		}
		if (ptrarray)
			sq_ptr_array_append(ptrarray, column);
		count++;
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

SqColumn* sq_table_add_int(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_INT);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_uint(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_UINT);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_int64(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_INT64);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_uint64(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_UINT64);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_double(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_DOUBLE);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_timestamp(SqTable* table, const char* name, size_t offset)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_TIME);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_string(SqTable* table, const char* name, size_t offset, int length)
{
	SqColumn* column;

	column = sq_column_new(name, SQ_TYPE_STRING);
	column->offset = offset;
	column->size = length;

	sq_table_append_column(table, column);
	return column;
}

SqColumn* sq_table_add_custom(SqTable* table, const char* name,
                              size_t offset, const SqType* sqtype)
{
	SqColumn* column;

	column = sq_column_new(name, sqtype);
	column->offset = offset;

	sq_table_append_column(table, column);
	return column;
}

// --------------------------------------------------------
// SqTable C functions for CONSTRAINT

// ----------------------------------------------------------------------------
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqTable.h
#else

SqColumn* sq_table_add_constraint(SqTable* table,
                                  unsigned int bit_field,
                                  const char* name,
                                  const char* column1_name, ...)
{
	SqColumn* column;

	va_list  arg_list;
	va_start(arg_list, column1_name);
	column = sq_table_add_constraint_va(table, bit_field, name, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

#endif  // __STDC_VERSION__

SqColumn* sq_table_add_constraint_va(SqTable* table,
                                     unsigned int bit_field,
                                     const char* name,
                                     const char* column1_name,
                                     va_list arg_list)
{
	SqColumn* column;

	column = calloc(1, sizeof(SqColumn));
	column->bit_field = SQB_DYNAMIC | SQB_CONSTRAINT | bit_field;
#if 1
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
	sq_column_set_constraint_va(column, column1_name, arg_list);
	sq_table_append_column(table, column);
	return column;
}

void      sq_table_drop_constraint(SqTable* table,
                                   unsigned int bit_field,
                                   const char* name)
{
	SqColumn* column;

	column = calloc(1, sizeof(SqColumn));
	column->bit_field = SQB_DYNAMIC | SQB_CONSTRAINT | bit_field;
	column->old_name = strdup(name);
	sq_table_append_column(table, column);

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

	va_list  arg_list;
	va_start(arg_list, column1_name);
	column = sq_table_add_constraint_va(table, SQB_INDEX, index_name, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_index(SqTable* table, const char* index_name)
{
	sq_table_drop_constraint(table, SQB_INDEX, index_name);
}

SqColumn* sq_table_add_unique(SqTable* table,
                              const char* unique_name,
                              const char* column1_name, ...)
{
	SqColumn* column;

	va_list  arg_list;
	va_start(arg_list, column1_name);
	column = sq_table_add_constraint_va(table, SQB_UNIQUE, unique_name, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_unique(SqTable* table, const char* unique_name)
{
	sq_table_drop_constraint(table, SQB_UNIQUE, unique_name);
}

SqColumn* sq_table_add_primary(SqTable* table,
                               const char* primary_name,
                               const char* column1_name, ...)
{
	SqColumn* column;

	va_list  arg_list;
	va_start(arg_list, column1_name);
	column = sq_table_add_constraint_va(table, SQB_PRIMARY, primary_name, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_primary(SqTable* table, const char* primary_name)
{
	sq_table_drop_constraint(table, SQB_PRIMARY, primary_name);
}

SqColumn* sq_table_add_foreign(SqTable* table, const char* name, const char* column_name)
{
	return sq_table_add_constraint(table, SQB_FOREIGN, name, column_name, NULL);
}

void   sq_table_drop_foreign(SqTable* table, const char* name)
{
	sq_table_drop_constraint(table, SQB_FOREIGN, name);
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
					free(column->name);
				column->name = strdup(column_src->name);
				column->bit_field |= SQB_RENAMED;
			}
			// set bit_field: column renamed
			table->bit_field |= SQB_TABLE_COL_RENAMED;
		}
		else {
			// === ADD COLUMN / CONSTRAINT / KEY ===
			// set bit_field: column added
			if (column_src->constraint)
				table->bit_field |= SQB_TABLE_COL_ADDED_CONSTRAINT;
			else if (column->bit_field & (SQB_UNIQUE | SQB_PRIMARY))
				table->bit_field |= SQB_TABLE_COL_ADDED_UNIQUE;
			else
				table->bit_field |= SQB_TABLE_COL_ADDED;
		}

		// steal 'column_src' if 'table_src->type' is not static.
		if (table_src->type->bit_field & SQB_TYPE_DYNAMIC)
			reentries_src->data[index] = NULL;
		// append 'column_src' to table->type->entry
		sq_reentries_add(reentries, column_src);

		// ADD or ALTER COLUMN that having foreign reference
		if (column_src->foreign && column_src->old_name == NULL) {
			if (table->foreigns.data == NULL)
				sq_ptr_array_init(&table->foreigns, 4, NULL);
			sq_ptr_array_append(&table->foreigns, column_src);
		}
	}

	// SqTable.type.entry must sort again
	table->type->bit_field &= ~SQB_TYPE_SORTED;

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

void   sq_table_complete(SqTable* table)
{
	SqPtrArray* reentries;
	SqColumn*   column;
	bool        has_null = false;

	sq_ptr_array_final(&table->foreigns);

	if (table->type->bit_field & SQB_DYNAMIC) {
		reentries = sq_type_get_ptr_array(table->type);
		for (int index = 0;  index < reentries->length;  index++) {
			column = reentries->data[index];
			if (column->bit_field & (SQB_INDEX | SQB_CONSTRAINT) || column->constraint) {
				sq_column_free(column);
				reentries->data[index] = NULL;
				has_null = true;
			}
			if (column->bit_field & (SQB_RENAMED | SQB_DYNAMIC))
				free(column->old_name);
		}
		if (has_null)
			sq_reentries_remove_null(reentries);
		// sort columns by name
		sq_type_sort_entry(table->type);
	}
}

// ----------------------------------------------------------------------------
//

static void  sq_foreign_free(SqForeign* foreign)
{
	free(foreign->table);
	free(foreign->column);
	free(foreign->on_delete);
	free(foreign->on_update);
	free(foreign);
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

#define sq_constraint_allocated(constraint)    *( (intptr_t*) ((constraint)-1) )
#define sq_constraint_alloc(n)                 ( (char**)malloc(sizeof(char*) *(n+1)) +1)
#define sq_constraint_realloc(constraint, n)   ( (char**)realloc((constraint)-1, sizeof(char*) *(n+1)) +1)
#define sq_constraint_free(constraint)         free((constraint)-1)

// ----------------------------------------------------------------------------
// SqColumn C functions

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
		// clear SqEntry
		sq_entry_final((SqEntry*)column);
		// free SqColumn
		free(column->default_value);
		free(column->check);
//		free(column->comment);
		free(column->extra);
		free(column->old_name);
		if (column->foreign)
			sq_foreign_free(column->foreign);
		if (column->constraint)
			sq_constraint_free(column->constraint);
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
//	column->comment       = column_src->comment ? strdup(column_src->comment) : NULL;
	column->extra         = column_src->extra ? strdup(column_src->extra) : NULL;
	column->old_name      = column_src->old_name ? strdup(column_src->old_name) : NULL;

	column->foreign = NULL;
	if (column_src->foreign) {
		if (column_src->old_name)
			column->bit_field |= SQB_FOREIGN;  // DROP or RENAME
		else
			column->foreign = sq_foreign_copy(column_src->foreign);
	}

	column->constraint = NULL;
	if (column_src->constraint) {
		if (column_src->old_name)
			column->bit_field |= SQB_CONSTRAINT;  // DROP or RENAME
		else {
			for (index = 0;  column_src->constraint[index];  index++)
				;
			if (index > 0) {
				length = index + 1;
				column->constraint = sq_constraint_alloc(length);
				column->constraint[index] = NULL;
				for (index = 0;  index < length;  index++)
					column->constraint[index] = strdup(column_src->constraint[index]);
			}
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
		free(column->foreign->table);
		free(column->foreign->column);
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

void  sq_column_set_constraint(SqColumn* column, ...)
{
	const char* first;
	va_list  arg_list;

	va_start(arg_list, column);
	first = va_arg(arg_list, const char*);
	sq_column_set_constraint_va(column, first, arg_list);
	va_end(arg_list);
}

void  sq_column_set_constraint_va(SqColumn* column, const char *name, va_list arg_list)
{
	int   index, allocated;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	if (column->constraint == NULL) {
		column->constraint = sq_constraint_alloc(N_CONSTRAINT_LEN);
		allocated = N_CONSTRAINT_LEN;
	}
	else {
		allocated = sq_constraint_allocated(column->constraint);
		for (index = 0;  column->constraint[index];  index++)
			free(column->constraint[index]);
	}

	for (index = 0;  ;  index++) {
		// add string to null terminated string array 
		if (index == allocated) {
			allocated *= 2;
			column->constraint = sq_constraint_realloc(column->constraint, allocated);
			sq_constraint_allocated(column->constraint) = allocated;
		}
		column->constraint[index] = name ? strdup(name) : NULL;
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

	if ((*column1)->constraint)
		var1 += 3;
	if ((*column1)->foreign || (*column1)->bit_field & SQB_FOREIGN)
		var1 += 1;
	else if (((*column1)->bit_field & SQB_PRIMARY) == 0)
		var1 += 2;

	if ((*column2)->constraint)
		var2 += 3;
	if ((*column2)->foreign || (*column2)->bit_field & SQB_FOREIGN)
		var2 += 1;
	else if (((*column2)->bit_field & SQB_PRIMARY) == 0)
		var2 += 2;

	return var1 - var2;
}
