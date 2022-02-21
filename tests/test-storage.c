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

#include <sqxclib.h>
#include <SqSchema-macro.h>

#define USE_SQLITE_IF_POSSIBLE    1

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
	Company *company_temp;
	Company  company;
	int      id;
	int      n_changes;

	company.id = 0;    // for auto increment
	company.name = "Tome";
	company.salary = 10245;
	company.age = 25;
	company.address = "Texas";

	// insert to companies and get inserted row id.
	id = sq_storage_insert(storage, "companies", NULL, &company);

	fprintf(stderr, "insert(): inserted id = %d.\n", id);
	assert(id != 0);

	company_temp = sq_storage_get(storage, "companies", NULL, id);
	assert(company_temp != NULL);
	assert(company_temp->age == company.age);
	company_free(company_temp);
	fprintf(stderr, "insert(): ok.\n");

	// update
	company.id = id;
	company.name = "Alex";
	company.salary = 15341;
	company.age = 28;
	n_changes = sq_storage_update(storage, "companies", NULL, &company);

	fprintf(stderr, "update(): number of changes = %d.\n", n_changes);
	assert(n_changes == 1);

	company_temp = sq_storage_get(storage, "companies", NULL, id);
	assert(company_temp != NULL);
	assert(company_temp->age == company.age);
	company_free(company_temp);
	fprintf(stderr, "update(): ok.\n");

	// remove
	sq_storage_remove(storage, "companies", NULL, id);

	company_temp = sq_storage_get(storage, "companies", NULL, id);
	assert(company_temp == NULL);
	fprintf(stderr, "remove(): ok.\n");
}

void test_storage(const SqdbInfo *dbinfo, SqdbConfig *config)
{
	Sqdb      *db;
	SqStorage *storage;
	SqSchema  *schema;

	db = sqdb_new(dbinfo, config);
	storage = sq_storage_new(db);

	sq_storage_open(storage, "test-storage");

	// migrate
	schema = sq_schema_new(NULL);
	create_company_table(schema);
	sq_storage_migrate(storage, schema);
	sq_storage_migrate(storage, NULL);
	sq_schema_free(schema);

	// test get, insert, update, and remove
	test_storage_crud(storage);

	sq_storage_close(storage);
}


int  main(int argc, char *argv[])
{
#if defined(SQ_CONFIG_HAVE_SQLITE) && USE_SQLITE_IF_POSSIBLE == 1

	SqdbConfigSqlite  config_sqlite = {
//		.folder = "/tmp",
		.folder = ".",
		.extension = "db",
	};

	fprintf(stderr, "\n\n" "test SqStorage with SQLite..." "\n\n");
	test_storage(SQDB_INFO_SQLITE, (SqdbConfig*) &config_sqlite);

#elif defined(SQ_CONFIG_HAVE_MYSQL)

	SqdbConfigMysql  config_mysql = {
		.host = "localhost",
		.port = 3306,
		.user = "root",
		.password = "",
	};

	fprintf(stderr, "\n\n" "test SqStorage with MySQL..." "\n\n");
	test_storage(SQDB_INFO_MYSQL, (SqdbConfig*) &config_mysql);

#else
	printf("No supported database");
#endif

	return EXIT_SUCCESS;
}
