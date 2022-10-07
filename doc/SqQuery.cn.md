[English](SqQuery.md)

# SqQuery

SqQuery 是支持嵌套和子查询的查询构建器。  
如果在 SqQuery.c 中删除 sq_query_get_table_as_names() 和 sq_query_select_table_as()，它可以在没有 sqxclib 的情况下独立工作。  
您还可以通过从 Makefile 中删除 SqQuery.c 和 SqStorage-query.c 来使用其他查询构建器来替换 sqxclib 中的 SqQuery。

## 创建查询并生成 SQL 语句

例如: 生成下面的 SQL 语句。它从数据库表 "companies" 中选择所有列。

```sql
SELECT * FROM companies
```

SqQuery 提供 sq_query_to_sql() 和 sq_query_c() 来生成 SQL 语句。
* sq_query_to_sql() 的结果在您不需要时必须释放。
* 您不能释放 sq_query_c() 的结果，它由 SqQuery 管理。
* 调用 sq_query_c() 后，用户可以访问 SqQuery::str 以重用生成的 SQL 语句。

注意: 如果用户没有通过 select() 指定列，则默认选择数据库表中的所有列。  
  
使用 C 语言

```c
	SqQuery *query;
	char    *sql;

	// 创建查询并指定数据库表 "companies"
	query = sq_query_new("companies");

	// 生成 SQL 语句
	sql = sq_query_to_sql(query);

	// 不使用时释放 'sql'
	free(sql);
```

use C++ language

```c++
	Sq::Query *query;
	char      *sql;

	// 创建查询并指定数据库表 "companies"
	query = new Sq::Query("companies");

	// 生成 SQL 语句
	sql = query->toSql();

	// 不使用时释放 'sql'
	free(sql);
```

## 清除和重用查询实例

调用 sql_query clear() 后，用户可以在现有实例中生成新的 SQL 语句。  
  
使用 C 语言

```c
	// 重置 SqQuery (删除所有语句)
	sq_query_clear(query);

	// 从数据库表 "users" 中选择列
	sq_query_table(query, "users");

	// 生成新的 SQL 语句
	sql = sq_query_c(query);

	// 调用 sq_query_c() 后，用户可以访问 SqQuery::str 以重用生成的 SQL 语句。
	sql = query->str;
```

使用 C++ 语言

```c++
	// 重置 SqQuery (删除所有语句)
	query->clear();

	// 从数据库表 "users" 中选择列
	query->table("users");

	// 生成新的 SQL 语句
	sql = query->c();

	// 调用 sq_query_c() 后，用户可以访问 SqQuery::str 以重用生成的 SQL 语句。
	sql = query->str;
```

## SQL 语句

支持 printf 格式字符串的函数有很多。如果要在这些函数中使用 SQL 通配符 '%'，则必须使用 "%%" 打印 "%"。  
  
以下 C 函数在第二个参数中支持 printf 格式字符串：

	sq_query_printf(),
	sq_query_join(),
	sq_query_left_join(),
	sq_query_right_join(),
	sq_query_full_join(),
	sq_query_on(),        sq_query_or_on(),
	sq_query_where(),     sq_query_or_where(),
	sq_query_where_not(), sq_query_or_where_not(),
	sq_query_having(),    sq_query_or_having(),

其他支持 printf 格式字符串的 C 函数：
	sq_query_where_between() 系列
	sq_query_where_in() 系列

C 语言示例：

```c
	sq_query_where(query, "id < %d", 100);

	// output "city LIKE 'ber%'"
	sq_query_where(query, "city LIKE 'ber%%'");
```

以下 C++ 方法在第一个参数中支持 printf 格式字符串：

	join(),
	leftJoin(),
	rightJoin(),
	fullJoin(),
	on(),       orOn(),
	where(),    orWhere(),
	whereNot(), orWhereNot(),
	having(),   orHaving(),

其他支持 printf 格式字符串的 C++ 方法：
	whereBetween() 系列
	whereIn() 系列

C++ 语言示例：

```c++
	query->where("id < %d", 100);

	// 如果存在第二个参数，则将第一个参数作为 printf 格式字符串处理。
	// 输出 "city LIKE 'ber%'"
	query->where("city LIKE 'ber%%'", NULL);
```

