/*
 *   Copyright (C) 2020-2026 by C.H. Huang
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
#include <stdlib.h>            // calloc(), realloc()
#include <string.h>            // memcpy(), strcpy(), strlen()

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

// void sq_buffer_free(SqBuffer *buf);
#define sq_buffer_free(buf)    free(sq_buffer_resize(buf, 0))

// void sq_buffer_r_at(SqBuffer *buffer, size_t reversePosition)
#define sq_buffer_r_at(buffer, reversePosition)    \
		(buffer)->mem[(buffer)->writed - (reversePosition) -1]

// allocation length+1 character for null-terminated string
//char *sq_buffer_alloc(SqBuffer *buf, size_t length);
#define sq_buffer_alloc(buffer, length)         \
		sq_buffer_alloc_at((SqBuffer*)buffer, ((SqBuffer*)(buffer))->writed, length)

// void sq_buffer_write_c(SqBuffer *buffer, char character);
#define sq_buffer_write_c(buffer, character)   \
		*sq_buffer_alloc(buffer, 1) = (character)

// void sq_buffer_insert_c(SqBuffer *buffer, size_t position, char character);
#define sq_buffer_insert_c(buffer, position, character)  \
		*sq_buffer_alloc_at(buffer, position, 1) = (character)

/* --- macro functions --- for maintaining C/C++ inline functions easily */

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))    // C99
// void SQ_BUFFER_INIT(SqBuffer *buffer);
#define SQ_BUFFER_INIT(buffer)    \
		*(buffer) = (SqBuffer){NULL, 0, 0}
#else
// void SQ_BUFFER_INIT(SqBuffer *buffer);
#define SQ_BUFFER_INIT(buffer)    \
		{ ((SqBuffer*)(buffer))->mem = NULL;  ((SqBuffer*)(buffer))->size = 0;  ((SqBuffer*)(buffer))->writed = 0; }
#endif  // C99

//void *SQ_BUFFER_FINAL(SqBuffer *buffer)
#define SQ_BUFFER_FINAL(buffer)   \
		sq_buffer_resize((SqBuffer*)buffer, 0)

//char* SQ_BUFFER_WRITE(SqBuffer *buffer, const char *string);
#define SQ_BUFFER_WRITE(buffer, string)  \
		strcpy(sq_buffer_alloc(buffer, strlen(string)), string)

//char* SQ_BUFFER_WRITE_LEN(SqBuffer *buffer, const char *string, size_t length);
#define SQ_BUFFER_WRITE_LEN(buffer, string, length)   \
		(char*)memcpy(sq_buffer_alloc(buffer, length), string, length)

// void SQ_BUFFER_INSERT(SqBuffer *buffer, size_t position, const char *string);
#define SQ_BUFFER_INSERT(buffer, position, string)                \
		{	size_t  length = strlen(string);                      \
			memcpy(sq_buffer_alloc_at(buffer, position, length),  \
			       string, length);                               \
		}

// void SQ_BUFFER_INSERT_LEN(SqBuffer *buffer, size_t position, const char *string, size_t length);
#define SQ_BUFFER_INSERT_LEN(buffer, position, string, length)  \
		memcpy(sq_buffer_alloc_at(buffer, position, length),    \
		       string, length)

// void SQ_BUFFER_REQUIRE(SqBuffer *buffer, size_t length);
#define SQ_BUFFER_REQUIRE(buffer, length)               \
		if (((SqBuffer*)(buffer))->size < (length)) {   \
			((SqBuffer*)(buffer))->size = (length);     \
			((SqBuffer*)(buffer))->mem  = (char*)realloc(((SqBuffer*)(buffer))->mem, length); }

/* --- C functions --- */

void  sq_buffer_init(SqBuffer *buf);
void *sq_buffer_final(SqBuffer *buf);

void *sq_buffer_resize(SqBuffer *buf, size_t size);

