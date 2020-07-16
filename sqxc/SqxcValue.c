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

#include <SqError.h>
#include <SqxcValue.h>

/* ----------------------------------------------------------------------------
   destination of input chain

	SQXC_TYPE_xxxx ---> SqxcValue ---> C instance
	               send           send

	SqxcSqlite ---> SqxcJsonc ---> SqxcValue
	   src ---------> middle -------> dest

	SqxcJsonc ---> SqxcValue
	   src ---------> dest
 */

static int  sqxc_value_send_in(SqxcValue* xcvalue, Sqxc* src)
{
	SqxcNested* nested;
	SqType*     type;
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
		// parse elements in array or fields in object
		src->code = type->parse(instance, type, src);
		return src->code;
	}

	type = xcvalue->current;
	if (type == NULL)
		return (src->code = SQCODE_TYPE_NOT_SUPPORT);
	src->code = type->parse(xcvalue->instance, type, src);

	return src->code;
}

static int  sqxc_value_ctrl_in(SqxcValue* xcvalue, int id, void* data)
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
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_value_init_in(SqxcValue* xcvalue)
{
//	memset(xcvalue, 0, sizeof(SqxcValue));
//	sqxc_init(xcvalue);
	xcvalue->ctrl = (SqxcCtrlFunc)sqxc_value_ctrl_in;
	xcvalue->send = (SqxcSendFunc)sqxc_value_send_in;
	xcvalue->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_value_final_in(SqxcValue* xcvalue)
{
//	if (xcvalue->instance)
//		sq_type_final_instance(xcvalue->current, &xcvalue->instance, true);
//	sqxc_final(xcvalue);
}

/* ----------------------------------------------------------------------------
   source of output chain

	C instance ---> SqxcValue ---> SQXC_TYPE_xxxx
	           send           send

	SqxcValue ---> SqxcJsonc ---> SqxcSqlite
	   src --------> middle -------> dest

	SqxcValue ---> SqxcSqlite
	   src ---------> dest
 */

static int  sqxc_value_send_out(SqxcValue* xcvalue, Sqxc* src)
{
	SqType*    type;

	if (xcvalue->instance == NULL)
		xcvalue->instance = src->value.pointer;
	if (xcvalue->current == NULL)
		return (src->code = SQCODE_NO_ELEMENT_TYPE);
	type = xcvalue->current;

	xcvalue->field = NULL;
	src->code = type->write(xcvalue->instance, type, (Sqxc*)xcvalue);
	return src->code;
}

static int  sqxc_value_ctrl_out(SqxcValue* xcvalue, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		if (xcvalue->container)
			xcvalue->current = xcvalue->container;
		else
			xcvalue->current = xcvalue->element;
		break;

	case SQXC_CTRL_FINISH:
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}


static void  sqxc_value_init_out(SqxcValue* xcvalue)
{
//	memset(xcvalue, 0, sizeof(SqxcValue));
//	sqxc_init(xcvalue);
	xcvalue->ctrl = (SqxcCtrlFunc)sqxc_value_ctrl_out;
	xcvalue->send = (SqxcSendFunc)sqxc_value_send_out;
	xcvalue->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_value_final_out(SqxcValue* xcvalue)
{
}

/* ----------------------------------------------------------------------------
   C to/from
   SQXC_INFO_VALUE[0] for Output
   SQXC_INFO_VALUE[1] for Input
 */
const SqxcInfo SQXC_INFO_VALUE[2] =
{
	{sizeof(SqxcValue), (SqInitFunc)sqxc_value_init_out, (SqFinalFunc)sqxc_value_final_out},
	{sizeof(SqxcValue), (SqInitFunc)sqxc_value_init_in,  (SqFinalFunc)sqxc_value_final_in},
};
