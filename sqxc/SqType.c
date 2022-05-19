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

#include <string.h>

#include <SqConfig.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqEntry.h>

#define SQ_TYPE_N_ENTRY_DEFAULT    SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

#ifdef _MSC_VER
#define strcasecmp   _stricmp
#define strncasecmp  _strnicmp
#define strdup       _strdup
#endif

SqType *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func)
{
	SqType  *type;

	type = malloc(sizeof(SqType));
	sq_type_init_self(type, prealloc_size, entry_destroy_func);
	return type;
}

void  sq_type_free(SqType *type)
{
	if (type->bit_field & SQB_TYPE_DYNAMIC) {
		sq_type_final_self(type);
		free(type);
	}
}

SqType  *sq_type_copy_static(SqType *type_dest, const SqType *static_type_src, SqDestroyFunc entry_free_func)
{
//	if (static_type_src->bit_field & SQB_TYPE_DYNAMIC)
//		entry_free_func = sq_ptr_array_destroy_func(sq_type_get_ptr_array(static_type_src));
	if (type_dest == NULL)
		type_dest = malloc(sizeof(SqType));
	memcpy(type_dest, static_type_src, sizeof(SqType));
	type_dest->bit_field |= SQB_TYPE_DYNAMIC;
	// alloc & copy SqEntry pointer array
	sq_ptr_array_init(sq_type_get_ptr_array(type_dest), static_type_src->n_entry, entry_free_func);
	type_dest->n_entry = static_type_src->n_entry;
	if (static_type_src->n_entry > 0)
		memcpy(type_dest->entry, static_type_src->entry, sizeof(void*) * static_type_src->n_entry);
	// copy name string
	if (static_type_src->name)
		type_dest->name = strdup(static_type_src->name);
	return type_dest;
}

void  sq_type_init_self(SqType *type, int prealloc_size, SqDestroyFunc entry_destroy_func)
{
	type->size  = 0;
	type->init  = NULL;
	type->final = NULL;
	type->parse = sq_type_object_parse;
	type->write = sq_type_object_write;
	type->name  = NULL;
	type->bit_field = SQB_TYPE_DYNAMIC;

	if (prealloc_size == -1) {
		// SqType.entry can't be freed if SqType.n_entry == -1
		type->n_entry = -1;
		type->entry = NULL;
	}
	else {
		// if prealloc_size == 0, apply default value for small array
		if (prealloc_size == 0)
			prealloc_size = SQ_TYPE_N_ENTRY_DEFAULT;
		sq_ptr_array_init(sq_type_get_ptr_array(type), SQ_TYPE_N_ENTRY_DEFAULT, entry_destroy_func);
	}
}

void  sq_type_final_self(SqType *type)
{
	free(type->name);
	// SqType.entry can't be freed if SqType.n_entry == -1
	if (type->n_entry != -1)
		sq_ptr_array_final(sq_type_get_ptr_array(type));
}

void *sq_type_init_instance(const SqType *type, void *instance, int is_pointer)
{
	SqTypeFunc  init = type->init;
	SqPtrArray *array;

	// This instance pointer to pointer
	if (is_pointer) {
		if (type->size > 0)
			*(void**)instance = calloc(1, type->size);
		instance = *(void**)instance;
	}

	// call init() if it exist
	if (init)
		init(instance, type);
	// initialize SqEntry in SqType.entry if no init() function
	else if (type->entry) {
		array = sq_type_get_ptr_array(type);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqEntry *entry = *element_addr;
			type = entry->type;
			if (SQ_TYPE_NOT_BUILTIN(type)) {
				sq_type_init_instance(type,
						(char*)instance + entry->offset,
						entry->bit_field & SQB_POINTER);
			}
		}
	}
	return instance;
}

void  sq_type_final_instance(const SqType *type, void *instance, int is_pointer)
{
	SqTypeFunc  final = type->final;
	SqPtrArray *array;

	// This instance pointer to pointer
	if (is_pointer) {
		instance = *(void**)instance;
		if (instance == NULL)
			return;
	}

	// call final() if it exist
	if (final)
		final(instance, type);
	// finalize SqEntry in SqType.entry if no final() function
	else if (type->entry) {
		array = sq_type_get_ptr_array(type);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqEntry *entry = *element_addr;
			type = entry->type;
			if (SQ_TYPE_NOT_ARITHMETIC(type)) {
				sq_type_final_instance(type,
						(char*)instance + entry->offset,
						entry->bit_field & SQB_POINTER);
			}
		}
	}

	// free memory if this instance is C pointer
	if (is_pointer)
		free(instance);
}

