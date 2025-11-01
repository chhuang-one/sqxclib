/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

/*
	This example code use C99 designated initializer to define constant columns in tables.

	storage_make_migrated_schema() and city_add_column_dynamically() are
	example code for dynamically defining columns in cities table.
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>        // printf(), fprintf(), stderr

#include <sqxclib.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

typedef struct Post     Post;
typedef struct City     City;
typedef struct User     User;

struct Post
{
	char  *title;
	char  *desc;
};

struct City
{
	int    id;
	char  *name;
	bool   visited;
};

struct User
{
	int    id;         // primary key
	char  *name;
	char  *email;
	int    city_id;    // foreign key

	char  *comment;          // SQL Type: TEXT

	// If you use SQLite or MySQL to store binary data in database column,
	// Please make sure that SQ_CONFIG_QUERY_ONLY_COLUMN is enabled.
	// If you use PostgreSQL to do this, you don't need to care about SQ_CONFIG_QUERY_ONLY_COLUMN.
	SqBuffer       picture;  // SQL Type: BLOB, BINARY, etc.

	// make sure that SQ_CONFIG_HAVE_JSONC is enabled if you want to store array/object in database column
	SqIntArray     ints;     // integer array  (JSON array  in database column)
	Post          *post;     // object pointer (JSON object in database column)

	time_t         created_at;
	time_t         updated_at;

	// add, drop, and rename
	unsigned int   test_add;
	unsigned int   test_drop;
	unsigned int   test_rename;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to define object (JSON object in database column)
// SqType for structure Post. It also work if SqEntry is replaced by SqColumn.

// If you define constant SqType for structure, it must use with pointer array of SqEntry.
static const SqEntry *postEntryPointers[] = {
	&(SqEntry) {SQ_TYPE_STR,    "title",      offsetof(Post, title),     0},
	&(SqEntry) {SQ_TYPE_STR,    "desc",       offsetof(Post, desc),      0},
};

static const SqType   typePost = SQ_TYPE_INITIALIZER(Post, postEntryPointers, 0);
#define SQ_TYPE_POST &typePost

// ----------------------------------------------------------------------------
// use C99 designated initializer to define table and column

// Define SqEntry array (NOT pointer array) because it use with dynamic SqType (in SqTable).

// CREATE TABLE "cities"
static const SqColumn cityColumnsVer1[] = {
	{SQ_TYPE_INT,    "id",        offsetof(City, id),        SQB_PRIMARY | SQB_AUTOINCREMENT | SQB_HIDDEN},
	{SQ_TYPE_STR,    "name",      offsetof(City, name),      SQB_NULLABLE},

	// This column will be added in schema Ver5.
//	{SQ_TYPE_BOOL,   "visited",   offsetof(City, visited),   SQB_NULLABLE},
};

// CREATE TABLE "users"
static const SqColumn userColumnsVer1[] = {
	{SQ_TYPE_INT,    "id",        offsetof(User, id),        SQB_PRIMARY | SQB_AUTOINCREMENT | SQB_HIDDEN},
	{SQ_TYPE_STR,    "name",      offsetof(User, name),      0},
	{SQ_TYPE_STR,    "email",     offsetof(User, email),     0,
		.size = 60},

	// FOREIGN KEY (city_id) REFERENCES cities(id)  ON DELETE CASCADE  ON UPDATE CASCADE
	{SQ_TYPE_INT,    "city_id",   offsetof(User, city_id),   SQB_FOREIGN,
		.foreign = (char *[]) {"cities", "id",  "",  "CASCADE", "CASCADE", NULL} },

	// type mapping: SQ_TYPE_STR map to SQL data type - TEXT
	{SQ_TYPE_STR,    "comment",   offsetof(User, comment),   0,
		.sql_type = SQ_SQL_TYPE_TEXT},

#if SQ_CONFIG_QUERY_ONLY_COLUMN
	// get length of picture and set it to SqBuffer::size before parsing picture.
	// This is mainly used by SQLite, MySQL to get length of BLOB column.
	// If you use PostgreSQL and don't need store result of special query to C structure's member,
	// you can disable SQ_CONFIG_QUERY_ONLY_COLUMN.
	// ---
	// If you join multiple tables using the same column name and use query-only column with function,
	// you can add table name as prefix in column name of function parameter.
//	{SQ_TYPE_INT,    "length(users.picture)", offsetof(User, picture.size), SQB_QUERY_ONLY},
	{SQ_TYPE_INT,    "length(picture)",       offsetof(User, picture.size), SQB_QUERY_ONLY},
#endif
	{SQ_TYPE_BUFFER, "picture",               offsetof(User, picture),                            0,
		.sql_type = SQ_SQL_TYPE_BLOB},

#if SQ_CONFIG_HAVE_JSONC
	{SQ_TYPE_INT_ARRAY,    "ints",  offsetof(User, ints),    0},
	{SQ_TYPE_POST,         "post",  offsetof(User, post),    SQB_POINTER | SQB_NULLABLE},    // User.post is pointer
#endif

	// CONSTRAINT users_city_id_foreign
	// FOREIGN KEY (city_id) REFERENCES cities(id)  ON DELETE NO ACTION  ON UPDATE NO ACTION
	{SQ_TYPE_CONSTRAINT,   "users_city_id_foreign",    0,    SQB_FOREIGN,
		.foreign   = (char *[]) {"cities", "id",  "",  "NO ACTION", "NO ACTION", NULL},
		.composite = (char *[]) {"city_id", NULL} },

	// created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at",   offsetof(User, created_at),  SQB_CURRENT},
	// updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at",   offsetof(User, updated_at),  SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// This column will be deleted in schema Ver3.
	{SQ_TYPE_UINT,   "test_drop",   offsetof(User, test_drop),   0},

	// This column will be renamed in schema Ver4.
	{SQ_TYPE_UINT,   "test_rename", offsetof(User, test_rename), 0},
};


// ALTER TABLE "users"
static const SqColumn userColumnsVer2[] = {
	// ADD COLUMN "test_add"
	{SQ_TYPE_UINT,   "test_add",  offsetof(User, test_add),  SQB_NULLABLE},

	// ADD INDEX
	{SQ_TYPE_INDEX,  "id_city_id_index",
		.composite = (char *[]) {"id", "city_id", NULL} },

/*
	// ADD CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "id_city_id_unique", 0,  SQB_UNIQUE,
		.composite = (char *[]) {"id", "city_id", NULL} },

	// ADD CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "id_city_id_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"id", "city_id", NULL} },
 */
};

