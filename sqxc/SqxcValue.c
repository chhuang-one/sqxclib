/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#include <SqError.h>
#include <SqxcValue.h>

/* ----------------------------------------------------------------------------
	SqxcInfo functions - destination of input chain

	SQXC_TYPE_xxx ---> SqxcValue ---> SqType.parse()
 */

static int  sqxc_value_send(SqxcValue* xcvalue, Sqxc* src)
{
	const SqType* type;
	SqxcNested* nested;
	void*       instance;

	nested = xcvalue->nested;
	if (nested->data != NULL) {
		instance = nested->data;
		type     = nested->data2;
		// End of Array or Object
		if (src->type & SQXC_TYPE_END) {
			sqxc_pop_nested((Sqxc*)xcvalue);
			return (src->code = SQCODE_OK);
		}
		// parse elements in array or entries in object
		src->code = type->parse(instance, type, src);
		return src->code;
	}

	type = xcvalue->current;
	if (type == NULL)
		return (src->code = SQCODE_TYPE_NOT_SUPPORT);
	src->code = type->parse(xcvalue->instance, type, src);
	return src->code;
}

static int  sqxc_value_ctrl(SqxcValue* xcvalue, int id, void* data)
{
	switch (id) {
	case SQXC_CTRL_READY:
		if (xcvalue->container)
			xcvalue->current = xcvalue->container;
		else
			xcvalue->current = xcvalue->element;
		xcvalue->instance = sq_type_init_instance(xcvalue->current,
		                                         &xcvalue->instance, true);
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

static void  sqxc_value_init(SqxcValue* xcvalue)
{
//	memset(xcvalue, 0, sizeof(SqxcValue));
	xcvalue->supported_type = SQXC_TYPE_ALL;
	// SqTypeWriteFunc like sq_type_object_write(), sq_type_xxx_array_write() need this line
	xcvalue->dest = (Sqxc*)xcvalue;
}

static void  sqxc_value_final(SqxcValue* xcvalue)
{
//	if (xcvalue->instance)
//		sq_type_final_instance(xcvalue->current, &xcvalue->instance, true);
//	sqxc_final(xcvalue);
}

// ----------------------------------------------------------------------------
// SqxcInfo

static const SqxcInfo sqxc_info_value =
{
	sizeof(SqxcValue),
	(SqInitFunc)sqxc_value_init,
	(SqFinalFunc)sqxc_value_final,
	(SqxcCtrlFunc)sqxc_value_ctrl,
	(SqxcSendFunc)sqxc_value_send,
};

const SqxcInfo *SQXC_INFO_VALUE = &sqxc_info_value;
