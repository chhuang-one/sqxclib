/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#include <stdio.h>

#include <sqxclib.h>
#include <SqSchema-macro.h>

#define USE_SQLITE_IF_POSSIBLE    1

// ----------------------------------------------------------------------------
// C structure

typedef struct User     User;
typedef struct City     City;
typedef struct Company  Company;

struct User {
	int    id;
	char  *name;
	char  *email;
	int    city_id;
	int    company_id;

	SqIntptrArray   posts;

	unsigned int test_add;
};

struct City {
	int    id;
	char  *name;
};

struct Company {
	int    id;
	char  *name;
	int    city_id;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to define table and column

// --- UserColumns is sorted by programer... :)
static const SqColumn  *UserColumns[] = {
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE CASCADE ON UPDATE CASCADE
	&(SqColumn) {SQ_TYPE_INT,    "city_id", offsetof(User, city_id),    SQB_HIDDEN,
	             .foreign = &(SqForeign) {"cities", "id",  "CASCADE",  "CASCADE"} },
	// "company_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE CASCADE ON UPDATE CASCADE
	&(SqColumn) {SQ_TYPE_INT, "company_id", offsetof(User, company_id), SQB_HIDDEN,
	             .foreign = &(SqForeign) {"companies", "id",  "CASCADE",  "CASCADE"} },

	// "company_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE NO ACTION ON UPDATE NO ACTION
	&(SqColumn) {SQ_TYPE_INT, "company_test_id", offsetof(User, company_id), SQB_HIDDEN,
	             .foreign = &(SqForeign) {"companies", "id",  "NO ACTION",  "NO ACTION"} },

	// "email"  VARCHAR
	&(SqColumn) {SQ_TYPE_STRING, "email",   offsetof(User, email), SQB_HIDDEN_NULL},

	// CONSTRAINT FOREIGN KEY
	&(SqColumn) {SQ_TYPE_CONSTRAINT,  "fk_cities_id",
	             .foreign = &(SqForeign) {"cities", "id", "no action", "cascade"},
	             .composite = (char *[]) {"city_id", NULL} },
	// COLUMN
	&(SqColumn) {SQ_TYPE_INT,    "id",      offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
	&(SqColumn) {SQ_TYPE_STRING, "name",    offsetof(User, name),  0},
	&(SqColumn) {SQ_TYPE_INTPTR_ARRAY, "posts", offsetof(User, posts), 0},
};

// --- UserType use sorted UserColumns
const SqType UserType = SQ_TYPE_INITIALIZER(User, UserColumns, SQB_TYPE_SORTED);
/*
const SqType UserType = {
	.size  = sizeof(User),
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),
	.entry   = (SqEntry**) UserColumns,
	.n_entry = sizeof(UserColumns) / sizeof(SqColumn*),
	.bit_field  = SQB_TYPE_SORTED,                          // UserColumns is sorted
};
 */

/* ----------------------------------------------------------------------------
   use C99 designated initializer to define table and column changed (migration)
*/
static const SqColumn  *UserColumnsChange[] = {
	// ADD COLUMN "test_add"
	&(SqColumn) {SQ_TYPE_UINT, "test_add", offsetof(User, test_add), SQB_NULLABLE},

	// ALTER COLUMN "city_id"   (.bit_field = SQB_CHANGED)
//	&(SqColumn) {SQ_TYPE_INT,  "city_id", offsetof(User, city_id), SQB_CHANGED},

	// "company_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE NO ACTION ON UPDATE NO ACTION
	&(SqColumn) {SQ_TYPE_INT, "company_test_id", offsetof(User, company_id), SQB_NULLABLE | SQB_CHANGED },

	// DROP CONSTRAINT FOREIGN KEY "fk_cities_id"
	&(SqColumn) {SQ_TYPE_CONSTRAINT,  .old_name = "fk_cities_id",  .name = NULL,
	             .bit_field = SQB_FOREIGN},

	// DROP COLUMN "name"
	&(SqColumn) {.old_name = "name",   .name = NULL},

	// RENAME COLUMN "email"  TO "email2"
	&(SqColumn) {.old_name = "email",  .name = "email2"},

	// RENAME COLUMN "email2" TO "email3"
	&(SqColumn) {.old_name = "email2", .name = "email3"},
};

const SqType UserTypeChange = {
	.name  = SQ_GET_TYPE_NAME(User),
	.entry   = (SqEntry**) UserColumnsChange,
	.n_entry = sizeof(UserColumnsChange) / sizeof(SqColumn*)
};

// ------------------------------------

SqTable *create_user_table_by_type(SqSchema *schema)
{
	return sq_schema_create_by_type(schema, "users", &UserType);
//	return sq_schema_create_full(schema, "users", NULL, &UserType, 0);
}

SqTable *change_user_table_by_c_type(SqSchema *schema)
{
	return sq_schema_alter(schema, "users", &UserTypeChange);
}

// ----------------------------------------------------------------------------
// define table and column

SqTable *create_user_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

//	table = sq_schema_create(schema, "users", User);
	table = sq_schema_create_full(schema, "users", SQ_GET_TYPE_NAME(User), NULL, sizeof(User));

	column = sq_table_add_int(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");

	column = sq_table_add_int(table, "company_id", offsetof(User, city_id));
	sq_column_reference(column, "companies", "id");
	sq_column_on_delete(column, "cascade");

	column = sq_table_add_string(table, "email", offsetof(User, email), -1);

	column = sq_table_add_foreign(table, "fk_cities_id", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "no action");
	sq_column_on_update(column, "cascade");

	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_custom(table, "posts", offsetof(User, posts), SQ_TYPE_INTPTR_ARRAY, -1);

	return table;
}

SqTable *change_user_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	table = sq_schema_alter(schema, "users", NULL);
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	column->bit_field |= SQB_NULLABLE;
	sq_table_drop_foreign(table, "fk_cities_id");
	sq_table_drop_column(table, "name");
	sq_table_rename_column(table, "email", "email2");
	return table;
}

// ----------------------------------------------------------------------------
// use C macro to define dynamic table and column

void  create_user_table_by_macro(SqSchema *schema)
{
	SQ_SCHEMA_CREATE(schema, "users", User, {
		SQT_INTEGER_AS(User, id);  SQC_PRIMARY();  SQC_HIDDEN();
		SQT_STRING_AS(User, name, -1);
		SQT_STRING_AS(User, email, -1);
		SQT_INTEGER_AS(User, city_id);  SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("set null");
		SQT_CUSTOM_AS(User, posts, SQ_TYPE_INTPTR_ARRAY, -1);
	});

#if 0
	SQ_SCHEMA_CREATE(schema, "users", User, {
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();  SQC_HIDDEN();
		SQT_STRING("name", User, name, -1);
		SQT_STRING("email", User, email, -1);
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("set null");
		SQT_CUSTOM("posts", User, posts, SQ_TYPE_INTPTR_ARRAY);
	});
#endif
}

void  change_user_table_by_macro(SqSchema *schema)
{
	SQ_SCHEMA_ALTER(schema, "users", {
		SQT_UINT_AS(User, test_add);
		SQT_INTEGER_AS(User, city_id);  SQC_CHANGE();
		SQT_DROP("name");
		SQT_RENAME("email", "email2");
	});
}

// ----------------------------------------------------------------------------

void create_company_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	// change schema
	table = sq_schema_create(schema, "companies", Company);
	column = sq_table_add_integer(table, "id", offsetof(Company, id));
	sq_column_primary(column);
	column = sq_table_add_string(table, "name", offsetof(Company, name), 0);
	column = sq_table_add_integer(table, "city_id", offsetof(Company, city_id));
	sq_column_foreign(column, "cities", "id");
//	sq_column_on_delete(column, "set null");
}

