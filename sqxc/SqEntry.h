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
	SqEntry: declare object/field or table/column in it's extended structure.

	SqEntry
	|
	`--- SqReentry
         |
	     +--- SqTable
	     |
	     `--- SqColumn

	See SqType.h to know how to declare static SqEntry for SqType.
 */

//typedef struct SqEntry          SqEntry;
typedef struct SqReentry        SqReentry;

// SqEntry::bit_field
#define SQB_DYNAMIC        (1 << 0)   // C: entry can be changed and freed
#define SQB_POINTER        (1 << 1)   // C: entry is pointer
#define SQB_RESERVE_0      (1 << 2)   // reserve
#define SQB_HIDDEN         (1 << 3)   // JSON: hidden entry
#define SQB_HIDDEN_NULL    (1 << 4)   // JSON: hidden entry if value is NULL
#define SQB_RESERVE_1      (1 << 5)   // reserve
#define SQB_ENTRY_END      SQB_RESERVE_1

// SqColumn::bit_field
#define SQB_PRIMARY        (1 << 6)   // SQL: PRIMARY KEY
#define SQB_FOREIGN        (1 << 7)   // SQL: FOREIGN KEY
#define SQB_UNIQUE         (1 << 8)   // SQL: UNIQUE
#define SQB_CONSTRAINT     (1 << 9)   // SQL: CONSTRAINT
#define SQB_INDEX          (1 << 10)  // SQL: INDEX
#define SQB_INCREMENT      (1 << 11)  // SQL: AUTOINCREMENT == SQB_AUTOINCREMENT
#define SQB_AUTOINCREMENT  (1 << 11)  // SQL: AUTOINCREMENT == SQB_INCREMENT
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
	SqType*      type;        // type information of entry
	char*        name;
	size_t       offset;
	unsigned int bit_field;
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

/* ----------------------------------------------------------------------------
	SqReentry: reentry previously-defined entries.
	           add old_name in SqEntry to record changes (rename or drop).
*/

#define SQ_REENTRY_MEMBERS     \
	SqType*      type;         \
	char*        name;         \
	size_t       offset;       \
	unsigned int bit_field;    \
	char*        old_name

struct SqReentry
{
	SQ_REENTRY_MEMBERS;
/*	// ------ SqReentry members ------
	SqType*      type;        // type information of entry
	char*        name;
	size_t       offset;
	unsigned int bit_field;
	char*        old_name;    // rename or drop
 */
	// if name is NULL, it will drop old_name
	// if name is NOT NULL, it will rename from old_name to name
};

// SqCompareFunc for finding unsorted array of SqReentry by name or old_name
int  sq_reentry_cmp_str__name(const char* str, SqReentry** reentry);
int  sq_reentry_cmp_str__old_name(const char* str, SqReentry** reentry);

/* --------------------------------------------------------
	SqReentries: unsorted SqReentry pointer array.
	             element can be NULL in array.
	             It used by migration, change list...etc

typedef SQ_PTR_ARRAY(SqReentry*)      SqReentries;
 */

// find and delete reentry, set NULL to index of deleted reentry.
// return address of deleted reentry pointer.
void**  sq_reentries_erase(void* reentry_ptr_array, const void* key, SqCompareFunc cmp_func);

// find and delete all change record (reentry->old_name != NULL), set NULL to index of deleted reentry.
// return address of deleted reentry pointer.
void    sq_reentries_erase_changes(void* reentry_ptr_array);

// find and replace 'reentry' by 'reentry_new'.
// if 'reentry' not found and 'reentry_new' != NULL, It will append 'reentry_new'.
// if 'reentry' not found and 'reentry_new' == NULL, It will NOT add 'reentry_new'.
// return address of 'reentry' pointer.
void**  sq_reentries_replace(void* reentry_ptr_array, const void* reentry, const void* reentry_new);

// remove all NULL pointer in array
void    sq_reentries_remove_null(void* reentry_ptr_array);

// void sq_reentries_add(void* reentry_ptr_array, void* reentry);
#define sq_reentries_add		sq_ptr_array_append

// void sq_reentries_erase_name(void* reentry_ptr_array, const char* old_name)
#define sq_reentries_erase_name(reentry_ptr_array, name)    \
		sq_reentries_erase((void*)(reentry_ptr_array), name, (SqCompareFunc)sq_reentry_cmp_str__name)

// void sq_reentries_erase_old_name(void* reentry_ptr_array, const char* old_name)
#define sq_reentries_erase_old_name(reentry_ptr_array, old_name)    \
		sq_reentries_erase((void*)(reentry_ptr_array), name, (SqCompareFunc)sq_reentry_cmp_str__old_name)

// void** sq_reentries_find_name(void* reentry_ptr_array, const char* name)
#define sq_reentries_find_name(reentry_ptr_array, name)    \
		sq_ptr_array_find((void*)(reentry_ptr_array), name, (SqCompareFunc)sq_reentry_cmp_str__name)

// void** sq_reentries_find_old_name(void* reentry_ptr_array, const char* old_name)
#define sq_reentries_find_old_name(reentry_ptr_array, old_name)    \
		sq_ptr_array_find((void*)(reentry_ptr_array), old_name, (SqCompareFunc)sq_reentry_cmp_str__old_name)

// trace all renamed record by 'old_name' and return the newest name.
// if reentry dropped,   return NULL.
// if reentry not found, return pointer equal 'old_name'
const char* sq_reentries_trace_renamed(void* reentry_ptr_array, const char* old_name, int index_beg);

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
typedef struct SqReentry        Reentry;
};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_ENTRY_H
