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

#ifndef SQ_COMMAND_MAKE_H
#define SQ_COMMAND_MAKE_H

#include <SqCommand.h>
#include <SqCommandCommon.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqCommandMake        SqCommandMake;

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
// C/C++ common definitions: define structure

/*	SqCommandMake: make command

	SqCommandValue
	|
	`--- SqCommandCommon
	     |
	     `--- SqCommandMake
 */

#define SQ_COMMAND_MAKE_MEMBERS      \
	SQ_COMMAND_COMMON_MEMBERS;       \
	char          *table_to_create;  \
	char          *table_to_migrate

#ifdef __cplusplus
struct SqCommandMake : Sq::CommandValueMethod      // <-- 1. inherit C++ member function(method)
#else
struct SqCommandMake
#endif
{
	SQ_COMMAND_MAKE_MEMBERS;                       // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts is an array that sorted by SqOption.shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;

	// ------ SqCommandCommon members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version

	// ------ SqCommandMigrate members ------      // <-- 3. Add variable and non-virtual function in derived struct.
	char          *table_to_create;
	char          *table_to_migrate;
 */
};


#endif  // SQ_COMMAND_MAKE_H
