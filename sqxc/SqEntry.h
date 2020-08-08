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

#ifndef SQ_ENTRY_H
#define SQ_ENTRY_H

#include <stddef.h>    // NULL
#include <stdbool.h>   // bool, true, false

#include <SqType.h>    // typedef struct SqEntry

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	SqEntry
	|
	+--- SqTable
	|
	`--- SqColumn

	See SqType.h to know how to declare static SqEntry for SqType.
 */

// typedef struct SqEntry           SqEntry;

// SqEntry::bit_field
#define SQB_DYNAMIC        (1 << 0)   // C: entry can be changed and freed
#define SQB_POINTER        (1 << 1)   // C: entry is pointer
#define SQB_RESERVE_0      (1 << 2)   // reserve
#define SQB_HIDDEN         (1 << 3)   // JSON: hidden entry
#define SQB_HIDDEN_NULL    (1 << 4)   // JSON: hidden entry if value is NULL
#define SQB_RESERVE_1      (1 << 5)   // reserve

#define SQB_PRIMARY        (1 << 6)   // SQL: PRIMARY KEY
#define SQB_FOREIGN        (1 << 7)   // SQL: FOREIGN KEY
#define SQB_UNIQUE         (1 << 8)   // SQL: UNIQUE
#define SQB_CONSTRAINT     (1 << 9)   // SQL: CONSTRAINT
#define SQB_INDEX          (1 << 10)  // SQL: INDEX
#define SQB_INCREMENT      (1 << 11)  // SQL: AUTOINCREMENT
#define SQB_AUTOINCREMENT  (1 << 11)  // SQL: AUTOINCREMENT
#define SQB_NULLABLE       (1 << 12)  // SQL: remove "NOT NULL"
// #define SQB_UNSIGNED       (1 << 13)  // SQL: attribute "UNSIGNED"

// #define SQB_IGNORE         (1 << 30)  // SQL: ignore this entry/column

#define SQB_COLUMN_ATTRIB  (SQB_PRIMARY | SQB_FOREIGN | SQB_UNIQUE)

//#define SQB_CURRENT_TIMESTAMP
#define SQB_CHANGE         (1 << 31)  // SQL: alter/modify


#define SQ_ENTRY_MEMBERS       \
	SqType*      type;         \
	char*        name;         \
	size_t       offset;       \
	unsigned int bit_field

struct SqEntry
{
	SQ_ENTRY_MEMBERS;
/*	// ------ SqEntry members ------
	SqType*      type;     // type information of entry
	char*        name;
	size_t       offset;
	unsigned int bit_field
 */
};

// create/destroy entry for JSON or XML
SqEntry* sq_entry_new(const SqType* type_info);
void     sq_entry_free(SqEntry* entry);

// sq_entry_init() and sq_entry_final() are called by derived struct
void  sq_entry_init(SqEntry* entry, const SqType* type_info);
void  sq_entry_final(SqEntry* entry);

// SqCompareFunc for sorting and finding SqEntry
int  sq_entry_cmp_str__name(const char* str,  SqEntry** entry);
int  sq_entry_cmp_name(SqEntry** entry1, SqEntry** entry2);

// SqCompareFunc for sorting and finding SqEntry by SqType::name
int  sq_entry_cmp_str__type_name(const char* str,  SqEntry** entry);
int  sq_entry_cmp_type_name(SqEntry** entry1, SqEntry** entry2);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqEntry          Entry;
};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_ENTRY_H
