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

static int  sq_joint_parse(void* instance, const SqType *type, Sqxc* src);

SqJoint* sq_joint_new()
{
	SqJoint*  joint;

	joint = sq_entry_new(NULL);
	joint->type->parse = sq_joint_parse;
	joint->type->write = NULL;
	return joint;
}

void     sq_joint_free(SqJoint* joint)
{
	sq_entry_free(joint);
}

void sq_joint_add(SqJoint* joint, SqTable* table, const char *as_table_name)
{
	SqEntry*  jentry;

	jentry = sq_entry_new(table->type);
	if (as_table_name)
		jentry->name = strdup(as_table_name);
	else
		jentry->name = strdup(table->name);
	jentry->bit_field |= SQB_POINTER;
	jentry->offset = joint->type->n_entry * sizeof(void*);
	sq_type_insert_entry(joint->type, jentry);
//	sq_type_decide_size(joint->type, jentry);
}

// ----------------------------------------------------------------------------
// static function

static int  sq_joint_parse(void* instance, const SqType *type, Sqxc* src)
{
	SqxcNested*  nested;
	SqEntry*     table;
	SqBuffer*    buf;
	int          buf_beg;
	union {
		char*    dot;
		int      len;
	} temp;

	// Start of Object - Frist time to call this function to parse object
	nested = src->nested;
	if (nested->data != instance) {
		if (src->type != SQXC_TYPE_OBJECT) {
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested(src);
		nested->data = instance;
		nested->data2 = (void*)type;
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
	temp.len = temp.dot - src->name;
	// use tail of src->buf to find entry
	buf = sqxc_get_buffer(src);
	buf_beg = buf->writed;             // save buf->writed
	strncpy(sq_buffer_alloc(buf, temp.len*2), src->name, temp.len);   // alloc(buf, (temp.len+1)*2)
	buf->buf[buf_beg + temp.len] = 0;  // null-terminated
	buf->writed = buf_beg;             // restore buf->writed
	temp.len++;                        // + '.'

	table = (SqEntry*)sq_type_find_entry(type, buf->buf +buf_beg, NULL);
	if (table) {
		table = *(SqEntry**)table;
		// push nested for parser of 'table'
		nested = sqxc_push_nested(src);
		nested->data = *(void**)((char*)instance +table->offset);
		nested->data2 = table->type;
		// call parser of 'table'
		src->name += temp.len;
		src->code  = table->type->parse(nested->data, nested->data2, src);
		src->name -= temp.len;
		// pop nested for 'table'
		sqxc_pop_nested(src);
		return src->code;
	}

	return SQCODE_ENTRY_NOT_FOUND;
}
