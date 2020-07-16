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
#include <SqField.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	Sqxc
	|
	`--- SqxcValue

	Sqxc data to/from C language Value

	*** In input chain:
	SQXC_TYPE_xxxx ---> SqxcValue ---> C instance
	               send           send

	*** In output chain:
	C instance -------> SqxcValue ---> SQXC_TYPE_xxxx
	               send           send

 */

typedef struct SqxcValue        SqxcValue;

extern const SqxcInfo SQXC_INFO_VALUE[2];

// ----------------------------------------------------------------------------
// macro for accessing variable of SqxcValue

#define sqxc_value_type(xcvalue)          ((SqxcValue*)xcvalue)->element
#define sqxc_value_container(xcvalue)     ((SqxcValue*)xcvalue)->container
#define sqxc_value_element(xcvalue)       ((SqxcValue*)xcvalue)->element
#define sqxc_value_instance(xcvalue)      ((SqxcValue*)xcvalue)->instance

#ifdef __cplusplus
}  // extern "C"
#endif


#ifdef __cplusplus
struct SqxcValue : Sq::XcMethod
#else
struct SqxcValue
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
	int          supported_type;  // supported SqxcType (bit field)

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
		bool     boolean;
		int      integer;
		int64_t  int64;
		double   fraction;
		double   double_;
		char*    string;
		void*    pointer;
	} value;

	// input arguments - optional.  this one can be NULL.
	SqField*     field;

	// input - user data
//	void*        user_data;
//	void*        user_data2;

	// input / output
	void**       error;

	// output
	int          code;     // error code (SQCODE_xxxx)
 */

	void*        instance;

	// current pointer to container when calling get_all()
	// current pointer to element when calling get(id)
	SqType*      current;    // type of instance
	SqType*      element;    // type of table or field
	SqType*      container;  // type of array or list
};

#endif  // SQXC_VALUE_H
