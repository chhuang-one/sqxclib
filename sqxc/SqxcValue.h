/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#ifndef SQXC_VALUE_H
#define SQXC_VALUE_H

#include <Sqxc.h>
#include <SqEntry.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqxcValue        SqxcValue;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo        SqxcInfo_Value_;
#define SQXC_INFO_VALUE    (&SqxcInfo_Value_)

#define sqxc_value_new()        sqxc_new(SQXC_INFO_VALUE)

// macro for accessing variable of SqxcValue

// instance type = (container) ? container : element
#define sqxc_value_element(xcvalue)       ( ((SqxcValue*)xcvalue)->element )
#define sqxc_value_container(xcvalue)     ( ((SqxcValue*)xcvalue)->container )

// instance of container (or element)
#define sqxc_value_instance(xcvalue)      ( ((SqxcValue*)xcvalue)->instance )

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqxcValue - convert data to C language Value. (destination of input chain)

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

#ifdef __cplusplus
struct SqxcValue : Sq::XcMethod          // <-- 1. inherit C++ member function(method)
#else
struct SqxcValue
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
	int          buf_size;
	int          buf_writed;

	// ------------------------------------------
	// properties

	uint16_t     supported_type;  // supported SqxcType (bit field) for inputting, it can change at runtime.
//	uint16_t     outputable_type; // supported SqxcType (bit field) for outputting, it can change at runtime.

	// ------------------------------------------
	// arguments that used by SqxcInfo->send()

	// output arguments
//	uint16_t     required_type;   // required SqxcType (bit field) if 'code' == SQCODE_TYPE_NOT_MATCH
	uint16_t     code;            // error code (SQCODE_xxxx)

	// input arguments
	uint16_t     type;            // input SqxcType
	const char  *name;
	SqValue      value;           // union SqValue defined in SqDefine.h

	// special input arguments
	SqEntry     *entry;           // SqxcJsonc and SqxcSql use it to decide output. this can be NULL (optional).

	// input / output arguments
	void       **error;
 */

	// ------ SqxcValue members ------   // <-- 3. Add variable and non-virtual function in derived struct.
	void        *instance;

	// instance type = container when calling get_all()
	// instance type = element when calling get(id)
	const SqType *element;    // type of table (or entry)
	const SqType *container;  // type of array (or list)
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus
namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

struct XcValue : SqxcValue
{
	XcValue() {
		sqxc_init((Sqxc*)this, SQXC_INFO_VALUE);
	}
	~XcValue() {
		sqxc_final((Sqxc*)this);
	}

	void  setElement(const SqType *element) {
		this->element = element;
	}
	const SqType *getElement() {
		return this->element;
	}

	void  setContainer(const SqType *container) {
		this->container = container;
	}
	const SqType *getContainer() {
		return this->container;
	}

	void  setInstance(void *instance) {
		this->instance = instance;
	}
	void *getInstance() {
		return this->instance;
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQXC_VALUE_H
