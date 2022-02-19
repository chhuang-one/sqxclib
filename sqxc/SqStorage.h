/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

// int   sq_storage_begin_trans(SqStorage *storage);
#define  SQ_STORAGE_BEGIN_TRANS(storage)     (storage)->db->info->exec((storage)->db, "BEGIN", NULL, NULL);

// int   sq_storage_commit_trans(SqStorage *storage);
#define  SQ_STORAGE_COMMIT_TRANS(storage)    (storage)->db->info->exec((storage)->db, "COMMIT", NULL, NULL);

// int   sq_storage_rollback_trans(SqStorage *storage);
#define  SQ_STORAGE_ROLLBACK_TRANS(storage)  (storage)->db->info->exec((storage)->db, "ROLLBACK", NULL, NULL);

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* SqStorage C Functions */

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
	1. Parameter 'table_type' can be NULL.
	2. These can run a bit faster if user specify 'table_name' and 'table_type' at the same time.
 */
void *sq_storage_get(SqStorage    *storage,
                     const char   *table_name,
                     const SqType *table_type,
                     int           id);

// parameter 'sql_where_having' is SQL statement that exclude "SELECT * FROM table_name"
void *sq_storage_get_all(SqStorage    *storage,
                         const char   *table_name,
                         const SqType *table_type,
                         const SqType *container_type,
                         const char   *sql_where_having);

/* return id if no error
   return -1 if error occurred (or id is unknown while TRANSACTION)
 */
int   sq_storage_insert(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance);

void  sq_storage_update(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance);

void  sq_storage_remove(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        int           id);

// parameter 'sql_where_having' is SQL statement that exclude "DELETE FROM table_name"
void  sq_storage_remove_all(SqStorage    *storage,
                            const char   *table_name,
                            const char   *sql_where_having);

// ------------------------------------
// find table by SqTable.name or SqType.name

#define  sq_storage_find(storage, table_name)    sq_schema_find((storage)->schema, table_name)

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
void *sq_storage_query(SqStorage    *storage,
                       SqQuery      *query,
                       const SqType *table_type,
                       const SqType *container_type);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

struct QueryMethod;    // define in SqQuery.h

/*	StorageMethod is used by SqStorage and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqStorage members.
 */
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
	void       *get(const char *table_name, int id);
	void       *get(const char *table_name, const SqType *table_type, int id);

	template <class Element, class StlContainer>
	StlContainer *getAll(const char *sql_where_having = NULL);
	template <class StlContainer>
	StlContainer *getAll(const char *sql_where_having = NULL);
	template <class StructType>
	void *getAll(const SqType *container_type, const char *sql_where_having);
	void *getAll(const char *table_name, const SqType *container_type = NULL, const char *sql_where_having = NULL);
	void *getAll(const char *table_name, const SqType *table_type, const SqType *container_type, const char *sql_where_having = NULL);

	template <class StlContainer>
	StlContainer *query(Sq::QueryMethod *query);
	void *query(Sq::QueryMethod *query, const SqType *container_type = NULL);
	void *query(Sq::QueryMethod *query, const SqType *table_type, const SqType *container_type);

	template <class StructType>
	int   insert(StructType& instance);
	template <class StructType>
	int   insert(StructType *instance);
	template <class StructType>
	int   insert(void *instance);
	int   insert(const char *table_name, void *instance);
	int   insert(const char *table_name, const SqType *table_type, void *instance);

	template <class StructType>
	void  update(StructType& instance);
	template <class StructType>
	void  update(StructType *instance);
	template <class StructType>
	void  update(void *instance);
	void  update(const char *table_name, void *instance);
	void  update(const char *table_name, const SqType *table_type, void *instance);

	template <class StructType>
	void  remove(int id);
	void  remove(const char *table_name, int id);
	void  remove(const char *table_name, const SqType *table_type, int id);

	void  removeAll(const char *table_name, const char *sql_where_having = NULL);

	int   beginTrans();
	int   commitTrans();
	int   rollbackTrans();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqStorage
	  It access database by using Sqxc to convert data to/from Sqdb interface.

	Notes about multithreading:
	1. 'schema', 'tables', 'tables_version' must be shared between threads.
	   use readers-writer lock to access these data member.
	2. 'xc_input', 'xc_output' is NOT shared between threads.
	   each thread has its Sqxc chain ('xc_input' and 'xc_output').
 */

#define SQ_STORAGE_MEMBERS     \
	Sqdb      *db;             \
	SqSchema  *schema;         \
	SqPtrArray tables;         \
	int        tables_version; \
	Sqxc      *xc_input;       \
	Sqxc      *xc_output;      \
	const SqType   *container_default

