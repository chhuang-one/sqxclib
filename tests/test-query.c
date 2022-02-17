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

void test_query_c(SqQuery *query)
{
	char       *sql;
	const char *result = "SELECT DISTINCT id, age "
	                     "FROM companies AS a "
	                     "WHERE salary > '1200' OR id < 9 "
	                     "JOIN city AS c ON city.id < 100 AND city.age > 10 "
	                     "GROUP BY companies.age "
	                     "ORDER BY companies.id ASC";

	// "SELECT DISTINCT id, age"
	sq_query_select(query, "id", "age", NULL);
	sq_query_distinct(query);

	// "FROM companies AS a"
	sq_query_from(query, "companies");
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

	// "GROUP BY companies.age"
	sq_query_group_by(query, "companies.age", NULL);

	// "ORDER BY companies.id ASC"
	sq_query_order_by(query, "companies.id", NULL);
	sq_query_order_by_asc(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, result) == 0);
	puts(sql);
	free(sql);
}

void test_query_c_raw(SqQuery *query)
{
	char       *sql;

	sq_query_table(query, "users");
	// if 3rd argument is NULL, the 2nd argument is handled as raw string.
	sq_query_select(query, "id, name", NULL);
	// if 3rd argument is NULL, the string "city LIKE 'ber%'" is handled as raw string.
	sq_query_where(query, "city LIKE 'ber%'", NULL);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, "SELECT id, name FROM users WHERE city LIKE 'ber%'") == 0);
	puts(sql);
	free(sql);
}

void test_query_c_nested(SqQuery *query)
{
	char       *sql;
	const char *result = "SELECT * FROM companies WHERE salary > 2150 AND ( id > 22 AND age < 10 )";

	// SELECT * FROM companies
	// WHERE salary > 2150
	sq_query_from(query, "companies");
	sq_query_where(query, "salary > %d", 2150);

	// AND ( id > 22 AND age < 10 )
	sq_query_where(query, NULL);             // start of Subquery/Nested
		sq_query_where(query, "id",  ">", "22");
		sq_query_where(query, "age", "<", "10");
	sq_query_pop_nested(query);              // end of Subquery/Nested

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, result) == 0);
	puts(sql);
	free(sql);
}

void test_query_c_no_select_from(SqQuery *query)
{
	char    *sql;

	sq_query_where(query, "id > 10", NULL);
	sq_query_where(query, "id < %d", 99);
	sq_query_having(query, "city_id > 3", NULL);
	sq_query_or_having(query, "city_id < 9");
	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, "WHERE id > 10 AND id < 99 HAVING city_id > 3 OR city_id < 9") == 0);
	puts(sql);
	free(sql);
}

void test_query_c_delete(SqQuery *query)
{
	char    *sql;

	sq_query_where(query, "id > 10", NULL);
	sq_query_where(query, "id < %d", 99);
	sq_query_table(query, "users");
	sq_query_delete(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, "DELETE FROM users WHERE id > 10 AND id < 99") == 0);
	puts(sql);
	free(sql);
}

void test_query_c_truncate(SqQuery *query)
{
	char    *sql;

	sq_query_table(query, "users");
	sq_query_truncate(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	assert(strcmp(sql, "TRUNCATE TABLE users") == 0);
	puts(sql);
	free(sql);
}

// SqQuery-macro.h
void test_query_macro_get_table_as(SqQuery *query)
{
	char    *sql;

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

	// get table name and it's as name in query.
	SqPtrArray *table_as = sq_ptr_array_new(4, NULL);
	sq_query_get_table_as_names(query, table_as);
	for (int i = 0;  i < table_as->length;  i += 2)
		printf("%s - %s\n", (char*)table_as->data[i], (char*)table_as->data[i+1]);

	assert(strcmp(table_as->data[0], "companies") == 0);
	assert(strcmp(table_as->data[2], "city") == 0);
	assert(strcmp(table_as->data[3], "c") == 0);
	sq_ptr_array_free(table_as);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	free(sql);
}

// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	SqQuery *query;

	query = sq_query_new(NULL);

	test_query_c(query);
	test_query_c_raw(query);
	test_query_c_nested(query);
	test_query_c_no_select_from(query);
	test_query_c_delete(query);
	test_query_c_truncate(query);
	test_query_macro_get_table_as(query);

	sq_query_free(query);

	return EXIT_SUCCESS;
}
