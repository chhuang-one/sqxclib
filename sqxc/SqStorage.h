/*
 *   Copyright (C) 2020-2026 by C.H. Huang
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

#include <sqxc/SqConfig.h>
#include <sqxc/Sqdb.h>
#include <sqxc/SqSchema.h>
#include <sqxc/SqJoint.h>
#include <sqxc/SqQuery.h>
#ifdef __cplusplus
#include <sqxc/SqType-stl-cxx.h>
#endif

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqStorage         SqStorage;

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

// open & close database
int   sq_storage_open(SqStorage *storage, const char *database_name);
int   sq_storage_close(SqStorage *storage);

// synchronize SqStorage::schema to database if 'schema' == NULL (Mainly used by SQLite)
int   sq_storage_migrate(SqStorage *storage, SqSchema *schema);

/* ------------------------------------
	CRUD functions:
	1. If 'table_type' is NULL, SqStorage will try to find 'table_type' in SqStorage::schema.
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

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
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

int   sq_storage_begin_trans(SqStorage *storage);
int   sq_storage_commit_trans(SqStorage *storage);
int   sq_storage_rollback_trans(SqStorage *storage);

// ------------------------------------
// find table by SqTable.name or SqType::name

// SqTable *sq_storage_find(SqStorage *storage, const char *table_name);
#define  sq_storage_find(storage, table_name)    sq_schema_find((storage)->schema, table_name)

SqTable *sq_storage_find_by_type(SqStorage *storage, const char *type_name);

// ------------------------------------
// SqStorage-query.c

/* sq_storage_setup_query()
   It setup 'query' and 'type_joint' then return SqType for calling sq_storage_query().
   It will add "SELECT table.column AS 'table.column'" in 'query' if 'query' has joined multi-table.

   return NULL          if table not found and 'type_joint' can NOT parse unknown table type.

   return 'type_joint'  if 'query' has joined multi-table. It will setup 'type_joint' and 'query'.

   return type of table if 'query' has only 1 table. It will setup 'type_joint' but keep 'query' no change.
                        In this case, user can call sq_storage_query() with returned type or 'type_joint'.
 */
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

/* sq_storage_query() execute 'query' and get result.

   If 'query' has only 1 table, it use type of table to create row data.
   If 'query' has joined multi-table, it use SqStorage::joint_default to create row data.

   SqTypeJoint is default type of SqStorage::joint_default, it can be replaced by user custom type.
   SqTypeRow is derived from SqTypeJoint, it can parse unknown result.

   e.g. execute statement "SELECT * FROM table1 JOIN table2 ON ... JOIN table3 ON ..."
        if you use SqTypeJoint to create row data, the result looks like:
   void **element = row;
   table1 = element[0];
   table2 = element[1];
   table3 = element[2];
 */
void *sq_storage_query(SqStorage    *storage,
                       SqQuery      *query,
                       const SqType *table_type,
                       const SqType *container_type);

