/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

#include <stdio.h>        // fprintf(), stderr
#include <SqError.h>
#include <SqType.h>
#include <Sqdb-migration.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

static int  sq_entry_update(SqEntry *entry, SqEntry *entry_src, SqDestroyFunc destroy_func);
static void sq_type_sort_recursive(SqType *type, int level);


int  sq_schema_update(SqSchema *schema, SqSchema *schema_src)
{
	return sq_entry_update((SqEntry*)schema, (SqEntry*)schema_src, (SqDestroyFunc)sq_table_free);
}

void  sq_schema_sort_table_column(SqSchema *schema)
{
	sq_type_sort_recursive((SqType*)schema->type, 2);
}

// ----------------------------------------------------------------------------
// static function

static int  sq_entry_update(SqEntry *entry, SqEntry *entry_src, SqDestroyFunc destroy_func)
{
	//         *entry,     *entry_src
	SqReentry  *reentry,   *reentry_src;
	SqPtrArray *reentries, *reentries_src;
	// other variable
	void   **src_cur, **src_end;    // address in reentries_src
	void   **addr;
	union {
		unsigned int  index;
		void **addr;
	} temp;

	if ((entry->type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		entry->type = sq_type_copy(NULL, entry->type, destroy_func, NULL);
	if ((entry->type->bit_field & SQB_TYPE_SORTED) == 0)
		sq_type_sort_entry((SqType*)entry->type);
	reentries = sq_type_entry_array(entry->type);
	reentries_src = sq_type_entry_array(entry_src->type);

	src_cur = reentries_src->data;
	src_end = src_cur + reentries_src->length;
	for (;  src_cur < src_end;  src_cur++) {
		reentry_src = *src_cur;
		if (SQ_TYPE_IS_FAKE(reentry_src->type))
			continue;
		if (reentry_src->bit_field & SQB_CHANGED) {
			// === ALTER ===
			// find reentry if reentry->name == reentry_src->name
			addr = sq_ptr_array_find_sorted(reentries,
					reentry_src->name, sq_entry_cmp_str__name, NULL);
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
						*src_cur = NULL;
				}
				else
					sq_entry_update((SqEntry*)reentry, (SqEntry*)reentry_src, (SqDestroyFunc)sq_column_free);
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "%s: Can't alter column '%s' in table '%s'.\n",
					        "sq_entry_update()", reentry_src->name, entry->name);
				else
					fprintf(stderr, "%s: Can't alter table '%s' in schema '%s'.\n",
					        "sq_entry_update()", reentry_src->name, entry->name);
			}
#endif
		}
		else if (reentry_src->name == NULL) {
			// === DROP ===
			// erase original reentry if reentry->name == reentry_src->old_name
			addr = sq_ptr_array_find_sorted(reentries,
					reentry_src->old_name, sq_entry_cmp_str__name, &temp.index);
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
					fprintf(stderr, "%s: Can't drop column '%s' in table '%s'.\n",
					        "sq_entry_update()", reentry_src->old_name, entry->name);
				else
					fprintf(stderr, "%s: Can't drop table '%s' in schema '%s'.\n",
					        "sq_entry_update()", reentry_src->old_name, entry->name);
			}
#endif
		}
		else if (reentry_src->old_name) {
			// === RENAME ===
			// find existing if reentry->name == reentry_src->old_name
			addr = sq_ptr_array_find_sorted(reentries,
					reentry_src->old_name, sq_entry_cmp_str__name, NULL);
			if (addr) {
				reentry = *(SqReentry**)addr;
				if (destroy_func == (SqDestroyFunc)sq_column_free) {
					if ((reentry->bit_field & SQB_DYNAMIC) == 0)
						reentry = (SqReentry*)sq_column_copy(NULL, (SqColumn*)reentry);
				}
				// get new index after renaming
				sq_ptr_array_find_sorted(reentries,
						reentry_src->name, sq_entry_cmp_str__name, &temp.index);
				// change current reentry's name
				free((char*)reentry->name);
				reentry->name = strdup(reentry_src->name);
				// move renamed reentry. temp.addr = new address of 'reentry'
				temp.addr = reentries->data + temp.index;
				if (temp.addr < addr)          // insert after
					memmove(temp.addr +1, temp.addr, (char*)addr - (char*)temp.addr);
				else if (temp.addr != addr)    // insert before
					memmove(addr,         addr +1,   (char*)(--temp.addr) - (char*)addr);
				*temp.addr = reentry;
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "%s: Can't rename column '%s' in table '%s'.\n",
					        "sq_entry_update()", reentry_src->old_name, entry->name);
				else
					fprintf(stderr, "%s: Can't rename table '%s' in schema '%s'.\n",
					        "sq_entry_update()", reentry_src->old_name, entry->name);
			}
#endif
		}
		else {
			// === ADD ===
			addr = sq_ptr_array_find_sorted(reentries,
					reentry_src->name, sq_entry_cmp_str__name, &temp.index);
			if (addr == NULL) {
				// add 'reentry_src' to entry->type.
				sq_ptr_array_push_in(reentries, temp.index, reentry_src);
				// steal 'reentry_src' from 'entry_src->type'.
				if (entry_src->type->bit_field & SQB_DYNAMIC)
					*src_cur = NULL;
				// calculate size if 'entry' is instance of SqTable
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					sq_type_decide_size((SqType*)entry->type, (SqEntry*)reentry_src, false);
			}
#ifndef NDEBUG
			else {
				if (destroy_func == (SqDestroyFunc)sq_column_free)
					fprintf(stderr, "%s: Can't add column '%s' to table '%s'.\n",
					        "sq_entry_update()", reentry_src->name, entry->name);
				else
					fprintf(stderr, "%s: Can't add table '%s' to schema '%s'.\n",
					        "sq_entry_update()", reentry_src->name, entry->name);
			}
#endif
		}
		// loop for reentries_src
	}

	return SQCODE_OK;
}

static void sq_type_sort_recursive(SqType *type, int level)
{
	SqEntry **cur, **end;

	if (type->n_entry > 0 && type->bit_field & SQB_TYPE_DYNAMIC) {
		sq_type_sort_entry(type);
		if (level > 1) {
			cur = type->entry;
			end = cur + type->n_entry;
			for (;  cur < end;  cur++)
				sq_type_sort_recursive((SqType*)cur[0]->type, level-1);
		}
	}
}

#if SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB

void sq_schema_erase_fake_type(SqSchema *schema)
{
	SqType   *table_type;
	SqColumn *column;
	unsigned int  n_tables = schema->type->n_entry;
	unsigned int  n_nulls = 0;

	for (unsigned int i = 0;  i < n_tables;  i++) {
		table_type = (SqType*)schema->type->entry[i]->type;
		if ((table_type->bit_field & SQB_TYPE_DYNAMIC) == 0)
			continue;
		for (unsigned int j = 0;  j < table_type->n_entry;  j++) {
			column = (SqColumn*)table_type->entry[j];
			if (SQ_TYPE_IS_FAKE(column->type)) {
				table_type->entry[j] = NULL;
				n_nulls++;
			}
		}
		if (n_nulls > 0) {
			n_nulls = 0;
			sq_reentries_remove_null(sq_type_entry_array(table_type), 0);
		}
	}
}

#endif  // SQ_CONFIG_ERASE_FAKE_TYPE_WHEN_SYNC_DB
