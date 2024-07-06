/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#include <assert.h>
#include <stdio.h>

#include <SqConfig.h>
#include <SqPtrArray.h>
#include <SqStrArray.h>
#include <SqSchema-macro.h>
#include <SqJoint.h>
#include <SqRow.h>
#include <SqxcSql.h>
#include <SqxcValue.h>
#include <SqxcEmpty.h>
#if SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

#ifdef _MSC_VER
#define strdup       _strdup
#endif

// ----------------------------------------------------------------------------
// C structure

typedef struct User     User;

struct User {
	int    id;
	char  *name;
	char  *email;

	SqStrArray     strs;
	SqIntArray     ints;
};

// ----------------------------------------------------------------------------
// define static SqType and SqColumn.

// --- UserColumns is sorted by programer... :)
static const SqColumn  *UserColumns[] = {
	&(SqColumn) {SQ_TYPE_STR,          "email",   offsetof(User, email), 0},
	&(SqColumn) {SQ_TYPE_INT,          "id",      offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_INT_ARRAY,    "ints",    offsetof(User, ints),  0},
	&(SqColumn) {SQ_TYPE_STR,          "name",    offsetof(User, name),  0},
	&(SqColumn) {SQ_TYPE_STR_ARRAY,    "strs",    offsetof(User, strs),  0},
};

// --- UserType use sorted UserColumns
const SqType UserType = {
	.size  = sizeof(User),
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),
	.entry   = (SqEntry**) UserColumns,
	.n_entry = sizeof(UserColumns) / sizeof(SqColumn*),
	.bit_field  = SQB_TYPE_SORTED,                           // UserColumns is sorted
};

// ------------------------------------

void print_user(User *user)
{
	unsigned int  index;

	printf("User" "\n"
	       "  id = %d" "\n"
	       "  name = %s" "\n"
	       "  email = %s" "\n",
		   user->id, user->name, user->email);

	for (index = 0;  index < user->strs.length;  index++)
		printf("  strs[%d] = %s\n", index, user->strs.data[index]);
	for (index = 0;  index < user->ints.length;  index++)
		printf("  ints[%d] = %d\n", index, (int)user->ints.data[index]);
	puts("\n");
}

SqTable *create_user_table_by_type(SqSchema *schema)
{
	return sq_schema_create_by_type(schema, "users", &UserType);
//	return sq_schema_create_full(schema, "users", NULL, &UserType, 0);
}

// ----------------------------------------------------------------------------
// Sqxc - Input

