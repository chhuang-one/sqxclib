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

#include <SqError.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqEntry.h>
#include <SqxcValue.h>

/* ----------------------------------------------------------------------------
	SQ_TYPE_PTR_ARRAY
	User must add a SqEntry that declare type of element to SqType
 */

static void sq_type_ptr_array_init(void* array, SqType* type)
{
	sq_ptr_array_init(array, 8, NULL);
}

static void sq_type_ptr_array_final(void* array, SqType* type)
{
	SqEntry*  entry;

	// get element type information in SqType::entry and free elements
	if (type->n_entry) {
		entry = *type->entry;
		sq_ptr_array_foreach(array, element) {
			sq_type_final_instance(entry->type, element, true);
		}
	}
	// clear array
	sq_ptr_array_final(array);
}

static int  sq_type_ptr_array_parse(void* array, SqType* type, Sqxc* src)
{
	SqxcNested* nested;
	SqType*     element_type;
	SqxcValue*  dest;
	void*       element;

	dest = (SqxcValue*) src->dest;
	// get element type information
	if (type->n_entry > 0)
		element_type = type->entry[0]->type;
	else if (dest->nested_count < 2)
		element_type = dest->element;
	else
		return (src->code = SQCODE_NO_ELEMENT_TYPE);

	// Start of Array - Frist time to call this function to parse array
	nested = dest->nested;
	if (nested->data != array) {
		if (src->type != SQXC_TYPE_ARRAY) {
			dest->type = SQXC_TYPE_ARRAY;    // set required type in dest->type
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested((Sqxc*)dest);
		nested->data = array;
		nested->data2 = type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Array : sqxc_value_send_in() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(dest);
		return SQCODE_OK;
	}
	 */

	element = sq_ptr_array_alloc(array, 1);
	element = sq_type_init_instance(element_type, element, true);
	src->name = NULL;
	src->code = element_type->parse(element, element_type, src);
	return src->code;
}

static int  sq_type_ptr_array_write(void* array, SqType* type, Sqxc* src)
{
//	SqxcValue*  src;
	Sqxc*       dest;
	SqType*     element_type;
	const char* array_name = src->name;

	dest = src->dest;
	// get element type information.
	if (type->n_entry > 0)
		element_type = type->entry[0]->type;
	else if (src->nested_count < 1 && ((SqxcValue*)src)->element)
		element_type = ((SqxcValue*)src)->element;
	else
		return (src->code = SQCODE_NO_ELEMENT_TYPE);

	// Begin of SQXC_TYPE_ARRAY
	src->nested_count++;
//	sqxc_send_array_beg(src, array_name);
	src->type = SQXC_TYPE_ARRAY;
//	src->name = array_name;    // "name" was set by caller of this function
//	src->value.pointer = NULL;
	src->code = dest->send(dest, (Sqxc*)src);
	if (src->code != SQCODE_OK)
		return src->code;

	// output elements
	sq_ptr_array_foreach(array, element) {
		src->code = element_type->write(element, element_type, src);
		if (src->code != SQCODE_OK)
			return src->code;
	}

	// End of SQXC_TYPE_ARRAY
	src->nested_count--;
//	sqxc_send_array_end(src, array_name);
	src->type = SQXC_TYPE_ARRAY_END;
	src->name = array_name;
//	src->value.pointer = NULL;
	src->code = dest->send(dest, src);
	return src->code;
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
	SQ_TYPE_STRING_ARRAY
 */

static int  sq_type_string_array_parse(void* array, SqType* type, Sqxc* src)
{
	SqxcValue*  dest;
	SqxcNested* nested;
	void*       element;

	dest = (SqxcValue*) src->dest;
	// Start of Array - Frist time to call this function to parse array
	nested = dest->nested;
	if (nested->data != array) {
		if (src->type != SQXC_TYPE_ARRAY) {
			dest->type = SQXC_TYPE_ARRAY;    // set required type in dest->type
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested((Sqxc*)dest);
		nested->data = array;
		nested->data2 = type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Array : sqxc_value_send_in() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(dest);
		return SQCODE_OK;
	}
	 */

	element = sq_ptr_array_alloc(array, 1);
	src->name = NULL;    // set "name" before calling parse()
	src->code = SQ_TYPE_STRING->parse(element, SQ_TYPE_STRING, src);
	return src->code;
}

static int  sq_type_string_array_write(void* array, SqType* type, Sqxc* src)
{
//	SqxcValue*  src;
	Sqxc*       dest;
	const char* array_name = src->name;

	dest = src->dest;
	// Begin of SQXC_TYPE_ARRAY
	src->nested_count++;
//	sqxc_send_array_beg(src, array_name);
	src->type = SQXC_TYPE_ARRAY;
//	src->name = array_name;    // "name" was set by caller of this function
//	src->value.pointer = NULL;
	src->code = dest->send(dest, src);
	if (src->code != SQCODE_OK)
		return src->code;

	// output elements
	sq_ptr_array_foreach_addr(array, element) {
		src->name = NULL;      // set "name" before calling write()
		src->code = SQ_TYPE_STRING->write(element, SQ_TYPE_STRING, src);
		if (src->code != SQCODE_OK)
			return src->code;
	}

	// End of SQXC_TYPE_ARRAY
	src->nested_count--;
//	sqxc_send_array_end(src, array_name);
	src->type = SQXC_TYPE_ARRAY_END;
	src->name = array_name;
//	src->value.pointer = NULL;
	src->code = dest->send(dest, src);
	return src->code;
}

// extern
const SqType SqType_StringArray_ =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	sq_type_ptr_array_final,
	sq_type_string_array_parse,
	sq_type_string_array_write,
};

/* ----------------------------------------------------------------------------
	SQ_TYPE_INTPTR_ARRAY
 */

static int  sq_type_intptr_array_parse(void* array, SqType* type, Sqxc* src)
{
	SqxcValue*  dest;
	SqxcNested* nested;
	void*       element;

	dest = (SqxcValue*) src->dest;
	// Start of Array - Frist time to call this function to parse array
	nested = dest->nested;
	if (nested->data != array) {
		if (src->type != SQXC_TYPE_ARRAY) {
			dest->type = SQXC_TYPE_ARRAY;    // set required type in dest->type
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested((Sqxc*)dest);
		nested->data = array;
		nested->data2 = type;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Array : sqxc_value_send_in() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(dest);
		return SQCODE_OK;
	}
	 */

	element = sq_ptr_array_alloc(array, 1);
	src->name = NULL;    // set "name" before calling parse()
	src->code = SQ_TYPE_INTPTR->parse(element, SQ_TYPE_INTPTR, src);
	return src->code;
}

static int  sq_type_intptr_array_write(void* array, SqType* type, Sqxc* src)
{
//	SqxcValue*  src;
	Sqxc*       dest;
	const char* array_name = src->name;

	dest = src->dest;
	// Begin of SQXC_TYPE_ARRAY
	src->nested_count++;
//	sqxc_send_array_beg(src, array_name);
	src->type = SQXC_TYPE_ARRAY;
//	src->name = array_name;    // "name" was set by caller of this function
//	src->value.pointer = NULL;
	src->code = dest->send(dest, src);
	if (src->code != SQCODE_OK)
		return src->code;

	// output elements
	sq_ptr_array_foreach_addr(array, element) {
		src->name = NULL;      // set "name" before calling write()
		src->code = SQ_TYPE_INTPTR->write(element, SQ_TYPE_INTPTR, src);
		if (src->code != SQCODE_OK)
			return src->code;
	}

	// End of SQXC_TYPE_ARRAY
	src->nested_count--;
//	sqxc_send_array_end(src, array_name);
	src->type = SQXC_TYPE_ARRAY_END;
	src->name = array_name;
//	src->value.pointer = NULL;
	src->code = dest->send(dest, src);
	return src->code;
}

// extern
const SqType SqType_IntptrArray_ =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	sq_type_ptr_array_final,
	sq_type_intptr_array_parse,
	sq_type_intptr_array_write,
};

