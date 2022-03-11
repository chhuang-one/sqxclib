# SqQuery

SqQuery is query builder that supports nested and subquery.  
It can work independently without sqxclib if you remove sq_query_get_table_as_names() in SqQuery.c.  
You can also use other query builder to replace SqQuery in sqxclib by removing SqQuery.c and SqStorage-query.c from Makefile.  

## create query and generate SQL statement

e.g. generate below SQL statement. It select all columns from a database table "companies".

```sql
SELECT * FROM companies
```

use C language

```c
	SqQuery *query;
	char    *sql;

	// create query and specify database table "companies"
	query = sq_query_new("companies");

	// generate SQL statement
	sql = sq_query_to_sql(query);
```

use C++ language

```c++
	Sq::Query *query;
	char      *sql;

	// create query and specify database table "companies"
	query = new Sq::Query("companies");

	// generate SQL statement
	sql = query->toSql();
```

Note: If user doesn't specify column by select(), it select all columns from a database table by default.

## clear and reuse query instance

After calling sq_query_clear(), user can generate new SQL statement in existed instance.  
  
use C language

```c
	// reset SqQuery and remove all statements
	sq_query_clear(query);

	// select columns from a database table "users"
	sq_query_table(query, "users");

	// generate new SQL statement
	sql = sq_query_to_sql(query);
```

use C++ language

```c++
	// reset Sq::Query and remove all statements
	query->clear();

	// select columns from a database table "users"
	query->table("users");

	// generate new SQL statement
	sql = query->toSql();
```

## SQL Statements

There are many functions support printf format string, so user need to pay attention:
* If you want to use SQL Wildcard Characters '%' in these functions, you must print “%” using “%%”.  
* If the 3rd argument of these C functions is NULL, the 2nd argument is handled as raw string.

below C functions support printf format string in 2nd argument:
	sq_query_raw(),
	sq_query_join(),
	sq_query_on(),     sq_query_or_on(),
	sq_query_where(),  sq_query_or_where(),
	sq_query_having(), sq_query_or_having(),

C language example:

```c
	sq_query_where(query, "id < %d", 100);

	// Because the 3rd argument is NULL, the 2nd argument is handled as raw string.
	sq_query_where(query, "city LIKE 'ber%'", NULL);
```

below C++ methods support printf format string in 1st argument:

	join(),
	on(),     orOn(),
	where(),  orWhere(),
	having(), orHaving(),

C++ language example:

```c++
	query->where("id < %d", 100);

	// Because the 2nd argument is NULL, the 1st argument is handled as raw string.
	query->where("city LIKE 'ber%'", NULL);
```

#### select

You can specify columns for the query by using select method.  
sq_query_select() can specify multiple columns in argument (the last argument must be NULL).  
  
use C language

```c
	// the last argument of sq_query_select() must be NULL
	sq_query_select(query, "id", "name", NULL);
	sq_query_select(query, "email", NULL);

	// The sq_query_distinct() allows you to force the query to return distinct results
	sq_query_distinct(query);
```

Because C++ method select() use parameter pack, the last argument can pass (or not) NULL.  
  
use C++ language

```c++
	query->select("id", "name");
	query->select("email");

	// The distinct method allows you to force the query to return distinct results
	query->distinct();
```

#### where / orWhere

```sql
SELECT * FROM companies WHERE id > 15 OR city_id = 6 OR members < 100
```

use C language

```c
	sq_query_table(query, "companies");
	sq_query_where(query, "id", ">", "15");
	sq_query_or_where(query, "city_id", "6");
	sq_query_or_where(query, "members < %d", 100);
```

use C++ language

```c++
	query->table("companies")
	     ->where("id", ">", "15")
	     ->orWhere("city_id", "6")
	     ->orWhere("members < %d", 100);
```

#### having / orHaving

use C language

```c
	sq_query_table(query, "companies");
	sq_query_having(query, "age", ">", "10");
	sq_query_or_having(query, "members < %d", 50);
```

