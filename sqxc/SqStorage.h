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

#include <SqConfig.h>
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
                     int64_t       id);

// parameter 'sql_where_having' is SQL statement that exclude "SELECT * FROM table_name"
void *sq_storage_get_all(SqStorage    *storage,
                         const char   *table_name,
                         const SqType *table_type,
                         const SqType *container_type,
                         const char   *sql_where_having);

// return inserted row id if primary key has auto increment attribute.
int64_t sq_storage_insert(SqStorage    *storage,
                          const char   *table_name,
                          const SqType *table_type,
                          void         *instance);

// return number of rows changed.
int   sq_storage_update(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance);

// parameter 'sql_where_having' is SQL statement that exclude "UPDATE table_name SET column=value"
// pass column_name list after parameter 'sql_where_having' and the last argument must be NULL
// return number of rows changed.
int64_t sq_storage_update_all(SqStorage    *storage,
                              const char   *table_name,
                              const SqType *table_type,
                              void         *instance,
                              const char   *sql_where_having,
                              ...);

#ifdef SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
// parameter 'sql_where_having' is SQL statement that exclude "UPDATE table_name SET column=value"
// pass field_offset list after parameter 'sql_where_having' and the last argument must be -1
// return number of rows changed.
int64_t sq_storage_update_field(SqStorage    *storage,
                                const char   *table_name,
                                const SqType *table_type,
                                void         *instance,
                                const char   *sql_where_having,
                                ...);
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

void  sq_storage_remove(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        int64_t       id);

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

/* sq_storage_setup_query() setup 'query' and return SqType for calling sq_storage_query() with 'query'.

   return NULL          if table not found and 'type_joint' can NOT handle unknown table type.

   return 'type_joint'  if 'query' has joined multi-table. It will setup 'type_joint' and 'query'.

   return type of table if 'query' has only 1 table. It will setup 'type_joint' but keep 'query' no change.
                        In this case, user can call sq_storage_query() with returned type or 'type_joint'.
 */
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

