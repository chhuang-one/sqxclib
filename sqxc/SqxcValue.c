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

#include <SqError.h>
#include <SqType.h>
#include <SqxcValue.h>

/* ----------------------------------------------------------------------------
	SqxcInfo functions - destination of input chain

	SQXC_TYPE_xxx ---> SqxcValue ---> SqType.parse()
 */

static int  sqxc_value_send(SqxcValue *xcvalue, Sqxc *src)
{
	const SqType *type;
	SqxcNested   *nested;
	void         *instance;

	if (xcvalue->nested_count != 0) {
		nested   = xcvalue->nested;
		instance = nested->data;
		type     = nested->data2;
		// End of Array or Object
		if (src->type & SQXC_TYPE_END) {
			sqxc_pop_nested((Sqxc*)xcvalue);
			return (src->code = SQCODE_OK);
		}
		// parse elements in array or entries in object
		src->code = type->parse(instance, type, src);
		// Start of unknown Array or Object
		if (src->code == SQCODE_ENTRY_NOT_FOUND && src->type & SQXC_TYPE_NESTED) {
			nested = sqxc_push_nested((Sqxc*)xcvalue);
//			nested->data  = instance;
			nested->data2 = (void*)SQ_TYPE_UNKNOWN;
		}
		// return result
		return src->code;
	}

	type = xcvalue->container;
	if (type == NULL)
		type = xcvalue->element;

	src->code = type->parse(xcvalue->instance, type, src);
	return src->code;
}

static int  sqxc_value_ctrl(SqxcValue *xcvalue, int id, void *data)
{
	const SqType *type;

	switch (id) {
	case SQXC_CTRL_READY:
		// SqTypeParseFunc like sq_type_object_parse(), sq_type_xxx_array_parse() need this line
		xcvalue->dest = (Sqxc*)xcvalue;
		// if instance already exist
		if (xcvalue->instance)
			break;
		// create instance
		type = xcvalue->container;
		if (type == NULL)
			type = xcvalue->element;
		xcvalue->instance = sq_type_init_instance(type, &xcvalue->instance, true);
		break;

	case SQXC_CTRL_FINISH:
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcvalue);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_value_init(SqxcValue *xcvalue)
{
//	memset(xcvalue, 0, sizeof(SqxcValue));
	xcvalue->supported_type = SQXC_TYPE_ALL;
	// SqTypeParseFunc like sq_type_object_parse(), sq_type_xxx_array_parse() need this line
	xcvalue->dest = (Sqxc*)xcvalue;
}

static void  sqxc_value_final(SqxcValue *xcvalue)
{
//	if (xcvalue->instance)
//		sq_type_final_instance(xcvalue->current, &xcvalue->instance, true);
//	sqxc_final(xcvalue);
}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo SqxcInfo_Value_ =
{
	sizeof(SqxcValue),
	(SqInitFunc)sqxc_value_init,
	(SqFinalFunc)sqxc_value_final,
	(SqxcCtrlFunc)sqxc_value_ctrl,
	(SqxcSendFunc)sqxc_value_send,
};