如果以下 C++ 方法的第二个参数不存在，则将第一个参数作为原始字符串处理。  
这些 C++ 方法具有处理原始字符串的重载函数：

	on(),       orOn(),
	where(),    orWhere(),
	whereNot(), orWhereNot(),
	having(),   orHaving(),
	select(),
	groupBy(),
	orderBy()

C++ 语言示例：

```c++
	// 如果第二个参数不存在，则将第一个参数作为原始字符串处理。
	// 输出 "city LIKE 'ber%'"
	query->where("city LIKE 'ber%'");
```

#### select

您可以使用 select 方法为查询指定列。  
sq_query_select() 可以在参数中指定多个列（最后一个参数必须为 NULL）。  
  
使用 C 语言

```c
	//sq_query_select() 的最后一个参数必须为 NULL
	sq_query_select(query, "id", "name", NULL);
	sq_query_select(query, "email", NULL);

	// sq_query_distinct() 允许您强制查询返回不同的结果
	sq_query_distinct(query);
```

因为 C++ 方法 select() 使用参数包，所以最后一个参数可以传递（或不传递）NULL。  
  
使用 C++ 语言

```c++
	query->select("id", "name");
	query->select("email");

	// distinct 方法允许您强制查询返回不同的结果
	query->distinct();
```

#### where / whereNot / orWhere / orWhereNot

这些函数/方法用于过滤结果和应用条件。

* 参数的顺序是列名、比较运算符、要比较的值。
* 如果列名有 % 字符，则作为 printf 格式字符串处理。
* 如果比较运算符的参数是 =，则可以省略。

例如: 生成下面的 SQL 语句。

```sql
SELECT * FROM companies WHERE id > 15 OR city_id = 6 OR NOT members < 100
```

使用 C 语言

```c
	// SELECT * FROM companies
	sq_query_table(query, "companies");
	// WHERE id > 15
	sq_query_where(query, "id", ">", "15");
	// OR city_id = 6
	sq_query_or_where(query, "city_id", "6");
	// OR NOT members < 100
	sq_query_or_where_not(query, "members < %d", 100);
```

使用 C++ 语言

```c++
	// SELECT * FROM companies
	query->table("companies")
	     // WHERE id > 15
	     ->where("id", ">", "15")
	     // OR city_id = 6
	     ->orWhere("city_id", "6")
	     // OR NOT members < 100
	     ->orWhereNot("members < %d", 100);
```

这些方法也可以用来指定一组查询条件。  
  
使用 C 语言

```c
	// SELECT * FROM products WHERE NOT ( city_id = 6 OR price < 100 )
	sq_query_table(query, "products");
	sq_query_where_not(query);
		sq_query_where(query, "city_id", "6");
		sq_query_or_where(query, "price < %d", 100);
	sq_query_pop_nested(query);
```

使用 C++ 语言

```c++
	// SELECT * FROM products WHERE NOT ( city_id = 6 OR price < 100 )
	query->table("products")
	     ->whereNot([query] {
	         query->where("city_id", "6")
	              ->orWhere("price < %d", 100);
		 });
```

#### whereBetween / orWhereBetween

whereBetween 方法驗證列的值是否在兩個值之間。
* 这些方法指定 printf 格式字符串一次，使用两次。
  
使用 C 语言

```c
	// SELECT * FROM users WHERE votes BETWEEN 1 AND 100
	sq_query_table(query, "users");
	sq_query_where_between(query, "votes", "%d", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_between(query, "name", "'%s'", "Ray", "Zyx");
```

使用 C++ 语言

```c++
	// SELECT * FROM users WHERE votes BETWEEN 1 AND 100
	query->table("users")
	     ->whereBetween("votes", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	query->orWhereBetween("name", "'%s'", "Ray", "Zyx");
```

#### whereNotBetween / orWhereNotBetween

whereNotBetween 方法驗證列的值是否位於兩個值之外。  
  
使用 C 语言

```c
	// SELECT * FROM users WHERE votes NOT BETWEEN 1 AND 100
	sq_query_table(query, "users");
	sq_query_where_not_between(query, "votes", "%d", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_not_between(query, "name", "'%s'", "Ray", "Zyx");
```

使用 C++ 语言

