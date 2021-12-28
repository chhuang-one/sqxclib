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

#ifndef COMMAND_COMMON_H
#define COMMAND_COMMON_H

#include <SqCommand.h>

/*
	SqCommandValue
	|
	`--- CommandCommon
	     |
	     +--- CommandMake
	     |
	     `--- CommandMigrate
 */

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct CommandCommon        CommandCommon;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqOption command_common_help_option;
extern const SqOption command_common_quiet_option;
extern const SqOption command_common_version_option;

#define COMMAND_COMMON_OPTION_HELP       (&command_common_help_option)
#define COMMAND_COMMON_OPTION_QUIET      (&command_common_quiet_option)
#define COMMAND_COMMON_OPTION_VERSION    (&command_common_version_option)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	CommandCommon: SqCommandValue + common command options

	SqCommandValue
	|
	`--- CommandCommon
 */

#define COMMAND_COMMON_MEMBERS    \
    SQ_COMMAND_VALUE_MEMBERS;     \
	bool           help;          \
	bool           quiet;         \
    bool           version

#ifdef __cplusplus
struct CommandCommon : Sq::CommandValueMethod      // <-- 1. inherit C++ member function(method)
#else
struct CommandCommon
#endif
{
	COMMAND_COMMON_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts is an array that sorted by SqOption.shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;

	// ------ CommandCommon members ------         // <-- 3. Add variable and non-virtual function in derived struct.
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */
};

#endif  // COMMAND_COMMON_H
