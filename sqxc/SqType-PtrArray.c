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

#include <stdbool.h>

#include <SqConfig.h>
#include <SqError.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqEntry.h>
#include <SqxcValue.h>

#define SQ_TYPE_PTR_ARRAY_SIZE_DEFAULT    SQ_CONFIG_TYPE_PTR_ARRAY_SIZE_DEFAULT

/* ----------------------------------------------------------------------------
	SQ_TYPE_PTR_ARRAY
	User must add a SqEntry that declare type of element to SqType
 */

static void sq_type_ptr_array_init(void* array, const SqType *type)
{
	sq_ptr_array_init(array, SQ_TYPE_PTR_ARRAY_SIZE_DEFAULT, NULL);
}

static void sq_type_ptr_array_final(void* array, const SqType *type)
{
	SqType*  element_type;

	// get element type information in SqType.entry and free elements
	if (sq_ptr_array_destroy_func(array) == NULL && type->entry) {
		// User must assign element type in SqType.entry and set SqType.n_entry to -1.
		element_type = (SqType*)type->entry;
		sq_ptr_array_foreach(array, element) {
			sq_type_final_instance(element_type, element, true);
		}
	}
	// free memory that allocated by array
	sq_ptr_array_final(array);
}

static int  sq_type_ptr_array_parse(void* array, const SqType *type, Sqxc* src)
{
	const SqType* element_type;
	SqxcValue*    xc_value = (SqxcValue*)src->dest;
	SqxcNested*   nested;
	void*         element;

	// get element type information
	if (type->n_entry == -1)    // SqType.entry can't be freed if SqType.n_entry == -1
		element_type = (SqType*)type->entry;
	else if (xc_value->nested_count < 2)
		element_type = xc_value->element;
	else
		return (src->code = SQCODE_NO_ELEMENT_TYPE);

	// Start of Array - Frist time to call this function to parse array
	nested = xc_value->nested;
	if (nested->data != array) {
		if (src->type != SQXC_TYPE_ARRAY) {
//			src->required_type = SQXC_TYPE_ARRAY;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = array;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Array : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	element = sq_ptr_array_alloc(array, 1);
	element = sq_type_init_instance(element_type, element, true);
	src->name = NULL;    // set "name" before calling parse()
	src->code = element_type->parse(element, element_type, src);
	return src->code;
}

static Sqxc* sq_type_ptr_array_write(void* array, const SqType *type, Sqxc* dest)
{
	const SqType* element_type;
	const char*   array_name = dest->name;

	// get element type information.
	if (type->n_entry == -1)    // SqType.entry can't be freed if SqType.n_entry == -1
		element_type = (SqType*)type->entry;
//	else if (dest->nested_count < 1)
//		element_type = ((SqxcValue*)dest)->element;
	else {
		dest->code = SQCODE_NO_ELEMENT_TYPE;
		return dest;
	}

	// Begin of SQXC_TYPE_ARRAY
	dest->type = SQXC_TYPE_ARRAY;
//	dest->name = array_name;    // "name" was set by caller of this function
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	// output elements
	sq_ptr_array_foreach(array, element) {
		dest->name = NULL;      // set "name" before calling write()
		dest = element_type->write(element, element_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	// End of SQXC_TYPE_ARRAY
	dest->type = SQXC_TYPE_ARRAY_END;
	dest->name = array_name;
	dest = sqxc_send(dest);
	return dest;
}

// extern
const SqType SqType_PtrArray_ =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	sq_type_ptr_array_final,
	sq_type_ptr_array_parse,
	sq_type_ptr_array_write,
};

/* ----------------------------------------------------------------------------
	SQ_TYPE_STRING_ARRAY and SQ_TYPE_INTPTR_ARRAY
 */

static int  sq_type_notptr_array_parse(void* array, const SqType *type, Sqxc* src)
{
	const SqType* element_type;
	SqxcValue*    xc_value = (SqxcValue*)src->dest;
	SqxcNested*   nested;
	void*         element;

	// get element type information. different from sq_type_ptr_array_parse()
	element_type = (SqType*)type->entry;    // SqPtrArray use SqType.entry to store element type

	// Start of Array - Frist time to call this function to parse array
	nested = xc_value->nested;
	if (nested->data != array) {
		if (src->type != SQXC_TYPE_ARRAY) {
//			src->required_type = SQXC_TYPE_ARRAY;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = array;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Array : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// different from sq_type_ptr_array_parse()
	element = sq_ptr_array_alloc(array, 1);
	src->name = NULL;    // set "name" before calling parse()
	src->code = element_type->parse(element, element_type, src);
	return src->code;
}

static Sqxc* sq_type_notptr_array_write(void* array, const SqType *type, Sqxc* dest)
{
	const SqType* element_type;
	const char*   array_name = dest->name;

	// get element type information. different from sq_type_ptr_array_write()
	element_type = (SqType*)type->entry;    // SqPtrArray use SqType.entry to store element type

	// Begin of SQXC_TYPE_ARRAY
	dest->type = SQXC_TYPE_ARRAY;
//	dest->name = array_name;    // "name" was set by caller of this function
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	// output elements. different from sq_type_ptr_array_write()
	sq_ptr_array_foreach_addr(array, element) {
		dest->name = NULL;      // set "name" before calling write()
		dest = element_type->write(element, element_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	// End of SQXC_TYPE_ARRAY
	dest->type = SQXC_TYPE_ARRAY_END;
	dest->name = array_name;
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	return dest;
}

// extern
const SqType SqType_StringArray_ =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	(SqTypeFunc)sq_ptr_array_final,
	sq_type_notptr_array_parse,
	sq_type_notptr_array_write,
	NULL,                          // name
	(SqEntry**) SQ_TYPE_STRING,    // entry      // SqPtrArray use SqType.entry to store element type
	-1,                            // n_entry    // SqType.entry can't be freed if SqType.n_entry == -1
};

// extern
const SqType SqType_IntptrArray_ =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	(SqTypeFunc)sq_ptr_array_final,
	sq_type_notptr_array_parse,
	sq_type_notptr_array_write,
	NULL,                          // name
	(SqEntry**) SQ_TYPE_INTPTR,    // entry      // SqPtrArray use SqType.entry to store element type
	-1,                            // n_entry    // SqType.entry can't be freed if SqType.n_entry == -1
};

