/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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
#include <stdlib.h>
#include <string.h>

#include <SqTable.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

SqTable *sq_table_new(const char *name, const SqType *table_type)
{
	SqTable *table;

	table = malloc(sizeof(SqTable));
	sq_table_init(table, name, table_type);
	return (SqTable*)table;
}

void  sq_table_free(SqTable *table)
{
	if (table->bit_field & SQB_DYNAMIC) {
		sq_table_final(table);
		// free SqTable
		free(table);
	}
}

void  sq_table_init(SqTable *table, const char *name, const SqType *table_type)
{
	// create dynamic SqType
	if (table_type == NULL)
		table_type = sq_type_new(8, (SqDestroyFunc)sq_column_free);
	sq_entry_init((SqEntry*)table, table_type);
	table->name = (name) ? strdup(name) : NULL;
//	table->bit_field |= SQB_POINTER;
	table->old_name = NULL;
	// callback for derived Sqdb
	table->on_destory = NULL;
	// for (SQLite) migration.
	table->relation = NULL;
}

void  sq_table_final(SqTable *table)
{
	if (table->bit_field & SQB_DYNAMIC) {
		// table->type == NULL if 'table' is migration record
		if (table->type)
			sq_type_free((SqType*)table->type);
		// finalize parent struct - SqEntry
		sq_entry_final((SqEntry*)table);
		free((char*)table->old_name);

		// call on_destory() callback to free 'relation' for SQLite
		if (table->on_destory)
			table->on_destory(table);
	}
}

void  sq_table_add_column(SqTable *table, const SqColumn *column, int n_column)
{
	SqType *type = (SqType*)table->type;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		type = sq_type_copy_static(NULL, type, (SqDestroyFunc)sq_column_free);
		table->type = type;
	}
	sq_type_add_entry(type, (SqEntry*)column, n_column, sizeof(SqColumn));
}

void  sq_table_add_column_ptrs(SqTable *table, const SqColumn **column_ptrs, int n_column_ptrs)
{
	SqType  *type = (SqType*)table->type;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0) {
		type = sq_type_copy_static(NULL, type, (SqDestroyFunc)sq_column_free);
		table->type = type;
	}
	sq_type_add_entry_ptrs(type, (const SqEntry**)column_ptrs, n_column_ptrs);
}

void  sq_table_drop_column(SqTable *table, const char *column_name)
{
	SqColumn  *column;

	column = calloc(1, sizeof(SqColumn));
	column->old_name = strdup(column_name);

	sq_table_add_column(table, column, 1);
	table->bit_field |= SQB_TABLE_CHANGED;

#if 0
	column = (SqColumn*)sq_type_find_entry(table->type, column_name, NULL);
	if (column) {
		sq_type_steal_entry_addr(table->type, (void**)column, 1);
	}
#endif
}

void  sq_table_rename_column(SqTable *table, const char *from, const char *to)
{
	SqColumn  *column;

	column = calloc(1, sizeof(SqColumn));
	column->old_name = strdup(from);
	column->name = strdup(to);
	column->bit_field = SQB_DYNAMIC;

	sq_table_add_column(table, column, 1);
	table->bit_field |= SQB_TABLE_CHANGED;

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

int  sq_table_get_columns(SqTable *table, SqPtrArray *ptrarray,
                          const SqType *type, unsigned int bit_field)
{
	SqPtrArray  *colarray;
	SqColumn    *column;
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
		if (column->foreign  &&  bit_field & SQB_COLUMN_FOREIGN)
			bit_field &= ~SQB_COLUMN_FOREIGN;
		// check 2 conditions
		if ((column->bit_field & bit_field) == bit_field)
			matched_count++;
		if (column->type == type || type == NULL)
			matched_count++;
		// if matched 2 conditions
		if (matched_count == 2) {
			if (ptrarray)
				sq_ptr_array_push(ptrarray, column);
			count++;
		}
	}
	return count;
}

