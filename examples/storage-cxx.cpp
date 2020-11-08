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

#include <SqError.h>
#include <SqPtrArray.h>
#include <SqTable.h>
#include <SqStorage.h>
#include <SqdbSqlite.h>

typedef struct User       User;
typedef struct Company    Company;

struct User {
	int    id;
	char*  name;
	int    companies_id;
};

struct Company
{
	int    id;
	char*  name;
	int    age;
	char*  address;
	double salary;
};

void company_free(Company* company)
{
	free(company->name);
	free(company->address);
	free(company);
}

void  company_object_print(Company* company)
{
	printf("company.id = %d\n"
	       "company.name = %s\n"
	       "company.age = %d\n"
	       "company.address = %s\n"
	       "company.salary = %lf\n\n",
	       company->id, company->name, company->age,
	       company->address, company->salary);
}

void  company_array_print(SqPtrArray* array)
{
	Company*   company;
	int        index;

	for (index = 0;  index < array->length;  index++) {
		company = (Company*)(array->data[index]);
		company_object_print(company);
	}
}

// ----------------------------------------------------------------------------

void  storage_make_fixed_schema(Sq::Storage* storage)
{
	Sq::Schema*   schema;
	Sq::Table*    table;

	// create table in storage->schema
	schema = storage->schema;

	table = schema->create<Company>("companies");
	table->integer("id", &Company::id)->primary();
	table->string("name", &Company::name);
	table->integer("age", &Company::age);
	table->string("address", &Company::address);
	table->double_("salary", &Company::salary);

	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("companies_id", &User::companies_id);
	table->addForeign("users_companies_id_foreign", "companies_id")->reference("companies", "id");
	table->addIndex("users_id_index", "index", NULL);

	// End of migration. create SQL tables based on storage->schema
	storage->migrate(NULL);
}

void  storage_make_migrated_schema(Sq::Storage* storage)
{
	Sq::Schema*   schemaVer1;
	Sq::Schema*   schemaVer2;
	Sq::Table*    table;

	// create table in schemaVer1 and schemaVer2
	schemaVer1 = new Sq::Schema("Ver1");
	schemaVer2 = new Sq::Schema("Ver2");
	schemaVer1->version = 1;
	schemaVer2->version = 2;

	table = schemaVer1->create<Company>("companies");
	table->integer("id", &Company::id)->primary();
	table->string("name", &Company::name);
	table->integer("age", &Company::age);
	table->string("address", &Company::address);
	table->double_("salary", &Company::salary);

	table = schemaVer2->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("companies_id", &User::companies_id);
	table->addForeign("users_companies_id_foreign", "companies_id")->reference("companies", "id");
	table->addIndex("users_id_index", "id", NULL);

	// migrate schemaVer1, schemaVer2 to storage->schema
	storage->migrate(schemaVer1);
	storage->migrate(schemaVer2);
	// End of migration. create SQL tables based on storage->schema
	storage->migrate(NULL);
}

int main (int argc, char* argv[])
{
//	Sq::DbConfigSqlite* dbconfig;
	Sq::DbSqlite* db;
	Sq::Storage*  storage;
	Company*      company;

	db = new Sq::DbSqlite(NULL);
	storage = new Sq::Storage(db);

	storage->open("sample");

//	storage_make_fixed_schema(storage);
	storage_make_migrated_schema(storage);

	company = new Company();

	company->id = 1;
	company->name = (char*)"Mr.T";
	company->age = 21;
	company->address = (char*)"Norway";
	company->salary = 1200.00;
	storage->insert<Company>(company);

	company->id = 2;
	company->name = (char*)"Paul";
	company->age = 32;
	company->address = (char*)"Texas";
	company->salary = 3300.00;
	storage->insert<Company>(company);

	delete company;
	company = storage->get<Company>(1);
	company_object_print(company);
	company_free(company);

	storage->close();
	delete storage;
	delete db;

	return 0;
}

