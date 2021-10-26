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

#ifndef COMMON_OPTIONS_H
#define COMMON_OPTIONS_H

#include <SqCommand.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct CommandCommon        CommandCommon;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqOption  common_option_array[];    // common option array
extern const SqOption *common_options[];         // common option pointer array
extern const int       n_common_options;

#define COMMON_OPTION_HELP       (&common_option_array[0])
#define COMMON_OPTION_QUIET      (&common_option_array[1])
#define COMMON_OPTION_VERSION    (&common_option_array[2])

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

#define COMMON_OPTION_MEMBERS     \
	bool           help;          \
	bool           quiet;         \
    bool           version

/*	CommandCommon: SqCommandValue + common command options
 */

#define COMMAND_COMMON_MEMBERS    \
    SQ_COMMAND_VALUE_MEMBERS;     \
	COMMON_OPTION_MEMBERS

#ifdef __cplusplus
struct CommandCommon : Sq::CommandValueMethod      // <-- 1. inherit C++ member function(method)
#else
struct CommandCommon
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
/*	// ------ CommandCommon members ------         // <-- 3. Add variable and non-virtual function in derived struct.
	bool           help;
	bool           quiet;
    bool           version;    // Display this application version
 */
};

#endif  // COMMON_OPTIONS_H
