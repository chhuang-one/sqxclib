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


#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>

#include <SqConfig.h>
#include <SqError.h>
#include <SqPtrArray.h>
#include <SqType-stl-cpp.h>
#include <SqTable.h>
#include <SqStorage.h>
#include <SqdbSqlite.h>

// C++ std::vector<int>
#define SQ_TYPE_INT_VECTOR    &SqTypeIntVector
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);

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

	Sq::IntptrArray  ints;    // C array for intptr_t
	Sq::StringArray  strs;    // C array for char*
	std::vector<int> intsCpp; // C++ type, it use Sq::TypeStl<std::vector<int>>
	std::string      strCpp;  // C++ type

	// --------------------------------
	// member functions

	void print() {
		std::cout << std::endl
		          << "company.id = "      << this->id      << std::endl
		          << "company.name = "    << this->name    << std::endl
		          << "company.age = "     << this->age     << std::endl
		          << "company.address = " << this->address << std::endl
		          << "company.salary = "  << this->salary  << std::endl
		          << "company.strCpp = "  << this->strCpp  << std::endl;
		std::cout << "company.strs[] = ";
		for (int index = 0;  index < this->strs.length;  index++) {
			if (index > 0)
				std::cout << ",";
			std::cout << strs[index];
		}
		std::cout << std::endl;
		std::cout << "company.intsCpp[] = ";
		for (int index = 0;  index < (int)this->intsCpp.size();  index++) {
			if (index > 0)
				std::cout << ",";
			std::cout << intsCpp[index];
		}
		std::cout << std::endl;
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
	table->stdstring("strCpp", &Company::strCpp);
#ifdef SQ_CONFIG_JSON_SUPPORT
	table->custom("strs", &Company::strs, SQ_TYPE_STRING_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
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
	table->stdstring("strCpp", &Company::strCpp);
#ifdef SQ_CONFIG_JSON_SUPPORT
	table->custom("strs", &Company::strs, SQ_TYPE_STRING_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
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

void  storage_ptr_array_get_all(Sq::Storage* storage)
{
	SqPtrArray* array;
	Company*    company;

//	array = (SqPtrArray*)storage->get_all<Company>(NULL);    // deprecated
	array = (SqPtrArray*)storage->getAll<Company>(NULL);
	for (int i = 0;  i < array->length;  i++) {
		company = (Company*)array->data[i];
		company->print();
		delete company;
	}
}

void  storage_stl_container_get_all(Sq::Storage* storage)
{
	std::list<Company>* container;
	std::list<Company>::iterator cur, end;
//	std::vector<Company>* container;
//	std::vector<Company>::iterator cur, end;

	container = storage->getAll<std::list<Company>>();
//	container = storage->getAll<std::vector<Company>>();
	cur = container->begin();
	end = container->end();
	for (;  cur != end;  cur++)
		(*cur).print();

	// free
	delete container;
}

// ----------------------------------------------------------------------------

intptr_t  intptrArray1[] = { 1,  3,  5,  7};
intptr_t  intptrArray2[] = { 9, 11, 13, 15};
int       intArray1[]    = { 2,  4,  6,  8};
int       intArray2[]    = {10, 12, 14, 16};

void check_standard_layout()
{
	std::cout << "Sq::IntptrArray is standard layout = "
	          << std::is_standard_layout<Sq::IntptrArray>::value << std::endl;
	std::cout << "Sq::StringArray is standard layout = "
	          << std::is_standard_layout<Sq::StringArray>::value << std::endl;
	std::cout << "Sq::TypeStl<std::vector<int>> is standard layout = "
	          << std::is_standard_layout< Sq::TypeStl<std::vector<int>> >::value << std::endl;
	std::cout << "Company is standard layout = "
	          << std::is_standard_layout<Company>::value << std::endl;
}

int  main(int argc, char* argv[])
{
//	Sq::DbConfigSqlite* dbconfig;
	Sq::DbSqlite* db;
	Sq::Storage*  storage;
	Company*      company;

	check_standard_layout();

	db = new Sq::DbSqlite(NULL);
	storage = new Sq::Storage(db);

	storage->open("sample");

//	storage_make_fixed_schema(storage);
	storage_make_migrated_schema(storage);

	company = new Company();
	company->id = 1;
	company->name = strdup("Mr.T");
	company->age = 21;
	company->address = strdup("Norway");
	company->salary = 1200.00;
	company->strCpp = "test std::string 1";
	company->strs.append("str1");
	company->strs.append("str2");
	company->ints.append(intptrArray1, 4);
	company->intsCpp.assign(intArray1, intArray1+4);
//	storage->insert<Company>(company);
	storage->insert(company);
	delete company;

	company = new Company();
	company->id = 2;
	company->name = strdup("Paul");
	company->age = 32;
	company->address = strdup("Texas");
	company->salary = 3300.00;
	company->strCpp = "test std::string 2";
	company->strs.append("str3");
	company->strs.append("str4");
	company->ints.append(intptrArray2, 4);
	company->intsCpp.assign(intArray2, intArray2+4);
//	storage->insert<Company>(company);
	storage->insert(company);
	delete company;

	company = storage->get<Company>(1);
	company->print();
	delete company;

	// call Sq::Storage.getAll()
	storage_ptr_array_get_all(storage);
	storage_stl_container_get_all(storage);

	storage->close();
	delete storage;
	delete db;

	return 0;
}

