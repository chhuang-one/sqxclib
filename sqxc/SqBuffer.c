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

#include <SqBuffer.h>

#define SQ_BUFFER_DEFAULT_SIZE    128

void* sq_buffer_resize(SqBuffer* buf, int size)
{
	if (size == 0) {
		free(buf->buf);
		buf->buf = NULL;
		buf->size = 0;
		buf->writed = 0;
	}
	else {
		buf->buf = realloc(buf->buf, size);
		buf->size = size;
		if (buf->writed > size)
			buf->writed = size;
	}
	return (void*)buf;
}

char* sq_buffer_alloc_at(SqBuffer* buf, int position, int count)
{
	int  new_length = buf->writed + count +1;  // +1 for null-terminated string

	if (buf->size < new_length) {
		buf->size = (buf->size < SQ_BUFFER_DEFAULT_SIZE) ? SQ_BUFFER_DEFAULT_SIZE : buf->size * 2;
		if (buf->size < new_length)
			buf->size = new_length * 2;
		buf->buf = realloc(buf->buf, buf->size);
	}
	if (position < buf->writed) {
		memmove(buf->buf + position + count,
		        buf->buf + position,
		        buf->writed - position);
	}
	buf->writed = new_length -1;    // -1 for null-terminated string
	return buf->buf + position;
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline function.

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqBuffer.h
#else

void  sq_buffer_write(SqBuffer* buffer, const char* string)
{
	SQ_BUFFER_WRITE(buffer, string);
}

void  sq_buffer_write_n(SqBuffer* buffer, const char* string, int length)
{
	SQ_BUFFER_WRITE_N(buffer, string, length);
}

void  sq_buffer_insert(SqBuffer* buffer, int position, const char* string)
{
	SQ_BUFFER_INSERT(buffer, position, string);
}

void  sq_buffer_insert_n(SqBuffer* buffer, int position, const char* string, int length)
{
	SQ_BUFFER_INSERT_N(buffer, position, string, length);
}

void  sq_buffer_require(SqBuffer* buffer, int length)
{
	SQ_BUFFER_REQUIRE(buffer, length);
}

#endif  // __STDC_VERSION__
