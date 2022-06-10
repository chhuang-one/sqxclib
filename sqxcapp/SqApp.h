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

#ifndef SQ_APP_H
#define SQ_APP_H

#include <Sqdb.h>
#include <SqStorage.h>
#include <SqMigration.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqApp             SqApp;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* struct SqAppSetting for internal use only.
   used by SqApp.c and SqApp-config.c
 */
struct SqAppSetting {
	const SqdbInfo      *db_info;
	const SqdbConfig    *db_config;
	const char          *db_database;
	const SqMigration  **migrations;
	const int           *n_migrations;

    const char          *error;
};

// 'SQ_APP_DEFAULT' has database settings and migration data for user application.
extern const struct SqAppSetting   SqAppSetting_default_;
#define SQ_APP_DEFAULT           (&SqAppSetting_default_)

/* SqApp C functions */
SqApp *sq_app_new(const struct SqAppSetting *setting);
void   sq_app_free(SqApp *app);

void  sq_app_init(SqApp *app, const struct SqAppSetting *setting);
void  sq_app_final(SqApp *app);

// if db_database is NULL, open default database that specify in SqAppConfig.h
int   sq_app_open_database(SqApp *app, const char *db_database);
void  sq_app_close_database(SqApp *app);

// make schema by migrations (defined in ../database/migrations)
// if 'migration_id' is 0, 'migration_id' will use version of database schema.
// return SQCODE_DB_VERSION_0 : if the database vesion is 0 (no migrations have been done)
// return SQCODE_DB_VERSION_MISMATCH : if these migrations are not for this database
int   sq_app_make_schema(SqApp *app, int migration_id);

// if 'step' is 0:
// sq_app_migrate() will run all of your outstanding migrations
// sq_app_rollback() will roll back the latest migration operation
int   sq_app_migrate(SqApp *app, int step);
int   sq_app_rollback(SqApp *app, int step);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	AppMethod is used by SqApp and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqApp members.
 */
struct AppMethod {
	void  init(const struct SqAppSetting *setting = SQ_APP_DEFAULT);
	void  final();

	int   openDatabase(const char *db_database = NULL);
	void  closeDatabase(void);

	int   makeSchema(int migration_id = 0);

	int   migrate(int step = 0);
	int   rollback(int step = 0);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqApp use configuration file (SqApp-config.h) to initialize database and do migrations. It used by user's application.

	SqApp
	|
	`--- SqAppTool
 */

#define SQ_APP_MEMBERS                  \
	Sqdb                *db;            \
	const SqdbConfig    *db_config;     \
	const char          *db_database;   \
	const SqMigration  **migrations;    \
	int                  n_migrations;  \
	SqStorage           *storage

#ifdef __cplusplus
struct SqApp : Sq::AppMethod                 // <-- 1. inherit C++ member function(method)
#else
struct SqApp
#endif
{
	SQ_APP_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ SqApp members ------
	Sqdb                *db;
	SqdbConfig          *db_config;
	const char          *db_database;
	const SqMigration  **migrations;
	int                  n_migrations;
	SqStorage           *storage;
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

/* define AppMethod functions. */

inline void  AppMethod::init(const struct SqAppSetting *setting) {
	sq_app_init((SqApp*)this, setting);
}
inline void  AppMethod::final() {
	sq_app_final((SqApp*)this);
}

inline int   AppMethod::openDatabase(const char *db_database) {
	return sq_app_open_database((SqApp*)this, db_database);
}
inline void  AppMethod::closeDatabase(void) {
	sq_app_close_database((SqApp*)this);
}

inline int   AppMethod::makeSchema(int migration_id) {
	return sq_app_make_schema((SqApp*)this, migration_id);
}

inline int   AppMethod::migrate(int step) {
	return sq_app_migrate((SqApp*)this, step);
}
inline int   AppMethod::rollback(int step) {
	return sq_app_rollback((SqApp*)this, step);
}

/* All derived struct/class must be C++11 standard-layout. */

struct App : SqApp {
	App(const struct SqAppSetting *setting = SQ_APP_DEFAULT) {
		sq_app_init(this, setting);
	}
	~App() {
		sq_app_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_APP_H
