/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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
	SqEntry: define field in structure/class.
	SqColumn: define SQL column (field) in SQL table (structure/class).

	SqEntry
	|
	`--- SqReentry
	     |
	     +--- SqTable
	     |
	     `--- SqColumn
 */

//typedef struct SqEntry          SqEntry;
typedef struct SqReentry        SqReentry;

// SqEntry::bit_field
#define SQB_DYNAMIC        (1 << 0)   // C: entry can be changed and freed
#define SQB_POINTER        (1 << 1)   // C: entry's instance is pointer
#define SQB_RESERVE_0      (1 << 2)   // reserve
#define SQB_HIDDEN         (1 << 3)   // JSON: hidden entry
#define SQB_HIDDEN_NULL    (1 << 4)   // JSON: hidden entry if value is NULL
#define SQB_RESERVE_1      (1 << 5)   // reserve
#define SQB_ENTRY_END      SQB_RESERVE_1

// SqColumn::bit_field
#define SQB_PRIMARY        (1 << 6)   // SQL: PRIMARY KEY
#define SQB_FOREIGN        (1 << 7)   // SQL: FOREIGN KEY
#define SQB_UNIQUE         (1 << 8)   // SQL: UNIQUE
#define SQB_INCREMENT      (1 << 9)   // SQL: AUTOINCREMENT == SQB_AUTOINCREMENT
#define SQB_AUTOINCREMENT  (1 << 9)   // SQL: AUTOINCREMENT == SQB_INCREMENT
#define SQB_NULLABLE       (1 << 10)  // SQL: remove "NOT NULL"

// #define SQB_UNSIGNED       (1 << 13)  // SQL: attribute "UNSIGNED"
// #define SQB_IGNORE         (1 << 14)  // SQL: ignore this entry/column
// #define SQB_CURRENT_TIMESTAMP

#define SQB_COLUMN_ATTRIB  (SQB_PRIMARY | SQB_FOREIGN | SQB_UNIQUE)

// SQL common bit_field (for internal use only. use it when SQLite recreate)
#define SQB_RENAMED        (1 << 30)  // SQL: rename. column/table has been renamed.
// SQL common bit_field
#define SQB_CHANGED        (1 << 31)  // SQL: alter/modify. column/table has been altered.

// ----------------------------------------------------------------------------
// SQ_N_PTRS() calculate number of pointer in array. for example:
//	SqEntry *FooEntries[] = {...};
//	int  n_entry = SQ_N_PTRS(FooEntries);
#define SQ_N_PTRS(PointerArray) ( sizeof(PointerArray)/sizeof(void*) )

/* ----------------------------------------------------------------------------
	SqEntry: define object/field in structure.

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.
*/

#define SQ_ENTRY_MEMBERS        \
	const SqType *type;         \
	const char   *name;         \
	size_t        offset;       \
	unsigned int  bit_field

struct SqEntry
{
	SQ_ENTRY_MEMBERS;
/*	// ------ SqEntry members ------
	const SqType *type;        // type information of entry
	const char   *name;
	size_t        offset;
	unsigned int  bit_field;
 */
};

// create/destroy entry for JSON or XML
SqEntry *sq_entry_new(const SqType *type_info);
void     sq_entry_free(SqEntry *entry);

// sq_entry_init() and sq_entry_final() are called by derived struct
void  sq_entry_init(SqEntry *entry, const SqType *type_info);
void  sq_entry_final(SqEntry *entry);

// SqCompareFunc for sorting and finding SqEntry
int  sq_entry_cmp_str__name(const char *str,  SqEntry **entry);
int  sq_entry_cmp_name(SqEntry **entry1, SqEntry **entry2);

// SqCompareFunc for sorting and finding SqEntry by SqType::name
int  sq_entry_cmp_str__type_name(const char *str,  SqEntry **entry);
int  sq_entry_cmp_type_name(SqEntry **entry1, SqEntry **entry2);

/* ----------------------------------------------------------------------------
	SqReentry: reentry previously-defined entries.
	           add old_name in SqEntry to record changes (rename or drop).

	Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
	      C++ user can initialize static structure easily.
*/

#define SQ_REENTRY_MEMBERS      \
	const SqType *type;         \
	const char   *name;         \
	size_t        offset;       \
	unsigned int  bit_field;    \
	const char   *old_name

struct SqReentry
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	const SqType *type;        // type information of entry
	const char   *name;
	size_t        offset;
	unsigned int  bit_field;
	const char   *old_name;    // rename or drop
 */
	// if name is NULL, it will drop old_name
	// if name is NOT NULL, it will rename from old_name to name
};

// SqCompareFunc for finding unsorted array of SqReentry by name or old_name
int  sq_reentry_cmp_str__name(const char *str, SqReentry **reentry);
int  sq_reentry_cmp_str__old_name(const char *str, SqReentry **reentry);

/* --------------------------------------------------------
	SqReentries: unsorted SqReentry pointer array.
	             element can be NULL in array.
	             It used by migration, change list...etc

typedef SQ_PTR_ARRAY(SqReentry*)      SqReentries;
 */

// It remove all NULL pointer in array
// n_old_elements: number of old elements before removing NULL
// return:         number of old elements after  removing NULL
int     sq_reentries_remove_null(void *reentry_ptr_array, int n_old_elements);

// void sq_reentries_add(void *reentry_ptr_array, void *reentry);
#define sq_reentries_add		sq_ptr_array_append

// void **sq_reentries_find_name(void *reentry_ptr_array, const char *name)
#define sq_reentries_find_name(reentry_ptr_array, name)    \
		sq_ptr_array_find((void*)(reentry_ptr_array), name, (SqCompareFunc)sq_reentry_cmp_str__name)

// void **sq_reentries_find_old_name(void *reentry_ptr_array, const char *old_name)
#define sq_reentries_find_old_name(reentry_ptr_array, old_name)    \
		sq_ptr_array_find((void*)(reentry_ptr_array), old_name, (SqCompareFunc)sq_reentry_cmp_str__old_name)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqEntry          Entry;
typedef struct SqReentry        Reentry;
};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_ENTRY_H
