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

#include <SqConfig.h>
#include <SqError.h>
#include <SqType.h>
#include <SqxcValue.h>
#include <SqJoint.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

static void sq_type_joint_init_instance(void *instance, const SqType *type);
static void sq_type_joint_final_instance(void *instance, const SqType *type);
static int  sq_type_joint_parse(void *instance, const SqType *type, Sqxc *src);

SqTypeJoint *sq_type_joint_new(void)
{
	SqTypeJoint *type_joint;

	type_joint = malloc(sizeof(SqTypeJoint));
	sq_type_joint_init(type_joint);
	return type_joint;
}

void    sq_type_joint_init(SqTypeJoint *type_joint)
{
	sq_type_init_self((SqType*)type_joint, 4, (SqDestroyFunc)sq_entry_free);
	type_joint->init  = sq_type_joint_init_instance;
	type_joint->final = sq_type_joint_final_instance;
	type_joint->parse = sq_type_joint_parse;
	type_joint->write = NULL;
	type_joint->size  = sizeof(void*);    // default size
}

void    sq_type_joint_add(SqTypeJoint *type_joint, SqTable *table, const char *table_as_name)
{
	SqEntry *jentry;

//	if ((type_joint->bit_field & SQB_TYPE_DYNAMIC) == 0)
//		return;
	if (table == NULL && table_as_name == NULL)
		return;  // error

	jentry = sq_entry_new(table ? table->type : NULL);
	jentry->name = strdup(table_as_name ? table_as_name : table->name);
	jentry->bit_field |= SQB_POINTER;
	jentry->offset = type_joint->n_entry * sizeof(void*);
	sq_type_add_entry((SqType*)type_joint, jentry, 1, 0);
}

void    sq_type_joint_erase(SqTypeJoint *type_joint, SqTable *table, const char *table_as_name)
{
	union {
		const char  *name;
		SqTable    **table_addr;	
	} temp;

//	if ((type_joint->bit_field & SQB_TYPE_DYNAMIC) == 0)
//		return;
	if (table_as_name)
		temp.name = table_as_name;
	else if (table)
		temp.name = table->name;
	else
		return;

	temp.table_addr = (SqTable**)sq_type_find_entry((SqType*)type_joint, temp.name, NULL);
	if (temp.table_addr)
		sq_type_erase_entry_addr((SqType*)type_joint, (SqEntry**)temp.table_addr, 1);
}

void  sq_type_joint_clear(SqTypeJoint *type_joint)
{
	sq_type_clear_entry((SqType*)type_joint);
}

// ----------------------------------------------------------------------------
// static function

static void sq_type_joint_init_instance(void *instance, const SqType *type)
{
	SqEntry    *table;

	// initialize structure of joined tables
	for (int index = 0;  index < type->n_entry;  index++) {
		table = type->entry[index];
		sq_type_init_instance(table->type, (char*)instance + table->offset, true);
	}
}

static void sq_type_joint_final_instance(void *instance, const SqType *type)
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
#if defined SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// Frist time to call this function to parse joint object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data = instance;
			nested->data2 = (void*)type;
			nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
		}
		if (src->type != SQXC_TYPE_OBJECT) {
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// SqxcNested is ready to parse joint object, type has been matched.
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
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = *(void**)((char*)instance +table->offset);
		nested->data2 = (void*)table->type;
#if defined SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
		nested->data3 = nested->data;    // SqxcNested is ready to parse, type has been matched.
#endif
		// call parser of 'table'
		src->name += temp.len;
		src->code  = table->type->parse(nested->data, nested->data2, src);
		src->name -= temp.len;
		// erase nested for 'table'
		sqxc_erase_nested((Sqxc*)xc_value, nested);
		return src->code;
	}

	return SQCODE_ENTRY_NOT_FOUND;
}

