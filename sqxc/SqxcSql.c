/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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
#include <limits.h>     // __WORDSIZE
#include <stdint.h>     // __WORDSIZE (Apple)
#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqBuffer.h>
#include <SqUtil.h>
#include <SqTable.h>
#include <SqxcSql.h>

#ifdef _MSC_VER
#ifdef _WIN64
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif  // _WIN64
#define strncasecmp  _strnicmp
#define snprintf     _snprintf
#define strdup       _strdup
#endif  // _MSC_VER

/*
	Because SQL table == C struct (object),
	SqxcSql must "support" SQXC_TYPE_OBJECT at startup.
	             unsupport SQXC_TYPE_OBJECT while parsing/writing column.

	multiple row == array of object
	SqxcSql must "support" SQXC_TYPE_ARRAY at startup.
	             unsupport SQXC_TYPE_ARRAY while parsing/writing object.
 */

static void sqxc_sql_use_insert_command(SqxcSql *xcsql, const char *table_name);
static void sqxc_sql_use_update_command(SqxcSql *xcsql, const char *table_name);
static int  sqxc_sql_write_value(SqxcSql *xcsql, Sqxc *src, SqBuffer *buffer);

/* ----------------------------------------------------------------------------
	SqxcInfo functions - destination of output chain

	SQXC_TYPE_xxx ---> SqxcSql ---> Sqdb.exec()
 */

