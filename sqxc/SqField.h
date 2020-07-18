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

#ifndef SQ_FIELD_H
#define SQ_FIELD_H

#include <stddef.h>    // NULL
#include <stdbool.h>   // bool, true, false

#include <SqType.h>    // typedef struct SqField

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	SqField
	|
	+--- SqTable
	|
	`--- SqColumn

	See SqType.h to know how to declare static SqField for SqType.
 */

// typedef struct SqField           SqField;

// SqField::bit_field
#define SQB_DYNAMIC        (1 << 0)   // C: field can be changed and freed
#define SQB_POINTER        (1 << 1)   // C: field is pointer
#define SQB_RESERVE_0      (1 << 2)   // reserve
#define SQB_HIDDEN         (1 << 3)   // JSON: hidden field
#define SQB_HIDDEN_NULL    (1 << 4)   // JSON: hidden field if value is NULL
#define SQB_RESERVE_1      (1 << 5)   // reserve

#define SQB_PRIMARY        (1 << 6)   // SQL: PRIMARY KEY
#define SQB_FOREIGN        (1 << 7)   // SQL: FOREIGN KEY
#define SQB_UNIQUE         (1 << 8)   // SQL: UNIQUE
#define SQB_CONSTRAINT     (1 << 9)   // SQL: CONSTRAINT
#define SQB_INDEX          (1 << 10)  // SQL: INDEX
#define SQB_INCREMENT      (1 << 11)  // SQL: AUTOINCREMENT
#define SQB_AUTOINCREMENT  (1 << 11)  // SQL: AUTOINCREMENT
#define SQB_NULLABLE       (1 << 12)  // SQL: remove "NOT NULL"
#define SQB_UNSIGNED       (1 << 13)  // SQL: attribute "UNSIGNED"

#define SQB_IGNORE         (1 << 30)  // SQL: ignore this field/column

#define SQB_COLUMN_ATTRIB  (SQB_PRIMARY | SQB_FOREIGN | SQB_UNIQUE)

//#define SQB_CURRENT_TIMESTAMP
#define SQB_CHANGE         (1 << 31)  // SQL: alter/modify


#define SQ_FIELD_MEMBERS       \
	SqType*      type;         \
	char*        name;         \
	size_t       offset;       \
	unsigned int bit_field

struct SqField
{
	SQ_FIELD_MEMBERS;
/*	// ------ SqField members ------
	SqType*      type;     // type information of field
	char*        name;
	size_t       offset;
	unsigned int bit_field
 */
};

// create/destroy field for JSON or XML
SqField* sq_field_new(const SqType* type_info);
void     sq_field_free(SqField* field);

// sq_field_init() and sq_field_final() are called by derived struct
void  sq_field_init(SqField* field, const SqType* type_info);
void  sq_field_final(SqField* field);

// SqCompareFunc for sorting and finding SqField
int  sq_field_cmp_str__name(const char* str,  SqField** field);
int  sq_field_cmp_name(SqField** field1, SqField** field2);

// SqCompareFunc for sorting and finding SqField by SqType::name
int  sq_field_cmp_str__type_name(const char* str,  SqField** field);
int  sq_field_cmp_type_name(SqField** field1, SqField** field2);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqField          Field;
};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_FIELD_H
