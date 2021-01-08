/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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
#include <SqxcSql.h>
#include <SqPtrArray.h>
#include <SqSchema-macro.h>
#include <SqJoint.h>

// ----------------------------------------------------------------------------
// C structure

typedef struct User     User;

struct User {
	int    id;
	char*  name;
	char*  email;

	SqStringArray  strs;
	SqIntptrArray  ints;
};

// ----------------------------------------------------------------------------
// define static SqType and SqColumn.

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
	.entry   = (SqEntry**) UserColumns,
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
//	return sq_schema_create_full(schema, "users", NULL, &UserType, 0);
}

// ----------------------------------------------------------------------------
// Sqxc - Input

void test_sqxc_joint_input()
{
	SqJoint*  joint;
	SqTable*  table;
	Sqxc*     xc;
	User*     user;
	void**    instance;

	table = sq_table_new("users", &UserType);
	joint = sq_joint_new();
	sq_joint_add(joint, table, "tb1");
	sq_joint_add(joint, table, "tb2");

	xc = sqxc_new(SQXC_INFO_VALUE);
	sqxc_value_type(xc) = joint->type;

	sqxc_ready(xc, NULL);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT;
	sqxc_send(xc);

	xc->name = "tb1.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 1233;
	sqxc_send(xc);

	xc->name = "tb2.id";
	xc->type = SQXC_TYPE_INT;
	xc->value.int_ = 233;
	sqxc_send(xc);

	xc->name = NULL;
	xc->type = SQXC_TYPE_OBJECT_END;
	sqxc_send(xc);

	sqxc_finish(xc, NULL);
	instance = sqxc_value_instance(xc);
	user = instance[0];
	printf("tb1.id = %d\n", user->id);
	user = instance[1];
	printf("tb2.id = %d\n", user->id);

	sqxc_free(xc);
	sq_joint_free(joint);
	sq_table_free(table);
}

#ifdef SQ_CONFIG_JSON_SUPPORT

const char* json_array_string =
"["
	"{"
		"\"id\": 10,"
		"\"email\": \"guest@\","
		"\"strs\": [\"first\", \"second\"],"
		"\"ints\": [1, 2]"
	"}"
"]";

const char* json_object_string =
"{"
	"\"id\": 10,"
	"\"email\": \"guest@\","
	"\"strs\": [\"first\", \"second\"],"
	"\"ints\": [1, 2]"
"}";

void test_sqxc_jsonc_input()
{
	Sqxc* xcjsonc;
	Sqxc* xcchain;

	xcchain = sqxc_new_chain(SQXC_INFO_EMPTY, SQXC_INFO_JSONC_PARSER, NULL);
	xcjsonc = sqxc_find(xcchain, SQXC_INFO_JSONC_PARSER);

	sqxc_ready(xcchain, NULL);
	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = NULL;
	xcjsonc->value.string = (char*)json_array_string;
	xcjsonc->info->send(xcjsonc, xcjsonc);
	sqxc_finish(xcchain, NULL);

	sqxc_free_chain(xcchain);
}

User* test_sqxc_jsonc_input_user()
{
	Sqxc* xcjsonc;
	Sqxc* xcvalue;
	User* user;

	xcvalue = sqxc_new_chain(SQXC_INFO_VALUE, SQXC_INFO_JSONC_PARSER, NULL);
	xcjsonc = sqxc_find(xcvalue, SQXC_INFO_JSONC_PARSER);
	sqxc_value_type(xcvalue) = &UserType;
	sqxc_value_container(xcvalue) = NULL;

	sqxc_ready(xcvalue, NULL);

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = NULL;
	xcjsonc->value.string = (char*)json_object_string;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	sqxc_finish(xcvalue, NULL);

	user = (User*)sqxc_value_instance(xcvalue);
	print_user(user);

	return user;
}

// ----------------------------------------------------------------------------
// Sqxc - Output

void test_sqxc_jsonc_output(User* instance)
{
	Sqxc* xcjsonc;
	Sqxc* xcchain;

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

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = "name";
	xcjsonc->value.string = instance->name;
	xcjsonc->info->send(xcjsonc, xcjsonc);

	xcjsonc->type = SQXC_TYPE_STRING;
	xcjsonc->name = "email";
	xcjsonc->value.string = instance->email;
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
	SqTable* table;
	Sqxc* xcchain;
	Sqxc* xcsql;
	Sqxc* xccur;

	table = sq_table_new("User", &UserType);

	xcchain = sqxc_new_chain(SQXC_INFO_SQL, SQXC_INFO_JSONC_WRITER, NULL);
	xcsql = sqxc_find(xcchain, SQXC_INFO_SQL);
	if (use_update) {
		sqxc_sql_id(xcsql) = 2333;
		xcsql->info->ctrl(xcsql, SQXC_SQL_USE_UPDATE, table);
	}
	else
		xcsql->info->ctrl(xcsql, SQXC_SQL_USE_INSERT, table);

	sqxc_ready(xcchain, NULL);

	xccur = xcchain;
	xccur->type = SQXC_TYPE_OBJECT;
	xccur->name = NULL;
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_STRING;
	xccur->name = "email";
	xccur->value.string = (char*)"guest@";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_INT;
	xccur->name = "id";
	xccur->value.integer = 2333;
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_ARRAY;
	xccur->name = "ints";
	xccur = sqxc_send(xccur);
	xccur->type = SQXC_TYPE_ARRAY_END;
	xccur->name = "ints";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_STRING;
	xccur->name = "name";
	xccur->value.string = (char*)"Bob";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_ARRAY;
	xccur->name = "strs";
	xccur = sqxc_send(xccur);
	xccur->type = SQXC_TYPE_ARRAY_END;
	xccur->name = "strs";
	xccur = sqxc_send(xccur);

	xccur->type = SQXC_TYPE_OBJECT_END;
	xccur->name = NULL;
	xccur->value.pointer = NULL;
	xccur = sqxc_send(xccur);

	puts(xcsql->buf);
	sqxc_finish(xcchain, NULL);

	sqxc_free_chain(xcchain);
}

#endif  // SQ_CONFIG_JSON_SUPPORT

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

	test_sqxc_joint_input();
#ifdef SQ_CONFIG_JSON_SUPPORT
	test_sqxc_jsonc_input();
	test_sqxc_jsonc_input_user();
	test_sqxc_jsonc_output(user);
	test_sqxc_sql_output(true);
#endif  // SQ_CONFIG_JSON_SUPPORT

//	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
	return 0;
}
