/*
 *   Copyright (C) 2025-2026 by C.H. Huang
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

#include <limits.h>            // __WORDSIZE
#include <stdint.h>            // __WORDSIZE  for Apple Developer
#include <stdio.h>             // fprintf(), stderr

#include <sqxc/SqError.h>
#include <sqxc/SqEntry.h>
#include <sqxc/SqxcCjson.h>

#if SQ_CONFIG_HAVE_CJSON       // defined in SqConfig.h

/* ----------------------------------------------------------------------------
	SqxcInfo functions - Middleware of input chain

	(JSON string)
	SQXC_TYPE_STR ---> SqxcCjson Parser ---> SQXC_TYPE_xxxx
 */

// send data(arguments) from SqxcCjson(source) to SqxcValue(destination)
static int  sqxc_cjson_send_value_in(SqxcCjson *xcjson, cJSON *cur)
{
	Sqxc *xcdest = xcjson->dest;
	const char *name = cur->string;

	switch(cur->type) {
	case cJSON_NULL:
		xcjson->type = SQXC_TYPE_NULL;
		xcjson->name = name;
		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_True:
		xcjson->type = SQXC_TYPE_BOOL;
		xcjson->name = name;
		xcjson->value.boolean = true;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_False:
		xcjson->type = SQXC_TYPE_BOOL;
		xcjson->name = name;
		xcjson->value.boolean = false;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_Number:
		// integer or float?
		xcjson->value.int64 = (int64_t)cur->valuedouble;
		if (xcjson->value.int64 == cur->valuedouble) {
			// it is integer
#if defined(__WORDSIZE) && (__WORDSIZE == 64)
			// integer is 64-bit
			xcjson->type = SQXC_TYPE_INT;
#else
			// integer is 32-bit
			if (xcjson->value.int64 > INT32_MAX || xcjson->value.int64 < INT32_MIN)
				xcjson->type = SQXC_TYPE_INT64;
			else {
				xcjson->type = SQXC_TYPE_INT;
				xcjson->value.integer = (int)xcjson->value.int64;
//				xcjson->value.integer = cur->valueint;
			}
#endif
		}
		else {
			// it is float
			xcjson->type = SQXC_TYPE_DOUBLE;
			xcjson->value.double_ = cur->valuedouble;
		}
		xcjson->name = name;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_String:
		xcjson->type = SQXC_TYPE_STR;
		xcjson->name = name;
		xcjson->value.str = cur->valuestring;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_Object:
		xcjson->type = SQXC_TYPE_OBJECT;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		for (cur = cur->child;  cur;  cur = cur->next)
			sqxc_cjson_send_value_in(xcjson, cur);
		xcjson->type = SQXC_TYPE_OBJECT_END;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;

	case cJSON_Array:
		xcjson->type = SQXC_TYPE_ARRAY;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		for (cur = cur->child;  cur;  cur = cur->next)
			sqxc_cjson_send_value_in(xcjson, cur);
		xcjson->type = SQXC_TYPE_ARRAY_END;
		xcjson->name = name;
//		xcjson->value.pointer = NULL;
		xcdest->info->send(xcdest, (Sqxc*)xcjson);
		break;
	}

	return xcjson->code;
}

static int  sqxc_cjson_send_in(SqxcCjson *xcjson, Sqxc *src)
{
#if 0
	if (src->type != SQXC_TYPE_STR) {
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_STR;
		*/
		return (src->code = SQCODE_TYPE_NOT_SUPPORTED);
	}
#endif

	xcjson->jRoot = cJSON_Parse(src->value.str);
	if (xcjson->jRoot == NULL)
		return (src->code = SQCODE_JSON_ERROR);

	// send xc data from xcjson to dest
	// result code to src
	return (src->code = sqxc_cjson_send_value_in(xcjson, xcjson->jRoot));
}

static int  sqxc_cjson_ctrl_in(SqxcCjson *xcjson, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		break;

	case SQXC_CTRL_FINISH:
		// cJSON: delete the root and its child objects
		cJSON_Delete(xcjson->jRoot);
		xcjson->jRoot = NULL;
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcjson);
		break;

	default:
		return SQCODE_NOT_SUPPORTED;
	}

	return SQCODE_OK;
}

static void  sqxc_cjson_init_in(SqxcCjson *xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcCjson));
	xcjson->supported_type = SQXC_TYPE_STR;

	// cJSON: initial the root object
	xcjson->jRoot = NULL;
}

static void  sqxc_cjson_final_in(SqxcCjson *xcjson)
{
	// cJSON: delete the root and its child objects
	cJSON_Delete(xcjson->jRoot);
}

/* ----------------------------------------------------------------------------
   Middleware of output chain

	SQXC_TYPE_xxxx ---> SqxcCjson Writer ---> SQXC_TYPE_STR
	                                          (JSON string)
 */