static int  sqxc_sql_send_insert_command(SqxcSql *xcsql, Sqxc *src)
{
	SqBuffer *values_buf = &xcsql->values_buf;
	SqBuffer *names_buf = sqxc_get_buffer(xcsql);
	SqEntry  *entry;

	switch (src->type) {
	case SQXC_TYPE_ARRAY:
//		src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		if (xcsql->outer_type & (SQXC_TYPE_ARRAY | SQXC_TYPE_OBJECT))
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		xcsql->outer_type |= SQXC_TYPE_ARRAY;
		xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
		xcsql->supported_type |= SQXC_TYPE_END;
		// --- Begin of Array ---
		xcsql->row_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT:
//		src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		if (xcsql->outer_type & SQXC_TYPE_OBJECT)
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		xcsql->outer_type |= SQXC_TYPE_OBJECT;
		xcsql->supported_type &= ~(SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY);
		xcsql->supported_type |= SQXC_TYPE_END;
		// --- Begin of row ---
		if (xcsql->row_count)
			sq_buffer_write_c(values_buf, ',');
		sq_buffer_write_c(values_buf, '(');
		xcsql->row_count++;
		xcsql->col_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT_END:
		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_OBJECT;
		xcsql->supported_type |= SQXC_TYPE_OBJECT;
		// --- End of row ---
		sq_buffer_write_c(values_buf, ')');
		if ((xcsql->outer_type & SQXC_TYPE_ARRAY) == 0) {
			sq_buffer_write_c(values_buf, 0);    // null-terminated
			// SQL INSERT VALUES has written in xcsql->values_buf
		}
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_ARRAY_END:
		if ((xcsql->outer_type & SQXC_TYPE_ARRAY) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_ARRAY;
		xcsql->supported_type |= SQXC_TYPE_ARRAY;
		// --- End of Array ---
		sq_buffer_write_c(values_buf, 0);    // null-terminated
		// SQL INSERT VALUES has written in xcsql->values_buf
		return (src->code = SQCODE_OK);

	default:
		break;
	}

	entry = src->entry;
	if (entry) {
		// Don't output column that has AUTO INCREMENT and value.integer is 0
		if (entry->bit_field & SQB_AUTOINCREMENT) {
			if (src->value.int64 == 0)    //  && (entry->type == SQ_TYPE_INT64 || entry->type == SQ_TYPE_UINT64)
				return (src->code = SQCODE_OK);
			if (src->value.int_  == 0 && (entry->type == SQ_TYPE_INT   || entry->type == SQ_TYPE_UINT))
				return (src->code = SQCODE_OK);
		}
		// Don't output column that has DEFAULT CURRENT_XXXX and value.rawtime is 0
		if (entry->type == SQ_TYPE_TIME && src->type == SQXC_TYPE_TIME && src->value.rawtime == 0) {
			if (entry->bit_field & SQB_CURRENT)
				return (src->code = SQCODE_OK);
			if ( ((SqColumn*)entry)->default_value && strncasecmp("CURRENT_", ((SqColumn*)entry)->default_value, 8) == 0 )
				return (src->code = SQCODE_OK);
		}
	}

	// SQL statement multiple columns
	if (xcsql->col_count) {
		sq_buffer_write_c(names_buf, ',');
		sq_buffer_write_c(values_buf, ',');
	}

	// value
	if (sqxc_sql_write_value(xcsql, src, values_buf) != SQCODE_OK) {
		if (xcsql->col_count) {
			names_buf->writed--;     // remove ',' from names_buf
			values_buf->writed--;    // remove ',' form values_buf
		}
	}
	// "name"
	else {
		sq_buffer_write_c(names_buf, xcsql->quote[0]);
		sq_buffer_write(names_buf, src->name);
		sq_buffer_write_c(names_buf, xcsql->quote[1]);

		xcsql->col_count++;
	}

	return src->code;
}

static int  sqxc_sql_send_update_command(SqxcSql *xcsql, Sqxc *src)
{
	SqBuffer  *buffer = sqxc_get_buffer(xcsql);
	SqEntry   *entry;
	int        len;

	switch (src->type) {
	case SQXC_TYPE_ARRAY:
//		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);

	case SQXC_TYPE_ARRAY_END:
		return (src->code = SQCODE_TYPE_END_ERROR);

	case SQXC_TYPE_OBJECT:
//		src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
		if (xcsql->outer_type & SQXC_TYPE_OBJECT)
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		xcsql->outer_type |= SQXC_TYPE_OBJECT;
		xcsql->supported_type &= ~(SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY);
		xcsql->supported_type |= SQXC_TYPE_END;
		xcsql->col_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT_END:
		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_OBJECT;
		xcsql->supported_type |= SQXC_TYPE_OBJECT;
		// SQL statement
		if (xcsql->condition) {
			// WHERE condition
			sq_buffer_write_n(buffer, " WHERE ", 7);
			sq_buffer_write(buffer, xcsql->condition);
		}
		sq_buffer_write_c(buffer, 0);    // null-terminated
		return (src->code = SQCODE_OK);

	default:
		break;
	}

	entry = src->entry;
	if (entry) {
		// Don't output primary key
		if (entry->bit_field & SQB_PRIMARY)
			return (src->code = SQCODE_OK);
		// Don't output column that has DEFAULT CURRENT_XXXX and value.rawtime is 0
		if (entry->type == SQ_TYPE_TIME && src->type == SQXC_TYPE_TIME && src->value.rawtime == 0) {
			if (entry->bit_field & SQB_CURRENT_ALL)
				return (src->code = SQCODE_OK);
			if ( ((SqColumn*)entry)->default_value && strncasecmp("CURRENT_", ((SqColumn*)entry)->default_value, 8) == 0 )
				return (src->code = SQCODE_OK);
		}
	}

	len = buffer->writed;
	// SQL statement multiple columns
	if (xcsql->col_count)
		sq_buffer_write_c(buffer, ',');

	// "name"=value
	sq_buffer_write_c(buffer, xcsql->quote[0]);
	sq_buffer_write(buffer, src->name);
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = xcsql->quote[1];
	sq_buffer_r_at(buffer, 0) = '=';
	if (sqxc_sql_write_value(xcsql, src, NULL) != SQCODE_OK)
		buffer->writed = len;
	else
		xcsql->col_count++;

	return src->code;
}

static int  sqxc_sql_send(SqxcSql *xcsql, Sqxc *src)
{
	// 1 == INSERT, 0 == UPDATE
	if (xcsql->mode == 1)
		return sqxc_sql_send_insert_command(xcsql, src);
	else
		return sqxc_sql_send_update_command(xcsql, src);
}

static int  sqxc_sql_ctrl(SqxcSql *xcsql, int id, void *data)
{
	int  code;

	switch (id) {
	case SQXC_CTRL_READY:
		xcsql->supported_type = SQXC_TYPE_ALL;
		xcsql->outer_type = SQXC_TYPE_NONE;
		// reset Sqdb result variable
		xcsql->id = 0;
		xcsql->changes = 0;
		break;

	case SQXC_CTRL_FINISH:
		xcsql->condition = NULL;
		// write INSERT VALUES to xcsql->buf
		if (xcsql->mode == 1) {
			SqBuffer *buffer = sqxc_get_buffer(xcsql);
			SqBuffer *values = &xcsql->values_buf;
	
			// length of ") VALUES " is 9
			sq_buffer_resize(buffer, buffer->writed + 9 + values->writed + 1);
			sq_buffer_write(buffer, ") VALUES ");
			sq_buffer_write_n(buffer, values->mem, values->writed);
			// reset values buffer
			values->writed = 0;
		}
		// SQL statement has written in xcsql->buf
		if (xcsql->db && xcsql->buf_writed > 0) {
			code = sqdb_exec(xcsql->db, xcsql->buf, (Sqxc*)xcsql, NULL);
			if (code != SQCODE_OK)
				return (xcsql->code = SQCODE_EXEC_ERROR);
		}
		// clear SqxcNested if problem occurred during processing
		sqxc_clear_nested((Sqxc*)xcsql);
		// reset buffer
		xcsql->buf_writed = 0;
		break;

	case SQXC_SQL_CTRL_INSERT:
		xcsql->mode = 1;
		xcsql->row_count = 0;
//		xcsql->col_count = 0;
		sqxc_sql_use_insert_command(xcsql, data);
		break;

	case SQXC_SQL_CTRL_UPDATE:
		xcsql->mode = 0;
//		xcsql->row_count = 0;
		xcsql->col_count = 0;
		sqxc_sql_use_update_command(xcsql, data);
		break;

	default:
		return (xcsql->code = SQCODE_NOT_SUPPORT);
	}

	return SQCODE_OK;
}

static void  sqxc_sql_init(SqxcSql *xcsql)
{
//	memset(xcsql, 0, sizeof(SqxcSql));
	sq_buffer_resize(sqxc_get_buffer(xcsql), SQ_CONFIG_SQXC_SQL_BUFFER_SIZE_DEAULT);
	sq_buffer_init(&xcsql->values_buf);

	xcsql->supported_type = SQXC_TYPE_ALL;
	xcsql->outer_type = SQXC_TYPE_NONE;
	// default of quote identifier
	xcsql->quote[0] = '"';
	xcsql->quote[1] = '"';
	// controlled variable
//	xcsql->mode = 1;
//	xcsql->condition = NULL;
	// Sqdb result variable
	xcsql->id = 0;
	xcsql->changes = 0;
}

static void  sqxc_sql_final(SqxcSql *xcsql)
{
	sq_buffer_final(&xcsql->values_buf);
}

// ----------------------------------------------------------------------------
// others functions

static void sqxc_sql_use_insert_command(SqxcSql *xcsql, const char *table_name)
{
	SqBuffer  *buffer;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// "INSERT INTO "table_name" ("column1","column2") OUTPUT Inserted.id VALUES "
	sq_buffer_write(buffer, "INSERT INTO");
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = xcsql->quote[0];
	sq_buffer_write(buffer, table_name);
	sq_buffer_alloc(buffer, 3);
	sq_buffer_r_at(buffer, 2) = xcsql->quote[1];
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = '(';
}

static void sqxc_sql_use_update_command(SqxcSql *xcsql, const char *table_name)
{
	SqBuffer  *buffer;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// "UPDATE 'table_name' SET "
	sq_buffer_write(buffer, "UPDATE");
	sq_buffer_alloc(buffer, 2);
	sq_buffer_r_at(buffer, 1) = ' ';
	sq_buffer_r_at(buffer, 0) = xcsql->quote[0];
	sq_buffer_write(buffer, table_name);
	sq_buffer_write_c(buffer, xcsql->quote[1]);

	sq_buffer_write(buffer, " SET ");

	xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
	// reuse after running Sqdb.exec()
	xcsql->buf_reuse = xcsql->buf_writed;
}

static int  sqxc_sql_write_value(SqxcSql *xcsql, Sqxc *src, SqBuffer *buffer)
{
	int   len, idx;
	char *tempstr;

	if (buffer == NULL)
		buffer = sqxc_get_buffer(xcsql);

	switch (src->type) {
	case SQXC_TYPE_BOOL:
#if 1
		if (src->value.boolean)
			sq_buffer_write_c(buffer, '1');
		else
			sq_buffer_write_c(buffer, '0');
#else
		if (src->value.boolean)
			sq_buffer_write(buffer, "TRUE");
		else
			sq_buffer_write(buffer, "FALSE");
#endif
		break;

	case SQXC_TYPE_INT:
		len = snprintf(NULL, 0, "%d", src->value.integer);
		sprintf(sq_buffer_alloc(buffer, len), "%d", src->value.integer);
		break;

	case SQXC_TYPE_UINT:
		len = snprintf(NULL, 0, "%u", src->value.uint);
		sprintf(sq_buffer_alloc(buffer, len), "%u", src->value.uint);
		break;

	case SQXC_TYPE_INT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "%I64d", src->value.int64);
		sprintf(sq_buffer_alloc(buffer, len), "%I64d", src->value.int64);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		len = snprintf(NULL, 0, "%ld", src->value.int64);
		sprintf(sq_buffer_alloc(buffer, len), "%ld", src->value.int64);
#else
		len = snprintf(NULL, 0, "%lld", (long long int)src->value.int64);
		sprintf(sq_buffer_alloc(buffer, len), "%lld", (long long int)src->value.int64);
#endif
		break;

	case SQXC_TYPE_UINT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "%I64u", src->value.uint64);
		sprintf(sq_buffer_alloc(buffer, len), "%I64u", src->value.uint64);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		len = snprintf(NULL, 0, "%lu", src->value.uint64);
		sprintf(sq_buffer_alloc(buffer, len), "%lu", src->value.uint64);
#else
		len = snprintf(NULL, 0, "%llu", (long long unsigned int)src->value.uint64);
		sprintf(sq_buffer_alloc(buffer, len), "%llu", (long long unsigned int)src->value.uint64);
#endif
		break;

	case SQXC_TYPE_TIME:
		tempstr = sq_time_to_string(src->value.rawtime, 0);
		sq_buffer_write_c(buffer, '\'');
		sq_buffer_write(buffer, tempstr);
		sq_buffer_write_c(buffer, '\'');
		free(tempstr);
		break;

	case SQXC_TYPE_DOUBLE:
		len = snprintf(NULL, 0, "%lf", src->value.double_);
		sprintf(sq_buffer_alloc(buffer, len), "%lf", src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		if (src->value.string == NULL) {
			sq_buffer_write(buffer, "NULL");
			break;
		}
		// handle SQL string apostrophe (single quotes)
		for (len = 0, idx = 0;  src->value.string[idx];  idx++, len++) {
			// double up on the single quotes
			if (src->value.string[idx] == '\'')
				len++;
		}
		idx = buffer->writed;
		sq_buffer_alloc(buffer, len +2);    // + '\'' x 2
		buffer->mem[idx++] = '\'';
		buffer->mem[idx +len] = '\'';
		for (len = idx, idx = 0;  src->value.string[idx];  idx++, len++) {
			// double up on the single quotes
			if (src->value.string[idx] == '\'')
				buffer->mem[len++] = '\'';
			buffer->mem[len] = src->value.string[idx];
		}
		break;

	default:
		return (src->code = SQCODE_TYPE_NOT_SUPPORT);
	}

	return (src->code = SQCODE_OK);
}

// ----------------------------------------------------------------------------
// SqxcInfo

const SqxcInfo sqxc_info_sql =
{
	sizeof(SqxcSql),
	(SqInitFunc)sqxc_sql_init,
	(SqFinalFunc)sqxc_sql_final,
	(SqxcCtrlFunc)sqxc_sql_ctrl,
	(SqxcSendFunc)sqxc_sql_send,
};

const SqxcInfo *SQXC_INFO_SQL = &sqxc_info_sql;