```c++
	// SELECT * FROM users WHERE votes NOT BETWEEN 1 AND 100
	query->table("users")
	     ->whereNotBetween("votes", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	query->orWhereNotBetween("name", "'%s'", "Ray", "Zyx");
```

#### whereIn / whereNotIn / orWhereIn / orWhereNotIn

* 這些方法指定 printf 格式字符串一次，使用多次。
  
使用 C 语言  
  
sq_query_where_in() 的第三個參數必須指定 printf 格式字符串後的值的數量。

```c
	// SELECT * FROM users WHERE id IN (1,2,4)
	sq_query_table(query, "users");
	sq_query_where_in(query, "id", 3, "%d", 1, 2, 4);
```

使用 C++ 语言

```c++
	// SELECT * FROM users WHERE id IN (1,2,4)
	query->table("users")
	     ->whereIn("id", 1, 2, 4);
```

whereIn() 可以与 printf 格式字符串一起使用：
* 第二个参数是 printf 格式字符串之后的值的数量。
* 如果用户将第二个参数指定为 0，它将由编译器计算。

```c++
	// SELECT * FROM users WHERE id IN ('Ray','Alex','Xyz')
	query->table("users")
	     ->whereIn("id", 0, "'%s'", "Ray", "Alex", "Xyz");
```

#### whereNull / whereNotNull / orWhereNull / orWhereNotNull

这些方法用于指定 SQL 条件 "IS NULL" 或 "IS NOT NULL"。  
  
使用 C 语言

```c
	// SELECT * FROM users WHERE updated_at IS NULL
	sq_query_table(query, "users");
	sq_query_where_null(query, "updated_at");
```

使用 C++ 语言

```c++
	// SELECT * FROM users WHERE updated_at IS NOT NULL
	query->table("users")
	     ->whereNotNull("updated_at");
```

#### having / orHaving

having 方法的用法与 where 方法类似。  

使用 C 语言

```c
	sq_query_table(query, "companies");
	sq_query_group_by(query, "city_id", NULL);    // 最后一个参数必须为 NULL
	sq_query_having(query, "age", ">", "10");
	sq_query_or_having(query, "members < %d", 50);
```

使用 C++ 语言

```c++
	query->table("companies")
	     ->groupBy("city_id")
	     ->having("age", ">", "10")
	     ->orHaving("members < %d", 50);
```

#### groupBy / orderBy

sq_query_order_by() 和 sq_query_group_by() 可以在参数中指定多个列（最后一个参数必须为 NULL）。  
  
使用 C 语言

```c
	// "GROUP BY companies.age, companies.name"
	// sq_query_group_by() 的最后一个参数必须为 NULL
	sq_query_group_by(query, "companies.age", "companies.name", NULL);

	// "ORDER BY companies.id DESC"
	// sq_query_order_by() 的最后一个参数必须为 NULL
	sq_query_order_by(query, "companies.id", NULL);
	sq_query_desc(query);
```

因为 C++ 方法 orderBy() 和 groupBy() 使用参数包，所以最后一个参数可以传递（或不传递）NULL。  
* orderBy() 的用法与 Laravel 不同。  
  
使用 C++ 语言

```c++
	// "GROUP BY companies.age, companies.name"
	query->groupBy("companies.age", "companies.name");

	// "ORDER BY companies.id DESC"
	query->orderBy("companies.id")->desc();
	// 或
	query->orderByDesc("companies.id");
```

#### limit / offset

limit() 方法可以限制查询返回的结果数量，使用 offset() 方法跳过查询中指定数量的结果。  
用户可以多次调用 limit() 和 offset()。最后一次呼叫指定的引數將會覆寫上一次呼叫時指定的引數。  
  
使用 C 语言

```c
	sq_query_table("users");

	// 指定 LIMIT 3 OFFSET 6
	sq_query_offset(6);
	sq_query_limit(3);

	// 覆盖上一次调用的参数。
	sq_query_offset(10);
	sq_query_limit(5);

	// SQL语句的最终结果：
	// SELECT * FROM users LIMIT 5 OFFSET 10
```

使用 C++ 语言

```c++
	query->table("users");

	// 指定 LIMIT 3 OFFSET 6
	query->offset(6)->limit(3);

	// 覆盖上一次调用的参数。
	query->offset(10)->limit(5);

	// SQL语句的最终结果：
	// SELECT * FROM users LIMIT 5 OFFSET 10
```

