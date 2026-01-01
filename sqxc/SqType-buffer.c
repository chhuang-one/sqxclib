/*
 *   Copyright (C) 2023-2026 by C.H. Huang
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
#include <stdio.h>             // fprintf(), stderr
#endif

#include <sqxc/SqError.h>
#include <sqxc/SqType.h>
#include <sqxc/SqBuffer.h>
#include <sqxc/SqConvert.h>
#include <sqxc/SqxcSql.h>

#if SQ_CONFIG_HAVE_JSON
#include <sqxc/SqxcJson.h>
#endif

static int   sq_type_buffer_parse(void *instance, const SqType *type, Sqxc *src)
{
	SqBuffer *buf = instance;
	size_t    len;

	switch (src->type) {
	case SQXC_TYPE_NULL:
	case SQXC_TYPE_STR:
//	case SQXC_TYPE_RAW:
		buf->writed = 0;
		if (src->value.str == NULL)
			break;

#if SQ_CONFIG_HAVE_JSON
		if (src->info == SQXC_INFO_JSON_PARSER) {
			// TODO:
			// convert BASE64 to binary
			len = strlen(src->value.str);
			sq_buffer_resize(buf, len +1);
			memcpy(buf->mem, src->value.str, len);
			buf->writed = len;
			buf->mem[len] = 0;        // null-terminated
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
				// message
				fprintf(stderr, "%s: string is not hex format.\n",
				        "sq_type_buffer_parse()");
#endif
				// User can set length of BLOB in SqBuffer::size before parsing.
				len = buf->size;
				if (len == 0)
					len =  strlen(src->value.str);
				// allocate memory and write data
				sq_buffer_resize(buf, len);
				memcpy(buf->mem, src->value.str, len);
				buf->writed = len;
				break;
			}

			// string is hex format:
			// User can set length of BLOB in SqBuffer::size before parsing.
			if (buf->size > 0)
				len = buf->size << 1;    // len = buf->size * 2;
			else {
				len = strlen(src->value.str) - len;
				buf->size = len >> 1;    // buf->size = len / 2;
			}

			sq_buffer_resize(buf, buf->size);
			buf->writed = sq_hex_to_bin(buf->mem, src->value.str+2, len);
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

static Sqxc *sq_type_buffer_write(void *instance, const SqType *type, Sqxc *dest)
{
	SqBuffer *buf = instance;
	Sqxc     *xc;
	char     *mem = NULL;
	size_t    len;

	// use SQXC_TYPE_RAW to send hex (exclude PostgreSQL)
	dest->type = SQXC_TYPE_RAW;
//	dest->name = dest->name;    // "name" was set by caller of this function

#if SQ_CONFIG_HAVE_JSON
	if (dest->info == SQXC_INFO_JSON_WRITER) {
		// TODO:
		// convert binary to BASE64
		len = buf->writed;
		mem = malloc(len +1);      // + null-terminated
		memcpy(mem, buf->mem, len);
		mem[len] = 0;
	}
	else
#endif
	if (dest->info == SQXC_INFO_SQL) {
		// convert binary to HEX
		len = buf->writed *2 +3;   // + 0x, x'', or \x
		mem = malloc(len +1);      // + null-terminated
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

const SqType sqType_Buffer =
{
	sizeof(SqBuffer),
	(SqTypeFunc)sq_buffer_init,
	(SqTypeFunc)sq_buffer_final,
	sq_type_buffer_parse,
	sq_type_buffer_write,
};