use C++ language

```c++
	query->table("companies")
	     ->having("age", ">", "10");
	     ->orHaving("members < %d", 50);
```

#### groupBy / orderBy

sq_query_order_by() and sq_query_group_by() can specify multiple columns in argument (the last argument must be NULL).  
  
use C language

```c
	// "GROUP BY companies.age, companies.name"
	// the last argument of sq_query_group_by() must be NULL
	sq_query_group_by(query, "companies.age", "companies.name", NULL);

	// "ORDER BY companies.id DESC"
	// the last argument of sq_query_order_by() must be NULL
	sq_query_order_by(query, "companies.id", NULL);
	sq_query_desc(query);
```

Because C++ method orderBy() and groupBy() use parameter pack, the last argument can pass (or not) NULL.  
* The usage of orderBy() is different from Laravel.  
  
use C++ language

```c++
	// "GROUP BY companies.age, companies.name"
	query->groupBy("companies.age", "companies.name");

	// "ORDER BY companies.id DESC"
	query->orderBy("companies.id")->desc();
	// or
	query->orderByDesc("companies.id");
```

#### deleteFrom / truncate

call these function at last (before generating SQL statement).  
  
DELETE FROM

```c++
	// C functions
	sq_query_table(query, "companies");
	sq_query_delete(query);
	sql = sq_query_to_sql(query);

	// C++ methods
	query->table("companies");
	query->deleteFrom();
	sql = query->toSql();
```

TRUNCATE TABLE

```c++
	// C functions
	sq_query_table(query, "companies");
	sq_query_truncate(query);
	sql = sq_query_to_sql(query);

	// C++ methods
	query->table("companies");
	query->truncate();
	sql = query->toSql();
```

## SQL statement exclude "SELECT * FROM table_name"

If you don't specify table name and column name in SqQuery, it will generate SQL statement exclude "SELECT * FROM table_name".  
The last parameter in sq_storage_get_all() and sq_storage_remove_all() can use this.

```c++
	sq_query_where(query, "id > 10", NULL);
	sq_query_or_where(query, "city_id < 9", NULL);

	// WHERE id > 10 OR city_id < 9
	sql = sq_query_to_sql(query);
```

## Raw Methods

insert a raw expression into various parts of your query.  

* If the 3rd argument of these C functions is NULL, the 2nd argument is handled as raw string.
* C++ users don't need to care this because sqxclib provide raw methods to do these.

#### selectRaw

use C language

```c
	sq_query_table(query, "users");
	sq_query_select(query, "COUNT(column_name)", NULL);
```

use C++ language

```c++
	query->table("users")
	     ->selectRaw("COUNT(column_name)");
```

#### whereRaw / orWhereRaw

use C language

```c
	sq_query_table(query, "users");
	sq_query_where(query, "id > 100 AND id < 300", NULL);

	// Because the 3rd argument is NULL, the 2nd argument is handled as raw string.
	sq_query_where(query, "city LIKE 'ber%'", NULL);
```

use C++ language

```c++
	query->table("users")
	     ->whereRaw("id > 100 AND id < 300")
	     ->whereRaw("city LIKE 'ber%'");
```

#### havingRaw / orHavingRaw

use C language

```c
	sq_query_table(query, "orders");
	sq_query_having(query, "SUM(price) > 3000", NULL);
```

use C++ language

```c++
	query->table("orders")
	     ->havingRaw("SUM(price) > 3000");
```

#### orderByRaw

use C language

```c
	sq_query_table(query, "orders");
	sq_query_order_by(query, "updated_at DESC", NULL);
```

use C++ language

```c++
	query->table("orders")
	     ->orderByRaw("updated_at DESC");
```

#### groupByRaw

use C language

```c
	sq_query_table(query, "companies");
	sq_query_group_by(query, "city, state", NULL);
```

use C++ language

