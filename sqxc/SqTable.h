/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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

#ifndef SQ_TABLE_H
#define SQ_TABLE_H

#include <SqType.h>
#include <SqEntry.h>
#include <SqColumn.h>
#include <SqRelation.h>
#include <SqTypeMapping.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqTable        SqTable;


// SQL common bit_field
#define SQB_TABLE_CHANGED                 SQB_CHANGED         // SQL: table has been changed.
// SQL common bit_field (for internal use only. use it when SQLite recreate)
#define SQB_TABLE_RENAMED                 SQB_RENAMED         // SQL: table has been renamed.

/* SqTable::bit_field for SQLite migration
   SQB_TABLE_XXX must large then SQB_RESERVE_END because it derived from SqEntry. (SqEntry.h)
   SQB_TABLE_XXX can overlap SQB_COLUMN_XXX (SqColumn.h)
 */

// current table has been created in database
#define SQB_TABLE_SQL_CREATED             (1 << 13)

// use these bit fields to decide whether or not to recreate table
// COL = column
#define SQB_TABLE_COL_ALTERED             (1 << 16)
#define SQB_TABLE_COL_RENAMED             (1 << 17)
#define SQB_TABLE_COL_DROPPED             (1 << 18)
#define SQB_TABLE_COL_ADDED               (1 << 19)
#define SQB_TABLE_COL_ADDED_UNIQUE        (1 << 20)    // UNIQUE or PRIMARY KEY
#define SQB_TABLE_COL_ADDED_CONSTRAINT    (1 << 21)
#define SQB_TABLE_COL_ADDED_EXPRESSION    (1 << 22)    // DEFAULT (expression)
#define SQB_TABLE_COL_ADDED_CURRENT_TIME  (1 << 23)    // DEFAULT CURRENT_TIME or CURRENT_DATE...etc

#define SQB_TABLE_COL_CHANGED             (SQB_TABLE_COL_ALTERED          | \
                                           SQB_TABLE_COL_RENAMED          | \
                                           SQB_TABLE_COL_DROPPED          | \
                                           SQB_TABLE_COL_ADDED            | \
                                           SQB_TABLE_COL_ADDED_UNIQUE     | \
                                           SQB_TABLE_COL_ADDED_CONSTRAINT | \
                                           SQB_TABLE_COL_ADDED_EXPRESSION | \
                                           SQB_TABLE_COL_ADDED_CURRENT_TIME)

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

// If 'table_type' == NULL, program will create dynamic SqType in it.
// If 'table_type' is dynamic SqType, it will be freed when program free table.
SqTable  *sq_table_new(const char *name, const SqType *table_type);
void      sq_table_free(SqTable *table);

void      sq_table_init(SqTable *table, const char *name, const SqType *table_type);
void      sq_table_final(SqTable *table);

void      sq_table_drop_column(SqTable *table, const char *column_name);
void      sq_table_rename_column(SqTable *table, const char *from, const char *to);

int       sq_table_get_columns(SqTable *table, SqPtrArray *ptr_array,
                               const SqType *type, unsigned int bit_field);

// user must specify one of 'table' or 'type_in_table'.
SqColumn *sq_table_get_primary(SqTable *table, const SqType *type_in_table);

// add SqColumn from array (NOT pointer array)
void      sq_table_add_column(SqTable *table, const SqColumn *column, int n_column);
// add SqColumn from pointer array
void      sq_table_add_column_ptrs(SqTable *table, const SqColumn **column_ptrs, int n_column_ptrs);

SqColumn *sq_table_add_bool(SqTable *table, const char *column_name,
                            size_t offset);
SqColumn *sq_table_add_int(SqTable *table, const char *column_name,
                           size_t offset);
SqColumn *sq_table_add_uint(SqTable *table, const char *column_name,
                            size_t offset);
SqColumn *sq_table_add_int64(SqTable *table, const char *column_name,
                             size_t offset);
SqColumn *sq_table_add_uint64(SqTable *table, const char *column_name,
                              size_t offset);