// ALTER TABLE "users"
static const SqColumn userColumnsVer3[] = {
	// DROP COLUMN "test_drop"
	{.old_name = "test_drop",     .name = NULL },

	// DROP CONSTRAINT FOREIGN KEY "users_city_id_foreign"
	{.old_name = "users_city_id_foreign",     .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_FOREIGN },

/*
	// DROP INDEX "id_city_id_index"
	{.old_name = "id_city_id_index",  .name = NULL,
	 .type = SQ_TYPE_INDEX },

	// DROP CONSTRAINT UNIQUE "id_city_id_unique"
	{.old_name = "id_city_id_unique",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_UNIQUE },

	// DROP CONSTRAINT PRIMARY KEY "id_city_id_primary"
	{.old_name = "id_city_id_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_PRIMARY },
 */
};

// ALTER TABLE "users"
static const SqColumn userColumnsVer4[] = {
	// RENAME COLUMN "test_rename" TO "test_rename2"
	{.old_name = "test_rename",   .name = "test_rename2" },

//	{.old_name = "ints",   .name = "test_ints" },
};

// ----------------------------------------------------------------------------
// C Functions for City & User

User *user_new(void)
{
	User *user;

	user = calloc(1, sizeof(User));
//	user->post = calloc(1, sizeof(Post));
	sq_buffer_init(&user->picture);
	sq_int_array_init(&user->ints, 8);
	return user;
}

