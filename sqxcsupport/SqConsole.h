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

enum SqConsoleParsingMode {
	SQ_CONSOLE_PARSE_ALL,
	SQ_CONSOLE_PARSE_AUTO,
	SQ_CONSOLE_PARSE_OPTION,
};

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

// The parameter 'parsing_mode' has 3 mode:
// SQ_CONSOLE_PARSE_ALL    if you need to parse command, options, and arguments.
// SQ_CONSOLE_PARSE_AUTO   if you want to parse both a command or just options.
// SQ_CONSOLE_PARSE_OPTION if you only need to parse options and arguments and no command.
SqCommandValue *sq_console_parse(SqConsole *console, int argc, char **argv, int parsing_mode);

void       sq_console_print_options(const SqConsole *console, SqOption **options, unsigned int n_options);

// if 'command' is NULL, function use first added SqCommand to print help message without name of command.
void       sq_console_print_help(const SqConsole *console, const SqCommand *command);

// print command list
void       sq_console_print_list(const SqConsole *console, const char *program_description);

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
	void  printOptions(SqOption **options, unsigned int nOptions) const;
	void  printOptions(Sq::OptionMethod **options, unsigned int nOptions) const;
	void  printHelp(const SqCommand *command = NULL) const;
	void  printHelp(const Sq::CommandMethod *command = NULL) const;
	void  printList(const char *programDescription) const;

	Sq::Command      *find(const char *name);
	Sq::CommandValue *parse(int argc, char **argv, int parsingMode = SQ_CONSOLE_PARSE_ALL);
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
	Sqxc      *xc_input

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
inline void  ConsoleMethod::printOptions(SqOption **options, unsigned int nOptions) const {
	sq_console_print_options((SqConsole*)this, options, nOptions);
}
inline void  ConsoleMethod::printOptions(Sq::OptionMethod **options, unsigned int nOptions) const {
	sq_console_print_options((SqConsole*)this, (SqOption**)options, nOptions);
}
inline void  ConsoleMethod::printHelp(const SqCommand *command) const {
	sq_console_print_help((SqConsole*)this, command);
}
inline void  ConsoleMethod::printHelp(const Sq::CommandMethod *command) const {
	sq_console_print_help((SqConsole*)this, (const SqCommand*)command);
}
inline void  ConsoleMethod::printList(const char *programDescription) const {
	sq_console_print_list((SqConsole*)this, programDescription);
}

inline Sq::Command *ConsoleMethod::find(const char *name) {
	return (Sq::Command*)sq_console_find((SqConsole*)this, name);
}
inline Sq::CommandValue *ConsoleMethod::parse(int argc, char **argv, int parsingMode) {
	return (Sq::CommandValue*)sq_console_parse((SqConsole*)this, argc, argv, parsingMode);
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
