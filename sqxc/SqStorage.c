/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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
#include <stdarg.h>     // va_list, va_start, va_end, va_arg...etc
#include <stdio.h>      // snprintf
#include <inttypes.h>   // PRId64, PRIu64

#include <SqError.h>
#include <SqStorage.h>
#include <SqxcSql.h>
#include <SqxcValue.h>
#if SQ_CONFIG_HAVE_JSONC
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

#define SCHEMA_INITIAL_VERSION       0

static int  print_where_column(const SqColumn *column, void *instance, SqBuffer *buf, const char quote[2]);
static int  sqxc_sql_set_columns(SqxcSql      *xcsql,
                                 const SqType *table_type,
                                 const char   *sql_where_having,
                                 va_list       arg_list);
#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
static int  sqxc_sql_set_fields(SqxcSql      *xcsql,
                                const SqType *table_type,
                                const char   *sql_where_having,
                                va_list       arg_list);
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

void  sq_storage_init(SqStorage *storage, Sqdb *db)
{
	storage->db = db;
	storage->schema = sq_schema_new_ver(SCHEMA_INITIAL_VERSION, "current");
	storage->tables_version = SCHEMA_INITIAL_VERSION;
	sq_ptr_array_init(&storage->tables, 16, NULL);

	storage->joint_default     = sq_type_joint_new();
	storage->container_default = SQ_TYPE_PTR_ARRAY;

	storage->xc_input  = sqxc_new(SQXC_INFO_VALUE);
	storage->xc_output = sqxc_new(SQXC_INFO_SQL);

#if SQ_CONFIG_HAVE_JSONC
	// append JSON parser/writer to tail of list
	sqxc_insert(storage->xc_input,  sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
	sqxc_insert(storage->xc_output, sqxc_new(SQXC_INFO_JSONC_WRITER), -1);
#endif
}

void  sq_storage_final(SqStorage *storage)
{
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
                     int64_t       id)
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

	// SQL statement
	buf = sqxc_get_buffer(xcvalue);
	buf->writed = 0;
	// select query-only columns before others if table has query-only column
	temp.column = sqdb_sql_select(storage->db, buf, table_name, table_type);
	// WHERE primaryKey=...
	if (temp.column == NULL)
		temp.column = sq_table_get_primary(NULL, table_type);
	print_where_column(temp.column, &id, buf, storage->db->info->quote.identifier);

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
	// select query-only columns before others if table has query-only column
	sqdb_sql_select(storage->db, temp.buf, table_name, table_type);

	// SQL WHERE ... HAVING ...
	if (sql_where_having)
		sq_buffer_write(temp.buf, sql_where_having);

	sqxc_ready(xcvalue, NULL);
	sqdb_exec(storage->db, temp.buf->mem, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	temp.instance = sqxc_value_instance(xcvalue);
	return temp.instance;
}

int64_t sq_storage_insert(SqStorage    *storage,
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
			return 0;
		table_type = temp.table->type;
	}

	// destination of output
	temp.xcsql = storage->xc_output;
	sqxc_sql_set_db(temp.xcsql, storage->db);
	sqxc_ctrl(temp.xcsql, SQXC_SQL_CTRL_INSERT, (void*)table_name);

	sqxc_ready(temp.xcsql, NULL);
	table_type->write(instance, table_type, temp.xcsql);
	sqxc_finish(temp.xcsql, NULL);

	// return the last inserted row id
	return sqxc_sql_id(temp.xcsql);
}

int   sq_storage_update(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        void         *instance)
{
	Sqxc      *xcsql;
	SqBuffer  *buf;
	union {
		SqTable   *table;
		SqColumn  *column;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return 0;
		table_type = temp.table->type;
	}

	// destination of output
	xcsql = storage->xc_output;
	sqxc_sql_set_db(xcsql, storage->db);
	if (sqxc_sql_condition(xcsql) == NULL) {
		temp.column = sq_table_get_primary(NULL, table_type);
		// SQL statement. Because input buffer doesn't use here, I use it temporary.
		buf = sqxc_get_buffer(storage->xc_input);
		buf->writed = 0;
		print_where_column(temp.column, (char*)instance + temp.column->offset,
		             buf, storage->db->info->quote.identifier);
		sqxc_sql_condition(xcsql) = buf->mem;
	}
	sqxc_ctrl(xcsql, SQXC_SQL_CTRL_UPDATE, table_name);

	sqxc_ready(xcsql, NULL);
	table_type->write(instance, table_type, xcsql);
	sqxc_finish(xcsql, NULL);
	// free WHERE condition
//	sqxc_sql_condition(temp.xcsql) = NULL;    // this has been done in sqxc_finish()

	// return number of rows changed
	return (int)sqxc_sql_changes(xcsql);
}

