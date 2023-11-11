[中文](SqQuery.cn.md)

# SqQuery

SqQuery is query builder that supports subquery and brackets.  
  
**SqQuery is designed to work independently.**  
You can use SqQuery alone if you copy SqQuery.h, SqQuery.c, and SqQuery-proxy.h to other software projects and set SQ_QUERY_USE_ALONE to 1 in SqQuery.h header file.
It just removes the sq_query_get_table_as_names() and sq_query_select_table_as() declarations and functions in SqQuery.

## create query and generate SQL statement

e.g. generate below SQL statement. It select all columns from a database table "companies".

```sql
SELECT * FROM companies
```

SqQuery provide sq_query_to_sql() and sq_query_c() to generate SQL statement.
* The result of sq_query_to_sql() must free when you don't need it.
* You can NOT free the result of sq_query_c(), it managed by SqQuery.
* After calling sq_query_c(), user can use sq_query_last() to reuse generated SQL statement.

Note: If user doesn't specify column by select(), it select all columns from a database table by default.  
  
use C language

```c
	SqQuery *query;
	char    *sql;

	// create query and specify database table "companies"
	query = sq_query_new("companies");

	// generate SQL statement
	sql = sq_query_to_sql(query);

	// free 'sql' when you don't use it
	free(sql);
```

use C++ language

```c++
	Sq::Query *query;
	char      *sql;

	// create query and specify database table "companies"
	query = new Sq::Query("companies");

	// generate SQL statement
	sql = query->toSql();

	// free 'sql' when you don't use it.
	free(sql);
```

## clear and reuse query instance

After calling sq_query_clear(), user can generate new SQL statement in existed instance.  
  
use C language

```c
	// reset SqQuery (remove all statements)
	sq_query_clear(query);

	// select columns from a database table "users"
	sq_query_table(query, "users");

	// generate new SQL statement
	sql = sq_query_c(query);

	// After calling sq_query_c(), user can use sq_query_last() to reuse generated SQL statement.
	sql = sq_query_last(query);
```

use C++ language

```c++
	// reset Sq::Query (remove all statements)
	query->clear();

	// select columns from a database table "users"
	query->table("users");

	// generate new SQL statement
	sql = query->c();

	// After calling Sq::Query::c(), user can use last() to reuse generated SQL statement.
	sql = query->last();
```

## SQL Statements

#### from / table

from() and table() can specify database table. They do the same thing and support subquery, other details are explained in the titled "Subquery and Brackets".  
  
use C language

```c
	// select columns from a database table "users"
	// SELECT * FROM users
	sq_query_from(query, "users");

	// reset Sq::Query (remove all statements)
	sq_query_clear(query);

	// subquery
	// SELECT * FROM ( SELECT * FROM companies WHERE id < 65 )
//	sq_query_from_sub(query);
	sq_query_from(query, NULL);
		sq_query_from(query, "companies");
		sq_query_where_raw(query, "id < 65");
	sq_query_end_sub(query);
```

use C++ language

```c++
	// select columns from a database table "users"
	// SELECT * FROM users
	query->from("users");

	// reset Sq::Query (remove all statements)
	query->clear();

	// subquery
	// SELECT * FROM ( SELECT * FROM companies WHERE id < 65 )
	query->from([query] {
		query->from("companies")
		     ->whereRaw("id < 65");
	});
```

**convenient C++ class 'from'**  
  
use C++ Sq::From (or lower case Sq::from) to generate SQL statement. It can use with query method of [SqStorage](SqStorage.md).

```c++
	char *sql = Sq::from("users").where("id", "<", 10).toSql();

	// use with query method of SqStorage
	array = storage->query(Sq::from("users").whereRaw("city_id > 5"));
```

#### select

You can specify columns for the query by using select method.  
sq_query_select() can specify multiple columns in argument.  
  
use C language

```c
	sq_query_select(query, "id", "name");
	sq_query_select(query, "email");

	// The sq_query_distinct() allows you to force the query to return distinct results
	sq_query_distinct(query);
```

use C++ language

```c++
	query->select("id", "name");
	query->select("email");

	// The distinct method allows you to force the query to return distinct results
	query->distinct();
```

**convenient C++ class 'select'**  
  
