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

#ifndef COMMAND_MIGRATE_H
#define COMMAND_MIGRATE_H

#include <SqCommand.h>
#include <CommonOptions.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct CommandMigrate        CommandMigrate;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

void  sq_console_add_command_migrate(SqConsole *console);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	CommandMigrate: migrate command

	SqCommandValue
	|
	`--- CommandCommon    (CommonOptions.h)
	     |
	     `--- CommandMigrate
 */

#ifdef __cplusplus
struct CommandMigrate : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
struct CommandMigrate
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                      // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts is an array that sorted by SqOption.shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;
 */

	COMMON_OPTION_MEMBERS;
/*	// ------ CommandCommon members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */

	// ------ CommandMigrate members ------        // <-- 3. Add variable and non-virtual function in derived struct.
	int            step;
};


#endif  // End of COMMAND_MIGRATE_H
