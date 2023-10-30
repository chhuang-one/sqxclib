/*
 *   Copyright (C) 2023 by C.H. Huang
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

#ifndef NDEBUG
#include <stdio.h>        // stderr
#endif

#include <SqType.h>
#include <SqBuffer.h>
#include <SqError.h>
#include <SqUtil.h>
#include <SqxcSql.h>

#if SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

void  sq_type_buffer_init(void *instance, const SqType *type)
{
	sq_buffer_init((SqBuffer*)instance);
}

void  sq_type_buffer_final(void *instance, const SqType *type)
{
	sq_buffer_final((SqBuffer*)instance);
}

int   sq_type_buffer_parse(void *instance, const SqType *type, Sqxc *src)
{
	SqBuffer *buf = instance;
	int   len;

	switch (src->type) {
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_STR:
//	case SQXC_TYPE_RAW:
		buf->writed = 0;
		if (src->value.str == NULL)
			break;
		len = (int)strlen(src->value.str);

#if SQ_CONFIG_HAVE_JSONC
		if (src->info == SQXC_INFO_JSONC_PARSER) {
			// TODO:
			// convert BASE64 to binary
			sq_buffer_resize(buf, len +1);
			memcpy(buf->mem, src->value.str, len);
			buf->writed = len;
			buf->mem[len] = 0;        // null-terminated
		}
		else
#endif
		{         // if (src->info == SQXC_INFO_VALUE)
			// convert Hex string to binary
			// Hex format is \xFF  - PostgreSQL
			if (src->value.str[0] == '\\' && src->value.str[1] == 'x')
				len -= 2;    // remove prefix \x
/*
			// Hex format is x'FF' - MySQL?
			else if (src->value.str[0] == 'x' && src->value.str[1] == '\'')
				len -= 3;    // remove prefix x' and ' in tail
			// Hex format is 0xFF  - MySQL?, SQL Server?
			else if (src->value.str[0] == '0' && src->value.str[1] == 'x')
				len -= 2;    // remove prefix 0x
 */
			// No Hex format       - SQLite, MySQL
			else {
#ifndef NDEBUG
				fprintf(stderr, "sq_type_buffer_parse(): string is not hex format.\n");
#endif
				// User can assign length of BLOB in SqBuffer.size before parsing
				if (len < buf->size)
					len = buf->size;
				sq_buffer_resize(buf, len);
				memcpy(buf->mem, src->value.str, len);
				buf->writed = len;
				break;
			}

			// hex string
			sq_buffer_resize(buf, len >> 1);    // sq_buffer_resize(buf, len / 2)
			buf->writed = sq_hex_to_bin(buf->mem, src->value.str+2, len);
		}
		break;

	default:
		/* set required type if return SQCODE_TYPE_NOT_MATCH
		src->required_type = SQXC_TYPE_BOOL;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

Sqxc *sq_type_buffer_write(void *instance, const SqType *type, Sqxc *dest)
{
	SqBuffer *buf = instance;
	Sqxc     *xc;
	char *mem = NULL;
	int   len;

	// use SQXC_TYPE_RAW to send hex (exclude PostgreSQL)
	dest->type = SQXC_TYPE_RAW;
//	dest->name = dest->name;    // "name" was set by caller of this function

#if SQ_CONFIG_HAVE_JSONC
	if (dest->info == SQXC_INFO_JSONC_WRITER) {
		// TODO:
		// convert binary to BASE64
		len = buf->writed +1;    // + '\0'
		mem = malloc(len);
		memcpy(mem, buf->mem, buf->writed);
		mem[len-1] = 0;
	}
	else
#endif
	if (dest->info == SQXC_INFO_SQL) {
		// convert binary to HEX
		len = buf->writed *2 +3 +1;    // + (0x , x'' , or \x ) + '\0'
		mem = malloc(len);
		// Hex format is x'FF' - SQLite, MySQL
		if (((SqxcSql*)dest)->db->info->product == SQDB_PRODUCT_SQLITE) {
			mem[0] = 'x';
			mem[1] = '\'';
			mem[len-2] = '\'';
			mem[len-1] = 0;        // null-terminated
		}
		// Hex format is \xFF  - PostgreSQL
		else if (((SqxcSql*)dest)->db->info->product == SQDB_PRODUCT_POSTGRE) {
			mem[0] = '\\';
			mem[1] = 'x';
			mem[len-2] = 0;        // null-terminated
			// PostgreSQL send hex like SQL string
			dest->type = SQXC_TYPE_STR;
		}
		// Hex format is 0xFF  - MySQL, SQL Server
		else {
			mem[0] = '0';
			mem[1] = 'x';
			mem[len-2] = 0;        // null-terminated
		}
		sq_bin_to_hex(mem+2, buf->mem, buf->writed);
	}

	dest->value.raw = mem;
	xc = sqxc_send(dest);
	free(mem);
	return xc;
}

/* ----------------------------------------------------------------------------
	SQ_TYPE_BUFFER
 */

const SqType SqType_Buffer_ =
{
	sizeof(SqBuffer),
	sq_type_buffer_init,
	sq_type_buffer_final,
	sq_type_buffer_parse,
	sq_type_buffer_write,
};