SqColumn *sq_table_get_primary(SqTable *table, const SqType *type_in_table)
{
	SqPtrArray *array;
	SqColumn   *column;

	if (type_in_table)
		array = sq_type_get_ptr_array(type_in_table);
	else
		array = sq_type_get_ptr_array(table->type);

	for (int index = 0;  index < array->length;  index++) {
		column = array->data[index];
		if (column->bit_field & SQB_COLUMN_PRIMARY && SQ_TYPE_IS_INT(column->type))
			return column;
	}
	return NULL;
}

SqColumn *sq_table_add_bool(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_BOOL);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_int(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_INT);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_uint(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_UINT);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_int64(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_INT64);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_uint64(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_UINT64);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_double(SqTable *table, const char *name, size_t offset, int precision, int scale)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_DOUBLE);
	column->offset = offset;
	column->size = precision;
	column->digits = scale;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_timestamp(SqTable *table, const char *name, size_t offset)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_TIME);
	column->offset = offset;

	sq_table_add_column(table, column, 1);
	return column;
}

void  sq_table_add_timestamps(SqTable *table,
                              const char *created_at_name, size_t created_at_offset,
                              const char *updated_at_name, size_t updated_at_offset)
{
	SqColumn *column;

	if (created_at_name == NULL)
		created_at_name = "created_at";
	column = sq_column_new(created_at_name, SQ_TYPE_TIME);
	column->offset = created_at_offset;
	column->bit_field |= SQB_COLUMN_CURRENT;
	sq_table_add_column(table, column, 1);

	if (updated_at_name == NULL)
		updated_at_name = "updated_at";
	column = sq_column_new(updated_at_name, SQ_TYPE_TIME);
	column->offset = updated_at_offset;
	column->bit_field |= SQB_COLUMN_CURRENT | SQB_COLUMN_CURRENT_ON_UPDATE;
	sq_table_add_column(table, column, 1);
}