// It reserve space in tail of buffer for NULL-terminated
char *sq_buffer_alloc_at(SqBuffer *buf, size_t position, size_t length);

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
struct BufferMethod
{
	void   init();
	void   final();

	void   resize(size_t size);

	char  *alloc(size_t position, size_t size);
	char  *alloc(size_t size);

	char  *require(size_t size);

	void   write(char character);
	char  *write(const char *string);
	char  *write(const char *string, size_t length);

	void   insert(size_t position, char character);
	void   insert(size_t position, const char *string);
	void   insert(size_t position, const char *string, size_t length);
};

};  // namespace Sq

#endif // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

#define SQ_BUFFER_MEMBERS(mem_name, size_name, writed_name)  \
	char   *mem_name;      \
	size_t  size_name;     \
	size_t  writed_name

#ifdef __cplusplus
struct SqBuffer : Sq::BufferMethod           // <-- 1. inherit member function(method)
#else
struct SqBuffer
#endif
{
	SQ_BUFFER_MEMBERS(mem, size, writed);    // <-- 2. inherit member variable
/*	// ------ SqBuffer members ------
	char   *mem;
	size_t  size;
	size_t  writed;
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
char *sq_buffer_write(SqBuffer *buffer, const char *string) {
	return SQ_BUFFER_WRITE(buffer, string);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
char *sq_buffer_write_len(SqBuffer *buffer, const char *string, size_t length) {
	return SQ_BUFFER_WRITE_LEN(buffer, string, length);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_insert(SqBuffer *buffer, size_t position, const char *string) {
	SQ_BUFFER_INSERT(buffer, position, string);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_buffer_insert_len(SqBuffer *buffer, size_t position, const char *string, size_t length) {
	SQ_BUFFER_INSERT_LEN(buffer, position, string, length);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
char *sq_buffer_require(SqBuffer *buffer, size_t length) {
	SQ_BUFFER_REQUIRE(buffer, length);
	return buffer->mem;
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

char *sq_buffer_write(SqBuffer *buffer, const char *string);
char *sq_buffer_write_len(SqBuffer *buffer, const char *string, size_t length);
void  sq_buffer_insert(SqBuffer *buffer, size_t position, const char *string);
void  sq_buffer_insert_len(SqBuffer *buffer, size_t position, const char *string, size_t length);
char *sq_buffer_require(SqBuffer *buffer, size_t length);

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

inline void   BufferMethod::resize(size_t size) {
	sq_buffer_resize((SqBuffer*)this, size);
}

inline char  *BufferMethod::alloc(size_t position, size_t size) {
	return sq_buffer_alloc_at((SqBuffer*)this, position, size);
}
inline char  *BufferMethod::alloc(size_t size) {
	return sq_buffer_alloc_at((SqBuffer*)this, ((SqBuffer*)this)->writed, size);
}

inline char  *BufferMethod::require(size_t size) {
	SQ_BUFFER_REQUIRE((SqBuffer*)this, size);
	return ((SqBuffer*)this)->mem;
}

inline void   BufferMethod::write(char character) {
	sq_buffer_write_c((SqBuffer*)this, character);
}
inline char  *BufferMethod::write(const char *string) {
	return SQ_BUFFER_WRITE((SqBuffer*)this, string);
}
inline char  *BufferMethod::write(const char *string, size_t length) {
	return SQ_BUFFER_WRITE_LEN((SqBuffer*)this, string, length);
}

inline void   BufferMethod::insert(size_t position, char character) {
	sq_buffer_insert_c((SqBuffer*)this, position, character);
}
inline void   BufferMethod::insert(size_t position, const char *string) {
	SQ_BUFFER_INSERT((SqBuffer*)this, position, string);
}
inline void   BufferMethod::insert(size_t position, const char *string, size_t length) {
	SQ_BUFFER_INSERT_LEN((SqBuffer*)this, position, string, length);
}

/* All derived struct/class must be C++11 standard-layout. */
struct Buffer : SqBuffer
{
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
