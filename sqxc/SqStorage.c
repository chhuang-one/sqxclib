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

#include <SQLite3.h>

#include <SqError.h>
#include <SqQuery.h>
#include <SqxcValue.h>
#include <SqxcSqlite.h>
#include <SqStorage.h>

static char*  get_primary_key_string(void* instance, SqTable* type);

SqStorage*  sq_storage_global_ = NULL;

SqStorage* sq_storage_new()
{
	SqStorage* db;

	db = malloc(sizeof(SqStorage));
	db->schema = sq_schema_new("default");

	db->container_default = SQ_TYPE_PTR_ARRAY;
	// SqxcInfo
	db->xcinfo.sql   = SQXC_INFO_SQLITE;
	db->xcinfo.value = SQXC_INFO_VALUE;
	// Sqxc - Output
	db->xc[0].src = sqxc_new_output(SQXC_INFO_VALUE, SQXC_INFO_SQLITE);
	db->xc[0].sql = (SqxcSqlite*)sqxc_get(db->xc[0].src, SQXC_INFO_SQLITE, 0);
	db->xc[0].value = (SqxcValue*)db->xc[0].src;
	// Sqxc - Input
	db->xc[1].src = sqxc_new_input(SQXC_INFO_SQLITE, SQXC_INFO_VALUE);
	db->xc[1].sql = (SqxcSqlite*)db->xc[1].src;
	db->xc[1].value = (SqxcValue*)sqxc_get(db->xc[1].src, SQXC_INFO_VALUE, 0);

	return db;
}

void  sq_storage_free(SqStorage* storage)
{
//	sq_type_free(storage->container_default);
	free(storage);
}

SqQuery* sq_storage_table(SqStorage* db, const char* table_name)
{
	SqTable* table;

	table = sq_schema_find(db->schema, table_name);
	if (table)
		return sq_query_new(table);
	else
		return NULL;
//	sq_entry_free(db->input_container_default);
}

SqQuery* sq_storage_type(SqStorage* db, const char* type_name)
{
	SqTable* table;

	table = sq_schema_find_type(db->schema, type_name);
	if (table)
		return sq_query_new(table);
	else
		return NULL;
}

int   sq_storage_open(SqStorage* storage, const char* path_or_name)
{
	sqlite3* db = storage->db;
	int      rc;

	if (db)
		return 0;
	rc = sqlite3_open(path_or_name, &db);
	if(rc != SQLITE_OK)
		return 0;
	storage->db = db;
	storage->xc[1].sql->db = db;
	return 1;
}

void* sq_storage_get(SqStorage* storage,
                     const char* table_name,
                     const char* type_name, int id)
{
	SqColumn* column;
	SqTable*  table;
	Sqxc*     xcsql;
	Sqxc*     xcvalue;
	void*     instance;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);
	if (table == NULL)
		return NULL;

	// destination of input
	xcvalue = (Sqxc*)storage->xc[1].value;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = NULL;

	column = sq_table_get_primary(table);
	// source of input
	xcsql = (Sqxc*)storage->xc[1].sql;
	xcsql->buf_writed = snprintf(NULL, 0, "SELECT * FROM \"%s\" WHERE \"%s\"=%d",
	                             table->name, column->name, id) +1;
	if (xcsql->buf_size < xcsql->buf_writed) {
		xcsql->buf_size = xcsql->buf_writed;
		xcsql->buf = realloc(xcsql->buf, xcsql->buf_size);
	}
	snprintf(xcsql->buf, xcsql->buf_size, "SELECT * FROM \"%s\" WHERE \"%s\"=%d",
	         table->name, column->name, id);

	sqxc_ready(xcsql, NULL);
	xcsql->type  = SQXC_TYPE_OBJECT;
	xcsql->name  = xcsql->buf;
	xcsql->value.string = xcsql->buf;
	xcsql->code = xcsql->send(xcsql, xcsql);
	sqxc_finish(xcsql, NULL);
	if (xcsql->code != SQCODE_OK)
		return NULL;

	instance = sqxc_value_instance(xcvalue);
	return instance;
}

