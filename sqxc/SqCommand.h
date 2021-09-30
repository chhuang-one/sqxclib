/*
 *   Copyright (C) 2021 by C.H. Huang
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

#ifndef SQ_COMMAND_H
#define SQ_COMMAND_H

#include <SqPtrArray.h>
#include <SqBuffer.h>
#include <SqOption.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqConsole         SqConsole;
typedef struct SqCommand         SqCommand;
typedef struct SqCommandType     SqCommandType;

#define SQ_COMMAND_TYPE_INITIALIZER(StructType, bit_value, Command, parameter_string, description_string) \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.init  = (SqTypeFunc) Command##_init,                          \
	.final = (SqTypeFunc) Command##_final,                         \
	.parse = sq_type_command_parse_option,                         \
	.write = NULL,                                                 \
	.name  = #Command,                                             \
	.entry   = (SqEntry**) Command##_options,                      \
	.n_entry = sizeof(Command##_options) / sizeof(SqOption*),      \
	.bit_field = bit_value,                                        \
	.ref_count = 0,                                                \
	.handle      = (SqCommandFunc) Command##_handle,               \
	.parameter   = parameter_string,                               \
	.description = description_string,                             \
}

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SqCommandFunc)(SqCommand *cmd, SqConsole *console, void* data);

/* --- SqCommand C functions --- */
SqCommand  *sq_command_new(const SqCommandType *cmd_type);
void        sq_command_free(SqCommand *cmd);

void  sq_command_init(SqCommand *cmd, const SqCommandType *cmd_type);
void  sq_command_final(SqCommand *cmd);

// this function is called by SqConsole
void  sq_command_sort_shortcuts(SqCommand *cmd);

/* --- SqCommandType C functions --- */

// SqCommandType parse function for SqCommand
int   sq_type_command_parse_option(void *cmd_instance, const SqType *cmd_type, Sqxc *src);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- declare methods for Sq::Command --- */
struct CommandMethod {
	void *operator new(size_t size) {
		return calloc(1, size);
	}

	void init(const SqCommandType *cmdtype) {
		sq_command_init((SqCommand*)this, cmdtype);
	}
	void final() {
		sq_command_final((SqCommand*)this);
	}
};

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqCommandType: parse/handle command and it's options.

	SqType
	|
	`--- SqCommandType
 */

struct SqCommandType {
	SQ_TYPE_MEMBERS;
/*	// ------ SqType members ------
	unsigned int   size;        // instance size

	SqTypeFunc     init;        // initialize instance
	SqTypeFunc     final;       // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to assign "name"
	// or use macro SQ_GET_TYPE_NAME()
	char          *name;

//	SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry);
//	// ------ SqPtrArray members ------
	SqEntry      **entry;
	int            n_entry;

//	SqType.entry is array of SqEntry pointer if current SqType is for C struct.
//	SqType.entry can't be freed if SqType.n_entry == -1

	// SqType::bit_field has SQB_TYPE_DYNAMIC if this is dynamic SqType and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED if SqType::entry is sorted.
	uint16_t       bit_field;
	uint16_t       ref_count;    // reference count for dynamic SqType only
 */

	// ------ SqCommandType members ------
	SqCommandFunc  handle;
	const char    *parameter;
	const char    *description;
};

/*	SqCommand: define a command that used by command line interface.
 */
#define SQ_COMMAND_MEMBERS       \
	const SqCommandType  *type;  \
	SqPtrArray     shortcuts;    \
	SqPtrArray     arguments

#ifdef __cplusplus
struct SqCommand : Sq::CommandMethod     // <-- 1. inherit C++ member function(method)
#else
struct SqCommand
#endif
{
	SQ_COMMAND_MEMBERS;                  // <-- 2. inherit member variable
/*	// ------ SqCommand members ------
	const SqCommandType  *type;

	// shortcuts & arguments are used by SqConsole
	SqPtrArray     shortcuts;    // sorted by SqOption.shortcut
	SqPtrArray     arguments;
 */

	/* Add variable and function */      // <-- 3. Add variable and non-virtual function in derived struct.
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- define C++11 standard-layout structures --- */
typedef struct SqCommandType     CommandType;
typedef struct SqCommand         Command;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_COMMAND_H
