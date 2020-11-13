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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqTable        SqTable;
typedef struct SqColumn       SqColumn;
typedef struct SqForeign      SqForeign;    // used by SqColumn

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

#define SQB_TABLE_COL_CHANGED             (SQB_TABLE_COL_ALTERED | \
                                           SQB_TABLE_COL_RENAMED | \
                                           SQB_TABLE_COL_DROPPED | \
                                           SQB_TABLE_COL_ADDED   | \
                                           SQB_TABLE_COL_ADDED_UNIQUE | \
                                           SQB_TABLE_COL_ADDED_CONSTRAINT )

// --------------------------------------------------------
// SqTable C functions

SqTable*  sq_table_new(const char* name, const SqType* type_info);
void      sq_table_free(SqTable* table_pub);

bool      sq_table_has_column(SqTable* table, const char* column_name);
void      sq_table_drop_column(SqTable* table, const char* column_name);
void      sq_table_rename_column(SqTable* table, const char* from, const char* to);

int       sq_table_get_columns(SqTable* table, SqPtrArray* ptr_array, unsigned int bit_field);
SqColumn* sq_table_get_primary(SqTable* table);

// TODO: static declared SqColumn
// int    sq_table_add_static(SqTable* table, const SqColumn* columns,
//                            int  n_columns);

#define sq_table_add_integer    sq_table_add_int

SqColumn* sq_table_add_int(SqTable* table, const char* column_name,
                           size_t offset);
SqColumn* sq_table_add_uint(SqTable* table, const char* column_name,
                            size_t offset);
SqColumn* sq_table_add_int64(SqTable* table, const char* column_name,
                             size_t offset);
SqColumn* sq_table_add_uint64(SqTable* table, const char* column_name,
                              size_t offset);
SqColumn* sq_table_add_double(SqTable* table, const char* column_name,
                              size_t offset);
SqColumn* sq_table_add_timestamp(SqTable* table, const char* column_name,
                                 size_t offset);
SqColumn* sq_table_add_string(SqTable* table, const char* column_name,
                              size_t offset, int length);
SqColumn* sq_table_add_custom(SqTable* table, const char* column_name,
                              size_t offset, const SqType* sqtype,
                              int  length);

/*
#define sq_table_add_int_as(table, structure, member)    \
		sq_table_add_int(table, #member, offsetof(structure, member))

#define sq_table_add_uint_as(table, structure, member)    \
		sq_table_add_uint(table, #member, offsetof(structure, member))

#define sq_table_add_int64_as(table, structure, member)    \
		sq_table_add_int64(table, #member, offsetof(structure, member))

#define sq_table_add_uint64_as(table, structure, member)    \
		sq_table_add_uint64(table, #member, offsetof(structure, member))

#define sq_table_add_double_as(table, structure, member)    \
		sq_table_add_double(table, #member, offsetof(structure, member))

#define sq_table_add_string_as(table, structure, member, length)    \
		sq_table_add_string(table, #member, offsetof(structure, member), length)

#define sq_table_add_custom_as(table, structure, member, type)    \
		sq_table_add_custom(table, #member, offsetof(structure, member), type)
 */

// --------------------------------------------------------
// SqTable C functions for CONSTRAINT

SqColumn* sq_table_add_composite_va(SqTable* table,
                                    unsigned int bit_field,
                                    const char* name,
                                    const char* column1_name,
                                    va_list arg_list);

void      sq_table_drop_composite(SqTable* table,
                                  unsigned int bit_field,
                                  const char* name);

// CREATE INDEX "index_name" ON "table" ("column");
// CREATE INDEX "index_name" ON "table" ("column1", "column2");
// the last argument must be NULL
SqColumn* sq_table_add_index(SqTable* table,
                             const char* index_name,
                             const char* column1_name, ...);

// MySQL: ALTER TABLE table_name DROP INDEX index_name;
// DROP INDEX index_name
// DROP INDEX table_name.index_name;
void      sq_table_drop_index(SqTable* table, const char* index_name);

// CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// ADD CONSTRAINT "unique_name" UNIQUE ("column1_name", "column2_name")
// the last argument must be NULL
SqColumn* sq_table_add_unique(SqTable* table,
                              const char* unique_name,
                              const char* column1_name, ...);

