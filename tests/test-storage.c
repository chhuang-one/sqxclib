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
		SQT_INTEGER("id", Company, id); SQC_PRIMARY(); SQC_INCREMENT();
		SQT_STRING("name", Company, name, -1);
		SQT_INTEGER("age", Company, age);
		SQT_STRING("address", Company, address, 50);
		SQT_DOUBLE("salary", Company, salary, 0, 0);
	});

	schema->version = 1;
}

void test_storage_insert_update(SqStorage *storage)
{
	Company *company_temp;
	Company  company;
	int      id;

	company.id = 0;    // auto increment
	company.name = "Tome";
	company.salary = 10245;
	company.age = 25;
	company.address = "Texas";

	// insert to companies and get row id.
	id = sq_storage_insert(storage, "companies", NULL, &company);

	if (id == -1)
		fprintf(stderr, "test_storage_insert_update(): insert failed.\n");
	assert(id != -1);

	company_temp = sq_storage_get(storage, "companies", NULL, id);
	assert(company_temp != NULL);
	assert(company_temp->age == company.age);
	fprintf(stderr, "test_storage_insert_update(): insert ok.\n");
	company_free(company_temp);

	// update
	company.id = id;
	company.name = "Alex Kuo";
	company.salary = 15341;
	company.age = 28;
	sq_storage_update(storage, "companies", NULL, &company);

	company_temp = sq_storage_get(storage, "companies", NULL, id);
	assert(company_temp != NULL);
	assert(company_temp->age == company.age);
	fprintf(stderr, "test_storage_insert_update(): update ok.\n");
	company_free(company_temp);
}

void test_storage(const SqdbInfo *dbinfo, SqdbConfig *config)
{
	Sqdb      *db;
	SqStorage *storage;
	SqSchema  *schema;

	db = sqdb_new(dbinfo, config);
	storage = sq_storage_new(db);

	sq_storage_open(storage, "test-storage");

	schema = sq_schema_new(NULL);
	create_company_table(schema);
	sq_storage_migrate(storage, schema);
	sq_storage_migrate(storage, NULL);
	sq_schema_free(schema);

	test_storage_insert_update(storage);

	sq_storage_close(storage);
}



#ifdef SQ_CONFIG_HAVE_SQLITE

SqStorage *create_storage_by_sqlite(sqlite3 *sqlitedb)
{
	Sqdb       *db;
	SqStorage  *storage;

	db = sqdb_new(SQDB_INFO_SQLITE, NULL);
	((SqdbSqlite*)db)->self = sqlitedb;

	storage = sq_storage_new(db);
	create_company_table(storage->schema);
	return storage;
}

static int callback(void *user_data, int argc, char **argv, char **columnName)
{
	int i;

	for (i=0; i<argc; i++) {
		printf("%s = %s\n", columnName[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}

int  test_sqlite_c(int argc, char *argv[])
{
	sqlite3 *db;
	char  *errorMsg;
	char  *sql;
	int    rc;

	// sqxc
	SqStorage  *storage;
	SqPtrArray *array;
	Company    *company;

	/* Open database */
	rc = sqlite3_open("test.db", &db);

	if( rc != SQLITE_OK ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "CREATE TABLE companies("
	      "ID INT PRIMARY KEY     NOT NULL,"
	      "NAME           TEXT    NOT NULL,"
	      "AGE            INT     NOT NULL,"
	      "ADDRESS        CHAR(50),"
	      "SALARY         REAL );";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &errorMsg);
	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errorMsg);
		sqlite3_free(errorMsg);
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}

	/* Create SQL statement */
	sql = "INSERT INTO companies (ID,NAME,AGE,ADDRESS,SALARY) "
	      "VALUES (1, 'Paul', 32, 'California', 20000.00 ); "
	      "INSERT INTO companies (ID,NAME,AGE,ADDRESS,SALARY) "
	      "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "
	      "INSERT INTO companies (ID,NAME,AGE,ADDRESS,SALARY)"
	      "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"
	      "INSERT INTO companies (ID,NAME,AGE,ADDRESS,SALARY)"
	      "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &errorMsg);
	if( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s", errorMsg);
		sqlite3_free(errorMsg);
	}
	else {
		fprintf(stdout, "Records created successfully\n");
	}

	sql = "INSERT INTO companies (ID,NAME,AGE,ADDRESS,SALARY) "
	      "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); ";
	rc = sqlite3_exec(db, sql, callback, 0, &errorMsg);

	// sqxc
	puts("\n");
	storage = create_storage_by_sqlite(db);
	array = sq_storage_get_all(storage, "companies", NULL, NULL, NULL);
	company_array_print(array);
	sq_ptr_array_foreach(array, element) {
		company_free((Company*)element);
	}
	sq_ptr_array_free(array);

	company = sq_storage_get(storage, "companies", NULL, 2);
	company_object_print(company);
	// update after get
	free(company->name);
	company->name = strdup("Allen's");
	company->age = 59;
#if 1
	company->id = 5;
	sq_storage_insert(storage, "companies", NULL, company);
#elif 1
	sq_storage_remove(storage, "companies", NULL, 5);
#else
	sq_storage_update(storage, "companies", NULL, company);
#endif
	company_free(company);

//  "DELETE from companies where ID='2'; "
	sql = "SELECT * FROM companies";
	rc = sqlite3_exec(db, sql, callback, 0, &errorMsg);

//	rc = sqlite3_db_cacheflush(db);
	sqlite3_close(db);

	return EXIT_SUCCESS;
}

#endif  // SQ_CONFIG_HAVE_SQLITE

int  main(int argc, char *argv[])
{
#ifdef SQ_CONFIG_HAVE_SQLITE
//	test_sqlite_c(argc, argv);
#endif

#ifdef SQ_CONFIG_HAVE_SQLITE
	test_storage(SQDB_INFO_SQLITE, NULL);
#endif

#ifdef SQ_CONFIG_HAVE_MYSQL
//	test_storage(SQDB_INFO_MYSQL, NULL);
#endif

	return EXIT_SUCCESS;
}