/* sq_storage_query() execute 'query' and get result.

   If 'query' has only 1 table, it use type of table to create row data.
   If 'query' has joined multi-table, it use SqStorage.joint_default to create row data.

   SqTypeJoint is default type of SqStorage.joint_default, it can be replaced by user custom type.

   e.g. execute statement "SELECT * FROM table1 JOIN table2 ON ... JOIN table3 ON ..."
        if you use SqTypeJoint to create row data, the result looks like:
   void **element = row;
   table1 = element[0];
   table2 = element[1];
   table3 = element[2];
   ...etc
 */
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

	// get<StructType>(id)
	template <class StructType>
	StructType *get(int64_t id);
	// get() without template
	void       *get(const char *table_name, int64_t id);
	void       *get(const char *table_name, const SqType *table_type, int64_t id);

	// getAll<StructType, std::vector<StructType>>()
	template <class Element, class StlContainer>
	StlContainer *getAll(const char *sql_where_having = NULL);
	// getAll<std::vector<StructType>>()
	template <class StlContainer>
	StlContainer *getAll(const char *sql_where_having = NULL);
	// getAll<StructType>(NULL, NULL)
	template <class StructType>
	void *getAll(const SqType *container_type, const char *sql_where_having);
	// getAll without template
	void *getAll(const char *table_name, const char *sql_where_having = NULL);
	void *getAll(const char *table_name, const SqType *container_type, const char *sql_where_having = NULL);
	void *getAll(const char *table_name, const SqType *table_type, const SqType *container_type, const char *sql_where_having = NULL);

	Sq::Type *setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
	Sq::Type *setupQuery(Sq::QueryMethod *query, Sq::TypeJointMethod *jointType);

	// query(struct_reference)
	template <class StlContainer>
	StlContainer *query(Sq::QueryMethod &query);
	// query(struct_pointer)
	template <class StlContainer>
	StlContainer *query(Sq::QueryMethod *query);
	// query(struct_reference) + tableType
	template <class StlContainer>
	StlContainer *query(Sq::QueryMethod &query, const SqType *tableType);
	// query(struct_pointer)   + tableType
	template <class StlContainer>
	StlContainer *query(Sq::QueryMethod *query, const SqType *tableType);
	// query() without template
	void *query(Sq::QueryMethod &query, const SqType *containerType = NULL);
	void *query(Sq::QueryMethod *query, const SqType *containerType = NULL);
	void *query(Sq::QueryMethod &query, const SqType *tableType, const SqType *containerType);
	void *query(Sq::QueryMethod *query, const SqType *tableType, const SqType *containerType);

	// insert(struct_reference);
	template <class StructType>
	int64_t  insert(StructType &instance);
	// insert(struct_pointer);
	template <class StructType>
	int64_t  insert(StructType *instance);
	// insert<StructType>(struct_pointer);
	template <class StructType>
	int64_t  insert(void *instance);
	// insert() without template
	int64_t  insert(const char *table_name, void *instance);
	int64_t  insert(const char *table_name, const SqType *table_type, void *instance);

	// update(struct_reference)
	template <class StructType>
	int   update(StructType &instance);
	// update(struct_pointer)
	template <class StructType>
	int   update(StructType *instance);
	// update<StructType>(struct_pointer)
	template <class StructType>
	int   update(void *instance);
	// update() without template
	int   update(const char *table_name, void *instance);
	int   update(const char *table_name, const SqType *table_type, void *instance);

	// updateAll(struct_reference)
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType &instance, const char *sql_where_having = NULL, const Args... args);
	// updateAll(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType *instance, const char *sql_where_having = NULL, const Args... args);
	// updateAll<StructType>(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateAll(void *instance, const char *sql_where_having = NULL, const Args... args);
	// updateAll() without StructType
	template <typename... Args>
	int64_t  updateAll(const char *table_name, void *instance, const char *sql_where_having = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateAll(const char *table_name, const SqType *table_type, void *instance, const char *sql_where_having = NULL, const Args... args);

#ifdef SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
	// updateField(struct_reference)
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType &instance, const char *sql_where_having = NULL, const Args... args);
	// updateField(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType *instance, const char *sql_where_having = NULL, const Args... args);
	// updateField<StructType>(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateField(void *instance, const char *sql_where_having = NULL, const Args... args);
	// updateField() without StructType
	template <typename... Args>
	int64_t  updateField(const char *table_name, void *instance, const char *sql_where_having = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateField(const char *table_name, const SqType *table_type, void *instance, const char *sql_where_having = NULL, const Args... args);
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

	template <class StructType>
	void  remove(int64_t id);
	void  remove(const char *table_name, int64_t id);
	void  remove(const char *table_name, const SqType *table_type, int64_t id);

	template <class StructType>
	void  removeAll(const char *sql_where_having = NULL);
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

#define SQ_STORAGE_MEMBERS               \
	Sqdb      *db;                       \
	SqSchema  *schema;                   \
	SqPtrArray tables;                   \
	int        tables_version;           \
	Sqxc      *xc_input;                 \
	Sqxc      *xc_output;                \
	SqTypeJoint    *joint_default;       \
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

	SqTypeJoint    *joint_default;
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
inline StructType *StorageMethod::get(int64_t id) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return NULL;
	return (StructType*)sq_storage_get((SqStorage*)this, table->name, table->type, id);
}
inline void  *StorageMethod::get(const char *table_name, int64_t id) {
	return sq_storage_get((SqStorage*)this, table_name, NULL, id);
}
inline void  *StorageMethod::get(const char *table_name, const SqType *table_type, int64_t id) {
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
inline void *StorageMethod::getAll(const char *table_name, const char *sql_where_having) {
	return sq_storage_get_all((SqStorage*)this, table_name, NULL, NULL, sql_where_having);
}
inline void *StorageMethod::getAll(const char *table_name, const SqType *container_type, const char *sql_where_having) {
	return sq_storage_get_all((SqStorage*)this, table_name, NULL, container_type, sql_where_having);
}
inline void *StorageMethod::getAll(const char *table_name, const SqType *table_type, const SqType *container_type, const char *sql_where_having) {
	return sq_storage_get_all((SqStorage*)this, table_name, table_type, container_type, sql_where_having);
}

inline Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType) {
	return (Sq::Type*)sq_storage_setup_query((SqStorage*)this, (SqQuery*)&query, (SqTypeJoint*)jointType);
}
inline Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod *query, Sq::TypeJointMethod *jointType) {
	return (Sq::Type*)sq_storage_setup_query((SqStorage*)this, (SqQuery*) query, (SqTypeJoint*)jointType);
}

