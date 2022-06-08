/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <SqError.h>
#include <SqxcFile.h>

/* ----------------------------------------------------------------------------
	*** In output chain:
	SQXC_TYPE_STRING ---> SqxcFile Writer
 */

static int  sqxc_file_writer_send(SqxcFile *xcfile, Sqxc *src)
{
	if (src->type != SQXC_TYPE_STRING) {
//		src->required_type = SQXC_TYPE_STRING;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	fputs(src->value.string, xcfile->file);
	return (src->code = SQCODE_OK);
}

static int  sqxc_file_writer_ctrl(SqxcFile *xcfile, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		if (xcfile->filename)
			xcfile->file = fopen(xcfile->filename, "w");
		break;

	case SQXC_CTRL_FINISH:
		if (xcfile->file)
			fclose(xcfile->file);
		xcfile->file = NULL;
		// Because SqxcFile never use SqxcNested, it doesn't need to clear SqxcNested stack.
//		sqxc_clear_nested((Sqxc*)xcfile);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_file_writer_init(SqxcFile *xcfile)
{
	sq_buffer_resize(sqxc_get_buffer(xcfile), 4096);
	xcfile->file = NULL;
	xcfile->filename = NULL;
}

static void  sqxc_file_writer_final(SqxcFile *xcfile)
{
	if (xcfile->file)
		fclose(xcfile->file);
}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo SqxcInfo_FileWriter_ =
{
	sizeof(SqxcFile),
	(SqInitFunc)sqxc_file_writer_init,
	(SqFinalFunc)sqxc_file_writer_final,
	(SqxcCtrlFunc)sqxc_file_writer_ctrl,
	(SqxcSendFunc)sqxc_file_writer_send,
};
