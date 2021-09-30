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

#ifndef SQ_MIGRATION_H
#define SQ_MIGRATION_H

#include <SqPtrArray.h>
#include <SqSchema.h>
#include <SqStorage.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqMigration         SqMigration;
typedef struct SqMigrationTable    SqMigrationTable;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SqMigrationFunc)(SqSchema *schema, SqStorage *storage);

extern const SqType SqType_migration_table_;

#define SQ_TYPE_MIGRATION_TABLE    (&SqType_migration_table_)
#define SQ_MIGRATION_TABLE_NAME    "migrations"

/* --- SqMigrationTable functions --- */

// install migrations table
int  sq_migration_install(Sqdb *db);

int  sq_migration_get_last(SqStorage *storage, int *batch);
int  sq_migration_count_batch(SqStorage *storage, int batch);

int  sq_migration_insert(SqStorage *storage, SqMigration **migrations, int begin, int n, int batch);
void sq_migration_remove(SqStorage *storage, int begin);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {
}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

struct SqMigration {
	SqMigrationFunc  up;
	SqMigrationFunc  down;
	const char      *name;
};

struct SqMigrationTable {
	int   id;           // PRIMARY KEY
	char *migration;    // VARCHAR(255)
	int   batch;
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
typedef struct SqMigration        Migration;
typedef struct SqMigrationTable   MigrationTable;

/* --- sample code --- use C++ lambda functions
Sq::Migration CreateUsersTable = {
	.up = [](SqSchema *schema, SqStorage *storage) {
	},

	.down = [](SqSchema *schema, SqStorage *storage) {
	},

#ifdef SQ_TOOL
	.name = "2021_07_06_000000_create_users_table",
#endif
};
 */

}  // namespace Sq
#endif  // __cplusplus


#endif  // SQ_MIGRATION_H