void test_sqxc_joint_input()
{
	SqTypeJoint  *type;
	SqTable  *table;
	Sqxc     *xc;
	User     *user;
	void    **instance;

	table = sq_table_new("users", &UserType);
	type  = sq_type_joint_new();
	sq_type_joint_add(type, table, "tb1");
	sq_type_joint_add(type, table, "tb2");

	xc = sqxc_new(SQXC_INFO_VALUE);
#if SQ_CONFIG_HAVE_JSONC
	sqxc_insert(xc, sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
#endif

	sqxc_value_element(xc) = type;

	sqxc_ready(xc, NULL);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT;
	xc->value.pointer = NULL;
	sqxc_send(xc);

	xc->name = "tb1.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 183;
	sqxc_send(xc);

	// program can't parse JSON array string if no JSON parser in sqxc chain
	xc->name = "tb2.strs";
	xc->type = SQXC_TYPE_STR;
	xc->value.str = "[ \"str0\", \"str1\", \"str2\" ]";
	sqxc_send(xc);

	xc->name = "tb2.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 233;
	sqxc_send(xc);

	xc->name = "tb2.name";
	xc->type = SQXC_TYPE_NULL;
	xc->value.pointer = NULL;
	sqxc_send(xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	xc->value.pointer = NULL;
	sqxc_send(xc);

	sqxc_finish(xc, NULL);

	instance = sqxc_value_instance(xc);
	user = instance[0];
	printf("tb1.id = %d\n", user->id);
	assert(user->id == 183);
	free(user);

	user = instance[1];
	printf("tb2.id = %d\n", user->id);
	assert(user->id == 233);
#if SQ_CONFIG_HAVE_JSONC
	assert(user->strs.length == 3);
#else
	// program can't parse JSON array string if no JSON parser in sqxc chain
	assert(user->strs.length == 0);
#endif
	free(user);
	free(instance);

	sqxc_free_chain(xc);
	sq_type_joint_free(type);
	sq_table_free(table);
}

void test_sqxc_row_input_output()
{
	SqTypeRow *type;
	SqTable   *table;
	Sqxc      *xc;
	SqRow     *row;

	table = sq_table_new("users", &UserType);
	type  = sq_type_row_new();
	sq_type_row_add(type, table, "tb1");
	sq_type_row_add(type, table, "tb2");

	xc = sqxc_new(SQXC_INFO_VALUE);
#if SQ_CONFIG_HAVE_JSONC
	sqxc_insert(xc, sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
#endif
	sqxc_value_element(xc) = type;

	sqxc_ready(xc, NULL);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT;
	xc->value.pointer = NULL;
	sqxc_send(xc);

	xc->name = "tb1.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 9741;
	sqxc_send(xc);

	xc->name = "tb2.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 1775;
	sqxc_send(xc);

	// program can't parse JSON array string if no JSON parser in sqxc chain
	xc->name = "tb2.strs";
	xc->type = SQXC_TYPE_STR;
	xc->value.str = "[ \"str0\", \"str1\", \"str2\" ]";
	sqxc_send(xc);

	// unknown table.column
	xc->name = "unknown_table.unknown_column";
	xc->type = SQXC_TYPE_STR;
	xc->value.str = "unknown string";
	sqxc_send(xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	xc->value.pointer = NULL;
	sqxc_send(xc);

	sqxc_finish(xc, NULL);
	row = sqxc_value_instance(xc);
	assert(row->cols[0].type == SQ_TYPE_INT);
	assert(row->cols[1].type == SQ_TYPE_INT);
	assert(row->cols[2].type == SQ_TYPE_STR_ARRAY);
	assert(row->cols[3].type == SQ_TYPE_STR);
	printf("%s = %d\n", row->cols[0].name, row->data[0].integer);
	printf("%s = %d\n", row->cols[1].name, row->data[1].integer);
	printf("%s = %s\n", row->cols[3].name, row->data[3].str);

	sqxc_free_chain(xc);
	sq_table_free(table);

	// --- output ---
	Sqxc *xc_output;

	xc_output = sqxc_new_chain(SQXC_INFO_EMPTY, NULL);
	sqxc_ready(xc_output, NULL);

	type->write(row, type, xc_output);

	sqxc_finish(xc_output, NULL);

	sqxc_free_chain(xc_output);

	sq_row_free(row);
	sq_type_row_free(type);
}

#if SQ_CONFIG_HAVE_JSONC

const char *json_array_string =
"["
	"{"
		"\"id\": 10,"
		"\"email\": \"guest@\","
		"\"strs\": [\"first\", \"second\"],"
		"\"ints\": [1, 2]"
	"}"
"]";

const char *json_object_string =
"{"
	"\"id\": 10,"
	"\"email\": \"guest@\","
	"\"strs\": [\"first\", \"second\"],"
	"\"ints\": [1, 2]"
"}";

void test_sqxc_jsonc_input()
{
	Sqxc *xcjsonc;
	Sqxc *xcchain;

	xcchain = sqxc_new_chain(SQXC_INFO_EMPTY, SQXC_INFO_JSONC_PARSER, NULL);
	xcjsonc = sqxc_find(xcchain, SQXC_INFO_JSONC_PARSER);

	sqxc_ready(xcchain, NULL);
	xcjsonc->type = SQXC_TYPE_STR;
	xcjsonc->name = NULL;
	xcjsonc->value.str = (char*)json_array_string;
	xcjsonc->info->send(xcjsonc, xcjsonc);
	sqxc_finish(xcchain, NULL);

	sqxc_free_chain(xcchain);
}

void test_sqxc_jsonc_input_user()
{
	Sqxc *xcvalue;
	User *user;

	xcvalue = sqxc_new_chain(SQXC_INFO_VALUE, SQXC_INFO_JSONC_PARSER, NULL);
	sqxc_value_element(xcvalue) = &UserType;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcvalue, NULL);

	Sqxc *xcjsonc;
	xcjsonc = sqxc_find(xcvalue, SQXC_INFO_JSONC_PARSER);
	xcjsonc->type = SQXC_TYPE_STR;
	xcjsonc->name = NULL;
	xcjsonc->value.str = (char*)json_object_string;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	sqxc_finish(xcvalue, NULL);

	user = (User*)sqxc_value_instance(xcvalue);
	assert(user->id == 10);

	print_user(user);
	free(user);
	sqxc_free_chain(xcvalue);
}

void test_sqxc_jsonc_input_unknown()
{
	Sqxc *xcvalue;
	User *user;

	xcvalue = sqxc_new_chain(SQXC_INFO_VALUE, SQXC_INFO_JSONC_PARSER, NULL);
	sqxc_value_element(xcvalue) = &UserType;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcvalue, NULL);
	Sqxc *xc;
	xc = xcvalue;

	xc->type = SQXC_TYPE_OBJECT;
	xc = sqxc_send(xc);

	/* send unknown data to SqxcValue to test SQ_TYPE_UNKNOWN */
	xc->type = SQXC_TYPE_OBJECT;
	xc->name = "xobj";
	xc = sqxc_send(xc);

	xc->type = SQXC_TYPE_ARRAY;
	xc->name = "xarray";
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);

	xc->type = SQXC_TYPE_INT;
	xc->name = "xint";
	xc->value.integer = 100;
	xc = sqxc_send(xc);

	xc->type = SQXC_TYPE_ARRAY_END;
	xc->name = "xarray";
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);

	xc->type = SQXC_TYPE_OBJECT_END;
	xc->name = "xobj";
	xc = sqxc_send(xc);

	/* send known data */
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 99;
	xc = sqxc_send(xc);

	xc->type = SQXC_TYPE_OBJECT_END;
	xc = sqxc_send(xc);

	sqxc_finish(xcvalue, NULL);

	user = (User*)sqxc_value_instance(xcvalue);
	assert(user->id == 99);

	print_user(user);
	free(user);
	sqxc_free_chain(xcvalue);
}