int64_t sq_storage_update_all(SqStorage    *storage,
                              const char   *table_name,
                              const SqType *table_type,
                              void         *instance,
                              const char   *sql_where_having,
                              ...)
{
	va_list  arg_list;
	union {
		SqTable  *table;
		SqxcSql  *xcsql;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return 0;
		table_type = temp.table->type;
	}

	// set SqxcSql's variable for UPDATE command
	temp.xcsql = (SqxcSql*)storage->xc_output;
	va_start(arg_list, sql_where_having);
	sqxc_sql_set_columns(temp.xcsql, table_type, sql_where_having, arg_list);
	va_end(arg_list);

	sq_storage_update(storage, table_name, table_type, instance);
	// return number of rows changed
	return temp.xcsql->changes;
}

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

int64_t sq_storage_update_field(SqStorage    *storage,
                                const char   *table_name,
                                const SqType *table_type,
                                void         *instance,
                                const char   *sql_where_having,
                                ...)
{
	va_list  arg_list;
	union {
		SqTable  *table;
		SqxcSql  *xcsql;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table == NULL)
			return 0;
		table_type = temp.table->type;
	}

	// set SqxcSql's variable for UPDATE command
	temp.xcsql = (SqxcSql*)storage->xc_output;
	va_start(arg_list, sql_where_having);
	sqxc_sql_set_fields(temp.xcsql, table_type, sql_where_having, arg_list);
	va_end(arg_list);

	sq_storage_update(storage, table_name, table_type, instance);
	// return number of rows changed
	return temp.xcsql->changes;
}
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

void  sq_storage_remove(SqStorage    *storage,
                        const char   *table_name,
                        const SqType *table_type,
                        int64_t       id)
{
	SqBuffer  *buf;
	union {
		SqTable   *table;
		SqColumn  *column;
	} temp;

	if (table_type == NULL) {
		// find SqTable by table_name
		temp.table = sq_schema_find(storage->schema, table_name);
		if (temp.table)
			table_type = temp.table->type;
	}

	temp.column = table_type ? sq_table_get_primary(NULL, table_type) : NULL;

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = 0;
	sqdb_sql_delete(storage->db, buf, table_name);
	print_where_column(temp.column, &id, buf, storage->db->info->quote.identifier);
	sqdb_exec(storage->db, buf->mem, NULL, NULL);
}

void  sq_storage_remove_all(SqStorage    *storage,
                            const char   *table_name,
                            const char   *sql_where_having)
{
	SqBuffer  *buf;

	buf = sqxc_get_buffer(storage->xc_output);
	buf->writed = 0;
	sqdb_sql_delete(storage->db, buf, table_name);
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
	table_addr = (SqTable**)sq_ptr_array_search(type_tables,
	                                type_name, sq_entry_cmp_str__type_name);
	if (table_addr)
		return *table_addr;
	return NULL;
}

// ----------------------------------------------------------------------------
// static function

static void sqxc_sql_init_columns(SqxcSql *xcsql, const char *sql_where_having)
{
	// set SqxcSql's variable for UPDATE command
	xcsql->condition = (sql_where_having) ? sql_where_having : "";
	if (xcsql->columns.data == NULL)
		sq_ptr_array_init(&xcsql->columns, 0, NULL);
}

static int  sqxc_sql_set_columns(SqxcSql      *xcsql,
                                 const SqType *table_type,
                                 const char   *sql_where_having,
                                 va_list       arg_list)
{
	union {
		const char *name;
		SqColumn  **column_addr;
		SqColumn   *column;
	} temp;

	sqxc_sql_init_columns(xcsql, sql_where_having);
	for(;;) {
		temp.name = va_arg(arg_list, const char*);
		if (temp.name == NULL)
			break;
		temp.column_addr = (SqColumn**)sq_type_find_entry(table_type, temp.name, NULL);
		if (temp.column_addr == NULL)
			continue;
		temp.column = *temp.column_addr;
		sq_ptr_array_push(&xcsql->columns, temp.column);
	}

	return xcsql->columns.length;
}

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

// used by sqxc_sql_set_fields(). Its actual parameter type:
//     int  sq_entry_cmp_offset(SqEntry   **entryAddr1, SqEntry   **entryAddr2);
static int  sq_entry_cmp_offset(const void *entryAddr1, const void *entryAddr2)
{
	return (int)(*(SqEntry**)entryAddr1)->offset - (int)(*(SqEntry**)entryAddr2)->offset;
}

