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

#include <SqConfig.h>
#include <SqError.h>
#include <SqEntry.h>

#define SQ_TYPE_N_ENTRY_DEFAULT    SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

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

	if (type_info) {
		entry->type = (SqType*)type_info;
		sq_type_ref((SqType*)type_info);
	}
	else {
		// use dynamic type if type_info == NULL
		entry->type = sq_type_new(SQ_TYPE_N_ENTRY_DEFAULT,
		                          (SqDestroyFunc)sq_entry_free);
	}
}

void  sq_entry_final(SqEntry* entry)
{
	if (entry->bit_field & SQB_DYNAMIC) {
		if (entry->type)
			sq_type_unref((SqType*)entry->type);
		free((char*)entry->name);
	}
}

// ------------------------------------
// SqEntry SqCompareFunc

// used by find()
int  sq_entry_cmp_str__name(const char* str, SqEntry** entry)
{
	const char* name;

	name = (*entry) ? (*entry)->name : "";
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
	return strcmp(str, name);
#else
	return strcasecmp(str, name);
#endif
}

// used by sort()
int  sq_entry_cmp_name(SqEntry** entry1, SqEntry** entry2)
{
	const char* name1;
	const char* name2;

	name1 = (*entry1) ? (*entry1)->name : "";
	name2 = (*entry2) ? (*entry2)->name : "";
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
	return strcmp(name1, name2);
#else
	return strcasecmp(name1, name2);
#endif
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

void  sq_reentries_clear_records(void* reentry_ptr_array, char version_comparison, int n_old_elements)
{
	SqDestroyFunc destroy;
	SqReentry*    reentry;
	bool          is_renamed;

	destroy = sq_ptr_array_destroy_func(reentry_ptr_array);
	for (int index = n_old_elements;  index < ((SqPtrArray*)reentry_ptr_array)->length; index++) {
		reentry = ((SqPtrArray*)reentry_ptr_array)->data[index];
		if (reentry == NULL || reentry->old_name == NULL)
			continue;
		is_renamed = (reentry->bit_field & SQB_RENAMED) ? true : false;
		// clear altered and renamed status in column/table
		if (version_comparison != '<' && reentry->bit_field & SQB_DYNAMIC) {
			free((char*)reentry->old_name);
			reentry->old_name = NULL;
			reentry->bit_field &= ~(SQB_CHANGED | SQB_RENAMED);
		}
		// Don't destroy renamed column/table
		if (is_renamed)
			continue;
		// destory dropped & renamed records
		if (destroy)
			destroy(reentry);
		// For performance reasons, It set freed pointer to NULL first and remove all NULL pointer later.
		((SqPtrArray*)reentry_ptr_array)->data[index] = NULL;
	}
}

int  sq_reentries_remove_null(void* reentry_ptr_array, int n_old_elements)
{
	SqPtrArray* array = (SqPtrArray*)reentry_ptr_array;
	int  index_src, index_dest;

	// find first NULL pointer
	for (index_dest = 0;  index_dest < array->length;  index_dest++) {
		if (array->data[index_dest] == NULL)
			break;
	}
	// move non-NULL pointer to overwrite NULL pointer
	n_old_elements -= 1;
	for (index_src = index_dest +1;  index_src < array->length;  index_src++) {
		if (n_old_elements == index_src)
			n_old_elements = index_dest;
		if (array->data[index_src])
			array->data[index_dest++] = array->data[index_src];
	}

	array->length = index_dest;
	return n_old_elements +1;
}

// ------------------------------------
// SqReentry SqCompareFunc

int  sq_reentry_cmp_str__name(const char* str, SqReentry** reentry_addr)
{
	SqReentry* reentry = *reentry_addr;

	if (reentry) {
		if (reentry->old_name == NULL || reentry->bit_field & SQB_RENAMED)
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
			return strcmp(str, reentry->name);
#else
			return strcasecmp(str, reentry->name);
#endif
	}
	return -1;
}

int  sq_reentry_cmp_str__old_name(const char* str, SqReentry** reentry_addr)
{
	SqReentry* reentry = *reentry_addr;

	if (reentry) {
		if (reentry->old_name == NULL || reentry->bit_field & SQB_RENAMED)
			return -1;
	}
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
	return strcmp(str, reentry->old_name);
#else
	return strcasecmp(str, reentry->old_name);
#endif
}
