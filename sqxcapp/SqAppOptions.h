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

#ifndef SQ_APP_OPTIONS_H
#define SQ_APP_OPTIONS_H

#include <SqCommand.h>

/*
	SqCommonOptions
	|
	+--- SqListOptions       (defined in SqAppTool.c)
	|
	+--- SqMakeOptions
	|
	`--- SqMigrateOptions
 */

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqCommonOptions     SqCommonOptions;
typedef struct SqMakeOptions       SqMakeOptions;
typedef struct SqMigrateOptions    SqMigrateOptions;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

// SqOption for SqCommonOptions
extern const SqOption SqCommonOption_Help_;
extern const SqOption SqCommonOption_Quiet_;
extern const SqOption SqCommonOption_Version_;

#define SQ_COMMON_OPTION_HELP       (&SqCommonOption_Help_)
#define SQ_COMMON_OPTION_QUIET      (&SqCommonOption_Quiet_)
#define SQ_COMMON_OPTION_VERSION    (&SqCommonOption_Version_)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqCommonOptions: common option values */

#define SQ_COMMON_OPTIONS_MEMBERS      \
	bool           help;               \
	bool           quiet;              \
    bool           version

struct SqCommonOptions
{
	SQ_COMMON_OPTIONS_MEMBERS;
/*	// ------ SqCommonOptions members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */
};

/*	SqMakeOptions: make command option values

	SqCommonOptions
	|
	`--- SqMakeOptions
 */

#define SQ_MAKE_OPTIONS_MEMBERS        \
	SQ_COMMON_OPTIONS_MEMBERS;         \
	char          *table_to_create;    \
	char          *table_to_migrate

struct SqMakeOptions
{
	SQ_MAKE_OPTIONS_MEMBERS;
/*	// ------ SqCommonOptions members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version

	// ------ SqMakeOptions members ------
	char          *table_to_create;
	char          *table_to_migrate;
 */
};

/*	SqMigrateOptions: migrate command option values

	SqCommonOptions
	|
	`--- SqMigrateOptions
 */

#define SQ_MIGRATE_OPTIONS_MEMBERS     \
	SQ_COMMON_OPTIONS_MEMBERS;         \
	char          *database;           \
	int            step

struct SqMigrateOptions
{
	SQ_MIGRATE_OPTIONS_MEMBERS;
/*	// ------ SqCommonOptions members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version

	// ------ SqMigrateOptions members ------
	char          *database;
	int            step;
 */
};

#endif  // SQ_APP_OPTIONS_H
