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
#define _CRT_SECURE_NO_WARNINGS
#define snprintf	_snprintf
#endif

#include <stdio.h>      // snprintf

#include <SqError.h>
#include <SqBuffer.h>
#include <SqTable.h>
#include <SqxcSqlite.h>

/*
	Because SQL table == C struct (object),
	SqxcSqlite must "support" SQXC_TYPE_OBJECT at startup.
	                unsupport SQXC_TYPE_OBJECT while parsing/writing column.

	multiple row == array of object
	SqxcSqlite must "support" SQXC_TYPE_ARRAY at startup.
	                unsupport SQXC_TYPE_ARRAY while parsing/writing object.
 */

/* ----------------------------------------------------------------------------
   source of input chain

	SQL command ---> SqxcSqlit ---> SQXC_TYPE_xxxx
	            send           send
 */

static int sqxc_sqlite_callback_in(void *user_data, int argc, char **argv, char **columnName)
{
	SqxcSqlite* xcsql = user_data;
	int   index;

	if (xcsql->supported_type & SQXC_TYPE_ARRAY) {
		sqxc_send_array_beg(xcsql, xcsql->table_name);
		// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
		if (xcsql->code != SQCODE_OK)
			return 1;
		// call sqxc_send_array_end() in sqxc_sqlite_send_in()
		xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
		xcsql->outer_type |= SQXC_TYPE_ARRAY;
	}

	sqxc_send_object_beg(xcsql, xcsql->row_name);
	xcsql->supported_type &= ~(SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY);
	// returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT
	if (xcsql->code != SQCODE_OK)
		return 1;
	// columns
	for (index = 0;  index < argc;  index++) {
		sqxc_send_string(xcsql, columnName[index], argv[index]);
		if (xcsql->code != SQCODE_OK)
			break;
	}
	sqxc_send_object_end(xcsql, xcsql->row_name);
	xcsql->supported_type |= SQXC_TYPE_OBJECT;

	return 0;
}

static int  sqxc_sqlite_send_in(SqxcSqlite* xcsql, Sqxc* src)
{
	int    rc = SQLITE_OK;
/*
	// SQXC_TYPE_OBJECT_END or SQXC_TYPE_ARRAY_END
	if (src->type & SQXC_TYPE_END)
		return (src->code = SQCODE_OK);
	// SQXC_TYPE_OBJECT or SQXC_TYPE_ARRAY
	if (src->type != SQXC_TYPE_OBJECT && src->type != SQXC_TYPE_ARRAY) {
	    // set required type in dest->type
		dest->type = SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY;
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}
 */

	if (src->type == SQXC_TYPE_OBJECT)
		xcsql->supported_type &= ~SQXC_TYPE_ARRAY;

	if (src->name == NULL) {
		// src->value.pointer = prepared statements
	}
	else // if (src->name == src->value)
	{
		// src->value.string = SQL statements
		rc = sqlite3_exec(xcsql->db, src->value.string,
		                  sqxc_sqlite_callback_in,
		                  xcsql, &xcsql->errorMsg);
		// call sqxc_send_array_beg() in sqxc_sqlite_callback_in()
		if (xcsql->outer_type & SQXC_TYPE_ARRAY)
			sqxc_send_array_end(src, xcsql->table_name);
	}

	xcsql->supported_type |= SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY;
	xcsql->outer_type = SQXC_TYPE_NONE;
	if (rc != SQLITE_OK)
		return (src->code = SQCODE_EXEC_ERROR);
	return (src->code = SQCODE_OK);
}

static void sqxc_sqlite_set_select_command(SqxcSqlite* xcsql, SqTable* table)
{
	SqBuffer* buffer;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// SELECT * FROM 'table_name'
	sq_buffer_write(buffer, "SELECT * FROM \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write_c(buffer, '"');

	if (xcsql->where_condition) {
		// WHERE condition
		sq_buffer_write(buffer, " WHERE ");
		sq_buffer_write(buffer, xcsql->where_condition);
	}
}

static void sqxc_sqlite_set_where_command(SqxcSqlite* xcsql, char* data)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);

	if (data &&
	    ((xcsql->where_condition == data) ||
		 (xcsql->send == (SqxcSendFunc)sqxc_sqlite_send_in && buffer->writed > 0)) )
	{
		// WHERE condition
		sq_buffer_write_n(buffer, " WHERE ", 7);
		sq_buffer_write(buffer, data);
		data = NULL;
	}
	free(xcsql->where_condition);
	xcsql->where_condition = (data) ? strdup(data) : NULL;
}