// MySQL: ALTER TABLE "table" DROP INDEX "unique_name";
// other: ALTER TABLE "table" DROP CONSTRAINT "unique_name";
void      sq_table_drop_unique(SqTable* table, const char* unique_name);

// CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// ADD CONSTRAINT "primary_name" PRIMARY KEY ("column1_name", "column2_name");
// the last argument must be NULL
SqColumn* sq_table_add_primary(SqTable* table,
                               const char* primary_name,
                               const char* column1_name, ...);

// MySQL: ALTER TABLE "customer" DROP PRIMARY KEY;
// other: ALTER TABLE "customer" DROP CONSTRAINT "name";
void      sq_table_drop_primary(SqTable* table, const char* name);

// CONSTRAINT "name" FOREIGN KEY (column_name) REFERENCES customers(id);
// ADD CONSTRAINT "name" FOREIGN KEY (column_name) REFERENCES customers(id);
SqColumn* sq_table_add_foreign(SqTable* table,
                               const char* foreign_name,
                               const char* column_name);
void      sq_table_drop_foreign(SqTable* table, const char* name);

/* --------------------------------------------------------
	migration functions
 */

// function will free old column and replace it by 'new_one'
// if 'new_one' is NULL, it remove old column and set NULL in it's address.
void      sq_table_replace_column(SqTable*   table,
                                  SqColumn** old_in_type,
                                  SqColumn** old_in_foreigns,
                                  SqColumn*  new_one);

// This used by migration: include and apply changes from 'table_src'.
// It may move/steal columns from 'table_src'.
int       sq_table_include(SqTable* table, SqTable* table_src);

// table->type->entry remove columns found in 'excluded_columns', remained columns output to 'result'.
void      sq_table_exclude(SqTable* table, SqPtrArray* excluded_columns, SqPtrArray* result);

void      sq_table_complete(SqTable* table);

// sort column by it's attribute
//	sq_ptr_array_sort(result, (SqCompareFunc)sq_column_cmp_attrib);

// --------------------------------------------------------
// SqColumn C functions

SqColumn*  sq_column_new(const char* name, const SqType* type_info);
void       sq_column_free(SqColumn* column);

// create new SqColumn and copy data from static one.
SqColumn*  sq_column_copy_static(const SqColumn* column_src);

// foreign key references
void       sq_column_reference(SqColumn* column,
                               const char* foreign_table_name,
                               const char* foreign_column_name);
// foreign key on delete
void       sq_column_on_delete(SqColumn* column, const char* act);
// foreign key on update
void       sq_column_on_update(SqColumn* column, const char* act);

#define sq_column_foreign    sq_column_reference

// the last argument must be NULL
// sq_column_set_composite(column, colume_name1, column_name2, NULL);
void       sq_column_set_composite(SqColumn* column, ...);
void       sq_column_set_composite_va(SqColumn* column, const char *name, va_list arg_list);

// used by sq_table_arrange()
// primary key = 0
// foreign key = 1
// normal      = 2
// constraint  = 3
int  sq_column_cmp_attrib(SqColumn** column1, SqColumn** column2);

#ifdef __cplusplus
}  // extern "C"
#endif


// ----------------------------------------------------------------------------
// C/C++ inline functions

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// C99 or C++ inline functions

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
SqColumn* sq_table_add_composite(SqTable* table,
                                 unsigned int bit_field,
                                 const char* name,
                                 const char* column1_name, ...)
{
	SqColumn* column;

	va_list  arg_list;
	va_start(arg_list, column1_name);
	column = sq_table_add_composite_va(table, bit_field, name, column1_name, arg_list);
	va_end(arg_list);
	return column;
}

#else  // __STDC_VERSION__ || __cplusplus

// C functions
SqColumn* sq_table_add_composite(SqTable* table,
                                 unsigned int bit_field,
                                 const char* name,
                                 const char* column1_name, ...);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// Sq C++ functions

#ifdef __cplusplus

namespace Sq {

#if 0
template<class Store, class Type>
inline size_t offsetOf(Type Store::*member) {
	static Store obj;
	return size_t(&(obj.*member)) - size_t(&obj);
}
#else
template<typename T, typename U> constexpr size_t offsetOf(U T::*member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}
#endif

};

#endif  // __cplusplus

