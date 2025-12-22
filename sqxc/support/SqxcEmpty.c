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

#include <limits.h>       // __WORDSIZE
#include <stdint.h>       // __WORDSIZE  for Apple Developer
#include <stdio.h>        // printf()
#include <inttypes.h>     // PRId64, PRIu64

#include <sqxc/SqError.h>
#include <sqxc/support/SqxcEmpty.h>

// ----------------------------------------------------------------------------
// SqxcInfo functions

static int  sqxc_empty_send(SqxcEmpty *xcempty, Sqxc *args_src)
{
	if (args_src->type & xcempty->not_matched_type) {
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = args_src->type;
		*/
		return SQCODE_TYPE_NOT_MATCHED;
	}

	switch (args_src->type) {
	case SQXC_TYPE_RAW:
		printf("%s  %2d  RAW  %s = %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.raw);
		break;

	case SQXC_TYPE_NULL:
		printf("%s  %2d  NULL  %s = %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       "NULL");
		break;

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

	case SQXC_TYPE_UINT:
		printf("%s  %2d  INT  %s = %u\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.uint);
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
#elif defined(__GNUC__)
		printf("%s  %2d  INT64  %s = %lld\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.int64);
#else // C99
		printf("%s  %2d  INT64  %s = %" PRId64 "\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.int64);
#endif
		break;

	case SQXC_TYPE_UINT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		printf("%s  %2d  UINT64  %s = %I64u\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.uint64);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		printf("%s  %2d  UINT64  %s = %lu\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.uint64);
#elif defined(__GNUC__)
		printf("%s  %2d  UINT64  %s = %llu\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.uint64);
#else // C99
		printf("%s  %2d  UINT64  %s = %" PRIu64 "\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.uint64);
#endif
		break;

	case SQXC_TYPE_DOUBLE:
		printf("%s  %2d  DOUBLE  %s = %lf\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.double_);
		break;

	case SQXC_TYPE_STR:
		printf("%s  %2d  STRING  %s = %s\n",
		       xcempty->tag ? xcempty->tag : "",
		       xcempty->nested_count,
		       args_src->name,
		       args_src->value.str);
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
		return SQCODE_NOT_SUPPORTED;
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

const SqxcInfo sqxcInfo_Empty =
{
	sizeof(SqxcEmpty),
	(SqInitFunc)sqxc_empty_init,
	(SqFinalFunc)sqxc_empty_final,
	(SqxcCtrlFunc)sqxc_empty_ctrl,
	(SqxcSendFunc)sqxc_empty_send,
};
