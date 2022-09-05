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


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <list>
#include <vector>
#include <string>
#include <iostream>

#include <sqxclib.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

#define USE_CXX_AGGREGATE_INITIALIZATION    0

// SqType for C++ std::vector<int>
Sq::TypeStl< std::vector<int> >  SqTypeIntVector(SQ_TYPE_INT);
#define SQ_TYPE_INT_VECTOR      &SqTypeIntVector

typedef struct User       User;
typedef struct Company    Company;

struct User {
	int    id;
	char  *name;
	int    company_id;

	time_t        created_at;
	time_t        updated_at;

	unsigned int  test_add;

	// --------------------------------
	// member functions
	void print() {
		char *created_at = sq_time_to_string(this->created_at, 0);
		char *updated_at = sq_time_to_string(this->updated_at, 0);

		std::cout << std::endl
		          << "user.id = "         << this->id         << std::endl
		          << "user.name = "       << this->name       << std::endl
		          << "user.company_id = " << this->company_id << std::endl
		          << "user.created_at = " << created_at       << std::endl
		          << "user.updated_at = " << updated_at       << std::endl
		          << std::endl;
		free(created_at);
		free(updated_at);
	}
};

struct Company
{
	int    id;
	char  *name;
	int    age;
	char  *address;
	double salary;

	time_t updated_at;   // alter table add column

	// make sure that SQ_CONFIG_HAVE_JSONC is enabled if you want to store array (vector) in SQL column
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

void  company_array_print(SqPtrArray *array)
{
	Company   *company;
	int        index;

	for (index = 0;  index < array->length;  index++) {
		company = (Company*)(array->data[index]);
		company->print();
	}
}

#if USE_CXX_AGGREGATE_INITIALIZATION == 1

// ----------------------------------------------------------------------------
// use C++ aggregate initialization to define static SqColumn

static const SqForeign userForeign = {"companies",  "id",  "CASCADE",  "CASCADE"};
static const char     *userForeignComposite[] = {"company_id", NULL};
static const char     *userIndexComposite[]   = {"id", NULL};

// CREATE TABLE "users"
static const SqColumn userColumns[] = {
	{SQ_TYPE_INT,    "id",           offsetof(User, id),           SQB_PRIMARY | SQB_HIDDEN},
	{SQ_TYPE_STRING, "name",         offsetof(User, name),         0,
		NULL,                              // .old_name
		50},                               // .size    // VARCHAR(50)
	{SQ_TYPE_TIME,   "created_at",   offsetof(User, created_at),   SQB_CURRENT},    // DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at",   offsetof(User, updated_at),   SQB_CURRENT | SQB_CURRENT_ON_UPDATE},
	/* FOREIGN KEY
	{SQ_TYPE_INT,    "company_id",   offsetof(User, company_id),   0,
		NULL, 0, 0, NULL, NULL,            // .old_name, .size, .digits, .default_value, .check
		(SqForeign*)&userForeign},         // .foreign
	 */
	// CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,   "users_companies_id_foreign", 0, 0,
		NULL, 0, 0, NULL, NULL,            // .old_name, .size, .digits, .default_value, .check
		(SqForeign*)&userForeign,          // .foreign
		(char **) userForeignComposite },  // .composite
	// INDEX
	{SQ_TYPE_INDEX,  "users_id_index", 0, 0,
		NULL, 0, 0, NULL, NULL,            // .old_name, .size, .digits, .default_value, .check
		NULL,                              // .foreign
		(char **) userIndexComposite },    // .composite
};
#endif  // USE_CXX_AGGREGATE_INITIALIZATION

// ----------------------------------------------------------------------------
// use C++ functions to define dynamic SqColumn

void  storage_make_fixed_schema(Sq::Storage *storage)
{
	Sq::Schema   *schema;
	Sq::Table    *table;

	// create table in storage->schema
	schema = (Sq::Schema*)storage->schema;
	schema->version++;

	table = schema->create<Company>("companies");
	table->integer("id", &Company::id)->primary()->autoIncrement();
	table->string("name", &Company::name)->nullable();
	table->integer("age", &Company::age);
	table->string("address", &Company::address);
	table->double_("salary", &Company::salary);
	table->stdstring("strCpp", &Company::strCpp);
#if SQ_CONFIG_HAVE_JSONC
	table->custom("strs", &Company::strs, SQ_TYPE_STRING_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
	table->custom("ints", &Company::ints, SQ_TYPE_INTPTR_ARRAY);
#endif

	table = schema->create<User>("users");
#if USE_CXX_AGGREGATE_INITIALIZATION == 1
	// create table by static columns
	table->addColumn(userColumns, sizeof(userColumns) / sizeof(SqColumn));
#else
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name, 50);
	table->integer("company_id", &User::company_id);
	// call table->timestamps() to use default column names
	table->timestamps(&User::created_at,
	                  &User::updated_at);
	// CONSTRAINT FOREIGN KEY
	table->addForeign("users_companies_id_foreign", "company_id")
	     ->reference("companies", "id")->onDelete("CASCADE")->onUpdate("CASCADE");
	// INDEX
	table->addIndex("users_id_index", "id");
#endif

	// synchronize schema to database. create/alter SQL tables based on storage->schema
	// This is mainly used by SQLite
	storage->migrate(NULL);
}