void  sq_type_clear_entry(SqType *type)
{
	if (type->bit_field & SQB_TYPE_DYNAMIC && type->n_entry > 0)
		sq_ptr_array_erase(sq_type_get_ptr_array(type), 0, type->n_entry);
}

void  sq_type_add_entry(SqType *type, const SqEntry *entry, int n_entry, size_t sizeof_entry)
{
	SqPtrArray  *array;
	void  **entry_addr;

	if (type->bit_field & SQB_TYPE_DYNAMIC) {
		if (sizeof_entry == 0)
			sizeof_entry = sizeof(SqEntry);
		type->bit_field &= ~SQB_TYPE_SORTED;
		array = sq_type_get_ptr_array(type);
		entry_addr = sq_ptr_array_alloc(array, n_entry);
		for (;  n_entry;  n_entry--, entry_addr++) {
			*entry_addr = (void*)entry;
			sq_type_decide_size(type, entry, false);
			entry = (SqEntry*) ((char*)entry + sizeof_entry);
		}
	}
}

void  sq_type_add_entry_ptrs(SqType *type, const SqEntry **entry_ptrs, int n_entry_ptrs)
{
	SqPtrArray *array;

	if (type->bit_field & SQB_TYPE_DYNAMIC) {
		type->bit_field &= ~SQB_TYPE_SORTED;
		array = sq_type_get_ptr_array(type);
		SQ_PTR_ARRAY_APPEND_N(array, entry_ptrs, n_entry_ptrs);
		for (int index = 0;  index < n_entry_ptrs;  index++, entry_ptrs++)
			sq_type_decide_size(type, *entry_ptrs, false);
	}
}

void **sq_type_find_entry(const SqType *type, const void *key, SqCompareFunc cmp_func)
{
	SqPtrArray *array = (SqPtrArray*) &type->entry;

	if (type->n_entry == 0)
		return NULL;
	if (cmp_func == NULL)
		cmp_func = (SqCompareFunc)sq_entry_cmp_str__name;

	if (type->bit_field & SQB_TYPE_SORTED && cmp_func == (SqCompareFunc)sq_entry_cmp_str__name)
		return sq_ptr_array_search(array, key, cmp_func);
	else
		return sq_ptr_array_find(array, key, cmp_func);
}

void  sq_type_sort_entry(SqType *type)
{
	SqPtrArray *array = (SqPtrArray*)&type->entry;

	if ( type->bit_field & SQB_TYPE_DYNAMIC &&
	    (type->bit_field & SQB_TYPE_SORTED) == 0 )
	{
		type->bit_field |= SQB_TYPE_SORTED;
		sq_ptr_array_sort(array, sq_entry_cmp_name);
	}
}

unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed)
{
	SqPtrArray   *array;
	unsigned int  size;

	if (type->bit_field & SQB_TYPE_DYNAMIC) {
		if (inner_entry) {
			// calculate new one entry 
			if (SQ_TYPE_IS_FAKE(inner_entry->type) || inner_entry->type == NULL)
				return type->size;
			else if (inner_entry->bit_field & SQB_POINTER)
				size = sizeof(void*);
			else
				size = inner_entry->type->size;
			size += (unsigned int)inner_entry->offset;
			// removing or adding entry
			if (entry_removed) {
				if (type->size == size) {
					type->size = (unsigned int)inner_entry->offset;
					return type->size;
				}
			}
			else {
				if (type->size < size)
					type->size = size;
				return type->size;
			}
		}

		// recalculate size
		type->size = 0;
		if (type->entry == NULL)
			return 0;
		array = sq_type_get_ptr_array(type);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqEntry *inner = *element_addr;
			if (inner->type == NULL)
				continue;
			else if (inner->bit_field & SQB_POINTER)
				size = sizeof(void*);
			else
				size = inner->type->size;
			size += (unsigned int)inner->offset;
			if (type->size < size)
				type->size = size;
		}
	}
	return type->size;
}

// ----------------------------------------------------------------------------
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqType.h

#else   // __STDC_VERSION__
// declare functions here if compiler does NOT support inline function.

void  sq_type_erase_entry_addr(SqType *type, void **element_addr, int count)
{
	SQ_TYPE_ERASE_ENTRY_ADDR(type, element_addr, count);
}

void  sq_type_steal_entry_addr(SqType *type, void **element_addr, int count)
{
	SQ_TYPE_STEAL_ENTRY_ADDR(type, element_addr, count);
}

#endif  // __STDC_VERSION__
