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

/*
	This example code use C99 designated initializer to declare table/column.
 */

#include <stdio.h>

#include <SqdbSqlite.h>
#include <SqStorage.h>

typedef struct City     City;
typedef struct User     User;

struct City {
	int    id;
	char*  name;
};

struct User {
	int    id;         // primary key
	char*  name;
	char*  email;
	int    city_id;    // foreign key

	// make sure that SQ_CONFIG_JSON_SUPPORT is enabled if you want to store array in SQL column
	SqIntptrArray  ints;    // intptr_t array

    // add/drop/rename
	unsigned int test_add;
    unsigned int test_drop;
    unsigned int test_rename;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to declare table/column

// CREATE TABLE "cities"
static const SqColumn* CityColumnsVer1[] = {
	&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(City, id),        SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_STRING, "name",      offsetof(City, name),      0},
};

// CREATE TABLE "users"
static const SqColumn* UserColumnsVer1[] = {
	&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(User, id),        SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_STRING, "name",      offsetof(User, name),      0},
	&(SqColumn) {SQ_TYPE_STRING, "email",     offsetof(User, email),     0},
    // FOREIGN KEY
	&(SqColumn) {SQ_TYPE_INT,    "city_id",   offsetof(User, city_id),   0,
                 .foreign = &(SqForeign) {"cities",    "id",  "CASCADE",  "CASCADE"} },

#ifdef SQ_CONFIG_JSON_SUPPORT
	&(SqColumn) {SQ_TYPE_INTPTR_ARRAY, "ints",offsetof(User, ints),      0},
#endif

	// This column will be deleted in Ver3
	&(SqColumn) {SQ_TYPE_INT,    "test_drop", offsetof(User, test_drop), 0},

	// This column will be renamed in Ver4
	&(SqColumn) {SQ_TYPE_INT,    "test_rename", offsetof(User, test_rename), 0},
};


// ALTER TABLE "users"
static const SqColumn* UserColumnsVer2[] = {
    // ADD COLUMN "test_add"
	&(SqColumn) {SQ_TYPE_INT,    "test_add",  offsetof(User, test_add),  0},
};

// ALTER TABLE "users"
static const SqColumn* UserColumnsVer3[] = {
    // DROP COLUMN "test_drop"
	&(SqColumn) {.old_name = "test_drop",     .name = NULL },
};

// ALTER TABLE "users"
static const SqColumn* UserColumnsVer4[] = {
    // RENAME COLUMN "test_rename"  TO "test_rename2"
	&(SqColumn) {.old_name = "test_rename",   .name = "test_rename2" },
};

// ----------------------------------------------------------------------------
// C Functions for City & User

User* user_new(void) {
	User* user;

	user = calloc(1, sizeof(User));
	sq_intptr_array_init(&user->ints, 8);
	return user;
}

void user_free(User* user) {
	sq_ptr_array_final(&user->ints);
	free(user->name);
	free(user->email);
	free(user);
}

void user_print(User* user) {
	printf("\n"
	       "user.id = %d\n"
	       "user.name = %s\n"
	       "user.email = %s\n"
	       "user.city_id = %d\n",
	       user->id, user->name, user->email, user->city_id);
	printf("user.ints[] = ");
	for (int i = 0;  i < user->ints.length;  i++) {
		if (i > 0)
			printf(",");
		printf("%d", (int)user->ints.data[i]);
	}
	printf("\n"
	       "user.test_add = %d\n"
	       "user.test_drop = %d\n"
	       "user.test_rename = %d\n",
	       user->test_add, user->test_drop, user->test_rename);
}

City* city_new(void) {
	return calloc(1, sizeof(City));
}

void city_free(City* city) {
	free(city->name);
	free(city);
}

void city_print(City* city) {
	printf("\n"
	       "city.id = %d\n"
	       "city.name = %s\n",
	       city->id, city->name);
}

// ----------------------------------------------------------------------------

void storage_make_migrated_schema(SqStorage* storage, int end_version)
{
	SqSchema* schema;

	if (end_version >= 1) {
		schema = sq_schema_new("Ver1");
//		schema->version = 1;
		// CREATE TABLE "cities"
		sq_schema_create_from_columns(schema, "cities", SQ_GET_TYPE_NAME(City),
		                              CityColumnsVer1, SQ_N_ENTRY(CityColumnsVer1));
		// CREATE TABLE "users"
		sq_schema_create_from_columns(schema, "users", SQ_GET_TYPE_NAME(User),
		                              UserColumnsVer1, SQ_N_ENTRY(UserColumnsVer1));
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 2) {
		schema = sq_schema_new("Ver2");
//		schema->version = 2;
		// ALTER TABLE "users"
		sq_schema_alter_from_columns(schema, "users", UserColumnsVer2,
		                             SQ_N_ENTRY(UserColumnsVer2));
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 3) {
		schema = sq_schema_new("Ver3");
//		schema->version = 3;
		// ALTER TABLE "users"
		sq_schema_alter_from_columns(schema, "users", UserColumnsVer3,
		                             SQ_N_ENTRY(UserColumnsVer3));
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 4) {
		schema = sq_schema_new("Ver4");
//		schema->version = 4;
		// ALTER TABLE "users"
		sq_schema_alter_from_columns(schema, "users", UserColumnsVer4,
		                             SQ_N_ENTRY(UserColumnsVer4));
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	// End of migration. create SQL tables based on storage->schema
	sq_storage_migrate(storage, NULL);
}

int  main(void)
{
	Sqdb*       db;
	SqStorage*  storage;
	SqPtrArray* array;
	City*       city;
	User*       user;

	db = sqdb_new(SQDB_INFO_SQLITE, NULL);
	storage = sq_storage_new(db);
	sq_storage_open(storage, "sample-c99");

	// This program migrate to next version every run. (from Ver1 to Ver4)
	storage_make_migrated_schema(storage, ((SqdbSqlite*)db)->version +1);

	if (storage->schema->version == 1) {
		city = city_new();
		city->id = 1;
		city->name = strdup("Los Angeles");
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		city = city_new();
		city->id = 2;
		city->name = strdup("San Francisco");
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		user = user_new();
		user->id = 1;
		user->city_id = 1;
		user->name = strdup("Paul");
		user->email = strdup("guest@");
		sq_ptr_array_append(&user->ints, (intptr_t)3);
		sq_ptr_array_append(&user->ints, (intptr_t)6);
		user->test_add = 0;
		user->test_drop = 0;
		user->test_rename = 0;
		sq_storage_insert(storage, "users", NULL, user);
		user_free(user);
	}

	array = sq_storage_get_all(storage, "cities", NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		city = array->data[i];
		city_print(city);
		city_free(city);
	}
	sq_ptr_array_free(array);

	user = sq_storage_get(storage, "users", NULL, 1);
	user_print(user);
	user_free(user);

	sq_storage_close(storage);
	return EXIT_SUCCESS;
}
