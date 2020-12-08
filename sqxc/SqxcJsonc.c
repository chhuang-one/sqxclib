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
#include <SqEntry.h>
#include <SqxcJsonc.h>

#ifdef SQ_CONFIG_JSON_SUPPORT

/* ----------------------------------------------------------------------------
	SqxcInfo functions - Middleware of input chain

	  (JSON string)
	SQXC_TYPE_STRING ---> SqxcJsonc Parser ---> SQXC_TYPE_xxxx
           or
	SQXC_TYPE_STREAM
 */

// send data(arguments) from SqxcJsonc(source) to SqxcValue(destination)
static int  sqxc_jsonc_send_value_in(SqxcJsonc* xcjson, const char* name, json_object* value)
{
	json_object*  jelement;
	Sqxc* xcdest = xcjson->dest;
	int   idx, len;

	switch(json_object_get_type(value)) {
	case json_type_null:
		xcjson->type = SQXC_TYPE_STRING;
		xcjson->name = name;
		xcjson->value.string = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_boolean:
		xcjson->type = SQXC_TYPE_BOOL;
		xcjson->name = name;
		xcjson->value.boolean = json_object_get_boolean(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_int:
		xcjson->type = SQXC_TYPE_INT;
		xcjson->name = name;
		xcjson->value.integer = json_object_get_int(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_double:
		xcjson->type = SQXC_TYPE_DOUBLE;
		xcjson->name = name;
		xcjson->value.double_ = json_object_get_double(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_string:
		xcjson->type = SQXC_TYPE_STRING;
		xcjson->name = name;
		xcjson->value.string = (char*)json_object_get_string(value);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_object:
		xcjson->type = SQXC_TYPE_OBJECT;
		xcjson->name = name;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		{	// this can avoid many problem...
			json_object_object_foreach(value, fname, fvalue)
				sqxc_jsonc_send_value_in(xcjson, fname, fvalue);
		}
		xcjson->type = SQXC_TYPE_OBJECT_END;
		xcjson->name = name;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case json_type_array:
		xcjson->type = SQXC_TYPE_ARRAY;
		xcjson->name = name;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);

		len = json_object_array_length(value);
		for (idx = 0;  idx < len;  idx++) {
			// send array elements
			jelement = json_object_array_get_idx(value, idx);
			sqxc_jsonc_send_value_in(xcjson, NULL, jelement);
		}

		xcjson->type = SQXC_TYPE_ARRAY_END;
		xcjson->name = name;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
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
		if (src->type == SQXC_TYPE_STREAM) {
			// JSON stream (completed in future)
			return (src->code = SQCODE_OK);
		}
		return (src->code = SQCODE_UNCOMPLETED_JSON);
	}

	// send xc data from xcjson to dest
	// result code to src
	return (src->code = sqxc_jsonc_send_value_in(xcjson, src->name, jobject));
}

static int  sqxc_jsonc_ctrl_in(SqxcJsonc* xcjson, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		break;

	case SQXC_CTRL_FINISH:
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcjson);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_in(SqxcJsonc* xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_jsonc_final_in(SqxcJsonc* xcjson)
{

}

/* ----------------------------------------------------------------------------
   Middleware of output chain

	SQXC_TYPE_xxxx ---> SqxcJsonc Writer ---> SQXC_TYPE_STRING
	                                            (JSON string)
 */

static int  sqxc_jsonc_send_out(SqxcJsonc* xcjson, Sqxc* src)
{
	SqxcNested*   nested;
	json_object*  jobject;

	if (src->entry && src->entry->bit_field & SQB_HIDDEN)
		return (src->code = SQCODE_OK);

	switch(src->type) {
	case SQXC_TYPE_BOOL:
		jobject = json_object_new_boolean(src->value.boolean);
		break;

	case SQXC_TYPE_INT:
		jobject = json_object_new_int(src->value.integer);
		break;

	case SQXC_TYPE_UINT:
		jobject = json_object_new_uint64(src->value.uint);
		break;

	case SQXC_TYPE_INT64:
		jobject = json_object_new_int64(src->value.int64);
		break;

	case SQXC_TYPE_UINT64:
		jobject = json_object_new_uint64(src->value.uint64);
		break;

	case SQXC_TYPE_DOUBLE:
		jobject = json_object_new_double(src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string) {
			if (src->entry && src->entry->bit_field & SQB_HIDDEN_NULL)
				return (src->code = SQCODE_OK);
		}
		if (src->value.string == NULL)
			jobject = json_object_new_null();
		else
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
		xcjson->jcur = (json_object*)xcjson->nested->data;
		xcjson->jcur_type = (SqxcType)(intptr_t)xcjson->nested->data2;
		goto check_nested_0;
//		break;

	default:
//		jobject = json_object_new_null();
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
		Sqxc* xcdest = xcjson->dest;
		xcjson->type = SQXC_TYPE_STRING;
		xcjson->name = xcjson->jroot_name;
		xcjson->value.string = (char*)json_object_to_json_string(xcjson->jroot);
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		// End of JSON string
		json_object_put(xcjson->jroot);
		xcjson->jroot = NULL;
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
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcjson);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_jsonc_init_out(SqxcJsonc* xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcJsonc));
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_jsonc_final_out(SqxcJsonc* xcjson)
{

}

// ----------------------------------------------------------------------------
// SqxcInfo

static const SqxcInfo sqxc_jsonc_parser =
{
	sizeof(SqxcJsonc),
	(SqInitFunc)sqxc_jsonc_init_in,
	(SqFinalFunc)sqxc_jsonc_final_in,
	(SqxcCtrlFunc)sqxc_jsonc_ctrl_in,
	(SqxcSendFunc)sqxc_jsonc_send_in,
};

static const SqxcInfo sqxc_jsonc_writer =
{
	sizeof(SqxcJsonc),
	(SqInitFunc)sqxc_jsonc_init_out,
	(SqFinalFunc)sqxc_jsonc_final_out,
	(SqxcCtrlFunc)sqxc_jsonc_ctrl_out,
	(SqxcSendFunc)sqxc_jsonc_send_out,
};

const SqxcInfo *SQXC_INFO_JSONC_PARSER = &sqxc_jsonc_parser;
const SqxcInfo *SQXC_INFO_JSONC_WRITER = &sqxc_jsonc_writer;

#endif  // SQ_CONFIG_JSON_SUPPORT
