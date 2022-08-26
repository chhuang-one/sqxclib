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
#include <assert.h>
#include <inttypes.h>   // PRId64, PRIu64

#include <sqxclib.h>
#include <SqSchema-macro.h>

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

// ----------------------------------------------------------------------------

typedef struct Company    Company;

struct Company
{
	int    id;
	char  *name;
	int    age;
	char  *address;
	double salary;
};

void company_free(Company *company)
{
	free(company->name);
	free(company->address);
	free(company);
}

void  company_object_print(Company *company)
{
	printf("company.id = %d\n"
	       "company.name = %s\n"
	       "company.age = %d\n"
	       "company.address = %s\n"
	       "company.salary = %lf\n\n",
	       company->id, company->name, company->age,
	       company->address, company->salary);
}

void  company_array_print(SqPtrArray *array)
{
	Company   *company;
	int        index;

	for (index = 0;  index < array->length;  index++) {
		company = (Company*)(array->data[index]);
		company_object_print(company);
	}
}

void create_company_table(SqSchema *schema)
{
	SQ_SCHEMA_CREATE(schema, "companies", Company, {
		SQT_INTEGER("id", Company, id); SQC_PRIMARY(); SQC_AUTOINCREMENT();
		SQT_STRING("name", Company, name, -1);
		SQT_INTEGER("age", Company, age);
		SQT_STRING("address", Company, address, 50);
		SQT_DOUBLE("salary", Company, salary, 0, 0);
	});

	schema->version = 1;
}

void test_storage_crud(SqStorage *storage)
{
	Company *company_ptr;
	Company  company;
	int64_t  id;
	int      n_changes;

	company.id = 0;    // for auto increment
	company.name = "Tom";
	company.salary = 10245;
	company.age = 25;
	company.address = "Texas";

	// insert to companies and get inserted row id.
	id = sq_storage_insert(storage, "companies", NULL, &company);

	fprintf(stderr, "insert(): inserted id = %"PRId64"\n", id);
	assert(id != 0);

	company_ptr = sq_storage_get(storage, "companies", NULL, id);
	assert(company_ptr != NULL);
	assert(company_ptr->age == company.age);
	company_free(company_ptr);
	fprintf(stderr, "insert(): ok.\n");

	// update
	company.id = (int)id;
	company.name = "Alex";
	company.salary = 15341;
	company.age = 28;
	n_changes = sq_storage_update(storage, "companies", NULL, &company);

	fprintf(stderr, "update(): number of changes = %d.\n", n_changes);
	assert(n_changes == 1);

	company_ptr = sq_storage_get(storage, "companies", NULL, id);
	assert(company_ptr != NULL);
	assert(company_ptr->age == company.age);
	company_free(company_ptr);
	fprintf(stderr, "update(): ok.\n");

	// remove
	sq_storage_remove(storage, "companies", NULL, id);

	company_ptr = sq_storage_get(storage, "companies", NULL, id);
	assert(company_ptr == NULL);
	fprintf(stderr, "remove(): ok.\n");
}

