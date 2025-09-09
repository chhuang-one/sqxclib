/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#include <stdio.h>        // fprintf(), stderr
#include <stdbool.h>      // bool, true, false
#include <stdarg.h>       // va_list, va_start, va_end, va_arg, etc.
#include <stdlib.h>
#include <string.h>

#include <SqColumn.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

// foreign key actions
#define FOREIGN_ON_DELETE           1
#define FOREIGN_ON_UPDATE           2
#define FOREIGN_N_ACTIONS           2     // ON DELETE, ON UPDATE
// reserve array length for empty string "", ON DELETE, ON UPDATE
#define FOREIGN_N_RESERVE          (1+FOREIGN_N_ACTIONS)

// ----------------------------------------------------------------------------
// --- Simple C string array ---
// It's separator is empty string "", the last element must be NULL.
// It used by SqColumn::foreign and SqColumn::composite

#define SQ_STRS_HEADER_LEN          2
#define SQ_STRS_DEFAULT_LEN         4
#define sq_strs_header(strs)       ((strs) - SQ_STRS_HEADER_LEN)

#define sq_strs_length(strs)      *( (uintptr_t*) ((strs)-2) )
#define sq_strs_capacity(strs)    *( (uintptr_t*) ((strs)-1) )

#define sq_strs_malloc(n)          ( (char**)malloc(sizeof(char*) *(n+SQ_STRS_HEADER_LEN))    +SQ_STRS_HEADER_LEN )
#define sq_strs_realloc(strs, n)   ( (char**)realloc(sq_strs_header(strs), sizeof(char*) *(n+SQ_STRS_HEADER_LEN)) +SQ_STRS_HEADER_LEN )

static const char *sq_strs_empty_string = "";
#define SQ_STRS_EMPTY_STRING       ( (char*)sq_strs_empty_string )

static void sq_strs_clear(char **strs, bool release_array)
{
	char  **element;

	if (strs) {
		for (element = strs;  *element;  element++)
			if (*element != SQ_STRS_EMPTY_STRING)
				free(*element);
		if (release_array)
			free(sq_strs_header(strs));
	}
}

static char **sq_strs_copy(char **src, unsigned int reserve_len)
{
	char **strs, **cur;
	unsigned int   capacity;
	unsigned int   length = -1;    // number of strings in array before separator

	if (src == NULL)
		return NULL;
	// count number of strings in 'src'
	for (cur = src;  *cur;  cur++) {
		if (**cur == 0)    // separator is empty string ""
			if (length == -1)
				length = (int)(cur - src);
	}
	// decide array capacity
	capacity = (int)(cur - src);
	if (length == -1) {
		length = capacity;
		capacity += reserve_len;
	}
	else if (capacity < length + reserve_len)
		capacity = length + reserve_len;
	capacity += 1;    // + 1 for NULL-terminated array
	// create & initialize array
	strs = sq_strs_malloc(capacity);
	sq_strs_capacity(strs) = capacity;
	sq_strs_length(strs) = length;
	// copy strings from 'src'
	for (cur = strs;  *src;  src++, cur++)
		*cur = (**src) ? strdup(*src) : SQ_STRS_EMPTY_STRING;
	// NULL-terminated array
	*cur = NULL;
	// return newly created 'strs'
	return strs;
}

static char **sq_strs_set_va(char **strs, unsigned int reserve_len, va_list arg_list)
{
	unsigned int  index, capacity;

	if (strs) {
		capacity = (unsigned int)sq_strs_capacity(strs);
		sq_strs_clear(strs, false);
	}
	else {
		capacity = SQ_STRS_DEFAULT_LEN + reserve_len;
		strs = sq_strs_malloc(capacity);
	}

	// add & replace strings in array
	for (index = 0;  ;  index++) {
		if (index + reserve_len >= capacity) {
			capacity *= 2;
			strs = sq_strs_realloc(strs, capacity);
		}

		const char *name = va_arg(arg_list, const char*);
		if (name == NULL)
			break;
//		strs[index] = (*name) ? strdup(name) : SQ_STRS_EMPTY_STRING;
		strs[index] = strdup(name);
	}
	// update capacity & length
	sq_strs_capacity(strs) = capacity;
	sq_strs_length(strs) = index;
	// NULL-terminated array
	strs[index] = NULL;
	// return 'strs' or newly created string array
	return strs;
}