#### deleteFrom / truncate

最后调用这些函数（在生成 SQL 语句之前）。  
  
例如: 生成下面的 SQL 语句。

```sql
DELETE FROM companies
```

```c++
	// C 函数
	sq_query_table(query, "companies");
	sq_query_delete(query);
	sql = sq_query_to_sql(query);

	// C++ 方法
	query->table("companies");
	query->deleteFrom();
	sql = query->toSql();
```

例如: 生成下面的 SQL 语句。

```sql
TRUNCATE TABLE companies
```

```c++
	// C 函数
	sq_query_table(query, "companies");
	sq_query_truncate(query);
	sql = sq_query_to_sql(query);

	// C++ 方法
	query->table("companies");
	query->truncate();
	sql = query->toSql();
```

## 排除 "SELECT * FROM table_name" 的 SQL 语句

如果在 SqQuery 中不指定表名和列名，它将生成排除 "SELECT * FROM table_name" 的 SQL 语句。  
sq_storage_get_all()、sq_storage_update_all() 和 sq_storage_remove_all() 中的 'SQL 语句' 参数可以使用这个。  
  
使用 C 语言

```c
	sq_query_clear(query);
	// WHERE id > 10 OR city_id < 9
	sq_query_where_raw(query, "id > 10");
	sq_query_or_where_raw(query, "city_id < 9");

	// 使用 sq_query_c() 生成 SQL 语句
	array = sq_storage_remove_all(storage, "users",
	                              sq_query_c(query));

	// 或使用 sq_query_to_sql() 生成 SQL 语句
	sql_where = sq_query_to_sql(query);
	array = sq_storage_remove_all(storage, "users",
	                              sql_where);
	free(sql_where);
```

使用 C++ 语言

```c++
	query->clear();
	// WHERE id > 10 OR city_id < 9
	query->where("id > 10");
	query->orWhere("city_id < 9");

	// 使用 Sql::Query::c() 生成 SQL 语句
	array = storage->removeAll("users", query->c());

	// 或使用 Sql::Query::toSql() 生成 SQL 语句
	sql_where = query->toSql();
	array = storage->removeAll("users", sql_where);
	free(sql_where);
```

#### 方便的 C++ 类 'where' 系列

使用 C++ Sq::Where 和 Sq::WhereRaw（或 Sq::where 和 Sq::whereRaw）生成 SQL 语句  
  
1. 使用 Sq::Where（或 Sq::where）的 operator()

```c++
	Sq::Where  where;

	array = storage->removeAll("users",
			where("id < %d", 11).orWhere("city_id < %d", 33));
```

2. 使用参数包构造函数

```c++
	array = storage->removeAll("users",
			Sq::where("id < %d", 11).orWhere("city_id < %d", 33));
```

3. 使用默认构造函数和 operator()

```c++
	array = storage->removeAll("users",
			Sq::where()("id < %d", 11).orWhere("city_id < %d", 33));
```

4. 下面是目前提供的方便的 C++ 类：

```
	Sq::Where,        Sq::WhereNot,
	Sq::WhereRaw,     Sq::WhereNotRaw,
	Sq::WhereExists,  Sq::WhereNotExists,
	Sq::WhereBetween, Sq::WhereNotBetween,
	Sq::WhereIn,      Sq::WhereNotIn,
	Sq::WhereNull,    Sq::WhereNotNull,

	'Where' 类系列使用 'typedef' 给它们新名称：小写的 'where' 类系列。

	Sq::where,        Sq::whereNot,
	Sq::whereRaw,     Sq::whereNotRaw,
	Sq::whereExists,  Sq::whereNotExists,
	Sq::whereBetween, Sq::whereNotBetween,
	Sq::whereIn,      Sq::whereNotIn,
	Sq::whereNull,    Sq::whereNotNull,
```

## 原始方法 Raw Methods

将原始表达式插入查询的各个部分。  

#### selectRaw

使用 C 语言

```c
	sq_query_table(query, "users");
	sq_query_select_raw(query, "COUNT(column_name)");
```

使用 C++ 语言

```c++
	query->table("users")
	     ->selectRaw("COUNT(column_name)");
```

