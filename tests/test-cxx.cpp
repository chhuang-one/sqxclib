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
	{SQ_TYPE_STRING,   "name",    offsetof(User, name),  0 },
	{SQ_TYPE_STRING,   "email",   offsetof(User, email), 0 },
};

static const SqColumn  *UserColumns[] = {
	&UserColumnArray[0],
	&UserColumnArray[1],
	&UserColumnArray[2],
};

void test_schema_with_designated_initializer()
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
	Sq::Query *query;
	char      *sql;
	const char *result = "SELECT DISTINCT age, name, u.id "
	                     "FROM city "
	                     "JOIN ( SELECT * FROM User WHERE name = tom ) AS u ON u.city_id = city.id "
	                     "WHERE id > 30 AND ( age < 15 AND name = 'abc' ) ORDER BY name DESC, name";

	/*
		SELECT DISTINCT age, name, u.id
		FROM city
		JOIN ( SELECT * FROM User WHERE name = tom ) AS u ON u.city_id = city.id
		WHERE id > 30 AND ( age < '15' AND name = 'abc' ) ORDER BY name DESC, name
	 */
	query = new Sq::Query();
	query->from("city")
		 ->join([query] {
			query->from("User")
			     ->where("name", "tom");
		 })
		 ->as("u")->on("u.city_id", "city.id")
		 ->where("id > %d", 30)
		 ->where([query] {
			query->where("age", "<", "15")
				 ->whereRaw("name = 'abc'");
		 })
		 ->select("age", "name", "u.id")->distinct()
		 ->orderByDesc("name")->orderBy("name");

	sql = query->toSql();
	puts(sql);

	assert(strcmp(sql, result) == 0);
	free(sql);
	delete query;
}

void test_query()
{
	test_query_cpp();
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
	Sq::Type   *type;
	Sq::Entry  *entry;
	Sq::Entry **entry_addr;

	type = new Sq::Type;
	type->initSelf(8, sq_entry_free);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STRING);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	type->addEntry(entry);

	entry_addr = type->findEntry("email");
	type->stealEntry(entry_addr);
	type->decideSize(*entry_addr, true);

	// add entry "email" again
	type->addEntry(entry);
	type->decideSize();
	printf("type instance size = %d\n", type->size);

	assert(type->size == 12);
	type->unref();
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
