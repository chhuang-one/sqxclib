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

/*
	This example code use C99 designated initializer to define table/column.
 */

#include <stdio.h>

#include <SqdbSqlite.h>
#include <SqStorage.h>
#include <SqQuery.h>

typedef struct Post     Post;
typedef struct City     City;
typedef struct User     User;

struct Post {
	char*  title;
	char*  desc;
};

struct City {
	int    id;
	char*  name;
	bool   visited;
};

struct User {
	int    id;         // primary key
	char*  name;
	char*  email;
	int    city_id;    // foreign key

	// make sure that SQ_CONFIG_JSON_SUPPORT is enabled if you want to store array/object in SQL column
	SqIntptrArray  ints;    // intptr_t array (JSON array  in SQL column)
	Post*          post;    // object pointer (JSON object in SQL column)

	// add, drop, and rename
	unsigned int   test_add;
	unsigned int   test_drop;
	unsigned int   test_rename;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to define object (JSON object in SQL column)

// SqType for structure Post. It also work if SqEntry is replaced by SqColumn.
static const SqEntry* PostEntry[] = {
	&(SqEntry) {SQ_TYPE_STRING, "title",      offsetof(Post, title),     0},
	&(SqEntry) {SQ_TYPE_STRING, "desc",       offsetof(Post, desc),      0},
};
static SqType         type_post = SQ_TYPE_INITIALIZER(Post, PostEntry, 0);
#define SQ_TYPE_POST &type_post

// ----------------------------------------------------------------------------
// use C99 designated initializer to define table/column

// CREATE TABLE "cities"
static const SqColumn* CityColumnsVer1[] = {
	&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(City, id),        SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_STRING, "name",      offsetof(City, name),      SQB_NULLABLE},
//	&(SqColumn) {SQ_TYPE_BOOL,   "visited",   offsetof(City, visited)},
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
	&(SqColumn) {SQ_TYPE_INTPTR_ARRAY, "ints",  offsetof(User, ints),    0},
	&(SqColumn) {SQ_TYPE_POST,         "post",  offsetof(User, post),    SQB_POINTER | SQB_NULLABLE},    // User.post is pointer
#endif

	// CONSTRAINT FOREIGN KEY
	&(SqColumn) {SQ_TYPE_CONSTRAINT,   "users_city_id_foreign",
	             .foreign = &(SqForeign) {"cities", "id", "NO ACTION", "NO ACTION"},
	             .composite = (char *[]) {"city_id", NULL} },

	// This column will be deleted in Ver3
	&(SqColumn) {SQ_TYPE_UINT,   "test_drop",   offsetof(User, test_drop),   0},

	// This column will be renamed in Ver4
	&(SqColumn) {SQ_TYPE_UINT,   "test_rename", offsetof(User, test_rename), 0},
};


// ALTER TABLE "users"
static const SqColumn* UserColumnsVer2[] = {
    // ADD COLUMN "test_add"
	&(SqColumn) {SQ_TYPE_UINT,   "test_add",  offsetof(User, test_add),  0},
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
//	user->post = calloc(1, sizeof(Post));
	sq_intptr_array_init(&user->ints, 8);
	return user;
}

