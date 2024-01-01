/*
 *   Copyright (C) 2021-2024 by C.H. Huang
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
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqConsole         SqConsole;
typedef struct SqCommand         SqCommand;
typedef struct SqCommandValue    SqCommandValue;

/*	SqCommand initializer macro. below is sample code:

	typedef struct MyCommandValue {
		SQ_COMMAND_VALUE_MEMBERS;
		int  option1_value;
		int  option2_value;
	} MyCommandValue;

	static void mycommand_handle(MyCommandValue *cmd_value, SqConsole *console, void *data) {
		// The function will be called when your command is executed.
	}

	static const SqOption *mycommand_options[] = {
		option1,
		option2,
	};

	const SqCommand  mycommand = SQ_COMMAND_INITIALIZER(
		MyCommandValue, 0,                             // StructureType, bit_field
		"mycommand",                                   // command string
		mycommand_options,                             // pointer array of SqOption
		mycommand_handle,                              // handle function
		"mycommand parameterName",                     // parameter string
		"mycommand description"                        // description string
	);
 */
#define SQ_COMMAND_INITIALIZER(StructType, bit_value, command_string, command_options, handle_func, parameter_string, description_string) \
{                                                                  \
	sizeof(StructType),                                            \
	(SqTypeFunc) NULL,                                             \
	(SqTypeFunc) NULL,                                             \
	sq_command_parse_option,                                       \
	NULL,                                                          \
	command_string,                                                \
	(SqEntry**) command_options,                                   \
	sizeof(command_options) / sizeof(SqOption*),                   \
	bit_value,                                                     \
	NULL,                                                          \
	                                                               \
	(SqCommandFunc) handle_func,                                   \
	parameter_string,                                              \
	description_string,                                            \
}

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SqCommandFunc)(SqCommandValue *cmd_value, SqConsole *console, void *data);

/* --- macro functions --- for maintaining C/C++ inline functions easily */
#define SQ_COMMAND_SET_PARAMETER(command, parameter)    \
		sq_type_set_str_addr((SqType*)command, (char**) &((SqCommand*)command)->parameter, parameter)

#define SQ_COMMAND_SET_DESCRIPTION(command, description)    \
		sq_type_set_str_addr((SqType*)command, (char**) &((SqCommand*)command)->description, description)

/* --- SqCommandValue C functions --- */
SqCommandValue *sq_command_value_new(const SqCommand *cmd_type);
void            sq_command_value_free(SqCommandValue *cmd_value);

void  sq_command_value_init(SqCommandValue *cmd_value, const SqCommand *cmd_type);
void  sq_command_value_final(SqCommandValue *cmd_value);


/* --- SqCommand C functions --- */
SqCommand *sq_command_new(const char *cmd_name);
void       sq_command_free(SqCommand *cmd_type);

void  sq_command_init_self(SqCommand *cmd_type, const char *cmd_name);
void  sq_command_final_self(SqCommand *cmd_type);

// copy data from static SqCommand to dynamic SqCommand. 'cmd_type_dest' must be raw memory.
// if 'cmd_type_dest' is NULL, function will create dynamic SqCommand.
// if 'option_free_func' is NULL, function will use default value - sq_option_free
// return dynamic SqCommand.
SqCommand *sq_command_copy_static(SqCommand       *cmd_type_dest,
                                  const SqCommand *static_cmd_type_src,
                                  SqDestroyFunc    option_free_func);

void  sq_command_add_option(SqCommand *cmd_type, const SqOption *option, int n_option);

// sort options by SqOption.shortcut and save result in array. this function is called by SqConsole
void  sq_command_sort_shortcuts(const SqCommand *cmd_type, SqPtrArray *array);

// SqCommand parse function
int   sq_command_parse_option(void *cmd_value, const SqType *cmd_type, Sqxc *src);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

struct Command;
struct CommandMethod;

/*	CommandValueMethod is used by SqCommandValue and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqCommandValue members.
 */
struct CommandValueMethod {
	void *operator new(size_t size);

