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

#include <Sqdb.h>
#include <SqSchema-macro.h>

// ----------------------------------------------------------------------------
// declare C structure

typedef struct User     User;
typedef struct City     City;
typedef struct Company  Company;

struct User {
	int    id;
	char*  name;
	char*  email;
	int    city_id;
	int    company_id;

	SqIntptrArray   posts;

	unsigned int test_add;
};

struct City {
	int    id;
	char*  name;
};

struct Company {
	int    id;
	char*  name;
	int    city_id;
};

// ----------------------------------------------------------------------------
// use C99 designated initializer to declare table/column

// --- UserColumns is sorted by programer... :)
static const SqColumn  *UserColumns[] = {
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE set null ON UPDATE cascade
	&(SqColumn) {SQ_TYPE_INT,    "city_id", offsetof(User, city_id),    SQB_HIDDEN,
	             .foreign = &(SqForeign) {"cities", "id",  "set null",  "cascade"} },
	// "company_id"  INT  FOREIGN KEY REFERENCES "cities"("id") ON DELETE cascade ON UPDATE set null
	&(SqColumn) {SQ_TYPE_INT, "company_id", offsetof(User, company_id), SQB_HIDDEN,
	             .foreign = &(SqForeign) {"companies", "id",  "cascade",  "set null"} },

	// "email"  VARCHAR
	&(SqColumn) {SQ_TYPE_STRING, "email",   offsetof(User, email), SQB_HIDDEN_NULL},

	// CONSTRAINT FOREIGN KEY
	&(SqColumn) {.name = "fk_cities_id",
	             .foreign = &(SqForeign) {"cities", "id", "no action", "cascade"},
	             .constraint = (char *[]) {"city_id", NULL} },
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
	.bit_field  = SQB_TYPE_SORTED                           // UserColumns is sorted
};
 */

