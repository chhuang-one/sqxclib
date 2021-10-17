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

#ifndef SQ_CONSOLE_H
#define SQ_CONSOLE_H

#include <SqCommand.h>    // typedef struct SqConsole

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

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

void       sq_console_add(SqConsole *console, const SqCommandType *command_type);

SqCommandType  *sq_console_find(SqConsole *console, const char* name);
SqCommandValue *sq_console_parse(SqConsole *console, int argc, char **argv, bool argv_has_command);

void       sq_console_print_help(SqConsole  *console,
                                 const char *command_name,
                                 const char *program_name);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- declare methods for Sq::Console --- */
struct ConsoleMethod {
	void  add(const SqCommandType *command_type);
	void  printHelp(const char *command_name, const char *program_name);

	SqCommandType  *find(const char *name);
	SqCommandValue *parse(int argc, char **argv, bool argv_has_command = true);
};

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqConsole: command-line interface
 */

#define SQ_CONSOLE_MEMBERS         \
	SqPtrArray commands;           \
	bool       commands_sorted;    \
	Sqxc      *xc_input;           \
	SqBuffer   buf

#ifdef __cplusplus
struct SqConsole : Sq::ConsoleMethod           // <-- 1. inherit C++ member function(method)
#else
struct SqConsole
#endif
{
//	SQ_CONSOLE_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ SqConsole members ------  */
	SqPtrArray commands;
	bool       commands_sorted;

	Sqxc      *xc_input;
	SqBuffer   buf;

#ifdef __cplusplus
	SqConsole() {
		sq_console_init(this);
	}
	~SqConsole() {
		sq_console_final(this);
	}
#endif  // __cplusplus
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

/* --- define methods for Sq::Relation --- */
inline void  ConsoleMethod::add(const SqCommandType *command_type) {
	sq_console_add((SqConsole*)this, command_type);
}
inline void  ConsoleMethod::printHelp(const char *command_name, const char *program_name) {
	sq_console_print_help((SqConsole*)this, command_name, program_name);
}
inline SqCommandType *ConsoleMethod::find(const char *name) {
	return sq_console_find((SqConsole*)this, name);
}
inline SqCommandValue *ConsoleMethod::parse(int argc, char **argv, bool argv_has_command) {
	return sq_console_parse((SqConsole*)this, argc, argv, argv_has_command);
}


/* --- define C++11 standard-layout structures --- */
typedef struct SqConsole         Console;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_CONSOLE_H
