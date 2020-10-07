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
#include <SqxcSql.h>
#include <SqxcValue.h>
#include <SqxcJsonc.h>
#include <SqStorage.h>

static char*  get_primary_key_string(void* instance, SqTable* type);

SqStorage* sq_storage_new(Sqdb* db)
{
	SqStorage* storage;

	storage = malloc(sizeof(SqStorage));
	storage->db = db;
	storage->schema = sq_schema_new("default");
//	storage->types_sorted = false;
//	sq_ptr_array_init(&storage->types, 16, NULL);

	storage->container_default = SQ_TYPE_PTR_ARRAY;
#if 1
	storage->xc_input = sqxc_new(SQXC_INFO_VALUE);
	storage->xc_output = sqxc_new(SQXC_INFO_SQL);
#else
	// add JSON support
	storage->xc_input = sqxc_new_chain(SQXC_INFO_VALUE, SQXC_INFO_JSONC_PARSER, NULL);
	storage->xc_output = sqxc_new_chain(SQXC_INFO_SQL, SQXC_INFO_JSONC_WRITER, NULL);
#endif

	return storage;
}

void  sq_storage_free(SqStorage* storage)
{
//	sq_type_free(storage->container_default);
	sq_schema_free(storage->schema);
//	sq_ptr_array_final(&storage->types);

	sqxc_free_chain(storage->xc_input);
	sqxc_free_chain(storage->xc_output);
	free(storage);
}

SqQuery* sq_storage_table(SqStorage* storage, const char *table_name)
{
	SqTable* table;

	table = sq_schema_find(storage->schema, table_name);
	if (table)
		return sq_query_new(table);
	else
		return NULL;
//	sq_entry_free(db->input_container_default);
}

SqQuery* sq_storage_type(SqStorage* storage, const char *type_name)
{
	SqTable* table;

	table = sq_schema_find_type(storage->schema, type_name);
	if (table)
		return sq_query_new(table);
	else
		return NULL;
}

int   sq_storage_open(SqStorage* storage, const char *database_name)
{
	return sqdb_open(storage->db, database_name);
}

int   sq_storage_close(SqStorage* storage)
{
	return sqdb_close(storage->db);
}

void* sq_storage_get(SqStorage* storage,
                     const char *table_name,
                     const char *type_name, int id)
{
	SqBuffer* buf;
	SqColumn* column;
	SqTable*  table;
	Sqxc*     xcvalue;
	void*     instance;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);
	if (table == NULL)
		return NULL;

	// destination of input
	xcvalue = storage->xc_input;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = NULL;

	column = sq_table_get_primary(table);

	// SQL statement
	buf = sqxc_get_buffer(xcvalue);
	buf->writed = snprintf(NULL, 0, "SELECT * FROM \"%s\" WHERE \"%s\"=%d",
	                       table->name, column->name, id) +1;
	sq_buffer_require(buf, buf->writed);
	snprintf(buf->buf, buf->size, "SELECT * FROM \"%s\" WHERE \"%s\"=%d",
	         table->name, column->name, id);

	sqxc_ready(xcvalue, NULL);
	sqdb_exec(storage->db, buf->buf, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	instance = sqxc_value_instance(xcvalue);
	return instance;
}

void* sq_storage_get_all(SqStorage* storage,
                         const char *table_name,
                         const char *type_name,
                         SqType* container)
{
	SqBuffer* buf;
	SqTable*  table;
	Sqxc*     xcvalue;
	void*     instance;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);
	if (table == NULL)
		return NULL;
	if (container == NULL)
		container = (SqType*)storage->container_default;

	// destination of input
	xcvalue = (Sqxc*) storage->xc_input;
	sqxc_value_type(xcvalue) = table->type;
	sqxc_value_container(xcvalue) = container;

	// SQL statement
	buf = sqxc_get_buffer(xcvalue);
	buf->writed = snprintf(NULL, 0, "SELECT * FROM \"%s\"", table->name) +1;
	sq_buffer_require(buf, buf->writed);
	snprintf(buf->buf, buf->size, "SELECT * FROM \"%s\"", table->name);

	sqxc_ready(xcvalue, NULL);
	sqdb_exec(storage->db, buf->buf, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	instance = sqxc_value_instance(xcvalue);
	return instance;
}

int   sq_storage_insert(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance)
{
	Sqxc*      xcsql;
	SqTable*   table;
	int        id = -1;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);

	// destination of output
	xcsql = storage->xc_output;
	sqxc_sql_db(xcsql) = storage->db;
	xcsql->info->ctrl(xcsql, SQXC_SQL_USE_INSERT, table);

	sqxc_ready(xcsql, NULL);
	table->type->write(instance, table->type, xcsql);
	sqxc_finish(xcsql, NULL);

	return (id = sqxc_sql_id(xcsql));
}

void  sq_storage_update(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        void* instance)
{
	Sqxc*      xcsql;
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
	xcsql = storage->xc_output;
	xcsql->info->ctrl(xcsql, SQXC_SQL_USE_UPDATE, table);
	xcsql->info->ctrl(xcsql, SQXC_SQL_USE_WHERE, where);
	sqxc_sql_db(xcsql) = storage->db;
	free(where);

	sqxc_ready(xcsql, NULL);
	table->type->write(instance, table->type, xcsql);
	sqxc_finish(xcsql, NULL);
}

void  sq_storage_remove(SqStorage* storage,
                        const char* table_name,
                        const char* type_name,
                        int   id)
{
	SqBuffer*  buf;
	SqTable*   table;
	SqColumn*  column;
//	int    code;

	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = sq_schema_find_type(storage->schema, type_name);

	column = sq_table_get_primary(table);

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = snprintf(NULL, 0, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	                       table->name, column->name, id) + 1;
	sq_buffer_require(buf, buf->writed);
	snprintf(buf->buf, buf->size, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	         table->name, column->name, id);
	sqdb_exec(storage->db, buf->buf, NULL, NULL);
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
