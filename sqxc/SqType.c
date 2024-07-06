/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

SqType *sq_type_new(unsigned int prealloc_size, SqDestroyFunc entry_destroy_func)
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

SqType  *sq_type_copy(SqType *type_dest, const SqType *type_src,
                      SqDestroyFunc entry_free_func,
                      SqCopyFunc    entry_copy_func)
{
//	if (type_src->bit_field & SQB_TYPE_DYNAMIC)
//		entry_free_func = sq_ptr_array_clear_func(sq_type_entry_array(type_src));
	if (type_dest == NULL)
		type_dest = malloc(sizeof(SqType));
	memcpy(type_dest, type_src, sizeof(SqType));
	type_dest->bit_field |= SQB_TYPE_DYNAMIC;
	// copy name string
	if (type_src->name)
		type_dest->name = strdup(type_src->name);
	// copy SqEntry array if array in 'type_src' is initialized.
	if (type_src->entry != NULL && type_src->n_entry != -1) {
		// initialize SqEntry array in 'type_dest'
		sq_ptr_array_init(sq_type_entry_array(type_dest), type_src->n_entry, entry_free_func);
		type_dest->n_entry = type_src->n_entry;
		if (entry_copy_func == NULL) {
			// copy pointers from SqEntry array directly
			memcpy(type_dest->entry, type_src->entry, sizeof(void*) * type_src->n_entry);
		}
		else {
			// copy instances from SqEntry array if 'entry_copy_func' is present
			for (unsigned int i = 0;  i < type_src->n_entry;  i++)
				type_dest->entry[i] = entry_copy_func(NULL, type_src->entry[i]);
		}
	}
	// return 'type_dest' or newly created SqType
	return type_dest;
}

void  sq_type_init_self(SqType *type, unsigned int prealloc_size, SqDestroyFunc entry_destroy_func)
{
	type->size  = 0;
	type->init  = NULL;
	type->final = NULL;
	type->parse = sq_type_object_parse;
	type->write = sq_type_object_write;
	type->name  = NULL;
	type->bit_field  = SQB_TYPE_DYNAMIC;
	type->on_destroy = NULL;

	if (prealloc_size == -1) {
		// SqType::entry isn't freed if SqType::n_entry == -1
		type->n_entry = -1;
		type->entry = NULL;
	}
	else {
		// if prealloc_size == 0, apply default value for small array
		if (prealloc_size == 0)
			prealloc_size = SQ_TYPE_N_ENTRY_DEFAULT;
		sq_ptr_array_init(sq_type_entry_array(type), SQ_TYPE_N_ENTRY_DEFAULT, entry_destroy_func);
	}
}

void  sq_type_final_self(SqType *type)
{
	if (type->on_destroy)
		type->on_destroy(type);

	free((char*)type->name);
	// SqType::entry isn't freed if SqType::n_entry == -1
	if (type->n_entry != -1)
		sq_ptr_array_final(sq_type_entry_array(type));
}

