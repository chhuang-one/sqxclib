/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#include <stdio.h>        // fprintf(), stderr

#include <SqError.h>
#include <SqEntry.h>
#include <SqxcJsonc.h>

/* ----------------------------------------------------------------------------
	SqxcInfo functions - Middleware of input chain

	(JSON string)
	SQXC_TYPE_STR ---> SqxcJsonc Parser ---> SQXC_TYPE_xxxx
 */

// send data(arguments) from SqxcJsonc(source) to SqxcValue(destination)
static int  sqxc_jsonc_send_value_in(SqxcJsonc *xcjson, const char *name, json_object *value)
{
	json_object  *jelement;
	Sqxc *xcdest = xcjson->dest;
	size_t   idx, len;

	switch(json_object_get_type(value)) {
	case json_type_null:
		xcjson->type = SQXC_TYPE_NULL;
		xcjson->name = name;
		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_boolean:
		xcjson->type = SQXC_TYPE_BOOL;
		xcjson->name = name;
		xcjson->value.boolean = json_object_get_boolean(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_int:
		xcjson->name = name;
		xcjson->value.int64 = json_object_get_int64(value);
		if (xcjson->value.int64 > INT32_MAX || xcjson->value.int64 < INT32_MIN)
			xcjson->type = SQXC_TYPE_INT64;
		else {
			xcjson->type = SQXC_TYPE_INT;
			xcjson->value.integer = (int)xcjson->value.int64;
//			xcjson->value.integer = json_object_get_int(value);
		}
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_double:
		xcjson->type = SQXC_TYPE_DOUBLE;
		xcjson->name = name;
		xcjson->value.double_ = json_object_get_double(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_string:
		xcjson->type = SQXC_TYPE_STR;
		xcjson->name = name;
		xcjson->value.str = (char*)json_object_get_string(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_object:
		xcjson->type = SQXC_TYPE_OBJECT;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		{	// this can avoid many problem...
			json_object_object_foreach(value, fname, fvalue)
				sqxc_jsonc_send_value_in(xcjson, fname, fvalue);
		}
		xcjson->type = SQXC_TYPE_OBJECT_END;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_array:
		xcjson->type = SQXC_TYPE_ARRAY;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);

		len = json_object_array_length(value);
		for (idx = 0;  idx < len;  idx++) {
			jelement = json_object_array_get_idx(value, idx);
			// send jsonc array elements to Sqxc chain
			sqxc_jsonc_send_value_in(xcjson, NULL, jelement);
		}

		xcjson->type = SQXC_TYPE_ARRAY_END;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;
	}

	return xcjson->code;
}

static int  sqxc_jsonc_send_in(SqxcJsonc *xcjson, Sqxc *src)
{
	json_object *jObject;
	enum json_tokener_error  jerror;

#if 0
	if (src->type != SQXC_TYPE_STR) {
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_STR;
		*/
		return (src->code = SQCODE_TYPE_NOT_SUPPORTED);
	}
#endif

	// jsonc parser: If the parsing is incomplete, json_tokener_parse_ex() return NULL.
	jObject = json_tokener_parse_ex(xcjson->jTokener, src->value.str, -1);
	if (jObject == NULL) {
		jerror = json_tokener_get_error(xcjson->jTokener);
		// incomplete JSON string
		if (jerror == json_tokener_continue)
			return (src->code = SQCODE_JSON_CONTINUE);
		// parse failed
		return (src->code = SQCODE_JSON_ERROR);
	}
	xcjson->jRoot = jObject;

	// send xc data from xcjson to dest
	// result code to src
	return (src->code = sqxc_jsonc_send_value_in(xcjson, src->name, jObject));
}

static int  sqxc_jsonc_ctrl_in(SqxcJsonc *xcjson, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		break;

	case SQXC_CTRL_FINISH:
		// jsonc parser reset
		json_tokener_reset(xcjson->jTokener);
		// jsonc object decrease reference count
		if (xcjson->jRoot) {
			json_object_put(xcjson->jRoot);
			xcjson->jRoot = NULL;
		}
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcjson);
		break;

	default:
		return SQCODE_NOT_SUPPORTED;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_in(SqxcJsonc *xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->supported_type = SQXC_TYPE_STR;

	// jsonc parser
	xcjson->jTokener = json_tokener_new();
	xcjson->jRoot = NULL;
}

static void  sqxc_jsonc_final_in(SqxcJsonc *xcjson)
{
	// jsonc parser
	json_tokener_free(xcjson->jTokener);
	// jsonc object decrease reference count
	if (xcjson->jRoot)
		json_object_put(xcjson->jRoot);
}

/* ----------------------------------------------------------------------------
   Middleware of output chain

	SQXC_TYPE_xxxx ---> SqxcJsonc Writer ---> SQXC_TYPE_STR
	                                          (JSON string)
 */

static int  sqxc_jsonc_send_out(SqxcJsonc *xcjson, Sqxc *src)
{
	SqxcNested   *nested;
	json_object  *jObject;
	int  code;    // used by check_nested_0

	if (src->entry && src->entry->bit_field & SQB_HIDDEN)
		return (src->code = SQCODE_OK);

	switch(src->type) {
	case SQXC_TYPE_NULL:
		jObject = json_object_new_null();

	case SQXC_TYPE_BOOL:
		jObject = json_object_new_boolean(src->value.boolean);
		break;

	case SQXC_TYPE_INT:
		jObject = json_object_new_int(src->value.integer);
		break;

	case SQXC_TYPE_UINT:
		jObject = json_object_new_uint64(src->value.uint);
		break;

	case SQXC_TYPE_INT64:
		jObject = json_object_new_int64(src->value.int64);
		break;

	case SQXC_TYPE_UINT64:
		jObject = json_object_new_uint64(src->value.uint64);
		break;

	case SQXC_TYPE_TIME:
		jObject = json_object_new_int64(src->value.rawtime);
		break;

	case SQXC_TYPE_DOUBLE:
		jObject = json_object_new_double(src->value.double_);
		break;

	case SQXC_TYPE_STR:
	case SQXC_TYPE_RAW:
		if (src->value.str == NULL) {
			if (src->entry && src->entry->bit_field & SQB_HIDDEN_NULL)
				return (src->code = SQCODE_OK);
			jObject = json_object_new_null();
		}
		else
			jObject = json_object_new_string(src->value.str);
		break;

	case SQXC_TYPE_OBJECT:
		jObject = json_object_new_object();
		break;

	case SQXC_TYPE_ARRAY:
		jObject = json_object_new_array();
		break;

	case SQXC_TYPE_OBJECT_END:
	case SQXC_TYPE_ARRAY_END:
		sqxc_pop_nested((Sqxc*)xcjson);
		xcjson->jNested = (json_object*)xcjson->nested->data;
		xcjson->jNestedXcType = (SqxcType)(uintptr_t)xcjson->nested->data2;
		goto check_nested_0;
//		break;

	default:
//		jObject = json_object_new_null();
		return (src->code = SQCODE_TYPE_NOT_SUPPORTED);
	}

	if (xcjson->jRoot == NULL) {
		xcjson->jRoot = jObject;
		xcjson->jRootName = src->name;
	}
	else if (xcjson->jNestedXcType == SQXC_TYPE_ARRAY)
		json_object_array_add(xcjson->jNested, jObject);
	else if (xcjson->jNestedXcType == SQXC_TYPE_OBJECT)
		json_object_object_add(xcjson->jNested, src->name, jObject);
	else {
		json_object_put(jObject);
#ifndef NDEBUG
		fprintf(stderr, "%s: Error! Only JSON objects/arrays can add new items.", "SqxcJsonc");
#endif
	}

	if (src->type & (SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY)) {
		nested = sqxc_push_nested((Sqxc*)xcjson);
		nested->data = jObject;
		nested->data2 = (void*)(uintptr_t) src->type;
		xcjson->jNested = jObject;
		xcjson->jNestedXcType = src->type;
	}

check_nested_0:
	code = SQCODE_OK;

	if (xcjson->nested_count == 0) {
		Sqxc *xcdest = xcjson->dest;
		xcjson->type = SQXC_TYPE_STR;
		xcjson->name = xcjson->jRootName;
		xcjson->value.str = (char*)json_object_to_json_string(xcjson->jRoot);
		code = xcdest->info->send(xcdest, (Sqxc*)xcjson);
		// End of JSON string
		json_object_put(xcjson->jRoot);
		xcjson->jRoot = NULL;
	}

	return (src->code = code);
}

static int  sqxc_jsonc_ctrl_out(SqxcJsonc *xcjson, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcjson->jRoot = NULL;
		xcjson->jNested = NULL;
		xcjson->jNestedXcType = SQXC_TYPE_UNKNOWN;
		break;

	case SQXC_CTRL_FINISH:
		// jsonc object decrease reference count
		if (xcjson->jRoot) {
			json_object_put(xcjson->jRoot);
			xcjson->jRoot = NULL;
		}
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcjson);
		break;

	default:
		return SQCODE_NOT_SUPPORTED;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_out(SqxcJsonc *xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_jsonc_final_out(SqxcJsonc *xcjson)
{

}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo sqxcInfo_JsoncParser =
{
	sizeof(SqxcJsonc),
	(SqInitFunc)sqxc_jsonc_init_in,
	(SqFinalFunc)sqxc_jsonc_final_in,
	(SqxcCtrlFunc)sqxc_jsonc_ctrl_in,
	(SqxcSendFunc)sqxc_jsonc_send_in,
};

const SqxcInfo sqxcInfo_JsoncWriter =
{
	sizeof(SqxcJsonc),
	(SqInitFunc)sqxc_jsonc_init_out,
	(SqFinalFunc)sqxc_jsonc_final_out,
	(SqxcCtrlFunc)sqxc_jsonc_ctrl_out,
	(SqxcSendFunc)sqxc_jsonc_send_out,
};
