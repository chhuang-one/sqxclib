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

#ifndef SQXC_CJSON_H
#define SQXC_CJSON_H

#include <sqxc/SqConfig.h>
#if SQ_CONFIG_HAVE_CJSON       // defined in SqConfig.h

#include <cjson/cJSON.h>
#include <sqxc/Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqxcCjson        SqxcCjson;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo               sqxcInfo_CjsonParser;
extern const SqxcInfo               sqxcInfo_CjsonWriter;
#define SQXC_INFO_CJSON_PARSER    (&sqxcInfo_CjsonParser)
#define SQXC_INFO_CJSON_WRITER    (&sqxcInfo_CjsonWriter)

//Sqxc *sqxc_cjson_parser_new();
#define sqxc_cjson_parser_new()        sqxc_new(SQXC_INFO_CJSON_PARSER)
//Sqxc *sqxc_cjson_writer_new();
#define sqxc_cjson_writer_new()        sqxc_new(SQXC_INFO_CJSON_WRITER)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqxcCjson - Middleware of input/output chain. It use cJSON to implement.

	Sqxc
	|
	`--- SqxcCjson

	*** In input chain:
	SQXC_TYPE_STR  ---> SqxcCjson Parser ---> SQXC_TYPE_xxxx
	(JSON string)

	*** In output chain:
	SQXC_TYPE_xxxx ---> SqxcCjson Writer ---> SQXC_TYPE_STR
	                                          (JSON string)


   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

#ifdef __cplusplus
struct SqxcCjson : Sq::XcMethod          // <-- 1. inherit C++ member function(method)
#else
struct SqxcCjson
#endif
{
	SQXC_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ Sqxc members ------
	const SqxcInfo  *info;

	// Sqxc chain
	Sqxc        *peer;     // pointer to other Sqxc elements (single linked list)
	Sqxc        *dest;     // pointer to current destination in Sqxc chain (data flow)

	// stack of SqxcNested
	SqxcNested  *nested;          // current nested object/array
	int          nested_count;

	// ------------------------------------------
	// Buffer - common buffer for type conversion. To resize this buf:
	// buf = realloc(buf, buf_size);

//	SQ_BUFFER_MEMBERS(buf, buf_size, buf_writed);
	char        *buf;
	size_t       buf_size;
	size_t       buf_writed;

	// ------------------------------------------
	// properties

	uint16_t     supported_type;  // supported SqxcType (bit field) for inputting, it can change at runtime.
//	uint16_t     outputable_type; // supported SqxcType (bit field) for outputting, it can change at runtime.

	// ------------------------------------------
	// arguments that used by SqxcInfo::send()

	// output arguments
//	uint16_t     required_type;   // required SqxcType (bit field) if 'code' == SQCODE_TYPE_NOT_MATCHED
	uint16_t     code;            // error code (SQCODE_xxxx)

	// input arguments
	uint16_t     type;            // input SqxcType
	const char  *name;
	SqValue      value;           // union SqValue defined in SqDefine.h

	// special input arguments
	SqEntry     *entry;           // SqxcCjson and SqxcSql use it to decide output. this can be NULL (optional).

	// input / output arguments
	void       **error;
 */

	// ------ SqxcCjson members ------   // <-- 3. Add variable and non-virtual function in derived struct.

	const char   *jRootName;       // output: jRoot object or array name (database column name)
	cJSON        *jRoot;           // input / output: cJSON root object.
	cJSON        *jNested;         // output: The current JSON nested object or array.
	uint16_t      jNestedXcType;   // output: jNested currently maps to the SqxcType type.

	/*
	// at json entry begin from SQL
	xc->name = databaseColumnName;

	// at json begin from file
	xc->name = NULL;
	 */
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

struct XcCjsonParser : SqxcCjson
{
	// constructor
	XcCjsonParser() {
		sqxc_init((Sqxc*)this, SQXC_INFO_CJSON_PARSER);
	}
	// destructor
	~XcCjsonParser() {
		sqxc_final((Sqxc*)this);
	}
};

struct XcCjsonWriter : SqxcCjson
{
	// constructor
	XcCjsonWriter() {
		sqxc_init((Sqxc*)this, SQXC_INFO_CJSON_WRITER);
	}
	// destructor
	~XcCjsonWriter() {
		sqxc_final((Sqxc*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_CONFIG_HAVE_CJSON
#endif  // SQXC_CJSON_H
