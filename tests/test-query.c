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

#include <SqQuery-macro.h>

// ----------------------------------------------------------------------------

void test_query_c1()
{
	SqQuery* query;
	char*    sql;

	query = sq_query_new(NULL);

	// "SELECT DISTINCT id, age"
	sq_query_select(query, "id", "age", NULL);
	sq_query_distinct(query);

	// "FROM Company AS a"
	sq_query_from(query, "Company");
	sq_query_as(query, "a");

	// "WHERE salary > '1200' OR id < 9"
//	sq_query_where(query, "salary", ">", "1200");
//	sq_query_where(query, "%s %s '%d'", "salary", ">", 1200);
	sq_query_where(query, "salary > '%d'", 1200);
	sq_query_or_where(query, "id", "<", "9");

	// "JOIN city AS c ON city.id < '100' AND city.age > '10'"
	sq_query_join(query, "city", "city.id", "<", "100");
	sq_query_as(query, "c");
	sq_query_on(query, "city.age", ">", "10");

	// "GROUP BY Company.age"
	sq_query_group_by(query, "Company.age", NULL);

	// "ORDER BY Company.id ASC"
	sq_query_order_by(query, "Company.id", NULL);
	sq_query_order_by_asc(query);

	sql = sq_query_to_sql(query);
	puts(sql);
	free(sql);

	sq_query_free(query);
}

void test_query_c2()
{
	SqQuery* query;
	char*    sql;

	query = sq_query_new(NULL);
	// SELECT * FROM Company
	// WHERE salary > 2150
	sq_query_from(query, "Company");
	sq_query_where(query, "salary > %d", 2150);

	// AND (id > '22' AND age < '10')
	sq_query_where(query, NULL);             // start of Subquery/Nested
		sq_query_where(query, "id",  ">", "22");
		sq_query_where(query, "age", "<", "10");
	sq_query_pop_nested(query);              // end of Subquery/Nested

	sql = sq_query_to_sql(query);
	puts(sql);
	free(sql);

	sq_query_free(query);
}

void test_query_macro()
{
	SqQuery* query;
	char*    sql;

	query = sq_query_new(NULL);

	/*
		SELECT id, age
		FROM companies
		JOIN ( SELECT * FROM city WHERE id < '100' ) AS c ON c.id = companies.city_id
		WHERE age > 5
	 */
	SQ_QUERY(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "100");
		}); SQQ_AS("c"); SQQ_ON("c.id = companies.city_id");
		SQQ_WHERE("age > 5");
	});

	sql = sq_query_to_sql(query);
	puts(sql);
	free(sql);

	sq_query_free(query);
}

void test_query()
{
	test_query_c1();
	test_query_c2();
	test_query_macro();
}

// ----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	test_query();
	return 0;
}
