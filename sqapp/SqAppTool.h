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

#ifndef SQ_APP_TOOL_H
#define SQ_APP_TOOL_H

#include <SqApp.h>
#include <SqConsole.h>
#include <SqPairs.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqAppTool         SqAppTool;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- SqAppTool functions --- */
void    sq_app_tool_init(SqAppTool *app, const char *program_name);
void    sq_app_tool_final(SqAppTool *app);

// 'template_filename' = "migration-create.c.txt"
// 'migration_name'    = "create_users_table"
// key / value in 'pairs'
// key = "struct_name"      value = "User"
// key = "table_name"       value = "users"
// key = "timestamp"        value = "2021_10_10_010203"
int     sq_app_tool_make_migration(SqAppTool  *app,
                                   const char *template_filename,
                                   const char *migration_name,
                                   SqPairs    *pairs);

void    sq_app_tool_print_path();

/* --- template functions --- */
char *sq_template_write_buffer(const char *template_string, SqPairs *pairs, SqBuffer *result_buffer);
int   sq_template_write_file(const char *template_file, SqPairs *pairs, const char *result_file);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	AppMethod : C++ struct is used by SqApp and it's children.
 */

struct AppToolMethod : AppMethod {
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqAppTool

    SqApp
    |
    `--- SqAppTool
 */

#ifdef __cplusplus
struct SqAppTool : Sq::AppToolMethod          // <-- 1. inherit C++ member function(method)
#else
struct SqAppTool
#endif
{
	SQ_APP_MEMBERS;                           // <-- 2. inherit member variable
/*	// ------ SqApp members ------
	Sqdb                *db;
	SqdbConfig          *db_config;
	const char          *db_database;
	const SqMigration  **migrations;
	int                  n_migrations;
	SqStorage           *storage;
 */

	// ------ SqAppTool members ------        // <-- 3. Add variable and non-virtual function in derived struct.

	SqConsole           *console;

	// Key-Value Pairs
	SqPairs              pairs;
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
/* --- define C++11 standard-layout structures --- */
typedef struct SqAppTool     AppTool;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_APP_TOOL_H