void create_city_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	// change schema
	table = sq_schema_create(schema, "cities", City);
	column = sq_table_add_integer(table, "id", offsetof(City, id));
	sq_column_primary(column);
	column = sq_table_add_string(table, "name", offsetof(City, name), 0);
}

// ----------------------------------------------------------------------------

void test_sqdb_migrate(Sqdb *db)
{
	SqSchema   *schema;
	SqSchema   *schema_v1;
	SqSchema   *schema_v2;
	SqSchema   *schema_v3;
	SqSchema   *schema_v4;

	schema  = sq_schema_new("current");
	schema->version = 0;

	schema_v1  = sq_schema_new("ver1");
	schema_v1->version = 1;
	create_city_table_by_c(schema_v1);

	schema_v2 = sq_schema_new("ver2");
	schema_v2->version = 2;
	create_company_table_by_c(schema_v2);

	schema_v3 = sq_schema_new("ver3");
	schema_v3->version = 3;
	create_user_table_by_type(schema_v3);
//	create_user_table_by_macro(schema_v3);
//	create_user_table_by_c(schema_v3);

	schema_v4 = sq_schema_new("ver4");
	schema_v4->version = 4;
	change_user_table_by_c_type(schema_v4);
	// other testing in 'schema_v4'
	sq_schema_rename(schema_v4, "cities", "cities2");
//	sq_schema_drop(schema_v4, "users");

	sqdb_migrate(db, schema, schema_v1);
	sqdb_migrate(db, schema, schema_v2);
	sqdb_migrate(db, schema, schema_v3);
	sqdb_migrate(db, schema, schema_v4);
	sqdb_migrate(db, schema, NULL);

	sq_schema_free(schema);
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
	sq_schema_free(schema_v3);
	sq_schema_free(schema_v4);
}

