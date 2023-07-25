/*
 *   Copyright (C) 2022-2023 by C.H. Huang
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

#ifndef SQ_ROW_H
#define SQ_ROW_H

#include <stdbool.h>

#include <SqType.h>
#include <SqJoint.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqRow              SqRow;
typedef struct SqRowColumn        SqRowColumn;
typedef        SqTypeJoint        SqTypeRow;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern  const  SqType      SqType_Row_;
#define SQ_TYPE_ROW      (&SqType_Row_)

/* macro for accessing variable of SqArray */

#define sq_row_capacity         sq_array_capacity

#define sq_row_cols_capacity(row)    \
		sq_array_capacity(&(row)->cols)

// --- SqRow functions ---
SqRow *sq_row_new(int cols_capacity, int capacity);
void   sq_row_free(SqRow *row);

void   sq_row_init(SqRow *row, int cols_capacity, int capacity);
void   sq_row_final(SqRow *row);

// free 'name' in SqRow.cols
void   sq_row_free_cols_name(SqRow *row);

/* share SqRow.cols to other SqRow.

   Because all rows in the query result have the same column array,
   sharing the column array among these rows reduces memory usage.

   Note1: If column array have been shared, user can not add/remove elements in it.
   Note2: When SqRow gets the shared column array, it frees its own column array.
 */
bool   sq_row_share_cols(SqRow *row, SqRow *share_to);

// SqValue  *sq_row_alloc(SqRow *row, int n_element);
#define sq_row_alloc(row, n_element)    \
		((SqValue*)sq_array_alloc(row, n_element))

SqRowColumn *sq_row_alloc_column(SqRow *row, int n_element);

// --- SqTypeRow functions ---

SqTypeRow *sq_type_row_new(void);
void       sq_type_row_free(SqTypeRow *type_row);

void  sq_type_row_init(SqTypeRow *type_row);
void  sq_type_row_final(SqTypeRow *type_row);

// SqTypeRow is derived from SqTypeJoint, it can use these SqTypeJoint functions.

// void sq_type_row_add(SqTypeRow *type_row, SqTable *table, const char *table_as_name);
#define sq_type_row_add(type_row, table, table_as_name)    \
		sq_type_joint_add((SqTypeJoint*)(type_row), table, table_as_name)

// void sq_type_row_erase(SqTypeRow *type_row, SqTable *table, const char *table_as_name);
#define sq_type_row_erase(type_row, table, table_as_name)    \
		sq_type_joint_erase((SqTypeJoint*)(type_row), table, table_as_name)

// alias of sq_type_row_erase()
// void sq_type_row_remove(SqTypeRow *type_row, SqTable *table, const char *table_as_name);
#define sq_type_row_remove        sq_type_row_erase

// void sq_type_row_clear(SqTypeRow *type_row);
#define sq_type_row_clear(type_row)    \
		sq_type_joint_clear((SqTypeJoint*)(type_row))

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

typedef struct SqRowColumn  RowColumn;

/*
	TypeJointMethod
	|
	`--- TypeRowMethod

	TypeRowMethod is used by SqTypeRow and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqTypeRow members.
 */
struct TypeRowMethod : Sq::TypeJointMethod {
};

/*	RowMethod is used by SqRow and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqRow members.
 */
struct RowMethod : ArrayMethod<Sq::Value> {
	int   colsCapacity(void);

	void  freeColsName(void);
	bool  shareCols(SqRow *share_to);
	bool  shareCols(SqRow &share_to);

	Sq::RowColumn *allocColumn(int  n_element = 1);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*
	SqRow.cols.type is type of SqRow.data.xxx
	If SqRow.cols.type is SQ_TYPE_BOOL, value is SqRow.data.boolean
	If SqRow.cols.type is SQ_TYPE_STR,  value is SqRow.data.str
 */
struct SqRowColumn
{
	const char    *name;
	const SqType  *type;
	const SqEntry *entry;
};

/*	SqRow - This can parse unknown(or known) table and column

	SqArray
	|
	+--- SqRow
 */
#define SQ_ROW_MEMBERS                              \
	SQ_ARRAY_MEMBERS(SqValue,     data, length);    \
	SQ_ARRAY_MEMBERS(SqRowColumn, cols, cols_length)

#ifdef __cplusplus
struct SqRow : Sq::RowMethod                 // <-- 1. inherit C++ member function(method)
#else
struct SqRow
#endif
{
	SQ_ROW_MEMBERS;                          // <-- 2. inherit member variable
/*	// ------ SqRow members ------
	SqValue      *data;
	int           length;

	SqRowColumn  *cols;
	int           cols_length;
 */
};

/*	SqTypeRow - SqTypeRow is derived from SqTypeJoint. It support unknown (or known) table and column.
	            It can create instance of SqRow.

	SqType
	|
	`--- SqTypeJoint
	     |
	     `--- SqTypeRow
 */
#define SQ_TYPE_ROW_MEMBERS    SQ_TYPE_JOINT_MEMBERS

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

/* define TypeRowMethod functions. */

/* define RowMethod functions. */
int  RowMethod::colsCapacity(void) {
	return sq_row_cols_capacity((SqRow*)this);
}
void RowMethod::freeColsName(void) {
	sq_row_free_cols_name((SqRow*)this);
}
bool RowMethod::shareCols(SqRow *share_to) {
	return sq_row_share_cols((SqRow*)this, share_to);
}
bool RowMethod::shareCols(SqRow &share_to) {
	return sq_row_share_cols((SqRow*)this, &share_to);
}
Sq::RowColumn *RowMethod::allocColumn(int  n_element) {
	return sq_row_alloc_column((SqRow*)this, n_element);
}

/* All derived struct/class must be C++11 standard-layout. */

struct TypeRow : TypeRowMethod,                  // <-- 1. inherit C++ method
                 SqTypeRow                       // <-- 2. inherit member variable
{
	// It doesn't need this line because current struct already inherits from SqTypeRow.
//	SQ_TYPE_ROW_MEMBERS;

	TypeRow() {
		sq_type_row_init((SqTypeRow*)this);
	}
	~TypeRow() {
		sq_type_row_final((SqTypeRow*)this);
	}
};

struct Row : SqRow                               // inherit C++ method and member variable
{
	// It doesn't need this line because current struct already inherits from SqRow.
//	SQ_ROW_MEMBERS;

	// constructor
	Row(int colCapacity = 0, int capacity = 0) {
		sq_row_init((SqRow*)this, colCapacity, capacity);
	}
	// destructor
	~Row() {
		sq_row_final((SqRow*)this);
	}
	// move constructor
	Row(Row &&src) {
		this->data        = src.data;
		this->length      = src.length;
		this->cols        = src.cols;
		this->cols_length = src.cols_length;
		src.data        = NULL;
		src.length      = 0;
		src.cols        = NULL;
		src.cols_length = 0;
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_ROW_H
