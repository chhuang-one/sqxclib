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

#ifndef SQ_BUFFER_H
#define SQ_BUFFER_H

#include <stdlib.h>    // calloc(), realloc()
#include <string.h>    // memcpy(), strcpy(), strlen()

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqBuffer    SqBuffer;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- macro functions --- parameter used only once in macro (except parameter 'buf') */

// SqBuffer *sq_buffer_new(void);
#define sq_buffer_new()        (SqBuffer*) calloc(1, sizeof(SqBuffer))

#define sq_buffer_free(buf)    free(sq_buffer_resize(buf, 0))

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))    // C99
#define sq_buffer_init(buffer)    \
		*(buffer) = (SqBuffer){NULL, 0, 0}
#else
#define sq_buffer_init(buffer)    \
		{ ((SqBuffer*)(buffer))->buf = NULL;  ((SqBuffer*)(buffer))->size = 0;  ((SqBuffer*)(buffer))->writed = 0; }
#endif  // C99

#define sq_buffer_final(buffer)   \
		sq_buffer_resize((SqBuffer*)buffer, 0)

// void sq_buffer_r_at(SqBuffer *buffer, int reverse_pos)
#define sq_buffer_r_at(buffer, reverse_pos)    \
		(buffer)->buf[(buffer)->writed - (reverse_pos) -1]

// allocation count+1 character for null-terminated string
//char *sq_buffer_alloc(SqBuffer *buf, int count);
#define sq_buffer_alloc(buffer, count)   \
		sq_buffer_alloc_at((SqBuffer*)buffer, ((SqBuffer*)(buffer))->writed, count)

// void sq_buffer_write_c(SqBuffer *buffer, char character);
#define sq_buffer_write_c(buffer, character)  \
		*sq_buffer_alloc(buffer, 1) = (character)

// void sq_buffer_insert_c(SqBuffer *buffer, int position, char character);
#define sq_buffer_insert_c(buffer, position, character)  \
		*sq_buffer_alloc_at(buffer, position, 1) = (character)

/* --- macro functions --- for maintaining C/C++ inline functions easily */

// void sq_buffer_write(SqBuffer *buffer, const char *string);
#define SQ_BUFFER_WRITE(buffer, string)  \
		strcpy(sq_buffer_alloc(buffer, strlen(string)), string)

// void sq_buffer_write_n(SqBuffer *buffer, const char *string, int length);
#define SQ_BUFFER_WRITE_N(buffer, string, length)   \
		memcpy(sq_buffer_alloc(buffer, length), string, length)

// void sq_buffer_insert(SqBuffer *buffer, int position, const char *string);
#define SQ_BUFFER_INSERT(buffer, position, string)  \
		{	int  length = strlen(string);           \
			memcpy(sq_buffer_alloc_at(buffer, position, length),  \
			       string, length);                 \
		}

// void sq_buffer_insert_n(SqBuffer *buffer, int position, const char *string, int length);
#define SQ_BUFFER_INSERT_N(buffer, position, string, length)  \
		memcpy(sq_buffer_alloc_at(buffer, position, length),  \
		       string, length)

// void sq_buffer_require(SqBuffer *buffer, int length);
#define SQ_BUFFER_REQUIRE(buffer, length)               \
		if (((SqBuffer*)(buffer))->size < (length)) {   \
			((SqBuffer*)(buffer))->size = (length);     \
			((SqBuffer*)(buffer))->buf  = (char*)realloc(((SqBuffer*)(buffer))->buf, (length)); }

/* --- C functions --- */

void *sq_buffer_resize(SqBuffer *buf, int size);

// It reserve space in tail of buffer for NULL-terminated
char *sq_buffer_alloc_at(SqBuffer *buf, int position, int count);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

#define SQ_BUFFER_MEMBERS(buf_name, size_name, writed_name)  \
	char  *buf_name;      \
	int    size_name;     \
	int    writed_name

struct SqBuffer
{
//	SQ_BUFFER_MEMBERS(buf, size, writed)
	char  *buf;
	int    size;
	int    writed;

#ifdef __cplusplus
	void   init() {
		sq_buffer_init(this);
	}
	void   final() {
		sq_buffer_final(this);
	}

	void   resize(int size) {
		sq_buffer_resize(this, size);
	}

	char  *alloc(int position, int size) {
		return sq_buffer_alloc_at(this, position, size);
	}
	char  *alloc(int size) {
		return sq_buffer_alloc_at(this, this->writed, size);
	}

	char  *require(int size) {
		SQ_BUFFER_REQUIRE(this, size);  return buf;
	}

	void   write(char character) {
		sq_buffer_write_c(this, character);
	}
	void   write(const char *string) {
		SQ_BUFFER_WRITE(this, string);
	}
	void   write(const char *string, int length) {
		SQ_BUFFER_WRITE_N(this, string, length);
	}

	void   insert(int position, char character) {
		sq_buffer_insert_c(this, position, character);
	}
	void   insert(int position, const char *string) {
		SQ_BUFFER_INSERT(this, position, string);
	}
	void   insert(int position, const char *string, int length) {
		SQ_BUFFER_INSERT_N(this, position, string, length);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// C99 or C++ inline functions

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_write(SqBuffer *buffer, const char *string)
{
	SQ_BUFFER_WRITE(buffer, string);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_write_n(SqBuffer *buffer, const char *string, int length)
{
	SQ_BUFFER_WRITE_N(buffer, string, length);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_insert(SqBuffer *buffer, int position, const char *string)
{
	SQ_BUFFER_INSERT(buffer, position, string);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_insert_n(SqBuffer *buffer, int position, const char *string, int length)
{
	SQ_BUFFER_INSERT_N(buffer, position, string, length);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
char *sq_buffer_require(SqBuffer *buffer, int length)
{
	SQ_BUFFER_REQUIRE(buffer, length);
	return buffer->buf;
}

#else   // __STDC_VERSION__ || __cplusplus

// C functions  (If C compiler doesn't support C99 inline function.)

void  sq_buffer_write(SqBuffer *buffer, const char *string);
void  sq_buffer_write_n(SqBuffer *buffer, const char *string, int length);
void  sq_buffer_insert(SqBuffer *buffer, int position, const char *string);
void  sq_buffer_insert_n(SqBuffer *buffer, int position, const char *string, int length);
char *sq_buffer_require(SqBuffer *buffer, int length);

#endif  // __STDC_VERSION__ || __cplusplus


#endif  // SQ_BUFFER_H