// used by sqxc_sql_set_fields(). Its actual parameter type:
//     int  sq_entry_cmp_size_t__offset(size_t     *offset, SqEntry   **entryAddr)
static int  sq_entry_cmp_size_t__offset(const void *offset, const void *entryAddr)
{
	return (int)*(size_t*)offset - (int)(*(SqEntry**)entryAddr)->offset;
}

static int  sqxc_sql_set_fields(SqxcSql      *xcsql,
                                const SqType *table_type,
                                const char   *sql_where_having,
                                va_list       arg_list)
{
	SqPtrArray  array;
	union {
		size_t      offset;
		SqColumn  **column_addr;
		SqColumn   *column;
	} temp;

	// sort table_type->entry by entry->offset
	sq_ptr_array_init(&array, table_type->n_entry, NULL);
	memcpy(array.data, table_type->entry, table_type->n_entry * sizeof(void*));
	array.length = table_type->n_entry;
	sq_ptr_array_sort(&array, sq_entry_cmp_offset);

	sqxc_sql_init_columns(xcsql, sql_where_having);
	for(;;) {
		temp.offset = va_arg(arg_list, size_t);
		if (temp.offset == -1)
			break;
		temp.column_addr = (SqColumn**)sq_ptr_array_search(&array,
		                                       &temp.offset, sq_entry_cmp_size_t__offset);
		if (temp.column_addr == NULL)
			continue;
		temp.column = *temp.column_addr;
		sq_ptr_array_push(&xcsql->columns, temp.column);
	}

	sq_ptr_array_final(&array);
	return xcsql->columns.length;
}
#endif  // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

static int  print_where_column(const SqColumn *column, void *instance, SqBuffer *buf, const char quote[2])
{
	const SqType *type;
	const char   *name;
	union {
		char     *mem;
		int       len;
	} temp;

	// "WHERE" + " " string length = 6
	temp.mem = sq_buffer_alloc(buf, 6);
	memcpy(temp.mem, "WHERE", 5);
	temp.mem[5] = ' ';

	if (column) {
		name = column->name;
		type = column->type;
	}
	else {
		name = "id";
		type = SQ_TYPE_INT64;
	}
	// integer
	switch(SQ_TYPE_BUILTIN_INDEX(type)) {
	case SQ_TYPE_INT_INDEX:
		temp.len = snprintf(NULL, 0, "%c%s%c=%d",
				quote[0], name, quote[1], *(int*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%d",
				quote[0], name, quote[1], *(int*)instance);
		break;

	case SQ_TYPE_UINT_INDEX:
		temp.len = snprintf(NULL, 0, "%c%s%c=%u",
				quote[0], name, quote[1], *(unsigned int*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%u",
				quote[0], name, quote[1], *(unsigned int*)instance);
		break;

	case SQ_TYPE_INT64_INDEX:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%I64d",
				quote[0], name, quote[1], *(int64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%I64d",
				quote[0], name, quote[1], *(int64_t*)instance);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%ld",
				quote[0], name, quote[1], *(int64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%ld",
				quote[0], name, quote[1], *(int64_t*)instance);
#elif defined(__GNUC__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%lld",
				quote[0], name, quote[1], *(int64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%lld",
				quote[0], name, quote[1], *(int64_t*)instance);
#else // C99
		temp.len = snprintf(NULL, 0, "%c%s%c=%" PRId64,
				quote[0], name, quote[1], *(int64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%" PRId64,
				quote[0], name, quote[1], *(int64_t*)instance);
#endif
		break;

	case SQ_TYPE_UINT64_INDEX:
#if defined (_MSC_VER)  // || defined (__MINGW32__) || defined (__MINGW64__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%I64u",
				quote[0], name, quote[1], *(uint64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%I64u",
				quote[0], name, quote[1], *(uint64_t*)instance);
#elif defined(__WORDSIZE) && (__WORDSIZE == 64) && !defined(__APPLE__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%lu",
				quote[0], name, quote[1], *(uint64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%lu",
				quote[0], name, quote[1], *(uint64_t*)instance);
#elif defined(__GNUC__)
		temp.len = snprintf(NULL, 0, "%c%s%c=%llu",
				quote[0], name, quote[1], *(uint64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%llu",
				quote[0], name, quote[1], *(uint64_t*)instance);
#else // C99
		temp.len = snprintf(NULL, 0, "%c%s%c=%" PRIu64,
				quote[0], name, quote[1], *(uint64_t*)instance);
		snprintf(sq_buffer_alloc(buf, temp.len), temp.len+1, "%c%s%c=%" PRIu64,
				quote[0], name, quote[1], *(uint64_t*)instance);
#endif
		break;

	default:
		return 0;
	}

	return temp.len;
}


// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqStorage.h

#else   // __STDC_VERSION__
// define functions here if compiler does NOT support inline function.

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
