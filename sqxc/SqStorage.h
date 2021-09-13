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
#include <SqJoint.h>
#ifdef __cplusplus
#include <SqType-stl-cpp.h>
#endif

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqStorage         SqStorage;

typedef struct SqQuery           SqQuery;    // define in SqQuery.h

/* macro for maintaining C/C++ inline functions easily */

// int   sq_storage_begin(SqStorage *storage);
#define  SQ_STORAGE_BEGIN(storage)     (storage)->db->info->exec((storage)->db, "BEGIN", NULL, NULL);

// int   sq_storage_commit(SqStorage *storage);
#define  SQ_STORAGE_COMMIT(storage)    (storage)->db->info->exec((storage)->db, "COMMIT", NULL, NULL);

// int   sq_storage_rollback(SqStorage *storage);
#define  SQ_STORAGE_ROLLBACK(storage)  (storage)->db->info->exec((storage)->db, "ROLLBACK", NULL, NULL);

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- SqStorage C Functions --- */

SqStorage *sq_storage_new(Sqdb *db);
void       sq_storage_free(SqStorage *storage);

void  sq_storage_init(SqStorage *storage, Sqdb *db);
void  sq_storage_final(SqStorage *storage);

int   sq_storage_open(SqStorage *storage, const char *database_name);
int   sq_storage_close(SqStorage *storage);

// synchronize storage->schema to database if 'schema' == NULL (Mainly used by SQLite)
int   sq_storage_migrate(SqStorage *storage, SqSchema *schema);

/* ------------------------------------
   CRUD functions:
   1. can work if user only specify one of 'table_name' or 'type_name'.
   2. can run a bit faster if user specify 'table_name' and 'type' at the same time.
 */
void *sq_storage_get_full(SqStorage    *storage,
                          const char   *table_name,
                          const char   *type_name,
                          int           id,
                          const SqType *type);

// sq_storage_get_all_full() can run a bit faster if user specify 'table_name' and 'type' at the same time.
void *sq_storage_get_all_full(SqStorage    *storage,
                              const char   *table_name,
                              const char   *type_name,
                              const SqType *container,
                              const char   *sql_where_having,
                              const SqType *type);
/*
void *sq_storage_get(SqStorage    *storage,
                     const char   *table_name,
                     const char   *type_name,
                     int           id);
 */
#define sq_storage_get(storage, table_name, type_name, id)    \
		sq_storage_get_full(storage, table_name, type_name, id, NULL)
/*
void *sq_storage_get_all(SqStorage    *storage,
                         const char   *table_name,
                         const char   *type_name,
                         const SqType *container);
 */
#define sq_storage_get_all(storage, table_name, type_name, container)    \
		sq_storage_get_all_full(storage, table_name, type_name, container, NULL, NULL)
/*
 * This function will generate below SQL statement to get rows
 * SELECT * FROM table_name + 'sql_where_having'
void *sq_storage_get_by_sql(SqStorage    *storage,
                            const char   *table_name,
                            const char   *type_name,
                            const SqType *container,
                            const char   *sql_where_having);
 */
#define sq_storage_get_by_sql(storage, table_name, type_name, container, sql_where_having)    \
		sq_storage_get_all_full(storage, table_name, type_name, container, sql_where_having, NULL)

/* return id if no error
   return -1 if error occurred (or id is unknown while TRANSACTION)
 */
int   sq_storage_insert_full(SqStorage    *storage,
                             const char   *table_name,
                             const char   *type_name,
                             void         *instance,
                             const SqType *type);
/*
int   sq_storage_insert(SqStorage  *storage,
                        const char *table_name,
                        const char *type_name,
                        void       *instance);
 */
#define sq_storage_insert(storage, table_name, type_name, instance)    \
		sq_storage_insert_full(storage, table_name, type_name, instance, NULL)

void  sq_storage_update_full(SqStorage    *storage,
                             const char   *table_name,
                             const char   *type_name,
                             void         *instance,
                             const SqType *type);
/*
void  sq_storage_update(SqStorage  *storage,
                        const char *table_name,
                        const char *type_name,
                        void       *instance);
 */
#define sq_storage_update(storage, table_name, type_name, instance)    \
		sq_storage_update_full(storage, table_name, type_name, instance, NULL)

void  sq_storage_remove_full(SqStorage    *storage,
                             const char   *table_name,
                             const char   *type_name,
                             int           id,
                             const SqType *type);
/*
void  sq_storage_remove(SqStorage  *storage,
                        const char *table_name,
                        const char *type_name,
                        int         id);
 */
#define sq_storage_remove(storage, table_name, type_name, id)    \
		sq_storage_remove_full(storage, table_name, type_name, id, NULL)

void  sq_storage_remove_by_sql(SqStorage    *storage,
                               const char   *table_name,
                               const char   *sql_where_having);

// ------------------------------------

// find SqTable by SqTable.name
#define  sq_storage_find(storage, table_name)    sq_schema_find((storage)->schema, table_name)

