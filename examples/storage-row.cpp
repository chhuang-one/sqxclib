/*
 *   Copyright (C) 2022 by C.H. Huang
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
#include <assert.h>

#include <sqxclib.h>
#include <SqRow.h>

#define USE_SQLITE_IF_POSSIBLE        1
#define USE_MYSQL_IF_POSSIBLE         0
#define USE_POSTGRESQL_IF_POSSIBLE    0

/* If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct User       User;
typedef struct Company    Company;
 */

struct User {
	int    id;
	char  *name;
	int    company_id;

	time_t        created_at;
	time_t        updated_at;

	// --------------------------------
	// member functions

	static Sq::Schema *makeSchema(Sq::Schema *schema = NULL) {
		Sq::Table *table;

		if (schema == NULL)
			schema = new Sq::Schema();
		table = schema->create<User>("users");
		table->integer("id", &User::id)->primary();
		table->string("name", &User::name);
		table->integer("company_id", &User::company_id);
		// call table->timestamps() to use default column and member names
		table->timestamps<User>();
		table->addForeign("users_companies_id_foreign", "company_id")
		     ->reference("companies", "id")->onDelete("CASCADE")->onUpdate("CASCADE");;
		return schema;
	}
};

struct Company
{
	int    id;
	char  *name;
	int    age;
	char  *address;
	double salary;

	// --------------------------------
	// member functions

	static Sq::Schema *makeSchema(Sq::Schema *schema = NULL) {
		Sq::Table *table;

		if (schema == NULL)
			schema = new Sq::Schema();
		table = schema->create<Company>("companies");
		table->integer("id", &Company::id)->primary()->autoIncrement();
		table->string("name", &Company::name)->nullable();
		table->integer("age", &Company::age);
		table->string("address", &Company::address);
		table->double_("salary", &Company::salary);
		return schema;
	}
};

// ----------------------------------------------------------------------------

void printRow(Sq::Row *row)
{
	SqRowColumn *col;
	SqValue     *val;

	for (int i = 0;  i < row->length;  i++) {
		col = row->cols + i;
		val = row->data + i;

		// column name
		std::cout << col->name << " = ";

		// column value type is decided by 'col->type'
		if (SQ_TYPE_NOT_BUILTIN(col->type)) {
			if (col->type == NULL)
				std::cout << "(NULL)";
			else
				std::cout << "not built-in type";
		}
		else {
			switch (SQ_TYPE_BUILTIN_INDEX(col->type)) {
			case SQ_TYPE_INT_INDEX:
				std::cout << val->int_;
				break;

			case SQ_TYPE_UINT_INDEX:
				std::cout << val->uint;
				break;

			case SQ_TYPE_TIME_INDEX:
				std::cout << val->rawtime;
				break;

			case SQ_TYPE_DOUBLE_INDEX:
				std::cout << val->double_;
				break;

			case SQ_TYPE_STR_INDEX:
			case SQ_TYPE_CHAR_INDEX:
				std::cout << val->string;
				break;
			}
		}
		std::cout << std::endl;
	}
}

// ----------------------------------------------------------------------------

void makeSchema(Sq::Storage *storage)
{
	Sq::Schema *schema;

	schema = Company::makeSchema();
	schema->version = 1;
	storage->migrate(schema);
	delete schema;

	schema = User::makeSchema();
	schema->version = 2;
	storage->migrate(schema);
	delete schema;

	// synchronize schema to database. create/alter SQL tables based on storage->schema
	// This is mainly used by SQLite
	storage->migrate(NULL);
}

void insertData(Sq::Storage *storage)
{
	Company      *company;
	User         *user;

	// insert rows into table 'componies'
	company = new Company();
	company->id = 1;
	company->name = (char*)"Mr.T";
	company->age = 21;
	company->address = (char*)"Norway";
	company->salary = 1200.00;
	storage->insert(company);
	company->id = 2;
	company->name = (char*)"Paul";
	company->age = 32;
	company->address = (char*)"Texas";
	company->salary = 3300.00;
	storage->insert(company);
	delete company;

	// insert rows into table 'users'
	user = new User();
	user->id = 1;
	user->name = (char*)"Bob";
	user->company_id = 1;
	storage->insert(user);
	user->id = 2;
	user->name = (char*)"Tom";
	user->company_id = 2;
	storage->insert(user);
	delete user;
}

void  queryUnknown(Sq::Storage *storage)
{
	std::vector<Sq::Row*> *rowVector;
	std::vector<Sq::Row*>::iterator cur, end;
	Sq::Query   *query;

	std::cout << std::endl
	          << "queryUnknown(): use SqRow to parse UNKNOWN table and column" << std::endl;

	query = new Sq::Query;
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'", NULL);
	query->from("companies")->join("users",  "companies.id", "%s", "users.company_id");

	// specify the table type as SQ_TYPE_ROW.
	rowVector = storage->query< std::vector<Sq::Row*> >(query, SQ_TYPE_ROW);

	if (rowVector) {
		for (cur = rowVector->begin(), end = rowVector->end();  cur != end;  cur++) {
			// get Sq::Row from std::vector
			Sq::Row *row = (*cur);
			// handle columns in Sq::Row
			std::cout << "--- Sq::Row ---" << std::endl;
			printRow(row);
			delete row;
		}
		delete rowVector;
	}
	delete query;
}