static int  sqxc_sqlite_ctrl_in(SqxcSqlite* xcsql, int id, void* data)
{
	switch(id) {
	case SQXC_CTRL_READY:
		xcsql->supported_type = SQXC_TYPE_ALL;
		xcsql->outer_type = SQXC_TYPE_NONE;
		break;

	case SQXC_CTRL_FINISH:
		free(xcsql->where_condition);
		xcsql->where_condition = NULL;
		break;

	case SQXC_SQL_USE_SELECT:
		sqxc_sqlite_set_select_command(xcsql, data);
		break;

	case SQXC_SQL_USE_WHERE:
		sqxc_sqlite_set_where_command(xcsql, data);
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}

	return SQCODE_OK;
}

static void  sqxc_sqlite_init_in(SqxcSqlite* xcsql)
{
//	memset(xcsql, 0, sizeof(SqxcSqlite));
	xcsql->ctrl = (SqxcCtrlFunc)sqxc_sqlite_ctrl_in;
	xcsql->send = (SqxcSendFunc)sqxc_sqlite_send_in;
	xcsql->supported_type = SQXC_TYPE_ALL;
	xcsql->outer_type = SQXC_TYPE_NONE;
	xcsql->row_name = "row";
}

static void  sqxc_sqlite_final_in(SqxcSqlite* xcsql)
{

}

/* ----------------------------------------------------------------------------
   destination of output chain

	SQXC_TYPE_xxxx ---> SqxcSqlite ---> SQL command
	               send            send
 */

static int sqxc_sqlite_callback_out(void *user_data, int argc, char **argv, char **columnName)
{
	SqxcSqlite* xcsql = user_data;
	int   index;

	for (index = 0;  index < argc;  index++) {
		xcsql->id = strtol(argv[index], NULL, 10);
	}

	return 0;
}

static int  sqxc_sqlite_write_buffer(SqxcSqlite* xcsql, Sqxc* src)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);
	int       len, idx;

	switch (src->type) {
//	case SQXC_TYPE_BOOL:
//		// TODO: write boolean
//		break;

	case SQXC_TYPE_INT:
		len = snprintf(NULL, 0, "%d", src->value.integer);
		sprintf(sq_buffer_alloc(buffer, len), "%d", src->value.integer);
		break;

	case SQXC_TYPE_INT64:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "%I64d", src->value.int64);
		sprintf(sq_buffer_alloc(buffer, len), "%I64d", src->value.int64);
#else
		len = snprintf(NULL, 0, "%lld", src->value.int64);
		sprintf(sq_buffer_alloc(buffer, len), "%lld", src->value.int64);
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

