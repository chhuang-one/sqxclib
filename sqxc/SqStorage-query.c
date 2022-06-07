/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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
#include <stdio.h>      // snprintf

#include <SqType.h>
#include <SqxcValue.h>
#include <SqStorage.h>
#include <SqQuery.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint)
{
	SqType     *table_type = NULL;
	SqPtrArray  names;
	SqTable    *table;
	int         n_table;

	sq_ptr_array_init(&names, 8, NULL);
	n_table = sq_query_get_table_as_names(query, &names);
	sq_type_joint_clear(type_joint);
	// multiple table names, query has 'FROM' and 'JOIN'.
	for (int index = 0;  index < names.length;  index+=2) {
		table = sq_schema_find(storage->schema, names.data[index]);
		if (table == NULL) {
			if (type_joint->bit_field & SQB_TYPE_PARSE_UNKNOWN)
				continue;
			n_table = 0;
			break;
		}
		table_type = (SqType*)table->type;
		// add table and it's name to SqTypeJoint
		sq_type_joint_add(type_joint, table, names.data[index+1]);
		// if there are multiple table's names in 'query'
		if (n_table > 1) {
			// add "SELECT table.column AS 'table_as_name.column'" in query
			sq_query_select_table_as(query, table, names.data[index+1]);
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
	void       *instance;

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
	sqdb_exec(storage->db, sq_query_c(query), xcvalue, NULL);
	sqxc_finish(xcvalue, NULL);
	instance = sqxc_value_instance(xcvalue);

	return instance;
}
