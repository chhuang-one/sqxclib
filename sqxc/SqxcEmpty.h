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

#ifndef SQXC_EMPTY_H
#define SQXC_EMPTY_H

#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	Sqxc
	|
	`--- SqxcEmpty

	Sqxc data to/from JSON
 */

typedef struct SqxcEmpty        SqxcEmpty;

extern const SqxcInfo SQXC_INFO_EMPTY[2];

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
struct SqxcEmpty : Sq::XcMethod
#else
struct SqxcEmpty
#endif
{
	SQXC_MEMBERS;
/*	// ------ Sqxc members ------
	const SqxcInfo*  info;

	// Sqxc chain
	Sqxc*        next;     // next destination
	Sqxc*        prev;     // previous source

	// source and destination
//	Sqxc*        src;      // pointer to current source in Sqxc chain
	Sqxc*        dest;     // pointer to current destination in Sqxc chain

	// ----------------------------------------------------
	// properties

	unsigned int io_:1;           // Input = 1, Output = 0
	unsigned int supported_type;  // supported SqxcType (bit field)

	// ----------------------------------------------------
	// stack of SqxcNested (placed in dest)

	SqxcNested*  nested;          // current nested object/array
	int          nested_count;

	// ----------------------------------------------------
	// Buffer - common buffer for type conversion. To resize this buf:
	// buf = realloc(buf, buf_size);

//	SQ_BUFFER_MEMBERS(buf, buf_size, buf_writed);
	char*        buf;
	int          buf_size;
	int          buf_writed;

	// ====================================================
	// functions

	SqxcCtrlFunc ctrl;
	SqxcSendFunc send;

	// ----------------------------------------------------
	// function parameter

	// input
	SqxcType     type;     // if code = SQCODE_TYPE_NOT_MATCH, set required type in dest->type->type
	const char*  name;
	union {
		bool          boolean;
		int           integer;
		int           int_;
		unsigned int  uinteger;
		unsigned int  uint;
		int64_t       int64;
		int64_t       uint64;
		double        fraction;
		double        double_;
		char*         string;
		void*         pointer;
	} value;

	// input arguments - optional.  this one can be NULL.
	SqEntry*     entry;

	// input - user data
//	void*        user_data;
//	void*        user_data2;

	// input / output
	void**       error;

	// output
	int          code;     // error code (SQCODE_xxxx)
 */

	int    reverse;

};


#endif  // SQXC_EMPTY_H
