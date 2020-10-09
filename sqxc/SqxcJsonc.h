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

#ifndef SQXC_JSONC_H
#define SQXC_JSONC_H

#include<json-c/json.h>

#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	SqxcJsonc - Middleware of input/output chain. It use json-c to implement.

	Sqxc
	|
	`--- SqxcJsonc

	*** In input chain:
	SQXC_TYPE_STRING ---> SqxcJsonc Parser ---> SQXC_TYPE_xxxx
	 (JSON string)

	*** In output chain:
	SQXC_TYPE_xxxx -----> SqxcJsonc Writer ---> SQXC_TYPE_STRING
	                                              (JSON string)
 */

typedef struct SqxcJsonc        SqxcJsonc;

extern const SqxcInfo *SQXC_INFO_JSONC_PARSER;
extern const SqxcInfo *SQXC_INFO_JSONC_WRITER;

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
	const SqxcInfo  *info;

	// Sqxc chain
	Sqxc*        peer;     // pointer to other Sqxc elements
	Sqxc*        dest;     // pointer to current destination in Sqxc chain

	// stack of SqxcNested
	SqxcNested*  nested;          // current nested object/array
	int          nested_count;

	// ----------------------------------------------------
	// Buffer - common buffer for type conversion. To resize this buf:
	// buf = realloc(buf, buf_size);

//	SQ_BUFFER_MEMBERS(buf, buf_size, buf_writed);
	char*        buf;
	int          buf_size;
	int          buf_writed;

	// ----------------------------------------------------
	// properties

	uint16_t     supported_type;  // supported SqxcType (bit field) for inputting, it can change at runtime.
//	uint16_t     outputable_type; // supported SqxcType (bit field) for outputting, it can change at runtime.

	// ----------------------------------------------------
	// arguments that used by SqxcInfo->send()

	// output arguments
//	uint16_t     required_type;   // required SqxcType (bit field) if 'code' == SQCODE_TYPE_NOT_MATCH
	uint16_t     code;            // error code (SQCODE_xxxx)

	// input arguments
	uint16_t     type;            // input SqxcType
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
		char*         stream;     // Text stream must be null-terminated string
		void*         pointer;
	} value;

	// special input arguments
	SqEntry*     entry;           // SqxcJsonc and SqxcSql use it to decide output. this can be NULL (optional).

	// input / output arguments
	void**       error;
 */

	// ------ SqxcJsonc members ------

	// --- output ---
	const char*  jroot_name;
	json_object* jroot;
	json_object* jcur;
	int16_t      jcur_type;    // SqxcType
	/*
	// at json entry begin from SQL
	cx->name = SQL column name

	// at json begin from file
	cx->name = NULL
	 */
};


#endif  // SQXC_JSONC_H
