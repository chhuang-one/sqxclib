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

#if defined(_MSC_VER)
#ifdef _WIN64
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif

#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif  // _MSC_VER

#include <limits.h>     // __WORDSIZE
#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqBuffer.h>
#include <SqTable.h>
#include <SqxcSql.h>

/*
	Because SQL table == C struct (object),
	SqxcSql must "support" SQXC_TYPE_OBJECT at startup.
	             unsupport SQXC_TYPE_OBJECT while parsing/writing column.

	multiple row == array of object
	SqxcSql must "support" SQXC_TYPE_ARRAY at startup.
	             unsupport SQXC_TYPE_ARRAY while parsing/writing object.
 */

static void sqxc_sql_use_insert_command(SqxcSql* xcsql, SqTable* table);
static void sqxc_sql_use_update_command(SqxcSql* xcsql, SqTable* table);
static void sqxc_sql_use_where_condition(SqxcSql* xcsql, const char *condition);
static int  sqxc_sql_write_value(SqxcSql* xcsql, Sqxc* src);

/* ----------------------------------------------------------------------------
	SqxcInfo functions - destination of output chain

	SQXC_TYPE_xxx ---> SqxcSql ---> Sqdb.exec()
 */

static int  sqxc_sql_send_insert_command(SqxcSql* xcsql, Sqxc* src)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);
	SqEntry*  entry;

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
			sq_buffer_write_c(buffer, ',');
		sq_buffer_write_c(buffer, '(');
		xcsql->row_count++;
		xcsql->col_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT_END:
		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_OBJECT;
		xcsql->supported_type |= SQXC_TYPE_OBJECT;
		// --- End of row ---
		sq_buffer_write_c(buffer, ')');
		if ((xcsql->outer_type & SQXC_TYPE_ARRAY) == 0) {
			sq_buffer_write_c(buffer, 0);    // null-terminated
			// SQL statement has written in xcsql->buf
		}
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_ARRAY_END:
		if ((xcsql->outer_type & SQXC_TYPE_ARRAY) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_ARRAY;
		xcsql->supported_type |= SQXC_TYPE_ARRAY;
		// --- End of Array ---
		sq_buffer_write_c(buffer, 0);    // null-terminated
		// SQL statement has written in xcsql->buf
		return (src->code = SQCODE_OK);

	default:
		break;
	}

	// Don't output auto increment
	entry = src->entry;
	if (entry && entry->bit_field & SQB_INCREMENT && SQ_TYPE_NOT_INT(entry->type))
		return (src->code = SQCODE_OK);

	// SQL statement multiple columns
	if (xcsql->col_count)
		sq_buffer_write_c(buffer, ',');
	// value
	if (sqxc_sql_write_value(xcsql, src) != SQCODE_OK) {
		if (xcsql->col_count)
			buffer->writed--;
	}

	xcsql->col_count++;
	return src->code;
}

static int  sqxc_sql_send_update_command(SqxcSql* xcsql, Sqxc* src)
{
	SqBuffer*  buffer = sqxc_get_buffer(xcsql);
	SqEntry*   entry;
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
		sqxc_sql_use_where_condition(xcsql, xcsql->condition);
		sq_buffer_write_c(buffer, 0);    // null-terminated
		return (src->code = SQCODE_OK);

	default:
		break;
	}

	// Don't output primary key
	entry = src->entry;
	if (entry && entry->bit_field & SQB_PRIMARY) {
		// get primary key id if possible
		if (SQ_TYPE_IS_INT(entry->type) && xcsql->id == -1)
			xcsql->id = src->value.integer;
		return (src->code = SQCODE_OK);
	}

	// SQL statement multiple columns
	if (xcsql->col_count)
		sq_buffer_write_c(buffer, ',');
	// "name"=value
	len = snprintf(NULL, 0, "\"%s\"=", src->name);
	sprintf(sq_buffer_alloc(buffer, len), "\"%s\"=", src->name);
	if (sqxc_sql_write_value(xcsql, src) != SQCODE_OK) {
		buffer->writed -= len;
		if (xcsql->col_count)
			buffer->writed--;
	}

	xcsql->col_count++;
	return src->code;
}

static int  sqxc_sql_send(SqxcSql* xcsql, Sqxc* src)
{
	// 1 == INSERT, 0 == UPDATE
	if (xcsql->mode == 1)
		return sqxc_sql_send_insert_command(xcsql, src);
	else
		return sqxc_sql_send_update_command(xcsql, src);
}

