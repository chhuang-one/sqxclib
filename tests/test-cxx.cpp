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
#include <assert.h>
#include <stdio.h>
#include <type_traits>  // is_standard_layout<>
#include <iostream>     // cout

#include <SqSchema.h>
#include <SqSchema-macro.h>

#include <SqQuery.h>
#include <SqQuery-macro.h>

#include <SqdbEmpty.h>
#include <SqxcEmpty.h>
#include <SqStorage.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

using namespace std;


// ----------------------------------------------------------------------------
// SqSchema

typedef struct User       User;
typedef struct Company    Company;

struct User
{
	int    id;
	char*  name;
	char*  email;
};

struct Company
{
	int    id;
	char*  name;
	int    age;
	char*  address;
	double salary;
};

static const SqColumn  UserColumnArray[] = {
	{SQ_TYPE_INT,      "id",      offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
	{SQ_TYPE_STR,      "name",    offsetof(User, name),  0 },
	{SQ_TYPE_STR,      "email",   offsetof(User, email), 0 },
};

static const SqColumn  *UserColumns[] = {
	&UserColumnArray[0],
	&UserColumnArray[1],
	&UserColumnArray[2],
};

void test_schema_with_aggregate_initializer()
{
	Sq::Schema* schema;
	Sq::Table*  table;

	schema = new Sq::Schema("current");
	table = schema->create<Company>("companies");
	table->addColumn(UserColumns, SQ_N_PTRS(UserColumns));
	delete schema;
}

void test_schema()
{
	Sq::Schema* schema;
	Sq::Table*  table;

	schema = new Sq::Schema("current");

//	table = schema->create("companies", NULL);
	table = schema->create<Company>("companies");
	table->integer("id", &Company::id)->primary();
	table->string("name", &Company::name);
	table->integer("age", &Company::age);
	table->string("address", &Company::address, 50)->nullable();
	table->double_("salary", &Company::salary);

	delete schema;
}

// ----------------------------------------------------------------------------
// SqQuery

void test_query_cpp()
{
	Sq::Query  *query;
	char       *sql;
	const char *result;

	query = new Sq::Query();

	// --- test 1 ---
	result = "SELECT DISTINCT age, name, u.id "
	         "FROM city "
	         "JOIN ( SELECT * FROM User WHERE name = tom ) AS u ON u.city_id = city.id "
	         "WHERE id > 30 AND ( age < 15 AND name = 'abc' ) ORDER BY name DESC, id";

	query->from("city")
		 ->join([query] {
			query->from("User")
			     ->where("name", "tom");
		 })
		   ->as("u")
		   ->on("u.city_id", "=", "city.id")
		 ->whereRaw("id > %d", 30)
		 ->where([query] {
			query->where("age", "<", 15)
				 ->whereRaw("name = 'abc'");
		 })
		 ->select("age", "name", "u.id")->distinct()
		 ->orderByDesc("name")->orderBy("id");

	sql = query->toSql();
	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	// --- test 2 ---
	result = "SELECT * FROM users "
	         "WHERE NOT ( id IN (2,8,16) OR votes NOT BETWEEN 100 AND 200 ) "
			 "AND EXISTS ( SELECT * FROM cities WHERE name NOT IN ('DC') )";

	query->clear();
	query->from("users")
	     ->whereNot([query] {
	         query->whereIn("id", 2, 8, 16)
			      ->orWhereNotBetween("votes", 100, 200);
	     })
	     ->whereExists([query] {
	         query->from("cities")
	              ->whereNotIn("name", 0, "'%s'", "DC");
		 });

	sql = query->toSql();
	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	// --- test 3 ---
	result = "SELECT * FROM products "
	         "WHERE price < ( SELECT AVG(amount) FROM incomes )";

	query->clear();
	query->from("products")
	     ->where("price", "<", [query] {
	         query->selectRaw("AVG(amount)")
	              ->from("incomes");
	     });

	sql = query->toSql();
	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	// release memory
	delete query;
}

void test_query_cpp_convenient_class()
{
	std::string sql;
	Sq::Where where;

	sql = where("id", 3).orWhereRaw("city_id < %d", 20).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id = 3 OR city_id < 20") == 0);

	sql = Sq::Query("users").whereRaw("id < 20").c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "SELECT * FROM users WHERE id < 20") == 0);

	sql = Sq::whereRaw("id > %d", 10).where("id", "<", 99).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id > 10 AND id < 99") == 0);

	sql = Sq::whereRaw("id < 13").orWhere("city_id", "<", 22).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id < 13 OR city_id < 22") == 0);

	sql = Sq::whereNot("id", "<", 23).orWhereNot("city_id", "<", "%d", 33).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE NOT id < 23 OR NOT city_id < 33") == 0);

	sql = Sq::whereNotRaw("id < 33").orWhereNotRaw("city_id < 55").c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE NOT id < 33 OR NOT city_id < 55") == 0);

	// lambda
	sql = Sq::where("price", "<", [](SqQuery &query) {
		query.selectRaw("AVG(amount)").from("incomes");
	}).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE price < ( SELECT AVG(amount) FROM incomes )") == 0);

	// whereExists
	sql = Sq::whereExists([](SqQuery *query) {
		query->from("user")
		     ->whereIn("id", 3, 7, 11);
	}).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE EXISTS ( SELECT * FROM user WHERE id IN (3,7,11) )") == 0);

	// whereNotExists
	sql = Sq::whereNotExists([](SqQuery &query) {
		query.from("user")
		     .whereNotIn("id", 3, 7, 11);
	}).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE NOT EXISTS ( SELECT * FROM user WHERE id NOT IN (3,7,11) )") == 0);

	// whereBetween
	sql = Sq::whereBetween("id", 1, 13).orWhereBetween("city_id", 22, 50).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id BETWEEN 1 AND 13 OR city_id BETWEEN 22 AND 50") == 0);

	sql = Sq::WhereBetween("name", "Ben", "Ch").c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE name BETWEEN 'Ben' AND 'Ch'") == 0);

	// whereIn
	sql = Sq::whereIn("id", 1, 9, 13).orWhereIn("city_id", 22, 50, 74).c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id IN (1,9,13) OR city_id IN (22,50,74)") == 0);

	// whereNull
	sql = Sq::whereNull("id").orWhereNotNull("city_id").c();
	std::cout << sql << std::endl;
	assert(strcmp(sql.c_str(), "WHERE id IS NULL OR city_id IS NOT NULL") == 0);
}

