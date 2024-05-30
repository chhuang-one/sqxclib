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

#ifndef SQ_CONSOLE_H
#define SQ_CONSOLE_H

#include <SqCommand.h>    // typedef struct SqConsole

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

// typedef struct SqConsole         SqConsole;    // declare in SqCommand.h

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

SqConsole *sq_console_new();
void       sq_console_free(SqConsole *console);

void       sq_console_init(SqConsole *console);
void       sq_console_final(SqConsole *console);

void       sq_console_add(SqConsole *console, const SqCommand *command_type);

SqCommand *sq_console_find(SqConsole *console, const char* command_name);

// argv_has_command ==  0  if you only need to parse options and arguments and no command.
// argv_has_command ==  1  if you need to parse command, options, and arguments.
// argv_has_command == -1  if you want to parse both a command or just options.
SqCommandValue *sq_console_parse(SqConsole *console, int argc, char **argv, int argv_has_command);

void       sq_console_print_options(SqConsole *console, SqOption **options, int n_options);

// if 'command' is NULL, function use first added SqCommand to print help message without name of command.
void       sq_console_print_help(SqConsole  *console, const SqCommand *command);

// print command list
void       sq_console_print_list(SqConsole  *console, const char *program_description);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	ConsoleMethod is used by SqConsole and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqConsole members.
*/
struct ConsoleMethod
{
	void  add(const SqCommand *commandType);
	void  add(const Sq::CommandMethod *commandType);
	void  printOptions(SqOption **options, int nOptions);
	void  printOptions(Sq::OptionMethod **options, int nOptions);
	void  printHelp(const SqCommand *command = NULL);
	void  printHelp(const Sq::CommandMethod *command = NULL);
	void  printList(const char *programDescription);

	Sq::Command      *find(const char *name);
	Sq::CommandValue *parse(int argc, char **argv, int argvHasCommand = 1);
};

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqConsole: command-line interface
 */

#define SQ_CONSOLE_MEMBERS         \
	SqPtrArray commands;           \
	bool       commands_sorted;    \
	SqCommand *command_default;    \
	char      *program_name;       \
	Sqxc      *xc_input;           \
	SqBuffer   buf

#ifdef __cplusplus
struct SqConsole : Sq::ConsoleMethod         // <-- 1. inherit C++ member function(method)
#else
struct SqConsole
#endif
{
	SQ_CONSOLE_MEMBERS;                      // <-- 2. inherit member variable
/*	// ------ SqConsole members ------
	SqPtrArray commands;
	bool       commands_sorted;
	SqCommand *command_default;    // default command
	char      *program_name;
	Sqxc      *xc_input;
	SqBuffer   buf;
 */
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

/* define ConsoleMethod functions. */

inline void  ConsoleMethod::add(const SqCommand *commandType) {
	sq_console_add((SqConsole*)this, commandType);
}
inline void  ConsoleMethod::add(const Sq::CommandMethod *commandType) {
	sq_console_add((SqConsole*)this, (const SqCommand*)commandType);
}
inline void  ConsoleMethod::printOptions(SqOption **options, int nOptions) {
	sq_console_print_options((SqConsole*)this, options, nOptions);
}
inline void  ConsoleMethod::printOptions(Sq::OptionMethod **options, int nOptions) {
	sq_console_print_options((SqConsole*)this, (SqOption**)options, nOptions);
}
inline void  ConsoleMethod::printHelp(const SqCommand *command) {
	sq_console_print_help((SqConsole*)this, command);
}
inline void  ConsoleMethod::printHelp(const Sq::CommandMethod *command) {
	sq_console_print_help((SqConsole*)this, (const SqCommand*)command);
}
inline void  ConsoleMethod::printList(const char *programDescription) {
	sq_console_print_list((SqConsole*)this, programDescription);
}

inline Sq::Command *ConsoleMethod::find(const char *name) {
	return (Sq::Command*)sq_console_find((SqConsole*)this, name);
}
inline Sq::CommandValue *ConsoleMethod::parse(int argc, char **argv, int argvHasCommand) {
	return (Sq::CommandValue*)sq_console_parse((SqConsole*)this, argc, argv, argvHasCommand);
}

/* All derived struct/class must be C++11 standard-layout. */

struct Console : SqConsole
{
	Console() {
		sq_console_init(this);
	}
	~Console() {
		sq_console_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_CONSOLE_H
