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

#include <time.h>     // time_t
#include <stdio.h>    // sprintf()
#include <stdlib.h>   // realloc(), strtol()
#include <string.h>   // strdup()

#include <SqUtil.h>   // sq_time_to_string(), sq_time_from_string()
#include <SqError.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqField.h>

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define strtoll		_strtoi64
#define snprintf	_snprintf
#endif

// ------------------------------------
// SqType* SQ_TYPE_INT functions

int  sq_type_int_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(int*)instance = src->value.integer;
		break;

	case SQXC_TYPE_BOOL:
		*(int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STRING:
		*(int*)instance = strtol(src->value.string, NULL, 10);
		break;

	default:
		src->dest->type = SQXC_TYPE_INT;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

int  sq_type_int_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_int(src, src->name, *(int*)instance);
	src->type = SQXC_TYPE_INT;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.integer = *(int*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_UINT functions

int  sq_type_uint_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	switch (src->type) {
	case SQXC_TYPE_UINT:
		*(unsigned int*)instance = src->value.uinteger;
		break;

	case SQXC_TYPE_BOOL:
		*(unsigned int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STRING:
		*(unsigned int*)instance = strtoul(src->value.string, NULL, 10);
		break;

	default:
		src->dest->type = SQXC_TYPE_UINT;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

int  sq_type_uint_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_int(src, src->name, *(unsigned int*)instance);
	src->type = SQXC_TYPE_UINT;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.uinteger = *(unsigned int*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_INTPTR functions

int  sq_type_intptr_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING)
		*(intptr_t*)instance = strtol(src->value.string, NULL, 10);
	else if (src->type == SQXC_TYPE_INT)
		*(intptr_t*)instance = src->value.integer;
	return (src->code = SQCODE_OK);
}

int  sq_type_intptr_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_int(src, src->name, *(intptr_t*)instance);
	src->type = SQXC_TYPE_INT;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.integer = *(intptr_t*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_INT64 functions

int  sq_type_int64_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(int64_t*)instance = src->value.integer;
		break;

	case SQXC_TYPE_INT64:
		*(int64_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_STRING:
		*(int64_t*)instance = strtoll(src->value.string, NULL, 10);
		break;

	default:
		src->dest->type = SQXC_TYPE_INT64;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

int  sq_type_int64_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_int64(src, src->name, *(int64_t*)instance);
	src->type = SQXC_TYPE_INT64;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.int64 = *(int64_t*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_UINT64 functions

int  sq_type_uint64_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	switch (src->type) {
	case SQXC_TYPE_UINT:
		*(uint64_t*)instance = src->value.integer;
		break;

	case SQXC_TYPE_UINT64:
		*(uint64_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_STRING:
		*(uint64_t*)instance = strtoull(src->value.string, NULL, 10);
		break;

	default:
		src->dest->type = SQXC_TYPE_UINT64;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

int  sq_type_uint64_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_uint64(src, src->name, *(uint64_t*)instance);
	src->type = SQXC_TYPE_UINT64;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.uint64 = *(uint64_t*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_DOUBLE functions

int  sq_type_double_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING)
		*(double*)instance = strtod(src->value.string, NULL);
	else if (src->type == SQXC_TYPE_INT)
		*(double*)instance = src->value.double_;
	return (src->code = SQCODE_OK);
}

int  sq_type_double_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_double(src, src->name, *(double*)instance);
	src->type = SQXC_TYPE_DOUBLE;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.double_ = *(double*)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_TYPE_TIME functions

int  sq_type_time_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING) {
		*(time_t*)instance = sq_time_from_string(src->value.string);
		if (*(time_t*)instance == -1)
			*(time_t*)instance = time(NULL);
	}
	else if (src->type == SQXC_TYPE_INT)
		*(time_t*)instance = src->value.int_;
	else if (src->type == SQXC_TYPE_INT64)
		*(time_t*)instance = src->value.int64;
	return (src->code = SQCODE_OK);
}

int  sq_type_time_write(void* instance, SqType* fieldtype, Sqxc* src)
{
	int   result;
	char* timestr;

	timestr = sq_time_to_string(*(time_t*)instance);
//	sqxc_send_string(src, src->name, timestr);
	src->type = SQXC_TYPE_STRING;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.string = timestr;
	result = sqxc_send(src);
	free(timestr);
	return result;
}

// ------------------------------------
// SqType* SQ_TYPE_STRING functions

void sq_type_string_final(void* instance, SqType* fieldtype)
{
	free(*(char**)instance);
}

int  sq_type_string_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING) {
		if (src->value.string)
			*(char**)instance = strdup(src->value.string);
		else
			*(char**)instance = NULL;
	}
	else {
		// TODO: convert to string
		src->dest->type = SQXC_TYPE_STRING;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

int  sq_type_string_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	sqxc_send_string(src, src->name, *(char**)instance);
	src->type = SQXC_TYPE_STRING;
//	src->name = src->name;    // "name" was set by caller of this function
	src->value.string = *(char**)instance;
	return sqxc_send(src);
}

// ------------------------------------
// SqType* SQ_FIELD_OBJECT functions

/*
static const SqType SqTypeObjectStatic =
{
	0,     // size is 0, program will not allocate memory. User must do it.
	NULL,  // run default initialize. struct must set to 0 before this.
	NULL,  // run default finalize.
	sq_type_object_parse,
	sq_type_object_write,
};
// extern
const SqType*  SQ_FIELD_OBJECT = &SqTypeObjectStatic;
 */

int  sq_type_object_parse(void* instance, SqType* fieldtype, Sqxc* src)
{
	Sqxc*       dest;      // SqxcValue* dest;
	SqField*    field;
	SqxcNested* nested;

	// Start of Object - Frist time to call this function to parse object
	dest = src->dest;
	nested = dest->nested;
	if (nested->data != instance) {
		if (src->type != SQXC_TYPE_OBJECT) {
			dest->type = SQXC_TYPE_OBJECT;    // set required type in dest
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		nested = sqxc_push_nested(dest);
		nested->data  = instance;
		nested->data2 = fieldtype;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Object : sqxc_value_send_in() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(dest);
		return (src->code = SQCODE_OK);
	}
	 */

	// parse fields in entry
	field = sq_type_find_field(fieldtype, src->name, NULL);
	if (field == NULL)
		return (src->code = SQCODE_FIELD_NOT_FOUND);
	fieldtype = field->type;
	return fieldtype->parse((char*)instance + field->offset, fieldtype, src);
}

int  sq_type_object_write(void* instance, SqType* fieldtype, Sqxc* src)
{
//	SqxcValue*  src;
	SqType*     temp_type;
	SqPtrArray* array;
	const char* object_name = src->name;

//	sqxc_send_object_beg(src, object_name);
	src->type = SQXC_TYPE_OBJECT;
//	src->name = object_name;    // "name" was set by caller of this function
//	src->value.pointer = NULL;
	src->field = NULL;           // SqxcSqlite will use this
	src->code = sqxc_send(src);
	if (src->code != SQCODE_OK)
		return src->code;

	array = (SqPtrArray*) &fieldtype->entry;
	sq_ptr_array_foreach_addr(array, element_addr) {
		SqField* field = *element_addr;
		temp_type = field->type;
		src->name = field->name;    // set "name" before calling write()
		src->field = field;         // SqxcSqlite will use this
		temp_type->write((char*)instance + field->offset,
		                 temp_type, src);
		if (src->code != SQCODE_OK)
			return src->code;
	}

//	sqxc_send_object_end(src, object_name);
	src->type = SQXC_TYPE_OBJECT_END;
	src->name = object_name;
//	src->value.pointer = NULL;
	src->field = NULL;
	src->code = sqxc_send(src);
	return src->code;
}

// ----------------------------------------------------------------------------
// extern SqType

const SqType SqType_BuiltIn_[] = {
	// SQ_TYPE_INT
	{
		sizeof(int),
		NULL,
		NULL,
		sq_type_int_parse,
		sq_type_int_write,
	},
	// SQ_TYPE_UINT
	{
		sizeof(unsigned int),
		NULL,
		NULL,
		sq_type_uint_parse,
		sq_type_uint_write,
	},
	// SQ_TYPE_INTPTR
	{
		sizeof(intptr_t),
		NULL,
		NULL,
		sq_type_intptr_parse,
		sq_type_intptr_write,
	},
	// SQ_TYPE_INT64
	{
		sizeof(int64_t),
		NULL,
		NULL,
		sq_type_int64_parse,
		sq_type_int64_write,
	},
	// SQ_TYPE_UINT64
	{
		sizeof(uint64_t),
		NULL,
		NULL,
		sq_type_uint64_parse,
		sq_type_uint64_write,
	},
	// SQ_TYPE_DOUBLE
	{
		sizeof(double),
		NULL,
		NULL,
		sq_type_double_parse,
		sq_type_double_write,
	},
	// SQ_TYPE_TIME
	{
		sizeof(time_t),
		NULL,
		NULL,
		sq_type_time_parse,
		sq_type_time_write,
	},
	// SQ_TYPE_STRING
	{
		sizeof(char*),
		NULL,
		sq_type_string_final,
		sq_type_string_parse,
		sq_type_string_write,
	},
};
