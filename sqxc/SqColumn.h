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

#ifndef SQ_COLUMN_H
#define SQ_COLUMN_H

#include <stdarg.h>

#include <SqType.h>
#include <SqEntry.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqColumn       SqColumn;
typedef struct SqForeign      SqForeign;    // used by SqColumn


// SQL special type
#define SQ_TYPE_CONSTRAINT    SQ_TYPE_FAKE0
#define SQ_TYPE_INDEX         SQ_TYPE_FAKE1

#define SQ_N_COLUMNS(ColumnArray)    ( sizeof(ColumnArray) / sizeof(SqColumn) )

// SQL common bit_field
#define SQB_COLUMN_CHANGED              SQB_CHANGED         // SQL: column has been changed.
// SQL common bit_field (for internal use only. use it when SQLite recreate)
#define SQB_COLUMN_RENAMED              SQB_RENAMED         // SQL: column has been renamed.

/* SqColumn::bit_field for SQLite migration
   SQB_COLUMN_XXX must large then SQB_RESERVE_END because it derived from SqEntry. (SqEntry.h)
   SQB_COLUMN_XXX can overlap SQB_TABLE_XXX (SqTable.h)
 */
/* --- SqColumn::bit_field (Column Modifiers) --- */
#define SQB_COLUMN_PRIMARY              SQB_PRIMARY         // SQL: PRIMARY KEY
#define SQB_COLUMN_FOREIGN              SQB_FOREIGN         // SQL: FOREIGN KEY
#define SQB_COLUMN_UNIQUE               SQB_UNIQUE          // SQL: UNIQUE
#define SQB_COLUMN_INCREMENT            SQB_INCREMENT       // SQL: AUTOINCREMENT == SQB_AUTOINCREMENT
#define SQB_COLUMN_AUTOINCREMENT        SQB_AUTOINCREMENT   // SQL: AUTOINCREMENT == SQB_INCREMENT
#define SQB_COLUMN_NULLABLE             SQB_NULLABLE        // SQL: remove "NOT NULL"

// SqColumn::type == SQ_TYPE_TIME (use CURRENT_TIMESTAMP)
#define SQB_COLUMN_CURRENT              SQB_CURRENT              // SQL: DEFAULT CURRENT_TIMESTAMP
#define SQB_COLUMN_CURRENT_ON_UPDATE    SQB_CURRENT_ON_UPDATE    // SQL: CREATE TRIGGER AFTER UPDATE
#define SQB_COLUMN_CURRENT_ALL          SQB_CURRENT_ALL

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* SqColumn C functions */

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
void       sq_column_set_composite_va(SqColumn *column, va_list arg_list);

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

struct Column;

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*	SqForeign: foreign key data in SqColumn
 */
struct SqForeign
{
	// Note: use 'const char*' to declare string here, C++ user can initialize static structure easily.
	const char  *table;
	const char  *column;
	const char  *on_delete;
	const char  *on_update;
};

/*	SqColumn defines column in SQL table.

	SqEntry
	|
	`--- SqReentry
	     |
	     `--- SqColumn

	Migration - Alter Type : column->bit_field & SQB_CHANGED
	Migration - Drop   : column->name = NULL, column->old_name = column_name
	Migration - Rename : column->name = new_name, column->old_name = old_name

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.

	SqColumn must have no base struct because I need use aggregate initialization with it.
*/

#define SQ_COLUMN_MEMBERS       \
	SQ_REENTRY_MEMBERS;         \
	int16_t      size;          \
	int16_t      digits;        \
	const char  *default_value; \
	const char  *check;         \
	SqForeign   *foreign;       \
	char       **composite;     \
	const char  *raw

