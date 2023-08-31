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

#include <stdio.h>

#include <sqxclib.h>
#include <SqSchema-macro.h>

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

// ----------------------------------------------------------------------------
// C structure

typedef struct User     User;
typedef struct City     City;
typedef struct Company  Company;

struct User {
	int    id;
	char  *name;
	char  *email;
	char  *comment;        // SQL type: TEXT
	int    city_id;
	int    company_id;

	SqIntArray     posts;

	time_t         created_at;
	time_t         updated_at;

	unsigned int   test_add;
	double         test_add_float;
};

struct City {
	int    id;
	char  *name;
	char  *chars;  // SQL Type: CHAR

	time_t         created_at;
	time_t         updated_at;
};

struct Company {
	int    id;
	char  *name;
	int    city_id;

	time_t         created_at;
	time_t         updated_at;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to define table and column

// --- UserColumns is sorted by programer... :)
static const SqColumn  *UserColumns[] = {
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE CASCADE ON UPDATE CASCADE
	&(SqColumn) {SQ_TYPE_INT, "city_id",     offsetof(User, city_id),    SQB_HIDDEN,
	             .foreign = &(SqForeign) {"cities", "id",  "CASCADE",  "CASCADE"} },

	// "comment"  TEXT
	&(SqColumn) {SQ_TYPE_STR, "comment",     offsetof(User, comment),  0,
	             .sql_type = SQ_SQL_TYPE_TEXT},

	// "company_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE CASCADE ON UPDATE CASCADE
	&(SqColumn) {SQ_TYPE_INT, "company_id",  offsetof(User, company_id), SQB_HIDDEN,
	             .foreign = &(SqForeign) {"companies", "id",  "CASCADE",  "CASCADE"} },

	// "company_test_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE NO ACTION ON UPDATE NO ACTION
	&(SqColumn) {SQ_TYPE_INT, "company_test_id", offsetof(User, company_id), SQB_HIDDEN,
	             .foreign = &(SqForeign) {"companies", "id",  "NO ACTION",  "NO ACTION"} },

	// "created_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
	&(SqColumn) {SQ_TYPE_TIME, "created_at", offsetof(User, created_at), SQB_CURRENT},

	// "email"  VARCHAR
	&(SqColumn) {SQ_TYPE_STR,  "email",      offsetof(User, email), SQB_HIDDEN_NULL},

	// CONSTRAINT FOREIGN KEY
	&(SqColumn) {SQ_TYPE_CONSTRAINT,  "fk_cities_id",
	             .foreign = &(SqForeign) {"cities", "id", "no action", "cascade"},
	             .composite = (char *[]) {"city_id", NULL} },

	// PRIMARY KEY
	&(SqColumn) {SQ_TYPE_INT, "id",          offsetof(User, id), SQB_PRIMARY | SQB_HIDDEN},

	&(SqColumn) {SQ_TYPE_STR, "name",        offsetof(User, name),     0},
	&(SqColumn) {SQ_TYPE_INT_ARRAY, "posts", offsetof(User, posts),    0},

	// "updated_at" TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	&(SqColumn) {SQ_TYPE_TIME, "updated_at", offsetof(User, updated_at),  SQB_CURRENT | SQB_CURRENT_ON_UPDATE},
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

	// ADD COLUMN "test_add_float"
	&(SqColumn) {SQ_TYPE_DOUBLE, "test_add_float", offsetof(User, test_add_float), SQB_NULLABLE,
	             .size = 8,  .digits = 2},

	// ALTER COLUMN "city_id"   (.bit_field = SQB_CHANGED)
//	&(SqColumn) {SQ_TYPE_INT,  "city_id",  offsetof(User, city_id),  SQB_CHANGED},

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

	// type mapping: SQ_TYPE_STR map to SQL data type - TEXT
	column = sq_table_add_mapping(table, "comment", offsetof(User, comment),
	                              SQ_TYPE_STR, SQ_SQL_TYPE_TEXT);

	column = sq_table_add_string(table, "email", offsetof(User, email), -1);

	column = sq_table_add_foreign(table, "fk_cities_id", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "no action");
	sq_column_on_update(column, "cascade");

	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_custom(table, "posts", offsetof(User, posts), SQ_TYPE_INT_ARRAY, -1);

	column = sq_table_add_timestamp(table, "created_at", offsetof(User, created_at));
	sq_column_use_current(column);
	column = sq_table_add_timestamp(table, "updated_at", offsetof(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);

	return table;
}

SqTable *change_user_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	table = sq_schema_alter(schema, "users", NULL);
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	sq_column_nullable(column);
//	column->bit_field |= SQB_NULLABLE;
	column = sq_table_add_double(table, "test_add_float", offsetof(User, test_add_float), 8, 2);
	sq_column_nullable(column);
//	column->bit_field |= SQB_NULLABLE;
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
		SQT_INTEGER_AS(User, company_id);  SQC_REFERENCE("companies", "id");  SQC_ON_DELETE("cascade");
		SQT_CUSTOM_AS(User, posts, SQ_TYPE_INT_ARRAY, -1);
		SQT_TIMESTAMP_AS(User, created_at);  SQC_USE_CURRENT();
		SQT_TIMESTAMP_AS(User, updated_at);  SQC_USE_CURRENT();  SQC_USE_CURRENT_ON_UPDATE();
	});

