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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <time.h>     // time_t
#include <stdio.h>    // sprintf()
#include <stdlib.h>   // realloc(), strtol()
#include <string.h>   // strdup()

#include <SqUtil.h>   // sq_time_to_string(), sq_time_from_string()
#include <SqError.h>
#include <SqPtrArray.h>
#include <SqType.h>
#include <SqEntry.h>
#include <SqxcValue.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#define strtoll      _strtoi64
#define strdup       _strdup
#endif

// ------------------------------------
// SqType *SQ_TYPE_BOOL functions

int  sq_type_bool_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	char  ch;

	switch (src->type) {
	case SQXC_TYPE_BOOL:
		*(bool*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_INT:
		*(bool*)instance = (src->value.integer) ? true : false;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string) {
			ch = src->value.string[0]; 
			if (ch == '0' || ch == 'f' || ch == 'F')    // '0', 'false', or 'FALSE'
				*(bool*)instance = false;
			else
				*(bool*)instance = true;
		}
		else
			*(bool*)instance = false;
		break;

	default:
//		src->required_type = SQXC_TYPE_BOOL;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_bool_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_BOOL;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.boolean = *(bool*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_INT functions

int  sq_type_int_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(int*)instance = src->value.integer;
		break;

	case SQXC_TYPE_BOOL:
		*(int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(int*)instance = strtol(src->value.string, NULL, 10);
		else
			*(int*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_INT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_int_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_INT;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.integer = *(int*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_UINT functions

int  sq_type_uint_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_UINT:
		*(unsigned int*)instance = src->value.uinteger;
		break;

	case SQXC_TYPE_BOOL:
		*(unsigned int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(unsigned int*)instance = strtoul(src->value.string, NULL, 10);
		else
			*(unsigned int*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_UINT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_uint_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_UINT;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.uinteger = *(unsigned int*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_INTPTR functions

int  sq_type_intptr_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(intptr_t*)instance = src->value.integer;
		break;

	case SQXC_TYPE_BOOL:
		*(intptr_t*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(intptr_t*)instance = strtol(src->value.string, NULL, 10);
		else
			*(intptr_t*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_INT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_intptr_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_INT;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.integer = (int) *(intptr_t*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_INT64 functions

int  sq_type_int64_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(int64_t*)instance = src->value.integer;
		break;

	case SQXC_TYPE_INT64:
		*(int64_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(int64_t*)instance = strtoll(src->value.string, NULL, 10);
		else
			*(int64_t*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_INT64;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_int64_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_INT64;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.int64 = *(int64_t*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_UINT64 functions

int  sq_type_uint64_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_UINT:
		*(uint64_t*)instance = src->value.integer;
		break;

	case SQXC_TYPE_UINT64:
		*(uint64_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(uint64_t*)instance = strtoull(src->value.string, NULL, 10);
		else
			*(uint64_t*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_UINT64;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_uint64_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_UINT64;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.uint64 = *(uint64_t*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_DOUBLE functions

int  sq_type_double_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(double*)instance = src->value.integer;
		break;

	case SQXC_TYPE_DOUBLE:
		*(double*)instance = src->value.double_;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string)
			*(double*)instance = strtod(src->value.string, NULL);
		else
			*(double*)instance = 0;
		break;

	default:
//		src->required_type = SQXC_TYPE_DOUBLE;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_double_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_DOUBLE;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.double_ = *(double*)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_TYPE_TIME functions

int  sq_type_time_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT:
		*(time_t*)instance = src->value.int_;
		break;

	case SQXC_TYPE_INT64:
		*(time_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_TIME:
		*(time_t*)instance = src->value.rawtime;
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string) {
			*(time_t*)instance = sq_time_from_string(src->value.string);
			if (*(time_t*)instance == -1)
				*(time_t*)instance = 0;  // time(NULL);
		}
		else
			*(time_t*)instance = 0;  // time(NULL);
		break;

	default:
//		src->required_type = SQXC_TYPE_INT64;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_time_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_TIME;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.rawtime = *(time_t*)instance;
	dest = sqxc_send(dest);
	return dest;
}

// ------------------------------------
// SqType *SQ_TYPE_STRING functions

void sq_type_string_final(void *instance, const SqType *entrytype)
{
	free(*(char**)instance);
}

int  sq_type_string_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	/* TODO: convert to string
	case SQXC_TYPE_INT:
	case SQXC_TYPE_INT64:
		// free existed string
		if (*(char**)instance)
			free(*(char**)instance);
		break;
	 */

	case SQXC_TYPE_STRING:
		// free existed string
		if (*(char**)instance)
			free(*(char**)instance);

		if (src->value.string)
			*(char**)instance = strdup(src->value.string);
		else
			*(char**)instance = NULL;
		break;

	default:
//		src->required_type = SQXC_TYPE_STRING;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_string_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_STRING;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.string = *(char**)instance;
	return sqxc_send(dest);
}

// ------------------------------------
// SqType *SQ_ENTRY_OBJECT functions

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
const SqType  *SQ_ENTRY_OBJECT = &SqTypeObjectStatic;
 */

int  sq_type_object_parse(void *instance, const SqType *type, Sqxc *src)
{
	SqxcValue  *xc_value = (SqxcValue*)src->dest;
	SqxcNested *nested;
	SqEntry    *entry;

	// Start of Object
	nested = xc_value->nested;
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// Frist time to call this function to parse object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data  = instance;
			nested->data2 = (SqType*)type;
			nested->data3 = NULL;
		}
		if (src->type != SQXC_TYPE_OBJECT) {
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// ready to parse object
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// parse entries in type
	entry = (SqEntry*)sq_type_find_entry(type, src->name, NULL);
	if (entry) {
		entry = *(SqEntry**)entry;
		type = entry->type;
		if (type->parse == NULL)  // don't parse anything if function pointer is NULL
			return (src->code = SQCODE_OK);
		instance = (char*)instance + entry->offset;
		// special case : pointer to instance
		if (entry->bit_field & SQB_POINTER) {
			// try to use existed instance
			if (*(void**)instance)
				instance = *(void**)instance;
			else if (src->type != SQXC_TYPE_STRING || src->value.string != NULL)
				instance = sq_type_init_instance(type, instance, true);
			else
				return (src->code = SQCODE_OK);
		}
		return type->parse(instance, type, src);
	}
	return (src->code = SQCODE_ENTRY_NOT_FOUND);
}

Sqxc *sq_type_object_write(void *instance, const SqType *type, Sqxc *dest)
{
	void       *member;
	SqType     *member_type;
	const char *object_name = dest->name;

	dest->type = SQXC_TYPE_OBJECT;
//	dest->name = object_name;     // "name" was set by caller of this function
//	dest->value.pointer = NULL;
	dest->entry = NULL;           // SqxcSql and SqxcJsonc will use this
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	for (int index = 0;  index < type->n_entry;  index++) {
		SqEntry *entry = type->entry[index];
		member_type = (SqType*)entry->type;
		if (member_type->write == NULL)  // don't write anything if function pointer is NULL
			continue;
		member = (char*)instance + entry->offset;
		dest->name = entry->name;    // set "name" before calling write()
		dest->entry = entry;         // SqxcSql and SqxcJsonc will use this
		if (entry->bit_field & SQB_POINTER) {
			member = *(void**)member;
			if (member == NULL) {
				dest->type = SQXC_TYPE_STRING;
				dest->value.string = NULL;
				dest = sqxc_send(dest);
				if (dest->code != SQCODE_OK)
					return dest;
				continue;
			}
		}
		dest = member_type->write(member, member_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	dest->type = SQXC_TYPE_OBJECT_END;
	dest->name = object_name;
//	dest->value.pointer = NULL;
	dest->entry = NULL;
	return sqxc_send(dest);
}

// ----------------------------------------------------------------------------
// extern SqType

const SqType SqType_BuiltIn_[] = {
	// SQ_TYPE_BOOL
	{
		sizeof(bool),
		NULL,
		NULL,
		sq_type_bool_parse,
		sq_type_bool_write,
	},
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
	// SQ_TYPE_TIME
	{
		sizeof(time_t),
		NULL,
		NULL,
		sq_type_time_parse,
		sq_type_time_write,
	},
	// SQ_TYPE_DOUBLE
	{
		sizeof(double),
		NULL,
		NULL,
		sq_type_double_parse,
		sq_type_double_write,
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
