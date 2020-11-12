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

static SqTable* find_table_by_type_name(SqStorage* storage, const char *type_name);
static char*    get_primary_key_string(void* instance, SqTable* type);

void  sq_storage_init(SqStorage* storage, Sqdb* db)
{
	storage->db = db;
	storage->schema = sq_schema_new("current");
	storage->schema->version = 0;
	storage->tables_version = 0;
	sq_ptr_array_init(&storage->tables, 16, NULL);

	storage->container_default = SQ_TYPE_PTR_ARRAY;

	storage->xc_input = sqxc_new(SQXC_INFO_VALUE);
	storage->xc_output = sqxc_new(SQXC_INFO_SQL);

#ifdef SQ_CONFIG_JSON_SUPPORT
	sqxc_insert(storage->xc_input,  sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
	sqxc_insert(storage->xc_output, sqxc_new(SQXC_INFO_JSONC_WRITER), -1);
#endif
}

void  sq_storage_final(SqStorage* storage)
{
//	sq_type_free(storage->container_default);
	sq_schema_free(storage->schema);
//	sq_ptr_array_final(&storage->types);

	sqxc_free_chain(storage->xc_input);
	sqxc_free_chain(storage->xc_output);
}

SqStorage* sq_storage_new(Sqdb* db)
{
	SqStorage* storage;

	storage = malloc(sizeof(SqStorage));
	sq_storage_init(storage, db);

	return storage;
}

void  sq_storage_free(SqStorage* storage)
{
	sq_storage_final(storage);
	free(storage);
}

int   sq_storage_open(SqStorage* storage, const char *database_name)
{
	return sqdb_open(storage->db, database_name);
}

int   sq_storage_close(SqStorage* storage)
{
	return sqdb_close(storage->db);
}

int   sq_storage_migrate(SqStorage* storage, SqSchema* schema)
{
	return sqdb_migrate(storage->db, storage->schema, schema);
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

	// find SqTable by table_name or type_name
	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = find_table_by_type_name(storage, type_name);
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
                         const SqType *container)
{
	SqBuffer* buf;
	SqTable*  table;
	Sqxc*     xcvalue;
	void*     instance;

	// find SqTable by table_name or type_name
	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = find_table_by_type_name(storage, type_name);
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

	// find SqTable by table_name or type_name
	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = find_table_by_type_name(storage, type_name);
	if (table == NULL)
		return -1;

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

	// find SqTable by table_name or type_name
	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = find_table_by_type_name(storage, type_name);
	if (table == NULL)
		return;

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
//	int        code;

	// find SqTable by table_name or type_name
	if (table_name)
		table = sq_schema_find(storage->schema, table_name);
	else
		table = find_table_by_type_name(storage, type_name);
	if (table == NULL)
		return;

	column = sq_table_get_primary(table);

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = snprintf(NULL, 0, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	                       table->name, column->name, id) + 1;
	sq_buffer_require(buf, buf->writed);
	snprintf(buf->buf, buf->size, "DELETE FROM \"%s\" WHERE \"%s\"=%d",
	         table->name, column->name, id);
	sqdb_exec(storage->db, buf->buf, NULL, NULL);
}

// ------------------------------------

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

	table = find_table_by_type_name(storage, type_name);
	if (table)
		return sq_query_new(table);
	else
		return NULL;
}

// ----------------------------------------------------------------------------
// static function

static SqTable*  find_table_by_type_name(SqStorage* storage, const char *type_name)
{
	SqPtrArray* tables = &storage->tables;
	SqPtrArray* schema_tables = sq_type_get_ptr_array(storage->schema->type);
	SqTable**   table_addr;
	int         count;

	// if version is not the same
	if (storage->tables_version != storage->schema->version) {
		storage->tables_version  = storage->schema->version;
		count = schema_tables->length - tables->length;
		if (count > 0)
			sq_ptr_array_alloc(tables, count);
		memcpy(tables->data, schema_tables->data, sizeof(void*) * schema_tables->length);
		// sort storage->tables by SqTable.type.name
		sq_ptr_array_sort(tables, sq_entry_cmp_type_name);
	}
	// search storage->tables by SqTable.type.name
	table_addr = sq_ptr_array_search(tables, type_name,
	             (SqCompareFunc)sq_entry_cmp_str__type_name);
	if (table_addr)
		return *table_addr;
	return NULL;
}

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