static char **sq_strs_set(char **strs, unsigned int reserve_len, ...)
{
	va_list   arg_list;

	va_start(arg_list, reserve_len);
	strs = sq_strs_set_va(strs, reserve_len, arg_list);
	va_end(arg_list);
	return strs;
}

// ----------------------------------------------------------------------------
// SqColumn C functions

SqColumn  *sq_column_new(const char *name, const SqType *typeinfo)
{
	SqColumn  *column;

	column = malloc(sizeof(SqColumn));
	sq_column_init(column, name, typeinfo);
	return column;
}

void  sq_column_free(SqColumn *column)
{
	if (column->bit_field & SQB_DYNAMIC) {
		sq_column_final(column);
		free(column);
	}
}

void  sq_column_init(SqColumn *column, const char *name, const SqType *type_info)
{
	memset(column, 0, sizeof(SqColumn));
	// init SqEntry members
	sq_entry_init((SqEntry*)column, type_info);
	column->name = strdup(name);
}

void  sq_column_final(SqColumn *column)
{
	if (column->bit_field & SQB_DYNAMIC) {
		// finalize parent struct - SqEntry
		sq_entry_final((SqEntry*)column);
		// free SqColumn
		free((char*)column->default_value);
//		free((char*)column->reserve);
		free((char*)column->raw);
		free((char*)column->old_name);
		// free string array
		sq_strs_clear(column->foreign,   true);
		sq_strs_clear(column->composite, true);
	}
}

SqColumn *sq_column_copy(SqColumn *column, const SqColumn *column_src)
{
	if (column == NULL)
		column = malloc(sizeof(SqColumn));

	column->type      = column_src->type;
	column->offset    = column_src->offset;
	column->bit_field = column_src->bit_field | SQB_DYNAMIC;
	column->sql_type  = column_src->sql_type;
	column->size      = column_src->size;
	column->digits    = column_src->digits;

	column->name          = column_src->name ? strdup(column_src->name) : NULL;
	column->old_name      = column_src->old_name ? strdup(column_src->old_name) : NULL;
	column->default_value = column_src->default_value ? strdup(column_src->default_value) : NULL;
#if SQ_CONFIG_TABLE_COLUMN_COMMENTS
	column->comments      = column_src->comments ? strdup(column_src->comments) : NULL;
#else
	column->comments      = NULL;
#endif
	column->raw           = column_src->raw ? strdup(column_src->raw) : NULL;

	column->foreign   = sq_strs_copy(column_src->foreign,   FOREIGN_N_RESERVE);
	column->composite = sq_strs_copy(column_src->composite, 0);

	return column;
}

// foreign key references
void  sq_column_reference(SqColumn *column, ...)
{
	va_list   arg_list;
	char     *action[FOREIGN_N_RESERVE];
	char    **action_end = action + FOREIGN_N_RESERVE;
	char    **cur;
	union {
		char  **old;
		char  **newed;
		char  **reserve;
	} foreign;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	foreign.old = column->foreign;
	// backup reserve block: empty string, ON DELETE, ON UPDATE actions
	if (foreign.old) {
		foreign.reserve = foreign.old + sq_strs_length(foreign.old);
		for (cur = action;  cur < action_end;  cur++) {
			*cur = *foreign.reserve;
			*foreign.reserve++ = NULL;    // clear reserve block
		}
	}

	va_start(arg_list, column);
	column->foreign = sq_strs_set_va(column->foreign, FOREIGN_N_RESERVE, arg_list);
	va_end(arg_list);

	// restore reserve block: empty string, ON DELETE, ON UPDATE actions
	if (foreign.reserve) {
		foreign.newed = column->foreign;
		foreign.reserve = foreign.newed + sq_strs_length(foreign.newed);
		for (cur = action;  cur < action_end;  cur++)
			*foreign.reserve++ = *cur;
		*foreign.reserve = NULL;        // NULL-terminated
	}
}

