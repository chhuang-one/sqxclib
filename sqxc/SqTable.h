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


/*	DataBase object - [Server Name].[DataBase Name].[Schema].[Table Name]

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

#include <SqEntry.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqTable        SqTable;
typedef struct SqColumn       SqColumn;
typedef struct SqForeign      SqForeign;    // used by SqColumn

// SqTable::bit_field for SQLite
#define SQB_TABLE_COL_ALTERED             (1 << 16)
#define SQB_TABLE_COL_RENAMED             (1 << 17)
#define SQB_TABLE_COL_DROPPED             (1 << 18)
#define SQB_TABLE_COL_ADDED               (1 << 19)
#define SQB_TABLE_COL_ADDED_CONSTRAINT    (1 << 20)
#define SQB_TABLE_COL_FOREIGN             (1 << 21)

#define SQB_TABLE_COL_ATTRIB              (SQB_TABLE_COL_ALTERED | \
                                           SQB_TABLE_COL_RENAMED | \
                                           SQB_TABLE_COL_DROPPED | \
                                           SQB_TABLE_COL_ADDED   | \
                                           SQB_TABLE_COL_ADDED_CONSTRAINT | \
										   SQB_TABLE_COL_FOREIGN )

// --------------------------------------------------------
// SqTable C functions

SqTable*  sq_table_new(const char* name, const SqType* type_info);
void      sq_table_free(SqTable* table_pub);

#ifdef SQ_SUPPORT_STATIC_TABLE
// create new SqTable and copy data from static one.
SqTable*  sq_table_copy_static(const SqTable* table_src);
#endif   // SQ_SUPPORT_STATIC_TABLE

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

// This used by migration: accumulate changes from 'table_src'.
// It may move/steal columns from 'table_src'.
int       sq_table_accumulate(SqTable* table, SqTable* table_src);

// unique('column_name')
// index('column_name')
// index('column_name', 'name2')
// primary('column_name');
// primary('column_name', 'name2');

// $table->dropPrimary('users_id_primary');	從「users」資料表移除主鍵。
// $table->dropUnique('users_email_unique'); 從「users」資料表移除唯一索引。
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
void       sq_column_set_constraint_va(SqColumn* column, va_list arg_list);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// Sq C++ functions

#ifdef __cplusplus
template<class Store, class Type>
inline size_t sq_offset(Type Store::*member) {
	static Store obj;
	return size_t(&(obj.*member)) - size_t(&obj);
}
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
	size_t       offset;      // sq_schema_trace_foreign() use this
	unsigned int bit_field;
	char*        old_name;    // rename or drop
 */
	// if name is NULL, it will drop old_name
	// if name is NOT NULL, it will rename from old_name to name

	// SqColumn's array for temporary use.
	// 1. sq_table_accumulate() and sq_schema_accumulate() store columns that having foreign reference.
	SqPtrArray   arranged;

#ifdef __cplusplus
	// C++11 standard-layout
	// ----------------------------------------------------
	// Laravel-Eloquent-like API

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
		return *sq_table_add_int(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& int_(const char* column_name, Type Store::*member) {
		return *sq_table_add_int(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& uint(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& int64(const char* column_name, Type Store::*member) {
		return *sq_table_add_int64(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& uint64(const char* column_name, Type Store::*member) {
		return *sq_table_add_uint64(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& timestamp(const char* column_name, Type Store::*member) {
		return *sq_table_add_timestamp(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& double_(const char* column_name, Type Store::*member) {
		return *sq_table_add_double(this, column_name, sq_offset(member));
	};
	template<class Store, class Type>
	SqColumn& string(const char* column_name, Type Store::*member, int length = -1) {
		return *sq_table_add_string(this, column_name, sq_offset(member), length);
	};
	template<class Store, class Type>
	SqColumn& custom(const char* column_name, Type Store::*member, SqType* type) {
		return *sq_table_add_custom(this, column_name, sq_offset(member), type);
	};

/*
	void addColumn(SqColumn* column, int n_column = 1)
		{ sq_table_add_column(this, column, n_column); }
	bool hasColumn(const char* column_name)
		{ return sq_table_has_column(this, column_name); }	
	void dropColumn(const char* column_name)
		{ sq_table_drop_column(this, column_name); }
*/
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

	// size  : total number of digits is specified in size or length of string
	// digits: number of digits after the decimal point.
	int16_t      size;             // total digits or length of string
	int16_t      digits;           // decimal digits

	char*        default_value;    // create
	char*        check;            // CHECK (condition)
//	char*        comment;          // create

	SqForeign*   foreign;          // foreign key
	char**       constraint;       // Null-terminated string array

	char*        extra;            // raw SQL column property

	// if column->name is NULL, it will drop column->old_name
	// if column->name is NOT NULL, it will rename from column->old_name to column->name

#ifdef __cplusplus
	// C++11 standard-layout

	SqColumn* operator->()
		{ return this; }

	// ----------------------------------------------------
	// Laravel-Eloquent-like API

	SqColumn& reference(const char* table_name, const char* column_name)
		{ sq_column_reference(this, table_name, column_name); return *this; }
	SqColumn& onDelete(const char* act)
		{ sq_column_on_delete(this, act); return *this; }
	SqColumn& onUpdate(const char* act)
		{ sq_column_on_update(this, act); return *this; }

	SqColumn& primary()
		{ bit_field |= SQB_PRIMARY;   return *this; }
	SqColumn& unique()
		{ bit_field |= SQB_UNIQUE;    return *this; }
	SqColumn& increment()
		{ bit_field |= SQB_INCREMENT; return *this; }
	SqColumn& nullable()
		{ bit_field |= SQB_NULLABLE;  return *this; }
	SqColumn& change()
		{ bit_field |= SQB_CHANGE;    return *this; }
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