static int  sqxc_sql_ctrl(SqxcSql* xcsql, int id, void* data)
{
	int  code;

	switch (id) {
	case SQXC_CTRL_READY:
		xcsql->supported_type = SQXC_TYPE_ALL;
		xcsql->outer_type = SQXC_TYPE_NONE;
		break;

	case SQXC_CTRL_FINISH:
		free(xcsql->condition);
		xcsql->condition = NULL;
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

	case SQXC_SQL_USE_INSERT:
		xcsql->mode = 1;
		xcsql->row_count = 0;
//		xcsql->col_count = 0;
		sqxc_sql_use_insert_command(xcsql, (SqTable*)data);
		break;

	case SQXC_SQL_USE_UPDATE:
		xcsql->mode = 0;
//		xcsql->row_count = 0;
		xcsql->col_count = 0;
		sqxc_sql_use_update_command(xcsql, (SqTable*)data);
		break;

	case SQXC_SQL_USE_WHERE:
		free(xcsql->condition);
		if (data)
			xcsql->condition = strdup((char*)data);
		else
			xcsql->condition = NULL;
		break;

	default:
		return (xcsql->code = SQCODE_NOT_SUPPORT);
	}

	return SQCODE_OK;
}

static void  sqxc_sql_init(SqxcSql* xcsql)
{
//	memset(xcsql, 0, sizeof(SqxcSql));
	xcsql->supported_type  = SQXC_TYPE_ALL;
	xcsql->outer_type = SQXC_TYPE_NONE;
	xcsql->id = -1;
}

static void  sqxc_sql_final(SqxcSql* xcsql)
{

}

// ----------------------------------------------------------------------------
// others functions

static void sqxc_sql_use_insert_command(SqxcSql* xcsql, SqTable* table)
{
	SqEntry*   entry;
	SqBuffer*  buffer;
	int        index;
	int        index_beg = 0;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// "INSERT INTO 'table_name' ('column1','column2') OUTPUT Inserted.id VALUES "
	sq_buffer_write(buffer, "INSERT INTO \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" (");
	for (index = 0;  index < table->type->n_entry;  index++) {
		entry = table->type->entry[index];
		// Don't output anything if column is auto increment.
		if (entry->bit_field & SQB_INCREMENT && SQ_TYPE_NOT_INT(entry->type)) {
			index_beg++;
			continue;
		}
		if (index > index_beg)
			sq_buffer_write_c(buffer, ',');
		sq_buffer_write_c(buffer, '"');
		sq_buffer_write(buffer, entry->name);
		sq_buffer_write_c(buffer, '"');
	}
	sq_buffer_write(buffer, ") VALUES ");
	// reuse after running Sqdb.exec() if buffer size too large
	xcsql->buf_reuse = xcsql->buf_writed;
}

static void sqxc_sql_use_update_command(SqxcSql* xcsql, SqTable* table)
{
	SqBuffer*  buffer;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// "UPDATE 'table_name' SET "
	sq_buffer_write(buffer, "UPDATE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" SET ");

	xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
	// reuse after running Sqdb.exec()
	xcsql->buf_reuse = xcsql->buf_writed;
}

static void sqxc_sql_use_where_condition(SqxcSql* xcsql, const char *condition)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);
	int       len;

	// UPDATE (mode == 0)
	if (xcsql->mode == 0) {
		// WHERE condition
		sq_buffer_write_n(buffer, " WHERE ", 7);
		if (xcsql->condition)
			sq_buffer_write(buffer, condition);
		else {
			len = snprintf(NULL, 0, "id=%d", xcsql->id);
			sprintf(sq_buffer_alloc(buffer, len), "id=%d", xcsql->id);
		}
	}
}

static int  sqxc_sql_write_value(SqxcSql* xcsql, Sqxc* src)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);
	int       len, idx;

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
#elif defined(__WORDSIZE) && (__WORDSIZE == 64)
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
#elif defined(__WORDSIZE) && (__WORDSIZE == 64)
		len = snprintf(NULL, 0, "%lu", src->value.uint64);
		sprintf(sq_buffer_alloc(buffer, len), "%lu", src->value.uint64);
#else
		len = snprintf(NULL, 0, "%llu", (long long unsigned int)src->value.uint64);
		sprintf(sq_buffer_alloc(buffer, len), "%llu", (long long unsigned int)src->value.uint64);
#endif
		break;

	case SQXC_TYPE_DOUBLE:
		len = snprintf(NULL, 0, "%lf", src->value.double_);
		sprintf(sq_buffer_alloc(buffer, len), "%lf", src->value.double_);
		break;

	case SQXC_TYPE_STRING:
		// handle SQL string apostrophe (single quotes)
		for (len = 0, idx = 0;  src->value.string[idx];  idx++, len++) {
			// double up on the single quotes
			if (src->value.string[idx] == '\'')
				len++;
		}
		idx = buffer->writed;
		sq_buffer_alloc(buffer, len +2);    // + '\'' x 2
		buffer->buf[idx++] = '\'';
		buffer->buf[idx +len] = '\'';
		for (len = idx, idx = 0;  src->value.string[idx];  idx++, len++) {
			// double up on the single quotes
			if (src->value.string[idx] == '\'')
				buffer->buf[len++] = '\'';
			buffer->buf[len] = src->value.string[idx];
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