static int  sqxc_cjson_send_out(SqxcCjson *xcjson, Sqxc *src)
{
	SqxcNested   *nested;
	cJSON        *jObject;
	int  code;    // used by check_nested_0

	if (src->entry && src->entry->bit_field & SQB_HIDDEN)
		return (src->code = SQCODE_OK);

	switch(src->type) {
	case SQXC_TYPE_NULL:
		jObject = cJSON_CreateNull();

	case SQXC_TYPE_BOOL:
		if (src->value.boolean == false)
			jObject = cJSON_CreateFalse();
		else
			jObject = cJSON_CreateTrue();
		break;

	case SQXC_TYPE_INT:
		jObject = cJSON_CreateNumber((double)src->value.integer);
		break;

	case SQXC_TYPE_UINT:
		jObject = cJSON_CreateNumber((double)src->value.uint);
		break;

	case SQXC_TYPE_INT64:
		jObject = cJSON_CreateNumber((double)src->value.int64);
		break;

	case SQXC_TYPE_UINT64:
		jObject = cJSON_CreateNumber((double)src->value.uint64);
		break;

	case SQXC_TYPE_TIME:
		jObject = cJSON_CreateNumber((double)src->value.rawtime);
		break;

	case SQXC_TYPE_DOUBLE:
		jObject = cJSON_CreateNumber(src->value.double_);
		break;

	case SQXC_TYPE_STR:
	case SQXC_TYPE_RAW:
		if (src->value.str == NULL) {
			if (src->entry && src->entry->bit_field & SQB_HIDDEN_NULL)
				return (src->code = SQCODE_OK);
			jObject = cJSON_CreateNull();
		}
		else
			jObject = cJSON_CreateString(src->value.str);
		break;

	case SQXC_TYPE_OBJECT:
		jObject = cJSON_CreateObject();
		break;

	case SQXC_TYPE_ARRAY:
		jObject = cJSON_CreateArray();
		break;

	case SQXC_TYPE_OBJECT_END:
	case SQXC_TYPE_ARRAY_END:
		sqxc_pop_nested((Sqxc*)xcjson);
		xcjson->jNested = (cJSON*)xcjson->nested->data;
		xcjson->jNestedXcType = (SqxcType)(uintptr_t)xcjson->nested->data2;
		goto check_nested_0;
//		break;

	default:
//		jObject = cJSON_CreateNull();
		return (src->code = SQCODE_TYPE_NOT_SUPPORTED);
	}

	if (xcjson->jRoot == NULL) {
		xcjson->jRoot = jObject;
		xcjson->jRootName = src->name;
	}
	else if (xcjson->jNestedXcType == SQXC_TYPE_ARRAY)
		cJSON_AddItemToArray(xcjson->jNested, jObject);
	else if (xcjson->jNestedXcType == SQXC_TYPE_OBJECT)
		cJSON_AddItemToObject(xcjson->jNested, src->name, jObject);
	else {
#ifndef NDEBUG
		fprintf(stderr, "%s: Error! Only JSON objects/arrays can add new items.", "SqxcCjson");
#endif
		// cJSON: delete 'jObject'
		cJSON_Delete(jObject);
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
		xcjson->value.str = cJSON_PrintUnformatted(xcjson->jRoot);
		code = xcdest->info->send(xcdest, (Sqxc*)xcjson);
		// Release the string returned by cJSON_PrintUnformatted()
		cJSON_free((char*)xcjson->value.str);
		// End of JSON string
		cJSON_Delete(xcjson->jRoot);
		xcjson->jRoot = NULL;
	}

	return (src->code = code);
}

static int  sqxc_cjson_ctrl_out(SqxcCjson *xcjson, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		// This avoid memory leaks if developer uses SqxcCjson incorrectly.
		// cJSON: delete the root and its child objects
		if (xcjson->jRoot)
			cJSON_Delete(xcjson->jRoot);

		xcjson->jRoot = NULL;
		xcjson->jNested = NULL;
		xcjson->jNestedXcType = SQXC_TYPE_UNKNOWN;
		break;

	case SQXC_CTRL_FINISH:
		// cJSON: delete the root and its child objects
		if (xcjson->jRoot) {
			cJSON_Delete(xcjson->jRoot);
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

static void  sqxc_cjson_init_out(SqxcCjson *xcjson)
{
//	memset(xcjson, 0, sizeof(SqxcCjson));
	xcjson->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_cjson_final_out(SqxcCjson *xcjson)
{
	// cJSON: delete the root and its child objects
	if (xcjson->jRoot)
		cJSON_Delete(xcjson->jRoot);
}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo sqxcInfo_CjsonParser =
{
	sizeof(SqxcCjson),
	(SqInitFunc)sqxc_cjson_init_in,
	(SqFinalFunc)sqxc_cjson_final_in,
	(SqxcCtrlFunc)sqxc_cjson_ctrl_in,
	(SqxcSendFunc)sqxc_cjson_send_in,
};

const SqxcInfo sqxcInfo_CjsonWriter =
{
	sizeof(SqxcCjson),
	(SqInitFunc)sqxc_cjson_init_out,
	(SqFinalFunc)sqxc_cjson_final_out,
	(SqxcCtrlFunc)sqxc_cjson_ctrl_out,
	(SqxcSendFunc)sqxc_cjson_send_out,
};

#endif  // SQ_CONFIG_HAVE_CJSON