void  storage_make_migrated_schema(Sq::Storage *storage)
{
	Sq::Schema   *schemaVer1;
	Sq::Schema   *schemaVer2;
	Sq::Schema   *schemaVer3;
	Sq::Schema   *schemaVer4;
	Sq::Schema   *schemaVer5;
	Sq::Table    *table;

	// --- schema version 1 ---
	schemaVer1 = new Sq::Schema("Ver1");
	schemaVer1->version = 1;
	table = schemaVer1->create<Company>("companies");
	table->integer("id", &Company::id)->primary()->autoIncrement();
	table->string("name", &Company::name)->nullable();
	table->integer("age", &Company::age);
	table->string("address", &Company::address);
	table->double_("salary", &Company::salary);
	table->stdstring("strCpp", &Company::strCpp);
#if SQ_CONFIG_HAVE_JSONC
	table->custom("strs", &Company::strs, SQ_TYPE_STRING_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
	table->custom("ints", &Company::ints, SQ_TYPE_INTPTR_ARRAY);
#endif

	// --- schema version 2 ---
	schemaVer2 = new Sq::Schema("Ver2");
	schemaVer2->version = 2;
	table = schemaVer2->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("company_id", &User::company_id);
#if   1
	// call table->timestamps() to use default column and member names
	table->timestamps<User>();
#elif 0
	// call table->timestamps() to use default column names
	table->timestamps(&User::created_at,
	                  &User::updated_at);
#elif 0
	// call table->timestamps() to specify column names
	table->timestamps("created_at", &User::created_at,
	                  "updated_at", &User::updated_at);
#elif 0
	// call table->timestamps() will do these
	table->timestamp("created_at", &User::created_at)->useCurrent();
	table->timestamp("updated_at", &User::updated_at)->useCurrent()->useCurrentOnUpdate();
#endif
	table->addForeign("users_companies_id_foreign", "company_id")
	     ->reference("companies", "id")->onDelete("CASCADE")->onUpdate("CASCADE");;
	table->addIndex("users_id_index", "id");

	// --- schema version 3 ---
	schemaVer3 = new Sq::Schema("Ver3");
	schemaVer3->version = 3;
	// ALTER TABLE users
	table = schemaVer3->alter("users");
	table->string("name", &User::name, 256)->nullable()->change();
	table->uint("test_add", &User::test_add);
	table->dropColumn("updated_at");
	table->dropForeign("users_companies_id_foreign");

	// --- schema version 4 ---
	schemaVer4 = new Sq::Schema("Ver4");
	schemaVer4->version = 4;
	// ALTER TABLE users ADD COLUMN test_add
	table = schemaVer4->alter("users");
	table->renameColumn("test_add", "test_add2");

	// --- schema version 5 ---
	schemaVer5 = new Sq::Schema("Ver5");
	schemaVer5->version = 5;
	// ALTER TABLE companies ADD COLUMN updated_at
	table = schemaVer5->alter("companies");
	table->timestamp("updated_at", &Company::updated_at)->useCurrent()->useCurrentOnUpdate();

	// migrate schema version from 1 to 5 to storage->schema
	storage->migrate(schemaVer1);
	storage->migrate(schemaVer2);
	storage->migrate(schemaVer3);
	storage->migrate(schemaVer4);
	storage->migrate(schemaVer5);
	// synchronize schema to database. create/alter SQL tables based on storage->schema
	storage->migrate(NULL);

	// free migrated schema
	delete schemaVer1;
	delete schemaVer2;
	delete schemaVer3;
	delete schemaVer4;
	delete schemaVer5;
}

void  storage_ptr_array_get_all(Sq::Storage *storage)
{
	Sq::PtrArray *array;
	Company      *company;

	array = (Sq::PtrArray*)storage->getAll<Company>(NULL, NULL);
	if (array) {
		for (int i = 0;  i < array->length;  i++) {
			company = (Company*)array->data[i];
			company->print();
			delete company;
		}
		delete array;
	}
}

void  storage_stl_container_get_all(Sq::Storage *storage)
{
	std::list<Company> *container;
	std::list<Company>::iterator cur, end;
//	std::vector<Company> *container;
//	std::vector<Company>::iterator cur, end;

	container = storage->getAll< std::list<Company> >();
//	container = storage->getAll< std::vector<Company> >();
	if (container) {
		for (cur = container->begin(), end = container->end();  cur != end;  cur++)
			(*cur).print();
		// free
		delete container;
	}
}

void  storage_ptr_array_query_join(Sq::Storage *storage)
{
	Sq::PtrArray  *array;
	Sq::Query     *query;
	void         **element;
	Company       *company;
	User          *user;

	query = new Sq::Query();
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'", NULL);
	query->from("companies")->join("users",  "companies.id", "users.company_id");

	array = (Sq::PtrArray*)storage->query(query);
	if (array) {
		for (int i = 0;  i < array->length;  i++) {
			element = (void**)array->data[i];
			company = (Company*)element[0];
			company->print();
			delete company;
			user = (User*)element[1];
			user->print();
			delete user;
			free(element);
		}
		delete array;
	}
	delete query;
}

void  storage_stl_container_query_join(Sq::Storage *storage)
{
	std::vector< Sq::Joint<2> > *j2vector;
	std::vector< Sq::Joint<2> >::iterator cur, end;
	Sq::Query   *query;
	Company     *company;
	User        *user;

	query = new Sq::Query;
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'", NULL);
	query->from("companies")->join("users",  "companies.id", "users.company_id");

	j2vector = storage->query< std::vector< Sq::Joint<2> > >(query);
	if (j2vector) {
		for (cur = j2vector->begin(), end = j2vector->end();  cur != end;  cur++) {
			Sq::Joint<2> &element = (*cur);
			company = (Company*)element[0];
			company->print();
			delete company;
			user = (User*)element[1];
			user->print();
			delete user;
		}
		delete j2vector;
	}

	delete query;
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
	          << std::is_standard_layout< Sq::TypeStl< std::vector<int> > >::value << std::endl;
	std::cout << "Company is standard layout = "
	          << std::is_standard_layout<Company>::value << std::endl;
}

// ----------------------------------------------------------------------------

int  main(int argc, char *argv[])
{
	Sq::DbMethod *db;
	Sq::Storage  *storage;
	Company      *company;
	User         *user;

	check_standard_layout();

#if   SQ_CONFIG_HAVE_SQLITE && USE_SQLITE_IF_POSSIBLE
	SqdbConfigSqlite  config_sqlite;

	config_sqlite.folder    = ".";    // "/tmp"
	config_sqlite.extension = "db";

	db = new Sq::DbSqlite(&config_sqlite);

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
	SqdbConfigMysql  config_mysql;

	config_mysql.host = "localhost";
	config_mysql.port = 3306;
	config_mysql.user = "root";
	config_mysql.password = "";

	db = new Sq::DbMysql(&config_mysql);

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
	SqdbConfigPostgre  config_postgre;

	config_postgre.host = "localhost";
	config_postgre.port = 5432;
	config_postgre.user = "postgre";
	config_postgre.password = "";

	db = new Sq::DbPostgre(&config_postgre);

#else
	std::cerr << "No supported database" << std::endl;
	return EXIT_SUCCESS;
#endif

	storage = new Sq::Storage(db);

	if (storage->open("sample-cxx") != SQCODE_OK) {
		std::cerr << "Can't open database - " << "sample-cxx" << std::endl;
		return EXIT_FAILURE;
	}

	// --- make schema
//	storage_make_fixed_schema(storage);
	storage_make_migrated_schema(storage);

	// --- add rows to companies table
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

	// --- add rows to users table
	user = new User();

	user->id = 1;
	user->name = (char*)"Bob";
	user->company_id = 1;
	storage->insert(user);

	user->id = 2;
	user->name = (char*)"Tom";
	user->company_id = 2;
	storage->insert(user);

	user->name = NULL;
	delete user;

	// --- get data from database
	company = storage->get<Company>(1);
	if (company) {
		company->print();
		delete company;
	}

	// call Sq::Storage.getAll()
	storage_ptr_array_get_all(storage);
	storage_stl_container_get_all(storage);

	// call Sq::Storage.query()
	storage_ptr_array_query_join(storage);
	storage_stl_container_query_join(storage);

	storage->close();
	delete storage;
	delete db;

	return EXIT_SUCCESS;
}

