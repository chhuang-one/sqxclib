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


/*
	SqEntry
	|
	`--- SqReentry
	     |
	     +--- SqTable
	     |
	     `--- SqColumn
 */

#ifndef SQ_TABLE_H
#define SQ_TABLE_H

#include <stddef.h>     // size_t
#include <stdint.h>
#include <stdarg.h>

#include <SqEntry.h>
#include <SqRelation.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqTable        SqTable;
typedef struct SqColumn       SqColumn;
typedef struct SqForeign      SqForeign;    // used by SqColumn

// SQL special type
#define SQ_TYPE_CONSTRAINT    SQ_TYPE_FAKE0
#define SQ_TYPE_INDEX         SQ_TYPE_FAKE1

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

#define SQ_N_COLUMNS(ColumnArray)    ( sizeof(ColumnArray) / sizeof(SqColumn) )

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

/* --- SqColumn C functions --- */

SqColumn  *sq_column_new(const char *name, const SqType *type_info);
void       sq_column_free(SqColumn *column);

void       sq_column_default(SqColumn *column, const char *default_value);
void       sq_column_raw(SqColumn *column, const char *raw_property);

// create new SqColumn and copy data from static one.
SqColumn  *sq_column_copy_static(const SqColumn *column_src);

// foreign key references
void       sq_column_reference(SqColumn *column,
                               const char *foreign_table_name,
                               const char *foreign_column_name);
// foreign key on delete
void       sq_column_on_delete(SqColumn *column, const char *act);
// foreign key on update
void       sq_column_on_update(SqColumn *column, const char *act);

#define sq_column_foreign    sq_column_reference

// the last argument must be NULL
// sq_column_set_composite(column, colume_name1, column_name2, NULL);
void       sq_column_set_composite(SqColumn *column, ...);
void       sq_column_set_composite_va(SqColumn *column, const char *name, va_list arg_list);

// used by sq_table_arrange()
// primary key = 0
// foreign key = 1
// normal      = 2
// constraint  = 3
int  sq_column_cmp_attrib(SqColumn **column1, SqColumn **column2);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

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

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
	SqTable: SQL Table

	Migration - Alter Table : table->bit_field & SQB_CHANGED
	Migration - Drop   : table->name = NULL, table->old_name = column_name
	Migration - Rename : table->name = new_name, table->old_name = old_name

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.
*/

struct SqTable
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	const SqType *type;        // type information for this entry
	const char   *name;
	size_t        offset;      // migration use this. Number of columns have existed in database
	unsigned int  bit_field;
	const char   *old_name;    // rename or drop
 */

	// ------ SqTable members ------

	// SqColumn's relation for (SQLite) migration.
	// sq_table_include() and sq_schema_include() store columns that having foreign reference.
	// sq_schema_trace_name() use these to trace renamed (or dropped) column that was referenced by others.
	// free it if you don't need to sync table changed to database.
	SqRelation  *relation;

