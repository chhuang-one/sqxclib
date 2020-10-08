/*
 *   Copyright (C) 2020 by C.H. Huang
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


#ifndef SQ_STORAGE
#define SQ_STORAGE

#include <sqlite3.h>
#include <Sqdb.h>
#include <SqSchema.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqStorage         SqStorage;

typedef struct SqQuery           SqQuery;    // declare in SqQuery.h

// ----------------------------------------------------------------------------
// SqStorage C Functions

SqStorage* sq_storage_new(Sqdb* db);
void       sq_storage_free(SqStorage* storage);

int   sq_storage_open(SqStorage* storage, const char *database_name);
int   sq_storage_close(SqStorage* storage);

void* sq_storage_get(SqStorage* storage,
                     const char *table_name,
                     const char *type_name,
                     int   id);

void* sq_storage_get_all(SqStorage* storage,
                         const char *table_name,
                         const char *type_name,
                         const SqType *container);

// return -1 if error occurred
int   sq_storage_insert(SqStorage* storage,
                        const char *table_name,
                        const char *type_name,
                        void* instance);

void  sq_storage_update(SqStorage* storage,
                        const char *table_name,
                        const char *type_name,
                        void* instance);

void  sq_storage_remove(SqStorage* storage,
                        const char *table_name,
                        const char *type_name,
                        int   id);

// ------------------------------------
SqQuery* sq_storage_table(SqStorage* storage, const char *table_name);
SqQuery* sq_storage_type(SqStorage* storage, const char *type_name);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// SqStorage

struct SqStorage
{
	Sqdb*      db;

	SqSchema*  schema;      // current schema

	// tables is sorted by SqTable.type.name
	SqPtrArray tables;
    int        tables_version;

	// 1 thread use 1 Sqxc chain
	Sqxc*      xc_input;    // SqxcValue
	Sqxc*      xc_output;   // SqxcSql

	const SqType*   container_default;

#ifdef __cplusplus
	// C++11 standard-layout

#endif  // __cplusplus
};


// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqStorage          Storage;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STORAGE
