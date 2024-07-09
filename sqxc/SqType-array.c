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

#include <stdbool.h>

#include <SqConfig.h>
#include <SqError.h>
#include <SqPtrArray.h>
#include <SqStrArray.h>
#include <SqType.h>
#include <SqEntry.h>
#include <SqxcValue.h>

#define SQ_TYPE_ARRAY_SIZE_DEFAULT    SQ_CONFIG_TYPE_ARRAY_SIZE_DEFAULT

static void sq_type_ptr_array_init(void *array, const SqType *type);
static void sq_type_ptr_array_final(void *array, const SqType *type);

static void sq_type_array_clear_element(void *array, const SqType *element_type, bool is_pointer)
{
	unsigned int   element_size;
	uint8_t *cur, *end;

	if (element_type == NULL || SQ_TYPE_IS_ARITHMETIC(element_type))
		return;
	element_size = element_type->size;
	cur = sq_array_data(array);
	end = cur + sq_array_length(array) * element_size;

	for (;  cur < end;  cur += element_size)
		sq_type_final_instance(element_type, cur, is_pointer);
}

/* ----------------------------------------------------------------------------
	SQ_TYPE_ARRAY
	User must assign element type in SqType::entry and set SqType::n_entry to -1.
 */

static void sq_type_array_init(void *array, const SqType *type)
{
	unsigned int  element_size;
	unsigned int  capacity;

	if (type->n_entry == -1) {
		element_size = ((SqType*)type->entry)->size;
		capacity = SQ_TYPE_ARRAY_SIZE_DEFAULT;
	}
	// special case: SqArray doesn't known element_size
	else {
		element_size = sizeof(void*);
		capacity = 0;
	}

	sq_array_init(array, element_size, capacity);
}

static void sq_type_array_final(void *array, const SqType *type)
{
	// User must assign element type in SqType::entry and set SqType::n_entry to -1.
	sq_type_array_clear_element(array, (SqType*)type->entry, false);
	sq_array_final(array);
}

static int  sq_type_array_parse(void *array, const SqType *type, Sqxc *src)
{
	const SqType *element_type;
	SqxcValue    *xc_value = (SqxcValue*)src->dest;
	SqxcNested   *nested;
	void         *element;

	// get element type information
	if (type->n_entry == -1)    // SqType::entry isn't freed if SqType::n_entry == -1
		element_type = (SqType*)type->entry;
	else if (xc_value->nested_count < 2) {
		element_type = xc_value->element;
		// special case: SqArray doesn't known element_size
		if (sq_array_capacity(array) == 0) {
			sq_array_element_size(array) = element_type->size;
			sq_array_alloc(array, SQ_TYPE_ARRAY_SIZE_DEFAULT);
			sq_array_length(array) = 0;
		}
	}
	else
		return (src->code = SQCODE_NO_ELEMENT_TYPE);

	// Start of Array
	nested = xc_value->nested;
#if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	if (nested->data3 != array) {
		if (nested->data != array) {
			//  time to call this function to parse array
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data = array;
			nested->data2 = (void*)type;
			nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
		}
		if (src->type != SQXC_TYPE_ARRAY) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_ARRAY;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// SqxcNested is ready to parse array, type has been matched.
		nested->data3 = array;
		return (src->code = SQCODE_OK);
	}
#else
	if (nested->data != array) {
		// do type match
		if (src->type != SQXC_TYPE_ARRAY) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_ARRAY;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = array;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
#endif  // SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	/*
	// End of Array : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_ARRAY_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	element = sq_array_alloc(array, 1);
	element = sq_type_init_instance(element_type, element,
	        (type->final == sq_type_ptr_array_final) ? true : false);
	src->name = NULL;    // set "name" before calling parse()
	src->code = element_type->parse(element, element_type, src);
	return src->code;
}

static Sqxc *sq_type_array_write(void *array, const SqType *type, Sqxc *dest)
{
	const SqType *element_type;
	const char   *array_name = dest->name;
	uint8_t *cur, *end;

	// get element type information.
	if (type->n_entry == -1)    // SqType::entry isn't freed if SqType::n_entry == -1
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
	cur = sq_array_data(array);
	end = cur + sq_array_length(array) * element_type->size;
	for (;  cur < end;  cur += element_type->size) {
		dest->name = NULL;      // set "name" before calling write()
		if (type->final == sq_type_ptr_array_final)
			dest = element_type->write(*(void**)cur, element_type, dest);
		else
			dest = element_type->write(cur, element_type, dest);
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
const SqType sqType_Array =
{
	sizeof(SqArray),
	sq_type_array_init,
	sq_type_array_final,
	sq_type_array_parse,
	sq_type_array_write,
};

/* ----------------------------------------------------------------------------
	SQ_TYPE_INT_ARRAY
 */

// extern
const SqType sqType_IntArray =
{
	sizeof(SqArray),
	sq_type_array_init,
	(SqTypeFunc)sq_array_final,
	sq_type_array_parse,
	sq_type_array_write,
	NULL,                          // name
	(SqEntry**) SQ_TYPE_INT,       // entry   : assign element type in SqType::entry
	-1,                            // n_entry : SqType::entry isn't freed if SqType::n_entry == -1
};

/* ----------------------------------------------------------------------------
	SQ_TYPE_PTR_ARRAY
	User must assign element type in SqType::entry and set SqType::n_entry to -1.
 */

static void sq_type_ptr_array_init(void *array, const SqType *type)
{
	sq_ptr_array_init(array, SQ_TYPE_ARRAY_SIZE_DEFAULT, NULL);
}

static void sq_type_ptr_array_final(void *array, const SqType *type)
{
	// get element type information in SqType::entry and free elements
	if (sq_ptr_array_clear_func(array) == NULL)
		sq_type_array_clear_element(array, (SqType*)type->entry, true);
	// free memory that allocated by array
	sq_ptr_array_final(array);
}

// extern
const SqType sqType_PtrArray =
{
	sizeof(SqPtrArray),
	sq_type_ptr_array_init,
	sq_type_ptr_array_final,
	sq_type_array_parse,
	sq_type_array_write,
};

/* ----------------------------------------------------------------------------
	SQ_TYPE_STR_ARRAY
 */

static void sq_type_str_array_init(void *array, const SqType *type)
{
	sq_str_array_init(array, SQ_TYPE_ARRAY_SIZE_DEFAULT);
}

// extern
const SqType sqType_StrArray =
{
	sizeof(SqPtrArray),
	sq_type_str_array_init,
	(SqTypeFunc)sq_ptr_array_final,
	sq_type_array_parse,
	sq_type_array_write,
	NULL,                          // name
	(SqEntry**) SQ_TYPE_STR,       // entry   : assign element type in SqType::entry
	-1,                            // n_entry : SqType::entry isn't freed if SqType::n_entry == -1
};

