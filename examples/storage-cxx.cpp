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


#include <iostream>
#include <stdio.h>

#include <SqConfig.h>
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
	int    company_id;
};

struct Company
{
	int    id;
	char*  name;
	int    age;
	char*  address;
	double salary;

	Sq::IntptrArray  ints;

	// --------------------------------
	// internal member
	bool   dynamicString;

	Company(bool dynamicString = true) {
		this->dynamicString = dynamicString;
		ints.init(4);
	}
	~Company() {
		if (dynamicString) {
			free(name);
			free(address);
		}
		ints.final();
	}

	void print() {
		std::cout << std::endl
		          << "company.id = "      << this->id      << std::endl
		          << "company.name = "    << this->name    << std::endl
		          << "company.age = "     << this->age     << std::endl
		          << "company.address = " << this->address << std::endl
		          << "company.salary = "  << this->salary  << std::endl;
		std::cout << "company.ints[] = ";
		for (int index = 0;  index < this->ints.length;  index++) {
			if (index > 0)
				std::cout << ",";
			std::cout << (int)this->ints[index];
		}
		std::cout << std::endl;
	}
};

void  company_array_print(SqPtrArray* array)
{
	Company*   company;
	int        index;

	for (index = 0;  index < array->length;  index++) {
		company = (Company*)(array->data[index]);
		company->print();
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
#ifdef SQ_CONFIG_JSON_SUPPORT
	table->custom("ints", &Company::ints, SQ_TYPE_INTPTR_ARRAY);
#endif

	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("company_id", &User::company_id);
	table->addForeign("users_companies_id_foreign", "company_id")->reference("companies", "id");
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
#ifdef SQ_CONFIG_JSON_SUPPORT
	table->custom("ints", &Company::ints, SQ_TYPE_INTPTR_ARRAY);
#endif

	table = schemaVer2->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("company_id", &User::company_id);
	table->addForeign("users_companies_id_foreign", "company_id")->reference("companies", "id");
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
	intptr_t      array1[4] = {1, 3, 5, 7};
	intptr_t      array2[4] = {2, 4, 6, 8};

	db = new Sq::DbSqlite(NULL);
	storage = new Sq::Storage(db);

	storage->open("sample");

//	storage_make_fixed_schema(storage);
	storage_make_migrated_schema(storage);

	company = new Company(false);

	company->id = 1;
	company->name = (char*)"Mr.T";
	company->age = 21;
	company->address = (char*)"Norway";
	company->salary = 1200.00;
	company->ints.append(array1, 4);
	storage->insert<Company>(company);
	company->ints.length = 0;

	company->id = 2;
	company->name = (char*)"Paul";
	company->age = 32;
	company->address = (char*)"Texas";
	company->salary = 3300.00;
	company->ints.append(array2, 4);
	storage->insert<Company>(company);
	company->ints.length = 0;

	delete company;

	company = storage->get<Company>(1);
	company->print();
	delete company;

	storage->close();
	delete storage;
	delete db;

	return 0;
}

