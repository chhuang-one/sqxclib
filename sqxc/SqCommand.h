/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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

#ifndef SQ_COMMAND_H
#define SQ_COMMAND_H

#include <SqPtrArray.h>
#include <SqBuffer.h>
#include <SqOption.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqConsole         SqConsole;
typedef struct SqCommand         SqCommand;
typedef struct SqCommandValue    SqCommandValue;

#define SQ_COMMAND_INITIALIZER(StructType, bit_value, command_string, command_options, handle_func, parameter_string, description_string) \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.init  = (SqTypeFunc) NULL,                                    \
	.final = (SqTypeFunc) NULL,                                    \
	.parse = sq_command_parse_option,                              \
	.write = NULL,                                                 \
	.name  = command_string,                                       \
	.entry   = (SqEntry**) command_options,                        \
	.n_entry = sizeof(command_options) / sizeof(SqOption*),        \
	.bit_field = bit_value,                                        \
	.ref_count = 0,                                                \
	                                                               \
	.handle      = (SqCommandFunc) handle_func,                    \
	.parameter   = parameter_string,                               \
	.description = description_string,                             \
}

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SqCommandFunc)(SqCommandValue *cmd_value, SqConsole *console, void *data);

/* --- SqCommandValue C functions --- */
SqCommandValue *sq_command_value_new(const SqCommand *cmd_type);
void            sq_command_value_free(SqCommandValue *cmd_value);

void  sq_command_value_init(SqCommandValue *cmd_value, const SqCommand *cmd_type);
void  sq_command_value_final(SqCommandValue *cmd_value);


/* --- SqCommand C functions --- */
SqCommand *sq_command_new(const char *cmd_name);

// these function only work if SqCommand.bit_field has SQB_TYPE_DYNAMIC
void  sq_command_ref(SqCommand *cmd_type);
void  sq_command_unref(SqCommand *cmd_type);

// sort options by SqOption.shortcut and save result in array. this function is called by SqConsole
void  sq_command_sort_shortcuts(const SqCommand *cmd_type, SqPtrArray *array);

// void sq_command_add_option(SqCommand *cmd_type, SqOption *option, int n_option);
#define sq_command_add_option(cmd_type, option, n_option)    \
		sq_type_add_entry((SqType*)cmd_type, (SqEntry*)option, n_option, sizeof(SqOption));

// copy data from static SqCommand to dynamic SqCommand. 'type_dest' must be raw memory.
// if 'type_dest' is NULL, function will create dynamic SqCommand.
// if 'option_free_func' is NULL, function will use default value - sq_option_free
// return dynamic SqCommand.
SqCommand *sq_command_copy_static(SqCommand       *type_dest,
                                  const SqCommand *static_type_src,
                                  SqDestroyFunc    option_free_func);

// SqCommand parse function
int   sq_command_parse_option(void *cmd_value, const SqType *cmd_type, Sqxc *src);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- declare methods for Sq::Command --- */
struct CommandMethod {
	// these function only work if SqCommand.bit_field has SQB_TYPE_DYNAMIC
	void  ref() {
		sq_command_ref((SqCommand*)this);
	}
	void  unref() {
		sq_command_unref((SqCommand*)this);
	}

	void  sortShortcuts(SqPtrArray *array) {
		sq_command_sort_shortcuts((SqCommand*)this, array);
	}

	// create dynamic SqCommand and copy data from static SqCommand
	SqCommand *copyStatic(SqDestroyFunc option_free_func = NULL) {
		return sq_command_copy_static(NULL, (const SqCommand*)this, option_free_func);
	}

	// add option from SqOption array (NOT pointer array) to dynamic SqCommand.
	void  addOption(const SqOption *option, int n_option = 1) {
		sq_command_add_option((SqCommand*)this, option, n_option);
	}
};

/* --- declare methods for Sq::CommandValue --- */
struct CommandValueMethod {
	void *operator new(size_t size) {
		return calloc(1, size);
	}

	void  init(const SqCommand *cmd_type) {
		sq_command_value_init((SqCommandValue*)this, cmd_type);
	}
	void  final() {
		sq_command_value_final((SqCommandValue*)this);
	}
};

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqCommand: User can use SqCommand to define command and it's options
	           statically (or dynamically).

	           SqConsole use this to parse data from command-line and
	           store parsed data in SqCommandValue.

	SqType
	|
	`--- SqCommand
 */

#define SQ_COMMAND_MEMBERS        \
    SQ_TYPE_MEMBERS;              \
	SqCommandFunc  handle;        \
	const char    *parameter;     \
	const char    *description

#ifdef __cplusplus
struct SqCommand : Sq::CommandMethod               // <-- 1. inherit C++ member function(method)
#else
struct SqCommand
#endif
{
	SQ_COMMAND_MEMBERS;                            // <-- 2. inherit member variable
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

	// ------ SqCommand members ------             // <-- 3. Add variable and non-virtual function in derived struct.
	SqCommandFunc  handle;
	const char    *parameter;
	const char    *description;
 */
};

/*	SqCommandValue: define a command that used by command line interface.
 */
#define SQ_COMMAND_VALUE_MEMBERS \
	const SqCommand  *type;      \
	SqPtrArray     shortcuts;    \
	SqPtrArray     arguments

#ifdef __cplusplus
struct SqCommandValue : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
struct SqCommandValue
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                      // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts & arguments are used by SqConsole
	SqPtrArray     shortcuts;    // sorted by SqOption.shortcut
	SqPtrArray     arguments;
 */

	/* Add variable and function */                // <-- 3. Add variable and non-virtual function in derived struct.
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
typedef struct SqCommand         Command;
typedef struct SqCommandValue    CommandValue;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_COMMAND_H