C++ 方法 select() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("users")
	     ->select("COUNT(column_name)");
```

#### whereRaw / whereNotRaw / orWhereRaw / orWhereNotRaw

使用 C 语言

```c
	sq_query_table(query, "users");
	sq_query_where_raw(query, "id > 100 AND id < 300");
	sq_query_where_raw(query, "city LIKE 'ber%'");
```

使用 C++ 语言

```c++
	query->table("users")
	     ->whereRaw("id > 100 AND id < 300")
	     ->whereRaw("city LIKE 'ber%'");
```

C++ 方法 where()/orWhere() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("users")
	     ->where("id > 100 AND id < 300")
	     ->where("city LIKE 'ber%'");
```

#### havingRaw / orHavingRaw

使用 C 语言

```c
	sq_query_table(query, "orders");
	sq_query_group_by(query, "city_id", NULL);    // 最后一个参数必须为 NULL
	sq_query_having_raw(query, "SUM(price) > 3000");
```

使用 C++ 语言

```c++
	query->table("orders")
	     ->groupBy("city_id")
	     ->havingRaw("SUM(price) > 3000");
```

C++ 方法 have()/orHaving() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("orders")
	     ->groupBy("city_id")
	     ->having("SUM(price) > 3000");
```

#### orderByRaw

使用 C 语言

```c
	sq_query_table(query, "orders");
	sq_query_order_by_raw(query, "updated_at DESC");
```

使用 C++ 语言

```c++
	query->table("orders")
	     ->orderByRaw("updated_at DESC");
```

C++ 方法 orderBy() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("orders")
	     ->orderBy("updated_at DESC");
```

#### groupByRaw

使用 C 语言

```c
	sq_query_table(query, "companies");
	sq_query_group_by_raw(query, "city, state");
```

使用 C++ 语言

```c++
	query->table("companies")
	     ->groupByRaw("city, state");
```

C++ 方法 groupBy() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("companies")
	     ->groupBy("city, state");
```

#### 原始 SQL 语句

sq_query_raw() 和 sq_query_printf() 可以在当前嵌套或子查询中附加原始 SQL 语句。  
  
例如: 生成下面的 SQL 语句。

```sql
SELECT * FROM users WHERE city LIKE 'ber%' LIMIT 20 OFFSET 10
```

使用 C 语言

```c
	// "SELECT * FROM users"
	sq_query_table(query, "users");

	// "WHERE city LIKE 'ber%'" 是原始字符串
	sq_query_raw(query, "WHERE city LIKE 'ber%'");

	// 第二个参数是 printf 格式字符串。
	sq_query_printf(query, "LIMIT %d OFFSET %d", 20, 10);
```

使用 C++ 语言

```c++
	// "SELECT * FROM users"
	query->table("users");

	// 因为第二个参数不存在，所以第一个参数是原始字符串。
	query->raw("WHERE city LIKE 'ber%'");

	// 因为第二个参数确实存在，所以第一个参数是 printf 格式字符串。
	query->raw("LIMIT %d OFFSET %d", 20, 10);
```

## 连接 Joins

#### 内连接 Inner Join

要生成 "内连接" 语句，您可以在 SqQuery 实例上使用 sq_query_join()。  
  
使用 C 语言

```c
	sq_query_table(query, "companies");
	sq_query_join(query, "city", "users.id", "=", "posts.user_id");
```

使用 C++ 语言

```c++
	query->table("companies")
	     ->join("city", "users.id", "=", "posts.user_id");