void  queryKnown(Sq::Storage *storage)
{
	std::vector<Sq::Row*> *rowVector;
	std::vector<Sq::Row*>::iterator cur, end;
	Sq::Query   *query;

	std::cout << std::endl
	          << "queryKnown(): use SqRow to parse KNOWN table and column" << std::endl;

	query = new Sq::Query;
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'", NULL);
	query->from("companies")->join("users",  "companies.id", "%s", "users.company_id");

#if 0
	// replace default joint type by Sq::TypeRow in Sq::Storage
	delete storage->joint_default;
	storage->joint_default = (Sq::TypeJoint*) new Sq::TypeRow();
	rowVector = storage->query< std::vector<Sq::Row*> >(query);
#else
	Sq::TypeRow *rowType = new Sq::TypeRow();
	Sq::Type* type = storage->setupQuery(query, rowType);
	if (type != (Sq::Type*)rowType)
		std::cout << "'query' does NOT join multi-table" << std::endl;
	rowVector = storage->query< std::vector<Sq::Row*> >(query, rowType);
	delete rowType;
#endif

	if (rowVector) {
		for (cur = rowVector->begin(), end = rowVector->end();  cur != end;  cur++) {
			// get Sq::Row from std::vector
			Sq::Row *row = (*cur);
			// handle columns in Sq::Row
			std::cout << "--- Sq::Row ---" << std::endl;
			printRow(row);
			delete row;
		}
		delete rowVector;
	}
	delete query;
}

void queryOneTable(Sq::Storage *storage)
{
	std::vector<Sq::Row*> *rowVector;
	std::vector<Sq::Row*>::iterator cur, end;
	Sq::Query   *query;

	std::cout << std::endl
	          << "queryOneTable(): use SqRow to parse only one table" << std::endl;

	query = new Sq::Query;
	query->from("users");

#if 0
	// replace default joint type by Sq::TypeRow in Sq::Storage
	delete storage->joint_default;
	storage->joint_default = (Sq::TypeJoint*) new Sq::TypeRow();
	rowVector = storage->query< std::vector<Sq::Row*> >(query);
#else
	Sq::TypeRow *rowType = new Sq::TypeRow();
	Sq::Type* type = storage->setupQuery(query, rowType);
	if (type != (Sq::Type*)rowType)
		std::cout << "'query' does NOT join multi-table" << std::endl;
	rowVector = storage->query< std::vector<Sq::Row*> >(query, rowType);
	delete rowType;
#endif

	if (rowVector) {
		for (cur = rowVector->begin(), end = rowVector->end();  cur != end;  cur++) {
			// get Sq::Row from std::vector
			Sq::Row *row = (*cur);
			// handle columns in Sq::Row
			std::cout << "--- Sq::Row ---" << std::endl;
			printRow(row);
			delete row;
		}
		delete rowVector;
	}
	delete query;
}

void getRowStl(Sq::Storage *storage)
{
	std::vector<Sq::Row*> *rowVector;
	std::vector<Sq::Row*>::iterator cur, end;

	// specify the table type as SQ_TYPE_ROW.
	// specify the container type of returned data as std::vector<Sq::Row*>
	rowVector = storage->getAll< std::vector<Sq::Row*> >("users", SQ_TYPE_ROW, NULL);

	if (rowVector) {
		for (cur = rowVector->begin(), end = rowVector->end();  cur != end;  cur++) {
			// get Sq::Row from std::vector
			Sq::Row *row = (*cur);
			// handle columns in Sq::Row
			std::cout << "--- Sq::Row ---" << std::endl;
			printRow(row);
			delete row;
		}
		delete rowVector;
	}
}

void getRow(Sq::Storage *storage)
{
	Sq::Row     *row;

	// specify the table type as SQ_TYPE_ROW.
	row = (Sq::Row*)storage->get("users", SQ_TYPE_ROW, 1);
	printRow(row);
	delete row;

	// specify the table type as SQ_TYPE_ROW.
	Sq::PtrArray *array = (Sq::PtrArray*)storage->getAll("users", SQ_TYPE_ROW, NULL, NULL);

	if (array) {
		for (int  index = 0;  index < array->length;  index++) {
			// get Sq::Row from array
			row = (Sq::Row*)array->data[index];
			// handle columns in Sq::Row
			std::cout << "--- Sq::Row ---" << std::endl;
			printRow(row);
			delete row;
		}
		delete array;
	}
}

// ----------------------------------------------------------------------------

int  main(int argc, char *argv[])
{
	Sq::DbMethod *db;
	Sq::Storage  *storage;

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
	config_postgre.user = "postgres";
	config_postgre.password = "";

	db = new Sq::DbPostgre(&config_postgre);

#else
	std::cerr << "No supported database" << std::endl;
	return EXIT_SUCCESS;
#endif

	storage = new Sq::Storage(db);

	if (storage->open("sample-row") != SQCODE_OK) {
		std::cerr << "Can't open database - " << "sample-row" << std::endl;
		return EXIT_FAILURE;
	}

	makeSchema(storage);
	insertData(storage);
	queryUnknown(storage);
	queryKnown(storage);
	queryOneTable(storage);
	getRowStl(storage);
	getRow(storage);
}
