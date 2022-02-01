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

#ifndef SQ_TABLE_H
#define SQ_TABLE_H

#include <SqType.h>
#include <SqEntry.h>
#include <SqColumn.h>
#include <SqRelation.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqTable        SqTable;

// SqTable::bit_field
#define SQB_TABLE_SQL_CREATED             (1 << 13)

// REO = reference each other
// SqTable::bit_field for checking foreign reference each other (avoid infinite recursive)
#define SQB_TABLE_REO_CHECKING            (1 << 14)
// SqTable::bit_field for SQLite (constraint reference each other)
#define SQB_TABLE_REO_CONSTRAINT          (1 << 15)

// COL = column
// SqTable::bit_field for SQLite (decide to recreate)
#define SQB_TABLE_COL_ALTERED             (1 << 16)
#define SQB_TABLE_COL_RENAMED             (1 << 17)
#define SQB_TABLE_COL_DROPPED             (1 << 18)
#define SQB_TABLE_COL_ADDED               (1 << 19)
#define SQB_TABLE_COL_ADDED_UNIQUE        (1 << 20)    // UNIQUE or PRIMARY KEY
#define SQB_TABLE_COL_ADDED_CONSTRAINT    (1 << 21)
#define SQB_TABLE_COL_ADDED_EXPRESSION    (1 << 22)    // DEFAULT (expression)
#define SQB_TABLE_COL_ADDED_CURRENT_TIME  (1 << 23)    // DEFAULT CURRENT_TIME or CURRENT_DATE...etc

#define SQB_TABLE_COL_CHANGED             (SQB_TABLE_COL_ALTERED | \
                                           SQB_TABLE_COL_RENAMED | \
                                           SQB_TABLE_COL_DROPPED | \
                                           SQB_TABLE_COL_ADDED   | \
                                           SQB_TABLE_COL_ADDED_UNIQUE     | \
                                           SQB_TABLE_COL_ADDED_CONSTRAINT | \
                                           SQB_TABLE_COL_ADDED_EXPRESSION | \
                                           SQB_TABLE_COL_ADDED_CURRENT_TIME)

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

SqTable  *sq_table_new(const char *name, const SqType *type_info);
void      sq_table_free(SqTable *table_pub);

bool      sq_table_has_column(SqTable *table, const char *column_name);
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

#define sq_table_add_integer    sq_table_add_int
#define sq_table_add_boolean    sq_table_add_bool

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

