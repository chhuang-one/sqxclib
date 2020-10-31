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

SqColumn* sq_table_get_primary(SqTable* table);
int       sq_table_get_foreigns(SqTable* table, SqPtrArray* array);

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
                              size_t offset, const SqType* sqtype);

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

/*
// UNIQUE (C_Id)
// CONSTRAINT "u_Customer_Id" UNIQUE ("C_Id", "Name")
// ADD CONSTRAINT "u_Customer_Id" UNIQUE ("C_Id", "Name");
SqColumn* sq_table_add_unique(SqTable* table,
                              const char* index_name,
                              const char* column1_name, ...);

// MySQL: ALTER TABLE "table" DROP INDEX "u_Customer_Id";
// other: ALTER TABLE "table" DROP CONSTRAINT "u_Customer_Id";
void      sq_table_drop_unique(SqTable* table, const char* name);

// CONSTRAINT "pk_Customer_Id" PRIMARY KEY ("C_Id", "Name");
// ADD CONSTRAINT "u_Customer_Id" PRIMARY KEY ("C_Id", "Name");
//
// auto generate primary_name if primary_name == NULL
// "tableName_columnName_primary"
SqColumn* sq_table_add_primary(SqTable* table,
                               const char* primary_name,
                               const char* column1_name, ...);
// MySQL: ALTER TABLE "customer" DROP PRIMARY KEY;
// other: ALTER TABLE "customer" DROP CONSTRAINT "pk_PersonID";
void      sq_table_drop_primary(SqTable* table, const char* name);

// CREATE INDEX "index_name" ON "table" ("column");
// CREATE INDEX "index_name" ON "table" ("column1", "column2");
SqColumn* sq_table_add_index(SqTable* table,
                             const char* index_name,
                             const char* column1_name, ...);

// MySQL: ALTER TABLE table_name DROP INDEX index_name;
// DROP INDEX index_name
// DROP INDEX table_name.index_name;
void      sq_table_drop_index(SqTable* table, const char* name);
 */

// FOREIGN KEY (C_Id) REFERENCES customers(C_Id);
// ADD FOREIGN KEY (C_Id) REFERENCES customers(C_Id);
SqColumn* sq_table_add_foreign(SqTable* table, const char* name);
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

// sort column by it's attribute
//	sq_ptr_array_sort(result, (SqCompareFunc)sq_column_cmp_attrib);

// unique('column_name')
// index('column_name')
// index('column_name', 'name2')
// primary('column_name');
// primary('column_name', 'name2');

// $table->dropPrimary('users_id_primary');	 // remove primary key from table "users"
// $table->dropUnique('users_email_unique'); // remove unique      from table "users"
// $table->dropIndex('state');
// $table->dropForeign(['user_id']);

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
// sq_column_set_constraint(column, colume_name1, column_name2, NULL);
void       sq_column_set_constraint(SqColumn* column, ...);
void       sq_column_set_constraint_va(SqColumn* column, const char *name, va_list arg_list);

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
// Sq C++ functions

#ifdef __cplusplus

#if 0
template<class Store, class Type>
inline size_t sq_offsetOf(Type Store::*member) {
	static Store obj;
	return size_t(&(obj.*member)) - size_t(&obj);
}
#else
template<typename T, typename U> constexpr size_t sq_offsetOf(U T::*member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}
#endif

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
	SqColumn& custom(const char* column_name, size_t offset, SqType* type)
		{ return *sq_table_add_custom(this, column_name, offset, type); }

	template<class Store, class Type>
	SqColumn& integer(const char* column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int_(const char* column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& int64(const char* column_name, Type Store::*member) {
		return *sq_table_add_int64(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& uint64(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint64(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& timestamp(const char* column_name, Type Store::*member) {
		return *sq_table_add_timestamp(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& double_(const char* column_name, Type Store::*member) {
		return *sq_table_add_double(this, column_name, sq_offsetOf(member));
	};
	template<class Store, class Type>
	SqColumn& string(const char* column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_string(this, column_name, sq_offsetOf(member), length);
	};
	template<class Store, class Type>
	SqColumn& custom(const char* column_name, Type Store::*member, SqType* type) {
		return *sq_table_add_custom(this, column_name, sq_offsetOf(member), type);
	};

	// ----------------------------------------------------

	SqColumn* addForeign(const char* name)
		{ return sq_table_add_foreign(this, name); }
	void  dropForeign(const char* name)
		{ sq_table_drop_foreign(this, name); }

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
//	char*        comment;

	SqForeign*   foreign;          // foreign key
	char**       constraint;       // Null-terminated string array

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
	SqColumn& setConstraint(const char *name, ...) {
		va_list  arg_list;
		va_start(arg_list, name);
		sq_column_set_constraint_va(this, name, arg_list);
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
namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqTable     Table;
typedef struct SqColumn    Column;
};  // namespace Sq
#endif  // __cplusplus


#endif  // SQ_TABLE_H
