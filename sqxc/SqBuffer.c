/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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

#include <SqConfig.h>
#include <SqBuffer.h>

#define SQ_BUFFER_SIZE_DEFAULT    SQ_CONFIG_BUFFER_SIZE_DEAULT

void  sq_buffer_init(SqBuffer *buf)
{
	buf->mem    = NULL;
	buf->writed = 0;
	buf->size   = 0;
}

void *sq_buffer_final(SqBuffer *buf)
{
	return sq_buffer_resize(buf, 0);
}

void *sq_buffer_resize(SqBuffer *buf, int size)
{
	if (size == 0) {
		free(buf->mem);
		buf->mem = NULL;
		buf->size = 0;
		buf->writed = 0;
	}
	else {
		buf->mem = realloc(buf->mem, size);
		buf->size = size;
		if (buf->writed > size)
			buf->writed = size;
	}
	return (void*)buf;
}

char *sq_buffer_alloc_at(SqBuffer *buf, int position, int count)
{
	int  new_length = buf->writed + count +1;    // +1 to reserve space for NULL-terminated

	if (buf->size < new_length) {
		buf->size = (buf->size < SQ_BUFFER_SIZE_DEFAULT) ? SQ_BUFFER_SIZE_DEFAULT : buf->size * 2;
		if (buf->size < new_length)
			buf->size = new_length * 2;
		buf->mem = realloc(buf->mem, buf->size);
	}
	if (position < buf->writed) {
		memmove(buf->mem + position + count,
		        buf->mem + position,
		        buf->writed - position);
	}
	buf->writed = new_length -1;    // -1 because reserve space (for NULL-terminated) is not counted in length
	return buf->mem + position;
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline function.

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqBuffer.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

char *sq_buffer_write(SqBuffer *buffer, const char *string)
{
	return SQ_BUFFER_WRITE(buffer, string);
}

char *sq_buffer_write_len(SqBuffer *buffer, const char *string, int length)
{
	return SQ_BUFFER_WRITE_LEN(buffer, string, length);
}

void  sq_buffer_insert(SqBuffer *buffer, int position, const char *string)
{
	SQ_BUFFER_INSERT(buffer, position, string);
}

void  sq_buffer_insert_len(SqBuffer *buffer, int position, const char *string, int length)
{
	SQ_BUFFER_INSERT_LEN(buffer, position, string, length);
}

char *sq_buffer_require(SqBuffer *buffer, int length)
{
	SQ_BUFFER_REQUIRE(buffer, length);
	return buffer->mem;
}

#endif  // __STDC_VERSION__