#define sq_table_add_integer_as(table, Structure, Member)    \
		sq_table_add_int(table, #Member, offsetof(Structure, Member))

#define sq_table_add_int_as(table, Structure, Member)    \
		sq_table_add_int(table, #Member, offsetof(Structure, Member))

#define sq_table_add_uint_as(table, Structure, Member)    \
		sq_table_add_uint(table, #Member, offsetof(Structure, Member))

#define sq_table_add_int64_as(table, Structure, Member)    \
		sq_table_add_int64(table, #Member, offsetof(Structure, Member))

#define sq_table_add_uint64_as(table, Structure, Member)    \
		sq_table_add_uint64(table, #Member, offsetof(Structure, Member))

#define sq_table_add_double_as(table, Structure, Member, precision, scale)    \
		sq_table_add_double(table, #Member, offsetof(Structure, Member), precision, scale)

#define sq_table_add_timestamp_as(table, Structure, Member)    \
		sq_table_add_timestamp(table, #Member, offsetof(Structure, Member))

#define sq_table_add_timestamps_as(table, Structure, Member_created_at, Member_updated_at)   \
		sq_table_add_timestamps(table,                                                       \
		                        #Member_created_at, offsetof(Structure, Member_created_at),  \
		                        #Member_updated_at, offsetof(Structure, Member_updated_at))

#define sq_table_add_timestamps_struct(table, Structure)                        \
		sq_table_add_timestamps(table, NULL, offsetof(Structure, created_at),   \
		                               NULL, offsetof(Structure, updated_at))

#define sq_table_add_string_as(table, Structure, Member, length)    \
		sq_table_add_string(table, #Member, offsetof(Structure, Member), length)

#define sq_table_add_custom_as(table, Structure, Member, type, length)    \
		sq_table_add_custom(table, #Member, offsetof(Structure, Member), type, length)

/* --- SqTable C functions for CONSTRAINT --- */

SqColumn *sq_table_add_composite(SqTable *table,
                                 SqType  *column_type,
                                 unsigned int bit_field,
                                 const char  *name);

void      sq_table_drop_composite(SqTable *table,
                                  SqType  *column_type,
                                  unsigned int bit_field,
                                  const char  *name);

// CREATE INDEX "index_name" ON "table" ("column");
// CREATE INDEX "index_name" ON "table" ("column1", "column2");
// the last argument must be NULL
SqColumn *sq_table_add_index(SqTable *table,
                             const char *index_name,
                             const char *column1_name, ...);

// MySQL: ALTER TABLE table_name DROP INDEX index_name;
// DROP INDEX index_name
// DROP INDEX table_name.index_name;
void      sq_table_drop_index(SqTable *table, const char *index_name);

// CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// ADD CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// the last argument must be NULL
SqColumn *sq_table_add_unique(SqTable *table,
                              const char *unique_name,
                              const char *column1_name, ...);

// MySQL: ALTER TABLE "table" DROP INDEX "unique_name";
// other: ALTER TABLE "table" DROP CONSTRAINT "unique_name";
void      sq_table_drop_unique(SqTable *table, const char *unique_name);

// CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// ADD CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// the last argument must be NULL
SqColumn *sq_table_add_primary(SqTable *table,
                               const char *primary_name,
                               const char *column1_name, ...);

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
	bool        hasColumn(const char *column_name);
	void        dropColumn(const char *column_name);
	void        renameColumn(const char *from, const char *to);
	int         getColumns(SqPtrArray *ptr_array, const SqType *type, unsigned int bit_field);
	Sq::Column *getPrimary();

	void        addColumn(const SqColumn *columns, int n_columns = 1);
	void        addColumn(const SqColumn **column_ptrs, int n_column_ptrs = 1);

	Sq::Column& boolean(const char *column_name, size_t offset);
	Sq::Column& bool_(const char *column_name, size_t offset);
	Sq::Column& integer(const char *column_name, size_t offset);
	Sq::Column& int_(const char *column_name, size_t offset);
	Sq::Column& uint(const char *column_name, size_t offset);
	Sq::Column& int64(const char *column_name, size_t offset);
	Sq::Column& uint64(const char *column_name, size_t offset);
	Sq::Column& timestamp(const char *column_name, size_t offset);
	void        timestamps(const char *created_at_name, size_t created_at_offset,
	                       const char *updated_at_name, size_t updated_at_offset);
	void        timestamps(size_t created_at_offset, size_t updated_at_offset);
	Sq::Column& double_(const char *column_name, size_t offset, int precision = 0, int scale = 0);
	Sq::Column& string(const char *column_name, size_t offset, int length = -1);
	Sq::Column& char_(const char *column_name, size_t offset, int length = -1);
	Sq::Column& custom(const char *column_name, size_t offset, const SqType *type, int length = -1);

	Sq::Column& stdstring(const char *column_name, size_t offset, int length = -1);

	template<class Store, class Type>
	Sq::Column& boolean(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& bool_(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& integer(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& int_(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& uint(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& int64(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& uint64(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	Sq::Column& timestamp(const char *column_name, Type Store::*member);
	template<class Store, class Type>
	void        timestamps(const char *created_at_name, Type Store::*created_at_member,
	                       const char *updated_at_name, Type Store::*updated_at_member);
	template<class Store, class Type>
	void        timestamps(Type Store::*created_at_member, Type Store::*updated_at_member);
	template<class Store>
	void        timestamps();
	template<class Store, class Type>
	Sq::Column& double_(const char *column_name, Type Store::*member, int precision = 0, int scale = 0);
	template<class Store, class Type>
	Sq::Column& string(const char *column_name, Type Store::*member, int length = -1);
	template<class Store, class Type>
	Sq::Column& custom(const char *column_name, Type Store::*member, const SqType *type, int length = -1);

	template<class Store, class Type>
	Sq::Column& stdstring(const char *column_name, Type Store::*member, int length = -1);

	// ----------------------------------------------------
	// composite (constraint)

	Sq::Column *index(const char *name, const char *column1_name, ...);
	Sq::Column *addIndex(const char *name, const char *column1_name, ...);
	void        dropIndex(const char *name);

	Sq::Column *unique(const char *name, const char *column1_name, ...);
	Sq::Column *addUnique(const char *name, const char *column1_name, ...);
	void        dropUnique(const char *name);

	Sq::Column *primary(const char *name, const char *column1_name, ...);
	Sq::Column *addPrimary(const char *name, const char *column1_name, ...);
	void        dropPrimary(const char *name);

	Sq::Column *foreign(const char *name, const char *column_name);
	Sq::Column *addForeign(const char *name, const char *column_name);
	void        dropForeign(const char *name);

/*
	template<int N>
	Sq::Column *index(const char *(&column_array)[N], const char *name = NULL)
	Sq::Column *index(const char *column1_name, ...);

	template<int N>
	Sq::Column *unique(const char *(&column_array)[N], const char *name = NULL);
	Sq::Column *unique(const char *column1_name, ...);

	template<int N>
	Sq::Column *primary(const char *(&column_array)[N], const char *name = NULL);
	Sq::Column *primary(const char *column1_name, ...);

	Sq::Column *foreign(const char *column_name, const char *name = NULL);
 */
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqTable defines SQL Table

	SqEntry
	|
	`--- SqReentry
	     |
	     `--- SqTable

	Migration - Alter Table : table->bit_field & SQB_CHANGED
	Migration - Drop   : table->name = NULL, table->old_name = column_name
	Migration - Rename : table->name = new_name, table->old_name = old_name

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.
*/

#define SQ_TABLE_MEMBERS     \
	SQ_REENTRY_MEMBERS;      \
	SqRelation  *relation

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

	// SqColumn's relation for (SQLite) migration.
	// sq_table_include() and sq_schema_include() store columns that having foreign reference.
	// sq_schema_trace_name() use these to trace renamed (or dropped) column that was referenced by others.
	// free it if you don't need to sync table changed to database.
	SqRelation  *relation;
 */
};

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
inline bool  TableMethod::hasColumn(const char *column_name) {
	return sq_table_has_column((SqTable*)this, column_name);
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

inline Sq::Column&  TableMethod::boolean(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::bool_(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::integer(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::int_(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::uint(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_uint((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::int64(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_int64((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::uint64(const char *column_name, size_t offset) {
	return *(Sq::Column*)sq_table_add_uint64((SqTable*)this, column_name, offset);
}
inline Sq::Column&  TableMethod::timestamp(const char *column_name, size_t offset) {
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
inline Sq::Column&  TableMethod::double_(const char *column_name, size_t offset, int precision, int scale) {
	return *(Sq::Column*)sq_table_add_double((SqTable*)this, column_name, offset, precision, scale);
}
inline Sq::Column&  TableMethod::string(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, offset, length);
}
inline Sq::Column&  TableMethod::char_(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_char((SqTable*)this, column_name, offset, length);
}
inline Sq::Column&  TableMethod::custom(const char *column_name, size_t offset, const SqType *type, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, offset, type, length);
}

inline Sq::Column&  TableMethod::stdstring(const char *column_name, size_t offset, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, offset, SQ_TYPE_STD_STRING, length);
}

template<class Store, class Type>
inline Sq::Column&  TableMethod::boolean(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::bool_(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_bool((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::integer(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::int_(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::uint(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_uint((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::int64(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_int64((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::uint64(const char *column_name, Type Store::*member) {
	return *(Sq::Column*)sq_table_add_uint64((SqTable*)this, column_name, Sq::offsetOf(member));
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::timestamp(const char *column_name, Type Store::*member) {
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
inline Sq::Column&  TableMethod::double_(const char *column_name, Type Store::*member, int precision, int scale) {
	return *(Sq::Column*)sq_table_add_double((SqTable*)this, column_name, Sq::offsetOf(member), precision, scale);
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::string(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_string((SqTable*)this, column_name, Sq::offsetOf(member), length);
};
template<class Store, class Type>
inline Sq::Column&  TableMethod::custom(const char *column_name, Type Store::*member, const SqType *type, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, Sq::offsetOf(member), type, length);
};

template<class Store, class Type>
inline Sq::Column&  TableMethod::stdstring(const char *column_name, Type Store::*member, int length) {
	return *(Sq::Column*)sq_table_add_custom((SqTable*)this, column_name, Sq::offsetOf(member), SQ_TYPE_STD_STRING, length);
};

// define composite (constraint) methods of TableMethod

inline Sq::Column *TableMethod::index(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_INDEX, 0, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline Sq::Column *TableMethod::addIndex(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_INDEX, 0, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline void  TableMethod::dropIndex(const char *name) {
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_INDEX, 0, name);
}

inline Sq::Column *TableMethod::unique(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline Sq::Column *TableMethod::addUnique(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline void  TableMethod::dropUnique(const char *name) {
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
}

inline Sq::Column *TableMethod::primary(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline Sq::Column *TableMethod::addPrimary(const char *name, const char *column1_name, ...) {
	va_list   arg_list;
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
	va_start(arg_list, column1_name);
	sq_column_set_composite_va(column, column1_name, arg_list);
	va_end(arg_list);
	return (Sq::Column*)column;
}
inline void  TableMethod::dropPrimary(const char *name) {
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
}

inline Sq::Column *TableMethod::foreign(const char *name, const char *column_name) {
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
	sq_column_set_composite(column, column_name, NULL);
	return (Sq::Column*)column;
}
inline Sq::Column *TableMethod::addForeign(const char *name, const char *column_name) {
	SqColumn *column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
	sq_column_set_composite(column, column_name, NULL);
	return (Sq::Column*)column;
}
inline void  TableMethod::dropForeign(const char *name) {
	sq_table_drop_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
}

/*
template<int N>
inline Sq::Column *TableMethod::index(const char *(&column_array)[N], const char *name)
inline Sq::Column *TableMethod::index(const char *column1_name, ...);

template<int N>
inline Sq::Column *TableMethod::unique(const char *(&column_array)[N], const char *name);
inline Sq::Column *TableMethod::unique(const char *column1_name, ...);

template<int N>
inline Sq::Column *TableMethod::primary(const char *(&column_array)[N], const char *name);
inline Sq::Column *TableMethod::primary(const char *column1_name, ...);

inline Sq::Column *TableMethod::foreign(const char *column_name, const char *name) {
	SqColumn column;

	column = sq_table_add_composite((SqTable*)this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
	sq_column_set_composite(column, column_name, NULL);
	return (Sq::Column*)column;
}
 */


/* All derived struct/class must be C++11 standard-layout. */

struct Table : SqTable {
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TABLE_H
