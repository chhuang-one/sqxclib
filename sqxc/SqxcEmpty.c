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

#include <stdio.h>

#include <SqError.h>
#include <SqxcEmpty.h>

/* ----------------------------------------------------------------------------
   source of input chain
 */

static int  sqxc_empty_send_in(SqxcEmpty* xcempty, Sqxc* src)
{
	switch (src->type) {
	case SQXC_TYPE_BOOL:
		printf("%*c BOOL %s = %s\n", xcempty->nested_count, ' ',
		       src->name, (src->value.boolean) ? "true" : "false");
		break;

	case SQXC_TYPE_INT:
		printf("%*c INT  %s = %d\n", xcempty->nested_count, ' ',
		       src->name, src->value.integer);
		break;

	case SQXC_TYPE_INT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		printf("%*c INT64  %s = %I64d\n", xcempty->nested_count, ' ',
		       src->name, src->value.int64);
#else
		printf("%*c INT64  %s = %lld\n", xcempty->nested_count, ' ',
		       src->name, src->value.int64);
#endif
		break;

	case SQXC_TYPE_DOUBLE:
		printf("%*c DOUBLE  %s = %lf\n", xcempty->nested_count, ' ',
		       src->name, src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		printf("%*c STRING  %s = %s\n", xcempty->nested_count, ' ',
		       src->name, src->value.string);
		break;

	case SQXC_TYPE_OBJECT:
		printf("%*c OBJECT  %s\n", xcempty->nested_count, ' ',
		       src->name);
		xcempty->nested_count++;
		break;

	case SQXC_TYPE_OBJECT_END:
		xcempty->nested_count--;
		printf("%*c OBJECT_END  %s\n", xcempty->nested_count, ' ',
		       src->name);
		break;

	case SQXC_TYPE_ARRAY:
		printf("%*c ARRAY  %s\n", xcempty->nested_count, ' ',
		       src->name);
		xcempty->nested_count++;
		break;

	case SQXC_TYPE_ARRAY_END:
		xcempty->nested_count--;
		printf("%*c ARRAY_END  %s\n", xcempty->nested_count, ' ',
		       src->name);
		break;

	default:
		break;
	}

	// send to dest
	if (xcempty != (SqxcEmpty*)xcempty->dest && xcempty->dest) {
		xcempty->type = src->type;
		xcempty->name = src->name;
		memcpy(&xcempty->value, &src->value, sizeof(xcempty->value));
		xcempty->entry = src->entry;
		xcempty->send(xcempty->dest, (Sqxc*)xcempty);
	}
	return (src->code = SQCODE_OK);
}

static int  sqxc_empty_ctrl_in(SqxcEmpty* xcempty, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcempty->nested_count = 1;
		puts("SqxcEmpty: ready");
		break;

	case SQXC_CTRL_FINISH:
		xcempty->nested_count = 0;
		puts("SqxcEmpty: finish");
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_empty_init_in(SqxcEmpty* xcempty)
{
//	memset(xcempty, 0, sizeof(SqxcEmpty));
	xcempty->ctrl = (SqxcCtrlFunc)sqxc_empty_ctrl_in;
	xcempty->send = (SqxcSendFunc)sqxc_empty_send_in;
	xcempty->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_empty_final_in(SqxcEmpty* xcempty)
{

}

// ----------------------------------------------------------------------------
// destination of output chain

/*
static int  sqxc_empty_send_out(SqxcEmpty* xcempty, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING)
		puts(src->value.string);
	return (src->code = SQCODE_OK);
}

static int  sqxc_empty_ctrl_out(SqxcEmpty* xcempty, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		puts("SqxcEmpty: ready");
		break;

	case SQXC_CTRL_FINISH:
		puts("SqxcEmpty: finish");
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_empty_init_out(SqxcEmpty* xcempty)
{
//	memset(xcempty, 0, sizeof(SqxcEmpty));
	xcempty->ctrl = (SqxcCtrlFunc)sqxc_empty_ctrl_in;
	xcempty->send = (SqxcSendFunc)sqxc_empty_send_in;
	xcempty->supported_type = SQXC_TYPE_ALL;
}

static void  sqxc_empty_final_out(SqxcEmpty* xcempty)
{

}
 */

/* ----------------------------------------------------------------------------
   C to/from empty
   SQXC_INFO_EMPTY[0] for Output
   SQXC_INFO_EMPTY[1] for Input
 */
const SqxcInfo SQXC_INFO_EMPTY[2] =
{
	{sizeof(SqxcEmpty), (SqInitFunc)sqxc_empty_init_in, (SqFinalFunc)sqxc_empty_final_in},
	{sizeof(SqxcEmpty), (SqInitFunc)sqxc_empty_init_in, (SqFinalFunc)sqxc_empty_final_in},
};