static int  sqxc_sqlite_send_insert_command(SqxcSqlite* xcsql, Sqxc* src)
{
	SqBuffer* buffer = sqxc_get_buffer(xcsql);
	SqEntry*  entry;

	// set required type in dest->type if return SQCODE_TYPE_NOT_MATCH to src
	xcsql->type = SQXC_TYPE_ARITHMETIC | SQXC_TYPE_STRING;

	switch (src->type) {
	case SQXC_TYPE_ARRAY:
		if (xcsql->outer_type & SQXC_TYPE_ARRAY) {
			if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
				xcsql->type = SQXC_TYPE_OBJECT;    // set required type in dest->type
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		xcsql->outer_type |= SQXC_TYPE_ARRAY;
		xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
		// --- Begin of Array ---
		xcsql->row_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT:
		if (xcsql->outer_type & SQXC_TYPE_OBJECT)
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		xcsql->outer_type |= SQXC_TYPE_OBJECT;
		xcsql->supported_type &= ~(SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY);
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
	if (sqxc_sqlite_write_buffer(xcsql, src) != SQCODE_OK) {
		if (xcsql->col_count)
			buffer->writed--;
	}

	xcsql->col_count++;
	return src->code;
}

static int  sqxc_sqlite_send_update_command(SqxcSqlite* xcsql, Sqxc* src)
{
	SqBuffer*  buffer = sqxc_get_buffer(xcsql);
	SqEntry*   entry;
	int        len;

	// set required type in dest->type if return SQCODE_TYPE_NOT_MATCH to src
	xcsql->type = SQXC_TYPE_ARITHMETIC | SQXC_TYPE_STRING;

	switch (src->type) {
	case SQXC_TYPE_ARRAY:
		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
			xcsql->type = SQXC_TYPE_OBJECT;    // set required type in dest->type
		return (src->code = SQCODE_TYPE_NOT_MATCH);

	case SQXC_TYPE_ARRAY_END:
		return (src->code = SQCODE_TYPE_END_ERROR);

	case SQXC_TYPE_OBJECT:
		if (xcsql->outer_type & SQXC_TYPE_OBJECT)
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		xcsql->outer_type |= SQXC_TYPE_OBJECT;
		xcsql->supported_type &= ~(SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY);
		xcsql->col_count = 0;
		return (src->code = SQCODE_OK);

	case SQXC_TYPE_OBJECT_END:
		if ((xcsql->outer_type & SQXC_TYPE_OBJECT) == 0)
			return (src->code = SQCODE_TYPE_END_ERROR);
		xcsql->outer_type &= ~SQXC_TYPE_OBJECT;
		xcsql->supported_type |= SQXC_TYPE_OBJECT;
		// SQL statement
		sqxc_sqlite_set_where_command(xcsql, xcsql->where_condition);
		sq_buffer_write_c(buffer, 0);    // null-terminated
		// SQL statement has written in xcsql->buf
		return (src->code = SQCODE_OK);

	default:
		break;
	}

	// Don't output primary key
	entry = src->entry;
	if (entry && entry->bit_field & SQB_PRIMARY) {
		return (src->code = SQCODE_OK);
	}

	// SQL statement multiple columns
	if (xcsql->col_count)
		sq_buffer_write_c(buffer, ',');
	// "name"=value
	len = snprintf(NULL, 0, "\"%s\"=", src->name);
	sprintf(sq_buffer_alloc(buffer, len), "\"%s\"=", src->name);
	if (sqxc_sqlite_write_buffer(xcsql, src) != SQCODE_OK) {
		buffer->writed -= len;
		if (xcsql->col_count)
			buffer->writed--;
	}

	xcsql->col_count++;
	return src->code;
}

static void sqxc_sqlite_use_insert_command(SqxcSqlite* xcsql, SqTable* table)
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
}

static void sqxc_sqlite_use_update_command(SqxcSqlite* xcsql, SqTable* table)
{
	SqBuffer*  buffer;

	buffer = sqxc_get_buffer(xcsql);
	buffer->writed = 0;
	// "UPDATE 'table_name' SET "
	sq_buffer_write(buffer, "UPDATE \"");
	sq_buffer_write(buffer, table->name);
	sq_buffer_write(buffer, "\" SET ");

	xcsql->supported_type &= ~SQXC_TYPE_ARRAY;
}

static int  sqxc_sqlite_ctrl_out(SqxcSqlite* xcsql, int id, void* data)
{
	int  rc;

	switch (id) {
	case SQXC_CTRL_READY:
		xcsql->supported_type = SQXC_TYPE_ALL;
		xcsql->outer_type = SQXC_TYPE_NONE;
		xcsql->id = -1;
		break;

	case SQXC_CTRL_FINISH:
		if (xcsql->buf_writed > 0) {
			// SQL statement has written in xcsql->buf
			rc = sqlite3_exec(xcsql->db, xcsql->buf,
			                  sqxc_sqlite_callback_out, xcsql,
			                  &xcsql->errorMsg);
			if (rc != SQLITE_OK)
				return (xcsql->code = SQCODE_EXEC_ERROR);
		}
//		free(xcsql->where_condition);
//		xcsql->where_condition = NULL;
		break;

	case SQXC_SQL_USE_INSERT:
		xcsql->send = (SqxcSendFunc)sqxc_sqlite_send_insert_command;
		xcsql->row_count = 0;
//		xcsql->col_count = 0;
		sqxc_sqlite_use_insert_command(xcsql, data);
		break;

	case SQXC_SQL_USE_UPDATE:
		xcsql->send = (SqxcSendFunc)sqxc_sqlite_send_update_command;
//		xcsql->row_count = 0;
		xcsql->col_count = 0;
		sqxc_sqlite_use_update_command(xcsql, data);
		break;

	case SQXC_SQL_USE_WHERE:
		sqxc_sqlite_set_where_command(xcsql, data);
		break;

	default:
		return (xcsql->code = SQCODE_NOT_SUPPORT);
	}

	return SQCODE_OK;
}

static void  sqxc_sqlite_init_out(SqxcSqlite* xcsql)
{
//	memset(xcsql, 0, sizeof(SqxcSqlite));
	xcsql->ctrl = (SqxcCtrlFunc)sqxc_sqlite_ctrl_out;
	xcsql->send = (SqxcSendFunc)sqxc_sqlite_send_insert_command;
	xcsql->supported_type  = SQXC_TYPE_ALL;
	xcsql->outer_type = SQXC_TYPE_NONE;
}

static void  sqxc_sqlite_final_out(SqxcSqlite* xcsql)
{

}

/* ----------------------------------------------------------------------------
   C to/from SQLite
   SQXC_INFO_SQLITE[0] for Output
   SQXC_INFO_SQLITE[1] for Input
 */
const SqxcInfo SQXC_INFO_SQLITE[2] =
{
	{sizeof(SqxcSqlite), (SqInitFunc)sqxc_sqlite_init_out, (SqFinalFunc)sqxc_sqlite_final_out},
	{sizeof(SqxcSqlite), (SqInitFunc)sqxc_sqlite_init_in,  (SqFinalFunc)sqxc_sqlite_final_in},
};
