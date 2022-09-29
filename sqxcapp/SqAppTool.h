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

#ifndef SQ_APP_TOOL_H
#define SQ_APP_TOOL_H

#include <SqApp.h>
#include <SqConsole.h>
#include <SqPairs.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqAppTool         SqAppTool;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- SqAppTool functions --- */

void    sq_app_tool_init(SqAppTool *app, const char *program_name, const struct SqAppSetting *setting);
void    sq_app_tool_final(SqAppTool *app);

int     sq_app_tool_run(SqAppTool *app, int argc, char **argv);

// set workspace folder and save it in app->path
void    sq_app_tool_set_path(SqAppTool *app, const char *path);

// decide workspace folder and save it in app->path
int     sq_app_tool_decide_path(SqAppTool *app);

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

/*	AppToolMethod is used by SqAppTool and it's children.

	AppMethod
	|
	`--- AppToolMethod

	It's derived struct/class must be C++11 standard-layout and has SqAppTool members.
 */
struct AppToolMethod : AppMethod {
	void init(const char *program_name, const struct SqAppSetting *setting = SQ_APP_DEFAULT);
	void final();

	int  run(int argc, char **argv);

	int  makeMigration(const char *template_filename,
	                   const char *migration_name,
	                   SqPairs    *pairs);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqAppTool is used by command-line program - sqxctool and sqxcpptool.

	SqApp
	|
	`--- SqAppTool
 */

#define SQ_APP_TOOL_MEMBERS                \
	SQ_APP_MEMBERS;                        \
	const char     *template_extension;    \
	char           *path;                  \
	SqConsole      *console;               \
	SqBuffer        buffer;                \
	SqPairs         pairs

#ifdef __cplusplus
struct SqAppTool : Sq::AppToolMethod         // <-- 1. inherit C++ member function(method)
#else
struct SqAppTool
#endif
{
	SQ_APP_TOOL_MEMBERS;                     // <-- 2. inherit member variable
/*	// ------ SqApp members ------
	Sqdb                *db;
	SqdbConfig          *db_config;
	const char          *db_database;
	const SqMigration  **migrations;
	int                  n_migrations;
	SqStorage           *storage;

	// ------ SqAppTool members ------
	const char     *template_extension;    // template file extension
	char           *path;       // workspace folder
	SqConsole      *console;
	SqBuffer        buffer;     // buffer for temporary use
	SqPairs         pairs;    	// Key-Value pairs for temporary use
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

/* define AppToolMethod functions. */

inline void AppToolMethod::init(const char *program_name, const struct SqAppSetting *setting) {
	sq_app_tool_init((SqAppTool*)this, program_name, setting);
	((SqAppTool*)this)->template_extension = ".cpp.txt";
}
inline void AppToolMethod::final() {
	sq_app_tool_final((SqAppTool*)this);
}

inline int  AppToolMethod::run(int argc, char **argv) {
	return sq_app_tool_run((SqAppTool*)this, argc, argv);
}
inline int  AppToolMethod::makeMigration(const char *template_filename,
                                         const char *migration_name,
                                         SqPairs    *pairs)
{
	return sq_app_tool_make_migration((SqAppTool*)this,
			template_filename, migration_name, pairs);
}

/* All derived struct/class must be C++11 standard-layout. */

struct AppTool : SqAppTool {
	AppTool(const char *program_name) {
		init(program_name);
	}
	~AppTool() {
		final();
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_APP_TOOL_H
