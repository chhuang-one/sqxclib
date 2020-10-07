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
#include <type_traits>  // is_standard_layout<>
#include <iostream>     // cout

#include <SqSchema.h>
#include <SqSchema-macro.h>

#include <SqQuery.h>
#include <SqQuery-macro.h>

#include <SqxcEmpty.h>

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
	{SQ_TYPE_INT,    (char*)"id",      offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
	{SQ_TYPE_STRING, (char*)"name",    offsetof(User, name),  0 },
	{SQ_TYPE_STRING, (char*)"email",   offsetof(User, email), 0 },
};

static const SqColumn  *UserColumns[] = {
	&UserColumnArray[0],
	&UserColumnArray[1],
	&UserColumnArray[2],
};


void test_schema()
{
	Sq::Schema* schema;
	Sq::Table*  table;

	schema = sq_schema_new("uGetSchema");

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
	Sq::Query* query;
	char*      sql;

	/*
		SELECT DISTINCT age, name, u.id
		FROM city
		JOIN ( SELECT * FROM User WHERE name = 'tom' ) AS u ON u.city_id = city.id
		WHERE id > 30 AND ( age < '15' AND name = 'abc' )
	 */
	query = new SqQuery();
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
		 ->select("age", "name", "u.id", NULL)->distinct();

	sql = query->toSql();
	puts(sql);
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
	SqxcEmpty* xcempty;
	Sqxc* xc;
	Sqxc* xc2;
	Sqxc* xc3;
	Sqxc* cur;

	xc  = sqxc_new(SQXC_INFO_EMPTY);
	xc2 = sqxc_new(SQXC_INFO_EMPTY);
	xc3 = sqxc_new(SQXC_INFO_EMPTY);

	xc->insert(xc2, -1);
	xc->insert(xc3, -1);

	xcempty = (SqxcEmpty*)xc;
	xcempty->not_matched_type = SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY;
	xcempty->tag = "L1";

	xcempty = (SqxcEmpty*)xc2;
	xcempty->not_matched_type = SQXC_TYPE_OBJECT;
	xcempty->tag = "L2";

	xcempty = (SqxcEmpty*)xc3;
	xcempty->not_matched_type = 0;
	xcempty->tag = "L3";

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

	sqxc_free_chain(xc);
}

// ----------------------------------------------------------------------------

int main(void)
{
	test_schema();
	test_query();
	test_sqxc();
	return 0;
}
