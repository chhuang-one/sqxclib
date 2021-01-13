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

#ifndef SQXC_EMPTY_H
#define SQXC_EMPTY_H

#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	SqxcEmpty - A sample of Sqxc element.

	Sqxc
	|
	`--- SqxcEmpty

   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

typedef struct SqxcEmpty        SqxcEmpty;

extern const SqxcInfo *SQXC_INFO_EMPTY;

#define sqxc_empty_new()        sqxc_new(SQXC_INFO_EMPTY)

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
struct SqxcEmpty : Sq::XcMethod          // <-- 1. inherit member function(method)
#else
struct SqxcEmpty
#endif
{
	SQXC_MEMBERS;                        // <-- 2. inherit member variable
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

	// ------ SqxcEmpty members ------   // <-- 3. Add variable and non-virtual function in derived struct.
	const char*  tag;
	int          nested_count_when_ready;
	int          send_to_dest_if_no_nested;    // boolean, 
	uint16_t     not_matched_type;             // SqxcType (bit field)
};

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq {

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct XcEmpty : SqxcEmpty
{
	XcEmpty() {
		sqxc_init((Sqxc*)this, SQXC_INFO_EMPTY);
	}
	~XcEmpty() {
		sqxc_final((Sqxc*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQXC_EMPTY_H