/* ----------------------------------------------------------------------------
	SqTable: SQL Table

	Migration - Alter Table : table->bit_field & SQB_CHANGED
	Migration - Drop   : table->name = NULL, table->old_name = column_name
	Migration - Rename : table->name = new_name, table->old_name = old_name
*/

struct SqTable
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	SqType*      type;        // type information for this entry
	char*        name;
	size_t       offset;      // sq_schema_trace_foreign() and migration use this
	unsigned int bit_field;
	char*        old_name;    // rename or drop
 */

	// ------ SqTable members ------

	// SqColumn's array for temporary use.
	// sq_table_include() and sq_schema_include() store columns that having foreign reference.
	// sq_schema_trace_foreign() use these to trace renamed (or dropped) column that was referenced by others.
	// finalize it after creating table in SQL
	SqPtrArray   foreigns;


#ifdef __cplusplus
	// C++11 standard-layout

	SqColumn& integer(const char* column_name, size_t offset)
		{ return *sq_table_add_int(this, column_name, offset); }
	SqColumn& int_(const char* column_name, size_t offset)
		{ return *sq_table_add_int(this, column_name, offset); }
	SqColumn& uint(const char* column_name, size_t offset)
		{ return *sq_table_add_uint(this, column_name, offset); }
	SqColumn& int64(const char* column_name, size_t offset)
		{ return *sq_table_add_int64(this, column_name, offset); }
	SqColumn& uint64(const char* column_name, size_t offset)
		{ return *sq_table_add_uint64(this, column_name, offset); }
	SqColumn& timestamp(const char* column_name, size_t offset)
		{ return *sq_table_add_timestamp(this, column_name, offset); }
	SqColumn& double_(const char* column_name, size_t offset)
		{ return *sq_table_add_double(this, column_name, offset); }
	SqColumn& string(const char* column_name, size_t offset, int length = -1)
		{ return *sq_table_add_string(this, column_name, offset, length); }
	SqColumn& custom(const char* column_name, size_t offset, SqType* type, int length = -1)
		{ return *sq_table_add_custom(this, column_name, offset, type, length); }

	SqColumn& stdstring(const char* column_name, size_t offset, int length = -1) {
		return *sq_table_add_custom(this, column_name, offset, SQ_TYPE_STD_STRING, length);
	}

	template<class Store, class Type>
	SqColumn& integer(const char* column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int_(const char* column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int64(const char* column_name, Type Store::*member) {
		return *sq_table_add_int64(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint64(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint64(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& timestamp(const char* column_name, Type Store::*member) {
		return *sq_table_add_timestamp(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& double_(const char* column_name, Type Store::*member) {
		return *sq_table_add_double(this, column_name, Sq::offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& string(const char* column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_string(this, column_name, Sq::offsetOf(member), length);
	};
	template<class Store, class Type>
	SqColumn& custom(const char* column_name, Type Store::*member, SqType* type, int length = -1) {
		return *sq_table_add_custom(this, column_name, Sq::offsetOf(member), type, length);
	};

	template<class Store, class Type>
	SqColumn& stdstring(const char* column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_custom(this, column_name, offset, SQ_TYPE_STD_STRING, length);
	};

	// ----------------------------------------------------
	// composite (constraint)

	SqColumn* addIndex(const char* name, const char* column1_name, ...) {
		SqColumn* column;
		va_list  arg_list;
		va_start(arg_list, column1_name);
		column = sq_table_add_composite_va(this, SQB_INDEX, name, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropIndex(const char* name) {
		sq_table_drop_composite(this, SQB_INDEX, name);
	}

	SqColumn* addUnique(const char* name, const char* column1_name, ...) {
		SqColumn* column;
		va_list  arg_list;
		va_start(arg_list, column1_name);
		column = sq_table_add_composite_va(this, SQB_UNIQUE | SQB_CONSTRAINT, name, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropUnique(const char* name) {
		sq_table_drop_composite(this, SQB_UNIQUE | SQB_CONSTRAINT, name);
	}

	SqColumn* addPrimary(const char* name, const char* column1_name, ...) {
		SqColumn* column;
		va_list  arg_list;
		va_start(arg_list, column1_name);
		column = sq_table_add_composite_va(this, SQB_PRIMARY | SQB_CONSTRAINT, name, column1_name, arg_list);
		va_end(arg_list);
		return column;
	}
	void  dropPrimary(const char* name) {
		sq_table_drop_composite(this, SQB_PRIMARY | SQB_CONSTRAINT, name);
	}

	SqColumn* addForeign(const char* name, const char* column_name) {
		return sq_table_add_composite(this, SQB_FOREIGN | SQB_CONSTRAINT, name, column_name, NULL);
	}
	void  dropForeign(const char* name) {
		sq_table_drop_composite(this, SQB_FOREIGN | SQB_CONSTRAINT, name);
	}

/*
	template<int N>
	SqColumn* index(const char* (&column_array)[N], const char* name = NULL)
	SqColumn* index(const char* column1_name, ...);

	template<int N>
	SqColumn* unique(const char* (&column_array)[N], const char* name = NULL);
	SqColumn* unique(const char* column1_name, ...);

	template<int N>
	SqColumn* primary(const char* (&column_array)[N], const char* name = NULL);
	SqColumn* primary(const char* column1_name, ...);

	SqColumn* foreign(const char* column, const char* name = NULL) {
		sq_table_add_composite(this, SQB_FOREIGN | SQB_CONSTRAINT, name, column, NULL);
	}
 */

	// ----------------------------------------------------

	int  include(SqTable* table_src)
		{ return sq_table_include(this, table_src); }
	void exclude(SqPtrArray* excluded_columns, SqPtrArray* result)
		{ sq_table_exclude(this, excluded_columns, result); }
#endif  // __cplusplus
};


/* ----------------------------------------------------------------------------
	SqForeign: foreign key data
 */
struct SqForeign
{
	char*  table;
	char*  column;
	char*  on_delete;
	char*  on_update;
};

/* ----------------------------------------------------------------------------
	SqColumn: SQL Column

	Migration - Alter Type : column->bit_field & SQB_CHANGED
	Migration - Drop   : column->name = NULL, column->old_name = column_name
	Migration - Rename : column->name = new_name, column->old_name = old_name
*/

struct SqColumn
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	SqType*      type;        // type information for this entry
	char*        name;
	size_t       offset;
	unsigned int bit_field;
	char*        old_name;    // rename or drop
 */

	// ------ SqColumn members ------

	// size  : total number of digits is specified in size or length of string
	// digits: number of digits after the decimal point.
	int16_t      size;             // total digits or length of string
	int16_t      digits;           // decimal digits

	char*        default_value;    // create
	char*        check;            // CHECK (condition)

	SqForeign*   foreign;          // foreign key
	char**       composite;        // Null-terminated (column-name) string array

	char*        extra;            // raw SQL column property

	/*
	struct SqExtra {
		char*    comment;          // create
		char*    raw;              // raw SQL column property
	} *extra;
	 */

	// if column->name is NULL, it will drop column->old_name
	// if column->name is NOT NULL, it will rename from column->old_name to column->name

#ifdef __cplusplus
	// C++11 standard-layout

	SqColumn* operator->()
		{ return this; }

	// ----------------------------------------------------

	SqColumn& reference(const char *table_name, const char *column_name)
		{ sq_column_reference(this, table_name, column_name); return *this; }
	SqColumn& onDelete(const char *act)
		{ sq_column_on_delete(this, act); return *this; }
	SqColumn& onUpdate(const char *act)
		{ sq_column_on_update(this, act); return *this; }
	SqColumn& setComposite(const char *name, ...) {
		va_list  arg_list;
		va_start(arg_list, name);
		sq_column_set_composite_va(this, name, arg_list);
		va_end(arg_list);
		return *this;
	}

	SqColumn& primary()
		{ bit_field |= SQB_PRIMARY;   return *this; }
	SqColumn& unique()
		{ bit_field |= SQB_UNIQUE;    return *this; }
	SqColumn& increment()
		{ bit_field |= SQB_INCREMENT; return *this; }
	SqColumn& nullable()
		{ bit_field |= SQB_NULLABLE;  return *this; }
	SqColumn& change()
		{ bit_field |= SQB_CHANGED;   return *this; }
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus
namespace Sq {

// These are for directly use only. You can NOT derived it.
typedef struct SqTable     Table;
typedef struct SqColumn    Column;
typedef struct SqForeign   Foreign;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TABLE_H
