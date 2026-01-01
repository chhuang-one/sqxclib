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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <sqxc/SqError.h>
#include <sqxc/support/SqxcMem.h>

/* ----------------------------------------------------------------------------
	*** In output chain:
	SQXC_TYPE_STR ---> SqxcMem Writer

	All output will write to SqxcMem::buf
 */

static int  sqxc_mem_writer_send(SqxcMem *xcmem, Sqxc *src)
{
	if (src->type != SQXC_TYPE_STR) {
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_STR;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
	}

	SqBuffer *buffer = sqxc_get_buffer(xcmem);
	sq_buffer_write(buffer, src->value.str);
	buffer->mem[buffer->writed] = 0;    // NULL-terminated

	return (src->code = SQCODE_OK);
}

static int  sqxc_mem_writer_ctrl(SqxcMem *xcmem, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcmem->buf_writed = 0;
		xcmem->buf[0] = 0;    // NULL-terminated
		break;

	case SQXC_CTRL_FINISH:
		// Because Sqxcmem never use SqxcNested, it doesn't need to clear SqxcNested stack.
//		sqxc_clear_nested((Sqxc*)xcmem);
		break;

	default:
		return SQCODE_NOT_SUPPORTED;
	}

	return SQCODE_OK;
}

static void  sqxc_mem_writer_init(SqxcMem *xcmem)
{
	sq_buffer_resize(sqxc_get_buffer(xcmem), 4096);
	xcmem->buf[0] = 0;    // NULL-terminated

	// sqxc_send() will forward data to xcmem->peer
//	xcmem->supported_type = 0;
}

static void  sqxc_mem_writer_final(SqxcMem *xcmem)
{
}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo sqxcInfo_MemWriter =
{
	sizeof(SqxcMem),
	(SqInitFunc)sqxc_mem_writer_init,
	(SqFinalFunc)sqxc_mem_writer_final,
	(SqxcCtrlFunc)sqxc_mem_writer_ctrl,
	(SqxcSendFunc)sqxc_mem_writer_send,
};
