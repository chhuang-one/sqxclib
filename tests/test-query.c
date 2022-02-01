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

#include <SqQuery-macro.h>

// ----------------------------------------------------------------------------

void test_query_c1()
{
	SqQuery *query;
	char    *sql;
	const char* result = "SELECT DISTINCT id, age "
	                     "FROM Company AS a "
	                     "WHERE salary > '1200' OR id < 9 "
	                     "JOIN city AS c ON city.id < 100 AND city.age > 10 "
	                     "GROUP BY Company.age "
	                     "ORDER BY Company.id ASC";

	query = sq_query_new(NULL);

	// "SELECT DISTINCT id, age"
	sq_query_select(query, "id", "age", NULL);
	sq_query_distinct(query);

	// "FROM Company AS a"
	sq_query_from(query, "Company");
	sq_query_as(query, "a");

	// "WHERE salary > '1200' OR id < 9"
//	sq_query_where(query, "salary", ">", "'1200'");
//	sq_query_where(query, "%s %s '%d'", "salary", ">", 1200);
	sq_query_where(query, "salary > '%d'", 1200);
	sq_query_or_where(query, "id", "<", "9");

	// "JOIN city AS c ON city.id < 100 AND city.age > 10"
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

	assert(strcmp(sql, result) == 0);
	free(sql);
	sq_query_free(query);
}

void test_query_c2()
{
	SqQuery *query;
	char    *sql;
	const char *result = "SELECT * FROM Company WHERE salary > 2150 AND ( id > 22 AND age < 10 )";

	query = sq_query_new(NULL);
	// SELECT * FROM Company
	// WHERE salary > 2150
	sq_query_from(query, "Company");
	sq_query_where(query, "salary > %d", 2150);

	// AND ( id > 22 AND age < 10 )
	sq_query_where(query, NULL);             // start of Subquery/Nested
		sq_query_where(query, "id",  ">", "22");
		sq_query_where(query, "age", "<", "10");
	sq_query_pop_nested(query);              // end of Subquery/Nested

	sql = sq_query_to_sql(query);
	puts(sql);

	assert(strcmp(sql, result) == 0);
	free(sql);
	sq_query_free(query);
}

void test_query_macro()
{
	SqQuery *query;
	char    *sql;

	query = sq_query_new(NULL);

	/*
		SELECT id, age
		FROM companies
		JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
		WHERE age > 5
	 */
	SQ_QUERY_DO(query, {
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

	// get table name and it's as name in query.
	SqPtrArray *table_as = sq_ptr_array_new(4, NULL);
	sq_query_get_table_as_names(query, table_as);
	for (int i = 0;  i < table_as->length;  i += 2)
		printf("%s - %s\n", (char*)table_as->data[i], (char*)table_as->data[i+1]);
	sq_ptr_array_free(table_as);

	sq_query_free(query);
}

void test_query()
{
	test_query_c1();
	test_query_c2();
	test_query_macro();
}

// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	test_query();
	return EXIT_SUCCESS;
}