// find SqTable by SqType.name
SqTable *sq_storage_find_by_type(SqStorage *storage, const char *type_name);

// ------------------------------------
// SqStorage-query.c

// sq_storage_type_from_query() is for internal use only.
// return: table's type in query. It must call sq_type_unref() to free.
SqType  *sq_storage_type_from_query(SqStorage *storage, SqQuery *query, int *n_tables_in_query);

// 'query' must has FROM table_name or JOIN table_name
// e.g. SELECT * FROM table1 JOIN table2 ON ... JOIN table3 ON ...
// void **element = row;
// element[0] = table1
// element[1] = table2
// element[2] = table3
// ...etc
void *sq_storage_query(SqStorage *storage, SqQuery *query, const SqType *container, const SqType *type);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq
{

/* StorageMethod : C++ struct is used by SqStorage and it's children. */

struct StorageMethod
{
	/*
	void  init(Sqdb *db);
	void  init(DbMethod *dbmethod);
	void  final(void);
	*/

	int   open(const char *database_name);
	int   close(void);

	int   migrate(SqSchema *schema);

	template <class StructType>
	StructType *get(int id);
	void       *get(const char *table_name, int id, const SqType *type = NULL);

	template <class Element, class StlContainer>
	StlContainer *getBySql(const char *sql_where_having);
	template <class StlContainer>
	StlContainer *getBySql(const char *sql_where_having);
	template <class StructType>
	void *getBySql(const SqType *container, const char *sql_where_having);
	void *getBySql(const char *table_name, const SqType *container, const char *sql_where_having, const SqType *type = NULL);

	template <class Element, class StlContainer>
	StlContainer *getAll();
	template <class StlContainer>
	StlContainer *getAll();
	template <class StructType>
	void *getAll(const SqType *container);
	void *getAll(const char *table_name, const SqType *container, const SqType *type = NULL);

	template <class StlContainer>
	StlContainer *query(SqQuery *query);
	void *query(SqQuery *query, const SqType *container = NULL, const SqType *type = NULL);

	template <class StructType>
	int   insert(StructType& instance);
	template <class StructType>
	int   insert(StructType *instance);
	template <class StructType>
	int   insert(void *instance);
	int   insert(const char *table_name, void *instance, const SqType *type = NULL);

	template <class StructType>
	void  update(StructType& instance);
	template <class StructType>
	void  update(StructType *instance);
	template <class StructType>
	void  update(void *instance);
	void  update(const char *table_name, void *instance, const SqType *type = NULL);

	template <class StructType>
	void  remove(int id);
	void  remove(const char *table_name, int id, const SqType *type = NULL);

	void  removeBySql(const char *table_name, const char *sql_where_having);

	int   begin();
	int   commit();
	int   rollback();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/* --- SqStorage --- */

#ifdef __cplusplus
struct SqStorage : Sq::StorageMethod
#else
struct SqStorage
#endif
{
	Sqdb      *db;          // database

	SqSchema  *schema;      // current schema

	// tables is an array that sorted by SqTable.type.name
	SqPtrArray tables;
	int        tables_version;

	// 1 thread use 1 Sqxc chain
	Sqxc      *xc_input;    // SqxcValue
	Sqxc      *xc_output;   // SqxcSql

	const SqType   *container_default;
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// C99 or C++ inline functions

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
int      sq_storage_begin(SqStorage *storage) {
	return SQ_STORAGE_BEGIN(storage);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
int      sq_storage_commit(SqStorage *storage) {
	return SQ_STORAGE_COMMIT(storage);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
int      sq_storage_rollback(SqStorage *storage) {
	return SQ_STORAGE_ROLLBACK(storage);
}

#else   // __STDC_VERSION__ || __cplusplus

// C functions
int      sq_storage_begin(SqStorage *storage);
int      sq_storage_commit(SqStorage *storage);
int      sq_storage_rollback(SqStorage *storage);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

inline int   StorageMethod::open(const char *database_name) {
	return sqdb_open(((SqStorage*)this)->db, database_name);
}
inline int   StorageMethod::close(void) {
	return sqdb_close(((SqStorage*)this)->db);
}

inline int   StorageMethod::migrate(SqSchema *schema) {
	return sqdb_migrate(((SqStorage*)this)->db, ((SqStorage*)this)->schema, schema);
}

template <class StructType>
inline StructType *StorageMethod::get(int id) {
	return (StructType*)sq_storage_get((SqStorage*)this, NULL, typeid(StructType).name(), id);
}
inline void       *StorageMethod::get(const char *table_name, int id, const SqType *type) {
	return (void*)sq_storage_get_full((SqStorage*)this, table_name, NULL, id, type);
}

template <class StlContainer>
inline StlContainer *StorageMethod::getBySql(const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<typename StlContainer::value_type>::type).name());
	if (table == NULL)
		return NULL;
	SqType  *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_by_sql((SqStorage*)this, table->name, NULL, containerType, sql_where_having);
	delete (Sq::TypeStl<StlContainer>*)containerType;
	return instance;
}
template <class ElementType, class StlContainer>
inline StlContainer *StorageMethod::getBySql(const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<ElementType>::type).name());
	if (table == NULL)
		return NULL;
	SqType  *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_by_sql((SqStorage*)this, table->name, NULL, containerType, sql_where_having);
	delete (Sq::TypeStl<StlContainer>*)containerType;
	return instance;
}

template <class StructType>
inline void *StorageMethod::getBySql(const SqType *container, const char *sql_where_having) {
	return (StructType*)sq_storage_get_all_full((SqStorage*)this, NULL, typeid(StructType).name(), container, sql_where_having, NULL);
}
inline void *StorageMethod::getBySql(const char *table_name, const SqType *container, const char *sql_where_having, const SqType *type) {
	return (void*)sq_storage_get_all_full((SqStorage*)this, table_name, NULL, container, sql_where_having, type);
}

template <class StlContainer>
inline StlContainer *StorageMethod::getAll() {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<typename StlContainer::value_type>::type).name());
	if (table == NULL)
		return NULL;
	SqType  *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, NULL, containerType);
	delete (Sq::TypeStl<StlContainer>*)containerType;
	return instance;
}
template <class ElementType, class StlContainer>
inline StlContainer *StorageMethod::getAll() {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(typename std::remove_pointer<ElementType>::type).name());
	if (table == NULL)
		return NULL;
	SqType  *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, NULL, containerType);
	delete (Sq::TypeStl<StlContainer>*)containerType;
	return instance;
}

