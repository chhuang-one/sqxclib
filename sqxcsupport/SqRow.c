/*
 *   Copyright (C) 2022 by C.H. Huang
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

typedef union  RowUnion        RowUnion;
typedef struct RowArray        RowArray;

struct RowArray {
	char  *data;
	int    length;
	int    allocated;
};

/*
	RowUnion.array[0] = SqRow.data array
	RowUnion.array[1] = SqRow.cols array
*/
union RowUnion {
	SqRow    row;
	RowArray array[2];
};

SqRow *sq_row_new(int cols_allocated, int allocated)
{
	SqRow *row;

	row = malloc(sizeof(SqRow));
	sq_row_init(row, cols_allocated, allocated);
	return row;
}

void   sq_row_free(SqRow *row)
{
	sq_row_final(row);
	free(row);
}

void  sq_row_init(SqRow *row, int cols_allocated, int allocated)
{
	memset(row, 0, sizeof(SqRow));
	if (cols_allocated) {
		sq_row_alloc_column(row, cols_allocated);
		row->cols_length = 0;
	}
	if (allocated) {
		sq_row_alloc(row, allocated);
		row->length = 0;
	}
}

void  sq_row_final(SqRow *row)
{
	SqRowColumn *col;
	SqValue     *val;
	int          index;

	for (index = 0;  index < row->length;  index++) {
		val = row->data + index;
		col = row->cols + ((row->cols_length == 1) ? 0 : index);
		if (col->type == SQ_TYPE_STRING || col->type == SQ_TYPE_CHAR)
			free((char*)val->string);
		else if (SQ_TYPE_NOT_BUILTIN(col->type))
			sq_type_final_instance(col->type, val, true);
		free((char*)col->name);
	}
	free(row->data);
	free(row->cols);
}

void *sq_row_alloc_part(SqRow *row, int n_element, int part, size_t element_size)
{
	RowArray *array = ((RowUnion*)row)->array + part;
	union {
		int  length;
		int  index;
	} temp;

	temp.length = array->length + n_element;
	if (array->allocated < temp.length) {
		if ( (array->allocated*=2) == 0)
			array->allocated = SQ_ROW_LENGTH_DEFAULT;
		if (array->allocated < temp.length)
			array->allocated = temp.length * 2;
		array->data = realloc(array->data, element_size * array->allocated);
	}
	temp.index = array->length;
	array->length += n_element;
	return array->data + element_size * temp.index;
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
	type_row->bit_field |= SQB_TYPE_JOINT_SUPPORT_UNKNOWN_TYPE;
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

	case SQXC_TYPE_STRING:
		temp.col->type = SQ_TYPE_STRING;
		sq_row_alloc(row, 1)->string = strdup(src->value.string);
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
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
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
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
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
		temp.col = sq_type_init_instance(type, temp.col, true);
	return type->parse(temp.col, type, src);
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