```

#### 左连接 Left Join / 右连接 Right Join / 全外连接 Full Join

使用 C 语言

```c
	sq_query_table(query, "users");
	sq_query_left_join(query, "posts", "users.id", "=", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_right_join(query, "posts", "users.id", "=", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_full_join(query, "posts", "users.id", "=", "posts.user_id");
```

使用 C++ 语言

```c++
	query->table("users")
	     ->leftJoin("posts", "users.id", "=", "posts.user_id");

	query->table("users")
	     ->rightJoin("posts", "users.id", "=", "posts.user_id");

	query->table("users")
	     ->fullJoin("posts", "users.id", "=", "posts.user_id");
```

#### 交叉连接 Cross Join

使用 C 语言

```c
	sq_query_table(query, "users");
	sq_query_cross_join(query, "posts");
```

使用 C++ 语言

```c++
	query->table("users")
	     ->crossJoin("posts");
```

#### join 方法与 on / orOn 方法一起使用

on 方法的用法与 where 方法类似。  
  
使用 C 语言

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// JOIN posts ON users.id = posts.user_id
	sq_query_join(query, "posts", "users.id", "=", "posts.user_id");
	// AND users.id > 120
	sq_query_on(query, "users.id > %d", 120);
```

使用 C++ 语言

```c++
	// SELECT * FROM users
	query->table("users")
	     // JOIN posts ON users.id = posts.user_id
	     ->join("posts", "users.id", "=", "posts.user_id")
	     // AND users.id > 120
	     ->on("users.id > %d", 120);
```

## 联合 Unions

将两个或多个查询 “联合” 在一起。  

```sql
SELECT name1 FROM product1 UNION SELECT name2 FROM product2
```

用户必须在调用 sq_query_union() 或 sq_query_union_all() 后添加其他查询，并在查询结束时调用 sq_query_pop_nested()。  
  
使用 C 语言

```c
	sq_query_select(query, "name1", NULL);
	sq_query_from(query, "product1");

	sq_query_union(query);                   // start of query
		sq_query_select(query, "name2", NULL);
		sq_query_from(query, "product2");
	sq_query_pop_nested(query);              // end of query
```

C++ 方法 union_() 和 unionAll() 使用 lambda 函数添加其他查询。
* 因为 'union' 是 C/C++ 关键字，所以我必须在此方法的尾部附加 '_'。

使用 C++ 语言

```c++
	query->select("name1");
	query->from("product1");

	query->union_([query] {
		query->select("name2");
		query->from("product2");
	});
```

## 嵌套和子查询

SqQuery 可以产生有限的嵌套和子查询。您也可以使用原始方法来执行这些操作。  
  
以下 C 函数支持子查询或嵌套：

	sq_query_join(),
	sq_query_left_join(),
	sq_query_right_join(),
	sq_query_full_join(),
	sq_query_cross_join(),
	sq_query_on(),           sq_query_or_on(),
	sq_query_where(),        sq_query_or_where(),
	sq_query_where_not(),    sq_query_or_where_not(),
	sq_query_where_exists(), sq_query_where_not_exists(),
	sq_query_having(),       sq_query_or_having(),

注意 1：除了 sq_query_where_exists() 和 sq_query_where_not_exists()，这些函数的第二个参数必须为 NULL。  
注意 2：您必须在子查询或嵌套的末尾调用 sq_query_pop_nested()。  
  
下面的 C++ 方法使用 lambda 函数来支持子查询或嵌套，用户不需要调用 sq_query_pop_nested()  

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

#### 嵌套 Nested

例如: 生成下面的 SQL 语句。

```sql
SELECT * FROM users WHERE (salary > 45 AND age < 21) OR id > 100
```

使用 C 函数生成嵌套：

```c
	sq_query_table(query, "users");
	sq_query_where(query, NULL);                // 嵌套的开始
		sq_query_where(query, "salary", ">", "45");
		sq_query_where(query, "age", "<", "21");
	sq_query_pop_nested(query);                 // 嵌套结束
	sq_query_or_where(query, "id > %d", 100);
```

使用 C++ lambda 函数生成嵌套：

```c++
	query->table("users")
	     ->where([query] {
	         query->where("salary", ">", "45")
	              ->where("age", "<", "21");
	     })
	     ->orWhere("id > %d", 100);
```

#### 子查询

例如: 下面是具有子查询的 SQL 语句。

```sql
SELECT id, age
FROM companies
JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
WHERE age > 5
```

使用 C++ lambda 函数生成子查询：

```c++
	query->select("id", "age")
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > 5");
```

## 使用宏生成查询

宏 SQ_QUERY_DO() 用于构建查询。宏中的最后一个参数类似于 lambda 函数。

```c
#include <sqxclib.h>
#include <SqQuery-macro.h>    // sqxclib.h 不包含特殊的宏

	SQ_QUERY_DO(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "100");
		}); SQQ_AS("c"); SQQ_ON("c.id = companies.city_id");
		SQQ_WHERE("age > 5");
	});
```