// foreign key ON DELETE, ON UPDATE actions
static void sq_column_on_event(SqColumn *column, int on_index, const char *action)
{
	char  **cur, **end, **on;
	bool    overwrite = false;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	// if SqColumn::foreign == NULL, allocate memory for it.
	if (column->foreign == NULL)
		column->foreign = sq_strs_set(column->foreign, FOREIGN_N_RESERVE, NULL);

	cur = column->foreign + sq_strs_length(column->foreign);
	end = cur + FOREIGN_N_RESERVE;
	on  = cur + on_index;
	for (;  cur < end;  cur++) {
		if (*cur == NULL || overwrite) {
			*cur = SQ_STRS_EMPTY_STRING;
			overwrite = true;
		}
	}
	// set action
	if (*on != SQ_STRS_EMPTY_STRING)
		free(*on);
	*on = (action) ? strdup(action) : SQ_STRS_EMPTY_STRING;
	// NULL-terminated array
	*end = NULL;
}

void  sq_column_on_delete(SqColumn *column, const char *action)
{
	sq_column_on_event(column, FOREIGN_ON_DELETE, action);
}

void  sq_column_on_update(SqColumn *column, const char *action)
{
	sq_column_on_event(column, FOREIGN_ON_UPDATE, action);
}

void  sq_column_set_composite(SqColumn *column, ...)
{
	va_list  arg_list;

	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	va_start(arg_list, column);
	column->composite = sq_strs_set_va(column->composite, 0, arg_list);
	va_end(arg_list);
}

void  sq_column_set_composite_va(SqColumn *column, va_list arg_list)
{
	if ((column->bit_field & SQB_DYNAMIC) == 0)
		return;

	column->composite = sq_strs_set_va(column->composite, 0, arg_list);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline function.

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqColumn.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

void  sq_column_set_name(SqColumn *column, const char *name) {
	SQ_ENTRY_SET_NAME(column, name);
}

void  sq_column_pointer(SqColumn *column) {
	column->bit_field |= SQB_POINTER;
}

void  sq_column_hidden(SqColumn *column) {
	column->bit_field |= SQB_HIDDEN;
}

void  sq_column_hidden_null(SqColumn *column) {
	column->bit_field |= SQB_HIDDEN_NULL;
}

void  sq_column_primary(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_PRIMARY;
}

void  sq_column_unique(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_UNIQUE;
}

void  sq_column_increment(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_INCREMENT;    // equal SQB_COLUMN_AUTOINCREMENT
}

void  sq_column_auto_increment(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_AUTOINCREMENT;
}

void  sq_column_nullable(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_NULLABLE;
}

void  sq_column_change(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_CHANGED;
}

void  sq_column_use_current(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_CURRENT;
}

void  sq_column_use_current_on_update(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_CURRENT_ON_UPDATE;
}

#if SQ_CONFIG_QUERY_ONLY_COLUMN
void  sq_column_query_only(SqColumn *column) {
	column->bit_field |= SQB_COLUMN_QUERY_ONLY;
}
#endif

void  sq_column_default(SqColumn *column, const char *default_value) {
	SQ_COLUMN_SET_DEFAULT(column, default_value);
}

#if SQ_CONFIG_TABLE_COLUMN_COMMENTS
void  sq_column_comment(SqColumn *column, const char *comment_str) {
	SQ_COLUMN_SET_COMMENT(column, comment_str);
}
#endif

void  sq_column_raw(SqColumn *column, const char *raw_property) {
	SQ_COLUMN_SET_RAW(column, raw_property);
}

#endif  // __STDC_VERSION__