void *sq_storage_query_raw(SqStorage    *storage,
                           const char   *query_str,
                           const SqType *table_type,
                           const SqType *container_type);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

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

	int   open(const char *databaseName);
	int   close(void);

	int   migrate(SqSchema *schema);

	Sq::Table  *find(const char *tableName);
	Sq::Table  *findByTypeName(const char *typeName);

	// get<StructType>(id)
	template <typename StructType>
	StructType *get(int64_t id);
	// get() without template
	void       *get(const char *tableName, int64_t id);
	void       *get(const char *tableName, const SqType *tableType, int64_t id);

	// getAll<StructType, std::vector<StructType>>()
	template <typename ElementType, typename StlContainer>
	StlContainer *getAll(const char *sqlWhereHaving = NULL);
	template <typename ElementType, typename StlContainer>
	StlContainer *getAll(QueryMethod &query);
	template <typename ElementType, typename StlContainer>
	StlContainer *getAll(QueryProxy &qproxy);
	// getAll<std::vector<StructType>>()
	template <typename StlContainer>
	StlContainer *getAll(const char *sqlWhereHaving = NULL);
	template <typename StlContainer>
	StlContainer *getAll(QueryMethod &query);
	template <typename StlContainer>
	StlContainer *getAll(QueryProxy &qproxy);
	template <typename StlContainer>
	StlContainer *getAll(const char *tableName, const SqType *tableType, const char *sqlWhereHaving);
	template <typename StlContainer>
	StlContainer *getAll(const char *tableName, const SqType *tableType, QueryMethod &query);
	template <typename StlContainer>
	StlContainer *getAll(const char *tableName, const SqType *tableType, QueryProxy &qproxy);
	// getAll<StructType>(NULL, NULL)
	template <typename StructType>
	void *getAll(const SqType *containerType, const char *sqlWhereHaving);
	template <typename StructType>
	void *getAll(const SqType *containerType, QueryMethod &query);
	template <typename StructType>
	void *getAll(const SqType *containerType, QueryProxy &qproxy);
	// getAll() with tableName
	void *getAll(const char *tableName, const char *sqlWhereHaving = NULL);
	void *getAll(const char *tableName, QueryMethod &query);
	void *getAll(const char *tableName, QueryProxy &qproxy);
	void *getAll(const char *tableName, const SqType *containerType, const char *sqlWhereHaving = NULL);
	void *getAll(const char *tableName, const SqType *containerType, QueryMethod &query);
	void *getAll(const char *tableName, const SqType *containerType, QueryProxy &qproxy);
	// getAll() with tableName + tableType
	void *getAll(const char *tableName, const SqType *tableType, const SqType *containerType, const char *sqlWhereHaving = NULL);
	void *getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryMethod &query);
	void *getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryProxy &qproxy);

//  --- getAll() for QueryProxy & QueryMethod pointer ---

	template <typename ElementType, typename StlContainer,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	StlContainer *getAll(QueryType *query);

	template <typename StlContainer,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	StlContainer *getAll(QueryType *query);

	template <typename StlContainer,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	StlContainer *getAll(const char *tableName, const SqType *tableType, QueryType *query);

	template <typename StructType,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void *getAll(const SqType *containerType, QueryType *query);

	template <typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void *getAll(const char *tableName, QueryType *query);

	template <typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void *getAll(const char *tableName, const SqType *containerType, QueryType *query);

	template <typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void *getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryType *query);