void user_free(User* user) {
	sq_ptr_array_final(&user->ints);
	free(user->name);
	free(user->email);
	if (user->post) {
		free(user->post->title);
		free(user->post->desc);
		free(user->post);
	}
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
	printf("\n" "user.post = 0x%p\n", user->post);
	if (user->post) {
		printf("user.post.title = %s\n"
		       "user.post.desc = %s\n",
		       user->post->title, user->post->desc);
	}
	printf("user.test_add = %d\n"
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
	SqTable*  table;

	if (end_version >= 1) {
		schema = sq_schema_new("Ver1");
//		schema->version = 1;
		// CREATE TABLE "cities"
		table = sq_schema_create(schema, "cities", City);
		sq_table_add_column_ptrs(table, CityColumnsVer1, SQ_N_PTRS(CityColumnsVer1));
		// CREATE TABLE "users"
		table = sq_schema_create(schema, "users", User);
		sq_table_add_column_ptrs(table, UserColumnsVer1, SQ_N_PTRS(UserColumnsVer1));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 2) {
		schema = sq_schema_new("Ver2");
//		schema->version = 2;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column_ptrs(table, UserColumnsVer2, SQ_N_PTRS(UserColumnsVer2));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 3) {
		schema = sq_schema_new("Ver3");
//		schema->version = 3;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column_ptrs(table, UserColumnsVer3, SQ_N_PTRS(UserColumnsVer3));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 4) {
		schema = sq_schema_new("Ver4");
//		schema->version = 4;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column_ptrs(table, UserColumnsVer4, SQ_N_PTRS(UserColumnsVer4));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 5) {
		schema = sq_schema_new("Ver5");
//		schema->version = 5;
		// RENAME TABLE "users" TO "users2"
		sq_schema_rename(schema, "users", "users2");
		// ALTER TABLE "cities" ADD COLUMN "visited"    (dynamic)
		table = sq_schema_alter(schema, "cities", NULL);
		sq_table_add_bool(table, "visited", offsetof(City, visited));
		// migrate changed of "users" and "cities"
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 6) {
		schema = sq_schema_new("Ver6");
//		schema->version = 6;
		// DROP TABLE "users2"
		sq_schema_drop(schema, "users2");
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	// End of migration. create SQL tables based on storage->schema
	sq_storage_migrate(storage, NULL);
}

void  storage_query(SqStorage* storage)
{
	SqPtrArray* array;
	SqQuery*    query;
	void**      element;
	City*       city;
	User*       user;

	query = sq_query_new(NULL);
//	sq_query_select(query, "cities.id AS 'cities.id'", "users.id AS 'users.id'", NULL);
	sq_query_from(query, "cities");
	sq_query_join(query, "users",  "cities.id", "users.city_id");

	array = sq_storage_query(storage, query, NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		element = (void**)array->data[i];
		city = (City*)element[0];
		city_print(city);
		city_free(city);
		user = (User*)element[1];
		user_print(user);
		user_free(user);
		// free joint object
		free(element);
	}

	sq_ptr_array_free(array);
	sq_query_free(query);
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

	// This program migrate to next version every run. (from Ver1 to Ver6)
	storage_make_migrated_schema(storage, ((SqdbSqlite*)db)->version +1);

	if (storage->schema->version == 1) {
		city = city_new();
		city->id = 1;
		city->name = strdup("Los Angeles");
		city->visited = false;
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		city = city_new();
		city->id = 2;
		city->name = NULL;
//		city->name = strdup("San Francisco");
		city->visited = true;
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		user = user_new();
		user->id = 1;
		user->city_id = 1;
		user->name = strdup("Paul");
		user->email = strdup("guest@");
		sq_ptr_array_append(&user->ints, (intptr_t)3);
		sq_ptr_array_append(&user->ints, (intptr_t)6);
#if 1
		user->post = calloc(1, sizeof(Post));
		user->post->title = strdup("PostTitle");
		user->post->desc = strdup("PostDesc");
#endif
		user->test_add = 1;
		user->test_drop = 2;
		user->test_rename = 3;
		sq_storage_insert(storage, "users", NULL, user);

		// update User.email
		free(user->email);
		user->email = strdup("paul@sqxc");
		sq_storage_update(storage, "users", NULL, user);
		// free 'user' after inserting and updating
		user_free(user);
	}

	array = sq_storage_get_all(storage, "cities", NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		city = array->data[i];
		city_print(city);
		city_free(city);
	}
	sq_ptr_array_free(array);

	// TABLE "users" was renamed to "users2" in schema version 5
	// DROP TABLE "user2" in schema version 6
	if (((SqdbSqlite*)db)->version >= 5)
		user = sq_storage_get(storage, "users2", NULL, 1);
	else
		user = sq_storage_get(storage, "users", NULL, 1);
	if (user) {
		user_print(user);
		user_free(user);
	}

	storage_query(storage);

	sq_storage_close(storage);
	return EXIT_SUCCESS;
}
