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

void  sq_storage_init(SqStorage* storage, Sqdb* db);
void  sq_storage_final(SqStorage* storage);

int   sq_storage_open(SqStorage* storage, const char *database_name);
int   sq_storage_close(SqStorage* storage);

// CRUD functions: user must specify one of 'table_name' or 'type_name'

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
// StorageMethod : C++ struct is used by SqStorage and it's children.

#ifdef __cplusplus

namespace Sq
{

struct StorageMethod
{
	void  init(Sqdb* db);
	void  final(void);

	int   open(const char *database_name);
	int   close(void);

	template <class StructType>
	StructType* get(int id);
	void*       get(const char *table_name, int id);

	template <class StructType>
	void* get_all(const SqType *container);
	void* get_all(const char *table_name, const SqType *container);

	template <class StructType>
	int   insert(void* instance);
	int   insert(const char *table_name, void* instance);

	template <class StructType>
	void  update(void* instance);
	void  update(const char *table_name, void* instance);

	template <class StructType>
	void  remove(int id);
	void  remove(const char *table_name, int id);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// SqStorage

#ifdef __cplusplus
struct SqStorage : Sq::StorageMethod
#else
struct SqStorage
#endif
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
};


// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{

int   StorageMethod::open(const char *database_name)
	{ return sq_storage_open((SqStorage*)this, database_name); }
int   StorageMethod::close(void)
	{ return sq_storage_close((SqStorage*)this); }

template <class StructType>
StructType* StorageMethod::get(int id)
	{ return (StructType*)sq_storage_get((SqStorage*)this, NULL, typeid(StructType).name(), id); }
void*       StorageMethod::get(const char *table_name, int id)
	{ return (void*)sq_storage_get((SqStorage*)this, table_name, NULL, id); }

template <class StructType>
void* StorageMethod::get_all(const SqType *container)
	{ return (StructType*)sq_storage_get_all((SqStorage*)this, NULL, typeid(StructType).name(), container); }
void* StorageMethod::get_all(const char *table_name, const SqType *container)
	{ return (void*)sq_storage_get_all((SqStorage*)this, table_name, NULL, container); }

template <class StructType>
int   StorageMethod::insert(void* instance)
	{ return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), instance); }
int   StorageMethod::insert(const char *table_name, void* instance)
	{ return sq_storage_insert((SqStorage*)this, table_name, NULL, instance); }

template <class StructType>
void  StorageMethod::update(void* instance)
	{ sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), instance); }
void  StorageMethod::update(const char *table_name, void* instance)
	{ sq_storage_update((SqStorage*)this, table_name, NULL, instance); }

template <class StructType>
void StorageMethod::remove(int id)
	{ sq_storage_remove((SqStorage*)this, NULL, typeid(StructType).name(), id); }
void StorageMethod::remove(const char *table_name, int id)
	{ sq_storage_remove((SqStorage*)this, table_name, NULL, id); }

// This is for directly use only. You can NOT derived it.
struct Storage : SqStorage
{
	Storage()
		{ }
	Storage(Sqdb* db)
		{ sq_storage_init((SqStorage*)this, db); }

	~Storage()
		{ sq_storage_final((SqStorage*)this); }
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STORAGE
