/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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
#include <SqType-stl-cpp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqStorage         SqStorage;

typedef struct SqQuery           SqQuery;    // define in SqQuery.h

// ----------------------------------------------------------------------------
// SqStorage C Functions

SqStorage* sq_storage_new(Sqdb* db);
void       sq_storage_free(SqStorage* storage);

void  sq_storage_init(SqStorage* storage, Sqdb* db);
void  sq_storage_final(SqStorage* storage);

int   sq_storage_open(SqStorage* storage, const char *database_name);
int   sq_storage_close(SqStorage* storage);

int   sq_storage_migrate(SqStorage* storage, SqSchema* schema);

// CRUD functions: user must specify one of 'table_name' or 'type_name'

void* sq_storage_get(SqStorage* storage,
                     const char *table_name,
                     const char *type_name,
                     int   id);

// void* sq_storage_get_all(SqStorage* storage,
//                          const char *table_name,
//                          const char *type_name,
//                          const SqType *container);
#define sq_storage_get_all(storage, table_name, type_name, container)    \
		sq_storage_get_by_sql(storage, table_name, type_name, container, NULL);

// This function will generate below SQL statement to get rows
// SELECT * FROM table_name + 'sql_where_having'
void* sq_storage_get_by_sql(SqStorage* storage,
                            const char *table_name,
                            const char *type_name,
                            const SqType *container,
                            const char *sql_where_having);

// return id if no error
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

// find SqTable by SqTable.name
#define  sq_storage_find(storage, table_name)    sq_schema_find((storage)->schema, table_name)

// find SqTable by SqType.name
SqTable* sq_storage_find_by_type(SqStorage* storage, const char *type_name);

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
	/*
	void  init(Sqdb* db);
	void  init(DbMethod* dbmethod);
	void  final(void);
	*/

	int   open(const char *database_name);
	int   close(void);

	int   migrate(SqSchema* schema);

	template <class StructType>
	StructType* get(int id);
	void*       get(const char *table_name, int id);

	template <class Element, class StlContainer>
	StlContainer* getBySql(const char* sql_where_having);
	template <class StlContainer>
	StlContainer* getBySql(const char* sql_where_having);
	template <class StructType>
	void* getBySql(const SqType *container, const char* sql_where_having);
	void* getBySql(const char *table_name, const SqType *container, const char* sql_where_having);

	template <class Element, class StlContainer>
	StlContainer* getAll();
	template <class StlContainer>
	StlContainer* getAll();
	template <class StructType>
	void* getAll(const SqType *container);
	void* getAll(const char *table_name, const SqType *container);

	template <class StructType>
	int   insert(StructType& instance);
	template <class StructType>
	int   insert(StructType* instance);
	template <class StructType>
	int   insert(void* instance);
	int   insert(const char *table_name, void* instance);

	template <class StructType>
	void  update(StructType& instance);
	template <class StructType>
	void  update(StructType* instance);
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
	Sqdb*      db;          // database

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

namespace Sq {

inline int   StorageMethod::open(const char *database_name) {
	return sqdb_open(((SqStorage*)this)->db, database_name);
}
inline int   StorageMethod::close(void) {
	return sqdb_close(((SqStorage*)this)->db);
}

inline int   StorageMethod::migrate(SqSchema* schema) {
	return sqdb_migrate(((SqStorage*)this)->db, ((SqStorage*)this)->schema, schema);
}

template <class StructType>
inline StructType* StorageMethod::get(int id) {
	return (StructType*)sq_storage_get((SqStorage*)this, NULL, typeid(StructType).name(), id);
}
inline void*       StorageMethod::get(const char *table_name, int id) {
	return (void*)sq_storage_get((SqStorage*)this, table_name, NULL, id);
}

template <class StlContainer>
inline StlContainer* StorageMethod::getBySql(const char* sql_where_having) {
	SqTable* table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<typename StlContainer::value_type>::type).name());
	if (table == NULL)
		return NULL;
	SqType*  containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer* instance = (StlContainer*) sq_storage_get_by_sql((SqStorage*)this, table->name, NULL, containerType, sql_where_having);
	delete containerType;
	return instance;
}
template <class ElementType, class StlContainer>
inline StlContainer* StorageMethod::getBySql(const char* sql_where_having) {
	SqTable* table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<ElementType>::type).name());
	if (table == NULL)
		return NULL;
	SqType*  containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer* instance = (StlContainer*) sq_storage_get_by_sql((SqStorage*)this, table->name, NULL, containerType, sql_where_having);
	delete containerType;
	return instance;
}

template <class StructType>
inline void* StorageMethod::getBySql(const SqType *container, const char* sql_where_having) {
	return (StructType*)sq_storage_get_by_sql((SqStorage*)this, NULL, typeid(StructType).name(), container, sql_where_having);
}
inline void* StorageMethod::getBySql(const char *table_name, const SqType *container, const char* sql_where_having) {
	return (void*)sq_storage_get_by_sql((SqStorage*)this, table_name, NULL, container, sql_where_having);
}

template <class StlContainer>
inline StlContainer* StorageMethod::getAll() {
	SqTable* table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<typename StlContainer::value_type>::type).name());
	if (table == NULL)
		return NULL;
	SqType*  containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer* instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, NULL, containerType);
	delete containerType;
	return instance;
}
template <class ElementType, class StlContainer>
inline StlContainer* StorageMethod::getAll() {
	SqTable* table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<ElementType>::type).name());
	if (table == NULL)
		return NULL;
	SqType*  containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer* instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, NULL, containerType);
	delete containerType;
	return instance;
}

template <class StructType>
inline void* StorageMethod::getAll(const SqType *container) {
	return (StructType*)sq_storage_get_all((SqStorage*)this, NULL, typeid(StructType).name(), container);
}
inline void* StorageMethod::getAll(const char *table_name, const SqType *container) {
	return (void*)sq_storage_get_all((SqStorage*)this, table_name, NULL, container);
}

template <class StructType>
inline int   StorageMethod::insert(StructType& instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), &instance);
}
template <class StructType>
inline int   StorageMethod::insert(StructType* instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
template <class StructType>
inline int   StorageMethod::insert(void* instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
inline int   StorageMethod::insert(const char *table_name, void* instance) {
	return sq_storage_insert((SqStorage*)this, table_name, NULL, instance);
}

template <class StructType>
inline void  StorageMethod::update(StructType& instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), &instance);
}
template <class StructType>
inline void  StorageMethod::update(StructType* instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
template <class StructType>
inline void  StorageMethod::update(void* instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
inline void  StorageMethod::update(const char *table_name, void* instance) {
	sq_storage_update((SqStorage*)this, table_name, NULL, instance);
}

template <class StructType>
inline void StorageMethod::remove(int id) {
	sq_storage_remove((SqStorage*)this, NULL, typeid(StructType).name(), id);
}
inline void StorageMethod::remove(const char *table_name, int id) {
	sq_storage_remove((SqStorage*)this, table_name, NULL, id);
}

// This is for directly use only. You can NOT derived it.
struct Storage : SqStorage
{
	Storage(Sqdb* db) {
		sq_storage_init((SqStorage*)this, db);
	}
	Storage(DbMethod* dbmethod) {
		sq_storage_init((SqStorage*)this, (Sqdb*)dbmethod);
	}

	~Storage() {
		sq_storage_final((SqStorage*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STORAGE