void user_free(User *user)
{
	sq_int_array_final(&user->ints);
	free(user->name);
	free(user->email);
	free(user->comment);
	sq_buffer_final(&user->picture);
	if (user->post) {
		free(user->post->title);
		free(user->post->desc);
		free(user->post);
	}
	free(user);
}

void user_print(User *user)
{
	printf("user.id = %d\n"
	       "user.name = %s\n"
	       "user.email = %s\n"
	       "user.city_id = %d\n"
	       "user.comment = %s\n",
	       user->id,
	       user->name,
	       user->email,
	       user->city_id,
	       user->comment);

	size_t  hex_size = user->picture.writed * 2;
	char   *hex_mem  = malloc(hex_size + 1);
	hex_mem[hex_size] = 0;
	sq_bin_to_hex(hex_mem, user->picture.mem, user->picture.writed);
	printf("user.picture has %d bytes\n"
	       "user.picture = 0x%*s\n",
	       (int)user->picture.writed,
	       (int)hex_size, hex_mem);
	free(hex_mem);

	printf("user.ints[] = ");
	for (unsigned int i = 0;  i < user->ints.length;  i++) {
		if (i > 0)
			printf(",");
		printf("%d", user->ints.data[i]);
	}
	printf("\n" "user.post = 0x%p\n", user->post);
	if (user->post) {
		printf("user.post.title = %s\n"
		       "user.post.desc = %s\n",
		       user->post->title, user->post->desc);
	}

	char *timestr;
	timestr = sq_time_to_string(user->created_at, 0);
	printf("user.created_at = %s\n", timestr);
	free(timestr);
	timestr = sq_time_to_string(user->updated_at, 0);
	printf("user.updated_at = %s\n", timestr);
	free(timestr);

	printf("user.test_add = %d\n"
	       "user.test_drop = %d\n"
	       "user.test_rename = %d\n",
	       user->test_add, user->test_drop, user->test_rename);
	puts("");
}

City *city_new(void)
{
	return calloc(1, sizeof(City));
}

void city_free(City *city)
{
	free(city->name);
	free(city);
}

void city_print(City *city)
{
	printf("city.id = %d\n"
	       "city.name = %s\n"
	       "city.visited = %d\n",
	       city->id,
	       city->name,
	       city->visited);
	puts("");
}

// use C functions to define table and column dynamically
void city_add_column_dynamically(SqTable *city_table)
{
	SqColumn *column;

	column = sq_table_add_int(city_table, "id", offsetof(City, id));
	sq_column_primary(column);
	sq_column_hidden(column);
	sq_column_auto_increment(column);

	column = sq_table_add_string(city_table, "name", offsetof(City, name), 0);
	sq_column_nullable(column);

	// This column will be added in schema Ver5.
//	column = sq_table_add_bool(city_table, "visited", offsetof(City, visited));
//	sq_column_nullable(column);
}

// ----------------------------------------------------------------------------

