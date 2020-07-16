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

#include <SqPtrArray.h>
#include <SqType.h>
#include <SqField.h>


#ifdef _MSC_VER
#define strcasecmp   stricmp
#define strncasecmp  strnicmp 
#endif

SqType* sq_type_new(int prealloc_size, SqDestroyFunc field_destroy_func)
{
	SqType*  fieldtype;
	SqPtrArray*  array;

	fieldtype = malloc(sizeof(SqType));
	fieldtype->size = 0;
	fieldtype->init = NULL;
	fieldtype->final = NULL;
	fieldtype->parse = sq_type_object_parse;
	fieldtype->write = sq_type_object_write;
	fieldtype->name = NULL;
	fieldtype->bit_field = SQB_TYPE_DYNAMIC;

	// if prealloc_size < 8, apply default value for small array
	if (prealloc_size < 8)
		prealloc_size = 8;
	array = sq_type_get_array(fieldtype);
	sq_ptr_array_init(array, 8, field_destroy_func);

	return fieldtype;
}

void  sq_type_free(SqType* fieldtype)
{
	if (fieldtype->bit_field & SQB_TYPE_DYNAMIC) {
		sq_ptr_array_final(&fieldtype->map);
		free(fieldtype);
	}
}

SqType*  sq_type_copy_static(const SqType* type_src)
{
	SqType* type;
	SqDestroyFunc func = (SqDestroyFunc)sq_field_free;

//	if (type_src->bit_field & SQB_TYPE_DYNAMIC)
//		func = sq_ptr_array_destroy_func(sq_type_get_array(type_src));
	type = malloc(sizeof(SqType));
	*type = *type_src;
	type->bit_field |= SQB_TYPE_DYNAMIC;
	type->map = sq_ptr_array_new(type_src->map_length, func);
	if (type_src->map_length > 0)
		memcpy(type->map, type_src->map, sizeof(void*) * type_src->map_length);
	if (type_src->name)
		type->name = strdup(type_src->name);
	return type;
}

void* sq_type_init_instance(SqType* fieldtype, void* instance, int is_pointer)
{
	SqTypeFunc  init = fieldtype->init;
	SqPtrArray* array;

	// This instance pointer to pointer
	if (is_pointer) {
		if (fieldtype->size > 0)
			*(void**)instance = calloc(1, fieldtype->size);
		instance = *(void**)instance;
	}

	// call init() if it exist
	if (init)
		init(instance, fieldtype);
	// initialize SqField in SqFieldMap if no init() function
	else if (fieldtype->map) {
		array = sq_type_get_array(fieldtype);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqField* field = *element_addr;
			fieldtype = field->type;
			if (fieldtype < SQ_TYPE_BUILTIN_BEG ||
			    fieldtype > SQ_TYPE_BUILTIN_END)
				continue;
			sq_type_init_instance(fieldtype, 
					instance + field->offset,
					field->bit_field & SQB_POINTER);
		}
	}
	return instance;
}

void  sq_type_final_instance(SqType* fieldtype, void* instance, int is_pointer)
{
	SqTypeFunc  final = fieldtype->final;
	SqPtrArray* array;

	// This instance pointer to pointer
	if (is_pointer) {
		instance = *(void**)instance;
		if (instance == NULL)
			return;
	}

	// call final() if it exist
	if (final)
		final(instance, fieldtype);
	// finalize SqField in SqFieldMap if no final() function
	else if (fieldtype->map) {
		array = sq_type_get_array(fieldtype);
		sq_ptr_array_foreach_addr(array, element_addr) {
			SqField* field = *element_addr;
			fieldtype = field->type;
			if (SQ_TYPE_NOT_ARITHMETIC(fieldtype))
				continue;
			sq_type_final_instance(fieldtype,
					instance + field->offset,
					field->bit_field & SQB_POINTER);
		}
	}

	// free memory if this instance is C pointer
	if (is_pointer)
		free(instance);
}

