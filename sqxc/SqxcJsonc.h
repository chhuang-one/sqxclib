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

#ifndef SQXC_JSONC_H
#define SQXC_JSONC_H

#include <sqxc/SqConfig.h>
#if SQ_CONFIG_SQXC_CJSON_ONLY_IF_POSSIBLE && SQ_CONFIG_HAVE_CJSON
#undef  SQ_CONFIG_HAVE_JSONC
#define SQ_CONFIG_HAVE_JSONC    0
#endif
#if SQ_CONFIG_HAVE_JSONC       // defined in SqConfig.h

#include <json-c/json.h>
#include <sqxc/Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqxcJsonc        SqxcJsonc;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo               sqxcInfo_JsoncParser;
extern const SqxcInfo               sqxcInfo_JsoncWriter;
#define SQXC_INFO_JSONC_PARSER    (&sqxcInfo_JsoncParser)
#define SQXC_INFO_JSONC_WRITER    (&sqxcInfo_JsoncWriter)

//Sqxc *sqxc_jsonc_parser_new();
#define sqxc_jsonc_parser_new()        sqxc_new(SQXC_INFO_JSONC_PARSER)
//Sqxc *sqxc_jsonc_writer_new();
#define sqxc_jsonc_writer_new()        sqxc_new(SQXC_INFO_JSONC_WRITER)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqxcJsonc - Middleware of input/output chain. It use json-c to implement.

	Sqxc
	|
	`--- SqxcJsonc

	*** In input chain:
	SQXC_TYPE_STR  ---> SqxcJsonc Parser ---> SQXC_TYPE_xxxx
	(JSON string)

	*** In output chain:
	SQXC_TYPE_xxxx ---> SqxcJsonc Writer ---> SQXC_TYPE_STR
	                                          (JSON string)


   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

#ifdef __cplusplus
struct SqxcJsonc : Sq::XcMethod          // <-- 1. inherit C++ member function(method)
#else
struct SqxcJsonc
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
	SqEntry     *entry;           // SqxcJson and SqxcSql use it to decide output. this can be NULL (optional).

	// input / output arguments
	void       **error;
 */

	// ------ SqxcJsonc members ------   // <-- 3. Add variable and non-virtual function in derived struct.

	json_tokener *jTokener;        // input: JSON parser

	const char   *jRootName;       // output: jRoot object or array name (database column name)
	json_object  *jRoot;           // input / output: json-c root object.
	json_object  *jNested;         // output: The current JSON nested object or array.
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

struct XcJsoncParser : SqxcJsonc
{
	// constructor
	XcJsoncParser() {
		sqxc_init((Sqxc*)this, SQXC_INFO_JSONC_PARSER);
	}
	// destructor
	~XcJsoncParser() {
		sqxc_final((Sqxc*)this);
	}
};

struct XcJsoncWriter : SqxcJsonc
{
	// constructor
	XcJsoncWriter() {
		sqxc_init((Sqxc*)this, SQXC_INFO_JSONC_WRITER);
	}
	// destructor
	~XcJsoncWriter() {
		sqxc_final((Sqxc*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_CONFIG_HAVE_JSONC
#endif  // SQXC_JSONC_H
