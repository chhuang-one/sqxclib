/*
 *   Copyright (C) 2021-2026 by C.H. Huang
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
#include <stdio.h>             // snprintf(), fprintf(), stderr

#include <sqxc/SqError.h>
#include <sqxc/SqType.h>
#include <sqxc/SqxcValue.h>
#include <sqxc/SqStorage.h>
#include <sqxc/SqQuery.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

// used by sq_storage_setup_query()
static char *to_quoted_str(const char *name, const char *quote, const char *tail)
{
	char *quotedStr;
	int   length;

	length = snprintf(NULL, 0, "%c%s%c%s",
			quote[0], name, quote[1], tail ? tail : "") + 1;
	quotedStr = malloc(length);
	snprintf(quotedStr, length, "%c%s%c%s",
			quote[0], name, quote[1], tail ? tail : "");
	return quotedStr;
}

SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint)
{
	SqType     *table_type = NULL;
	SqPtrArray  names;
	SqTable    *table;
	int         n_table;
	int         command = sq_query_get_command(query);

	sq_ptr_array_init(&names, 8, NULL);
	n_table = sq_query_get_table_as_names(query, &names);
	sq_type_joint_clear(type_joint);

#ifndef NDEBUG
	// message
	fprintf(stderr, "%s: query has %d tables.\n",
	        "sq_storage_setup_query()", n_table);

	if (command == SQ_QUERY_CMD_NONE) {
		// message
		fprintf(stderr, "%s: query has no command currently, it will add SELECT columns if needed.\n",
		        "sq_storage_setup_query()");
	}
	else if (command == SQ_QUERY_CMD_SELECT) {
		// message
		fprintf(stderr, "%s: query has SELECT columns, it will not add SELECT columns.\n",
		        "sq_storage_setup_query()");
	}
	else {
		// warning
		fprintf(stderr, "%s: query is not SQL SELECT statement.\n",
		        "sq_storage_setup_query()");
	}
#endif  // NDEBUG

	// multiple table names, query has 'FROM' and 'JOIN'.
	for (unsigned int index = 0;  index < names.length;  index+=2) {
		table = sq_schema_find(storage->schema, names.data[index]);
		if (table == NULL) {
#ifndef NDEBUG
			fprintf(stderr, "%s: table '%s' not found in SqStorage::schema.\n",
			        "sq_storage_setup_query()", (char*)names.data[index]);
#endif
			if (type_joint->bit_field & SQB_TYPE_PARSE_UNKNOWN)
				continue;
			n_table = 0;
			break;
		}
		table_type = (SqType*)table->type;
		// add table and it's name to SqTypeJoint
		sq_type_joint_add(type_joint, table, names.data[index+1]);

		// if user has not SELECT any column in 'query'
		if (command == SQ_QUERY_CMD_NONE) {
			// There are multiple table's names in 'query'
			if (n_table > 1) {
				// add "SELECT table.column AS table_as_name.column" in query
				sq_query_select_table_as(query, table, names.data[index+1],
				                         storage->db->info->quote.identifier);
			}
#if SQ_CONFIG_QUERY_ONLY_COLUMN
			// There is single table name in 'query'
			else if (table_type->bit_field & SQB_TYPE_QUERY_FIRST) {
				// add query-only column in 'query'
				for (unsigned int index = 0;  index < table_type->n_entry;  index++) {
					SqColumn *column = (SqColumn*)table_type->entry[index];
					// for PostgreSQL: append " AS length(columnName)"
					// SELECT length(columnName) as "length(columnName)"
					if (column->bit_field & SQB_COLUMN_QUERY) {
						sq_query_select(query, column->name);
						char *temp = to_quoted_str(column->name, storage->db->info->quote.identifier, NULL);
						sq_query_as(query, temp);
						free(temp);
					}
				}
				// for MySQL
				// SELECT `tableName`.*
				char *temp = to_quoted_str(table->name, storage->db->info->quote.identifier, ".*");
				sq_query_select(query, temp);
				free(temp);
			}
#endif  // SQ_CONFIG_QUERY_ONLY_COLUMN
		}
	}

	sq_ptr_array_final(&names);

	if (n_table > 1)
		return type_joint;
	if (table_type == NULL && type_joint->bit_field & SQB_TYPE_PARSE_UNKNOWN)
		return type_joint;
	// special case: if there is only 1 table in 'query'
	if (n_table == 1)
		return table_type;
	return NULL;
}

void *sq_storage_query(SqStorage    *storage,
                       SqQuery      *query,
                       const SqType *table_type,
                       const SqType *container_type)
{
	Sqxc       *xcvalue;
	int         code;

	if (container_type == NULL)
		container_type = storage->container_default;
	if (table_type == NULL) {
		table_type = sq_storage_setup_query(storage, query, storage->joint_default);
		if (table_type == NULL)
			return NULL;
	}

	// destination of input
	xcvalue = (Sqxc*) storage->xc_input;
	sqxc_value_element(xcvalue)   = table_type;
	sqxc_value_container(xcvalue) = container_type;
	sqxc_value_instance(xcvalue)  = NULL;

	// execute SQL statement and get result
	sqxc_ready(xcvalue, NULL);
	code = sqdb_exec(storage->db, sq_query_c(query), xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	if (code != SQCODE_OK) {
//		storage->xc_input->code = code;
		sq_type_final_instance(container_type ? container_type : table_type,
		                       sqxc_value_instance(xcvalue), false);
		free(sqxc_value_instance(xcvalue));
		sqxc_value_instance(xcvalue) = NULL;
	}
	return sqxc_value_instance(xcvalue);
}

void *sq_storage_query_raw(SqStorage    *storage,
                           const char   *query_str,
                           const SqType *table_type,
                           const SqType *container_type)
{
	Sqxc       *xcvalue;
	int         code;

#ifndef NDEBUG
	if (container_type == NULL) {
		fprintf(stderr, "%s: It will not use default container type if 'container_type' is NULL.\n",
		        "sq_storage_query_raw()");
	}
#endif
	if (table_type == NULL) {
#ifndef NDEBUG
		fprintf(stderr, "%s: User must specify parameter 'table_type' such as SQ_TYPE_ROW.\n",
		        "sq_storage_query_raw()");
#endif
		return NULL;
	}

	// destination of input
	xcvalue = (Sqxc*) storage->xc_input;
	sqxc_value_element(xcvalue)   = table_type;
	sqxc_value_container(xcvalue) = container_type;
	sqxc_value_instance(xcvalue)  = NULL;

	// execute SQL statement and get result
	sqxc_ready(xcvalue, NULL);
	code = sqdb_exec(storage->db, query_str, xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	if (code != SQCODE_OK) {
//		storage->xc_input->code = code;
		sq_type_final_instance(container_type ? container_type : table_type,
		                       sqxc_value_instance(xcvalue), false);
		free(sqxc_value_instance(xcvalue));
		sqxc_value_instance(xcvalue) = NULL;
	}
	return sqxc_value_instance(xcvalue);
}