```c++
	query->table("companies")
	     ->groupByRaw("city, state");
```

#### raw SQL statement

sq_query_raw() can append raw SQL statement in current nested/subquery.  
  
e.g. generate below SQL statement.

```sql
SELECT * FROM users WHERE city LIKE 'ber%' LIMIT 20 OFFSET 10
```

use C language

```c
	// "SELECT * FROM users"
	sq_query_table(query, "users");

	// Because the 3rd argument is NULL, the 2nd argument is handled as raw string.
	sq_query_raw(query, "WHERE city LIKE 'ber%'", NULL);

	// Because the 3rd argument is NOT NULL, the 2nd argument is handled as printf format string.
	sq_query_raw(query, "LIMIT %d OFFSET %d", 20, 10);
```

use C++ language

```c++
	// "SELECT * FROM users"
	query->table("users");

	// Because the 2rd argument does not exist, the 1st argument is handled as raw string.
	query->raw("WHERE city LIKE 'ber%'");

	// Because the 2nd argument is NOT NULL, the 1st argument is handled as printf format string.
	query->raw("LIMIT %d OFFSET %d", 20, 10);
```

## Joins

use C language

```c
	sq_query_table(query, "companies");
	sq_query_join(query, "city", "city.id", "<", "100");
```

use C++ language

```c++
	query->table("companies")
	     ->join("city", "city.id", "<", "100");
```

## Unions

"union" two or more queries together.  

```sql
SELECT name1 FROM product1 UNION SELECT name2 FROM product2
```

User must add other query after calling sq_query_union() or sq_query_union_all(), and calling sq_query_pop_nested() in end of query.  
  
use C language

```c
	sq_query_select(query, "name1", NULL);
	sq_query_from(query, "product1");

	sq_query_union(query);                   // start of query
		sq_query_select(query, "name2", NULL);
		sq_query_from(query, "product2");
	sq_query_pop_nested(query);              // end of query
```

C++ method union_() and unionAll() use lambda function to add other query.  
  
use C++ language

```c++
	query->select("name1");
	query->from("product1");

	query->union_([query] {
		query->select("name2");
		query->from("product2");
	});
```

## Nested and Subquery (experimental)

* SqQuery can produce limited Nested and Subquery. You may also use Raw Methods to do these.

below C functions support Subquery/Nested:  

	sq_query_join(),
	sq_query_on(),     sq_query_or_on(),
	sq_query_where(),  sq_query_or_where(), sq_query_where_exists(),
	sq_query_having(), sq_query_or_having()

Note1: except sq_query_where_exists(), second argument of these functions must be NULL.  
Note2: you must call sq_query_pop_nested() in end of Subquery/Nested.  
  
below C++ method use lambda function to support Subquery/Nested, user don't need to call sq_query_pop_nested()  

	join(),
	on(),     orOn(),
	where(),  orWhere(),  whereExists(),
	having(), orHaving()

#### Nested

e.g. generate below SQL statement.

```sql
SELECT * FROM users WHERE (salary > 45 AND age < 21) OR id > 100
```

use C functions to generate Nested:

```c
	sq_query_table(query, "users");
	sq_query_where(query, NULL);                // start of Nested
		sq_query_where(query, "salary", ">", "45");
		sq_query_where(query, "age", "<", "21");
	sq_query_pop_nested(query);                 // end of Nested
	sq_query_or_where(query, "id > %d", 100);
```

use C++ lambda functions to generate Nested:

```c++
	query->table("users")
	     ->where([query] {
	         query->where("salary", ">", "45")
	              ->where("age", "<", "21");
	     })
	     ->orWhere("id > %d", 100);
```

#### Subquery

e.g. generate below SQL statement that has subquery.

```sql
SELECT id, age
FROM companies
JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
WHERE age > 5
```

use C++ lambda functions to generate subquery:

```c++
	query->select("id", "age")
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > 5");
```
