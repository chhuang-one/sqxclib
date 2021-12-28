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

#ifndef COMMAND_MAKE_H
#define COMMAND_MAKE_H

#include <SqCommand.h>
#include <CommandCommon.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct CommandMake        CommandMake;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

void  sq_console_add_command_make(SqConsole *console);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	CommandMake: make command

	SqCommandValue
	|
	`--- CommandCommon
	     |
	     `--- CommandMake
 */

#ifdef __cplusplus
struct CommandMake : Sq::CommandValueMethod        // <-- 1. inherit C++ member function(method)
#else
struct CommandMake
#endif
{
	COMMAND_COMMON_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts is an array that sorted by SqOption.shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;

	// ------ CommandCommon members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */

	// ------ CommandMigrate members ------        // <-- 3. Add variable and non-virtual function in derived struct.
	char          *table_to_create;
	char          *table_to_migrate;
	int            step;
};


#endif  // COMMAND_MAKE_H