//  --- End of getAll() for QueryProxy & QueryMethod pointer ---

	Sq::Type *setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
	Sq::Type *setupQuery(Sq::QueryMethod *query, Sq::TypeJointMethod *jointType);

	// query(struct_reference)
	template <typename StlContainer>
	StlContainer *query(Sq::QueryMethod &query);
	template <typename StlContainer>
	StlContainer *query(Sq::QueryProxy &qproxy);
	// query(struct_pointer)
	template <typename StlContainer>
	StlContainer *query(Sq::QueryMethod *query);
	// query(struct_reference) + tableType
	template <typename StlContainer>
	StlContainer *query(Sq::QueryMethod &query, const SqType *tableType);
	template <typename StlContainer>
	StlContainer *query(Sq::QueryProxy &qproxy, const SqType *tableType);
	// query(struct_pointer) + tableType
	template <typename StlContainer>
	StlContainer *query(Sq::QueryMethod *query, const SqType *tableType);
	// query(C string) + tableType
	template <typename StlContainer>
	StlContainer *query(const char  *query_str, const SqType *tableType);
	// query() without template
	void *query(Sq::QueryMethod &query, const SqType *containerType = NULL);
	void *query(Sq::QueryProxy &qproxy, const SqType *containerType = NULL);
	void *query(Sq::QueryMethod *query, const SqType *containerType = NULL);
	void *query(Sq::QueryMethod &query, const SqType *tableType, const SqType *containerType);
	void *query(Sq::QueryProxy &qproxy, const SqType *tableType, const SqType *containerType);
	void *query(Sq::QueryMethod *query, const SqType *tableType, const SqType *containerType);
	void *query(const char  *query_str, const SqType *tableType, const SqType *containerType);

	// insert<StructType>(struct_pointer);
	template <typename StructType>
	int64_t  insert(void *instance);
	// insert(struct_pointer);
	template <typename StructType>
	int64_t  insert(StructType *instance);
	// insert(struct_reference);
	template <typename StructType>
	int64_t  insert(StructType &instance);
	// insert() without template
	int64_t  insert(const char *tableName, void *instance);
	int64_t  insert(const char *tableName, const SqType *tableType, void *instance);

	// update<StructType>(struct_pointer)
	template <typename StructType>
	int   update(void *instance);
	// update(struct_pointer)
	template <typename StructType>
	int   update(StructType *instance);
	// update(struct_reference)
	template <typename StructType>
	int   update(StructType &instance);
	// update() without template
	int   update(const char *tableName, void *instance);
	int   update(const char *tableName, const SqType *tableType, void *instance);

	// updateAll<StructType>(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateAll(void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(void *instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(void *instance, QueryProxy &qproxy, const Args... args);
	// updateAll(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType *instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType *instance, QueryProxy &qproxy, const Args... args);
	// updateAll(struct_reference)
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType &instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType &instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateAll(StructType &instance, QueryProxy &qproxy, const Args... args);
	// updateAll() with tableName
	template <typename... Args>
	int64_t  updateAll(const char *tableName, void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateAll(const char *tableName, void *instance, QueryMethod &query, const Args... args);
	template <typename... Args>
	int64_t  updateAll(const char *tableName, void *instance, QueryProxy &qproxy, const Args... args);
	// updateAll() with tableName + tableType
	template <typename... Args>
	int64_t  updateAll(const char *tableName, const SqType *tableType, void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateAll(const char *tableName, const SqType *tableType, void *instance, QueryMethod &query, const Args... args);
	template <typename... Args>
	int64_t  updateAll(const char *tableName, const SqType *tableType, void *instance, QueryProxy &qproxy, const Args... args);

//  --- updateAll() for QueryProxy & QueryMethod pointer ---

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateAll(void *instance, QueryType *query, const Args... args);

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateAll(StructType *instance, QueryType *query, const Args... args);

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateAll(StructType &instance, QueryType *query, const Args... args);

	template <typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateAll(const char *tableName, void *instance, QueryType *query, const Args... args);

	template <typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateAll(const char *tableName, const SqType *tableType, void *instance, QueryType *query, const Args... args);

//  --- End of updateAll() for QueryProxy & QueryMethod pointer ---

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
	// updateField<StructType>(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateField(void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(void *instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(void *instance, QueryProxy &qproxy, const Args... args);
	// updateField(struct_pointer)
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType *instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType *instance, QueryProxy &qproxy, const Args... args);
	// updateField(struct_reference)
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType &instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType &instance, QueryMethod &query, const Args... args);
	template <typename StructType, typename... Args>
	int64_t  updateField(StructType &instance, QueryProxy &qproxy, const Args... args);
	// updateField() with tableName
	template <typename... Args>
	int64_t  updateField(const char *tableName, void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateField(const char *tableName, void *instance, QueryMethod &query, const Args... args);
	template <typename... Args>
	int64_t  updateField(const char *tableName, void *instance, QueryProxy &qproxy, const Args... args);
	// updateField() with tableName + tableType
	template <typename... Args>
	int64_t  updateField(const char *tableName, const SqType *tableType, void *instance, const char *sqlWhereHaving = NULL, const Args... args);
	template <typename... Args>
	int64_t  updateField(const char *tableName, const SqType *tableType, void *instance, QueryMethod &query, const Args... args);
	template <typename... Args>
	int64_t  updateField(const char *tableName, const SqType *tableType, void *instance, QueryProxy &qproxy, const Args... args);

//  --- updateField() for QueryProxy & QueryMethod pointer ---

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateField(StructType &instance, QueryType *query, const Args... args);

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateField(StructType *instance, QueryType *query, const Args... args);

	template <typename StructType, typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateField(void *instance, QueryType *query, const Args... args);

	template <typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateField(const char *tableName, void *instance, QueryType *query, const Args... args);

	template <typename... Args,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	int64_t  updateField(const char *tableName, const SqType *tableType, void *instance, QueryType *query, const Args... args);

//  --- End of updateField() for QueryProxy & QueryMethod pointer ---

#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

	template <typename StructType>
	void  remove(int64_t id);
	void  remove(const char *tableName, int64_t id);
	void  remove(const char *tableName, const SqType *tableType, int64_t id);

	// removeAll<StructType>()
	template <typename StructType>
	void  removeAll(const char *sqlWhereHaving = NULL);
	template <typename StructType>
	void  removeAll(QueryMethod &query);
	template <typename StructType>
	void  removeAll(QueryProxy &qproxy);
	// removeAll() with tableName
	void  removeAll(const char *tableName, const char *sqlWhereHaving = NULL);
	void  removeAll(const char *tableName, QueryMethod &query);
	void  removeAll(const char *tableName, QueryProxy &qproxy);

//  --- removeAll() for QueryProxy & QueryMethod pointer ---

	template <typename StructType,
	          typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void  removeAll(QueryType *query);

	template <typename QueryType,
	          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
	                                   std::is_base_of<QueryMethod, QueryType>::value) &&
	                                  !std::is_same<QueryType, decltype(NULL)>::value >::type * = nullptr>
	void  removeAll(const char *tableName, QueryType *query);