template <class StructType>
inline void *StorageMethod::getAll(const SqType *container) {
	return (StructType*)sq_storage_get_all((SqStorage*)this, NULL, typeid(StructType).name(), container);
}
inline void *StorageMethod::getAll(const char *table_name, const SqType *container, const SqType *type) {
	return (void*)sq_storage_get_all_full((SqStorage*)this, table_name, NULL, container, NULL, type);
}

template <class StlContainer>
inline StlContainer *StorageMethod::query(SqQuery *query) {
	void    *instance = NULL;
	SqType  *type = sq_storage_type_from_query((SqStorage*)this, query, NULL);
	if (type) {
		SqType  *containerType = new Sq::TypeStl<StlContainer>(type);
		instance = sq_storage_query((SqStorage*)this, query, containerType, type);
		delete (Sq::TypeStl<StlContainer>*)containerType;
		sq_type_unref(type);
	}
	return (StlContainer*)instance;
}
inline void *StorageMethod::query(SqQuery *query, const SqType *container, const SqType *type) {
	return sq_storage_query((SqStorage*)this, query, container, type);
}

template <class StructType>
inline int   StorageMethod::insert(StructType& instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), &instance);
}
template <class StructType>
inline int   StorageMethod::insert(StructType *instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
template <class StructType>
inline int   StorageMethod::insert(void *instance) {
	return sq_storage_insert((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
inline int   StorageMethod::insert(const char *table_name, void *instance, const SqType *type) {
	return sq_storage_insert_full((SqStorage*)this, table_name, NULL, instance, type);
}

template <class StructType>
inline void  StorageMethod::update(StructType& instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), &instance);
}
template <class StructType>
inline void  StorageMethod::update(StructType *instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
template <class StructType>
inline void  StorageMethod::update(void *instance) {
	sq_storage_update((SqStorage*)this, NULL, typeid(StructType).name(), instance);
}
inline void  StorageMethod::update(const char *table_name, void *instance, const SqType *type) {
	sq_storage_update_full((SqStorage*)this, table_name, NULL, instance, type);
}

template <class StructType>
inline void StorageMethod::remove(int id) {
	sq_storage_remove((SqStorage*)this, NULL, typeid(StructType).name(), id);
}
inline void StorageMethod::remove(const char *table_name, int id, const SqType *type) {
	sq_storage_remove_full((SqStorage*)this, table_name, NULL, id, type);
}

inline void StorageMethod::removeBySql(const char *table_name, const char *sql_where_having) {
	sq_storage_remove_by_sql((SqStorage*)this, table_name, sql_where_having);
}

inline int  StorageMethod::begin() {
	return SQ_STORAGE_BEGIN((SqStorage*)this);
}
inline int  StorageMethod::commit() {
	return SQ_STORAGE_COMMIT((SqStorage*)this);
}
inline int  StorageMethod::rollback() {
	return SQ_STORAGE_ROLLBACK((SqStorage*)this);
}

// This is for directly use only. You can NOT derived it.
struct Storage : SqStorage
{
	Storage(Sqdb *db) {
		sq_storage_init((SqStorage*)this, db);
	}
	Storage(DbMethod *dbmethod) {
		sq_storage_init((SqStorage*)this, (Sqdb*)dbmethod);
	}

	~Storage() {
		sq_storage_final((SqStorage*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STORAGE