void *sq_type_init_instance(const SqType *type, void *instance, int is_pointer)
{
	SqTypeFunc  init = type->init;
	union {
		SqPtrArray *array;
		void      **cur;
	} temp;

	// This instance pointer to pointer
	if (is_pointer) {
		if (type->size > 0)
			*(void**)instance = calloc(1, type->size);
		instance = *(void**)instance;
	}

	// call SqType::init() to initialize instance if the function exists
	if (init)
		init(instance, type);
	// initialize fields in instance if there is no SqType::init() function
	else if (type->entry) {
		// initialize fields by using entry array
		temp.array = sq_type_entry_array(type);
		void **beg = sq_ptr_array_begin(temp.array);
		void **end = sq_ptr_array_end(temp.array);
		for (temp.cur = beg;  temp.cur < end;  temp.cur++) {
			// get field entry from array
			SqEntry *entry = *temp.cur;
			// get field data type from SqEntry::type
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
	union {
		SqPtrArray *array;
		void      **cur;
	} temp;

	// This instance pointer to pointer
	if (is_pointer) {
		instance = *(void**)instance;
		if (instance == NULL)
			return;
	}

	// call SqType::final() to finalize instance if the function exists
	if (final)
		final(instance, type);
	// finalize fields in instance if there is no SqType::final() function
	else if (type->entry) {
		// finalize fields by using entry array
		temp.array = sq_type_entry_array(type);
		void **beg = sq_ptr_array_begin(temp.array);
		void **end = sq_ptr_array_end(temp.array);
		for (temp.cur = beg;  temp.cur < end;  temp.cur++) {
			// get field entry from array
			SqEntry *entry = *temp.cur;
			// get field data type from SqEntry::type
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
	// SqType::entry isn't freed if SqType::n_entry == -1
	if (type->bit_field & SQB_TYPE_DYNAMIC && type->n_entry > 0) {
		sq_ptr_array_erase(sq_type_entry_array(type), 0, type->n_entry);
		type->size = 0;
	}
}

// if 'sizeof_entry' == size of pointer, type of 'entry' is 'const SqEntry**'.
// sq_type_add_entry() can NOT sort SqType::entry before doing migration because it needs to keep migration order.
void  sq_type_add_entry(SqType *type, const SqEntry *entry, unsigned int n_entry, size_t sizeof_entry)
{
	SqPtrArray  *array;
	SqEntry    **entry_addr;

	if ((type->bit_field & SQB_TYPE_DYNAMIC) == 0)
		return;
	if (sizeof_entry == 0)
		sizeof_entry = sizeof(SqEntry);

	type->bit_field &= ~SQB_TYPE_SORTED;
	array = sq_type_entry_array(type);
	entry_addr = (SqEntry**)sq_ptr_array_alloc(array, n_entry);

	for (;  n_entry;  n_entry--, entry_addr++) {
		// if 'sizeof_entry' == size of pointer, type of 'entry' is 'const SqEntry**'.
		if (sizeof(SqEntry*) == sizeof_entry)
			*entry_addr = *(SqEntry**)entry;
		else
			*entry_addr =  (SqEntry *)entry;
#if SQ_CONFIG_QUERY_ONLY_COLUMN
		if ((*entry_addr)->bit_field & SQB_QUERY)
			type->bit_field |= SQB_TYPE_QUERY_FIRST;
#endif
		sq_type_decide_size(type, *entry_addr, false);
		entry = (SqEntry*) ((char*)entry + sizeof_entry);
	}
}

void  sq_type_add_entry_ptrs(SqType *type, const SqEntry **entry_ptrs, unsigned int n_entry_ptrs)
{
	// if 'sizeof_entry' == size of pointer, type of 'entry' is 'const SqEntry**'.
	sq_type_add_entry(type, (SqEntry*)entry_ptrs, n_entry_ptrs, sizeof(SqEntry*));
}

void  sq_type_erase_entry_addr(SqType *type, SqEntry **inner_entry_addr, unsigned int count)
{
	if ((type)->bit_field & SQB_TYPE_DYNAMIC) {
		sq_type_decide_size(type, *inner_entry_addr, true);
		sq_ptr_array_erase(sq_type_entry_array(type),
				(int)(inner_entry_addr - type->entry), count);
	}
}

void  sq_type_steal_entry_addr(SqType *type, SqEntry **inner_entry_addr, unsigned int count)
{
	if ((type)->bit_field & SQB_TYPE_DYNAMIC) {
		sq_type_decide_size(type, *inner_entry_addr, true);
		SQ_PTR_ARRAY_STEAL_ADDR(sq_type_entry_array(type), inner_entry_addr, count);
	}
}

void **sq_type_find_entry(const SqType *type, const void *key, SqCompareFunc compareFunc)
{
	SqPtrArray *array = sq_type_entry_array(type);

	if (type->n_entry == 0)
		return NULL;
	if (compareFunc == NULL)
		compareFunc = sq_entry_cmp_str__name;

	if (type->bit_field & SQB_TYPE_SORTED && compareFunc == sq_entry_cmp_str__name)
		return sq_ptr_array_search(array, key, compareFunc);
	else
		return sq_ptr_array_find(array, key, compareFunc);
}

void  sq_type_sort_entry(SqType *type)
{
	SqPtrArray *array = sq_type_entry_array(type);

	if ( type->bit_field & SQB_TYPE_DYNAMIC &&
	    (type->bit_field & SQB_TYPE_SORTED) == 0 )
	{
		type->bit_field |= SQB_TYPE_SORTED;
		sq_ptr_array_sort(array, sq_entry_cmp_name);
	}
}

unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed)
{
	unsigned int  size;
	union {
		SqPtrArray *array;
		void      **cur;
	} temp;

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
		temp.array = sq_type_entry_array(type);
		void **beg = sq_ptr_array_begin(temp.array);
		void **end = sq_ptr_array_end(temp.array);
		for (temp.cur = beg;  temp.cur < end;  temp.cur++) {
			SqEntry *inner = *temp.cur;
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

// for internal use only
void  sq_type_set_str_addr(SqType *type, char **str_addr, const char *str_src)
{
	if (type->bit_field & SQB_TYPE_DYNAMIC) {
		free(*str_addr);
		if (str_src)
			*str_addr = strdup(str_src);
		else
			*str_addr = NULL;
	}
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqType.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

void  sq_type_set_name(SqType *type, const char *name)
{
	sq_type_set_str_addr(type, (char**)&type->name, name);
}

void  sq_type_use_constant(SqType *type)
{
	type->bit_field &= ~SQB_TYPE_DYNAMIC;
}

#endif  // __STDC_VERSION__
