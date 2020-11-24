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

#include <string.h>

#include <SqConfig.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqEntry.h>

#define SQ_TYPE_N_ENTRY_DEFAULT    SQ_CONFIG_TYPE_N_ENTRY_DEFAULT

#ifdef _MSC_VER
#define strcasecmp   stricmp
#define strncasecmp  strnicmp 
#endif

SqType* sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func)
{
	SqType*  entrytype;
	SqPtrArray*  array;

	entrytype = malloc(sizeof(SqType));
	entrytype->size = 0;
	entrytype->init = NULL;
	entrytype->final = NULL;
	entrytype->parse = sq_type_object_parse;
	entrytype->write = sq_type_object_write;
	entrytype->name = NULL;
	entrytype->bit_field = SQB_TYPE_DYNAMIC;

	// if prealloc_size < SQ_TYPE_N_ENTRY_DEFAULT, apply default value for small array
	if (prealloc_size < SQ_TYPE_N_ENTRY_DEFAULT)
		prealloc_size = SQ_TYPE_N_ENTRY_DEFAULT;
	array = sq_type_get_ptr_array(entrytype);
	sq_ptr_array_init(array, SQ_TYPE_N_ENTRY_DEFAULT, entry_destroy_func);

	return entrytype;
}

void  sq_type_free(SqType* entrytype)
{
	if (entrytype->bit_field & SQB_TYPE_DYNAMIC) {
		// SqType.entry can't be freed if SqType.n_entry == -1
		if (entrytype->n_entry != -1)
			sq_ptr_array_final(&entrytype->entry);
		free(entrytype);
	}
}

SqType*  sq_type_copy_static(const SqType* type_src, SqDestroyFunc entry_free_func)
{
	SqType* type;

//	if (type_src->bit_field & SQB_TYPE_DYNAMIC)
//		entry_free_func = sq_ptr_array_destroy_func(sq_type_get_ptr_array(type_src));
	type = malloc(sizeof(SqType));
	memcpy(type, type_src, sizeof(SqType));
	type->bit_field |= SQB_TYPE_DYNAMIC;
	// alloc & copy SqEntry pointer array
	sq_ptr_array_init(sq_type_get_ptr_array(type), type_src->n_entry, entry_free_func);
	type->n_entry = type_src->n_entry;
	if (type_src->n_entry > 0)
		memcpy(type->entry, type_src->entry, sizeof(void*) * type_src->n_entry);
	// copy name string
	if (type_src->name)
		type->name = strdup(type_src->name);
	return type;
}

void* sq_type_init_instance(const SqType* entrytype, void* instance, int is_pointer)
{
	SqTypeFunc  init = entrytype->init;
	SqPtrArray* array;

	// This instance pointer to pointer
	if (is_pointer) {
		if (entrytype->size > 0)
			*(void**)instance = calloc(1, entrytype->size);
		instance = *(void**)instance;
	}

	// call init() if it exist
	if (init)
		init(instance, entrytype);
	// initialize SqEntry in SqType.entry if no init() function
	else if (entrytype->entry) {
		array = sq_type_get_ptr_array(entrytype);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqEntry* entry = *element_addr;
			entrytype = entry->type;
			if (SQ_TYPE_NOT_BUILTIN(entrytype)) {
				sq_type_init_instance(entrytype, 
						instance + entry->offset,
						entry->bit_field & SQB_POINTER);
			}
		}
	}
	return instance;
}

void  sq_type_final_instance(const SqType* entrytype, void* instance, int is_pointer)
{
	SqTypeFunc  final = entrytype->final;
	SqPtrArray* array;

	// This instance pointer to pointer
	if (is_pointer) {
		instance = *(void**)instance;
		if (instance == NULL)
			return;
	}

	// call final() if it exist
	if (final)
		final(instance, entrytype);
	// finalize SqEntry in SqType.entry if no final() function
	else if (entrytype->entry) {
		array = sq_type_get_ptr_array(entrytype);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqEntry* entry = *element_addr;
			entrytype = entry->type;
			if (SQ_TYPE_NOT_ARITHMETIC(entrytype)) {
				sq_type_final_instance(entrytype,
						instance + entry->offset,
						entry->bit_field & SQB_POINTER);
			}
		}
	}

	// free memory if this instance is C pointer
	if (is_pointer)
		free(instance);
}

void  sq_type_insert_entry(SqType* entrytype, const SqEntry *entry)
{
	if (entrytype->bit_field & SQB_DYNAMIC) {
		if (entry == NULL)
			return;
		entrytype->bit_field &= ~SQB_TYPE_SORTED;
		sq_ptr_array_append(&entrytype->entry, (void*)entry);
		sq_type_decide_size(entrytype, entry);
	}
}

void** sq_type_find_entry(const SqType* entrytype, const void* key, SqCompareFunc cmp_func)
{
	SqPtrArray* array = (SqPtrArray*) &entrytype->entry;

	if (entrytype->n_entry == 0)
		return NULL;
	if (cmp_func == NULL)
		cmp_func = (SqCompareFunc)sq_entry_cmp_str__name;

	if (entrytype->bit_field & SQB_TYPE_SORTED && cmp_func == (SqCompareFunc)sq_entry_cmp_str__name)
		return sq_ptr_array_search(array, key, cmp_func);
	else
		return sq_ptr_array_find(array, key, cmp_func);
}

void  sq_type_sort_entry(SqType *type)
{
	SqPtrArray* array = (SqPtrArray*)&type->entry;

	if ( type->bit_field & SQB_TYPE_DYNAMIC &&
	    (type->bit_field & SQB_TYPE_SORTED) == 0 )
	{
		type->bit_field |= SQB_TYPE_SORTED;
		sq_ptr_array_sort(array, sq_entry_cmp_name);
	}
}

int   sq_type_decide_size(SqType* entrytype, const SqEntry *inner_entry)
{
	SqPtrArray* array;
	int   size;

	if (entrytype->bit_field & SQB_TYPE_DYNAMIC) {
		if (inner_entry) {
			// calculate new one entry 
			if (inner_entry->type == NULL)
				return entrytype->size;
			else if (inner_entry->bit_field & SQB_POINTER)
				size = sizeof(void*);
			else
				size = inner_entry->type->size;
			size += inner_entry->offset;
			if (entrytype->size < size)
				entrytype->size = size;
		}
		else {
			// recalculate size
			entrytype->size = 0;
			if (entrytype->entry == NULL)
				return 0;
			array = sq_type_get_ptr_array(entrytype);
			sq_ptr_array_foreach_addr(array, element_addr) {
				SqEntry* inner = *element_addr;
				if (inner->type == NULL)
					continue;
				else if (inner->bit_field & SQB_POINTER)
					size = sizeof(void*);
				else
					size = inner->type->size;
				size += inner->offset;
				if (entrytype->size < size)
					entrytype->size = size;
			}
		}
	}
	return entrytype->size;
}

// ----------------------------------------------------------------------------
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqArray.h
#else

void  sq_type_erase_entry_addr(SqType* type, void** element_addr, int count)
{
	if (type->bit_field & SQB_TYPE_DYNAMIC)
		sq_ptr_array_erase(&type->entry, (SqEntry**)element_addr - type->entry, count);
}

void  sq_type_steal_entry_addr(SqType* type, void** element_addr, int count)
{
	void* array = &type->entry;

	if (type->bit_field & SQB_TYPE_DYNAMIC)
		SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count);
}

#endif  // __STDC_VERSION__
