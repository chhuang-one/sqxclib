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

#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <SqxcValue.h>
#include <SqJoint.h>

static int  sq_joint_parse(void* instance, SqType* type, Sqxc* src);

SqJoint* sq_joint_new()
{
	SqJoint*  joint;

	joint = sq_field_new(NULL);
	joint->type->parse = sq_joint_parse;
	joint->type->write = NULL;
	return joint;
}

void     sq_joint_free(SqJoint* joint)
{
	sq_field_free(joint);
}

void sq_joint_add(SqJoint* joint, SqTable* table, const char* as_table_name)
{
	SqField*  jfield;

	jfield = sq_field_new(table->type);
	if (as_table_name)
		jfield->name = strdup(as_table_name);
	else
		jfield->name = strdup(table->name);
	jfield->bit_field |= SQB_POINTER;
	jfield->offset = joint->type->map_length * sizeof(void*);
	sq_type_insert_field(joint->type, jfield);
//	sq_type_decide_size(joint->type, jfield);
}

// ----------------------------------------------------------------------------
// static function

static int  sq_joint_parse(void* instance, SqType* type, Sqxc* src)
{
	SqxcNested*  nested;
	SqField*     table;
	Sqxc*        dest;
	union {
		char*    dot;
		int      len;
	} temp;

	dest = src->dest;
	// Start of Object - Frist time to call this function to parse object
	nested = dest->nested;
	if (nested->data != instance) {
		if (src->type != SQXC_TYPE_OBJECT) {
			dest->type = SQXC_TYPE_OBJECT;    // set required type in dest->type
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested(dest);
		nested->data = instance;
		nested->data2 = type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Object : sqxc_value_send_in() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(dest);
		return (src->code = SQCODE_OK);
	}
	 */

	temp.dot = strchr(src->name, '.');
	if (temp.dot == NULL || temp.dot == src->name)
		return (src->code = SQCODE_FIELD_NOT_FOUND);
	temp.len = temp.dot - src->name + 1;
	if (dest->buf_size < temp.len) {
		dest->buf_size = temp.len * 2;
		dest->buf = realloc(dest->buf, dest->buf_size);
		dest->buf[temp.len -1] = 0;
	}
	strncpy(dest->buf, src->name, temp.len -1);

	table = sq_type_find_field(type, dest->buf, NULL);
	if (table) {
		nested = sqxc_push_nested((Sqxc*)dest);
		nested->data = (char*)instance + table->offset;
		nested->data2 = table->type;
		src->name += temp.len;
		src->code  = table->type->parse(nested->data, nested->data2, src);
		src->name -= temp.len;
		sqxc_pop_nested((Sqxc*)dest);
	}
	return src->code;
}