// ----------------------------------------------------------------------------
// Sqxc - Output

void test_sqxc_jsonc_output(User *instance)
{
	Sqxc *xcjsonc;
	Sqxc *xcchain;

	xcchain = sqxc_new_chain(SQXC_INFO_EMPTY, SQXC_INFO_JSONC_WRITER, NULL);
	xcjsonc = sqxc_find(xcchain, SQXC_INFO_JSONC_WRITER);
	sqxc_ready(xcchain, NULL);

	xcjsonc->type = SQXC_TYPE_OBJECT;
	xcjsonc->name = NULL;
	xcjsonc->value.pointer = NULL;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_INT;
	xcjsonc->name = "id";
	xcjsonc->value.integer = instance->id;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_STR;
	xcjsonc->name = "name";
	xcjsonc->value.str = instance->name;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_STR;
	xcjsonc->name = "email";
	xcjsonc->value.str = instance->email;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_OBJECT_END;
	xcjsonc->name = NULL;
	xcjsonc->value.pointer = NULL;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	sqxc_finish(xcchain, NULL);

	sqxc_free_chain(xcchain);
}

void test_sqxc_sql_output(bool use_update)
{
	SqTable *table;
	Sqxc *xcchain;
	Sqxc *xcsql;
	Sqxc *xccur;

	table = sq_table_new("User", &UserType);

	xcchain = sqxc_new_chain(SQXC_INFO_SQL, SQXC_INFO_JSONC_WRITER, NULL);
	xcsql = sqxc_find(xcchain, SQXC_INFO_SQL);
	if (use_update) {
		sqxc_sql_id(xcsql) = 2333;
		sqxc_ctrl(xcsql, SQXC_SQL_CTRL_UPDATE, table->name);
	}
	else
		sqxc_ctrl(xcsql, SQXC_SQL_CTRL_INSERT, table->name);

	sqxc_ready(xcchain, NULL);

	xccur = xcchain;
	xccur->type = SQXC_TYPE_OBJECT;
	xccur->name = NULL;
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_STR;
	xccur->name = "email";
	xccur->value.str = (char*)"guest@";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_INT;
	xccur->name = "id";
	xccur->value.integer = 2333;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_RAW;
	xccur->name = "str_to_hex";
	//               = "This is a string.";
	xccur->value.raw = "0x54686973206973206120737472696E672E";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_ARRAY;
	xccur->name = "ints";
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);
	xccur->type = SQXC_TYPE_ARRAY_END;
	xccur->name = "ints";
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_STR;
	xccur->name = "name";
	xccur->value.str = (char*)"Bob";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_ARRAY;
	xccur->name = "strs";
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);
	xccur->type = SQXC_TYPE_ARRAY_END;
	xccur->name = "strs";
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_OBJECT_END;
	xccur->name = NULL;
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	puts(xcsql->buf);
	sqxc_finish(xcchain, NULL);

	sqxc_free_chain(xcchain);
}

#endif  // SQ_CONFIG_HAVE_JSONC

// ----------------------------------------------------------------------------

int  main(void)
{
	SqSchema   *schema;
	User       *user;

	schema = sq_schema_new("default");
	create_user_table_by_type(schema);

	user = malloc(sizeof(User));
	user->id = 10;
	user->name = "Bob";
	user->email = "guest@";
	sq_str_array_init(&user->strs, 8);
	sq_str_array_push(&user->strs, "first");
	sq_int_array_init(&user->ints, 8);
	sq_int_array_push(&user->ints, 1);

	test_sqxc_joint_input();
	test_sqxc_row_input_output();
#if SQ_CONFIG_HAVE_JSONC
	test_sqxc_jsonc_input();
	test_sqxc_jsonc_input_user();
	test_sqxc_jsonc_input_unknown();
	test_sqxc_jsonc_output(user);
	test_sqxc_sql_output(true);
#endif  // SQ_CONFIG_HAVE_JSONC

//	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
	return EXIT_SUCCESS;
}
