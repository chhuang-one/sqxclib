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

#include <stdio.h>

#include <SqxcValue.h>
#include <SqxcJsonc.h>
#include <SqxcEmpty.h>
#include <SqxcSqlite.h>
#include <SqPtrArray.h>
#include <SqSchema-macro.h>

#include <Sqdb.h>

// ----------------------------------------------------------------------------
// declare C structure

typedef struct User     User;

struct User {
	int    id;
	char*  name;
	char*  email;

	SqStringArray  strs;
	SqIntptrArray  ints;
};

// ----------------------------------------------------------------------------
// declare static SqType and SqColumn.

// --- UserColumns is sorted by programer... :)
static const SqColumn  *UserColumns[] = {
	&(SqColumn) {SQ_TYPE_STRING,       "email",   offsetof(User, email), 0},
	&(SqColumn) {SQ_TYPE_INT,          "id",      offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_INTPTR_ARRAY, "ints",    offsetof(User, ints),  0},
	&(SqColumn) {SQ_TYPE_STRING,       "name",    offsetof(User, name),  0},
	&(SqColumn) {SQ_TYPE_STRING_ARRAY, "strs",    offsetof(User, strs),  0},
};

// --- UserType use sorted UserColumns
const SqType UserType = {
	.size  = sizeof(User),
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),
	.entry   = (SqField**) UserColumns,
	.n_entry = sizeof(UserColumns) / sizeof(SqColumn*),
	.bit_field  = SQB_TYPE_SORTED                            // UserColumns is sorted
};

// ------------------------------------

void print_user(User* user)
{
	int  index;

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

SqTable* create_user_table_by_type(SqSchema* schema)
{
	return sq_schema_create_by_type(schema, "users", &UserType);
//	return sq_schema_create_full(schema, "users", &UserType, NULL, 0);
}

// ----------------------------------------------------------------------------
// Sqxc - Input

char* json_array_string =
"["
	"{"
		"\"id\": 10,"
		"\"email\": \"guest@\","
		"\"strs\": [\"first\", \"second\"],"
		"\"ints\": [1, 2]"
	"}"
"]";

char* json_object_string =
"{"
	"\"id\": 10,"
	"\"email\": \"guest@\","
	"\"strs\": [\"first\", \"second\"],"
	"\"ints\": [1, 2]"
"}";

void test_sqxc_jsonc_input()
{
	Sqxc* xcjsonc;

	xcjsonc = sqxc_new_input(SQXC_INFO_JSONC, SQXC_INFO_EMPTY);
	sqxc_ready(xcjsonc, NULL);

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = NULL;
	xcjsonc->value.string = json_array_string;
	xcjsonc->send(xcjsonc, xcjsonc);

	sqxc_finish(xcjsonc, NULL);
}

User* test_sqxc_jsonc_input_user()
{
	Sqxc* xcjsonc;
	Sqxc* xcvalue;
	User* user;

	xcjsonc = sqxc_new_input(SQXC_INFO_JSONC, SQXC_INFO_VALUE);
	xcvalue = sqxc_get(xcjsonc, SQXC_INFO_VALUE, 0);
	sqxc_value_type(xcvalue) = (SqType*)&UserType;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcjsonc, NULL);

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = NULL;
	xcjsonc->value.string = json_object_string;
	xcjsonc->send(xcjsonc, xcjsonc);

	sqxc_finish(xcjsonc, NULL);

	user = sqxc_value_instance(xcvalue);
	print_user(user);

	return user;
}

// ----------------------------------------------------------------------------
// Sqxc - Output

void test_sqxc_jsonc_output(User* instance)
{
	Sqxc* xcjsonc;

	xcjsonc = sqxc_new_output(SQXC_INFO_JSONC, SQXC_INFO_EMPTY);
	sqxc_ready(xcjsonc, NULL);

	xcjsonc->type = SQXC_TYPE_OBJECT;
	xcjsonc->name = NULL;
	xcjsonc->value.pointer = NULL;
	xcjsonc->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_INT;
	xcjsonc->name = "id";
	xcjsonc->value.integer = instance->id;
	xcjsonc->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = "email";
	xcjsonc->value.string = instance->email;
	xcjsonc->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_OBJECT_END;
	xcjsonc->name = NULL;
	xcjsonc->value.pointer = NULL;
	xcjsonc->send(xcjsonc, xcjsonc);

	sqxc_finish(xcjsonc, NULL);
}

void test_sqxc_output(SqSchema* schema, User* instance)
{
	Sqxc*    cxchain;
	Sqxc*    xcsql;
	Sqxc*    cxvalue;
	SqTable* table;

	table = sq_schema_find(schema, "users");

	cxchain = sqxc_new_output(SQXC_INFO_VALUE, SQXC_INFO_SQLITE);
	xcsql   = sqxc_get(cxchain, SQXC_INFO_SQLITE, -1);
	cxvalue = sqxc_get(cxchain, SQXC_INFO_VALUE, -1);

	sqxc_value_type(cxvalue) = table->type;
	xcsql->ctrl(xcsql, SQXC_SQL_USE_UPDATE, table);
	xcsql->ctrl(xcsql, SQXC_SQL_USE_WHERE, "id=1");

	sqxc_ready(cxvalue, NULL);
	cxvalue->type = SQXC_TYPE_OBJECT;
	cxvalue->name = table->name;
	cxvalue->value.pointer = instance;
	cxvalue->code = cxvalue->send(cxvalue, cxvalue);
	sqxc_finish(cxvalue, NULL);

	puts(xcsql->buf);
}

// ----------------------------------------------------------------------------


int  main(void)
{
	SqSchema*   schema;
	User*       user;

	schema = sq_schema_new("default");
	create_user_table_by_type(schema);

	user = malloc(sizeof(User));
	user->id = 10;
	user->name = "Bob";
	user->email = "guest@";
	sq_ptr_array_init(&user->strs, 8, NULL);
	sq_ptr_array_append(&user->strs, strdup("first"));
	sq_ptr_array_init(&user->ints, 8, NULL);
	sq_ptr_array_append(&user->ints, (void*)(intptr_t)1);

//	test_sqxc_output(schema, user);
	test_sqxc_jsonc_output(user);
	test_sqxc_jsonc_input();
	test_sqxc_jsonc_input_user();

//	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
	return 0;
}
