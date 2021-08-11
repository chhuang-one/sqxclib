/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <stdio.h>
#include <SqError.h>
#include <Sqdb-migration.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

int   sq_entry_update(SqEntry *entry, SqEntry *entry_src, SqDestroyFunc destroy_func) {
	//         *entry,     *entry_src
	SqReentry  *reentry,   *reentry_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	int      index;
	void   **addr;
	union {
		int    index;
		void **addr;
	} temp;

	if ((entry->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		entry->type = sq_type_copy_static(entry->type, destroy_func);
	if ((entry->type->bit_field & SQB_TYPE_SORTED) == 0)
		sq_type_sort_entry((SqType*)entry->type);
	reentries = sq_type_get_ptr_array(entry->type);
	reentries_src = sq_type_get_ptr_array(entry_src->type);

	for (index = 0;  index < reentries_src->length;  index++) {
		reentry_src = (SqReentry*)reentries_src->data[index];
		if (SQ_TYPE_IS_FAKE(reentry_src->type))
			continue;
		if (reentry_src->bit_field & SQB_CHANGED) {
			// === ALTER ===
			// find reentry if reentry->name == reentry_src->name
			addr = sq_ptr_array_find_sorted(reentries, reentry_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, NULL);
			if (addr) {
				reentry = *(SqReentry**)addr;
				if (destroy_func == (SqDestroyFunc)sq_column_free) {
					// replace old entry by new one
					*addr = reentry_src;
					// calculate size
					sq_type_decide_size((SqType*)entry->type, (SqEntry*)reentry_src, false);
					// free removed column
					destroy_func(reentry);
					// steal 'reentry_src' from 'entry_src->type'.
					if (entry_src->type->bit_field & SQB_DYNAMIC)
						reentries_src->data[index] = NULL;
				}
				else
					sq_entry_update((SqEntry*)reentry, (SqEntry*)reentry_src, (SqDestroyFunc)sq_column_free);
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "SqTable: Can't alter column %s. It is not found.\n", reentry_src->name);
				else
					fprintf(stderr, "SqSchema: Can't alter table %s. It is not found.\n", reentry_src->name);
			}
#endif
		}
		else if (reentry_src->name == NULL) {
			// === DROP ===
			// erase original reentry if reentry->name == reentry_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, reentry_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr) {
				reentry = *(SqReentry**)addr;
				// remove dropped entry from array
				sq_ptr_array_steal(reentries, temp.index, 1);
				// calculate size
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					sq_type_decide_size((SqType*)entry->type, (SqEntry*)reentry, true);
				// free removed entry
				if (destroy_func)
					destroy_func(reentry);
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "SqTable: Can't drop column %s. It is not found.\n", reentry_src->old_name);
				else
					fprintf(stderr, "SqSchema: Can't drop table %s. It is not found.\n", reentry_src->old_name);
			}
#endif
		}
		else if (reentry_src->old_name) {
			// === RENAME ===
			// find existing if reentry->name == reentry_src->old_name
			addr = sq_ptr_array_find_sorted(reentries, reentry_src->old_name,
					(SqCompareFunc) sq_entry_cmp_str__name, NULL);
			if (addr) {
				reentry = *(SqReentry**)addr;
				if (destroy_func == (SqDestroyFunc)sq_column_free) {
					if ((reentry->bit_field & SQB_DYNAMIC) == 0)
						reentry = (SqReentry*)sq_column_copy_static((SqColumn*)reentry);
				}
				// get new index after renaming
				sq_ptr_array_find_sorted(reentries, reentry_src->name,
						(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
				// change current reentry's name
				free((char*)reentry->name);
				reentry->name = strdup(reentry_src->name);
				// move renamed reentry. temp.addr = new address of 'reentry'
				temp.addr = reentries->data + temp.index;
				if (temp.addr < addr)          // insert after
					memmove(temp.addr +1, temp.addr, (char*)addr - (char*)temp.addr);
				else if (temp.addr != addr)    // insert before
					memmove(addr, addr +1, (char*)(--temp.addr) - (char*)addr);
				*temp.addr = reentry;
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "SqTable: Can't rename column %s. It is not found.\n", reentry_src->old_name);
				else
					fprintf(stderr, "SqSchema: Can't rename table %s. It is not found.\n", reentry_src->old_name);
			}
#endif
		}
		else {
			// === ADD ===
			addr = sq_ptr_array_find_sorted(reentries, reentry_src->name,
					(SqCompareFunc) sq_entry_cmp_str__name, &temp.index);
			if (addr == NULL) {
				// add 'reentry_src' to entry->type.
				sq_ptr_array_insert(reentries, temp.index, reentry_src);
				// steal 'reentry_src' from 'entry_src->type'.
				if (entry_src->type->bit_field & SQB_DYNAMIC)
					reentries_src->data[index] = NULL;
				// calculate size if 'entry' is instance of SqTable
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					sq_type_decide_size((SqType*)entry->type, (SqEntry*)reentry_src, false);
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "SqTable: column %s is exist.\n", reentry_src->name);
				else
					fprintf(stderr, "SqSchema: table %s is exist.\n", reentry_src->name);
			}
#endif
		}
		// loop for reentries_src
	}

	return SQCODE_OK;
}

