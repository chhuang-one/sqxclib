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

#ifndef SQ_SCHEMA_MACRO_H
#define SQ_SCHEMA_MACRO_H

#include <SqSchema.h>

/* C Macro functions to create SQL Table easily, for example:

typedef struct Company
{
	int    id;
	char  *name;
	int    age;
	char  *address;
	double salary;
} Company;

	SQ_SCHEMA_CREATE(schema, "companies", Company, {
		SQT_INTEGER("id", Company, id);  SQC_PRIMARY();  SQC_HIDDEN();
		SQT_STRING("name", Company, name, -1);
		SQT_INTEGER("age", Company, age);
		SQT_STRING("address", Company, address, 50);  SQC_HIDDEN_NULL();
		SQT_DOUBLE("salary", Company, salary);
	});
*/

// SQ_SCHEMA_C_DEF() define variable for these macro.
#define SQ_SCHEMA_C_DEF(initial_table_value)         \
		SqTable  *table_cur_ = initial_table_value;  \
		SqColumn *column_cur_

// SQ_SCHEMA_C_SET() set current table for these macro.
#define SQ_SCHEMA_C_SET(current_table_value)         \
		table_cur_ = current_table_value

#define SQ_SCHEMA(schema, lambda)           \
		{                                   \
			SqTable  *table_cur_;           \
			SqColumn *column_cur_;          \
		}

#define SQ_SCHEMA_CREATE(schema, table_name, StructType, lambda)  \
		{                                   \
			SqTable  *table_cur_;           \
			SqColumn *column_cur_;          \
			table_cur_ = sq_schema_create_full(schema, table_name, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType));  \
			lambda;                         \
		}

#define SQ_SCHEMA_ALTER(schema, name, lambda)  \
		{                                      \
			SqTable  *table_cur_;              \
			SqColumn *column_cur_;             \
			table_cur_ = sq_schema_alter(schema, name, NULL);  \
			lambda;                            \
		}

// ----------------------------------------------------------------------------

#define SQT_CUR    table_cur_

#define SQT_DROP(column_name)    sq_table_drop_column(table_cur_, column_name)

#define SQT_RENAME(from, to)     sq_table_rename_column(table_cur_, from, to)

#define SQT_CREATE(table_name, StructType)    \
		table_cur_ = sq_schema_create_full(schema, table_name, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType));

// void SQT_COLUMN_PTRS(const SqColumn **column_ptrs, int n_column_ptrs);
#define SQT_COLUMN_PTRS(column_ptrs, n_column_ptrs)    \
		sq_table_add_column_ptrs(table_cur_, column_ptrs, n_column_ptrs)

// void SQT_COLUMN(const SqColumn *column, int n_column);
#define SQT_COLUMN(column, n_column)    \
		sq_table_add_column(table_cur_, column, n_column)

#define SQT_BOOLEAN(column_name, structure, member)   \
		(column_cur_ = sq_table_add_bool(table_cur_, column_name, offsetof(structure, member)))