SqColumn *sq_table_add_double(SqTable *table, const char *column_name,
                              size_t offset, int precision, int scale);
SqColumn *sq_table_add_timestamp(SqTable *table, const char *column_name,
                                 size_t offset);
// if 'created_at_name' (or 'updated_at_name') is NULL, use default name "created_at" (or "updated_at").
void      sq_table_add_timestamps(SqTable *table,
                                  const char *created_at_name, size_t created_at_offset,
                                  const char *updated_at_name, size_t updated_at_offset);
SqColumn *sq_table_add_string(SqTable *table, const char *column_name,
                              size_t offset, int length);
SqColumn *sq_table_add_char(SqTable *table, const char *column_name,
                            size_t offset, int length);
SqColumn *sq_table_add_custom(SqTable *table, const char *column_name,
                              size_t offset, const SqType *sqtype,
                              int  length);
SqColumn *sq_table_add_mapping(SqTable *table, const char *column_name,
                              size_t offset, const SqType *sqtype,
                              int  sql_type);

// alias of sq_table_add_bool()
// SqColumn *sq_table_add_boolean(SqTable *table, const char *column_name, size_t offset);
#define sq_table_add_boolean    sq_table_add_bool

// alias of sq_table_add_int()
// SqColumn *sq_table_add_integer(SqTable *table, const char *column_name, size_t offset);
#define sq_table_add_integer    sq_table_add_int

// alias of sq_table_add_string()
// SqColumn *sq_table_add_str(SqTable *table, const char *column_name, size_t offset, int length);
#define sq_table_add_str        sq_table_add_string

/* sq_table_add_xxx_as() series */