//  --- End of removeAll() for QueryProxy & QueryMethod pointer ---

	int   beginTrans();
	int   commitTrans();
	int   rollbackTrans();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqStorage
	  SqStorage access database. It using Sqxc to convert data between C language and Sqdb instance.

	Notes about multithreading:
	1. 'schema', 'tables', 'tables_version' must be shared between threads.
	   use readers-writer lock to access these data member.
	2. 'xc_input' and 'xc_output' are NOT shared between threads.
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

	// 'tables' is an array that sorted by SqTable::type::name
	// 'tables_version' is used to compare with SqStorage::schema::version
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
// define inline functions here if compiler supports inline function.

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define StorageMethod functions. */

inline int   StorageMethod::open(const char *databaseName) {
	return sqdb_open(((SqStorage*)this)->db, databaseName);
}
inline int   StorageMethod::close(void) {
	return sqdb_close(((SqStorage*)this)->db);
}

inline int   StorageMethod::migrate(SqSchema *schema) {
	return sqdb_migrate(((SqStorage*)this)->db, ((SqStorage*)this)->schema, schema);
}

inline Sq::Table *StorageMethod::find(const char *tableName) {
	return (Sq::Table*)sq_storage_find((SqStorage*)this, tableName);
}
inline Sq::Table *StorageMethod::findByTypeName(const char *typeName) {
	return (Sq::Table*)sq_storage_find_by_type((SqStorage*)this, typeName);
}

template <typename StructType>
inline StructType *StorageMethod::get(int64_t id) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return NULL;
	return (StructType*)sq_storage_get((SqStorage*)this, table->name, table->type, id);
}
inline void  *StorageMethod::get(const char *tableName, int64_t id) {
	return sq_storage_get((SqStorage*)this, tableName, NULL, id);
}
inline void  *StorageMethod::get(const char *tableName, const SqType *tableType, int64_t id) {
	return sq_storage_get((SqStorage*)this, tableName, tableType, id);
}

template <typename ElementType, typename StlContainer>
inline StlContainer *StorageMethod::getAll(const char *sqlWhereHaving) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this,
			typeid(typename std::remove_reference< typename std::remove_pointer<ElementType>::type >::type).name());
	if (table == NULL)
		return NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, table->type, containerType, sqlWhereHaving);
	delete containerType;
	return instance;
}
template <typename ElementType, typename StlContainer>
inline StlContainer *StorageMethod::getAll(QueryMethod &query) {
	return getAll<ElementType, StlContainer>(query.c());
}
template <typename ElementType, typename StlContainer>
inline StlContainer *StorageMethod::getAll(QueryProxy &qproxy) {
	return getAll<ElementType, StlContainer>(qproxy.c());
}

