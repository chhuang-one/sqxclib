/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

/*
	Functions storage_make_migrated_schema() and storage_make_fixed_schema() are
	example code for dynamically defining tables and columns.

	This example code also use C++ designated initialization to define constant columns in tables.
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

struct User
{
	int    id;
	char  *name;
	int    company_id;

	std::string   comment;    // SQL Type: TEXT

	// If you use SQLite or MySQL to store binary data in database column,
	// Please make sure that SQ_CONFIG_QUERY_ONLY_COLUMN is enabled.
	// If you use PostgreSQL to do this, you don't need to care about SQ_CONFIG_QUERY_ONLY_COLUMN.
	Sq::Buffer         picture;    // SQL Type: BLOB, BINARY...etc
	std::vector<char>  angleShot;  // SQL Type: BLOB, BINARY...etc

	time_t        created_at;
	time_t        updated_at;

	unsigned int  test_add;

	// --------------------------------
	// member functions
	void print() {
		char *created_at = sq_time_to_string(this->created_at, 0);
		char *updated_at = sq_time_to_string(this->updated_at, 0);

		std::cout << "user.id = "         << this->id         << std::endl
		          << "user.name = "       << this->name       << std::endl
		          << "user.comment = "    << this->comment    << std::endl
		          << "user.company_id = " << this->company_id << std::endl
		          << "user.created_at = " << created_at       << std::endl
		          << "user.updated_at = " << updated_at       << std::endl;
		free(created_at);
		free(updated_at);

		int   hex_size;
		char *hex_mem;

		hex_size = this->picture.writed * 2;
		hex_mem  = (char*)malloc(hex_size + 1);
		hex_mem[hex_size] = 0;
		sq_bin_to_hex(hex_mem, this->picture.mem, this->picture.writed);
		std::cout << "user.picture has " << this->picture.writed << " bytes" << std::endl
		          << "user.picture = 0x" << hex_mem << std::endl;
		free(hex_mem);

		hex_size = (int)this->angleShot.size() * 2;
		hex_mem  = (char*)malloc(hex_size + 1);
		hex_mem[hex_size] = 0;
		sq_bin_to_hex(hex_mem, this->angleShot.data(), (int)this->angleShot.size());
		std::cout << "user.angleShot has " << this->angleShot.size() << " bytes" << std::endl
		          << "user.angleShot = 0x" << hex_mem << std::endl;
		free(hex_mem);

		std::cout << std::endl;
	}
};

struct Company
{
	int    id;
	char  *name;
	int    age;
	char  *address;
	double salary;

	time_t        created_at;   // alter table
	time_t        updated_at;   // alter table

	// make sure that SQ_CONFIG_HAVE_JSONC is enabled if you want to store array (vector) in database column
	Sq::IntArray     ints;    // C/C++ array for int
	Sq::StrArray     strs;    // C/C++ array for char*
	std::vector<int> intsCpp; // C++ type, it use Sq::TypeStl<std::vector<int>>
	std::string      strCpp;  // C++ type

	// --------------------------------
	// member functions

	void print() {
		std::cout << "company.id = "      << this->id      << std::endl
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
			std::cout << this->ints[index];
		}
		std::cout << std::endl;

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

//                                  {     table ,column,separator,  ON DELETE, ON UPDATE, NULL};
static const char  *userForeign[] = {"companies",  "id",       "",  "CASCADE", "CASCADE", NULL};
static const char  *userForeignComposite[] = {"company_id", NULL};
static const char  *userIndexComposite[]   = {"id", NULL};

// CREATE TABLE "users"
static const SqColumn userColumns[] = {
	{SQ_TYPE_INT,     "id",           offsetof(User, id),           SQB_PRIMARY | SQB_HIDDEN},
	{SQ_TYPE_STR,     "name",         offsetof(User, name),         0,
		NULL,                              // .old_name
		0,                                 // .sql_type
		50},                               // .size    // VARCHAR(50)

	// type mapping: SQ_TYPE_STD_STR map to SQL data type - TEXT
	{SQ_TYPE_STD_STR, "comment",      offsetof(User, comment),      0,
		NULL,                              // .old_name
		SQ_SQL_TYPE_TEXT},                 // .sql_type

#if SQ_CONFIG_QUERY_ONLY_COLUMN
	// get length of picture and set it to Sq::Buffer::size before parsing picture.
	// This is mainly used by SQLite, MySQL to get length of BLOB column.
	// If you use PostgreSQL and don't need store result of special query to C structure's member,
	// you can disable SQ_CONFIG_QUERY_ONLY_COLUMN.
	{SQ_TYPE_INT,     "length(picture)", offsetof(User, picture.size), SQB_QUERY_ONLY},
#endif
	{SQ_TYPE_BUFFER,  "picture",         offsetof(User, picture),      0,
		NULL,                              // .old_name
		SQ_SQL_TYPE_BLOB},                 // .sql_type

#if SQ_CONFIG_QUERY_ONLY_COLUMN
	// get length of angle_shot and call std::vector<char>::resize() before parsing angle_shot.
	// This is mainly used by SQLite, MySQL to get length of BLOB column.
	// If you use PostgreSQL and don't need store result of special query to C structure's member,
	// you can disable SQ_CONFIG_QUERY_ONLY_COLUMN.
	{SQ_TYPE_STD_VECTOR_SIZE,  "length(angle_shot)",  offsetof(User, angleShot),    SQB_QUERY_ONLY},
#endif
	{SQ_TYPE_STD_VECTOR,       "angle_shot",          offsetof(User, angleShot),    0,
		NULL,                              // .old_name
		SQ_SQL_TYPE_BLOB},                 // .sql_type

	// created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,    "created_at",   offsetof(User, created_at),   SQB_CURRENT},
	// updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,    "updated_at",   offsetof(User, updated_at),   SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// This column is used by CONSTRAINT FOREIGN KEY below.
	{SQ_TYPE_INT,     "company_id",   offsetof(User, company_id),   0},
	// CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,   "users_companies_id_foreign",       0,   SQB_FOREIGN,
		NULL,                              // .old_name,
		0, 0, 0,                           // .sql_type, .size, .digits,
		NULL,                              // .default_value,
		(char **) userForeign,             // .foreign
		(char **) userForeignComposite },  // .composite

	// INDEX
	{SQ_TYPE_INDEX,   "users_id_index",  0,  0,
		NULL,                              // .old_name,
		0, 0, 0,                           // .sql_type, .size, .digits,
		NULL,                              // .default_value,
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
	table->custom("strs", &Company::strs, SQ_TYPE_STR_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
	table->custom("ints", &Company::ints, SQ_TYPE_INT_ARRAY);
#endif

	table = schema->create<User>("users");
#if USE_CXX_AGGREGATE_INITIALIZATION == 1
	// create table by static columns
	table->addColumn(userColumns, SQ_N_ELEMENTS(userColumns));
#else
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name, 50);
	table->integer("company_id", &User::company_id);
	// type mapping: SQ_TYPE_STD_STR map to SQL data type - TEXT
	table->mapping("comment", &User::comment,
	               SQ_TYPE_STD_STR,
	               SQ_SQL_TYPE_TEXT);
	// binary, blob
	table->binary("picture", &User::picture);
	table->stdvector("angle_shot", &User::angleShot);
	// call table->timestamps() to use default column names
	table->timestamps(&User::created_at,
	                  &User::updated_at);
	// CONSTRAINT FOREIGN KEY
	table->addForeign("users_companies_id_foreign", "company_id")
	     ->reference("companies", "id")->onDelete("CASCADE")->onUpdate("CASCADE");
	// INDEX
	table->addIndex("users_id_index", "id");
#endif

	// synchronize schema to database. create/alter database tables based on storage->schema
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
	schemaVer1 = new Sq::Schema(1, "Ver1");
	table = schemaVer1->create<Company>("companies");
	table->integer("id", &Company::id)->primary()->autoIncrement();
	table->string("name", &Company::name)->nullable();
	table->integer("age", &Company::age);
	table->string("address", &Company::address);
	table->double_("salary", &Company::salary);
	table->stdstring("strCpp", &Company::strCpp);
#if SQ_CONFIG_HAVE_JSONC
	table->custom("strs", &Company::strs, SQ_TYPE_STR_ARRAY);
	table->custom("intsCpp", &Company::intsCpp, SQ_TYPE_INT_VECTOR);
	table->custom("ints", &Company::ints, SQ_TYPE_INT_ARRAY);
#endif

	// --- schema version 2 ---
	schemaVer2 = new Sq::Schema(2, "Ver2");
	table = schemaVer2->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->integer("company_id", &User::company_id);
	// type mapping: SQ_TYPE_STD_STR map to SQL data type - TEXT
	table->mapping("comment", &User::comment,
	               SQ_TYPE_STD_STR,
	               SQ_SQL_TYPE_TEXT);
	// binary, blob
	table->binary("picture", &User::picture);
	table->stdvector("angle_shot", &User::angleShot);
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
	// CONSTRAINT 'users_companies_id_foreign' will be dropped in schema ver 3.
	table->addForeign("users_companies_id_foreign", "company_id")
	     ->reference("companies", "id")->onDelete("CASCADE")->onUpdate("CASCADE");;
	table->addIndex("users_id_index", "id");

	// --- schema version 3 ---
	schemaVer3 = new Sq::Schema(3, "Ver3");
	// ALTER TABLE users
	table = schemaVer3->alter("users");
	table->string("name", &User::name, 256)->nullable()->change();
	table->uint("test_add", &User::test_add);
	table->dropForeign("users_companies_id_foreign");

	// --- schema version 4 ---
	schemaVer4 = new Sq::Schema(4, "Ver4");
	// ALTER TABLE users ADD COLUMN test_add
	table = schemaVer4->alter("users");
	table->renameColumn("test_add", "test_add2");

	// --- schema version 5 ---
	schemaVer5 = new Sq::Schema(5, "Ver5");
	// ALTER TABLE companies ADD COLUMN
	table = schemaVer5->alter("companies");
	table->timestamp("created_at", &Company::created_at)->useCurrent();
	table->timestamp("updated_at", &Company::updated_at)->useCurrent()->useCurrentOnUpdate();

	// migrate schema version from 1 to 5 to storage->schema
	storage->migrate(schemaVer1);
	storage->migrate(schemaVer2);
	storage->migrate(schemaVer3);
	storage->migrate(schemaVer4);
	storage->migrate(schemaVer5);
	// synchronize schema to database. create/alter database tables based on storage->schema
	storage->migrate(NULL);

	// free migrated schema
	delete schemaVer1;
	delete schemaVer2;
	delete schemaVer3;
	delete schemaVer4;
	delete schemaVer5;
}

void  storage_get_all_ptr_array(Sq::Storage *storage)
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

void  storage_get_all_stl_container(Sq::Storage *storage)
{
	std::list<Company> *container;
	std::list<Company>::iterator cur, end;
//	std::vector<Company> *container;
//	std::vector<Company>::iterator cur, end;

	container = storage->getAll< std::list<Company> >();
//	container = storage->getAll< std::vector<Company> >();
	if (container) {
		for (cur = container->begin(), end = container->end();  cur != end;  cur++)
			cur->print();
		// free
		delete container;
	}
}

// Sq::Storage::query() doesn't use Sq::Storage::joint_default because only 1 table in query.
void  storage_query_ptr_array(Sq::Storage *storage)
{
	Sq::PtrArray  *array;
	Sq::Query     *query;
	union {
		Company       *company;
		User          *user;
	} temp;

	query = new Sq::Query();
//	query->select("id", "name");
#if 0
	query->from("companies");
#else
	query->from("users");
#endif

	array = (Sq::PtrArray*)storage->query(query);
	if (array) {
		for (int i = 0;  i < array->length;  i++) {
#if 0
			temp.company = (Company*)array->data[i];
			temp.company->print();
			delete temp.company;
#else
			temp.user = (User*)array->data[i];
			temp.user->print();
			delete temp.user;
#endif
		}
		delete array;
	}
	delete query;
}

// Sq::Storage::query() use Sq::Storage::joint_default because multiple tables in query.
void  storage_query_join_array(Sq::Storage *storage)
{
	typedef void *Joint2[2];
	Sq::Array<Joint2> *array;
	Sq::Array<Joint2>::iterator cur, end;
	Sq::Query   *query;
	void       **element;
	Company     *company;
	User        *user;

	query = new Sq::Query;
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'");
	query->from("companies")->join("users", "companies.id", "=", "%s", "users.company_id");

	array = (Sq::Array<Joint2>*)storage->query(query, SQ_TYPE_ARRAY);
	if (array) {
		for (cur = array->begin(), end = array->end();  cur != end;  cur++) {
			element = *cur;
			company = (Company*)element[0];
			company->print();
			delete company;
			user = (User*)element[1];
			user->print();
			delete user;
		}
		delete array;
	}

	delete query;
}

void  storage_query_join_ptr_array(Sq::Storage *storage)
{
	Sq::PtrArray  *array;
	Sq::Query     *query;
	void         **element;
	Company       *company;
	User          *user;

	query = new Sq::Query();
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'");
	query->from("companies");
	query->join("users", "companies.id", "users.company_id");
//	query->join("users", "companies.id", "%s", "users.company_id");
//	query->join("users", "companies.id", "=",  "users.company_id");

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

void  storage_query_join_stl_container(Sq::Storage *storage)
{
	std::vector< Sq::Joint<2> > *j2vector;
	std::vector< Sq::Joint<2> >::iterator cur, end;
	Sq::Query   *query;
	Company     *company;
	User        *user;

	query = new Sq::Query;
//	query->select("companies.id AS 'companies.id'", "users.id AS 'users.id'");
	query->from("companies")->join("users", "companies.id", "=", "%s", "users.company_id");

	j2vector = storage->query< std::vector< Sq::Joint<2> > >(query);
	if (j2vector) {
		for (cur = j2vector->begin(), end = j2vector->end();  cur != end;  cur++) {
			Sq::Joint<2> &element = *cur;
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

int  intArray1[] = { 1,  3,  5,  7};
int  intArray2[] = { 9, 11, 13, 15};
int  intArray3[] = { 2,  4,  6,  8};
int  intArray4[] = {10, 12, 14, 16};

void check_standard_layout()
{
	std::cout << "Sq::IntArray is standard layout = "
	          << std::is_standard_layout<Sq::IntArray>::value << std::endl;
	std::cout << "Sq::StrArray is standard layout = "
	          << std::is_standard_layout<Sq::StrArray>::value << std::endl;
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
	SqdbConfigSqlite  configSqlite;

	configSqlite.folder    = ".";    // "/tmp"
	configSqlite.extension = "db";

	db = new Sq::DbSqlite(configSqlite);

#elif SQ_CONFIG_HAVE_MYSQL  && USE_MYSQL_IF_POSSIBLE
	SqdbConfigMysql  configMysql;

	configMysql.host = "localhost";
	configMysql.port = 3306;
	configMysql.user = "root";
	configMysql.password = "";

	db = new Sq::DbMysql(configMysql);

#elif SQ_CONFIG_HAVE_POSTGRESQL && USE_POSTGRESQL_IF_POSSIBLE
	SqdbConfigPostgre  configPostgre;

	configPostgre.host = "localhost";
	configPostgre.port = 5432;
	configPostgre.user = "postgres";
	configPostgre.password = "";

	db = new Sq::DbPostgre(configPostgre);

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
	company->ints.append(intArray1, 4);
	company->intsCpp.assign(intArray3, intArray3+4);
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
	company->ints.append(intArray2, 4);
	company->intsCpp.assign(intArray4, intArray4+4);
//	storage->insert<Company>(company);
	storage->insert(company);
	delete company;

	// --- add rows to users table
	user = new User();

	user->id = 1;
	user->name = (char*)"Bob";
	user->comment = (char*)"-- comment text 1";
	user->picture.mem = (char*)"1 binary\x00 1";
	user->picture.writed = (int)strlen(user->picture.mem) + 3;
	user->angleShot.assign(user->picture.mem, user->picture.mem+4);
	user->company_id = 1;
	storage->insert(user);

	user->id = 2;
	user->name = (char*)"Tom";
	user->comment = (char*)"-- comment text 2";
	user->picture.mem = (char*)"2 binary\x00 2";
	user->picture.writed = (int)strlen(user->picture.mem) + 3;
	user->angleShot.assign(user->picture.mem, user->picture.mem+4);
	user->angleShot.push_back(0);
	user->angleShot.push_back('X');
	user->company_id = 2;
	storage->insert(user);

	user->name = NULL;
	user->picture.mem = NULL;
	delete user;

	// --- get data from database
	company = storage->get<Company>(1);
	if (company) {
		company->print();
		delete company;
	}

	// call Sq::Storage.getAll()
	storage_get_all_ptr_array(storage);
	storage_get_all_stl_container(storage);

	// call Sq::Storage.query()
	storage_query_ptr_array(storage);
	storage_query_join_array(storage);
	storage_query_join_ptr_array(storage);
	storage_query_join_stl_container(storage);

	storage->close();
	delete storage;
	delete db;

	return EXIT_SUCCESS;
}

