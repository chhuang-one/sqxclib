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

#include <SqError.h>
#include <SqEntry.h>

// ----------------------------------------------------------------------------
// SqEntry

SqEntry* sq_entry_new(const SqType* type_info)
{
	SqEntry* entry;

	entry = malloc(sizeof(SqEntry));
	sq_entry_init(entry, type_info);
	return entry;
}

void  sq_entry_free(SqEntry* entry)
{
	if (entry->bit_field & SQB_DYNAMIC) {
		sq_entry_final(entry);
		free(entry);
	}
}

void  sq_entry_init(SqEntry* entry, const SqType* type_info)
{
	entry->name = NULL;
	entry->offset = 0;
	entry->bit_field = SQB_DYNAMIC;

	if (type_info)
		entry->type = (SqType*)type_info;
	else {
		// use dynamic type if type_info == NULL
		entry->type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);
	}
}

void  sq_entry_final(SqEntry* entry)
{
	if (entry->bit_field & SQB_DYNAMIC) {
		if (entry->type)
			sq_type_free(entry->type);
		free(entry->name);
	}
}

// ------------------------------------
// SqEntry SqCompareFunc

// used by find()
int  sq_entry_cmp_str__name(const char* str, SqEntry** entry)
{
	const char* name;

	name = (*entry) ? (*entry)->name : "";
	return strcasecmp(str, name);
}

// used by sort()
int  sq_entry_cmp_name(SqEntry** entry1, SqEntry** entry2)
{
	const char* name1;
	const char* name2;

	name1 = (*entry1) ? (*entry1)->name : "";
	name2 = (*entry2) ? (*entry2)->name : "";
	return strcasecmp(name1, name2);
}

int  sq_entry_cmp_str__type_name(const char* str,  SqEntry** entry)
{
	const char* name;

	name = (*entry) ? (*entry)->type->name : "";
	return strcmp(str, name);
}

int  sq_entry_cmp_type_name(SqEntry** entry1, SqEntry** entry2)
{
	const char* name1;
	const char* name2;

	name1 = (*entry1) ? (*entry1)->type->name : "";
	name2 = (*entry2) ? (*entry2)->type->name : "";
	return strcmp(name1, name2);
}

// ----------------------------------------------------------------------------
// SqReentry functions for SqPtrArray

void**  sq_reentries_erase(void* reentry_ptr_array, const void* key, SqCompareFunc cmp_func)
{
	SqDestroyFunc destroy;
	void **addr;

	addr = sq_ptr_array_find(reentry_ptr_array, key, cmp_func);
	if (addr) {
		destroy = sq_ptr_array_destroy_func(reentry_ptr_array);
		destroy(*addr);
		*addr = NULL;
		return addr;
	}
	return NULL;
}

void  sq_reentries_erase_changes(void* reentry_ptr_array)
{
	SqDestroyFunc destroy;
	SqReentry*    reentry;

	destroy = sq_ptr_array_destroy_func(reentry_ptr_array);
	for (int index = 0;  index < ((SqPtrArray*)reentry_ptr_array)->length; index++) {
		reentry = ((SqPtrArray*)reentry_ptr_array)->data[index];
		if (reentry == NULL || (reentry->bit_field & SQB_DYNAMIC) == 0 )
			continue;
		if (reentry->old_name) {
			destroy(reentry);
			((SqPtrArray*)reentry_ptr_array)->data[index] = NULL;
		}
	}
}

void**  sq_reentries_replace(void* reentry_ptr_array, const void* reentry, const void* reentry_new)
{
	// find reentry in array
	sq_ptr_array_foreach_addr(reentry_ptr_array, addr) {
		if (*addr == reentry) {
			*addr  = (void*)reentry_new;
			return addr;
		}
	}
	// append reentry_new if not found
	if (reentry_new)
		sq_ptr_array_append(reentry_ptr_array, reentry_new);
	return NULL;
}

void  sq_reentries_remove_null(void* reentry_ptr_array)
{
	SqPtrArray* array = (SqPtrArray*)reentry_ptr_array;
	int  index_src, index_dest;

	// find first NULL pointer
	for (index_dest = 0;  index_dest < array->length;  index_dest++) {
		if (array->data[index_dest] == NULL)
			break;
	}
	// move non-NULL pointer to overwrite NULL pointer
	for (index_src = index_dest +1;  index_src < array->length;  index_src++) {
		if (array->data[index_src])
			array->data[index_dest++] = array->data[index_src];
	}

	array->length = index_dest;
}

const char*  sq_reentries_trace_renamed(void* reentries, const char* old_name, int index_beg)
{
	SqReentry*  reentry;
	const char* cur_name = old_name;
	bool        cur_found = false;

	for (int index = index_beg;  index < sq_ptr_array_length(reentries);  index++) {
		reentry = sq_ptr_array_at(reentries, index);
		if (reentry == NULL || reentry->old_name == NULL)
			continue;
		if (strcasecmp(reentry->old_name, cur_name) == 0) {
			if (reentry->name == NULL)
				cur_name = old_name;         // dropped, reset 'cur_name'
			else
				cur_name = reentry->name;    // renamed, set 'cur_name'
			cur_found = true;
		}
	}

	if (cur_found) {
		// if reentry was dropped, return NULL.
		if (cur_name == old_name)
			return NULL;
	}
	return cur_name;
}

// ------------------------------------
// SqReentry SqCompareFunc

int  sq_reentry_cmp_str__name(const char* str, SqReentry** reentry)
{
	if (*reentry == NULL || reentry[0]->old_name)
		return -1;
	return strcasecmp(str, (*reentry)->name);
}

int  sq_reentry_cmp_str__old_name(const char* str, SqReentry** reentry)
{
	if (*reentry == NULL || reentry[0]->old_name == NULL)
		return -1;
	return strcasecmp(str, (*reentry)->old_name);
}