void* sq_storage_get_all(SqStorage* storage,
                         const char* table_name,
                         const char* type_name,
                         SqType* container)
{
	SqTable* table;
	Sqxc*    xcsql;
	Sqxc*    xcvalue;
	void*    instance;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);
	if (table == NULL)
		return NULL;
	if (container == NULL)
		container = (SqType*)storage->container_default;

	// destination of input
	xcvalue = (Sqxc*) storage->xc[1].value;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = container;

	// source of input
	xcsql = (Sqxc*)storage->xc[1].sql;
	xcsql->buf_writed = snprintf(NULL, 0, "SELECT * FROM \"%s\"", table->name) +1;
	if (xcsql->buf_size < xcsql->buf_writed) {
		xcsql->buf_size = xcsql->buf_writed;
		xcsql->buf = realloc(xcsql->buf, xcsql->buf_size);
	}
	snprintf(xcsql->buf, xcsql->buf_size, "SELECT * FROM \"%s\"", table->name);

	sqxc_ready(xcsql, NULL);
	xcsql->type  = SQXC_TYPE_ARRAY;
	xcsql->name  = xcsql->buf;
	xcsql->value.string = xcsql->buf;
	xcsql->code = xcsql->send(xcsql, xcsql);
	sqxc_finish(xcsql, NULL);
	if (xcsql->code != SQCODE_OK)
		return NULL;

	instance = sqxc_value_instance(xcvalue);
	return instance;
}

int   sq_storage_insert(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance)
{
	Sqxc*      xcsql;
	Sqxc*      xcvalue;
	SqTable*   table;
	int        id = -1;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);

	// destination of output
	xcsql = (Sqxc*)storage->xc[0].sql;
	sqxc_sqlite_db(xcsql) = storage->db;
	xcsql->ctrl(xcsql, SQXC_SQL_USE_INSERT, table);

	// source of output
	xcvalue = (Sqxc*)storage->xc[0].value;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcvalue, NULL);
	xcvalue->type  = SQXC_TYPE_OBJECT;
	xcvalue->name  = NULL;
	xcvalue->value.pointer = instance;
	xcvalue->code = xcvalue->send(xcvalue, xcvalue);
	sqxc_finish(xcvalue, NULL);

	return (id = sqxc_sqlite_id(xcsql));
}

void  sq_storage_update(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance)
{
	Sqxc*      xcsql;
	Sqxc*      xcvalue;
	SqTable*   table;
	char*      where;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);

	where = get_primary_key_string(instance, table);
	if (where == NULL)
		return;

	// destination of output
	xcsql = (Sqxc*)storage->xc[0].sql;
	xcsql->ctrl(xcsql, SQXC_SQL_USE_UPDATE, table);
	xcsql->ctrl(xcsql, SQXC_SQL_USE_WHERE, where);
	sqxc_sqlite_db(xcsql) = storage->db;
	free(where);

	// source of output
	xcvalue = (Sqxc*)storage->xc[0].value;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcvalue, NULL);
	xcvalue->type  = SQXC_TYPE_OBJECT;
	xcvalue->name  = NULL;
	xcvalue->value.pointer = instance;
	xcvalue->code = xcvalue->send(xcvalue, xcvalue);
	sqxc_finish(xcvalue, NULL);
}

void  sq_storage_remove(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        int   id)
{
	Sqxc*      xcsql;
	SqTable*   table;
	SqColumn*  column;
	int    rc;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);

	column = sq_table_get_primary(table);

	xcsql = (Sqxc*)storage->xc[0].sql;
	xcsql->buf_writed = snprintf(NULL, 0, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	                             table->name, column->name, id) + 1;
	if (xcsql->buf_size < xcsql->buf_writed) {
		xcsql->buf_size = xcsql->buf_writed;
		xcsql->buf = realloc(xcsql->buf, xcsql->buf_size);
	}
	snprintf(xcsql->buf, xcsql->buf_size, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	         table->name, column->name, id);

	rc = sqlite3_exec(storage->db, xcsql->buf,
	                  NULL, xcsql,
	                  NULL);
}

// ----------------------------------------------------------------------------
// static function

static char*  get_primary_key_string(void* instance, SqTable* table)
{
	SqColumn*   column;
	char*       condition = NULL;
	int         len;
	int         intval;

	column = sq_table_get_primary(table);

	// integer
	instance = (char*)instance + column->offset;
	if (column->type == SQ_TYPE_INT64) {
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "\"%s\"=%I64d", column->name, *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "\"%s\"=%I64d", column->name, *(int64_t*)instance);
#else
		len = snprintf(NULL, 0, "\"%s\"=%lld", column->name, *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "\"%s\"=%lld", column->name, *(int64_t*)instance);
#endif
	}
	else {
		if (column->type == SQ_TYPE_INT)
			intval = *(int*)instance;
		else if (column->type == SQ_TYPE_INTPTR)
			intval = *(intptr_t*)instance;
		else
			return NULL;

		len = snprintf(NULL, 0, "\"%s\"=%d", column->name, intval) +1;
		condition = malloc(len);
		snprintf(condition, len, "\"%s\"=%d", column->name, intval);
	}

	return condition;
}