template <class StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod &query) {
	void    *instance  = NULL;
	SqType  *tableType = sq_storage_setup_query((SqStorage*)this, (SqQuery*)&query, ((SqStorage*)this)->joint_default);
	if (tableType) {
		Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
		instance = sq_storage_query((SqStorage*)this, (SqQuery*)&query, tableType, containerType);
		delete containerType;
	}
	return (StlContainer*)instance;
}
template <class StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod *query) {
	void    *instance  = NULL;
	SqType  *tableType = sq_storage_setup_query((SqStorage*)this, (SqQuery*)query, ((SqStorage*)this)->joint_default);
	if (tableType) {
		Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
		instance = sq_storage_query((SqStorage*)this, (SqQuery*)query, tableType, containerType);
		delete containerType;
	}
	return (StlContainer*)instance;
}
template <class StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod &query, const SqType *tableType) {
	void    *instance  = NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
	instance = sq_storage_query((SqStorage*)this, (SqQuery*)&query, tableType, containerType);
	delete containerType;
	return (StlContainer*)instance;
}
template <class StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod *query, const SqType *tableType) {
	void    *instance  = NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
	instance = sq_storage_query((SqStorage*)this, (SqQuery*)query, tableType, containerType);
	delete containerType;
	return (StlContainer*)instance;
}
inline void *StorageMethod::query(Sq::QueryMethod &query, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)&query, NULL, container_type);
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, NULL, container_type);
}
inline void *StorageMethod::query(Sq::QueryMethod &query, const SqType *table_type, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)&query, table_type, container_type);
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *table_type, const SqType *container_type) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, table_type, container_type);
}

template <class StructType>
inline int64_t  StorageMethod::insert(StructType &instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, &instance);
}
template <class StructType>
inline int64_t  StorageMethod::insert(StructType *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, instance);
}
template <class StructType>
inline int64_t  StorageMethod::insert(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, instance);
}
inline int64_t  StorageMethod::insert(const char *table_name, void *instance) {
	return sq_storage_insert((SqStorage*)this, table_name, NULL, instance);
}
inline int64_t  StorageMethod::insert(const char *table_name, const SqType *table_type, void *instance) {
	return sq_storage_insert((SqStorage*)this, table_name, table_type, instance);
}

template <class StructType>
inline int  StorageMethod::update(StructType &instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update((SqStorage*)this, table->name, table->type, &instance);
}
template <class StructType>
inline int  StorageMethod::update(StructType *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update((SqStorage*)this, table->name, table->type, instance);
}
template <class StructType>
inline int  StorageMethod::update(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update((SqStorage*)this, table->name, table->type, instance);
}
inline int  StorageMethod::update(const char *table_name, void *instance) {
	return sq_storage_update((SqStorage*)this, table_name, NULL, instance);
}
inline int  StorageMethod::update(const char *table_name, const SqType *table_type, void *instance) {
	return sq_storage_update((SqStorage*)this, table_name, table_type, instance);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType &instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_all((SqStorage*)this, table->name, table->type, &instance, sql_where_having, args..., NULL);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType *instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_all((SqStorage*)this, table->name, table->type, instance, sql_where_having, args..., NULL);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(void *instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_all((SqStorage*)this, table->name, table->type, instance, sql_where_having, args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *table_name, void *instance, const char *sql_where_having, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, table_name, NULL, instance, sql_where_having, args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *table_name, const SqType *table_type, void *instance, const char *sql_where_having, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, table_name, table_type, instance, sql_where_having, args..., NULL);
}

#ifdef SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType &instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_field((SqStorage*)this, table->name, table->type, &instance, sql_where_having, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType *instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_field((SqStorage*)this, table->name, table->type, instance, sql_where_having, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(void *instance, const char *sql_where_having, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_field((SqStorage*)this, table->name, table->type, instance, sql_where_having, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *table_name, void *instance, const char *sql_where_having, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, table_name, NULL, instance, sql_where_having, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *table_name, const SqType *table_type, void *instance, const char *sql_where_having, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, table_name, table_type, instance, sql_where_having, Sq::offsetOf(args)..., (size_t) -1);
}
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

template <class StructType>
inline void StorageMethod::remove(int64_t id) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_remove((SqStorage*)this, table->name, table->type, id);
}
inline void StorageMethod::remove(const char *table_name, int64_t id) {
	sq_storage_remove((SqStorage*)this, table_name, NULL, id);
}
inline void StorageMethod::remove(const char *table_name, const SqType *table_type, int64_t id) {
	sq_storage_remove((SqStorage*)this, table_name, table_type, id);
}

template <class StructType>
inline void StorageMethod::removeAll(const char *sql_where_having) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_remove_all((SqStorage*)this, table->name, sql_where_having);
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