use C++ Sq::Select (or lower case Sq::select) to generate SQL statement. It can use with query method of [SqStorage](SqStorage.md).

```c++
	char *sql = Sq::select("id", "name").from("users").where("id", "<", 10).toSql();

	// use with query method of SqStorage
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));
```

#### where / whereNot / orWhere / orWhereNot

These functions/methods are used to filter the results and apply conditions.

* The order of arguments are - name of column, operator, printf format string, and values that depending on the format string.
* If user doesn't specify the values following format string, program handle printf format string as raw string.
* Not recommended: If the argument of operator is =, it can be omitted (like Laravel, but less readable).
* Deprecated: If name of column has % character, It handle as printf format string (This will NOT support in future).
* The usage of condition arguments is basically the same in where(), join(), on(), and having() series functions.

e.g. generate below SQL statement.

```sql
SELECT * FROM companies
WHERE id > 15 OR city_id = 6 OR name LIKE '%Motor'
```

use C language

```c
	// SELECT * FROM companies
	sq_query_table(query, "companies");
	// WHERE id > 15
	sq_query_where(query, "id", ">", "%d", 15);
	// OR city_id = 6
	sq_query_or_where(query, "city_id", "=", "%d", 6);

	// OR name LIKE '%Motor'
	sq_query_or_where(query, "name", "LIKE", "'%Motor'");
	// Program handle "'%Motor'" as raw string here.
	// If user doesn't specify the values following format string, program handle printf format string as raw string.
```

use C++ language  
  
C++ methods where() series have overloaded functions to omit printf format string:  

```c++
	// SELECT * FROM companies
	query->table("companies")
	     // WHERE id > 15
	     ->where("id", ">", 15)
	     // OR city_id = 6
	     ->orWhere("city_id", 6);

	// OR name LIKE '%Motor'
	query->orWhere("name", "LIKE", "'%Motor'");
```

These methods can also be used to specify a group of query conditions and subquery, other details are explained in the titled "Subquery and Brackets".  
  
use C language

```c
	// SELECT * FROM products
	sq_query_table(query, "products");

	// WHERE NOT ( city_id = 6 OR price < 100 )
	sq_query_where_not_sub(query);      // start of brackets
//	sq_query_where_not(query, NULL);    // start of brackets
		sq_query_where(query, "city_id", "=", "%d", 6);
		sq_query_or_where_raw(query, "price < %d", 100);
	sq_query_end_sub(query);            // end of brackets
```

use C++ language

```c++
	// SELECT * FROM products
	query->table("products")
	     // WHERE NOT ( city_id = 6 OR price < 100 )
	     ->whereNot([query] {
	         query->where("city_id", 6)
	              ->orWhereRaw("price < %d", 100);
	     });
```

#### whereBetween / orWhereBetween

The whereBetween method verifies that a column's value is between two values.
* These method specify printf format string once, use twice.
  
