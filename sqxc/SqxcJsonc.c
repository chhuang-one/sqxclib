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
#include <SqField.h>
#include <SqxcJsonc.h>

/* ----------------------------------------------------------------------------
   Middleware of input chain

	SQXC_TYPE_STRING ---> SqxcJsonc ---> SQXC_TYPE_xxxx
	 (JSON string)   send           send

	SqxcSqlite ---> SqxcJsonc ---> SqxcValue
	   src ---------> middle -------> dest

	SqxcJsonc ---> SqxcValue
	   src ---------> dest
 */

// send cx data from xcjson to cxvalue
static int  sqxc_jsonc_send_value_in(SqxcJsonc* xcjson, const char* name, json_object* value)
{
	json_object*  jelement;
	int  idx, len;

	switch(json_object_get_type(value)) {
	case json_type_null:
//		xcjson->type = SQXC_TYPE_POINTER;
		xcjson->type = SQXC_TYPE_STRING;
		sqxc_send_string(xcjson, name, NULL);
		break;

	case json_type_boolean:
		xcjson->type = SQXC_TYPE_BOOL;
		sqxc_send_bool(xcjson, name, json_object_get_boolean(value));
		break;

	case json_type_int:
		xcjson->type = SQXC_TYPE_INT;
		sqxc_send_int(xcjson, name, json_object_get_int(value));
		break;

	case json_type_double:
		xcjson->type = SQXC_TYPE_INT64;
		sqxc_send_double(xcjson, name, json_object_get_double(value));
		break;

	case json_type_string:
		xcjson->type = SQXC_TYPE_STRING;
		sqxc_send_string(xcjson, name, json_object_get_string(value));
		break;

	case json_type_object:
		xcjson->type = SQXC_TYPE_OBJECT;
		sqxc_send_object_beg(xcjson, name);
		json_object_object_foreach(value, fname, fvalue)
			sqxc_jsonc_send_value_in(xcjson, fname, fvalue);
		sqxc_send_object_end(xcjson, name);
		break;

	case json_type_array:
		xcjson->type = SQXC_TYPE_ARRAY;
		len = json_object_array_length(value);
		sqxc_send_array_beg(xcjson, name);
		for (idx = 0;  idx < len;  idx++) {
			// send array elements
			jelement = json_object_array_get_idx(value, idx);
			sqxc_jsonc_send_value_in(xcjson, NULL, jelement);
		}
		sqxc_send_array_end(xcjson, name);
		break;
	}

	return xcjson->code;
}

static int  sqxc_jsonc_send_in(SqxcJsonc* xcjson, Sqxc* src)
{
	json_object* jobject;

	jobject = json_tokener_parse(src->value.string);
	if (jobject == NULL) {
		// incomplete JSON data
		if (src->type == SQXC_TYPE_NONE) {
			// JSON stream (completed in future)
			return (src->code = SQCODE_OK);
		}
		return (src->code = SQCODE_UNCOMPLETED_JSON);
	}

	// send cx data from xcjson to dest
	// result code to src
	return (src->code = sqxc_jsonc_send_value_in(xcjson, src->name, jobject));
}

static int  sqxc_jsonc_ctrl_in(SqxcJsonc* xcjson, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		break;

	case SQXC_CTRL_FINISH:
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_in(SqxcJsonc* xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->ctrl = (SqxcCtrlFunc)sqxc_jsonc_ctrl_in;
	xcjson->send = (SqxcSendFunc)sqxc_jsonc_send_in;
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_jsonc_final_in(SqxcJsonc* xcjson)
{

}

/* ----------------------------------------------------------------------------
   Middleware of output chain

	SQXC_TYPE_xxxx ---> SqxcJsonc ---> SQXC_TYPE_STRING
	               send           send   (JSON string)

	SqxcValue ---> SqxcJsonc ---> SqxcSqlite
	   src --------> middle -------> dest

	SqxcValue ---> SqxcJsonc
	   src --------> dest
 */

static int  sqxc_jsonc_send_out(SqxcJsonc* xcjson, Sqxc* src)
{
	SqxcNested*   nested;
	json_object*  jobject;

	if (src->field && src->field->bit_field & SQB_HIDDEN)
		return (src->code = SQCODE_OK);

	switch(src->type) {
	case SQXC_TYPE_BOOL:
		jobject = json_object_new_boolean(src->value.boolean);
		break;

	case SQXC_TYPE_INT:
		jobject = json_object_new_int(src->value.integer);
		break;

	case SQXC_TYPE_INT64:
		jobject = json_object_new_int64(src->value.int64);
		break;

	case SQXC_TYPE_DOUBLE:
		jobject = json_object_new_double(src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string) {
			if (src->field && src->field->bit_field & SQB_HIDDEN_NULL)
				return (src->code = SQCODE_OK);
		}
		jobject = json_object_new_string(src->value.string);
		break;

	case SQXC_TYPE_OBJECT:
		jobject = json_object_new_object();
		break;

	case SQXC_TYPE_ARRAY:
		jobject = json_object_new_array();
		break;

	case SQXC_TYPE_OBJECT_END:
	case SQXC_TYPE_ARRAY_END:
		sqxc_pop_nested((Sqxc*)xcjson);
		xcjson->jcur = xcjson->nested->data;;
		xcjson->jcur_type = (intptr_t)xcjson->nested->data2;
		goto check_nested_0;
//		break;

	default:
		return (src->code = SQCODE_TYPE_NOT_SUPPORT);
	}

	if (xcjson->jroot == NULL) {
		xcjson->jroot = jobject;
		xcjson->jroot_name = src->name;
	}
	else if (xcjson->jcur_type == SQXC_TYPE_ARRAY)
		json_object_array_add(xcjson->jcur, jobject);
	else if (xcjson->jcur_type == SQXC_TYPE_OBJECT)
		json_object_object_add(xcjson->jcur, src->name, jobject);

	if (src->type & (SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY)) {
		nested = sqxc_push_nested((Sqxc*)xcjson);
		nested->data = jobject;
		nested->data2 = (void*)(intptr_t) src->type;
		xcjson->jcur = jobject;
		xcjson->jcur_type = src->type;
	}

check_nested_0:
	if (xcjson->nested_count == 0) {
		sqxc_send_string(xcjson, xcjson->jroot_name,
		                 json_object_to_json_string(xcjson->jroot));
	}

	return (src->code = SQCODE_OK);
}

static int  sqxc_jsonc_ctrl_out(SqxcJsonc* xcjson, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcjson->jroot = NULL;
		xcjson->jcur = NULL;
		xcjson->jcur_type = 0;
		break;

	case SQXC_CTRL_FINISH:
		if (xcjson->jroot) {
			json_object_put(xcjson->jroot);
			xcjson->jroot = NULL;
		}
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_out(SqxcJsonc* xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->ctrl = (SqxcCtrlFunc)sqxc_jsonc_ctrl_out;
	xcjson->send = (SqxcSendFunc)sqxc_jsonc_send_out;
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_jsonc_final_out(SqxcJsonc* xcjson)
{

}

/* ----------------------------------------------------------------------------
   C to/from json-c
   SQXC_INFO_JSONC[0] for Output
   SQXC_INFO_JSONC[1] for Input
 */
const SqxcInfo SQXC_INFO_JSONC[2] =
{
	{sizeof(SqxcJsonc), (SqInitFunc)sqxc_jsonc_init_out, (SqFinalFunc)sqxc_jsonc_final_out},
	{sizeof(SqxcJsonc), (SqInitFunc)sqxc_jsonc_init_in,  (SqFinalFunc)sqxc_jsonc_final_in},
};
