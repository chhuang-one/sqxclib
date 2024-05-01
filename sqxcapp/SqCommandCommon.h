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

#ifndef SQ_COMMAND_COMMON_H
#define SQ_COMMAND_COMMON_H

#include <SqCommand.h>

/*
	SqCommandValue
	|
	`--- SqCommandCommon
	     |
	     +--- SqCommandList    (defined in SqAppTool.c)
	     |
	     +--- SqCommandMake
	     |
	     `--- SqCommandMigrate
 */

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqCommandCommon        SqCommandCommon;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqOption SqOption_CommandCommonHelp_;
extern const SqOption SqOption_CommandCommonQuiet_;
extern const SqOption SqOption_CommandCommonVersion_;

#define SQ_OPTION_COMMAND_COMMON_HELP       (&SqOption_CommandCommonHelp_)
#define SQ_OPTION_COMMAND_COMMON_QUIET      (&SqOption_CommandCommonQuiet_)
#define SQ_OPTION_COMMAND_COMMON_VERSION    (&SqOption_CommandCommonVersion_)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqCommandCommon: SqCommandValue + common command options

	SqCommandValue
	|
	`--- SqCommandCommon
 */

#define SQ_COMMAND_COMMON_MEMBERS    \
	SQ_COMMAND_VALUE_MEMBERS;        \
	bool           help;             \
	bool           quiet;            \
    bool           version

#ifdef __cplusplus
struct SqCommandCommon : Sq::CommandValueMethod    // <-- 1. inherit C++ member function(method)
#else
struct SqCommandCommon
#endif
{
	SQ_COMMAND_COMMON_MEMBERS;                     // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// 'shortcuts' is an array that sorted by SqOption::shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;

	// The following are option values.

	// ------ SqCommandCommon members ------       // <-- 3. Add variable and non-virtual function in derived struct.
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */
};

#endif  // SQ_COMMAND_COMMON_H