#ifdef __cplusplus
	// C++11 standard-layout

	bool  hasColumn(const char *column_name) {
		return sq_table_has_column(this, column_name);
	}
	void  dropColumn(const char *column_name) {
		sq_table_drop_column(this, column_name);
	}
	void  renameColumn(const char *from, const char *to) {
		sq_table_rename_column(this, from, to);
	}
	int   getColumns(SqPtrArray *ptr_array, const SqType *type, unsigned int bit_field) {
		return sq_table_get_columns(this, ptr_array, type, bit_field);
	}
	SqColumn *getPrimary() {
		return sq_table_get_primary(this, NULL);
	}

	void  addColumn(const SqColumn *columns, int n_columns = 1) {
		sq_table_add_column(this, columns, n_columns);
	}
	void  addColumn(const SqColumn **column_ptrs, int n_column_ptrs = 1) {
		sq_table_add_column_ptrs(this, column_ptrs, n_column_ptrs);
	}

	SqColumn& boolean(const char *column_name, size_t offset) {
		return *sq_table_add_bool(this, column_name, offset);
	}
	SqColumn& bool_(const char *column_name, size_t offset) {
		return *sq_table_add_bool(this, column_name, offset);
	}
	SqColumn& integer(const char *column_name, size_t offset) {
		return *sq_table_add_int(this, column_name, offset);
	}
	SqColumn& int_(const char *column_name, size_t offset) {
		return *sq_table_add_int(this, column_name, offset);
	}
	SqColumn& uint(const char *column_name, size_t offset) {
		return *sq_table_add_uint(this, column_name, offset);
	}
	SqColumn& int64(const char *column_name, size_t offset) {
		return *sq_table_add_int64(this, column_name, offset);
	}
	SqColumn& uint64(const char *column_name, size_t offset) {
		return *sq_table_add_uint64(this, column_name, offset);
	}
	SqColumn& timestamp(const char *column_name, size_t offset) {
		return *sq_table_add_timestamp(this, column_name, offset);
	}
	void      timestamps(const char *created_at_name, size_t created_at_offset,
	                     const char *updated_at_name, size_t updated_at_offset) {
		sq_table_add_timestamps(this, created_at_name, created_at_offset,
		                              updated_at_name, updated_at_offset);
	}
	void      timestamps(size_t created_at_offset, size_t updated_at_offset) {
		sq_table_add_timestamps(this, NULL, created_at_offset,
		                              NULL, updated_at_offset);
	}
	SqColumn& double_(const char *column_name, size_t offset, int precision = 0, int scale = 0) {
		return *sq_table_add_double(this, column_name, offset, precision, scale);
	}
	SqColumn& string(const char *column_name, size_t offset, int length = -1) {
		return *sq_table_add_string(this, column_name, offset, length);
	}
	SqColumn& char_(const char *column_name, size_t offset, int length = -1) {
		return *sq_table_add_char(this, column_name, offset, length);
	}
	SqColumn& custom(const char *column_name, size_t offset, const SqType *type, int length = -1) {
		return *sq_table_add_custom(this, column_name, offset, type, length);
	}

	SqColumn& stdstring(const char *column_name, size_t offset, int length = -1) {
		return *sq_table_add_custom(this, column_name, offset, SQ_TYPE_STD_STRING, length);
	}

	template<class Store, class Type>
	SqColumn& boolean(const char *column_name, Type Store::*member) {
		return *sq_table_add_bool(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& bool_(const char *column_name, Type Store::*member) {
		return *sq_table_add_bool(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& integer(const char *column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int_(const char *column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint(const char *column_name, Type Store::*member) {
		return *sq_table_add_uint(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int64(const char *column_name, Type Store::*member) {
		return *sq_table_add_int64(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint64(const char *column_name, Type Store::*member) {
		return *sq_table_add_uint64(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& timestamp(const char *column_name, Type Store::*member) {
		return *sq_table_add_timestamp(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	void      timestamps(const char *created_at_name, Type Store::*created_at_member,
	                     const char *updated_at_name, Type Store::*updated_at_member) {
		sq_table_add_timestamps(this, created_at_name, Sq::offsetOf(created_at_member),
		                              updated_at_name, Sq::offsetOf(updated_at_member));
	};
	template<class Store, class Type>
	void      timestamps(Type Store::*created_at_member, Type Store::*updated_at_member) {
		sq_table_add_timestamps(this, NULL, Sq::offsetOf(created_at_member),
		                              NULL, Sq::offsetOf(updated_at_member));
	};
	template<class Store>
	void      timestamps() {
		sq_table_add_timestamps(this, NULL, Sq::offsetOf(&Store::created_at),
		                              NULL, Sq::offsetOf(&Store::updated_at));
	};
	template<class Store, class Type>
	SqColumn& double_(const char *column_name, Type Store::*member, int precision = 0, int scale = 0) {
		return *sq_table_add_double(this, column_name, Sq::offsetOf(member), precision, scale);
	};
	template<class Store, class Type>
	SqColumn& string(const char *column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_string(this, column_name, Sq::offsetOf(member), length);
	};
	template<class Store, class Type>
	SqColumn& custom(const char *column_name, Type Store::*member, const SqType *type, int length = -1) {
		return *sq_table_add_custom(this, column_name, Sq::offsetOf(member), type, length);
	};

	template<class Store, class Type>
	SqColumn& stdstring(const char *column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_custom(this, column_name, Sq::offsetOf(member), SQ_TYPE_STD_STRING, length);
	};

	// ----------------------------------------------------
	// composite (constraint)

	SqColumn *index(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_INDEX, 0, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	SqColumn *addIndex(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_INDEX, 0, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropIndex(const char *name) {
		sq_table_drop_composite(this, SQ_TYPE_INDEX, 0, name);
	}

	SqColumn *unique(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	SqColumn *addUnique(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropUnique(const char *name) {
		sq_table_drop_composite(this, SQ_TYPE_CONSTRAINT, SQB_UNIQUE, name);
	}

	SqColumn *primary(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	SqColumn *addPrimary(const char *name, const char *column1_name, ...) {
		va_list   arg_list;
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
		va_start(arg_list, column1_name);
		sq_column_set_composite_va(column, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropPrimary(const char *name) {
		sq_table_drop_composite(this, SQ_TYPE_CONSTRAINT, SQB_PRIMARY, name);
	}

	SqColumn *foreign(const char *name, const char *column_name) {
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
		sq_column_set_composite(column, column_name, NULL);
		return column;
	}
	SqColumn *addForeign(const char *name, const char *column_name) {
		SqColumn *column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
		sq_column_set_composite(column, column_name, NULL);
		return column;
	}
	void  dropForeign(const char *name) {
		sq_table_drop_composite(this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
	}

/*
	template<int N>
	SqColumn *index(const char *(&column_array)[N], const char *name = NULL)
	SqColumn *index(const char *column1_name, ...);

	template<int N>
	SqColumn *unique(const char *(&column_array)[N], const char *name = NULL);
	SqColumn *unique(const char *column1_name, ...);

	template<int N>
	SqColumn *primary(const char *(&column_array)[N], const char *name = NULL);
	SqColumn *primary(const char *column1_name, ...);

	SqColumn *foreign(const char *column_name, const char *name = NULL) {
		SqColumn column;

		column = sq_table_add_composite(this, SQ_TYPE_CONSTRAINT, SQB_FOREIGN, name);
		sq_column_set_composite(column, column_name, NULL);
		return column;
	}
 */
#endif  // __cplusplus
};


/*
	SqForeign: foreign key data in SqColumn
 */

struct SqForeign
{
	// Note: use 'const char*' to declare string here, C++ user can initialize static structure easily.
	const char  *table;
	const char  *column;
	const char  *on_delete;
	const char  *on_update;
};

/*
	SqColumn: SQL Column

	Migration - Alter Type : column->bit_field & SQB_CHANGED
	Migration - Drop   : column->name = NULL, column->old_name = column_name
	Migration - Rename : column->name = new_name, column->old_name = old_name

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.
*/

struct SqColumn
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	const SqType *type;        // type information for this entry
	const char   *name;
	size_t        offset;
	unsigned int  bit_field;
	const char   *old_name;    // rename or drop
 */

	// ------ SqColumn members ------

	// size  : total number of digits is specified in size or length of string
	// digits: number of digits after the decimal point.
	int16_t      size;             // total digits   (precision) or length of string
	int16_t      digits;           // decimal digits (scale)

	const char  *default_value;    // DEFAULT
	const char  *check;            // CHECK (condition)

	SqForeign   *foreign;          // foreign key
	char       **composite;        // Null-terminated (column-name) string array

	const char  *raw;              // raw SQL column property

	/*
	struct SqExtra {
		char    *comment;          // COMMENT
		char    *others;
	} *extra;
	 */

	// if column->name is NULL, it will drop column->old_name
	// if column->name is NOT NULL, it will rename from column->old_name to column->name

#ifdef __cplusplus
	// C++11 standard-layout

	SqColumn *operator->() {
		return this;
	}

	// ----------------------------------------------------

	SqColumn& reference(const char *table_name, const char *column_name) {
		sq_column_reference(this, table_name, column_name); return *this;
	}
	SqColumn& onDelete(const char *act) {
		sq_column_on_delete(this, act); return *this;
	}
	SqColumn& onUpdate(const char *act) {
		sq_column_on_update(this, act); return *this;
	}
	SqColumn& setComposite(const char *name, ...) {
		va_list  arg_list;
		va_start(arg_list, name);
		sq_column_set_composite_va(this, name, arg_list);
		va_end(arg_list);
		return *this;
	}

	// C/C++ pointer
	SqColumn& pointer() {
		bit_field |= SQB_POINTER;   return *this;
	}
	// JSON
	SqColumn& hidden() {
		bit_field |= SQB_HIDDEN;    return *this;
	}
	SqColumn& hiddenNull() {
		bit_field |= SQB_HIDDEN_NULL;   return *this;
	}
	// SQL column property
	SqColumn& primary() {
		bit_field |= SQB_PRIMARY;   return *this;
	}
	SqColumn& unique() {
		bit_field |= SQB_UNIQUE;    return *this;
	}
	SqColumn& increment() {
		bit_field |= SQB_INCREMENT; return *this;
	}
	SqColumn& autoIncrement() {
		bit_field |= SQB_INCREMENT; return *this;
	}
	SqColumn& nullable() {
		bit_field |= SQB_NULLABLE;  return *this;
	}
	SqColumn& change() {
		bit_field |= SQB_CHANGED;   return *this;
	}
	SqColumn& useCurrent() {
		bit_field |= SQB_CURRENT;   return *this;
	}
	SqColumn& useCurrentOnUpdate() {
		bit_field |= SQB_CURRENT_ON_UPDATE;   return *this;
	}

	SqColumn& default_(const char *default_val) {
		sq_column_default(this, default_val);  return *this;
	}
	SqColumn& defaultValue(const char *default_val) {
		sq_column_default(this, default_val);  return *this;
	}
	SqColumn& raw_(const char *raw_property) {
		sq_column_raw(this, raw_property);  return *this;
	}
	SqColumn& rawProperty(const char *raw_property) {
		sq_column_raw(this, raw_property);  return *this;
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus
namespace Sq {

// These are for directly use only. You can NOT derived it.
typedef struct SqTable     Table;
typedef struct SqColumn    Column;
typedef struct SqForeign   Foreign;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TABLE_H