struct SqColumn
{
	SQ_COLUMN_MEMBERS;
/*	// ------ SqEntry members ------
	const SqType *type;             // field type
	const char   *name;             // column name
	size_t        offset;
	unsigned int  bit_field;

	// ------ SqReentry members ------
	const char   *old_name;         // use this when renaming or dropping

	// ------ SqColumn members ------

	// size  : total number of digits is specified in size, or length of string
	int16_t       size;
	// digits: number of digits after the decimal point.
	int16_t       digits;

	const char   *default_value;    // DEFAULT
	const char   *check;            // CHECK (condition)

	SqForeign    *foreign;          // foreign key
	char        **composite;        // Null-terminated (column-name) string array

	const char   *raw;              // raw SQL column property

//	struct SqExtra {
//		char    *comment;           // COMMENT
//		char    *others;
//	} *extra;

	// if column->name is NULL, it will drop column->old_name
	// if column->name is NOT NULL, it will rename from column->old_name to column->name
 */

#ifdef __cplusplus
	/* Note: If you add, remove, or change methods here, do the same things in Sq::ColumnMethod. */

	Sq::Column *operator->() {
		return (Sq::Column*)this;
	}

	Sq::Column &reference(const char *table_name, const char *column_name) {
		sq_column_reference((SqColumn*)this, table_name, column_name);
		return *(Sq::Column*)this;
	}
	Sq::Column &onDelete(const char *act) {
		sq_column_on_delete((SqColumn*)this, act);
		return *(Sq::Column*)this;
	}
	Sq::Column &onUpdate(const char *act) {
		sq_column_on_update((SqColumn*)this, act);
		return *(Sq::Column*)this;
	}
	template <typename... Args>
	Sq::Column &setComposite(const Args... args) {
		sq_column_set_composite((SqColumn*)this, args..., NULL);
		return *(Sq::Column*)this;
	}

	/* --- Column Modifiers --- */
	// C/C++ pointer
	Sq::Column &pointer() {
		((SqColumn*)this)->bit_field |= SQB_POINTER;
		return *(Sq::Column*)this;
	}
	// JSON
	Sq::Column &hidden() {
		((SqColumn*)this)->bit_field |= SQB_HIDDEN;
		return *(Sq::Column*)this;
	}
	Sq::Column &hiddenNull() {
		((SqColumn*)this)->bit_field |= SQB_HIDDEN_NULL;
		return *(Sq::Column*)this;
	}
	// SQL column property
	Sq::Column &primary() {
		((SqColumn*)this)->bit_field |= SQB_PRIMARY;
		return *(Sq::Column*)this;
	}
	Sq::Column &unique() {
		((SqColumn*)this)->bit_field |= SQB_UNIQUE;
		return *(Sq::Column*)this;
	}
	Sq::Column &increment() {
		((SqColumn*)this)->bit_field |= SQB_INCREMENT;    // equal SQB_AUTOINCREMENT
		return *(Sq::Column*)this;
	}
	Sq::Column &autoIncrement() {
		((SqColumn*)this)->bit_field |= SQB_AUTOINCREMENT;
		return *(Sq::Column*)this;
	}
	Sq::Column &nullable() {
		((SqColumn*)this)->bit_field |= SQB_NULLABLE;
		return *(Sq::Column*)this;
	}
	Sq::Column &change() {
		((SqColumn*)this)->bit_field |= SQB_CHANGED;
		return *(Sq::Column*)this;
	}
	Sq::Column &useCurrent() {
		((SqColumn*)this)->bit_field |= SQB_CURRENT;
		return *(Sq::Column*)this;
	}
	Sq::Column &useCurrentOnUpdate() {
		((SqColumn*)this)->bit_field |= SQB_CURRENT_ON_UPDATE;
		return *(Sq::Column*)this;
	}