use C language

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// WHERE votes BETWEEN 1 AND 100
	sq_query_where_between(query, "votes", "%d", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_between(query, "name", "'%s'", "Ray", "Zyx");
```

use C++ language
  
C++ methods whereBetween() series have overloaded functions to omit printf format string:

```c++
	// SELECT * FROM users
	query->table("users")
	     // WHERE votes BETWEEN 1 AND 100
	     ->whereBetween("votes", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	query->orWhereBetween("name", "Ray", "Zyx");
```

#### whereNotBetween / orWhereNotBetween

The whereNotBetween method verifies that a column's value lies outside of two values.  
  
use C language

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// WHERE votes NOT BETWEEN 1 AND 100
	sq_query_where_not_between(query, "votes", "%d", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_not_between(query, "name", "'%s'", "Ray", "Zyx");
```

use C++ language

```c++
	// SELECT * FROM users
	query->table("users")
	     // WHERE votes NOT BETWEEN 1 AND 100
	     ->whereNotBetween("votes", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	query->orWhereNotBetween("name", "Ray", "Zyx");
```

#### whereIn / whereNotIn / orWhereIn / orWhereNotIn

* These method specify printf format string once, use multiple times.
  
use C language  
  
sq_query_where_in() must use with printf format string:
* The 3rd parameter is number of values after printf format string.
* If user specify 3rd parameter to 0, it will count number of arguments by macro.

```c
	// SELECT * FROM users WHERE id IN (1,2,4)
	sq_query_table(query, "users");
	sq_query_where_in(query, "id", 3, "%d", 1, 2, 4);
```

use C++ language
  
C++ methods whereIn() series have overloaded functions to omit printf format string:

```c++
	// SELECT * FROM users WHERE id IN (1,2,4)
	query->table("users")
	     ->whereIn("id", 1, 2, 4);
```

When you use whereIn() with printf format string:
* The second parameter is number of values after printf format string.
* If user specify second parameter to 0, it will count by compiler.

```c++
	// SELECT * FROM users WHERE id IN ('Ray','Alex','Xyz')
	query->table("users")
	     ->whereIn("id", 0, "'%s'", "Ray", "Alex", "Xyz");
```

#### whereNull / whereNotNull / orWhereNull / orWhereNotNull

These methods are used to specify a SQL condition "IS NULL" or "IS NOT NULL".  
  
use C language

```c
	// SELECT * FROM users WHERE updated_at IS NULL
	sq_query_table(query, "users");
	sq_query_where_null(query, "updated_at");
```

use C++ language

```c++
	// SELECT * FROM users WHERE updated_at IS NOT NULL
	query->table("users")
	     ->whereNotNull("updated_at");
```

#### having / orHaving

The usage of having() series is similar to the where().  
  
use C language

```c
	sq_query_table(query, "companies");
	sq_query_group_by(query, "city_id");
	sq_query_having(query, "age", ">", "%d", 10);
	sq_query_or_having_raw(query, "members < %d", 50);
```

use C++ language

```c++
	query->table("companies")
	     ->groupBy("city_id")
	     ->having("age", ">", 10)
	     ->orHavingRaw("members < %d", 50);
```

**Examples of brackets for having() series:**  
  
Other details are explained in the titled "Subquery and Brackets".  
  
use C language

```c
	// ... HAVING ( salary > 45 OR age < 21 )
	sq_query_having_sub(query);                 // start of brackets
//	sq_query_having(query, NULL);               // start of brackets
		sq_query_having(query, "salary", ">", "%d", 45);
		sq_query_or_having(query, "age", "<", "%d", 21);
	sq_query_end_sub(query);                    // end of brackets
```

use C++ language

```c++
	// ... HAVING ( salary > 45 OR age < 21 )
	query->having([query] {
		query->having("salary", ">", 45);
		query->orHaving("age", "<", 21);
	});
```

#### groupBy / orderBy

sq_query_order_by() and sq_query_group_by() can specify multiple columns in argument.  
  
use C language

```c
	// "GROUP BY companies.age, companies.name"
	sq_query_group_by(query, "companies.age", "companies.name");

	// "ORDER BY companies.id DESC"
	sq_query_order_by(query, "companies.id");
	sq_query_desc(query);
```

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

#### limit / offset

the limit() methods can limit the number of results returned from the query, or use offset() method to skip a given number of results in the query.  
User can call limit() and offset() multiple times. Arguments specified on the last call will override those specified on the previous call.  
  
use C language

```c
	sq_query_table("users");

	// specifiy LIMIT 3 OFFSET 6
	sq_query_offset(6);
	sq_query_limit(3);

	// override arguments on the previous call.
	sq_query_offset(10);
	sq_query_limit(5);

	// The final result of SQL staement:
	// SELECT * FROM users LIMIT 5 OFFSET 10
```

use C++ language

```c++
	query->table("users");

	// specifiy LIMIT 3 OFFSET 6
	query->offset(6)->limit(3);

	// override arguments on the previous call.
	query->offset(10)->limit(5);

	// The final result of SQL staement:
	// SELECT * FROM users LIMIT 5 OFFSET 10
```

#### deleteFrom / truncate

call these function at last (before generating SQL statement).  
  
e.g. generate below SQL statement.

```sql
DELETE FROM companies
```

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

e.g. generate below SQL statement.

```sql
TRUNCATE TABLE companies
```

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

## SQL statement that exclude "SELECT * FROM ..."

If you don't specify table name and column name in SqQuery, it will generate SQL statement that exclude "SELECT * FROM ...".  
The 'SQL statement' parameter in sq_storage_get_all(), sq_storage_update_all(), and sq_storage_remove_all() can use this.  
  
use C language

```c
	sq_query_clear(query);
	// WHERE id > 10 OR city_id < 9
	sq_query_where_raw(query, "id > 10");
	sq_query_or_where_raw(query, "city_id < 9");

	// use sq_query_c() to generate SQL statement
	array = sq_storage_remove_all(storage, "users",
	                              sq_query_c(query));

	// or use sq_query_to_sql() to generate SQL statement
	sql_where = sq_query_to_sql(query);
	array = sq_storage_remove_all(storage, "users",
	                              sql_where);
	free(sql_where);
```

use C++ language

```c++
	query->clear();
	// WHERE id > 10 OR city_id < 9
	query->whereRaw("id > 10");
	query->orWhereRaw("city_id < 9");

	// use Sq::Query::c() to generate SQL statement
	array = storage->removeAll("users", query->c());

	// or use Sq::Query::toSql() to generate SQL statement
	sql_where = query->toSql();
	array = storage->removeAll("users", sql_where);
	free(sql_where);
```

#### convenient C++ class 'where' series

use C++ Sq::Where and Sq::WhereRaw (or Sq::where and Sq::whereRaw) to generate SQL statement  
  
1. use operator() of Sq::Where (or Sq::where)

```c++
	Sq::Where  where;

	array = storage->removeAll("users",
			where("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

2. use parameter pack constructor

```c++
	array = storage->removeAll("users",
			Sq::where("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

3. use default constructor and operator()

```c++
	array = storage->removeAll("users",
			Sq::where()("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

4. use lambda function to generate subquery and brackets

```c++
	// ... WHERE ( id < 11 OR city_id < 33 )
	Sq::where([](SqQuery &query) {
		query.where("id", "<", 11).orWhereRaw("city_id < %d", 33);
	});

	// ... WHERE price < ( SELECT AVG(amount) FROM incomes )
	Sq::where("price", "<", [](SqQuery &query) {
		query.selectRaw("AVG(amount)").from("incomes");
	});
```

5. Below is currently provided convenient C++ class:

```
	Sq::Where,        Sq::WhereNot,
	Sq::WhereRaw,     Sq::WhereNotRaw,
	Sq::WhereExists,  Sq::WhereNotExists,
	Sq::WhereBetween, Sq::WhereNotBetween,
	Sq::WhereIn,      Sq::WhereNotIn,
	Sq::WhereNull,    Sq::WhereNotNull,

	'Where' class series use 'typedef' to give them new names: lower case 'where' class series.

	Sq::where,        Sq::whereNot,
	Sq::whereRaw,     Sq::whereNotRaw,
	Sq::whereExists,  Sq::whereNotExists,
	Sq::whereBetween, Sq::whereNotBetween,
	Sq::whereIn,      Sq::whereNotIn,
	Sq::whereNull,    Sq::whereNotNull,
```

## Raw Methods

insert a raw expression into various parts of your query.  

#### selectRaw

use C language

```c
	sq_query_table(query, "users");
	sq_query_select_raw(query, "COUNT(column_name)");
```

use C++ language

```c++
	query->table("users")
	     ->selectRaw("COUNT(column_name)");
```

C++ method select() has overloaded function to handle raw string.  
If the 2nd argument is NOT exist, the 1st argument is handled as raw string.

```c++
	query->table("users")
	     ->select("COUNT(column_name)");
```

#### whereRaw / whereNotRaw / orWhereRaw / orWhereNotRaw

use C language
  
C function sq_query_where_raw() series use macro to count number of arguments.  
If the 3rd argument is NOT exist, the 2nd argument is handled as raw string.

```c
	// The following two sets of query results are the same.

	// 2nd argument is raw string
	sq_query_where_raw(query, "id > 100 AND id < 300");
	sq_query_where_raw(query, "name LIKE 'ber%'");

	// 2nd argument is printf format string
	sq_query_where_raw(query, "id > %d AND id < %d", 100, 300);
	sq_query_where_raw(query, "name LIKE '%s'", "ber%");
```

use C++ language
  
C++ method whereRaw()/orWhereRaw() has overloaded function to handle raw string.  
If the 2nd argument is NOT exist, the 1st argument is handled as raw string.

```c++
	// The following two sets of query results are the same.

	// 1st argument is raw string
	query->whereRaw("id > 100 AND id < 300")
	     ->whereRaw("name LIKE 'ber%'");

	// 1st argument is printf format string
	query->whereRaw("id > %d AND id < %d", 100, 300)
	     ->whereRaw("name LIKE '%s'", "ber%");
```

#### havingRaw / orHavingRaw

use C language
  
C function sq_query_having_raw() series use macro to count number of arguments.  
If the 3rd argument is NOT exist, the 2nd argument is handled as raw string.

```c
	sq_query_table(query, "orders");
	sq_query_group_by(query, "customer_id");

	// bellow 3 lines has the same result
	sq_query_having_raw(query, "SUM(price) > 3000");
//	sq_query_having_raw(query, "SUM(price) > %d", 3000);
//	sq_query_having(query, "SUM(price)", ">", "%d", 3000);
```

use C++ language
  
C++ method havingRaw()/orHavingRaw() has overloaded function to handle raw string.  
If the 2nd argument is NOT exist, the 1st argument is handled as raw string.

```c++
	query->table("orders")
	     ->groupBy("customer_id");

	// bellow 3 lines has the same result
	query->havingRaw("SUM(price) > 3000");
//	query->havingRaw("SUM(price) > %d", 3000);
//	query->having("SUM(price)", ">", "%d", 3000);
```

#### orderByRaw

use C language

```c
	sq_query_table(query, "orders");
	sq_query_order_by_raw(query, "updated_at DESC");
```

use C++ language

```c++
	query->table("orders")
	     ->orderByRaw("updated_at DESC");
```

C++ method orderBy() has overloaded function to handle raw string.  
If the 2nd argument is NOT exist, the 1st argument is handled as raw string.

```c++
	query->table("orders")
	     ->orderBy("updated_at DESC");
```

#### groupByRaw

use C language

```c
	sq_query_table(query, "companies");
	sq_query_group_by_raw(query, "city, state");
```

use C++ language

```c++
	query->table("companies")
	     ->groupByRaw("city, state");
```

C++ method groupBy() has overloaded function to handle raw string.  
If the 2nd argument is NOT exist, the 1st argument is handled as raw string.

```c++
	query->table("companies")
	     ->groupBy("city, state");
```

#### raw SQL statement

sq_query_raw() and sq_query_printf() can append raw SQL statement in current nested or subquery.  
  
e.g. generate below SQL statement.

```sql
SELECT * FROM users
WHERE city LIKE 'ber%'
LIMIT 20 OFFSET 10
```

use C language  
  
sq_query_raw() use macro to count number of arguments. If the 3rd argument is NOT exist, the 2nd argument is handled as raw string.

```c
	// "SELECT * FROM users"
	sq_query_table(query, "users");

	// Because the 3rd argument does NOT exist, the 2nd argument is raw string.
	sq_query_raw(query, "WHERE city LIKE 'ber%'");

	// Because the 3rd argument does exist, the 2nd argument is printf format string.
	sq_query_raw(query, "LIMIT %d OFFSET %d", 20, 10);
```

use C++ language  
  
C++ method raw() has overloaded function to handle raw string.

```c++
	// "SELECT * FROM users"
	query->table("users");

	// Because the 2nd argument does NOT exist, the 1st argument is raw string.
	query->raw("WHERE city LIKE 'ber%'");

	// Because the 2nd argument does exist, the 1st argument is printf format string.
	query->raw("LIMIT %d OFFSET %d", 20, 10);
```

## Joins

#### Inner Join

To generate "inner join" statement , you may use the sq_query_join() on a SqQuery instance.  
  
use C language

```c
	sq_query_table(query, "companies");
	sq_query_join(query, "city", "users.id", "=", "%s", "posts.user_id");
```

use C++ language

```c++
	query->table("companies")
	     ->join("city", "users.id", "=", "posts.user_id");
```

#### Left Join / Right Join / Full Join

use C language

```c
	sq_query_table(query, "users");
	sq_query_left_join(query,  "posts", "users.id", "=", "%s", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_right_join(query, "posts", "users.id", "=", "%s", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_full_join(query,  "posts", "users.id", "=", "%s", "posts.user_id");
```

use C++ language
  
In the following example, the conditions for the three kind of joins are the same.

```c++
	query->table("users")
	     ->leftJoin("posts",  "users.id", "posts.user_id");

	query->table("users")
	     ->rightJoin("posts", "users.id", "=", "posts.user_id");

	query->table("users")
	     ->fullJoin("posts",  "users.id", "=", "%s", "posts.user_id");
```

#### Cross Join

use C language

```c
	sq_query_table(query, "users");
	sq_query_cross_join(query, "posts");
```

use C++ language

```c++
	query->table("users")
	     ->crossJoin("posts");
```

#### join method use with on / orOn methods

The usage of on method is similar to the where method.  
  
use C language

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// JOIN posts ON users.id = posts.user_id
	sq_query_join(query, "posts", "users.id", "=", "%s", "posts.user_id");
	// AND users.id > 120
	sq_query_on_raw(query, "users.id > %d", 120);
```

use C++ language

```c++
	// SELECT * FROM users
	query->table("users")
	     // JOIN posts ON users.id = posts.user_id
	     ->join("posts", "users.id", "=", "posts.user_id")
	     // AND users.id > 120
	     ->onRaw("users.id > %d", 120);
```

#### Subquery Joins

The join() and on() series can also use subquery and brackets.  
  
e.g. below is SQL joins that has subquery.

```sql
SELECT id, age
FROM companies
JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
WHERE age > 5
```

use C language to generate subquery:  
  
* sq_query_join_sub() and sq_query_on_sub() is start of subquery.
* sq_query_end_sub()  is end of subquery.
* sq_query_join()     and sq_query_on() passing NULL in the last parameter is also the start of subquery.

```c
	sq_query_select(query, "id", "age");
	sq_query_from(query, "companies");

	sq_query_join_sub(query);                   // start of subquery
//	sq_query_join(query, NULL);                 // start of subquery
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "%d", 100);
	sq_query_end_sub(query);                    // end of subquery

	sq_query_as(query, "c");
	sq_query_on_raw(query, "c.id = companies.city_id");
	sq_query_where_raw(query, "age > %d", 5);
```

use C++ lambda functions to generate subquery:

```c++
	query->select("id", "age")
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", 100);
	     })
	     ->as("c")
	     ->onRaw("c.id = companies.city_id")
	     ->whereRaw("age > %d", 5);
```

**More examples of subquery and brackets for join() and on(): series**  
  
use C language

```c
	// ... JOIN city ON ( city.id = companies.city_id )
	sq_query_join_sub(query, "city");           // start of brackets
//	sq_query_join(query, "city", NULL);         // start of brackets
		sq_query_on(query, "city.id", "=", "%s", "companies.city_id");
	sq_query_end_sub(query);                    // end of brackets

	// ... JOIN city ON city.id = ( SELECT city_id FROM companies )
	sq_query_join_sub(query, "city", "city.id", "=");    // start of subquery
//	sq_query_join(query, "city", "city.id", "=", NULL);  // start of subquery
		sq_query_from(query, "companies");
		sq_query_select(query, "city_id");
	sq_query_end_sub(query);                             // end of subquery

	// ... ON ( city.id < 100 )
	sq_query_on_sub(query);                     // start of brackets
//	sq_query_on(query, NULL);                   // start of brackets
		sq_query_on(query, "city.id", "<", "%d", 100);
	sq_query_end_sub(query);                    // end of brackets

	// ... ON city.id < ( SELECT city_id FROM companies WHERE id = 25 )
	sq_query_on_sub(query, "city.id", "<");     // start of subquery
//	sq_query_on(query, "city.id", "<", NULL);   // start of subquery
		sq_query_from(query, "companies");
		sq_query_select(query, "city_id");
		sq_query_where(query, "id", "=", "%d", 25);
	sq_query_end_sub(query);                    // end of subquery
```

use C++ language

```c++
	// ... JOIN city ON ( city.id = companies.city_id )
	query->join("city", [query] {
		query->on("city.id", "=", "companies.city_id");
	});

	// ... JOIN city ON city.id = ( SELECT city_id FROM companies )
	query->join("city", "city.id", "=", [query] {
		query->from("companies")
		     ->select("city_id");
	});

	// ... ON ( city.id < 100 )
	query->on([query] {
		query->on("city.id", "<", 100);
	});

	// ... ON city.id < ( SELECT city_id FROM companies WHERE id = 25 )
	query->on("city.id", "<", [query] {
		query->from("companies")
		     ->select("city_id")
		     ->where("id", "=", 25);
	});
```

## Unions

"union" two or more queries together.  

```sql
SELECT name1 FROM product1
UNION
SELECT name2 FROM product2
```

User must add other query after calling sq_query_union() or sq_query_union_all(), and calling sq_query_end_sub() in end of query.  
  
use C language

```c
	sq_query_select(query, "name1");
	sq_query_from(query, "product1");

	sq_query_union(query);                   // start of query
		sq_query_select(query, "name2");
		sq_query_from(query, "product2");
	sq_query_end_sub(query);                 // end of query
```

C++ method union_() and unionAll() use lambda function to add other query.
* Because 'union' is C/C++ keyword, it must append '_' in tail of this method.

use C++ language

```c++
	query->select("name1");
	query->from("product1");

	query->union_([query] {
		query->select("name2");
		query->from("product2");
	});
```

## Subquery and Brackets

SqQuery can produce subquery or brackets. In fact, They are implemented the same way inside programs.  
  
Below C functions/macros support subquery or brackets:  
Except sq_query_where_exists() series, the last argument in these functions/macros must be NULL.

	sq_query_from(),
	sq_query_join(),
	sq_query_left_join(),
	sq_query_right_join(),
	sq_query_full_join(),
	sq_query_cross_join(),
	sq_query_on(),               sq_query_or_on(),
	sq_query_where(),            sq_query_or_where(),
	sq_query_where_not(),        sq_query_or_where_not(),
	sq_query_where_exists(),     sq_query_where_not_exists(),
	sq_query_having(),           sq_query_or_having(),
	---
	Note: You must call sq_query_end_sub() in end of subquery or brackets.

Below C convenient macros for above functions/macros:  
These C macro use variadic macro to pass NULL in the last argument.

	sq_query_from_sub(),
	sq_query_join_sub(),
	sq_query_left_join_sub(),
	sq_query_right_join_sub(),
	sq_query_full_join_sub(),
	sq_query_cross_join_sub(),
	sq_query_on_sub(),           sq_query_or_on_sub(),
	sq_query_where_sub(),        sq_query_or_where_sub(),
	sq_query_where_not_sub(),    sq_query_or_where_not_sub(),
	sq_query_having_sub(),       sq_query_or_having_sub(),
	---
	Note: You must call sq_query_end_sub() in end of subquery or brackets.

Below C++ method use lambda function to support subquery or brackets, user does NOT need to call sq_query_end_sub()  

	from(),
	join(),
	leftJoin(),
	rightJoin(),
	fullJoin(),
	crossJoin(),
	on(),          orOn(),
	where(),       orWhere(),
	whereNot(),    orWhereNot(),
	whereExists(), whereNotExists(),
	having(),      orHaving(),

#### Brackets

The usage of brackets is basically the same in where(), on(), and having() series functions.  
If you specify table and columns in brackets, the brackets become subquery.  
  
e.g. generate below SQL statement.

```sql
SELECT * FROM users
WHERE ( salary > 45 AND age < 21 ) OR id > 100
```

use C functions to generate brackets:

```c
	sq_query_table(query, "users");

	sq_query_where_sub(query);                  // start of brackets
//	sq_query_where(query, NULL);                // start of brackets
		sq_query_where(query, "salary", ">", "%d", 45);
		sq_query_where(query, "age", "<", "%d", 21);
	sq_query_end_sub(query);                    // end of brackets

	sq_query_or_where_raw(query, "id > %d", 100);
```

use C++ lambda functions to generate brackets:

```c++
	query->table("users")
	     ->where([query] {
	         query->where("salary", ">", 45)
	              ->where("age", "<", 21);
	     })
	     ->orWhereRaw("id > %d", 100);
```

#### Subquery

The usage of subquery is basically the same in where(), on(), and having() series functions.  
If you don't specify table and columns in the subquery, the subquery becomes brackets.  
  
e.g. below is SQL statement that has subquery in condition.

```sql
SELECT * FROM products
WHERE price < ( SELECT AVG(amount) FROM incomes )
```

use C language to generate subquery in condition:

```c
	// SELECT * FROM products
	sq_query_from(query, "products");

	// WHERE price < ( SELECT AVG(amount) FROM incomes )
	sq_query_where_sub(query, "price", "<");    // start of subquery
//	sq_query_where(query, "price", "<", NULL);  // start of subquery
		sq_query_select_raw(query, "AVG(amount)");
		sq_query_from(query, "incomes");
	sq_query_end_sub(query);                    // end of subquery
```

use C++ language to generate subquery in condition:

```c
	// SELECT * FROM products
	query->from("products")
	// WHERE price < ( SELECT AVG(amount) FROM incomes )
	     ->where("price", "<", [query] {
	         query->selectRaw("AVG(amount)")
	              ->from("incomes");
	     });
```

## Appendix : functions that support printf format string

There are many functions can specify SQL condition and them also support printf format string. Please pass printf format string before passing value of condition. If you want to use SQL Wildcard Characters '%' in printf format string, you must print "%" using "%%".  

Below C functions support printf format string in 2nd argument:

	sq_query_raw(),
	sq_query_printf(),
	sq_query_on_raw(),        sq_query_or_on_raw(),
	sq_query_where_raw(),     sq_query_or_where_raw(),
	sq_query_where_not_raw(), sq_query_or_where_not_raw(),
	sq_query_having_raw(),    sq_query_or_having_raw(),
	---
	These C functions use macro to count number of arguments.
	If the 3rd argument is NOT exist, the 2nd argument is handled as raw string.

Below C functions support printf format string in 4th argument:

	sq_query_on(),            sq_query_or_on(),
	sq_query_where(),         sq_query_or_where(),
	sq_query_where_not(),     sq_query_or_where_not(),
	sq_query_having(),        sq_query_or_having(),

Below C functions support printf format string in 5th argument:

	sq_query_join(),
	sq_query_left_join(),
	sq_query_right_join(),
	sq_query_full_join(),

other C functions that support printf format string:

	sq_query_where_between() series
	sq_query_where_in() series

C language example:

```c
	// --- printf format string in 4th argument ---
	// WHERE id < 100
	sq_query_where(query, "id", "<", "%d", 100);
	// AND email LIKE 'guest%'
	sq_query_where(query, "email", "LIKE", "'%s'", "guest%");

	// --- printf format string in 2nd argument ---
	// AND city  LIKE 'ber%'
	sq_query_where_raw(query, "city  LIKE '%s'", "ber%");
```

Below C++ methods support printf format string in 1st argument:

	raw(),
	printf(),
	onRaw(),       orOnRaw(),
	whereRaw(),    orWhereRaw(),
	whereNotRaw(), orWhereNotRaw(),
	havingRaw(),   orHavingRaw(),

Below C++ methods support printf format string in 3rd argument:

	on(),          orOn(),
	where(),       orWhere(),
	whereNot(),    orWhereNot(),
	having(),      orHaving(),

Below C++ methods support printf format string in 4th argument:

	join(),
	leftJoin(),
	rightJoin(),
	fullJoin(),

other C++ methods that support printf format string:

	whereBetween() series
	whereIn() series

C++ language example:

```c++
	// --- printf format string in 3rd argument ---
	// WHERE id < 100
	query->where("id", "<", "%d", 100);
	// AND email LIKE 'guest%'
	query->where("email", "LIKE", "'%s'", "guest%");

	// --- printf format string in 1st argument ---
	// AND city  LIKE 'ber%'
	query->whereRaw("city  LIKE '%s'", "ber%");
```

C++ methods have overloaded functions to omit printf format string:

```c++
	// --- omit printf format string in 3rd argument ---
	// WHERE id < 100
	query->where("id", "<", 100);
	// AND email LIKE 'guest%'
	query->where("email", "LIKE", "guest%");
```

If the 2nd argument of below C++ methods is NOT exist, the 1st argument is handled as raw string.  
These C++ methods have overloaded function to handle raw string:

	onRaw(),       orOnRaw(),
	whereRaw(),    orWhereRaw(),
	whereNotRaw(), orWhereNotRaw(),
	havingRaw(),   orHavingRaw(),
	select(),
	groupBy(),
	orderBy()

C++ language example:

```c++
	// If the 2nd argument is NOT exist, the 1st argument is handled as raw string.
	// WHERE city LIKE 'ber%'
	query->whereRaw("city LIKE 'ber%'");
```