SqColumn *sq_table_add_string(SqTable *table, const char *name, size_t offset, int length)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_STR);
	column->offset = offset;
	column->size = length;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_char(SqTable *table, const char *name, size_t offset, int length)
{
	SqColumn *column;

	column = sq_column_new(name, SQ_TYPE_CHAR);
	column->offset = offset;
	column->size = length;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_text(SqTable *table, const char *name, size_t offset)
{
	return sq_table_add_mapping(table, name, offset, SQ_TYPE_STR, SQ_SQL_TYPE_TEXT);
}

SqColumn *sq_table_add_binary(SqTable *table, const char *name, size_t offset)
{
// This is mainly used by SQLite, MySQL to get length of BLOB column.
// If you use PostgreSQL and don't need store result of special query to C structure's member,
// you can disable SQ_CONFIG_QUERY_ONLY_COLUMN.
#if SQ_CONFIG_QUERY_ONLY_COLUMN
	sq_table_add_function(table, name, offset + offsetof(SqBuffer, size),
	                      SQ_TYPE_INT, "length");
#endif

	return sq_table_add_mapping(table, name, offset, SQ_TYPE_BUFFER, SQ_SQL_TYPE_BINARY);
}

SqColumn *sq_table_add_custom(SqTable *table, const char *name,
                              size_t offset, const SqType *sqtype,
                              int  length)
{
	SqColumn *column;

	column = sq_column_new(name, sqtype);
	column->offset = offset;
	column->size = length;

	sq_table_add_column(table, column, 1);
	return column;
}

SqColumn *sq_table_add_mapping(SqTable *table, const char *column_name,
                               size_t offset, const SqType *sqtype,
                               int  sql_type)
{
	SqColumn *column;

	column = sq_column_new(column_name, sqtype);
	column->offset = offset;
	column->sql_type = sql_type;

	sq_table_add_column(table, column, 1);
	return column;
}

#if SQ_CONFIG_QUERY_ONLY_COLUMN
// This is mainly used by SQLite, MySQL to get length of BLOB column.
// If you don't need store result of special query to C structure's member,
// you can disable SQ_CONFIG_QUERY_ONLY_COLUMN.
SqColumn *sq_table_add_function(SqTable *table, const char *column_name,
                                size_t offset, const SqType *sqtype,
                                const char *function_name)
{
	char *query_name;
	union {
		SqColumn *column;
		int       len;
	} temp;

	temp.len = (int)strlen(function_name);
	// function_name + ( + column_name + ) + null-terminated
	query_name = malloc(temp.len + strlen(column_name) + 3);
	strcpy(query_name, function_name);
	strcpy(query_name + temp.len, "(");
	strcpy(query_name + temp.len + 1, column_name);
	strcat(query_name + temp.len + 1, ")");
	temp.column = sq_column_new(query_name, sqtype);
	free(query_name);
	temp.column->offset = offset;
	temp.column->bit_field = SQB_COLUMN_QUERY_ONLY;

	// sq_table_add_column() will call sq_type_add_entry() to add column;
	// sq_type_add_entry() has set SQB_TYPE_QUERY_FIRST in table->type->bit_field.
//	table->type->bit_field |= SQB_TYPE_QUERY_FIRST;
	sq_table_add_column(table, temp.column, 1);

	return temp.column;
}
#endif  // SQ_CONFIG_QUERY_ONLY_COLUMN

// --------------------------------------------------------
// SqTable C functions for CONSTRAINT

SqColumn *sq_table_add_composite(SqTable *table,
                                 SqType  *column_type,
                                 unsigned int bit_field,
                                 const char  *name)
{
	SqColumn *column;

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
		const char *post_string;
		int  name_length;

		switch(bit_field) {
		case SQB_COLUMN_UNIQUE:
			post_string = "_unique";
			name_length = 7;
			break;
		case SQB_COLUMN_PRIMARY:
			post_string = "_primary";
			name_length = 8;
			break;
		case SQB_COLUMN_FOREIGN:
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

void      sq_table_drop_composite(SqTable *table,
                                  SqType  *column_type,
                                  unsigned int bit_field,
                                  const char  *name)
{
	SqColumn *column;

	column = calloc(1, sizeof(SqColumn));
	column->type = column_type;
	column->bit_field = SQB_DYNAMIC | bit_field;
	column->old_name = strdup(name);
	sq_table_add_column(table, column, 1);

#if 0
	void **addr = sq_type_find_entry(table->type, name, NULL);
	if (addr)
		sq_type_erase_entry_addr(table->type, addr, 1);
#endif
}

SqColumn *sq_table_add_index(SqTable *table,
                             const char *index_name,
                             ...)
{
	SqColumn *column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_INDEX, 0, index_name);
	va_start(arg_list, index_name);
	sq_column_set_composite_va(column, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_index(SqTable *table, const char *index_name)
{
	sq_table_drop_composite(table, SQ_TYPE_INDEX, 0, index_name);
}

SqColumn *sq_table_add_unique(SqTable *table,
                              const char *unique_name,
                              ...)
{
	SqColumn *column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_UNIQUE, unique_name);
	va_start(arg_list, unique_name);
	sq_column_set_composite_va(column, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_unique(SqTable *table, const char *unique_name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_UNIQUE, unique_name);
}

SqColumn *sq_table_add_primary(SqTable *table,
                               const char *primary_name,
                               ...)
{
	SqColumn *column;
	va_list   arg_list;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_PRIMARY, primary_name);
	va_start(arg_list, primary_name);
	sq_column_set_composite_va(column, arg_list);
	va_end(arg_list);
	return column;
}

void   sq_table_drop_primary(SqTable *table, const char *primary_name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_PRIMARY, primary_name);
}

SqColumn *sq_table_add_foreign(SqTable *table, const char *name, const char *column_name)
{
	SqColumn *column;

	column = sq_table_add_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_FOREIGN, name);
	sq_column_set_composite(column, column_name, NULL);
	return column;
}

void   sq_table_drop_foreign(SqTable *table, const char *name)
{
	sq_table_drop_composite(table, SQ_TYPE_CONSTRAINT, SQB_COLUMN_FOREIGN, name);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqTable.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

void  sq_table_set_name(SqTable *table, const char *name)
{
	SQ_ENTRY_SET_NAME(table, name);
}

bool  sq_table_has_column(SqTable *table, const char *column_name)
{
	return (sq_type_find_entry(table->type, column_name, NULL) != NULL);
}

SqColumn *sq_table_find_column(SqTable *table, const char *column_name)
{
	return (SqColumn*)sq_entry_find((SqEntry*)table, column_name, NULL);
}

#endif  // __STDC_VERSION