#ifdef __cplusplus
struct SqStorage : Sq::StorageMethod         // <-- 1. inherit C++ member function(method)
#else
struct SqStorage
#endif
{
	SQ_STORAGE_MEMBERS;                      // <-- 2. inherit member variable
/*	// ------ SqStorage members ------
	Sqdb      *db;          // database
	SqSchema  *schema;      // current schema

	// tables is an array that sorted by SqTable.type.name
	SqPtrArray tables;
	int        tables_version;

	// each thread has its Sqxc chain
	Sqxc      *xc_input;    // SqxcValue
	Sqxc      *xc_output;   // SqxcSql

	const SqType   *container_default;
 */
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
int      sq_storage_begin_trans(SqStorage *storage) {
	return SQ_STORAGE_BEGIN_TRANS(storage);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
int      sq_storage_commit_trans(SqStorage *storage) {
	return SQ_STORAGE_COMMIT_TRANS(storage);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
int      sq_storage_rollback_trans(SqStorage *storage) {
	return SQ_STORAGE_ROLLBACK_TRANS(storage);
}

#else   // __STDC_VERSION__ || __cplusplus

// C functions
int      sq_storage_begin_trans(SqStorage *storage);
int      sq_storage_commit_trans(SqStorage *storage);
int      sq_storage_rollback_trans(SqStorage *storage);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define StorageMethod functions. */

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
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return NULL;
	return (StructType*)sq_storage_get((SqStorage*)this, table->name, table->type, id);
}
inline void  *StorageMethod::get(const char *table_name, int id) {
	return sq_storage_get((SqStorage*)this, table_name, NULL, id);
}
inline void  *StorageMethod::get(const char *table_name, const SqType *table_type, int id) {
	return sq_storage_get((SqStorage*)this, table_name, table_type, id);
}

template <class StlContainer>
inline StlContainer *StorageMethod::getAll(const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this,
			typeid(typename std::remove_reference< typename std::remove_pointer<typename StlContainer::value_type>::type >::type).name());
	if (table == NULL)
		return NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, table->type, containerType, sql_where_having);
	delete containerType;
	return instance;
}
template <class ElementType, class StlContainer>
inline StlContainer *StorageMethod::getAll(const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this,
			typeid(typename std::remove_reference< typename std::remove_pointer<ElementType>::type >::type).name());
	if (table == NULL)
		return NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, table->type, containerType, sql_where_having);
	delete containerType;
	return instance;
}
template <class StructType>
inline void *StorageMethod::getAll(const SqType *container_type, const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return NULL;
	return sq_storage_get_all((SqStorage*)this, table->name, table->type, container_type, sql_where_having);
}
inline void *StorageMethod::getAll(const char *table_name, const SqType *container_type, const char *sql_where_having) {
	return sq_storage_get_all((SqStorage*)this, table_name, NULL, container_type, sql_where_having);
}
inline void *StorageMethod::getAll(const char *table_name, const SqType *table_type, const SqType *container_type, const char *sql_where_having) {
	return sq_storage_get_all((SqStorage*)this, table_name, table_type, container_type, sql_where_having);
}

template <class StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod *query) {
	void    *instance  = NULL;
	SqType  *tableType = sq_storage_type_from_query((SqStorage*)this, (SqQuery*)query, NULL);
	if (tableType) {
		Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
		instance = sq_storage_query((SqStorage*)this, (SqQuery*)query, tableType, containerType);
		delete containerType;
		sq_type_unref(tableType);
	}
	return (StlContainer*)instance;
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, NULL, container_type);
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *table_type, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, table_type, container_type);
}

template <class StructType>
inline int   StorageMethod::insert(StructType& instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		return sq_storage_insert((SqStorage*)this, table->name, table->type, &instance);
}
template <class StructType>
inline int   StorageMethod::insert(StructType *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return -1;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, instance);
}
template <class StructType>
inline int   StorageMethod::insert(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return -1;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, instance);
}
inline int   StorageMethod::insert(const char *table_name, void *instance) {
	return sq_storage_insert((SqStorage*)this, table_name, NULL, instance);
}
inline int   StorageMethod::insert(const char *table_name, const SqType *table_type, void *instance) {
	return sq_storage_insert((SqStorage*)this, table_name, table_type, instance);
}

template <class StructType>
inline void  StorageMethod::update(StructType& instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_update((SqStorage*)this, table->name, table->type, &instance);
}
template <class StructType>
inline void  StorageMethod::update(StructType *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_update((SqStorage*)this, table->name, table->type, instance);
}
template <class StructType>
inline void  StorageMethod::update(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_update((SqStorage*)this, table->name, table->type, instance);
}
inline void  StorageMethod::update(const char *table_name, void *instance) {
	sq_storage_update((SqStorage*)this, table_name, NULL, instance);
}
inline void  StorageMethod::update(const char *table_name, const SqType *table_type, void *instance) {
	sq_storage_update((SqStorage*)this, table_name, table_type, instance);
}

template <class StructType>
inline void StorageMethod::remove(int id) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_remove((SqStorage*)this, table->name, table->type, id);
}
inline void StorageMethod::remove(const char *table_name, int id) {
	sq_storage_remove((SqStorage*)this, table_name, NULL, id);
}
inline void StorageMethod::remove(const char *table_name, const SqType *table_type, int id) {
	sq_storage_remove((SqStorage*)this, table_name, table_type, id);
}

inline void StorageMethod::removeAll(const char *table_name, const char *sql_where_having) {
	sq_storage_remove_all((SqStorage*)this, table_name, sql_where_having);
}

inline int  StorageMethod::beginTrans() {
	return SQ_STORAGE_BEGIN_TRANS((SqStorage*)this);
}
inline int  StorageMethod::commitTrans() {
	return SQ_STORAGE_COMMIT_TRANS((SqStorage*)this);
}
inline int  StorageMethod::rollbackTrans() {
	return SQ_STORAGE_ROLLBACK_TRANS((SqStorage*)this);
}

/* All derived struct/class must be C++11 standard-layout. */

struct Storage : SqStorage
{
	// constructor
	Storage(Sqdb *db) {
		sq_storage_init((SqStorage*)this, db);
	}
	Storage(DbMethod *dbmethod) {
		sq_storage_init((SqStorage*)this, (Sqdb*)dbmethod);
	}
	// destructor
	~Storage() {
		sq_storage_final((SqStorage*)this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_STORAGE
