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

void    sq_app_init(SqApp *app);
void    sq_app_final(SqApp *app);


#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	AppMethod : C++ struct is used by SqApp and it's children.
 */

struct AppMethod {
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqApp
 */

#define SQ_APP_MEMBERS                  \
	Sqdb                *db;            \
	const SqdbConfig    *db_config;     \
	const char          *db_database;   \
	const SqMigration  **migrations;    \
	int                  n_migrations;  \
	SqStorage           *storage

#ifdef __cplusplus
struct SqApp : Sq::AppMethod                  // <-- 1. inherit C++ member function(method)
#else
struct SqApp
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
typedef struct SqApp     App;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_APP_H
