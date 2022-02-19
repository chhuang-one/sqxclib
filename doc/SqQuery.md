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

	// reset SqQuery and remove all statements
	sq_query_clear(query);
```

use C++ Language

```c++
	Sq::Query *query;
	char      *sql;

	// create query and specify database table "companies"
	query = new Sq::Query("companies");

	// generate SQL statement
	sql = query->toSql();

	// reset Sq::Query and remove all statements
	query->clear();
```

Note: If user doesn't specify column, it select all columns from a database table by default.

## SQL Statements

There are many functions support printf format string. If you want to use SQL Wildcard Characters '%' in these functions, you must print “%” using “%%”.  
  
below C functions support printf format string in 2nd argument:

	sq_query_join(),
	sq_query_on(),     sq_query_or_on(),

	sq_query_where(),  sq_query_or_where(),

	sq_query_having(), sq_query_or_having(),

* If the 3rd argument of above C functions is NULL, the 2nd argument is handled as raw string.

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

* If the 2nd argument of above C++ methods is NULL, the 1st argument is handled as raw string.

```c++
	query->where("id < %d", 100);

	// Because the 2nd argument is NULL, the 1st argument is handled as raw string.
	query->where("city LIKE 'ber%'", NULL);
```

#### select

You can specify columns for the query by using select method.  

use C language

```c
	sq_query_select(query, "id", "name", NULL);

	// The sq_query_distinct() allows you to force the query to return distinct results
	sq_query_distinct(query);
```

use C++ Language

```c++
	query->select("id", "name", NULL);

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

use C++ Language

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

use C++ Language

```c++
	query->table("companies")
	     ->having("age", ">", "10");
	     ->orHaving("members < %d", 50);
```

#### groupBy / orderBy

use C language

```c
	// "GROUP BY companies.age"
	sq_query_group_by(query, "companies.age", NULL);

	// "ORDER BY companies.id ASC"
	sq_query_order_by(query, "companies.id", NULL);
	sq_query_order_by_asc(query);
```

use C++ Language

```c++
	// "GROUP BY companies.age"
	query->groupBy("companies.age", NULL);

	// "ORDER BY companies.id ASC"
	query->orderBy("companies.id", NULL)
	     ->asc();
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

use C++ Language

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

use C++ Language

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

use C++ Language

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

use C++ Language

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

use C++ Language

```c++
	query->table("companies")
	     ->groupByRaw("city, state");
```

## Joins

use C language

```c
	sq_query_table(query, "companies");
	sq_query_join(query, "city", "city.id", "<", "100");
```

use C++ Language

```c++
	query->table("companies")
	     ->join("city", "city.id", "<", "100");
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

	where(),  orWhere(),  whereExists()
	
	having(), orHaving()
  
#### Nested

e.g. generate below SQL statement.

```sql
SELECT * FROM users WHERE (salary > 45 AND age < 21) OR id > 100
```

use below C functions that support Nested/Subquery:  

	sq_query_join(),
	sq_query_on(),     sq_query_or_on(),
	sq_query_where(),  sq_query_or_where(), sq_query_where_exists(),
	sq_query_having(), sq_query_or_having()
  
Note: except sq_query_where_exists(), second argument of these functions must be NULL when you use nested/subquery.  

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
	query->select("id", "age", NULL)
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > 5");
```