template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(const char *sqlWhereHaving) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this,
			typeid(typename std::remove_reference< typename std::remove_pointer<typename StlContainer::value_type>::type >::type).name());
	if (table == NULL)
		return NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(table->type);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, table->name, table->type, containerType, sqlWhereHaving);
	delete containerType;
	return instance;
}
template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(QueryMethod &query) {
	return getAll<StlContainer>(query.c());
}
template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(QueryProxy &qproxy) {
	return getAll<StlContainer>(qproxy.c());
}
template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(const char *tableName, const SqType *tableType, const char *sqlWhereHaving) {
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
	StlContainer *instance = (StlContainer*) sq_storage_get_all((SqStorage*)this, tableName, tableType, containerType, sqlWhereHaving);
	delete containerType;
	return instance;
}
template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(const char *tableName, const SqType *tableType, QueryMethod &query) {
	return getAll<StlContainer>(tableName, tableType, query.c());
}
template <typename StlContainer>
inline StlContainer *StorageMethod::getAll(const char *tableName, const SqType *tableType, QueryProxy &qproxy) {
	return getAll<StlContainer>(tableName, tableType, qproxy.c());
}

template <typename StructType>
inline void *StorageMethod::getAll(const SqType *containerType, const char *sqlWhereHaving) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return NULL;
	return sq_storage_get_all((SqStorage*)this, table->name, table->type, containerType, sqlWhereHaving);
}
template <typename StructType>
inline void *StorageMethod::getAll(const SqType *containerType, QueryMethod &query) {
	return getAll<StructType>(containerType, query.c());
}
template <typename StructType>
inline void *StorageMethod::getAll(const SqType *containerType, QueryProxy &qproxy) {
	return getAll<StructType>(containerType, qproxy.c());
}

inline void *StorageMethod::getAll(const char *tableName, const char *sqlWhereHaving) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, NULL, sqlWhereHaving);
}
inline void *StorageMethod::getAll(const char *tableName, QueryMethod &query) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, NULL, query.c());
}
inline void *StorageMethod::getAll(const char *tableName, QueryProxy &qproxy) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, NULL, qproxy.c());
}

inline void *StorageMethod::getAll(const char *tableName, const SqType *containerType, const char *sqlWhereHaving) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, containerType, sqlWhereHaving);
}
inline void *StorageMethod::getAll(const char *tableName, const SqType *containerType, QueryMethod &query) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, containerType, query.c());
}
inline void *StorageMethod::getAll(const char *tableName, const SqType *containerType, QueryProxy &qproxy) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, containerType, qproxy.c());
}

inline void *StorageMethod::getAll(const char *tableName, const SqType *tableType, const SqType *containerType, const char *sqlWhereHaving) {
	return sq_storage_get_all((SqStorage*)this, tableName, tableType, containerType, sqlWhereHaving);
}
inline void *StorageMethod::getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryMethod &query) {
	return sq_storage_get_all((SqStorage*)this, tableName, tableType, containerType, query.c());
}
inline void *StorageMethod::getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryProxy &qproxy) {
	return sq_storage_get_all((SqStorage*)this, tableName, tableType, containerType, qproxy.c());
}

//  --- getAll() for QueryProxy & QueryMethod pointer ---

template <typename ElementType, typename StlContainer,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline StlContainer *StorageMethod::getAll(QueryType *query) {
	return getAll<ElementType, StlContainer>(query->c());
}

template <typename StlContainer,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline StlContainer *StorageMethod::getAll(QueryType *query) {
	return getAll<StlContainer>(query->c());
}

template <typename StlContainer,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline StlContainer *StorageMethod::getAll(const char *tableName, const SqType *tableType, QueryType *query) {
	return getAll<StlContainer>(tableName, tableType, query->c());
}

template <typename StructType,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void *StorageMethod::getAll(const SqType *containerType, QueryType *query) {
	return getAll<StructType>(containerType, query->c());
}

template <typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void *StorageMethod::getAll(const char *tableName, QueryType *query) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, NULL, query->c());
}

template <typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void *StorageMethod::getAll(const char *tableName, const SqType *containerType, QueryType *query) {
	return sq_storage_get_all((SqStorage*)this, tableName, NULL, containerType, query->c());
}

template <typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void *StorageMethod::getAll(const char *tableName, const SqType *tableType, const SqType *containerType, QueryType *query) {
	return sq_storage_get_all((SqStorage*)this, tableName, tableType, containerType, query->c());
}

//  --- End of getAll() for QueryProxy & QueryMethod pointer ---

inline Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType) {
	return (Sq::Type*)sq_storage_setup_query((SqStorage*)this, (SqQuery*)&query, (SqTypeJoint*)jointType);
}
inline Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod *query, Sq::TypeJointMethod *jointType) {
	return (Sq::Type*)sq_storage_setup_query((SqStorage*)this, (SqQuery*) query, (SqTypeJoint*)jointType);
}

