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


// sqxc - Structured Query (Language) Library
// DataBase object - [Server Name].[DataBase Name].[Schema].[Table Name]

#ifndef SQ_STORAGE
#define SQ_STORAGE

#include <sqlite3.h>
#include <SqxcSqlite.h>

#include <SqSchema.h>
#include <SqxcValue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqQuery           SqQuery;
typedef struct SqStorage         SqStorage;

extern SqStorage*  sq_storage_global_;

// ----------------------------------------------------------------------------
// SqStorage C Functions

SqStorage* sq_storage_new();
void       sq_storage_free(SqStorage* storage);

SqQuery* sq_storage_table(SqStorage* storage, const char* table_name);
SqQuery* sq_storage_type(SqStorage* storage, const char* type_name);

void* sq_storage_get(SqStorage* storage,
                     const char* table_name,
                     const char* type_name,
                     int   id);

void* sq_storage_get_all(SqStorage* storage,
                         const char* table_name,
                         const char* type_name,
                         SqType* container);

// TODO return id
int   sq_storage_insert(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance);

void  sq_storage_update(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance);

void  sq_storage_remove(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        int   id);

int   sq_storage_open(SqStorage* storage, const char* path_or_name);

int   sq_storage_exec(SqStorage* storage, const char* sql);

void  sq_storage_prepare(SqStorage* storage, SqQuery* query);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// StorageMethod : C++ struct is used by SqStorage and it's children.

#ifdef __cplusplus

namespace Sq
{

struct StorageMethod
{
	SqTable* table(const char* table_name);

	template<class Type>
	SqTable* table();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// SqStorage

struct SqStorage
{
	SqSchema*  schema;    // default schema

	char*      name;
//	Migration
	const SqType*   container_default;

	// 1 thread 1 Sqxc chain
	struct {
		Sqxc*       src;
		SqxcSqlite* sql;
		Sqxc*       json;
		SqxcValue*  value;
	} xc[2];   // output xc[0], input xc[1]

	struct {
		const SqxcInfo*  sql;     // sqlite, mysql...etc
		const SqxcInfo*  json;    // json-c
		const SqxcInfo*  value;   // value
	} xcinfo;

	sqlite3*       db;

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
