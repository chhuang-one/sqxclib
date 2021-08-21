/*
 *   Copyright (C) 2021 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>      // snprintf

#include <SqxcValue.h>
#include <SqStorage.h>
#include <SqQuery.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

static void query_add_column_as_names(SqQuery *query, SqTable *table)
{
	SqType   *type = (SqType*)table->type;
	SqColumn *column;
	char     *buffer = NULL;
	int       buf_len;

	for (int index = 0;  index < type->n_entry;  index++) {
		column = (SqColumn*)type->entry[index];
		if (SQ_TYPE_IS_FAKE(column->type))
			continue;
		buf_len = snprintf(NULL, 0, "%s.%s AS '%s.%s'", table->name, column->name, table->name, column->name) + 1;
		buffer = realloc(buffer, buf_len);
		snprintf(buffer, buf_len, "%s.%s AS '%s.%s'", table->name, column->name, table->name, column->name);
		sq_query_select(query, buffer, NULL);
	}
	free(buffer);
}

SqType  *sq_storage_type_from_query(SqStorage *storage, SqQuery *query, int *n_tables_in_query)
{
	SqPtrArray  names;
	SqTable    *table;
	SqType     *type = NULL;
	int         n;

	sq_ptr_array_init(&names, 8, NULL);
	n = sq_query_get_table_as_names(query, &names);
	if (n != 0) {
		// multiple table names, query has 'FROM' and 'JOIN'.
		type = sq_type_joint_new();
		for (int index = 0;  index < names.length;  index+=2) {
			table = sq_schema_find(storage->schema, names.data[index]);
			if (table == NULL) {
				sq_type_unref(type);
				type = NULL;
				n = 0;
				break;
			}
			sq_type_joint_add(type, table, names.data[index+1]);
			// add 'SELECT' columns in query if there are multiple table's names in query
			if (n > 1)
				query_add_column_as_names(query, table);
		}
	}

	sq_ptr_array_final(&names);
	if (n_tables_in_query)
		*n_tables_in_query = n;
	return type;
}

void *sq_storage_query(SqStorage *storage, SqQuery *query, const SqType *container, const SqType *type)
{
	SqType     *type_cur;
	Sqxc       *xcvalue;
	char       *sql;
	void       *instance;

	if (container == NULL)
		container = storage->container_default;
	if (type)
		type_cur = (SqType*)type;
	else {
		type_cur = sq_storage_type_from_query(storage, query, NULL);
		if (type_cur == NULL)
			return NULL;
		// special case for SqPtrArray: if there is only 1 table in query
		if (container == SQ_TYPE_PTR_ARRAY && type_cur->n_entry == 1) {
			type = type_cur->entry[0]->type;
			sq_type_unref(type_cur);
			type_cur = (SqType*)type;
		}
	}

	// get SQL statement string
	sql = sq_query_to_sql(query);
	// destination of input
	xcvalue = (Sqxc*) storage->xc_input;
	sqxc_value_element(xcvalue) = type_cur;
	sqxc_value_container(xcvalue) = (container) ? container : (SqType*)storage->container_default;
	// get input from SQL
	sqxc_ready(xcvalue, NULL);
	sqdb_exec(storage->db, sql, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	instance = sqxc_value_instance(xcvalue);
	// free SQL statement string
	free(sql);

	if (type == NULL)
		sq_type_unref(type_cur);
	return instance;
}