template <typename StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod &query) {
	return this->query<StlContainer>(&query);
}
template <typename StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryProxy &qproxy) {
	return this->query<StlContainer>(qproxy.query());
}
template <typename StlContainer>
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
template <typename StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod &query, const SqType *tableType) {
	return this->query<StlContainer>(&query, tableType);
}
template <typename StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryProxy &qproxy, const SqType *tableType) {
	return this->query<StlContainer>(qproxy.query(), tableType);
}
template <typename StlContainer>
inline StlContainer *StorageMethod::query(Sq::QueryMethod *query, const SqType *tableType) {
	void    *instance  = NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
	instance = sq_storage_query((SqStorage*)this, (SqQuery*)query, tableType, containerType);
	delete containerType;
	return (StlContainer*)instance;
}
template <typename StlContainer>
inline StlContainer *StorageMethod::query(const char  *query_str, const SqType *tableType) {
	void    *instance  = NULL;
	Sq::TypeStl<StlContainer> *containerType = new Sq::TypeStl<StlContainer>(tableType);
	instance = sq_storage_query_raw((SqStorage*)this, query_str, tableType, containerType);
	delete containerType;
	return (StlContainer*)instance;
}
inline void *StorageMethod::query(Sq::QueryMethod &query, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)&query, NULL, containerType);
}
inline void *StorageMethod::query(Sq::QueryProxy &qproxy, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, qproxy.query(), NULL, containerType);
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, NULL, containerType);
}
inline void *StorageMethod::query(Sq::QueryMethod &query, const SqType *tableType, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)&query, tableType, containerType);
}
inline void *StorageMethod::query(Sq::QueryProxy &qproxy, const SqType *tableType, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, qproxy.query(), tableType, containerType);
}
inline void *StorageMethod::query(Sq::QueryMethod *query, const SqType *tableType, const SqType *containerType) {
	return sq_storage_query((SqStorage*)this, (SqQuery*)query, tableType, containerType);
}
inline void *StorageMethod::query(const char  *query_str, const SqType *tableType, const SqType *containerType) {
	return sq_storage_query_raw((SqStorage*)this, query_str, tableType, containerType);
}

template <typename StructType>
inline int64_t  StorageMethod::insert(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_insert((SqStorage*)this, table->name, table->type, instance);
}
template <typename StructType>
inline int64_t  StorageMethod::insert(StructType *instance) {
	return insert<StructType>((void*)instance);
}
template <typename StructType>
inline int64_t  StorageMethod::insert(StructType &instance) {
	return insert<StructType>((void*)&instance);
}
inline int64_t  StorageMethod::insert(const char *tableName, void *instance) {
	return sq_storage_insert((SqStorage*)this, tableName, NULL, instance);
}
inline int64_t  StorageMethod::insert(const char *tableName, const SqType *tableType, void *instance) {
	return sq_storage_insert((SqStorage*)this, tableName, tableType, instance);
}

