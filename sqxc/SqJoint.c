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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <SqxcValue.h>
#include <SqJoint.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

static void sq_type_joint_init(void *instance, const SqType *type);
static void sq_type_joint_final(void *instance, const SqType *type);
static int  sq_type_joint_parse(void *instance, const SqType *type, Sqxc *src);

SqType *sq_type_joint_new(void)
{
	SqType *type = sq_type_new(4, (SqDestroyFunc)sq_entry_free);
	type->init = sq_type_joint_init;
	type->final = sq_type_joint_final;
	type->parse = sq_type_joint_parse;
	type->write = NULL;
	type->size = sizeof(void*);    // default size
	return type;
}

void    sq_type_joint_add(SqType *type_joint, SqTable *table, const char *table_as_name)
{
	SqEntry *jentry;

	jentry = sq_entry_new(table->type);
	if (table_as_name)
		jentry->name = strdup(table_as_name);
	else
		jentry->name = strdup(table->name);
	jentry->bit_field |= SQB_POINTER;
	jentry->offset = type_joint->n_entry * sizeof(void*);
	sq_type_add_entry(type_joint, jentry, 1, 0);
	sq_type_decide_size(type_joint, jentry, false);
}

// ----------------------------------------------------------------------------
// static function

static void sq_type_joint_init(void *instance, const SqType *type)
{
	SqEntry    *table;

	// initialize structure of joined tables
	for (int index = 0;  index < type->n_entry;  index++) {
		table = type->entry[index];
		sq_type_init_instance(table->type, (char*)instance + table->offset, true);
	}
}

static void sq_type_joint_final(void *instance, const SqType *type)
{
	SqEntry    *table;

	// finalize structure of joined tables
	for (int index = 0;  index < type->n_entry;  index++) {
		table = type->entry[index];
		sq_type_final_instance(table->type, (char*)instance + table->offset, true);
	}
}

static int  sq_type_joint_parse(void *instance, const SqType *type, Sqxc *src)
{
	SqxcValue  *xc_value = (SqxcValue*)src->dest;
	SqxcNested *nested;
	SqEntry    *table;
	SqBuffer   *buf;
	union {
		char   *dot;
		int     len;
	} temp;

	// Start of Object
	nested = xc_value->nested;
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// Frist time to call this function to parse joint object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data = instance;
			nested->data2 = (void*)type;
			nested->data3 = NULL;
		}
		if (src->type != SQXC_TYPE_OBJECT) {
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// ready to parse joint object
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// get table name from "table.column" string
	temp.dot = strchr(src->name, '.');
	if (temp.dot == NULL || temp.dot == src->name)
		return (src->code = SQCODE_ENTRY_NOT_FOUND);
	temp.len = (int)(temp.dot - src->name);
	// use SqxcValue.buf to find entry
	buf = sqxc_get_buffer(xc_value);
	buf->writed = 0;
	strncpy(sq_buffer_alloc(buf, temp.len*2), src->name, temp.len);   // alloc(buf, (temp.len+1)*2)
	buf->mem[temp.len] = 0;    // null-terminated
	temp.len++;                // + '.'

	table = (SqEntry*)sq_type_find_entry(type, buf->mem, NULL);
	if (table) {
		table = *(SqEntry**)table;
		// push nested for parser of 'table'
		nested = sqxc_push_nested(src);
		nested->data = *(void**)((char*)instance +table->offset);
		nested->data2 = (void*)table->type;
		nested->data3 = nested->data;
		// call parser of 'table'
		src->name += temp.len;
		src->code  = table->type->parse(nested->data, nested->data2, src);
		src->name -= temp.len;
		// erase nested for 'table'
		sqxc_erase_nested(src, nested);
		return src->code;
	}

	return SQCODE_ENTRY_NOT_FOUND;
}

