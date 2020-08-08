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

// json-c

#ifndef SQXC_JSONC_H
#define SQXC_JSONC_H

#include<json-c/json.h>

#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	Sqxc
	|
	`--- SqxcJsonc

	Sqxc data to/from JSON

	*** In input chain:
	SQXC_TYPE_STRING ---> SqxcJsonc ---> SQXC_TYPE_xxxx
	 (JSON string)   send           send

	*** In output chain:
	SQXC_TYPE_xxxx -----> SqxcJsonc ---> SQXC_TYPE_STRING
	                 send           send   (JSON string)
 */

typedef struct SqxcJsonc        SqxcJsonc;

extern const SqxcInfo SQXC_INFO_JSONC[2];

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
struct SqxcJsonc : Sq::XcMethod
#else
struct SqxcJsonc
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
	int          supported_type;  // supported SqxcType (bit entry)

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

	// --- output ---
	const char*  jroot_name;
	json_object* jroot;
	json_object* jcur;
	SqxcType     jcur_type;
	/*
	// at json entry begin from SQL
	cx->name = SQL column name

	// at json begin from file
	cx->name = NULL
	 */


};


#endif  // SQXC_JSONC_H
