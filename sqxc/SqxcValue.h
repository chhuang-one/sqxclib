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

#ifndef SQXC_VALUE_H
#define SQXC_VALUE_H

#include <Sqxc.h>
#include <SqEntry.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	SqxcValue - convert data to C Language Value. (destination of input chain)

	Sqxc
	|
	`--- SqxcValue

	                 +-> SqxcJsonParser --+
	( input )        |                    |
	Sqdb.exec()    --+--------------------+-> SqxcValue ---> SqType.parse()
	                 |                    |
	                 +--> SqxcXmlParser --+


   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

typedef struct SqxcValue        SqxcValue;

extern const SqxcInfo *SQXC_INFO_VALUE;

// ----------------------------------------------------------------------------
// macro for accessing variable of SqxcValue

#define sqxc_value_instance(xcvalue)      ((SqxcValue*)xcvalue)->instance
// type
#define sqxc_value_type(xcvalue)          ((SqxcValue*)xcvalue)->element
#define sqxc_value_current(xcvalue)       ((SqxcValue*)xcvalue)->current
#define sqxc_value_element(xcvalue)       ((SqxcValue*)xcvalue)->element
#define sqxc_value_container(xcvalue)     ((SqxcValue*)xcvalue)->container

#ifdef __cplusplus
}  // extern "C"
#endif


#ifdef __cplusplus
struct SqxcValue : Sq::XcMethod          // <-- 1. inherit member function(method)
#else
struct SqxcValue
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

	// ------ SqxcValue members ------   // <-- 3. Add variable and non-virtual function in derived struct.
	void*        instance;

	// current pointer to container when calling get_all()
	// current pointer to element when calling get(id)
	const SqType *current;    // type of instance
	const SqType *element;    // type of table (or entry)
	const SqType *container;  // type of array (or list)
};

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus
namespace Sq {

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct XcValue : SqxcValue
{
	XcValue() {
		sqxc_init((Sqxc*)this, SQXC_INFO_VALUE);
	}
	~XcValue() {
		sqxc_final((Sqxc*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQXC_VALUE_H