void test_storage_xxx_all(SqStorage *storage)
{
	SqPtrArray *array;
	Company *company_ptr;
	Company  company;
	int64_t  id[2];
	int64_t  n_changes;

	company.id = 0;    // for auto increment
	company.name = "McD";
	company.salary = 10245;
	company.age = 25;
	company.address = "Texas";

	// insert to companies and get inserted row id.
	id[0] = sq_storage_insert(storage, "companies", NULL, &company);
	// insert to companies and get inserted row id.
	id[1] = sq_storage_insert(storage, "companies", NULL, &company);
	fprintf(stderr, "inserted id = %"PRId64", %"PRId64"\n", id[0], id[1]);

	// update_all
	// update 2 columns only - "name" and "age".
	company.name = "KFC";
	company.salary = 22345;
	company.age = 55;
	n_changes = sq_storage_update_all(storage, "companies", NULL, &company,
	                                  NULL,
	                                  "name", "age",
	                                  NULL);
	assert(n_changes == 2);

	// get_all
	array = sq_storage_get_all(storage, "companies", NULL, NULL, NULL);
	assert(array != NULL);
	assert(array->length == 2);
	fprintf(stderr, "get_all(): ok.\n");

	company_ptr = array->data[0];
	assert(company_ptr->age    == company.age);
	assert(company_ptr->salary != company.salary);
	assert(strcmp(company_ptr->name, company.name) == 0);
	company_object_print(company_ptr);
	company_free(company_ptr);

	company_ptr = array->data[1];
	assert(company_ptr->age    == company.age);
	assert(company_ptr->salary != company.salary);
	assert(strcmp(company_ptr->name, company.name) == 0);
	company_object_print(company_ptr);
	company_free(company_ptr);
	// free array
	sq_ptr_array_free(array);

	// update 2 columns only - "name" and "age".
	fprintf(stderr, "update_all(): ok.\n");

#if SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD
	// update_field by offsetof(User, name) and offsetof(User, email)
	company.name = "Panda";
	company.salary = 18765;
	company.age = 35;
	n_changes = sq_storage_update_field(storage, "companies", NULL, &company,
	                                    "WHERE id > 0",
	                                    offsetof(Company, name),
	                                    offsetof(Company, age),
	                                    -1);
	assert(n_changes == 2);
	company_ptr = sq_storage_get(storage, "companies", NULL, id[0]);
	assert(company_ptr != NULL);
	assert(company_ptr->age    == company.age);
	assert(company_ptr->salary != company.salary);
	assert(strcmp(company_ptr->name, company.name) == 0);
	company_free(company_ptr);

	company_ptr = sq_storage_get(storage, "companies", NULL, id[1]);
	assert(company_ptr != NULL);
	assert(company_ptr->age    == company.age);
	assert(company_ptr->salary != company.salary);
	assert(strcmp(company_ptr->name, company.name) == 0);
	company_free(company_ptr);
	fprintf(stderr, "update_field(): ok.\n");
#endif // SQ_CONFIG_HAS_STORAGE_UPDATE_FIELD

	// remove_all
	sq_storage_remove_all(storage, "companies", NULL);
	company_ptr = sq_storage_get(storage, "companies", NULL, id[0]);
	assert(company_ptr == NULL);
	company_ptr = sq_storage_get(storage, "companies", NULL, id[1]);
	assert(company_ptr == NULL);
	fprintf(stderr, "remove_all(): ok.\n");
}

void test_storage(const SqdbInfo *dbinfo, SqdbConfig *config)
{
	Sqdb      *db;
	SqStorage *storage;
	SqSchema  *schema;
	int        code;

	db = sqdb_new(dbinfo, config);
	storage = sq_storage_new(db);

	code = sq_storage_open(storage, "test-storage");
	if (code != SQCODE_OK)
		return;

	// migrate schema version 1
	schema = sq_schema_new(NULL);
	create_company_table(schema);
	sq_storage_migrate(storage, schema);
	sq_storage_migrate(storage, NULL);
	sq_schema_free(schema);

	// test get(), insert(), update(), and remove()
	test_storage_crud(storage);
	// test update_all(), get_all(), and remove_all()
	test_storage_xxx_all(storage);

	sq_storage_close(storage);
}

// ----------------------------------------------------------------------------

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
SqdbConfigSqlite db_config_sqlite = {
//	.folder = "/tmp",
	.folder = ".",
	.extension = "db",
};

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
SqdbConfigMysql  db_config_mysql = {
	.host     = "localhost",
	.port     = 3306,
	.user     = "root",
	.password = "",
};

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
SqdbConfigPostgre  db_config_postgre = {
	.host     = "localhost",
	.port     = 5432,
	.user     = "postgre",
	.password = "",
};

#endif

int  main(int argc, char *argv[])
{
#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	fprintf(stderr, "\n\n" "test SqStorage with SQLite..." "\n\n");
	test_storage(SQDB_INFO_SQLITE, (SqdbConfig*) &db_config_sqlite);

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
	fprintf(stderr, "\n\n" "test SqStorage with MySQL..." "\n\n");
	test_storage(SQDB_INFO_MYSQL, (SqdbConfig*) &db_config_mysql);

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
	fprintf(stderr, "\n\n" "test SqStorage with PostgreSQL..." "\n\n");
	test_storage(SQDB_INFO_POSTGRE, (SqdbConfig*) &db_config_postgre);

#else
	fprintf(stderr, "No supported database");

#endif

	return EXIT_SUCCESS;
}