template <typename StructType>
inline int  StorageMethod::update(void *instance) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update((SqStorage*)this, table->name, table->type, instance);
}
template <typename StructType>
inline int  StorageMethod::update(StructType *instance) {
	return update<StructType>((void*)instance);
}
template <typename StructType>
inline int  StorageMethod::update(StructType &instance) {
	return update<StructType>((void*)&instance);
}
inline int  StorageMethod::update(const char *tableName, void *instance) {
	return sq_storage_update((SqStorage*)this, tableName, NULL, instance);
}
inline int  StorageMethod::update(const char *tableName, const SqType *tableType, void *instance) {
	return sq_storage_update((SqStorage*)this, tableName, tableType, instance);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(void *instance, const char *sqlWhereHaving, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_all((SqStorage*)this, table->name, table->type, instance, sqlWhereHaving, args..., NULL);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(void *instance, QueryMethod &query, const Args... args) {
	return updateAll<StructType>(instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(void *instance, QueryProxy &qproxy, const Args... args) {
	return updateAll<StructType>(instance, qproxy.c(), args...);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType *instance, const char *sqlWhereHaving, const Args... args) {
	return updateAll<StructType>((void*)instance, sqlWhereHaving, args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType *instance, QueryMethod &query, const Args... args) {
	return updateAll<StructType>((void*)instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType *instance, QueryProxy &qproxy, const Args... args) {
	return updateAll<StructType>((void*)instance, qproxy.c(), args...);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType &instance, const char *sqlWhereHaving, const Args... args) {
	return updateAll<StructType>((void*)&instance, sqlWhereHaving, args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType &instance, QueryMethod &query, const Args... args) {
	return updateAll<StructType>((void*)&instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateAll(StructType &instance, QueryProxy &qproxy, const Args... args) {
	return updateAll<StructType>((void*)&instance, qproxy.c(), args...);
}

template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, void *instance, const char *sqlWhereHaving, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, NULL, instance, sqlWhereHaving, args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, void *instance, QueryMethod &query, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, NULL, instance, query.c(), args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, void *instance, QueryProxy &qproxy, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, NULL, instance, qproxy.c(), args..., NULL);
}

template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, const SqType *tableType, void *instance, const char *sqlWhereHaving, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, tableType, instance, sqlWhereHaving, args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, const SqType *tableType, void *instance, QueryMethod &query, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, tableType, instance, query.c(), args..., NULL);
}
template <typename... Args>
inline int64_t  StorageMethod::updateAll(const char *tableName, const SqType *tableType, void *instance, QueryProxy &qproxy, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, tableType, instance, qproxy.c(), args..., NULL);
}

//  --- updateAll() for QueryProxy & QueryMethod pointer ---

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateAll(void *instance, QueryType *query, const Args... args) {
	return updateAll<StructType>(instance, query->c(), args...);
}

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateAll(StructType *instance, QueryType *query, const Args... args) {
	return updateAll<StructType>((void*)instance, query->c(), args...);
}

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateAll(StructType &instance, QueryType *query, const Args... args) {
	return updateAll<StructType>((void*)&instance, query->c(), args...);
}

template <typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateAll(const char *tableName, void *instance, QueryType *query, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, NULL, instance, query->c(), args..., NULL);
}

template <typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateAll(const char *tableName, const SqType *tableType, void *instance, QueryType *query, const Args... args) {
	return sq_storage_update_all((SqStorage*)this, tableName, tableType, instance, query->c(), args..., NULL);
}

//  --- End of updateAll() for QueryProxy & QueryMethod pointer ---

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(void *instance, const char *sqlWhereHaving, const Args... args) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table == NULL)
		return 0;
	return sq_storage_update_field((SqStorage*)this, table->name, table->type, instance, sqlWhereHaving, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(void *instance, QueryMethod &query, const Args... args) {
	return updateField<StructType>(instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(void *instance, QueryProxy &qproxy, const Args... args) {
	return updateField<StructType>(instance, qproxy.c(), args...);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType *instance, const char *sqlWhereHaving, const Args... args) {
	return updateField<StructType>((void*)instance, sqlWhereHaving, args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType *instance, QueryMethod &query, const Args... args) {
	return updateField<StructType>((void*)instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType *instance, QueryProxy &qproxy, const Args... args) {
	return updateField<StructType>((void*)instance, qproxy.c(), args...);
}

template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType &instance, const char *sqlWhereHaving, const Args... args) {
	return updateField<StructType>((void*)&instance, sqlWhereHaving, args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType &instance, QueryMethod &query, const Args... args) {
	return updateField<StructType>((void*)&instance, query.c(), args...);
}
template <typename StructType, typename... Args>
inline int64_t  StorageMethod::updateField(StructType &instance, QueryProxy &qproxy, const Args... args) {
	return updateField<StructType>((void*)&instance, qproxy.c(), args...);
}

template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, void *instance, const char *sqlWhereHaving, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, NULL, instance, sqlWhereHaving, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, void *instance, QueryMethod &query, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, NULL, instance, query.c(), Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, void *instance, QueryProxy &qproxy, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, NULL, instance, qproxy.c(), Sq::offsetOf(args)..., (size_t) -1);
}

template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, const SqType *tableType, void *instance, const char *sqlWhereHaving, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, tableType, instance, sqlWhereHaving, Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, const SqType *tableType, void *instance, QueryMethod &query, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, tableType, instance, query.c(), Sq::offsetOf(args)..., (size_t) -1);
}
template <typename... Args>
inline int64_t  StorageMethod::updateField(const char *tableName, const SqType *tableType, void *instance, QueryProxy &qproxy, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, tableType, instance, qproxy.c(), Sq::offsetOf(args)..., (size_t) -1);
}

//  --- updateField() for QueryProxy & QueryMethod pointer ---

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateField(void *instance, QueryType *query, const Args... args) {
	return updateField<StructType>(instance, query->c(), args...);
}

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateField(StructType *instance, QueryType *query, const Args... args) {
	return updateField<StructType>((void*)instance, query->c(), args...);
}

template <typename StructType, typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateField(StructType &instance, QueryType *query, const Args... args) {
	return updateField<StructType>((void*)&instance, query->c(), args...);
}

template <typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateField(const char *tableName, void *instance, QueryType *query, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, NULL, instance, query->c(), Sq::offsetOf(args)..., (size_t) -1);
}

template <typename... Args,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline int64_t  StorageMethod::updateField(const char *tableName, const SqType *tableType, void *instance, QueryType *query, const Args... args) {
	return sq_storage_update_field((SqStorage*)this, tableName, tableType, instance, query->c(), Sq::offsetOf(args)..., (size_t) -1);
}

//  --- End of updateField() for QueryProxy & QueryMethod pointer ---

#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

template <typename StructType>
inline void StorageMethod::remove(int64_t id) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_remove((SqStorage*)this, table->name, table->type, id);
}
inline void StorageMethod::remove(const char *tableName, int64_t id) {
	sq_storage_remove((SqStorage*)this, tableName, NULL, id);
}
inline void StorageMethod::remove(const char *tableName, const SqType *tableType, int64_t id) {
	sq_storage_remove((SqStorage*)this, tableName, tableType, id);
}

