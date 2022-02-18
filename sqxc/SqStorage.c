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
#include <SqStorage.h>
#include <SqxcSql.h>
#include <SqxcValue.h>
#ifdef SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

#ifdef _MSC_VER
#ifdef _WIN64
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif  // _WIN64
#define snprintf     _snprintf
#endif  // _MSC_VER

#define STORAGE_SCHEMA_INITIAL_VERSION       0

static char    *get_primary_key_string(void *instance, const SqType *type, const char quote[2]);

void  sq_storage_init(SqStorage *storage, Sqdb *db)
{
	storage->db = db;
	storage->schema = sq_schema_new("current");
	storage->schema->version = STORAGE_SCHEMA_INITIAL_VERSION;
	storage->tables_version  = STORAGE_SCHEMA_INITIAL_VERSION;
	sq_ptr_array_init(&storage->tables, 16, NULL);

	storage->container_default = SQ_TYPE_PTR_ARRAY;

	storage->xc_input  = sqxc_new(SQXC_INFO_VALUE);
	storage->xc_output = sqxc_new(SQXC_INFO_SQL);

#ifdef SQ_CONFIG_HAVE_JSONC
	// append JSON parser/writer to tail of list
	sqxc_insert(storage->xc_input,  sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
	sqxc_insert(storage->xc_output, sqxc_new(SQXC_INFO_JSONC_WRITER), -1);
#endif
}

void  sq_storage_final(SqStorage *storage)
{
//	sq_type_unref(storage->container_default);
	sq_schema_free(storage->schema);
	sq_ptr_array_final(&storage->tables);

	sqxc_free_chain(storage->xc_input);
	sqxc_free_chain(storage->xc_output);
}

SqStorage *sq_storage_new(Sqdb *db)
{
	SqStorage *storage;

	storage = malloc(sizeof(SqStorage));
	sq_storage_init(storage, db);

	return storage;
}

void  sq_storage_free(SqStorage *storage)
{
	sq_storage_final(storage);
	free(storage);
}

int   sq_storage_open(SqStorage *storage, const char *database_name)
{
	return sqdb_open(storage->db, database_name);
}

int   sq_storage_close(SqStorage *storage)
{
	return sqdb_close(storage->db);
}

int   sq_storage_migrate(SqStorage *storage, SqSchema *schema)
{
	return sqdb_migrate(storage->db, storage->schema, schema);
}

void *sq_storage_get(SqStorage    *storage,
                     const char   *table_name,
                     const SqType *table_type,
                     int           id)
{
	SqBuffer *buf;
	Sqxc     *xcvalue;
	union {
		SqColumn *column;
		SqTable  *table;
		void     *instance;
		int       len;
		int       code;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return NULL;
		table_type = temp.table->type;
	}

	// destination of input
	xcvalue = storage->xc_input;
	sqxc_value_element(xcvalue)   = table_type;
	sqxc_value_container(xcvalue) = NULL;
	sqxc_value_instance(xcvalue)  = NULL;

	temp.column = sq_table_get_primary(NULL, table_type);

	// SQL statement
	buf = sqxc_get_buffer(xcvalue);
	buf->writed = 0;
	sqdb_sql_from(storage->db, buf, table_name, false);
	sq_buffer_write(buf, "WHERE");
	sq_buffer_alloc(buf, 2);
	sq_buffer_r_at(buf, 1) = ' ';
	sq_buffer_r_at(buf, 0) = storage->db->info->quote.identifier[0];
	sq_buffer_write(buf, temp.column->name);
	sq_buffer_alloc(buf, 2);
	sq_buffer_r_at(buf, 1) = storage->db->info->quote.identifier[1];
	sq_buffer_r_at(buf, 0) = '=';

	temp.len = snprintf(NULL, 0, "%d", id);
	sprintf(sq_buffer_alloc(buf, temp.len), "%d", id);

	sqxc_ready(xcvalue, NULL);
	temp.code = sqdb_exec(storage->db, buf->mem, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	if (temp.code != SQCODE_OK) {
		storage->xc_input->code = temp.code;
		sq_type_final_instance(table_type, sqxc_value_instance(xcvalue), false);
		free(sqxc_value_instance(xcvalue));
		sqxc_value_instance(xcvalue) = NULL;
		return NULL;
	}
	temp.instance = sqxc_value_instance(xcvalue);
	return temp.instance;
}

void *sq_storage_get_all(SqStorage    *storage,
                         const char   *table_name,
                         const SqType *table_type,
                         const SqType *container_type,
                         const char   *sql_where_having)
{
	Sqxc     *xcvalue;
	union {
		SqBuffer *buf;
		SqTable  *table;
		void     *instance;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return NULL;
		table_type = temp.table->type;
	}
	if (container_type == NULL)
		container_type = (SqType*)storage->container_default;

	// destination of input
	xcvalue = (Sqxc*) storage->xc_input;
	sqxc_value_element(xcvalue)   = table_type;
	sqxc_value_container(xcvalue) = container_type;
	sqxc_value_instance(xcvalue)  = NULL;

	// SQL statement
	temp.buf = sqxc_get_buffer(xcvalue);
	temp.buf->writed = 0;
	sqdb_sql_from(storage->db, temp.buf, table_name, false);

	// SQL WHERE ... HAVING ...
	if (sql_where_having)
		sq_buffer_write(temp.buf, sql_where_having);

	sqxc_ready(xcvalue, NULL);
	sqdb_exec(storage->db, temp.buf->mem, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	temp.instance = sqxc_value_instance(xcvalue);
	return temp.instance;
}

int   sq_storage_insert(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance)
{
	union {
		SqTable   *table;
		Sqxc      *xcsql;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return -1;
		table_type = temp.table->type;
	}

	// destination of output
	temp.xcsql = storage->xc_output;
	sqxc_sql_set_db(temp.xcsql, storage->db);
	sqxc_ctrl(temp.xcsql, SQXC_SQL_CTRL_INSERT, (void*)table_name);

	sqxc_ready(temp.xcsql, NULL);
	table_type->write(instance, table_type, temp.xcsql);
	sqxc_finish(temp.xcsql, NULL);

	return sqxc_sql_id(temp.xcsql);
}

void  sq_storage_update(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance)
{
	char      *where;
	union {
		SqTable   *table;
		Sqxc      *xcsql;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return;
		table_type = temp.table->type;
	}

	where = get_primary_key_string(instance, table_type, storage->db->info->quote.identifier);
	if (where == NULL)
		return;

	// destination of output
	temp.xcsql = storage->xc_output;
	sqxc_sql_set_db(temp.xcsql, storage->db);
	sqxc_ctrl(temp.xcsql, SQXC_SQL_CTRL_UPDATE, table_name);
	sqxc_ctrl(temp.xcsql, SQXC_SQL_CTRL_WHERE,  where);
	free(where);

	sqxc_ready(temp.xcsql, NULL);
	table_type->write(instance, table_type, temp.xcsql);
	sqxc_finish(temp.xcsql, NULL);
}

void  sq_storage_remove(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        int           id)
{
	SqBuffer  *buf;
	int        len;
	union {
		SqTable   *table;
		SqColumn  *column;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return;
		table_type = temp.table->type;
	}

	temp.column = sq_table_get_primary(NULL, table_type);

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = 0;
	sqdb_sql_from(storage->db, buf, table_name, true);
	sq_buffer_write(buf, "WHERE");
	len = snprintf(NULL, 0, " %c%s%c=%d",
	               storage->db->info->quote.identifier[0],
	               temp.column->name,
	               storage->db->info->quote.identifier[1],
	               id);
	sprintf(sq_buffer_alloc(buf, len), " %c%s%c=%d",
	        storage->db->info->quote.identifier[0],
	        temp.column->name,
	        storage->db->info->quote.identifier[1],
	        id);
	sqdb_exec(storage->db, buf->mem, NULL, NULL);
}

void  sq_storage_remove_all(SqStorage    *storage,
                            const char   *table_name,
                            const char   *sql_where_having)
{
	SqBuffer  *buf;

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = 0;
	sqdb_sql_from(storage->db, buf, table_name, true);
	if (sql_where_having)
		sq_buffer_write(buf, sql_where_having);
	sqdb_exec(storage->db, buf->mem, NULL, NULL);
}

// ------------------------------------

SqTable  *sq_storage_find_by_type(SqStorage *storage, const char *type_name)
{
	SqPtrArray *type_tables;      // sorted by SqType.name
	SqPtrArray *schema_tables;    // sorted by SqTable.name
	SqTable   **table_addr;
	int         count;

	type_tables = &storage->tables;
	schema_tables = sq_type_get_ptr_array(storage->schema->type);
	// if version is not the same
	if (storage->tables_version != storage->schema->version) {
		storage->tables_version  = storage->schema->version;
		count = schema_tables->length - type_tables->length;
		if (count > 0)
			sq_ptr_array_alloc(type_tables, count);
		memcpy(type_tables->data, schema_tables->data, sizeof(void*) * schema_tables->length);
		// sort storage->tables by SqTable.type.name
		sq_ptr_array_sort(type_tables, sq_entry_cmp_type_name);
	}
	// search storage->tables by SqTable.type.name
	table_addr = sq_ptr_array_search(type_tables, type_name,
	             (SqCompareFunc)sq_entry_cmp_str__type_name);
	if (table_addr)
		return *table_addr;
	return NULL;
}

// ----------------------------------------------------------------------------
// static function

static char  *get_primary_key_string(void *instance, const SqType *type, const char quote[2])
{
	SqColumn   *column;
	char       *condition = NULL;
	int         len;

	column = sq_table_get_primary(NULL, type);

	// integer
	instance = (char*)instance + column->offset;
	switch(SQ_TYPE_BUILTIN_INDEX(column->type)) {
	case SQ_TYPE_INT_INDEX:
		len = snprintf(NULL, 0, "%c%s%c=%d",
				quote[0], column->name, quote[1], *(int*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%d",
				quote[0], column->name, quote[1], *(int*)instance);
		break;

	case SQ_TYPE_UINT_INDEX:
		len = snprintf(NULL, 0, "%c%s%c=%u",
				quote[0], column->name, quote[1], *(unsigned int*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%u",
				quote[0], column->name, quote[1], *(unsigned int*)instance);
		break;

	case SQ_TYPE_INT64_INDEX:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "%c%s%c=%I64d",
				quote[0], column->name, quote[1], *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%I64d",
				quote[0], column->name, quote[1], *(int64_t*)instance);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		len = snprintf(NULL, 0, "%c%s%c=%ld",
				quote[0], column->name, quote[1], *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%ld",
				quote[0], column->name, quote[1], *(int64_t*)instance);
#else
		len = snprintf(NULL, 0, "%c%s%c=%lld",
				quote[0], column->name, quote[1], (long long int) *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%lld",
				quote[0], column->name, quote[1], (long long int) *(int64_t*)instance);
#endif
		break;

	case SQ_TYPE_UINT64_INDEX:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		len = snprintf(NULL, 0, "%c%s%c=%I64u",
				quote[0], column->name, quote[1], *(uint64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%I64u",
				quote[0], column->name, quote[1], *(uint64_t*)instance);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		len = snprintf(NULL, 0, "%c%s%c=%lu",
				quote[0], column->name, quote[1], *(uint64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%lu",
				quote[0], column->name, quote[1], *(uint64_t*)instance);
#else
		len = snprintf(NULL, 0, "%c%s%c=%llu",
				quote[0], column->name, quote[1], (long long unsigned int) *(int64_t*)instance) +1;
		condition = malloc(len);
		snprintf(condition, len, "%c%s%c=%llu",
				quote[0], column->name, quote[1], (long long unsigned int) *(int64_t*)instance);
#endif
		break;

	default:
		return NULL;
	}

	return condition;
}

// ----------------------------------------------------------------------------
// If compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions in SqStorage.h

#else   // __STDC_VERSION__
// declare functions here if compiler does NOT support inline function.

int  sq_storage_begin_trans(SqStorage *storage) {
	return SQ_STORAGE_BEGIN_TRANS(storage);
}

int  sq_storage_commit_trans(SqStorage *storage) {
	return SQ_STORAGE_COMMIT_TRANS(storage);
}

int  sq_storage_rollback_trans(SqStorage *storage) {
	return SQ_STORAGE_ROLLBACK_TRANS(storage);
}

#endif  // __STDC_VERSION