void storage_make_migrated_schema(SqStorage *storage, int end_version)
{
	SqSchema *schema;
	SqColumn *column;
	SqTable  *table;

	if (end_version >= 1) {
		schema = sq_schema_new_ver(1, "Ver1");
//		schema->version = 1;
		// CREATE TABLE "cities"
		table = sq_schema_create(schema, "cities", City);
		sq_table_add_column(table, cityColumnsVer1, SQ_N_ELEMENTS(cityColumnsVer1));
//		city_add_column_dynamically(table);
		// CREATE TABLE "users"
		table = sq_schema_create(schema, "users", User);
		sq_table_add_column(table, userColumnsVer1, SQ_N_ELEMENTS(userColumnsVer1));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 2) {
		schema = sq_schema_new_ver(2, "Ver2");
//		schema->version = 2;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column(table, userColumnsVer2, SQ_N_ELEMENTS(userColumnsVer2));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 3) {
		schema = sq_schema_new_ver(3, "Ver3");
//		schema->version = 3;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column(table, userColumnsVer3, SQ_N_ELEMENTS(userColumnsVer3));
		// add "test_drop" after dropping "test_drop"
		column = sq_table_add_uint(table, "test_drop", offsetof(User, test_drop));
		sq_column_nullable(column);
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 4) {
		schema = sq_schema_new_ver(4, "Ver4");
//		schema->version = 4;
		// ALTER TABLE "users"
		table = sq_schema_alter(schema, "users", NULL);
		sq_table_add_column(table, userColumnsVer4, SQ_N_ELEMENTS(userColumnsVer4));
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 5) {
		schema = sq_schema_new_ver(5, "Ver5");
//		schema->version = 5;
		// RENAME TABLE "users" TO "users2"
		sq_schema_rename(schema, "users", "users2");
		// ALTER TABLE "cities" ADD COLUMN "visited"    (dynamic)
		table = sq_schema_alter(schema, "cities", NULL);
		column = sq_table_add_bool(table, "visited", offsetof(City, visited));
		sq_column_nullable(column);
		// migrate changed of "users" and "cities"
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	if (end_version >= 6) {
		schema = sq_schema_new_ver(6, "Ver6");
//		schema->version = 6;
		// DROP TABLE "users2"
		sq_schema_drop(schema, "users2");
		// migrate
		sq_storage_migrate(storage, schema);
		sq_schema_free(schema);
	}

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort schema in SqStorage and synchronize schema to database (mainly for SQLite).
	sq_storage_migrate(storage, NULL);
}

// sq_storage_query() doesn't use SqStorage::joint_default because only 1 table in query.
void  storage_query_ptr_array(SqStorage *storage)
{
	SqPtrArray *array;
	SqQuery    *query;
	User       *user;

	query = sq_query_new(NULL);
//	sq_query_select(query, "id", "name");
	sq_query_from(query, "users");

	array = sq_storage_query(storage, query, NULL, NULL);
	if (array) {
		for (unsigned int i = 0;  i < array->length;  i++) {
			user = (User*)array->data[i];
			user_print(user);
			user_free(user);
		}
		sq_ptr_array_free(array);
	}

	sq_query_free(query);
}

// sq_storage_query() use SqStorage::joint_default because multiple tables in query.
void  storage_query_join_array(SqStorage *storage)
{
	typedef void *Joint2[2];
	SqArray    *array;
	SqQuery    *query;
	void      **element;
	City       *city;
	User       *user;

	query = sq_query_new(NULL);
//	sq_query_select(query, "cities.id AS 'cities.id'", "users.id AS 'users.id'");
	sq_query_from(query, "cities");
	if (storage->db->version < 5)
		sq_query_join(query, "users",  "cities.id", "=", "%s", "users.city_id");
	else {
		// TABLE "users" was renamed to "users2" in schema version 5
		// DROP TABLE "user2" in schema version 6
		sq_query_join(query, "users2", "cities.id", "=", "%s", "users2.city_id");
	}

	array = sq_storage_query(storage, query, NULL, SQ_TYPE_ARRAY);
	if (array) {
		for (unsigned int i = 0;  i < array->length;  i++) {
			element = sq_array_at(array, Joint2, i);
			city = (City*)element[0];
			city_print(city);
			city_free(city);
			user = (User*)element[1];
			user_print(user);
			user_free(user);
		}
		sq_array_free(array);
	}

	sq_query_free(query);
}

void  storage_query_join_ptr_array(SqStorage *storage)
{
	SqPtrArray *array;
	SqQuery    *query;
	void      **element;
	City       *city;
	User       *user;

	query = sq_query_new(NULL);
//	sq_query_select(query, "cities.id AS 'cities.id'", "users.id AS 'users.id'");
	sq_query_from(query, "cities");
	if (storage->db->version < 5)
		sq_query_join(query, "users",  "cities.id", "=", "%s", "users.city_id");
	else {
		// TABLE "users" was renamed to "users2" in schema version 5
		// DROP TABLE "user2" in schema version 6
		sq_query_join(query, "users2", "cities.id", "=", "%s", "users2.city_id");
	}

	array = sq_storage_query(storage, query, NULL, NULL);
	if (array) {
		for (unsigned int i = 0;  i < array->length;  i++) {
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
	}

	sq_query_free(query);
}

// ----------------------------------------------------------------------------

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
SqdbConfigSqlite db_config_sqlite = {
//	.bit_field = SQDB_CONFIG_NO_MIGRATION,
//	.folder = "/tmp",
	.folder = ".",
	.extension = "db",
};

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
SqdbConfigMysql  db_config_mysql = {
//	.bit_field = SQDB_CONFIG_NO_MIGRATION,
	.host      = "localhost",
	.port      = 3306,
	.user      = "root",
	.password  = "",
};

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
SqdbConfigPostgre  db_config_postgre = {
//	.bit_field = SQDB_CONFIG_NO_MIGRATION,
	.host      = "localhost",
	.port      = 5432,
	.user      = "postgres",
	.password  = "",
};

#endif

int  main(void)
{
	Sqdb       *db;
	SqStorage  *storage;
	SqPtrArray *array;
	City       *city;
	User       *user;

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	db = sqdb_sqlite_new(&db_config_sqlite);
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &db_config_sqlite);
#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
	db = sqdb_mysql_new(&db_config_mysql);
//	db = sqdb_new(SQDB_INFO_MYSQL,  (SqdbConfig*) &db_config_mysql);
#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
	db = sqdb_postgre_new(&db_config_postgre);
//	db = sqdb_new(SQDB_INFO_POSTGRE, (SqdbConfig*) &db_config_postgre);
#else
	fprintf(stderr, "No supported database\n");
	return EXIT_SUCCESS;
#endif

	storage = sq_storage_new(db);

	if (sq_storage_open(storage, "sample-c99") != SQCODE_OK) {
		fprintf(stderr, "Can't open database - %s\n", "sample-c99");
		return EXIT_FAILURE;
	}

	// This program migrate to next version every run. (from Ver1 to Ver6)
	storage_make_migrated_schema(storage, db->version +1);

	if (storage->schema->version == 1) {
		city = city_new();
//		city->id = 1;
		city->name = strdup("Los Angeles");
		city->visited = false;
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		city = city_new();
//		city->id = 2;
		city->name = NULL;
//		city->name = strdup("San Francisco");
		city->visited = true;
		sq_storage_insert(storage, "cities", NULL, city);
		city_free(city);

		user = user_new();
//		user->id = 1;
		user->city_id = 1;
		user->name = strdup("Paul");
		user->email = strdup("guest@");
		user->comment = strdup("-- comment text 1");
		sq_buffer_write_c(&user->picture, 0xFE);
		sq_buffer_write_c(&user->picture, 0x5A);
		sq_buffer_write_c(&user->picture, 0x00);
		sq_buffer_write(&user->picture, "test picture 1");
		sq_int_array_push(&user->ints, 3);
		sq_int_array_push(&user->ints, 6);
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
		user->id = 1;
		user->email = strdup("paul@sqxc");
		sq_storage_update(storage, "users", NULL, user);
		// free 'user' after inserting and updating
		user_free(user);
	}

	array = sq_storage_get_all(storage, "cities", NULL, NULL, NULL);
	if (array) {
		for (unsigned int i = 0;  i < array->length;  i++) {
			city = array->data[i];
			city_print(city);
			city_free(city);
		}
		sq_ptr_array_free(array);
	}

	// TABLE "users" was renamed to "users2" in schema version 5
	// DROP TABLE "user2" in schema version 6
	if (db->version >= 5)
		user = sq_storage_get(storage, "users2", NULL, 1);
	else
		user = sq_storage_get(storage, "users", NULL, 1);
	if (user) {
		user_print(user);
		user_free(user);
	}

	storage_query_ptr_array(storage);
	storage_query_join_array(storage);
	storage_query_join_ptr_array(storage);

	sq_storage_close(storage);

	// free instance
	sq_storage_free(storage);
	sqdb_free(db);

	return EXIT_SUCCESS;
}