template <typename StructType>
inline void StorageMethod::removeAll(const char *sqlWhereHaving) {
	SqTable *table = sq_storage_find_by_type((SqStorage*)this, typeid(StructType).name());
	if (table)
		sq_storage_remove_all((SqStorage*)this, table->name, sqlWhereHaving);
}
template <typename StructType>
inline void StorageMethod::removeAll(QueryMethod &query) {
	removeAll<StructType>(query.c());
}
template <typename StructType>
inline void StorageMethod::removeAll(QueryProxy &qproxy) {
	removeAll<StructType>(qproxy.c());
}

inline void StorageMethod::removeAll(const char *tableName, const char *sqlWhereHaving) {
	sq_storage_remove_all((SqStorage*)this, tableName, sqlWhereHaving);
}
inline void StorageMethod::removeAll(const char *tableName, QueryMethod &query) {
	sq_storage_remove_all((SqStorage*)this, tableName, query.c());
}
inline void StorageMethod::removeAll(const char *tableName, QueryProxy &qproxy) {
	sq_storage_remove_all((SqStorage*)this, tableName, qproxy.c());
}

//  --- removeAll() for QueryProxy & QueryMethod pointer ---

template <typename StructType,
          typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void  StorageMethod::removeAll(QueryType *query) {
	removeAll<StructType>(query->c());
}

template <typename QueryType,
          typename std::enable_if<(std::is_base_of<QueryProxy,  QueryType>::value ||
                                   std::is_base_of<QueryMethod, QueryType>::value) &&
                                  !std::is_same<QueryType, decltype(NULL)>::value >::type *>
inline void  StorageMethod::removeAll(const char *tableName, QueryType *query) {
	sq_storage_remove_all((SqStorage*)this, tableName, query->c());
}

//  --- End of removeAll() for QueryProxy & QueryMethod pointer ---

inline int  StorageMethod::beginTrans() {
	return sq_storage_begin_trans((SqStorage*)this);
}
inline int  StorageMethod::commitTrans() {
	return sq_storage_commit_trans((SqStorage*)this);
}
inline int  StorageMethod::rollbackTrans() {
	return sq_storage_rollback_trans((SqStorage*)this);
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