/* ----------------------------------------------------------------------------
   use C99 designated initializer to declare table/column changed (migration)
*/
static const SqColumn  *UserColumnsChange[] = {
	// ADD COLUMN "test_add"
	&(SqColumn) {SQ_TYPE_UINT, "test_add", offsetof(User, test_add), SQB_NULLABLE},

	// ALTER COLUMN "city_id"   (.bit_field = SQB_CHANGE)
//	&(SqColumn) {SQ_TYPE_INT,  "city_id", offsetof(User, city_id), SQB_CHANGE},

	// DROP CONSTRAINT FOREIGN KEY "fk_cities_id"
	&(SqColumn) {.old_name = "fk_cities_id",  .name = NULL,
	             .constraint = (char**)"",  .foreign = (SqForeign*)""},
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

SqTable* create_user_table_by_type(SqSchema* schema)
{
	return sq_schema_create_by_type(schema, "users", &UserType);
//	return sq_schema_create_full(schema, "users", &UserType, NULL, 0);
}

SqTable* change_user_table_by_c_type(SqSchema* schema)
{
	return sq_schema_alter(schema, "users", &UserTypeChange);
}

// ----------------------------------------------------------------------------
// use C function to declare table/column

SqTable* create_user_table_by_c(SqSchema* schema)
{
	SqTable*  table;
	SqColumn* column;

//	table = sq_schema_create(schema, "users", User);
	table = sq_schema_create_full(schema, "users", NULL, SQ_GET_TYPE_NAME(User), sizeof(User));

	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	column = sq_table_add_int(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");
//	column = sq_table_add_primary(table, "pk_name_email", "name", "email", NULL);
	sq_column_set_constraint(column, "name", "email", NULL);
	column = sq_table_add_custom(table, "posts", offsetof(User, posts), SQ_TYPE_INTPTR_ARRAY);

	return table;
}

SqTable* change_user_table_by_c(SqSchema* schema)
{
	SqTable*  table;
	SqColumn* column;

	table = sq_schema_alter(schema, "users", NULL);
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	column->bit_field |= SQB_NULLABLE;
	sq_table_drop_foreign(table, "cities_id_foreign");
	sq_table_drop_column(table, "name");
	sq_table_rename_column(table, "email", "email2");
	return table;
}

// ----------------------------------------------------------------------------
// use C macro to declare dynamic table/column

void  create_user_table_by_macro(SqSchema* schema)
{
	SQ_SCHEMA_CREATE(schema, "users", User, {
		SQT_INTEGER_AS(User, id);  SQC_PRIMARY();  SQC_HIDDEN();
		SQT_STRING_AS(User, name, -1);
		SQT_STRING_AS(User, email, -1);
		SQT_INTEGER_AS(User, city_id);  SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("set null");
		SQT_CUSTOM_AS(User, posts, SQ_TYPE_INTPTR_ARRAY);
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

void  change_user_table_by_macro(SqSchema* schema)
{
	SQ_SCHEMA_ALTER(schema, "users", {
		SQT_UINT_AS(User, test_add);
		SQT_INTEGER_AS(User, city_id);  SQC_CHANGE();
		SQT_DROP("name");
		SQT_RENAME("email", "email2");
	});
}

// ----------------------------------------------------------------------------

void create_company_table_by_c(SqSchema* schema)
{
	SqTable*  table;
	SqColumn* column;

	// use C function to change schema
	table = sq_schema_create(schema, "companies", Company);
	column = sq_table_add_integer(table, "id", offsetof(Company, id));
	column->bit_field |= SQB_PRIMARY;
	column = sq_table_add_string(table, "name", offsetof(Company, name), 0);
	column = sq_table_add_integer(table, "city_id", offsetof(Company, city_id));
	sq_column_foreign(column, "cities", "id");
//	sq_column_on_delete(column, "set null");
}

void create_city_table_and_rename_by_c(SqSchema* schema)
{
	SqTable*  table;
	SqColumn* column;

	// use C function to change schema
	table = sq_schema_create(schema, "cities", City);
	column = sq_table_add_integer(table, "id", offsetof(City, id));
	column->bit_field |= SQB_PRIMARY;
	column = sq_table_add_string(table, "name", offsetof(City, name), 0);

	sq_schema_rename(schema, "cities", "cities2");
//	sq_schema_drop(schema, "cities2");
}

// ----------------------------------------------------------------------------
typedef struct Migration    Migration;

struct Migration
{
	int   id;
	char* migration;
	int   batch;
};

// ----------------------------------------------------------------------------

void create_migration_table(SqSchema* schema)
{
	SQ_SCHEMA_CREATE(schema, "migrations", NULL, {
		SQT_INTEGER_AS(Migration, id); SQC_PRIMARY();
		SQT_STRING_AS(Migration, migration, -1);
		SQT_INTEGER_AS(Migration, batch);
	});
}

void test_db(SqSchema* schema, SqPtrArray* entries)
{
	Sqdb     db;
	SqdbInfo dbinfo;
	SqBuffer buffer;

	db.info = &dbinfo;
	sqdb_info_init_sqlite(db.info);
//	sqdb_info_init_mysql(db.info);

	buffer = (SqBuffer){NULL, 0, 0};

	sqdb_schema_to_sql(&db, &buffer, schema, entries);

	sq_buffer_write_c(&buffer, '\0');
	puts(buffer.buf);

	sq_buffer_final(&buffer);
}

// ----------------------------------------------------------------------------

int  main(void)
{
	SqSchema*   schema;
	SqSchema*   schema_v2;
	SqSchema*   schema_v3;
	SqSchema*   schema_v4;
	SqPtrArray  entries;

	schema  = sq_schema_new("default");
	create_user_table_by_type(schema);
//	create_user_table_by_macro(schema);
//	create_user_table_by_c(schema);

	schema_v2 = sq_schema_new("ver2");
	change_user_table_by_c_type(schema_v2);

	schema_v3 = sq_schema_new("ver3");
	create_company_table_by_c(schema_v3);

	schema_v4 = sq_schema_new("ver4");
	create_city_table_and_rename_by_c(schema_v4);

	sq_schema_accumulate(schema, schema_v2);
	sq_schema_accumulate(schema, schema_v3);
	sq_schema_accumulate(schema, schema_v4);

	// trace renamed (or dropped) table/column that was referenced by others
	sq_schema_trace_foreign(schema);
	// reset changed records before calling sq_schema_arrange()
	sq_schema_reset_changes(schema, 1, 0);

	sq_ptr_array_init(&entries, 8, NULL);
	sq_schema_arrange(schema, &entries);

	test_db(schema, &entries);

	return 0;
}