#define SQT_BOOLEAN_AS(structure, member)   \
		(column_cur_ = sq_table_add_bool(table_cur_, #member, offsetof(structure, member)))

#define SQT_BOOL(column_name, structure, member)   \
		(column_cur_ = sq_table_add_bool(table_cur_, column_name, offsetof(structure, member)))

#define SQT_BOOL_AS(structure, member)   \
		(column_cur_ = sq_table_add_bool(table_cur_, #member, offsetof(structure, member)))

#define SQT_INTEGER(column_name, structure, member)   \
		(column_cur_ = sq_table_add_int(table_cur_, column_name, offsetof(structure, member)))

#define SQT_INTEGER_AS(structure, member)   \
		(column_cur_ = sq_table_add_int(table_cur_, #member, offsetof(structure, member)))

#define SQT_INT(column_name, structure, member)   \
		(column_cur_ = sq_table_add_int(table_cur_, column_name, offsetof(structure, member)))

#define SQT_INT_AS(structure, member)   \
		(column_cur_ = sq_table_add_int(table_cur_, #member, offsetof(structure, member)))

#define SQT_UINT(column_name, structure, member)   \
		(column_cur_ = sq_table_add_uint(table_cur_, column_name, offsetof(structure, member)))

#define SQT_UINT_AS(structure, member)   \
		(column_cur_ = sq_table_add_uint(table_cur_, #member, offsetof(structure, member)))

#define SQT_INT64(column_name, structure, member)   \
		(column_cur_ = sq_table_add_int64(table_cur_, column_name, offsetof(structure, member)))

#define SQT_INT64_AS(structure, member)   \
		(column_cur_ = sq_table_add_int64(table_cur_, #member, offsetof(structure, member)))

#define SQT_UINT64(column_name, structure, member)   \
		(column_cur_ = sq_table_add_uint64(table_cur_, column_name, offsetof(structure, member)))

#define SQT_UINT64_AS(structure, member)   \
		(column_cur_ = sq_table_add_uint64(table_cur_, #member, offsetof(structure, member)))

#define SQT_DOUBLE(column_name, structure, member, precision, scale)    \
		(column_cur_ = sq_table_add_double(table_cur_, column_name, offsetof(structure, member), precision, scale))

#define SQT_DOUBLE_AS(structure, member)    \
		(column_cur_ = sq_table_add_double(table_cur_, #member, offsetof(structure, member)))

#define SQT_TIMESTAMP(column_name, structure, member)    \
		(column_cur_ = sq_table_add_timestamp(table_cur_, column_name, offsetof(structure, member)))

#define SQT_TIMESTAMP_AS(structure, member)    \
		(column_cur_ = sq_table_add_timestamp(table_cur_, #member, offsetof(structure, member)))

#define SQT_TIMESTAMPS_AS(structure, member_created_at, member_updated_at)    \
		sq_table_add_timestamps(table_cur_, #member_created_at, offsetof(structure, member_created_at),  \
		                                    #member_updated_at, offsetof(structure, member_updated_at))

#define SQT_TIMESTAMPS_STRUCT(structure)    \
		sq_table_add_timestamps(table_cur_, NULL, offsetof(structure, created_at),  \
		                                    NULL, offsetof(structure, updated_at))

#define SQT_STRING(column_name, structure, member, length)   \
		(column_cur_ = sq_table_add_string(table_cur_, column_name, offsetof(structure, member), length))

#define SQT_STRING_AS(structure, member, length)   \
		(column_cur_ = sq_table_add_string(table_cur_, #member, offsetof(structure, member), length))

#define SQT_CHAR(column_name, structure, member, length)   \
		(column_cur_ = sq_table_add_char(table_cur_, column_name, offsetof(structure, member), length))

#define SQT_CHAR_AS(structure, member, length)   \
		(column_cur_ = sq_table_add_char(table_cur_, #member, offsetof(structure, member), length))

#define SQT_CUSTOM(column_name, structure, member, sqtype, length)  \
		(column_cur_ = sq_table_add_custom(table_cur_, column_name, offsetof(structure, member), sqtype, length))

#define SQT_CUSTOM_AS(structure, member, sqtype, length)   \
		(column_cur_ = sq_table_add_custom(table_cur_, #member, offsetof(structure, member), sqtype, length))

// ----------------------------------------------------------------------------

//      SQT_ADD_INDEX(index_name, column1_name, ...)
#define SQT_ADD_INDEX(index_name, ...)    \
		(column_cur_ = sq_table_add_index(table_cur_, index_name, __VA_ARGS__, NULL))

#define SQT_DROP_INDEX(index_name)    \
		sq_table_drop_index(table_cur_, index_name)

//      SQT_ADD_UNIQUE(unique_name, column1_name, ...)
#define SQT_ADD_UNIQUE(unique_name, ...)    \
		(column_cur_ = sq_table_add_unique(table_cur_, unique_name, __VA_ARGS__, NULL))

#define SQT_DROP_UNIQUE(unique_name)    \
		sq_table_drop_unique(table_cur_, unique_name)

//      SQT_ADD_PRIMARY(primary_name, column1_name, ...)
#define SQT_ADD_PRIMARY(primary_name, ...)    \
		(column_cur_ = sq_table_add_primary(table_cur_, primary_name, __VA_ARGS__, NULL))

#define SQT_DROP_PRIMARY(primary_name)    \
		sq_table_drop_primary(table_cur_, primary_name)

#define SQT_ADD_FOREIGN(foreign_name, column_name)    \
		(column_cur_ = sq_table_add_foreign(table_cur_, foreign_name, column_name))

#define SQT_DROP_FOREIGN(foreign_name)    \
		sq_table_drop_foreign(table_cur_, foreign_name)

// ----------------------------------------------------------------------------

#define SQC_POINTER()     \
		column_cur_->bit_field   |= SQB_POINTER

#define SQC_HIDDEN()     \
		column_cur_->bit_field   |= SQB_HIDDEN

#define SQC_HIDDEN_NULL()   \
		column_cur_->bit_field   |= SQB_HIDDEN_NULL

#define SQC_PRIMARY()    \
		column_cur_->bit_field   |= SQB_COLUMN_PRIMARY

#define SQC_FOREIGN(table_name, column_name)   \
		column_cur_->bit_field   |= SQB_COLUMN_FOREIGN

#define SQC_UNIQUE()  \
		column_cur_->bit_field   |= SQB_COLUMN_UNIQUE

#define SQC_REFERENCE(table_name, column_name)   \
		sq_column_reference(column_cur_, table_name, column_name)

#define SQC_ON_DELETE(act)  \
		sq_column_on_delete(column_cur_, act)

#define SQC_ON_UPDATE(act)  \
		sq_column_on_update(column_cur_, act)

#define SQC_AUTOINCREMENT()  \
		column_cur_->bit_field   |= SQB_COLUMN_AUTOINCREMENT

#define SQC_INCREMENT()  \
		column_cur_->bit_field   |= SQB_COLUMN_INCREMENT    // equal SQB_COLUMN_AUTOINCREMENT

#define SQC_NULLABLE()   \
		column_cur_->bit_field   |= SQB_COLUMN_NULLABLE

#define SQC_CHANGE()   \
		column_cur_->bit_field   |= SQB_COLUMN_CHANGED

#define SQC_USE_CURRENT()   \
		column_cur_->bit_field   |= SQB_COLUMN_CURRENT

#define SQC_USE_CURRENT_ON_UPDATE()   \
		column_cur_->bit_field   |= SQB_COLUMN_CURRENT_ON_UPDATE

#define SQC_DEFAULT(default_val)   \
		sq_column_default(column_cur_, default_val)

#define SQC_RAW(raw_property)   \
		sq_column_raw(column_cur_, raw_property)

#endif  // SQ_SCHEMA_MACRO_H
