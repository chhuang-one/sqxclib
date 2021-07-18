/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <SqConfig.h>
#include <SqError.h>
#include <SqxcUnknown.h>

#ifdef SQ_CONFIG_SQXC_UNKNOWN_SKIP

// ----------------------------------------------------------------------------
// SqxcInfo functions

static int  sqxc_unknown_send(SqxcUnknown *xcunknown, Sqxc *args_src)
{
	switch (args_src->type) {
	case SQXC_TYPE_OBJECT:
	case SQXC_TYPE_ARRAY:
		xcunknown->nested_count++;
		break;

	case SQXC_TYPE_OBJECT_END:
	case SQXC_TYPE_ARRAY_END:
		xcunknown->nested_count--;
		break;

	default:
		break;
	}

	return SQCODE_OK;
}

static int  sqxc_unknown_ctrl(SqxcUnknown *xcunknown, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
	case SQXC_CTRL_FINISH:
		sqxc_steal(xcunknown->dest, (Sqxc*)xcunknown);
		sqxc_free((Sqxc*)xcunknown);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_unknown_init(SqxcUnknown *xcunknown)
{
//	memset(xcunknown, 0, sizeof(SqxcUnknown));
	xcunknown->supported_type = SQXC_TYPE_ALL;
	xcunknown->nested_count = 1;
	xcunknown->code = SQCODE_NOT_SUPPORT;    // SQCODE_OK;

#ifndef NDEBUG
	fprintf(stderr, "sqxc_unknown_init(): create SqxcUnknown. Start of unknown data.\n");
#endif
}

static void  sqxc_unknown_final(SqxcUnknown *xcunknown)
{
#ifndef NDEBUG
	fprintf(stderr, "sqxc_unknown_final(): destroy SqxcUnknown. End of unknown data.\n");
#endif
}

// ----------------------------------------------------------------------------
// SqxcInfo

static const SqxcInfo sqxc_info_unknown =
{
	sizeof(SqxcUnknown),
	(SqInitFunc)sqxc_unknown_init,
	(SqFinalFunc)sqxc_unknown_final,
	(SqxcCtrlFunc)sqxc_unknown_ctrl,
	(SqxcSendFunc)sqxc_unknown_send,
};

const SqxcInfo *SQXC_INFO_UNKNOWN = &sqxc_info_unknown;

#endif  // SQ_CONFIG_SQXC_UNKNOWN_SKIP
