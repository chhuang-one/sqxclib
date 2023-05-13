/*
 *   Copyright (C) 2022-2023 by C.H. Huang
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
#include <SqRow.h>
#include <SqxcValue.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

#define SQ_ROW_LENGTH_DEFAULT    16

#define sq_row_cols(row)              (&(row)->cols)
#define sq_row_cols_ref_count(row)    sq_array_ref_count(&(row)->cols)


SqRow *sq_row_new(int cols_capacity, int capacity)
{
	SqRow *row;

	row = malloc(sizeof(SqRow));
	sq_row_init(row, cols_capacity, capacity);
	return row;
}

void   sq_row_free(SqRow *row)
{
	sq_row_final(row);
	free(row);
}

void  sq_row_init(SqRow *row, int cols_capacity, int capacity)
{
	if (cols_capacity == 0)
		cols_capacity = SQ_ROW_LENGTH_DEFAULT;
	if (capacity == 0)
		capacity = SQ_ROW_LENGTH_DEFAULT;
	sq_array_init(row, sizeof(SqValue), capacity);
	sq_array_init(sq_row_cols(row), sizeof(SqRowColumn), cols_capacity);
	sq_row_cols_ref_count(row) = 1;
}

void  sq_row_final(SqRow *row)
{
	SqRowColumn *col;
	SqValue     *val;
	int          index;
	int          cols_ref_count = --sq_row_cols_ref_count(row);

	for (index = 0;  index < row->length;  index++) {
		val = row->data + index;
		col = row->cols + ((row->cols_length == 1) ? 0 : index);
		if (col->type == SQ_TYPE_STR || col->type == SQ_TYPE_CHAR)
			free((char*)val->str);
		else if (SQ_TYPE_NOT_BUILTIN(col->type))
			sq_type_final_instance(col->type, val, true);
		// Don't free col->name if row->cols is shared.
		if (cols_ref_count == 0)
			free((char*)col->name);
	}

	sq_array_final(row);
	// Don't free row->cols if it is shared.
	if (cols_ref_count == 0)
		sq_array_final(sq_row_cols(row));
}

void   sq_row_free_cols_name(SqRow *row)
{
	SqRowColumn *col = row->cols;
	SqRowColumn *end = col + row->cols_length;

	for (;  col < end;  col++) {
		free((char*)col->name);
		col->name = NULL;
	}
}

bool   sq_row_share_cols(SqRow *row, SqRow *share_to)
{
	if (row->cols == share_to->cols)
		return false;

	if (--sq_row_cols_ref_count(share_to) == 0) {
		sq_row_free_cols_name(share_to);
		sq_array_final(sq_row_cols(share_to));
	}
	sq_row_cols_ref_count(row) += 1;
	share_to->cols = row->cols;
	share_to->cols_length = row->cols_length;
	return true;
}

SqRowColumn *sq_row_alloc_column(SqRow *row, int n_element)
{
	// Don't allocate space if row->cols is shared
	if (sq_row_cols_ref_count(row) > 1)
		return NULL;

	return sq_array_alloc(sq_row_cols(row), 1);
}

// ----------------------------------------------------------------------------

static void  sq_type_row_init_instance(void *instance, const SqTypeRow *type_row);
static int   sq_type_row_parse(void *instance, const SqType *type, Sqxc *src);
static Sqxc *sq_type_row_write(void *instance, const SqType *type, Sqxc *dest);

SqTypeRow *sq_type_row_new(void)
{
	SqTypeRow *type_row;

	type_row = malloc(sizeof(SqTypeRow));
	sq_type_row_init(type_row);
	return type_row;
}

void  sq_type_row_free(SqTypeRow *type_row)
{
	sq_type_row_final(type_row);
	free(type_row);
}

void  sq_type_row_init(SqTypeRow *type_row)
{
	sq_type_init_self((SqType*)type_row, 4, (SqDestroyFunc)sq_entry_free);
	type_row->init  = (SqTypeFunc)sq_type_row_init_instance;
	type_row->final = (SqTypeFunc)sq_row_final;
	type_row->parse = (SqTypeParseFunc)sq_type_row_parse;
	type_row->write = (SqTypeWriteFunc)sq_type_row_write;
	type_row->size  = sizeof(SqRow);
	type_row->bit_field |= SQB_TYPE_PARSE_UNKNOWN;
}

void  sq_type_row_final(SqTypeRow *type_row)
{
	sq_type_final_self((SqType*)type_row);
}

static void  sq_type_row_init_instance(void *instance, const SqTypeRow *type_row)
{
	sq_row_init(instance, SQ_ROW_LENGTH_DEFAULT, SQ_ROW_LENGTH_DEFAULT);
}

static void  sq_type_row_parse_unknown(void *instance, Sqxc *src)
{
	const SqType *type;
	SqRow        *row = instance;
	union {
		SqRowColumn  *col;
		SqValue      *val;
	} temp;

	temp.col = sq_row_alloc_column(row, 1);
	temp.col->name = (src->name) ? strdup(src->name) : NULL;
	temp.col->entry = src->entry;

	switch (src->type) {
	case SQXC_TYPE_NULL:
		temp.col->type = NULL;
		sq_row_alloc(row, 1)->pointer = NULL;
		return;

	case SQXC_TYPE_BOOL:
		temp.col->type = SQ_TYPE_BOOL;
		break;

	case SQXC_TYPE_INT:
		temp.col->type = SQ_TYPE_INT;
		break;

	case SQXC_TYPE_UINT:
		temp.col->type = SQ_TYPE_UINT;
		break;

	case SQXC_TYPE_INT64:
		temp.col->type = SQ_TYPE_INT64;
		break;

	case SQXC_TYPE_UINT64:
		temp.col->type = SQ_TYPE_UINT64;
		break;

	case SQXC_TYPE_TIME:
		temp.col->type = SQ_TYPE_TIME;
		break;

	case SQXC_TYPE_DOUBLE:
		temp.col->type = SQ_TYPE_DOUBLE;
		break;

	case SQXC_TYPE_STR:
		temp.col->type = SQ_TYPE_STR;
		sq_row_alloc(row, 1)->str = strdup(src->value.str);
		return;

	default:
		temp.col->type = SQ_TYPE_UNKNOWN;
	}

	type = temp.col->type;
	temp.val = sq_row_alloc(row, 1);
	if (SQ_TYPE_NOT_BUILTIN(type))
		temp.val = sq_type_init_instance(type, temp.val, true);
	type->parse(temp.val, type, src);
}

static int  sq_type_row_parse(void *instance, const SqType *type, Sqxc *src)
{
	SqxcValue  *xc_value = (SqxcValue*)src->dest;
	SqxcNested *nested;
	SqBuffer   *buf;
	union {
		void       **addr;
		SqEntry     *entry;
		SqColumn    *column;
	} p;
	union {
		char        *dot;
		int          len;
		SqValue     *val;
		SqRowColumn *col;
	} temp;

	// Start of Object
	nested = xc_value->nested;
#if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// Frist time to call this function to parse object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data = instance;
			nested->data2 = (void*)type;
			nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
		}
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCH
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// SqxcNested is ready to parse object, type has been matched.
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
#else
	if (nested->data != instance) {
		// do type match
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCH
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = instance;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
#endif  // SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	if (type->n_entry == 0) {
		// There is no table in SqTypeRow
		sq_type_row_parse_unknown(instance, src);
		return (src->code = SQCODE_OK);
	}

	// get table name from "table.column" string
	temp.dot = strchr(src->name, '.');
	if (type->n_entry == 1 && temp.dot == NULL) {
		// There is only 1 table in SqTypeRow and no table name in 'src->name'
		p.addr = (void**)type->entry;
		temp.len = 0;
	}
	else {
		// There are multiple tables in SqTypeRow
		if (temp.dot == NULL || temp.dot == src->name) {
			sq_type_row_parse_unknown(instance, src);
			return (src->code = SQCODE_ENTRY_NOT_FOUND);
		}
		temp.len = (int)(temp.dot - src->name);
		// use SqxcValue.buf to find entry
		buf = sqxc_get_buffer(xc_value);
		buf->writed = 0;
		strncpy(sq_buffer_alloc(buf, temp.len*2), src->name, temp.len);   // alloc(buf, (temp.len+1)*2)
		buf->mem[temp.len] = 0;    // null-terminated
		temp.len++;                // + '.'
		// find table by it's name in SqTypeRow.entry
		p.addr = sq_type_find_entry(type, buf->mem, NULL);
		if (p.addr == NULL) {
			sq_type_row_parse_unknown(instance, src);
			return (src->code = SQCODE_ENTRY_NOT_FOUND);
		}
	}

	// 'p.entry' pointer to element of SqTypeRow.entry
	p.entry = *p.addr;
	// find column by it's name in 'p.entry->type' ('p.entry->type' pointer to SqTable.type)
	p.addr = sq_type_find_entry(p.entry->type, src->name + temp.len, NULL);
	if (p.addr == NULL) {
		sq_type_row_parse_unknown(instance, src);
		return (src->code = SQCODE_ENTRY_NOT_FOUND);
	}
	// 'p.addr' pointer to SqColumn in SqTable.type
	p.column = *p.addr;
	type = p.column->type;
	if (type->parse == NULL) {
		sq_type_row_parse_unknown(instance, src);
		return (src->code = SQCODE_OK);
	}
	// allocate SqRowColumn
	temp.col = sq_row_alloc_column(instance, 1);
	temp.col->name = src->name ? strdup(src->name) : NULL;
	temp.col->type = type;
	temp.col->entry = (SqEntry*)p.column;
	// allocate SqValue
	temp.val = sq_row_alloc(instance, 1);
	if (SQ_TYPE_NOT_BUILTIN(type))
		temp.val = sq_type_init_instance(type, temp.val, true);
	return type->parse(temp.val, type, src);
}

static Sqxc *sq_type_row_write(void *instance, const SqType *type, Sqxc *dest)
{
	SqRow        *row = instance;
	const SqType *member_type;
	const char   *object_name = dest->name;
	union {
		SqValue      *val;
		SqRowColumn  *col;
	} temp;

	dest->type = SQXC_TYPE_OBJECT;
//	dest->name = object_name;     // "name" was set by caller of this function
	dest->value.pointer = NULL;
	dest->entry = NULL;           // SqxcSql and SqxcJsonc will use this
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	for (int index = 0;  index < row->length;  index++) {
		// SqRow.cols
		temp.col = row->cols + ((row->cols_length==1) ? 0 : index);
		member_type = temp.col->type;
		if (member_type == NULL)           // NULL type
			continue;
		if (member_type->write == NULL)    // don't write anything if function pointer is NULL
			continue;
		dest->name  = temp.col->name;      // set "name" before calling write()
		dest->entry = (SqEntry*)temp.col->entry;
		// SqRow.data
		temp.val = &row->data[index];
		if (SQ_TYPE_NOT_BUILTIN(member_type)) {
			temp.val = temp.val->pointer;
			if (temp.val == NULL) {
				dest->type = SQXC_TYPE_NULL;
				dest->value.pointer = NULL;
				dest = sqxc_send(dest);
				if (dest->code != SQCODE_OK)
					return dest;
				continue;
			}
		}
		dest = member_type->write(temp.val, member_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	dest->type = SQXC_TYPE_OBJECT_END;
	dest->name = object_name;
	dest->value.pointer = NULL;
	dest->entry = NULL;
	return sqxc_send(dest);
}

// extern
const SqType SqType_Row_ =
{
	sizeof(SqRow),
	(SqTypeFunc)sq_type_row_init_instance,
	(SqTypeFunc)sq_row_final,
	sq_type_row_parse,
	sq_type_row_write,
};