// SqColumn *sq_table_add_bool_as(SqTable *table, Structure, Member)
#define sq_table_add_bool_as(table, Structure, Member)    \
		sq_table_add_bool(table, #Member, offsetof(Structure, Member))

// alias of sq_table_add_bool_as()
// SqColumn *sq_table_add_boolean_as(SqTable *table, Structure, Member)
#define sq_table_add_boolean_as    sq_table_add_bool_as

// SqColumn *sq_table_add_int_as(SqTable *table, Structure, Member)
#define sq_table_add_int_as(table, Structure, Member)    \
		sq_table_add_int(table, #Member, offsetof(Structure, Member))

// alias of sq_table_add_int_as()
// SqColumn *sq_table_add_integer_as(SqTable *table, Structure, Member)
#define sq_table_add_integer_as    sq_table_add_int_as

// SqColumn *sq_table_add_uint_as(SqTable *table, Structure, Member)
#define sq_table_add_uint_as(table, Structure, Member)    \
		sq_table_add_uint(table, #Member, offsetof(Structure, Member))

// SqColumn *sq_table_add_int64_as(SqTable *table, Structure, Member)
#define sq_table_add_int64_as(table, Structure, Member)    \
		sq_table_add_int64(table, #Member, offsetof(Structure, Member))

// SqColumn *sq_table_add_uint64_as(SqTable *table, Structure, Member)
#define sq_table_add_uint64_as(table, Structure, Member)    \
		sq_table_add_uint64(table, #Member, offsetof(Structure, Member))

// SqColumn *sq_table_add_double_as(SqTable *table, Structure, Member, int precision, int scale)
#define sq_table_add_double_as(table, Structure, Member, precision, scale)    \
		sq_table_add_double(table, #Member, offsetof(Structure, Member), precision, scale)

// SqColumn *sq_table_add_timestamp_as(SqTable *table, Structure, Member)
#define sq_table_add_timestamp_as(table, Structure, Member)    \
		sq_table_add_timestamp(table, #Member, offsetof(Structure, Member))

// SqColumn *sq_table_add_timestamps_as(SqTable *table, Structure, Member_created_at, Member_updated_at)
#define sq_table_add_timestamps_as(table, Structure, Member_created_at, Member_updated_at)   \
		sq_table_add_timestamps(table,                                                       \
		                        #Member_created_at, offsetof(Structure, Member_created_at),  \
		                        #Member_updated_at, offsetof(Structure, Member_updated_at))

// SqColumn *sq_table_add_timestamps_struct(SqTable *table, Structure)
#define sq_table_add_timestamps_struct(table, Structure)                        \
		sq_table_add_timestamps(table, NULL, offsetof(Structure, created_at),   \
		                               NULL, offsetof(Structure, updated_at))

// SqColumn *sq_table_add_string_as(SqTable *table, Structure, Member, int length)
#define sq_table_add_string_as(table, Structure, Member, length)    \
		sq_table_add_string(table, #Member, offsetof(Structure, Member), length)

// alias of sq_table_add_string_as()
// SqColumn *sq_table_add_str_as(SqTable *table, Structure, Member, int length)
#define sq_table_add_str_as    sq_table_add_string_as

// SqColumn *sq_table_add_char_as(SqTable *table, Structure, Member, int length)
#define sq_table_add_char_as(table, Structure, Member, length)    \
		sq_table_add_char(table, #Member, offsetof(Structure, Member), length)

// SqColumn *sq_table_add_custom_as(SqTable *table, Structure, Member, const SqType *type, int length)
#define sq_table_add_custom_as(table, Structure, Member, type, length)    \
		sq_table_add_custom(table, #Member, offsetof(Structure, Member), type, length)

// SqColumn *sq_table_add_mapping_as(SqTable *table, Structure, Member, const SqType *type, int sql_type)
#define sq_table_add_mapping_as(table, Structure, Member, type, sql_type)    \
		sq_table_add_mapping(table, #Member, offsetof(Structure, Member), type, sql_type)

/* --- SqTable C functions for CONSTRAINT --- */

SqColumn *sq_table_add_composite(SqTable *table,
                                 SqType  *column_type,
                                 unsigned int bit_field,
                                 const char  *name);

void      sq_table_drop_composite(SqTable *table,
                                  SqType  *column_type,
                                  unsigned int bit_field,
                                  const char  *name);

// sq_table_add_index(table, index_name, column_name..., NULL);
// CREATE INDEX "index_name" ON "table" ("column");
// CREATE INDEX "index_name" ON "table" ("column1", "column2");
// the last argument must be NULL
SqColumn *sq_table_add_index(SqTable *table,
                             const char *index_name,
                             ...);

// MySQL: ALTER TABLE table_name DROP INDEX index_name;
// DROP INDEX index_name
// DROP INDEX table_name.index_name;
void      sq_table_drop_index(SqTable *table, const char *index_name);

// sq_table_add_unique(table, unique_name, column_name..., NULL);
// CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// ADD CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// the last argument must be NULL
SqColumn *sq_table_add_unique(SqTable *table,
                              const char *unique_name,
                              ...);

// MySQL: ALTER TABLE "table" DROP INDEX "unique_name";
// other: ALTER TABLE "table" DROP CONSTRAINT "unique_name";
void      sq_table_drop_unique(SqTable *table, const char *unique_name);

// sq_table_add_primary(table, primary_name, column_name..., NULL);
// CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// ADD CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// the last argument must be NULL
SqColumn *sq_table_add_primary(SqTable *table,
                               const char *primary_name,
                               ...);

// MySQL: ALTER TABLE "customer" DROP PRIMARY KEY;
// other: ALTER TABLE "customer" DROP CONSTRAINT "name";
void      sq_table_drop_primary(SqTable *table, const char *name);

// CONSTRAINT "name" FOREIGN KEY (column_name) REFERENCES customers(id);
// ADD CONSTRAINT "name" FOREIGN KEY (column_name) REFERENCES customers(id);
SqColumn *sq_table_add_foreign(SqTable *table,
                               const char *foreign_name,
                               const char *column_name);
void      sq_table_drop_foreign(SqTable *table, const char *name);

// sort column by it's attribute
//	sq_ptr_array_sort(result, (SqCompareFunc)sq_column_cmp_attrib);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	TableMethod is used by SqTable and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqTable members.
 */
struct TableMethod
{
	void        setName(const char *tableName);
	bool        hasColumn(const char *column_name);
	Sq::Column *findColumn(const char *column_name);
	void        dropColumn(const char *column_name);
	void        renameColumn(const char *from, const char *to);
	int         getColumns(SqPtrArray *ptr_array, const SqType *type, unsigned int bit_field);
	Sq::Column *getPrimary();

	void        addColumn(const SqColumn *columns, int n_columns = 1);
	void        addColumn(const SqColumn **column_ptrs, int n_column_ptrs = 1);

	Sq::Column &boolean(const char *column_name, size_t offset);
	Sq::Column &bool_(const char *column_name, size_t offset);
	Sq::Column &integer(const char *column_name, size_t offset);
	Sq::Column &int_(const char *column_name, size_t offset);
	Sq::Column &uint(const char *column_name, size_t offset);
	Sq::Column &int64(const char *column_name, size_t offset);
	Sq::Column &uint64(const char *column_name, size_t offset);
	Sq::Column &timestamp(const char *column_name, size_t offset);
	void        timestamps(const char *created_at_name, size_t created_at_offset,
	                       const char *updated_at_name, size_t updated_at_offset);
	void        timestamps(size_t created_at_offset, size_t updated_at_offset);
	Sq::Column &double_(const char *column_name, size_t offset, int precision = 0, int scale = 0);
	Sq::Column &string(const char *column_name, size_t offset, int length = -1);
	Sq::Column &str(const char *column_name, size_t offset, int length = -1);
	Sq::Column &char_(const char *column_name, size_t offset, int length = -1);
	Sq::Column &custom(const char *column_name, size_t offset, const SqType *type, int length = -1);
	Sq::Column &mapping(const char *column_name, size_t offset, const SqType *type, int sql_type);

	Sq::Column &stdstring(const char *column_name, size_t offset, int length = -1);
	Sq::Column &stdstr(const char *column_name, size_t offset, int length = -1);

	template<class Store, class Type>
	Sq::Column &boolean(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &bool_(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &integer(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &int_(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &uint(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &int64(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &uint64(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column &timestamp(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	void        timestamps(const char *created_at_name, Type Store::*created_at_member,
	                       const char *updated_at_name, Type Store::*updated_at_member);
	template<class Store, class Type>
	void        timestamps(Type Store::*created_at_member, Type Store::*updated_at_member);
	template<class Store>
	void        timestamps();
	template<class Store, class Type>
	Sq::Column &double_(const char *column_name, Type Store::*member, int precision = 0, int scale = 0);
	template<class Store, class Type>
	Sq::Column &string(const char *column_name, Type Store::*member, int length = -1);
	template<class Store, class Type>
	Sq::Column &str(const char *column_name, Type Store::*member, int length = -1);
	template<class Store, class Type>
	Sq::Column &char_(const char *column_name, Type Store::*member, int length = -1);
	template<class Store, class Type>
	Sq::Column &custom(const char *column_name, Type Store::*member, const SqType *type, int length = -1);
	template<class Store, class Type>
	Sq::Column &mapping(const char *column_name, Type Store::*member, const SqType *type, int sql_type);

	template<class Store, class Type>
	Sq::Column &stdstring(const char *column_name, Type Store::*member, int length = -1);
	template<class Store, class Type>
	Sq::Column &stdstr(const char *column_name, Type Store::*member, int length = -1);

	// ----------------------------------------------------
	// composite (constraint)

	// index(index_name, column_name...)
	template <typename... Args>
	Sq::Column *index(const char *index_name, const Args... args);
	template <typename... Args>
	Sq::Column *addIndex(const char *index_name, const Args... args);
	void        dropIndex(const char *index_name);

	// unique(unique_name, column_name...)
	template <typename... Args>
	Sq::Column *unique(const char *unique_name, const Args... args);
	template <typename... Args>
	Sq::Column *addUnique(const char *unique_name, const Args... args);
	void        dropUnique(const char *unique_name);

	// primary(primary_name, column_name...)
	template <typename... Args>
	Sq::Column *primary(const char *primary_name, const Args... args);
	template <typename... Args>
	Sq::Column *addPrimary(const char *primary_name, const Args... args);
	void        dropPrimary(const char *primary_name);

	Sq::Column *foreign(const char *foreign_name, const char *column_name);
	Sq::Column *addForeign(const char *foreign_name, const char *column_name);
	void        dropForeign(const char *foreign_name);

/*
	template<int N>
	Sq::Column *index(const char *(&column_array)[N], const char *name = NULL)

	template<int N>
	Sq::Column *unique(const char *(&column_array)[N], const char *name = NULL);

	template<int N>
	Sq::Column *primary(const char *(&column_array)[N], const char *name = NULL);
 */
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqTable defines SQL Table

	SqEntry
	|
	`--- SqReentry
	     |
	     `--- SqTable

	Migration - Alter Table : table->bit_field & SQB_TABLE_CHANGED
	Migration - Drop   : table->name = NULL, table->old_name = column_name
	Migration - Rename : table->name = new_name, table->old_name = old_name

	Because 'const' is used to define string 'char*' and type 'SqType*',
	C++ user can initialize static structure easily.
 */

#define SQ_TABLE_MEMBERS       \
	SQ_REENTRY_MEMBERS;        \
	SqDestroyFunc  on_destory; \
	SqRelation    *relation

#ifdef __cplusplus
struct SqTable : Sq::TableMethod         // <-- 1. inherit C++ member function(method)
#else
struct SqTable
#endif
{
	SQ_TABLE_MEMBERS;                    // <-- 2. inherit member variable
/*	// ------ SqEntry members ------
	const SqType *type;        // type information for this entry
	const char   *name;
	size_t        offset;      // migration use this. Number of columns have existed in database
	unsigned int  bit_field;

	// ------ SqReentry members ------
	const char   *old_name;    // rename or drop

	// ------ SqTable members ------
	SqDestroyFunc on_destory;  // on destroy callback. It is used by derived Sqdb.

	// SqColumn's relation for (SQLite) migration.
	// sq_table_include() and sq_schema_include() store columns that having foreign reference.
	// sq_schema_trace_name() use these to trace renamed (or dropped) column that was referenced by others.
	// free it if you don't need to sync table changed to database.
	SqRelation   *relation;
 */
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_table_set_name(SqTable *table, const char *name) {
	SQ_ENTRY_SET_NAME(table, name);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
bool      sq_table_has_column(SqTable *table, const char *column_name)
{
	return (sq_type_find_entry(table->type, column_name, NULL) != NULL);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
SqColumn *sq_table_find_column(SqTable *table, const char *column_name)
{
	return (SqColumn*)sq_entry_find((SqEntry*)table, column_name, NULL);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

// C functions
void      sq_table_set_name(SqTable *table, const char *name);
bool      sq_table_has_column(SqTable *table, const char *column_name);
SqColumn *sq_table_find_column(SqTable *table, const char *column_name);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus
namespace Sq {

#if 0
template<class Store, class Type>
inline size_t offsetOf(Type Store::*member) {
	static Store obj;
	return size_t(&(obj.*member)) - size_t(&obj);
}
#else
// constexpr specifier (since C++11)
template<typename T, typename U> constexpr size_t offsetOf(U T::*member) {
	return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}
#endif

/*	TableMethod is used by SqTable and it's children.

	define TableMethod functions.
 */
inline void  TableMethod::setName(const char *tableName) {
	SQ_ENTRY_SET_NAME(this, tableName);
}
inline bool  TableMethod::hasColumn(const char *column_name) {
	return sq_table_has_column((SqTable*)this, column_name);
}
inline Sq::Column *TableMethod::findColumn(const char *column_name) {
	return (Sq::Column*)sq_table_find_column((SqTable*)this, column_name);
}
inline void  TableMethod::dropColumn(const char *column_name) {
	sq_table_drop_column((SqTable*)this, column_name);
}
inline void  TableMethod::renameColumn(const char *from, const char *to) {
	sq_table_rename_column((SqTable*)this, from, to);
}
inline int   TableMethod::getColumns(SqPtrArray *ptr_array, const SqType *type, unsigned int bit_field) {
	return sq_table_get_columns((SqTable*)this, ptr_array, type, bit_field);
}
inline Sq::Column *TableMethod::getPrimary() {
	return (Sq::Column*)sq_table_get_primary((SqTable*)this, NULL);
}

inline void  TableMethod::addColumn(const SqColumn *columns, int n_columns) {
	sq_table_add_column((SqTable*)this, columns, n_columns);
}
inline void  TableMethod::addColumn(const SqColumn **column_ptrs, int n_column_ptrs) {
	sq_table_add_column_ptrs((SqTable*)this, column_ptrs, n_column_ptrs);
}

inline Sq::Column &TableMethod::boolean(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::bool_(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::integer(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::int_(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::uint(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_uint((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::int64(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int64((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::uint64(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_uint64((SqTable*)this, column_name, offset);
}
inline Sq::Column &TableMethod::timestamp(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_timestamp((SqTable*)this, column_name, offset);
}
inline void  TableMethod::timestamps(const char *created_at_name, size_t created_at_offset,
                                     const char *updated_at_name, size_t updated_at_offset) {
	sq_table_add_timestamps((SqTable*)this, created_at_name, created_at_offset,
	                        updated_at_name, updated_at_offset);
}
inline void  TableMethod::timestamps(size_t created_at_offset, size_t updated_at_offset) {
	sq_table_add_timestamps((SqTable*)this, NULL, created_at_offset,
	                        NULL, updated_at_offset);
}
inline Sq::Column &TableMethod::double_(const char *column_name, size_t offset, int precision, int scale) {
	return *(Sq::Column*)sq_table_add_double((SqTable*)this, column_name, offset, precision, scale);
}
inline Sq::Column &TableMethod::string(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, offset, length);
}
inline Sq::Column &TableMethod::str(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, offset, length);
}
inline Sq::Column &TableMethod::char_(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_char((SqTable*)this, column_name, offset, length);
}
inline Sq::Column &TableMethod::custom(const char *column_name, size_t offset, const SqType *type, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, offset, type, length);
}
inline Sq::Column &TableMethod::mapping(const char *column_name, size_t offset, const SqType *type, int sql_type) {
	return *(Sq::Column*)sq_table_add_mapping((SqTable*)this, column_name, offset, type, sql_type);
}

inline Sq::Column &TableMethod::stdstring(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, offset, SQ_TYPE_STD_STRING, length);
}
inline Sq::Column &TableMethod::stdstr(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, offset, SQ_TYPE_STD_STR, length);
}

template<class Store, class Type>
inline Sq::Column &TableMethod::boolean(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::bool_(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::integer(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::int_(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::uint(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_uint((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::int64(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int64((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::uint64(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_uint64((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::timestamp(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_timestamp((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline void  TableMethod::timestamps(const char *created_at_name, Type Store::*created_at_member,
                                     const char *updated_at_name, Type Store::*updated_at_member) {
	sq_table_add_timestamps((SqTable*)this, created_at_name, Sq::offsetOf(created_at_member),
	                        updated_at_name, Sq::offsetOf(updated_at_member));
};
template<class Store, class Type>
inline void  TableMethod::timestamps(Type Store::*created_at_member, Type Store::*updated_at_member) {
	sq_table_add_timestamps((SqTable*)this, NULL, Sq::offsetOf(created_at_member),
	                        NULL, Sq::offsetOf(updated_at_member));
};
template<class Store>
inline void  TableMethod::timestamps() {
	sq_table_add_timestamps((SqTable*)this, NULL, Sq::offsetOf(&Store::created_at),
	                        NULL, Sq::offsetOf(&Store::updated_at));
};
template<class Store, class Type>
inline Sq::Column &TableMethod::double_(const char *column_name, Type Store::*member, int precision, int scale) {
	return *(Sq::Column*)sq_table_add_double((SqTable*)this, column_name, Sq::offsetOf(member), precision, scale);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::string(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, Sq::offsetOf(member), length);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::str(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, Sq::offsetOf(member), length);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::char_(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_char((SqTable*)this, column_name, Sq::offsetOf(member), length);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::custom(const char *column_name, Type Store::*member, const SqType *type, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, Sq::offsetOf(member), type, length);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::mapping(const char *column_name, Type Store::*member, const SqType *type, int sql_type) {
	return *(Sq::Column*)sq_table_add_mapping((SqTable*)this, column_name, Sq::offsetOf(member), type, sql_type);
};

template<class Store, class Type>
inline Sq::Column &TableMethod::stdstring(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, Sq::offsetOf(member), SQ_TYPE_STD_STRING, length);
};
template<class Store, class Type>
inline Sq::Column &TableMethod::stdstr(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, Sq::offsetOf(member), SQ_TYPE_STD_STR, length);
};

// define composite (constraint) methods of TableMethod

template <typename... Args>
inline Sq::Column *TableMethod::index(const char *index_name, const Args... args) {
	return (Sq::Column*)sq_table_add_index((SqTable*)this, index_name, args..., NULL);
}
template <typename... Args>
inline Sq::Column *TableMethod::addIndex(const char *index_name, const Args... args) {
	return (Sq::Column*)sq_table_add_index((SqTable*)this, index_name, args..., NULL);
}
inline void  TableMethod::dropIndex(const char *index_name) {
//	sq_table_drop_index((SqTable*)this, index_name);
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_INDEX, 0, index_name);
}

template <typename... Args>
inline Sq::Column *TableMethod::unique(const char *unique_name, const Args... args) {
	return (Sq::Column*)sq_table_add_unique((SqTable*)this, unique_name, args..., NULL);
}
template <typename... Args>
inline Sq::Column *TableMethod::addUnique(const char *unique_name, const Args... args) {
	return (Sq::Column*)sq_table_add_unique((SqTable*)this, unique_name, args..., NULL);
}
inline void  TableMethod::dropUnique(const char *unique_name) {
//	sq_table_drop_unique((SqTable*)this, unique_name);
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_COLUMN_UNIQUE, unique_name);
}

template <typename... Args>
inline Sq::Column *TableMethod::primary(const char *primary_name, const Args... args) {
	return (Sq::Column*)sq_table_add_primary((SqTable*)this, primary_name, args..., NULL);
}
template <typename... Args>
inline Sq::Column *TableMethod::addPrimary(const char *primary_name, const Args... args) {
	return (Sq::Column*)sq_table_add_primary((SqTable*)this, primary_name, args..., NULL);
}
inline void  TableMethod::dropPrimary(const char *primary_name) {
//	sq_table_drop_primary((SqTable*)this, primary_name);
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_COLUMN_PRIMARY, primary_name);
}

inline Sq::Column *TableMethod::foreign(const char *foreign_name, const char *column_name) {
	return (Sq::Column*)sq_table_add_foreign((SqTable*)this, foreign_name, column_name);
}
inline Sq::Column *TableMethod::addForeign(const char *foreign_name, const char *column_name) {
	return (Sq::Column*)sq_table_add_foreign((SqTable*)this, foreign_name, column_name);
}
inline void  TableMethod::dropForeign(const char *foreign_name) {
//	sq_table_drop_foreign((SqTable*)this, foreign_name);
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_COLUMN_FOREIGN, foreign_name);
}

/*
template<int N>
inline Sq::Column *TableMethod::index(const char *(&column_array)[N], const char *name)

template<int N>
inline Sq::Column *TableMethod::unique(const char *(&column_array)[N], const char *name);

template<int N>
inline Sq::Column *TableMethod::primary(const char *(&column_array)[N], const char *name);
 */


/* All derived struct/class must be C++11 standard-layout. */

struct Table : SqTable {
	Table(const char *name = NULL, const SqType *tableType = NULL) {
		sq_table_init(this, name, tableType);
	}
	~Table() {
		sq_table_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TABLE_H
