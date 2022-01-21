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

#include <SqConfig.h>
#include <SqError.h>
#include <SqEntry.h>
#include <SqType.h>

#define SQ_TYPE_N_ENTRY_DEFAULT    SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

#ifdef _MSC_VER
#define strcasecmp   _stricmp
#define strncasecmp  _strnicmp
#endif

// ----------------------------------------------------------------------------
// SqEntry

SqEntry *sq_entry_new(const SqType *type_info)
{
	SqEntry *entry;

	entry = malloc(sizeof(SqEntry));
	sq_entry_init(entry, type_info);
	return entry;
}

void  sq_entry_free(SqEntry *entry)
{
	if (entry->bit_field & SQB_DYNAMIC) {
		sq_entry_final(entry);
		free(entry);
	}
}

void  sq_entry_init(SqEntry *entry, const SqType *type_info)
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

void  sq_entry_final(SqEntry *entry)
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
int  sq_entry_cmp_str__name(const char *str, SqEntry **entry)
{
	const char *name;

	name = (*entry) ? (*entry)->name : "";
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
	return strcmp(str, name);
#else
	return strcasecmp(str, name);
#endif
}

// used by sort()
int  sq_entry_cmp_name(SqEntry **entry1, SqEntry **entry2)
{
	const char *name1;
	const char *name2;

	name1 = (*entry1) ? (*entry1)->name : "";
	name2 = (*entry2) ? (*entry2)->name : "";
#ifdef SQ_CONFIG_SQL_CASE_SENSITIVE
	return strcmp(name1, name2);
#else
	return strcasecmp(name1, name2);
#endif
}

int  sq_entry_cmp_str__type_name(const char *str,  SqEntry **entry)
{
	const char *name;

	name = (*entry) ? (*entry)->type->name : "";
	return strcmp(str, name);
}

int  sq_entry_cmp_type_name(SqEntry **entry1, SqEntry **entry2)
{
	const char *name1;
	const char *name2;

	name1 = (*entry1) ? (*entry1)->type->name : "";
	name2 = (*entry2) ? (*entry2)->type->name : "";
	return strcmp(name1, name2);
}

// ----------------------------------------------------------------------------
// SqReentry functions for SqPtrArray

int  sq_reentries_remove_null(void *reentry_ptr_array, int n_old_elements)
{
	SqPtrArray *array = (SqPtrArray*)reentry_ptr_array;
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