	void  init(const SqCommand *cmd_type);
	void  init(const Sq::CommandMethod *cmd_type);
	void  final();
};

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqCommand: User can use SqCommand to define command and it's options
	           statically (or dynamically).

	           SqConsole use this to parse data from command-line and
	           store parsed data in SqCommandValue.

	SqType
	|
	`--- SqCommand

	SqCommand must have no base struct because I need use aggregate initialization with it.
 */

#define SQ_COMMAND_MEMBERS        \
	SQ_TYPE_MEMBERS;              \
	SqCommandFunc  handle;        \
	const char    *parameter;     \
	const char    *description

struct SqCommand
{
	SQ_COMMAND_MEMBERS;
/*	// ------ SqType members ------
	unsigned int   size;           // instance size

	SqTypeFunc     init;           // initialize instance
	SqTypeFunc     final;          // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to generate type name,
	// or use macro SQ_GET_TYPE_NAME()
	char          *name;

	// SqType::entry is array of SqEntry pointer if current SqType is for C struct type.
	// SqType::entry isn't freed if SqType::n_entry == -1
	SqEntry      **entry;          // SqPtrArray::data
	int            n_entry;        // SqPtrArray::length
	// macro SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry) expands to above 2 fields.

	// SqType::bit_field has SQB_TYPE_DYNAMIC if SqType is dynamic and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED  if SqType::entry is sorted.
	unsigned int   bit_field;

	// SqType::on_destroy() is called when program releases SqType.
	// This is mainly used for deriving or customizing SqType.
	// Instance of SqType will be passed to SqType::on_destroy()
	SqDestroyFunc  on_destroy;     // destroy notifier for SqType. It can be NULL.

	// ------ SqCommand members ------
	SqCommandFunc  handle;
	const char    *parameter;
	const char    *description;
 */

#ifdef __cplusplus
	/* Note: If you add, remove, or change methods here, do the same things in Sq::CommandMethod. */

	// initialize/finalize self
	void  initSelf(const char *cmd_name) {
		sq_command_init_self((SqCommand*)this, cmd_name);
	}
	void  finalSelf() {
		sq_command_final_self((SqCommand*)this);
	}

	void  setParameter(const char *parameter) {
		SQ_COMMAND_SET_PARAMETER(this, parameter);
	}
	void  setDescription(const char *description) {
		SQ_COMMAND_SET_DESCRIPTION(this, description);
	}

	// create dynamic SqCommand and copy data from static SqCommand
	Sq::Command *copyStatic(SqDestroyFunc option_free_func = NULL) {
		return (Sq::Command*)sq_command_copy_static(NULL, (const SqCommand*)this, option_free_func);
	}

	// add option from SqOption array (NOT pointer array) to dynamic SqCommand.
	void  addOption(const SqOption *option, int n_option = 1) {
		sq_command_add_option((SqCommand*)this, option, n_option);
	}
	void  addOption(const Sq::OptionMethod *option, int n_option = 1) {
		sq_command_add_option((SqCommand*)this, (const SqOption*)option, n_option);
	}

	void  sortShortcuts(SqPtrArray *array) {
		sq_command_sort_shortcuts((SqCommand*)this, array);
	}
#endif  // __cplusplus
};

/*	SqCommandValue: It stores value of option from command-line.
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

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_command_set_parameter(SqCommand *command, const char *parameter)
{
	SQ_COMMAND_SET_PARAMETER(command, parameter);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_command_set_description(SqCommand *command, const char *description)
{
	SQ_COMMAND_SET_DESCRIPTION(command, description);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

void  sq_command_set_parameter(SqCommand *command, const char *parameter);
void  sq_command_set_description(SqCommand *command, const char *description);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	CommandMethod is used by SqCommand's children.

	It's derived struct/class must be C++11 standard-layout and has SqCommand members.

	Note: If you add, remove, or change methods here, do the same things in SqCommand.
 */
struct CommandMethod {
	// initialize/finalize self
	void  initSelf(const char *cmd_name) {
		sq_command_init_self((SqCommand*)this, cmd_name);
	}
	void  finalSelf() {
		sq_command_final_self((SqCommand*)this);
	}

	void  setParameter(const char *parameter) {
		SQ_COMMAND_SET_PARAMETER(this, parameter);
	}
	void  setDescription(const char *description) {
		SQ_COMMAND_SET_DESCRIPTION(this, description);
	}

	// create dynamic SqCommand and copy data from static SqCommand
	Sq::Command *copyStatic(SqDestroyFunc option_free_func = NULL) {
		return (Sq::Command*)sq_command_copy_static(NULL, (const SqCommand*)this, option_free_func);
	}

	// add option from SqOption array (NOT pointer array) to dynamic SqCommand.
	void  addOption(const SqOption *option, int n_option = 1) {
		sq_command_add_option((SqCommand*)this, option, n_option);
	}
	void  addOption(const Sq::OptionMethod *option, int n_option = 1) {
		sq_command_add_option((SqCommand*)this, (const SqOption*)option, n_option);
	}

	void  sortShortcuts(SqPtrArray *array) {
		sq_command_sort_shortcuts((SqCommand*)this, array);
	}
};

/* define CommandValueMethod functions. */

inline void *CommandValueMethod::operator new(size_t size) {
	return calloc(1, size);
}

inline void  CommandValueMethod::init(const SqCommand *cmd_type) {
	sq_command_value_init((SqCommandValue*)this, cmd_type);
}
inline void  CommandValueMethod::init(const Sq::CommandMethod *cmd_type) {
	sq_command_value_init((SqCommandValue*)this, (const SqCommand*)cmd_type);
}
inline void  CommandValueMethod::final() {
	sq_command_value_final((SqCommandValue*)this);
}

/* All derived struct/class must be C++11 standard-layout. */

struct Command : SqCommand {
	// constructor
	Command() {}
	Command(const char *cmd_name) {
		sq_command_init_self(this, cmd_name);
	}
	// destructor
	~Command() {
		sq_command_final_self(this);
	}
};

struct CommandValue : SqCommandValue {
	// constructor
	CommandValue() {}
	CommandValue(const SqCommand *cmd_type) {
		sq_command_value_init(this, cmd_type);
	}
	CommandValue(const Sq::CommandMethod *cmd_type) {
		sq_command_value_init(this, (const SqCommand*)cmd_type);
	}
	// destructor
	~CommandValue() {
		sq_command_value_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_COMMAND_H
