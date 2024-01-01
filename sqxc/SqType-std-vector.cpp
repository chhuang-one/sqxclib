/*
 *   Copyright (C) 2023-2024 by C.H. Huang
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
#ifndef NDEBUG
#include <stdio.h>        // stderr
#endif

#include <vector>
#include <SqError.h>
#include <SqType.h>
#include <SqUtil.h>
#include <SqxcSql.h>

#if SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

/* ----------------------------------------------------------------------------
	SqType for std::vector<char>

	We can call C++ constructor/destructor in init()/final()

	// ==== C++ constructor + C malloc()
	memory = malloc(size);
	object = new (memory) MyClass();

	// ==== C++ destructor  + C free()
	object->~MyClass();
	free(memory);
 */

static void sq_type_std_vector_init(void *instance, const SqType *type)
{
    new (instance) std::vector<char>();
}

static void sq_type_std_vector_final(void *instance, const SqType *type)
{
    ((std::vector<char>*)(instance))->std::vector<char>::~vector();
}

static int  sq_type_std_vector_parse(void *instance, const SqType *type, Sqxc *src)
{
	std::vector<char> *vector = (std::vector<char>*)instance;
	int   len;

	switch (src->type) {
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_STR:
//	case SQXC_TYPE_RAW:
		if (src->value.str == NULL)
			break;

#if SQ_CONFIG_HAVE_JSONC
		if (src->info == SQXC_INFO_JSONC_PARSER) {
			// TODO:
			// convert BASE64 to binary
			len = (int)strlen(src->value.str);
			vector->assign(src->value.str, src->value.str + len);
		}
		else
#endif
		// if (src->info == SQXC_INFO_VALUE)
		{
			// convert Hex string to binary
			// Hex format is \xFF  - PostgreSQL
			if (src->value.str[0] == '\\' && src->value.str[1] == 'x')
				len = 2;    // length of prefix \x
/*
			// Hex format is x'FF' - ?
			else if (src->value.str[0] == 'x' && src->value.str[1] == '\'')
				len = 3;    // length of prefix x' and postfix '
			// Hex format is 0xFF  - SQL Server?
			else if (src->value.str[0] == '0' && src->value.str[1] == 'x')
				len = 2;    // length of prefix 0x
 */
			// No Hex format       - SQLite, MySQL
			else {
				// string is not hex format:
#ifndef NDEBUG
				fprintf(stderr, "sq_type_std_vector_parse(): string is not hex format.\n");
#endif
				// User can set length of BLOB by calling std::vector<char>.resize() before parsing.
				len = (int)vector->size();
				if (len == 0)
					len = (int)strlen(src->value.str);
				vector->assign(src->value.str, src->value.str + len);
				break;
			}

			// string is hex format:
			// User can set length of BLOB by calling std::vector<char>.resize() before parsing.
			if (vector->size() > 0)
				len = (int)vector->size() << 1;    // len = (int)vector->size() * 2;
			else
				len = (int)strlen(src->value.str) - len;

			char *mem = (char*)malloc(len >> 1);
			sq_hex_to_bin(mem, src->value.str+2, len);
			vector->assign(mem, mem + (len >> 1));
			free(mem);
		}
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_BOOL;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
	}

	return (src->code = SQCODE_OK);
}

static Sqxc *sq_type_std_vector_write(void *instance, const SqType *type, Sqxc *dest)
{
	std::vector<char> *vector = (std::vector<char>*)instance;
	Sqxc *xc;
	char *mem = NULL;
	int   len;

	// use SQXC_TYPE_RAW to send hex (exclude PostgreSQL)
	dest->type = SQXC_TYPE_RAW;
//	dest->name = dest->name;    // "name" was set by caller of this function

#if SQ_CONFIG_HAVE_JSONC
	if (dest->info == SQXC_INFO_JSONC_WRITER) {
		// TODO:
		// convert binary to BASE64
		len = vector->size();
		mem = (char*)malloc(len +1);      // + null-terminated
		memcpy(mem, vector->data(), len);
		mem[len] = 0;
	}
	else
#endif
	if (dest->info == SQXC_INFO_SQL) {
		// convert binary to HEX
		len = (int)vector->size() *2 +3;       // + 0x, x'', or \x
		mem = (char*)malloc(len +1);      // + null-terminated
		// Hex format is x'FF' - SQLite, MySQL
		if (((SqxcSql*)dest)->db->info->product == SQDB_PRODUCT_SQLITE) {
			mem[0] = 'x';
			mem[1] = '\'';
			mem[len-1] = '\'';
			mem[len]   = 0;        // null-terminated
		}
		// Hex format is \xFF  - PostgreSQL
		else if (((SqxcSql*)dest)->db->info->product == SQDB_PRODUCT_POSTGRE) {
			mem[0] = '\\';
			mem[1] = 'x';
			mem[len-1] = 0;        // null-terminated
			// PostgreSQL send hex like SQL string
			dest->type = SQXC_TYPE_STR;
		}
		// Hex format is 0xFF  - MySQL, SQL Server
		else {
			mem[0] = '0';
			mem[1] = 'x';
			mem[len-1] = 0;        // null-terminated
		}
		sq_bin_to_hex(mem+2, vector->data(), (int)vector->size());
	}

	dest->value.raw = mem;
	xc = sqxc_send(dest);
	free(mem);
	return xc;
}

// extern
const SqType SqType_StdVector_ =
{
	sizeof(std::vector<char>),
	sq_type_std_vector_init,
	sq_type_std_vector_final,
	sq_type_std_vector_parse,
	sq_type_std_vector_write,
};

// ----------------------------------------------------------------------------

static int  sq_type_std_vector_size_parse(void *instance, const SqType *entrytype, Sqxc *src)
{
	std::vector<char> *vector = (std::vector<char>*)instance;

	switch (src->type) {
	case SQXC_TYPE_INT:
		vector->resize((int)src->value.integer);
		break;

	case SQXC_TYPE_STR:
		if (src->value.str)
			vector->resize((int)strtol(src->value.str, NULL, 10));
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCHED
		src->required_type = SQXC_TYPE_INT;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCHED);
	}

	return (src->code = SQCODE_OK);
}

// extern
const SqType SqType_StdVectorSize_ =
{
	sizeof(std::vector<char>),
	NULL,
	NULL,
	sq_type_std_vector_size_parse,
	NULL,
};
