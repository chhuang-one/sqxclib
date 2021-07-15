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

#include <limits.h>     // __WORDSIZE
#include <stdint.h>     // __WORDSIZE (Apple)
#include <stdio.h>

#include <SqError.h>
#include <SqxcEmpty.h>

#ifdef _MSC_VER
#ifdef _WIN64
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif  // _WIN64
#endif  // _MSC_VER

// ----------------------------------------------------------------------------
// SqxcInfo functions

static int  sqxc_empty_send(SqxcEmpty *xcempty, Sqxc *args_src)
{
	if (args_src->type & xcempty->not_matched_type) {
//		src->required_type = args_src->type;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return SQCODE_TYPE_NOT_MATCH;
	}

	switch (args_src->type) {
	case SQXC_TYPE_BOOL:
		printf("%s  %2d  BOOL  %s = %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.boolean ? "true" : "false");
		break;

	case SQXC_TYPE_INT:
		printf("%s  %2d  INT  %s = %d\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.integer);
		break;

	case SQXC_TYPE_INT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		printf("%s  %2d  INT64  %s = %I64d\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.int64);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		printf("%s  %2d  INT64  %s = %ld\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.int64);
#else
		printf("%s  %2d  INT64  %s = %lld\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       (long long int)args_src->value.int64);
#endif
		break;

	case SQXC_TYPE_DOUBLE:
		printf("%s  %2d  DOUBLE  %s = %lf\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		printf("%s  %2d  STRING  %s = %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.string);
		break;

	case SQXC_TYPE_OBJECT:
		xcempty->nested_count++;
		printf("%s  %2d  OBJECT  %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name);
		break;

	case SQXC_TYPE_OBJECT_END:
		xcempty->nested_count--;
		printf("%s  %2d  OBJECT_END  %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name);
		break;

	case SQXC_TYPE_ARRAY:
		xcempty->nested_count++;
		printf("%s  %2d  ARRAY  %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name);
		break;

	case SQXC_TYPE_ARRAY_END:
		xcempty->nested_count--;
		printf("%s  %2d  ARRAY_END  %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name);
		break;

	default:
		break;
	}

	// send result to xcempty->dest if xcempty->nested_count is 0
	if (xcempty->nested_count == 0 && xcempty->send_to_dest_if_no_nested) {
		if (xcempty != (SqxcEmpty*)xcempty->dest && xcempty->dest) {
			xcempty->type = args_src->type;
			xcempty->name = args_src->name;
			memcpy(&xcempty->value, &args_src->value, sizeof(xcempty->value));
			xcempty->entry = args_src->entry;
			xcempty->info->send(xcempty->dest, (Sqxc*)xcempty);
		}
	}

	return (args_src->code = SQCODE_OK);
}

static int  sqxc_empty_ctrl(SqxcEmpty *xcempty, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcempty->nested_count = xcempty->nested_count_when_ready;
		printf("%s  " "SqxcEmpty: ready" "\n",
		       xcempty->tag ? xcempty->tag : "");
		break;

	case SQXC_CTRL_FINISH:
		xcempty->nested_count = 0;
		printf("%s  " "SqxcEmpty: finish" "\n",
		       xcempty->tag ? xcempty->tag : "");
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcempty);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_empty_init(SqxcEmpty *xcempty)
{
//	memset(xcempty, 0, sizeof(SqxcEmpty));
	xcempty->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_empty_final(SqxcEmpty *xcempty)
{

}

// ----------------------------------------------------------------------------
// SqxcInfo

static const SqxcInfo sqxc_info_empty =
{
	sizeof(SqxcEmpty),
	(SqInitFunc)sqxc_empty_init,
	(SqFinalFunc)sqxc_empty_final,
	(SqxcCtrlFunc)sqxc_empty_ctrl,
	(SqxcSendFunc)sqxc_empty_send,
};

const SqxcInfo *SQXC_INFO_EMPTY = &sqxc_info_empty;