	Sq::Column &default_(const char *default_val) {
		sq_column_default((SqColumn*)this, default_val);
		return *(Sq::Column*)this;
	}
	Sq::Column &defaultValue(const char *default_val) {
		sq_column_default((SqColumn*)this, default_val);
		return *(Sq::Column*)this;
	}
	Sq::Column &raw_(const char *raw_property) {
		sq_column_raw((SqColumn*)this, raw_property);
		return *(Sq::Column*)this;
	}
	Sq::Column &rawProperty(const char *raw_property) {
		sq_column_raw((SqColumn*)this, raw_property);
		return *(Sq::Column*)this;
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus
namespace Sq {

/*	ColumnMethod is used by SqColumn's children.

	It's derived struct/class must be C++11 standard-layout and has SqColumn members.

	Note: If you add, remove, or change methods here, do the same things in SqColumn.
 */
struct ColumnMethod
{
	Sq::Column *operator->() {
		return (Sq::Column*)this;
	}

	Sq::Column &reference(const char *table_name, const char *column_name) {
		sq_column_reference((SqColumn*)this, table_name, column_name);
		return *(Sq::Column*)this;
	}
	Sq::Column &onDelete(const char *act) {
		sq_column_on_delete((SqColumn*)this, act);
		return *(Sq::Column*)this;
	}
	Sq::Column &onUpdate(const char *act) {
		sq_column_on_update((SqColumn*)this, act);
		return *(Sq::Column*)this;
	}
	template <typename... Args>
	Sq::Column &setComposite(const Args... args) {
		sq_column_set_composite((SqColumn*)this, args..., NULL);
		return *(Sq::Column*)this;
	}

	/* --- Column Modifiers --- */
	// C/C++ pointer
	Sq::Column &pointer() {
		((SqColumn*)this)->bit_field |= SQB_POINTER;
		return *(Sq::Column*)this;
	}
	// JSON
	Sq::Column &hidden() {
		((SqColumn*)this)->bit_field |= SQB_HIDDEN;
		return *(Sq::Column*)this;
	}
	Sq::Column &hiddenNull() {
		((SqColumn*)this)->bit_field |= SQB_HIDDEN_NULL;
		return *(Sq::Column*)this;
	}
	// SQL column property
	Sq::Column &primary() {
		((SqColumn*)this)->bit_field |= SQB_PRIMARY;
		return *(Sq::Column*)this;
	}
	Sq::Column &unique() {
		((SqColumn*)this)->bit_field |= SQB_UNIQUE;
		return *(Sq::Column*)this;
	}
	Sq::Column &increment() {
		((SqColumn*)this)->bit_field |= SQB_INCREMENT;    // equal SQB_AUTOINCREMENT
		return *(Sq::Column*)this;
	}
	Sq::Column &autoIncrement() {
		((SqColumn*)this)->bit_field |= SQB_AUTOINCREMENT;
		return *(Sq::Column*)this;
	}
	Sq::Column &nullable() {
		((SqColumn*)this)->bit_field |= SQB_NULLABLE;
		return *(Sq::Column*)this;
	}
	Sq::Column &change() {
		((SqColumn*)this)->bit_field |= SQB_CHANGED;
		return *(Sq::Column*)this;
	}
	Sq::Column &useCurrent() {
		((SqColumn*)this)->bit_field |= SQB_CURRENT;
		return *(Sq::Column*)this;
	}
	Sq::Column &useCurrentOnUpdate() {
		((SqColumn*)this)->bit_field |= SQB_CURRENT_ON_UPDATE;
		return *(Sq::Column*)this;
	}

	Sq::Column &default_(const char *default_val) {
		sq_column_default((SqColumn*)this, default_val);
		return *(Sq::Column*)this;
	}
	Sq::Column &defaultValue(const char *default_val) {
		sq_column_default((SqColumn*)this, default_val);
		return *(Sq::Column*)this;
	}
	Sq::Column &raw_(const char *raw_property) {
		sq_column_raw((SqColumn*)this, raw_property);
		return *(Sq::Column*)this;
	}
	Sq::Column &rawProperty(const char *raw_property) {
		sq_column_raw((SqColumn*)this, raw_property);
		return *(Sq::Column*)this;
	}
};

/* All derived struct/class must be C++11 standard-layout. */

struct Column : SqColumn {
};

typedef struct SqForeign   Foreign;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_COLUMN_H
