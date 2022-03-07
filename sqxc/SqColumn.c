/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqColumn.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

static void  sq_foreign_free(SqForeign *foreign);
static SqForeign *sq_foreign_copy(SqForeign *src);

// ----------------------------------------------------------------------------
// SqColumn C functions

#define SQ_COMPOSITE_LENGTH_DEFAULT    4

#define sq_composite_allocated(constraint)    *( (intptr_t*) ((constraint)-1) )
#define sq_composite_alloc(n)                 ( (char**)malloc(sizeof(char*) *(n+1)) +1)
#define sq_composite_realloc(constraint, n)   ( (char**)realloc((constraint)-1, sizeof(char*) *(n+1)) +1)
#define sq_composite_free(constraint)         free((constraint)-1)

SqColumn  *sq_column_new(const char *name, const SqType *typeinfo)
{
	SqColumn  *column;

	column = calloc(1, sizeof(SqColumn));
	// init SqEntry members
	sq_entry_init((SqEntry*)column, typeinfo);
	column->name = strdup(name);

	// init SqColumn members
	return column;
}

void  sq_column_free(SqColumn *column)
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
			for (char **cur = column->composite;  *cur;  cur++)
				free(*cur);
			sq_composite_free(column->composite);
		}
		free(column);
	}
}

void  sq_column_default(SqColumn *column, const char *default_value)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	free((char*)column->default_value);
	column->default_value = strdup(default_value);
}

void  sq_column_raw(SqColumn *column, const char *raw_property)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	free((char*)column->raw);
	column->raw = strdup(raw_property);
}

SqColumn *sq_column_copy_static(const SqColumn *column_src)
{
	SqColumn *column;
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
void  sq_column_reference(SqColumn *column,
                          const char *foreign_table_name,
                          const char *foreign_column_name)
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
void  sq_column_on_delete(SqColumn *column, const char *act)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;
	if (column->foreign)
		column->foreign->on_delete = strdup(act);
}

// foreign key on update
void  sq_column_on_update(SqColumn *column, const char *act)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;
	if (column->foreign)
		column->foreign->on_update = strdup(act);
}

void  sq_column_set_composite(SqColumn *column, ...)
{
	va_list  arg_list;

	va_start(arg_list, column);
	sq_column_set_composite_va(column, arg_list);
	va_end(arg_list);
}

void  sq_column_set_composite_va(SqColumn *column, va_list arg_list)
{
	int   index, allocated;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	if (column->composite == NULL) {
		column->composite = sq_composite_alloc(SQ_COMPOSITE_LENGTH_DEFAULT);
		allocated = SQ_COMPOSITE_LENGTH_DEFAULT;
	}
	else {
		allocated = (int)sq_composite_allocated(column->composite);
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
		const char *name = va_arg(arg_list, const char*);
		if (name)
			column->composite[index] = strdup(name);
		else {
			column->composite[index] = NULL;
			break;
		}
	}
}

// used by sq_table_arrange()
// primary key = 0
// foreign key = 1
// normal      = 2
// constraint  = 3
int  sq_column_cmp_attrib(SqColumn **column1, SqColumn **column2)
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

// ----------------------------------------------------------------------------
// SqForeign C functions

static void  sq_foreign_free(SqForeign *foreign)
{
	free((char*)foreign->table);
	free((char*)foreign->column);
	free((char*)foreign->on_delete);
	free((char*)foreign->on_update);
	free((char*)foreign);
}

static SqForeign *sq_foreign_copy(SqForeign *src)
{
	SqForeign  *foreign;

	foreign = malloc(sizeof(SqForeign));
	foreign->table  = strdup(src->table);
	foreign->column = strdup(src->column);
	foreign->on_delete = strdup(src->on_delete);
	foreign->on_update = strdup(src->on_update);
	return foreign;
}