void  sq_type_insert_field(SqType* fieldtype, const SqField* field)
{
	if (fieldtype->bit_field & SQB_DYNAMIC) {
		if (field == NULL)
			return;
		fieldtype->bit_field &= ~SQB_TYPE_SORTED;
		sq_ptr_array_append(&fieldtype->map, (void*)field);
		sq_type_decide_size(fieldtype, field);
	}
}

SqField*  sq_type_remove_field(SqType* type, const void* key, SqCompareFunc cmp_func, int do_destroy)
{
	SqPtrArray* array;
	SqField*    field;
	void**      addr;

	if ((type->bit_field & SQB_DYNAMIC) == 0)
		return NULL;
	if (cmp_func == NULL)
		cmp_func = (SqCompareFunc) sq_field_cmp_str__name;

	if (type->bit_field & SQB_TYPE_SORTED && cmp_func == (SqCompareFunc)sq_field_cmp_str__name)
		addr = sq_ptr_array_search(sq_type_get_array(type), key, cmp_func);
	else
		addr = sq_ptr_array_find(sq_type_get_array(type), key, cmp_func);

	if (addr == NULL)
		return NULL;
	else {
		field = *addr;
		array = sq_type_get_array(type);
		if (do_destroy == 1) {
			SqDestroyFunc  destroy_func;
			destroy_func = sq_ptr_array_destroy_func(array);
			if (destroy_func)
				destroy_func(field);
		}
		SQ_PTR_ARRAY_STEAL(array, addr - array->data, 1);
//		sq_type_decide_size(type, NULL);
		return field;
	}
}

SqField* sq_type_find_field(SqType* fieldtype, const void* key, SqCompareFunc cmp_func)
{
	SqPtrArray* array = (SqPtrArray*) &fieldtype->map;
	void**      addr;

	if (fieldtype->map_length == 0)
		return NULL;

	if ( cmp_func == NULL &&
	     fieldtype->bit_field & SQB_TYPE_DYNAMIC &&
	    (fieldtype->bit_field & SQB_TYPE_SORTED) == 0 )
	{
		fieldtype->bit_field |= SQB_TYPE_SORTED;
		sq_ptr_array_sort(array, sq_field_cmp_name);
	}

	if (cmp_func == NULL)
		cmp_func = (SqCompareFunc)sq_field_cmp_str__name;
	if (fieldtype->bit_field & SQB_TYPE_SORTED && cmp_func == (SqCompareFunc)sq_field_cmp_str__name)
		addr = sq_ptr_array_search(array, key, cmp_func);
	else
		addr = sq_ptr_array_find(array, key, cmp_func);

	if (addr == NULL)
		return NULL;
	else
		return *(SqField**)addr;
}

int   sq_type_decide_size(SqType* fieldtype, const SqField* inner_field)
{
	SqPtrArray* array;
	int   size;

	if (fieldtype->bit_field & SQB_TYPE_DYNAMIC) {
		if (inner_field) {
			// calculate new one field 
			if (inner_field->type == NULL)
				return fieldtype->size;
			else if (inner_field->bit_field & SQB_POINTER)
				size = sizeof(void*);
			else
				size = inner_field->type->size;
			size += inner_field->offset;
			if (fieldtype->size < size)
				fieldtype->size = size;
		}
		else {
			// recalculate size
			fieldtype->size = 0;
			if (fieldtype->map == NULL)
				return 0;
			array = sq_type_get_array(fieldtype);
			sq_ptr_array_foreach_addr(array, element_addr) {
				SqField* inner = *element_addr;
				if (inner->type == NULL)
					continue;
				else if (inner->bit_field & SQB_POINTER)
					size = sizeof(void*);
				else
					size = inner->type->size;
				size += inner->offset;
				if (fieldtype->size < size)
					fieldtype->size = size;
			}
		}
	}
	return fieldtype->size;
}