void test_query_cpp_sub()
{
	Sq::Query  *query;
	char       *sql;
	const char *result;

	query = new Sq::Query();

	// --- test 1 ---
	result = "SELECT * FROM ( SELECT * FROM users WHERE id > 50 )";

	query->table([](SqQuery &query) {
		query->from("users")
		     ->where("id", ">", 50);
	});
	puts(query->c());
	assert(strcmp(query->last(), result) == 0);
	query->clear();

	// --- test 2 ---
	result = "WHERE name = ( SELECT name FROM users WHERE id = 210 )";

	sql = Sq::Where("name", [](SqQuery &query) {
		query->select("name")
		     ->from("users")
		     ->where("id", "=", 210);
	}).toSql();
	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	// --- test 3 ---
	result = "JOIN city ON ( city.id = companies.city_id )";

	query->join("city", [](SqQuery &query) {
		query->on("city.id", "=", "companies.city_id");
	});
	puts(query->c());
	assert(strcmp(query->last(), result) == 0);
	query->clear();

	// --- test 4 ---
	result = "JOIN city ON city.id = ( SELECT city_id FROM companies )";

	query->join("city", "city.id", "=", [](SqQuery &query) {
		query->select("city_id")
		     ->from("companies");
	});
	puts(query->c());
	assert(strcmp(query->last(), result) == 0);
	query->clear();

	// release memory
	delete query;
}

void test_query()
{
	test_query_cpp();
	test_query_cpp_convenient_class();
	test_query_cpp_sub();
}

// ----------------------------------------------------------------------------
// Sqxc

void test_sqxc(void)
{
	Sq::XcEmpty  *xc;
	Sq::XcEmpty  *xc2;
	Sq::XcEmpty  *xc3;
	Sq::XcMethod *cur;

	xc  = new Sq::XcEmpty();
	xc2 = new Sq::XcEmpty();
	xc3 = new Sq::XcEmpty();

	xc->insert(xc2, -1);
	xc->insert(xc3, -1);

	xc->not_matched_type = SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY;
	xc->tag = "L1";

	xc2->not_matched_type = SQXC_TYPE_OBJECT;
	xc2->tag = "L2";

	xc3->not_matched_type = 0;
	xc3->tag = "L3";

	xc->ready();
	cur = xc;
	cur = cur->sendBool("is_cpp", 1);
	cur = cur->sendString("name", "Lee");
	cur = cur->sendArrayBeg("Cities");
	cur = cur->sendObjectBeg("User");
	cur = cur->sendInt("id", 2);
	cur = cur->sendInt("company_id", 3);
	cur = cur->sendObjectEnd("User");
	cur = cur->sendArrayEnd("Cities");
	xc->finish();

	xc->freeChain();
}

// ----------------------------------------------------------------------------
// Storage

void test_storage()
{
	Sq::DbEmpty *db = new Sq::DbEmpty();
	Sq::Storage *storage = new Sq::Storage(db);

	storage->insert<Company>(NULL);
	storage->get<Company>(1);
}

// ----------------------------------------------------------------------------
void test_type()
{
	Sq::Type      *type;
	Sq::Entry     *entry;
	Sq::Entry    **entry_addr;
	unsigned int   type_size;

	type = new Sq::Type;
	type->initSelf(8, sq_entry_free);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STR);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STR);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	type->addEntry(entry);

	// type size before remove entry "email"
	type_size = type->size;

	entry_addr = type->findEntry("email");
	type->stealEntry(entry_addr);
	type->decideSize(*entry_addr, true);

	// add entry "email" again
	type->addEntry(entry);
	type->decideSize();
	printf("type instance size = %d\n", type->size);

	assert(type->size == type_size);
	delete type;
}
// ----------------------------------------------------------------------------

int main(void)
{
	test_schema();
	test_query();
	test_sqxc();
	test_storage();
	test_type();
	return EXIT_SUCCESS;
}
