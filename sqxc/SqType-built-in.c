/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <limits.h>       // __WORDSIZE
#include <stdint.h>       // __WORDSIZE  for Apple Developer
#include <time.h>         // time_t
#include <stdio.h>        // sprintf(), fprintf(), stderr
#include <stdlib.h>       // realloc(), strtol()
#include <string.h>       // strdup()

#include <SqConvert.h>    // sq_time_to_string(), sq_time_from_string()
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
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_BOOL:
		*(bool*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_INT:
		*(bool*)instance = (src->value.integer) ? true : false;
		break;

	case SQXC_TYPE_STR:
		if (src->value.str) {
			ch = src->value.str[0];
			if (ch == '0' || ch == 'f' || ch == 'F')    // '0', 'false', or 'FALSE'
				*(bool*)instance = false;
			else
				*(bool*)instance = true;
		}
		else
			*(bool*)instance = false;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_BOOL;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
	case SQXC_TYPE_INT64:
#if 0 // ( defined(__WORDSIZE) && (__WORDSIZE == 32) )  ||  ( defined(_MSC_VER) )
		// integer is 32-bit
		if (src->value.int64 > INT32_MAX)
			*(int*)instance = INT32_MAX;
		else if (src->value.int64 < INT32_MIN)
			*(int*)instance = INT32_MIN;
		else
			*(int*)instance = (int)src->value.int64;
		break;
#elif !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(int*)instance = (int)src->value.int64;
		break;
#endif
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_INT:
		*(int*)instance = src->value.integer;
		break;

#if SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT
	case SQXC_TYPE_UINT64:
#if 0 // ( defined(__WORDSIZE) && (__WORDSIZE == 32) )  ||  ( defined(_MSC_VER) )
		// integer is 32-bit
		if (src->value.uint64 > UINT32_MAX)
			*(int*)instance = UINT32_MAX;
		else
			*(int*)instance = (int)src->value.uint64;
		break;
#elif !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(int*)instance = (int)src->value.uint64;
		break;
#endif
	case SQXC_TYPE_UINT:
		*(int*)instance = src->value.uint;
		break;
#endif  // SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT

	case SQXC_TYPE_BOOL:
		*(int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STR:
		if (src->value.str)
			*(int*)instance = strtol(src->value.str, NULL, 10);
		else
			*(int*)instance = 0;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_INT;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
	case SQXC_TYPE_UINT64:
#if 0 // ( defined(__WORDSIZE) && (__WORDSIZE == 32) )  ||  ( defined(_MSC_VER) )
		// integer is 32-bit
		if (src->value.uint64 > UINT32_MAX)
			*(unsigned int*)instance = UINT32_MAX;
		else
			*(unsigned int*)instance = (unsigned int)src->value.uint64;
		break;
#elif !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(unsigned int*)instance = (unsigned int)src->value.uint64;
		break;
#endif
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_UINT:
		*(unsigned int*)instance = src->value.uinteger;
		break;

#if SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT
	case SQXC_TYPE_INT64:
#if 0 // ( defined(__WORDSIZE) && (__WORDSIZE == 32) )  ||  ( defined(_MSC_VER) )
		// integer is 32-bit
		if (src->value.int64 > INT32_MAX)
			*(unsigned int*)instance = INT32_MAX;
		else if (src->value.int64 < INT32_MIN)
			*(unsigned int*)instance = INT32_MIN;
		else
			*(unsigned int*)instance = (unsigned int)src->value.int64;
		break;
#elif !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(unsigned int*)instance = (unsigned int)src->value.int64;
		break;
#endif
	case SQXC_TYPE_INT:
		*(unsigned int*)instance = src->value.integer;
		break;
#endif  // SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT

	case SQXC_TYPE_BOOL:
		*(unsigned int*)instance = src->value.boolean;
		break;

	case SQXC_TYPE_STR:
		if (src->value.str)
			*(unsigned int*)instance = strtoul(src->value.str, NULL, 10);
		else
			*(unsigned int*)instance = 0;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_UINT;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
// SqType *SQ_TYPE_INT64 functions

int  sq_type_int64_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	case SQXC_TYPE_INT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(int64_t*)instance = src->value.int64;
		break;
#endif
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_INT:
		*(int64_t*)instance = src->value.integer;
		break;

#if SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT
	case SQXC_TYPE_UINT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(int64_t*)instance = src->value.uint64;
		break;
#endif
	case SQXC_TYPE_UINT:
		*(int64_t*)instance = src->value.uinteger;
		break;
#endif  // SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT

	case SQXC_TYPE_STR:
		if (src->value.str)
			*(int64_t*)instance = strtoll(src->value.str, NULL, 10);
		else
			*(int64_t*)instance = 0;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_INT64;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
	case SQXC_TYPE_UINT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(uint64_t*)instance = src->value.uint64;
		break;
#endif
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_UINT:
		*(uint64_t*)instance = src->value.uinteger;
		break;

#if SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT
	case SQXC_TYPE_INT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(uint64_t*)instance = src->value.int64;
		break;
#endif
	case SQXC_TYPE_INT:
		*(uint64_t*)instance = src->value.integer;
		break;
#endif  // SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT

	case SQXC_TYPE_STR:
		if (src->value.str)
			*(uint64_t*)instance = strtoull(src->value.str, NULL, 10);
		else
			*(uint64_t*)instance = 0;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_UINT64;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
	case SQXC_TYPE_INT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(double*)instance = (double)src->value.int64;
		break;
#endif
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_INT:
		*(double*)instance = (double)src->value.integer;
		break;

#if SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT
	case SQXC_TYPE_UINT64:
#if !defined(__WORDSIZE) || (__WORDSIZE != 64)
		// integer is NOT 64-bit
		*(double*)instance = (double)src->value.uint64;
		break;
#endif
	case SQXC_TYPE_UINT:
		*(double*)instance = (double)src->value.uinteger;
		break;
#endif  // SQ_CONFIG_CONVERT_SIGNED_UNSIGNED_INT

	case SQXC_TYPE_DOUBLE:
		*(double*)instance = src->value.double_;
		break;

	case SQXC_TYPE_STR:
		if (src->value.str)
			*(double*)instance = strtod(src->value.str, NULL);
		else
			*(double*)instance = 0;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_DOUBLE;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_INT:
		*(time_t*)instance = src->value.int_;
		break;

	case SQXC_TYPE_INT64:
		*(time_t*)instance = src->value.int64;
		break;

	case SQXC_TYPE_TIME:
		*(time_t*)instance = src->value.rawtime;
		break;

	case SQXC_TYPE_STR:
		if (src->value.str) {
			*(time_t*)instance = sq_time_from_string(src->value.str);
			if (*(time_t*)instance == -1)
				*(time_t*)instance = 0;  // time(NULL);
		}
		else
			*(time_t*)instance = 0;  // time(NULL);
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_INT64;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
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
// SqType *SQ_TYPE_STR functions

void sq_type_str_final(void *instance, const SqType *entrytype)
{
	free(*(char**)instance);
}

int  sq_type_str_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	switch (src->type) {
	/* TODO: convert to string
	case SQXC_TYPE_INT:
	case SQXC_TYPE_INT64:
		break;
	 */

	case SQXC_TYPE_NULL:
	case SQXC_TYPE_STR:
		/* Don't free existed string in container. It may cause memory corruption.
		// free existed string
		if (*(char**)instance)
			free(*(char**)instance);
		*/

		if (src->value.str)
			*(char**)instance = strdup(src->value.str);
		else
			*(char**)instance = NULL;
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_STR;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_str_write(void *instance, const SqType *entrytype, Sqxc *dest)
{
	dest->type = SQXC_TYPE_STR;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.str = *(char**)instance;
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
	union {
		SqEntry    *entry;
		void      **addr;
	} p;

	// Start of Object
	nested = xc_value->nested;
#if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// First time to call this function to parse object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data  = instance;
			nested->data2 = (SqType*)type;
			nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
		}
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// SqxcNested is ready to parse object, type has been matched.
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
#else
	if (nested->data != instance) {
		// do type match
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = instance;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
#endif  // SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// parse entries in type
	p.addr = sq_type_find_entry(type, src->name, NULL);
	if (p.addr) {
		p.entry = *p.addr;
		type = p.entry->type;
		if (type->parse == NULL)  // don't parse anything if function pointer is NULL
			return (src->code = SQCODE_OK);
		instance = (char*)instance + p.entry->offset;
		// special case : pointer to instance
		if (p.entry->bit_field & SQB_POINTER) {
			// try to use existed instance
			if (*(void**)instance)
				instance = *(void**)instance;
			// allocate & initialize instance if source is not NULL
			else if (src->type != SQXC_TYPE_NULL)
				instance = sq_type_init_instance(type, instance, true);
			else
				return (src->code = SQCODE_OK);
		}
		return type->parse(instance, type, src);
	}

#ifndef NDEBUG
	// warning
	fprintf(stderr, "%s: entry or column '%s' not found.\n",
	        "sq_type_object_parse()", src->name);
#endif
	return (src->code = SQCODE_ENTRY_NOT_FOUND);
}

Sqxc *sq_type_object_write(void *instance, const SqType *type, Sqxc *dest)
{
//	SqEntry   **cur, **end;
	void      **cur, **end;
	void       *member;
	SqType     *member_type;
	const char *object_name = dest->name;

	dest->type = SQXC_TYPE_OBJECT;
//	dest->name = object_name;     // "name" was set by caller of this function
//	dest->value.pointer = NULL;
	dest->entry = NULL;           // SqxcSql and SqxcJson will use this
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	cur = (void**)type->entry;
	end = cur + type->n_entry;
	for (;  cur < end;  cur++) {
		SqEntry *entry = *cur;
		member_type = (SqType*)entry->type;
		if (member_type->write == NULL)  // don't write anything if function pointer is NULL
			continue;
		member = (char*)instance + entry->offset;
		dest->name = entry->name;    // set "name" before calling write()
		dest->entry = entry;         // SqxcSql and SqxcJson will use this
		if (entry->bit_field & SQB_POINTER) {
			member = *(void**)member;
			if (member == NULL) {
				dest->type = SQXC_TYPE_NULL;
				dest->value.pointer = NULL;
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

const SqType sqType_BuiltIn[SQ_TYPE_N_BUILT_IN] = {
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
	// SQ_TYPE_STR
	{
		sizeof(char*),
		NULL,
		sq_type_str_final,
		sq_type_str_parse,
		sq_type_str_write,
	},
	// SQ_TYPE_CHAR
	{
		sizeof(char*),
		NULL,
		sq_type_str_final,
		sq_type_str_parse,
		sq_type_str_write,
	},
};
