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

#ifndef SQ_BUFFER_H
#define SQ_BUFFER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>    // calloc(), realloc()
#include <string.h>    // memcpy(), strcpy(), strlen()

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

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
		{ ((SqBuffer*)(buffer))->mem = NULL;  ((SqBuffer*)(buffer))->size = 0;  ((SqBuffer*)(buffer))->writed = 0; }
#endif  // C99

#define sq_buffer_final(buffer)   \
		sq_buffer_resize((SqBuffer*)buffer, 0)

// void sq_buffer_r_at(SqBuffer *buffer, int reverse_pos)
#define sq_buffer_r_at(buffer, reverse_pos)    \
		(buffer)->mem[(buffer)->writed - (reverse_pos) -1]

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

//char* SQ_BUFFER_WRITE(SqBuffer *buffer, const char *string);
#define SQ_BUFFER_WRITE(buffer, string)  \
		strcpy(sq_buffer_alloc(buffer, (int)strlen(string)), string)

//char* SQ_BUFFER_WRITE_N(SqBuffer *buffer, const char *string, int length);
#define SQ_BUFFER_WRITE_N(buffer, string, length)   \
		(char*)memcpy(sq_buffer_alloc(buffer, length), string, length)

// void SQ_BUFFER_INSERT(SqBuffer *buffer, int position, const char *string);
#define SQ_BUFFER_INSERT(buffer, position, string)  \
		{	int  length = (int)strlen(string);           \
			memcpy(sq_buffer_alloc_at(buffer, position, length),  \
			       string, length);                 \
		}

// void SQ_BUFFER_INSERT_N(SqBuffer *buffer, int position, const char *string, int length);
#define SQ_BUFFER_INSERT_N(buffer, position, string, length)  \
		memcpy(sq_buffer_alloc_at(buffer, position, length),  \
		       string, length)

// void SQ_BUFFER_REQUIRE(SqBuffer *buffer, int length);
#define SQ_BUFFER_REQUIRE(buffer, length)               \
		if (((SqBuffer*)(buffer))->size < (length)) {   \
			((SqBuffer*)(buffer))->size = (length);     \
			((SqBuffer*)(buffer))->mem  = (char*)realloc(((SqBuffer*)(buffer))->mem, (length)); }

/* --- alias --- */
#define SQ_BUFFER_INSERT_LEN        SQ_BUFFER_INSERT_N
#define SQ_BUFFER_WRITE_LEN         SQ_BUFFER_WRITE_N
#define sq_buffer_insert_len        sq_buffer_insert_n
#define sq_buffer_write_len         sq_buffer_write_n

/* --- C functions --- */

void *sq_buffer_resize(SqBuffer *buf, int size);

// It reserve space in tail of buffer for NULL-terminated
char *sq_buffer_alloc_at(SqBuffer *buf, int position, int count);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	BufferMethod is used by SqBuffer and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqBuffer members.
 */
struct BufferMethod {
	void   init();
	void   final();

	void   resize(int size);

	char  *alloc(int position, int size);
	char  *alloc(int size);

	char  *require(int size);

	void   write(char character);
	char  *write(const char *string);
	char  *write(const char *string, int length);

	void   insert(int position, char character);
	void   insert(int position, const char *string);
	void   insert(int position, const char *string, int length);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

#define SQ_BUFFER_MEMBERS(mem_name, size_name, writed_name)  \
	char  *mem_name;      \
	int    size_name;     \
	int    writed_name

#ifdef __cplusplus
struct SqBuffer : Sq::BufferMethod           // <-- 1. inherit member function(method)
#else
struct SqBuffer
#endif
{
	SQ_BUFFER_MEMBERS(mem, size, writed);    // <-- 2. inherit member variable
/*	// ------ SqBuffer members ------
	char  *mem;
	int    size;
	int    writed;
 */
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
char *sq_buffer_write(SqBuffer *buffer, const char *string)
{
	return SQ_BUFFER_WRITE(buffer, string);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
char *sq_buffer_write_n(SqBuffer *buffer, const char *string, int length)
{
	return SQ_BUFFER_WRITE_N(buffer, string, length);
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
	return buffer->mem;
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

char *sq_buffer_write(SqBuffer *buffer, const char *string);
char *sq_buffer_write_n(SqBuffer *buffer, const char *string, int length);
void  sq_buffer_insert(SqBuffer *buffer, int position, const char *string);
void  sq_buffer_insert_n(SqBuffer *buffer, int position, const char *string, int length);
char *sq_buffer_require(SqBuffer *buffer, int length);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define BufferMethod functions. */

inline void   BufferMethod::init() {
	sq_buffer_init((SqBuffer*)this);
}
inline void   BufferMethod::final() {
	sq_buffer_final((SqBuffer*)this);
}

inline void   BufferMethod::resize(int size) {
	sq_buffer_resize((SqBuffer*)this, size);
}

inline char  *BufferMethod::alloc(int position, int size) {
	return sq_buffer_alloc_at((SqBuffer*)this, position, size);
}
inline char  *BufferMethod::alloc(int size) {
	return sq_buffer_alloc_at((SqBuffer*)this, ((SqBuffer*)this)->writed, size);
}

inline char  *BufferMethod::require(int size) {
	SQ_BUFFER_REQUIRE((SqBuffer*)this, size);
	return ((SqBuffer*)this)->mem;
}

inline void   BufferMethod::write(char character) {
	sq_buffer_write_c((SqBuffer*)this, character);
}
inline char  *BufferMethod::write(const char *string) {
	return SQ_BUFFER_WRITE((SqBuffer*)this, string);
}
inline char  *BufferMethod::write(const char *string, int length) {
	return SQ_BUFFER_WRITE_N((SqBuffer*)this, string, length);
}

inline void   BufferMethod::insert(int position, char character) {
	sq_buffer_insert_c((SqBuffer*)this, position, character);
}
inline void   BufferMethod::insert(int position, const char *string) {
	SQ_BUFFER_INSERT((SqBuffer*)this, position, string);
}
inline void   BufferMethod::insert(int position, const char *string, int length) {
	SQ_BUFFER_INSERT_N((SqBuffer*)this, position, string, length);
}

/* All derived struct/class must be C++11 standard-layout. */
struct Buffer : SqBuffer {
	// constructor
	Buffer() {
		sq_buffer_init(this);
	}
	// destructor
	~Buffer() {
		sq_buffer_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_BUFFER_H
