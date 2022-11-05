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
	const char *result;

	// --- test 1 ---
	result = "SELECT DISTINCT id, age "
	         "FROM companies AS a "
	         "JOIN city "
	             "AS c "
	             "ON city.id < 100 AND city.age > 10 "
	         "WHERE name > 'Ge' OR id < 9 "
	         "GROUP BY companies.age "
	         "ORDER BY companies.id ASC";

	// "SELECT DISTINCT id, age"
	sq_query_select(query, "id", "age");
	sq_query_distinct(query);

	// "FROM companies AS a"
	sq_query_from(query, "companies");
	sq_query_as(query, "a");

	// "WHERE name > 'Ge' OR id < 9"
	sq_query_where_raw(query, "name > '%s'", "Ge");
	sq_query_or_where(query, "id", "<", "%d", 9);

	// "JOIN city AS c ON city.id < 100 AND city.age > 10"
	sq_query_join(query, "city", "city.id", "<", "%d", 100);
	sq_query_as(query, "c");
	sq_query_on(query, "city.age", ">", "%d", 10);

	// "GROUP BY companies.age"
	sq_query_group_by(query, "companies.age");

	// "ORDER BY companies.id ASC"
	sq_query_order_by(query, "companies.id");
	sq_query_asc(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	// --- test 2 ---
	result = "WHERE city_id > 10 "
	            "OR column = 'strHas%sign' "
	            "OR name = 'strHas%sign' "
	            "OR addr = 'strHas%sign'";

	sq_query_where(query,    "city_id", ">", "10");
	// special case: 3rd or 4th argument is raw string
	sq_query_or_where(query, "column",         "'strHas%sign'");
	sq_query_or_where(query, "name",   "'%s'",  "strHas%sign");
	sq_query_or_where(query, "addr",   "=",    "'strHas%sign'");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_where_not(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * "
	         "FROM users "
	         "WHERE NOT ( "
	         "votes > 100 OR name IN ('Ray','Zyx') "
	         ")";

	// SELECT * FROM users
	sq_query_from(query, "users");
	// WHERE NOT (
	sq_query_where_not_sub(query);
		// votes > 100
		sq_query_where(query, "votes", ">", "%d", 100);
		// OR name IN ('Ray', 'Zyx')
		sq_query_or_where_in(query, "name", 2, "'%s'", "Ray", "Zyx");
	// )
	sq_query_end_sub(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_where_like(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * "
	         "FROM users "
	         "WHERE NOT name LIKE 'Bt%'";

	// SELECT * FROM users
	sq_query_from(query, "users");
	// WHERE NOT name LIKE 'Bt%'
	sq_query_where_not(query, "name", "LIKE", "'%s'", "Bt%");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_where_between(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * "
	         "FROM users "
	         "WHERE votes NOT BETWEEN 1 AND 100 "
	             "OR name NOT BETWEEN 'Ra' AND 'Zyx'";

	// SELECT * FROM users
	sq_query_from(query, "users");
	// WHERE votes NOT BETWEEN 1 AND 100
	sq_query_where_not_between(query, "votes", "%d", 1, 100);
	// OR name NOT BETWEEN 'Ra' AND 'Zyx'
	sq_query_or_where_not_between(query, "name", "'%s'", "Ra", "Zyx");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_where_in(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * "
	         "FROM users "
	         "WHERE votes NOT IN (1,3,5) "
	             "OR name NOT IN ('Alex','Ray','Zyx')";

	// SELECT * FROM users
	sq_query_from(query, "users");
	// WHERE votes IN (1,3,5)
	sq_query_where_not_in(query, "votes", 0, "%d", 1, 3, 5);
	// OR name NOT IN ('Alex','Ray','Zyx')
	sq_query_or_where_not_in(query, "name", 3, "'%s'", "Alex", "Ray", "Zyx");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_where_null(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * "
	         "FROM users "
	         "WHERE votes IS NOT NULL "
	             "OR name IS NOT NULL";

	// SELECT * FROM users
	sq_query_from(query, "users");
	// WHERE votes IN (1,3,5)
	sq_query_where_not_null(query, "votes");
	// OR name NOT IN ('Alex','Ray','Zyx')
	sq_query_or_where_not_null(query, "name");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_raw(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT id, name "
	         "FROM users "
	         "WHERE city LIKE 'ber%'";

	sq_query_table(query, "users");
	sq_query_select_raw(query, "id, name");
	sq_query_where_raw(query, "city LIKE 'ber%'");

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_raw_statement(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * FROM users "
	         "WHERE city LIKE 'ber%' LIMIT 10 OFFSET 5";

	// "SELECT * FROM users"
	sq_query_table(query, "users");

	sq_query_raw(query, "WHERE city LIKE 'ber%'");
	sq_query_printf(query, "LIMIT %d OFFSET %d", 10, 5);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_brackets(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * FROM companies "
	         "WHERE salary > 2150 AND ( id > 22 AND age < 10 )";

	// SELECT * FROM companies
	// WHERE salary > 2150
	sq_query_from(query, "companies");
	sq_query_where_raw(query, "salary > %d", 2150);

	// AND ( id > 22 AND age < 10 )
	sq_query_where_sub(query);                  // start of brackets
		sq_query_where(query, "id",  ">", "%d", 22);
		sq_query_where(query, "age", "<", "%d", 10);
	sq_query_end_sub(query);                    // end of brackets

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_subquery(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "WHERE price < ( SELECT amount FROM incomes )";

	sq_query_where_sub(query, "price", "<");    // start of subquery
		sq_query_select(query, "amount");
		sq_query_from(query, "incomes");
	sq_query_end_sub(query);                    // end of subquery

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_join(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * FROM users "
	         "JOIN contacts "
	             "ON users.id = contacts.user_id AND users.id > 120";

	sq_query_from(query, "users");
	sq_query_join(query, "contacts", "users.id", "contacts.user_id");
	sq_query_on(query, "users.id", ">", "%d", 120);
//	sq_query_or_on(query, "contacts.user_id", "<", "%d", 88);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);

	result = "SELECT * FROM users "
	         "JOIN ( SELECT * FROM contacts ) "
	             "ON users.id = contacts.user_id AND ( users.id > 120 OR contacts.id > 90 )";

	sq_query_from(query, "users");
	sq_query_join_sub(query);
		sq_query_from(query, "contacts");
	sq_query_end_sub(query);
	sq_query_on(query, "users.id", "=", "contacts.user_id");
	sq_query_on_sub(query);
		sq_query_on(query, "users.id", ">", "%d", 120);
		sq_query_or_on(query, "contacts.id", ">", "%d", 90);
	sq_query_end_sub(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_union(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT name "
	         "FROM product1 "
	         "UNION "
	         "SELECT name FROM product2";

	sq_query_select(query, "name");
	sq_query_from(query, "product1");

	sq_query_union(query);                   // start of query
		sq_query_select(query, "name");
		sq_query_from(query, "product2");
	sq_query_end_sub(query);                 // end of query

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_str(SqQuery *query)
{
	const char *result;

	result = "SELECT * FROM users";

	sq_query_from(query, "users");

	sq_query_c(query);

	puts(sq_query_c(query));
	assert(strcmp(query->str, result) == 0);

	sq_query_clear(query);
}

void test_query_c_no_select_from(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "WHERE id > 10 AND id < 99 "
	         "GROUP BY age "
	         "HAVING city_id > 3 OR city_id < 9";

	sq_query_where_raw(query, "id > 10");
	sq_query_where(query, "id", "<", "%d", 99);
	sq_query_group_by(query, "age");
	sq_query_having_raw(query, "city_id > 3");
	sq_query_or_having(query, "city_id", "<", "%d", 9);
	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_delete(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "DELETE FROM users "
	         "WHERE id > 10 AND id < 99";

	sq_query_where_raw(query, "id > 10");
	sq_query_where(query, "id", "<", "%d", 99);
	sq_query_table(query, "users");
	sq_query_delete(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_truncate(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "TRUNCATE TABLE users";

	sq_query_table(query, "users");
	sq_query_truncate(query);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

void test_query_c_limit(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT * FROM users LIMIT 110 OFFSET 55";

	sq_query_table(query, "users");
//	sq_query_limit(query, 10);
	sq_query_offset(query, 5);

	// overwrite LIMIT and OFFSET
	sq_query_limit(query, 110);
	sq_query_offset(query, 55);

	sql = sq_query_to_sql(query);
	sq_query_clear(query);

	puts(sql);
	assert(strcmp(sql, result) == 0);
	free(sql);
}

// SqQuery-macro.h
void test_query_macro_get_table_as(SqQuery *query)
{
	char       *sql;
	const char *result;

	result = "SELECT id, age "
	         "FROM companies "
	         "JOIN ( SELECT * FROM city WHERE id < 100 ) "
	             "AS c "
	             "ON c.id = companies.city_id "
	         "WHERE age > 5";

	SQ_QUERY_DO(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "%d", 100);
		});
		SQQ_AS("c");
		SQQ_ON_RAW("c.id = companies.city_id");
		SQQ_WHERE_RAW("age > 5");
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
	assert(strcmp(sql, result) == 0);
	free(sql);
}

// ----------------------------------------------------------------------------



int main(int argc, char **argv)
{
	SqQuery *query;

	query = sq_query_new(NULL);

	test_query_c(query);
	test_query_c_where_not(query);
	test_query_c_where_like(query);
	test_query_c_where_between(query);
	test_query_c_where_in(query);
	test_query_c_where_null(query);
	test_query_c_raw(query);
	test_query_c_raw_statement(query);
	test_query_c_brackets(query);
	test_query_c_subquery(query);
	test_query_c_join(query);
	test_query_c_union(query);
	test_query_c_str(query);
	test_query_c_no_select_from(query);
	test_query_c_delete(query);
	test_query_c_truncate(query);
	test_query_c_limit(query);
	test_query_macro_get_table_as(query);

	sq_query_free(query);

	return EXIT_SUCCESS;
}