void test_sqdb_migrate_sqlite_sync(Sqdb *db)
{
	SqSchema   *schema;
	SqSchema   *schema_v1;
	SqSchema   *schema_v2;
	SqSchema   *schema_v3;
	SqSchema   *schema_v4;

	schema  = sq_schema_new("current");
	schema->version = 0;

	schema_v1  = sq_schema_new("ver1");
	schema_v1->version = 1;
	create_user_table_by_type(schema_v1);
//	create_user_table_by_macro(schema_v1);
//	create_user_table_by_c(schema_v1);

	schema_v2 = sq_schema_new("ver2");
	schema_v2->version = 2;
	change_user_table_by_c_type(schema_v2);

	schema_v3 = sq_schema_new("ver3");
	schema_v3->version = 3;
	create_company_table_by_c(schema_v3);

	schema_v4 = sq_schema_new("ver4");
	schema_v4->version = 4;
	create_city_table_by_c(schema_v4);
	// other testing in 'schema_v4'
	sq_schema_rename(schema_v4, "cities", "cities2");
//	sq_schema_drop(schema_v4, "users");

	sqdb_migrate(db, schema, schema_v1);
	sqdb_migrate(db, schema, schema_v2);
	sqdb_migrate(db, schema, schema_v3);
	sqdb_migrate(db, schema, schema_v4);
	sqdb_migrate(db, schema, NULL);

	sq_schema_free(schema);
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
	sq_schema_free(schema_v3);
	sq_schema_free(schema_v4);
}

// ----------------------------------------------------------------------------

int  main(void)
{
	Sqdb   *db;

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	db = sqdb_new(SQDB_INFO_SQLITE, NULL);
#elif SQ_CONFIG_HAVE_MYSQL
	db = sqdb_new(SQDB_INFO_MYSQL, NULL);
#else
	#error No supported database
#endif

	if (sqdb_open(db, "test-migration") != SQCODE_OK) {
		fprintf(stderr, "Can't open database - %s\n", "test-migration");
		return EXIT_SUCCESS;
	}

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	test_sqdb_migrate_sqlite_sync(db);
#elif SQ_CONFIG_HAVE_MYSQL
	test_sqdb_migrate(db);
#endif

	sqdb_close(db);

	sqdb_free(db);
	return EXIT_SUCCESS;
}