#if 0
	SQ_SCHEMA_CREATE(schema, "users", User, {
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();  SQC_HIDDEN();
		SQT_STRING("name", User, name, -1);
		SQT_STRING("email", User, email, -1);
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("set null");
		SQT_INTEGER("company_id", User, company_id);  SQC_REFERENCE("companies", "id");  SQC_ON_DELETE("cascade");
		SQT_CUSTOM("posts", User, posts, SQ_TYPE_INT_ARRAY, -1);
		SQT_TIMESTAMP("created_at", User, created_at);  SQC_USE_CURRENT();
		SQT_TIMESTAMP("updated_at", User, updated_at);  SQC_USE_CURRENT();  SQC_USE_CURRENT_ON_UPDATE();
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

	table = sq_schema_create(schema, "companies", Company);
	column = sq_table_add_integer(table, "id", offsetof(Company, id));
	sq_column_primary(column);
	column = sq_table_add_string(table, "name", offsetof(Company, name), 0);
	column = sq_table_add_integer(table, "city_id", offsetof(Company, city_id));
	sq_column_foreign(column, "cities", "id");
//	sq_column_on_delete(column, "set null");

	column = sq_table_add_timestamp(table, "created_at", offsetof(Company, created_at));
	sq_column_use_current(column);
	column = sq_table_add_timestamp(table, "updated_at", offsetof(Company, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);
}

void change_company_table_by_c(SqSchema *schema)
{
	SqTable  *table;

	// ALTER TABLE
	table = sq_schema_alter(schema, "companies", NULL);
	// DROP COLUMN
	sq_table_drop_column(table, "created_at");
	sq_table_drop_column(table, "updated_at");
}

void create_city_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	table = sq_schema_create(schema, "cities", City);
	column = sq_table_add_integer(table, "id", offsetof(City, id));
	sq_column_primary(column);
	column = sq_table_add_string(table, "name", offsetof(City, name), 0);
}

void change_city_table_by_c(SqSchema *schema)
{
	SqTable  *table;
	SqColumn *column;

	// ALTER TABLE
	table = sq_schema_alter(schema, "cities", NULL);
	// ALTER COLUMN name
	column = sq_table_add_string(table, "name", offsetof(City, name), 256);
	sq_column_change(column);
	// ADD COLUMN chars
	column = sq_table_add_char(table, "chars", offsetof(City, chars), 20);
	// ADD COLUMN updated_at
	column = sq_table_add_timestamp(table, "created_at", offsetof(City, created_at));
	sq_column_use_current(column);
	// ADD COLUMN updated_at
	column = sq_table_add_timestamp(table, "updated_at", offsetof(City, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);
}

// ----------------------------------------------------------------------------

void test_sqdb_migrate(Sqdb *db)
{
	SqSchema   *schema;
	SqSchema   *schema_v1;
	SqSchema   *schema_v2;
	SqSchema   *schema_v3;
	SqSchema   *schema_v4;
	SqSchema   *schema_v5;
	SqSchema   *schema_v6;

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

	schema_v5 = sq_schema_new("ver5");
	schema_v5->version = 5;
	change_company_table_by_c(schema_v5);

	schema_v6 = sq_schema_new("ver6");
	schema_v6->version = 6;
	change_city_table_by_c(schema_v6);
	// other testing in 'schema_v6'
	sq_schema_rename(schema_v6, "cities", "cities2");
//	sq_schema_drop(schema_v6, "users");

	sqdb_migrate(db, schema, schema_v1);
	sqdb_migrate(db, schema, schema_v2);
	sqdb_migrate(db, schema, schema_v3);
	sqdb_migrate(db, schema, schema_v4);
	sqdb_migrate(db, schema, schema_v5);
	sqdb_migrate(db, schema, schema_v6);
	sqdb_migrate(db, schema, NULL);

	sq_schema_free(schema);
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
	sq_schema_free(schema_v3);
	sq_schema_free(schema_v4);
	sq_schema_free(schema_v5);
	sq_schema_free(schema_v6);
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

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
SqdbConfigSqlite db_config_sqlite = {
//	.folder = "/tmp",
	.folder = ".",
	.extension = "db",
};

const SqdbInfo *db_info   = SQDB_INFO_SQLITE;
SqdbConfig     *db_config = (SqdbConfig*) &db_config_sqlite;

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
SqdbConfigMysql  db_config_mysql = {
	.host     = "localhost",
	.port     = 3306,
	.user     = "root",
	.password = "",
};

const SqdbInfo *db_info   = SQDB_INFO_MYSQL;
SqdbConfig     *db_config = (SqdbConfig*) &db_config_mysql;

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
SqdbConfigPostgre  db_config_postgre = {
	.host     = "localhost",
	.port     = 5432,
	.user     = "postgres",
	.password = "",
};

const SqdbInfo *db_info   = SQDB_INFO_POSTGRE;
SqdbConfig     *db_config = (SqdbConfig*) &db_config_postgre;

#else

const SqdbInfo *db_info   = NULL;
SqdbConfig     *db_config = NULL;

#endif


int  main(void)
{
	Sqdb   *db;

	if (db_info == NULL) {
		fprintf(stderr, "No supported database.\n");
		return EXIT_SUCCESS;
	}

	db = sqdb_new(db_info, db_config);

	if (sqdb_open(db, "test-migration") != SQCODE_OK) {
		fprintf(stderr, "Can't open database - %s\n", "test-migration");
		return EXIT_SUCCESS;
	}

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	test_sqdb_migrate_sqlite_sync(db);
#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
	test_sqdb_migrate(db);
#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
	test_sqdb_migrate(db);
#endif

	sqdb_close(db);

	sqdb_free(db);
	return EXIT_SUCCESS;
}
