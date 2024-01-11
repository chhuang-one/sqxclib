[English](SqQuery.md)

# SqQuery

SqQuery 是支持子查询和括号的查询构建器。  
  
**SqQuery 设计上可以独立运作。**  
若要单独使用 SqQuery，可将 SqQuery.h、SqQuery.c 和 SqQuery-proxy.h 复制到其他软件项目中，并在 SqQuery.h 头文件中将 SQ_QUERY_USE_ALONE 设置为 1。
它只是删除了 SqQuery 中的 sq_query_get_table_as_names() 和 sq_query_select_table_as() 声明和函数。

## 创建查询并生成 SQL 语句

SqQuery 提供 sq_query_to_sql() 和 sq_query_c() 来生成 SQL 语句。
* sq_query_to_sql() 的结果在您不需要时必须释放。
* 您不能释放 sq_query_c() 的结果，它由 SqQuery 管理。
* 调用 sq_query_c() 后，用户可以使用 sq_query_last() 以重用生成的 SQL 语句。
  
例如: 生成下面的 SQL 语句。它从数据库表 "companies" 中选择所有列。

```sql
SELECT * FROM companies
```

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

调用 sql_query_clear() 后，用户可以在现有实例中生成新的 SQL 语句。  
  
使用 C 语言

```c
	// 重置 SqQuery (删除所有语句)
	sq_query_clear(query);

	// 从数据库表 "users" 中选择列
	sq_query_table(query, "users");

	// 生成新的 SQL 语句
	sql = sq_query_c(query);

	// 调用 sq_query_c() 后，用户可以使用 sq_query_last() 来重用生成的 SQL 语句。
	sql = sq_query_last(query);
```

使用 C++ 语言

```c++
	// 重置 SqQuery (删除所有语句)
	query->clear();

	// 从数据库表 "users" 中选择列
	query->table("users");

	// 生成新的 SQL 语句
	sql = query->c();

	// 调用 sq_query_c() 后，用户可以使用 last() 来重用生成的 SQL 语句。
	sql = query->last();
```

## SQL 语句

#### from / table

from() 和 table() 可以指定数据库表。他们做同样的事情并支持子查询，其他详细信息在标题为 "子查询和括号" 中解释。  
  
注意: 如果用户没有通过 select() 指定列，则默认选择数据库表中的所有列。  
  
使用 C 语言

```c
	// 从数据库表 "users" 中选择列
	// SELECT * FROM users
	sq_query_from(query, "users");

	// 重置 SqQuery (删除所有语句)
	sq_query_clear(query);

	// 子查询
	// SELECT * FROM ( SELECT * FROM companies WHERE id < 65 )
//	sq_query_from_sub(query);
	sq_query_from(query, NULL);
		sq_query_from(query, "companies");
		sq_query_where_raw(query, "id < 65");
	sq_query_end_sub(query);
```

使用 C++ 语言

```c++
	// 从数据库表 "users" 中选择列
	// SELECT * FROM users
	query->from("users");

	// 重置 SqQuery (删除所有语句)
	query->clear();

	// 子查询
	// SELECT * FROM ( SELECT * FROM companies WHERE id < 65 )
	query->from([query] {
		query->from("companies")
		     ->whereRaw("id < 65");
	});
```

**方便的 C++ 类 'from'**  
  
使用 C++ Sq::From（或小写 Sq::from）生成 SQL 语句。它可以与 [SqStorage](SqStorage.cn.md) 的 query 方法一起使用。

```c++
	char *sql = Sq::from("users").where("id", "<", 10).toSql();

	// 与 SqStorage 的 query 方法一起使用
	array = storage->query(Sq::from("users").whereRaw("city_id > 5"));
```

#### select

您可以使用 select 方法为查询指定列。  
sq_query_select() 可以在参数中指定多个列。  
  
使用 C 语言

```c
	sq_query_select(query, "id", "name");
	sq_query_select(query, "email");

	// sq_query_distinct() 允许您强制查询返回不同的结果
	sq_query_distinct(query);
```

使用 C++ 语言

```c++
	query->select("id", "name");
	query->select("email");

	// distinct 方法允许您强制查询返回不同的结果
	query->distinct();
```

**方便的 C++ 类 'select'**  
  
使用 C++ Sq::Select（或小写 Sq::select）生成 SQL 语句。它可以与 [SqStorage](SqStorage.cn.md) 的 query 方法一起使用。

```c++
	char *sql = Sq::select("id", "name").from("users").where("id", "<", 10).toSql();

	// 与 SqStorage 的 query 方法一起使用
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));
```

#### where / whereNot / orWhere / orWhereNot

这些函数/方法用于过滤结果和应用条件。

* 参数的顺序是 列名、运算符、printf 格式字符串、取决于格式字符串的值。
* 如果用户没有指定格式字符串后面的值，程序将 printf 格式字符串作为原始字符串处理。
* 不建议：如果运算符的参数是 =，则可以省略 (像 Laravel，但可读性较差)。
* 已弃用：如果列名有 % 字符，则作为 printf 格式字符串处理 (这将不再支持)。
* 条件参数的用法在 where()、join()、on() 和 having() 系列函数中基本相同。

例如: 生成下面的 SQL 语句。

```sql
SELECT * FROM companies
WHERE id > 15 OR city_id = 6 OR name LIKE '%Motor'
```

使用 C 语言

```c
	// SELECT * FROM companies
	sq_query_table(query, "companies");
	// WHERE id > 15
	sq_query_where(query, "id", ">", "%d", 15);
	// OR city_id = 6
	sq_query_or_where(query, "city_id", "=", "%d", 6);

	// OR name LIKE '%Motor'
	sq_query_or_where(query, "name", "LIKE", "'%Motor'");
	// 程序在此处将 "'%Motor'" 作为原始字符串处理。
	// 如果用户没有指定格式字符串后面的值，程序将 printf 格式字符串作为原始字符串处理。
```

使用 C++ 语言  
  
C++ 方法 where() 系列具有省略 printf 格式字符串的重载函数：

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

这些方法也可以用来指定一组查询条件和子查询，其他详细信息在标题为 "子查询和括号" 中进行了解释。  

使用 C 语言

```c
	// SELECT * FROM products
	sq_query_table(query, "products");
	// WHERE NOT ( city_id = 6 OR price < 100 )

	sq_query_where_not_sub(query);      // 括号的开始
//	sq_query_where_not(query, NULL);    // 括号的开始
		sq_query_where(query, "city_id", "=", "%d", 6);
		sq_query_or_where_raw(query, "price < %d", 100);
	sq_query_end_sub(query);            // 括号的结束
```

使用 C++ 语言

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

whereBetween 方法验证列的值是否在两个值之间。
* 这些方法指定 printf 格式字符串一次，使用两次。
  
使用 C 语言

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// WHERE votes BETWEEN 1 AND 100
	sq_query_where_between(query, "votes", "%d", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_between(query, "name", "'%s'", "Ray", "Zyx");
```

使用 C++ 语言  
  
C++ 方法 whereBetween() 系列具有省略 printf 格式字符串的重载函数：

```c++
	// SELECT * FROM users
	query->table("users")
	     // WHERE votes BETWEEN 1 AND 100
	     ->whereBetween("votes", 1, 100);

	// OR name BETWEEN 'Ray' AND 'Zyx'
	query->orWhereBetween("name", "Ray", "Zyx");
```

#### whereNotBetween / orWhereNotBetween

whereNotBetween 方法验证列的值是否位于两个值之外。  
  
使用 C 语言

```c
	// SELECT * FROM users
	sq_query_table(query, "users");
	// WHERE votes NOT BETWEEN 1 AND 100
	sq_query_where_not_between(query, "votes", "%d", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	sq_query_or_where_not_between(query, "name", "'%s'", "Ray", "Zyx");
```

使用 C++ 语言

```c++
	// SELECT * FROM users
	query->table("users")
	     // WHERE votes NOT BETWEEN 1 AND 100
	     ->whereNotBetween("votes", 1, 100);

	// OR name NOT BETWEEN 'Ray' AND 'Zyx'
	query->orWhereNotBetween("name", "Ray", "Zyx");
```

#### whereIn / whereNotIn / orWhereIn / orWhereNotIn

* 这些方法指定 printf 格式字符串一次，使用多次。
  
使用 C 语言  
  
sq_query_where_in() 必须与 printf 格式字符串一起使用：
* 第三个参数是 printf 格式字符串后的值的数量。
* 如果用户将第三个参数指定为 0，它将通过宏计算参数的数量。

```c
	// SELECT * FROM users WHERE id IN (1,2,4)
	sq_query_table(query, "users");
	sq_query_where_in(query, "id", 3, "%d", 1, 2, 4);
```

使用 C++ 语言
  
C++ 方法 whereIn() 系列具有省略 printf 格式字符串的重载函数：

```c++
	// SELECT * FROM users WHERE id IN (1,2,4)
	query->table("users")
	     ->whereIn("id", 1, 2, 4);
```

当您将 whereIn() 与 printf 格式字符串一起使用时：
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

having() 系列的用法与 where() 类似。  
  
使用 C 语言

```c
	sq_query_table(query, "companies");
	sq_query_group_by(query, "city_id");
	sq_query_having(query, "age", ">", "%d", 10);
	sq_query_or_having_raw(query, "members < %d", 50);
```

使用 C++ 语言

```c++
	query->table("companies")
	     ->groupBy("city_id")
	     ->having("age", ">", 10)
	     ->orHavingRaw("members < %d", 50);
```

**having() 系列的括号示例**  
  
其他详细信息在标题为 "子查询和括号" 中进行了解释。  
  
使用 C 语言

```c
	// ... HAVING ( salary > 45 OR age < 21 )
	sq_query_having_sub(query);                 // 括号的开始
//	sq_query_having(query, NULL);               // 括号的开始
		sq_query_having(query, "salary", ">", "%d", 45);
		sq_query_or_having(query, "age", "<", "%d", 21);
	sq_query_end_sub(query);                    // 括号的结束
```

使用 C++ 语言

```c++
	// ... HAVING ( salary > 45 OR age < 21 )
	query->having([query] {
		query->having("salary", ">", 45);
		query->orHaving("age", "<", 21);
	});
```

#### groupBy / orderBy

sq_query_order_by() 和 sq_query_group_by() 可以在参数中指定多个列。  
  
使用 C 语言

```c
	// "GROUP BY companies.age, companies.name"
	sq_query_group_by(query, "companies.age", "companies.name");

	// "ORDER BY companies.id DESC"
	sq_query_order_by(query, "companies.id");
	sq_query_desc(query);
```

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
用户可以多次调用 limit() 和 offset()。最后一次呼叫指定的引数将会覆写上一次呼叫时指定的引数。  
  
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

## 排除 "SELECT * FROM ..." 的 SQL 语句

如果在 SqQuery 中不指定表名和列名，它将生成排除 "SELECT * FROM ..." 的 SQL 语句。  
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
	query->whereRaw("id > 10");
	query->orWhereRaw("city_id < 9");

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
			where("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

2. 使用参数包构造函数

```c++
	array = storage->removeAll("users",
			Sq::where("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

3. 使用默认构造函数和 operator()

```c++
	array = storage->removeAll("users",
			Sq::where()("id", "<", 11).orWhereRaw("city_id < %d", 33));
```

4. 使用 lambda 函数生成子查询和括号

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

5. 下面是目前提供的方便的 C++ 类：

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
  
C 函数 sq_query_where_raw() 系列使用宏来计算参数的数量。  
如果第三个参数不存在，则将第二个参数作为原始字符串处理。

```c
	// 以下两组查询结果相同。

	// 第二个参数为原始字符串
	sq_query_where_raw(query, "id > 100 AND id < 300");
	sq_query_where_raw(query, "name LIKE 'ber%'");

	// 第二个参数为 printf 格式字符串
	sq_query_where_raw(query, "id > %d AND id < %d", 100, 300);
	sq_query_where_raw(query, "name LIKE '%s'", "ber%");
```

使用 C++ 语言
  
C++ 方法 whereRaw()/orWhereRaw() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	// 以下两组查询结果相同。

	// 第一个参数为原始字符串
	query->whereRaw("id > 100 AND id < 300")
	     ->whereRaw("name LIKE 'ber%'");

	// 第一个参数为 printf 格式字符串
	query->whereRaw("id > %d AND id < %d", 100, 300)
	     ->whereRaw("name LIKE '%s'", "ber%");
```

#### havingRaw / orHavingRaw

使用 C 语言
  
C 函数 sq_query_having_raw() 系列使用宏来计算参数的数量。  
如果第三个参数不存在，则将第二个参数作为原始字符串处理。

```c
	sq_query_table(query, "orders");
	sq_query_group_by(query, "customer_id");

	// 以下3行具有相同的结果
	sq_query_having_raw(query, "SUM(price) > 3000");
//	sq_query_having_raw(query, "SUM(price) > %d", 3000);
//	sq_query_having(query, "SUM(price)", ">", "%d", 3000);
```

使用 C++ 语言
  
C++ 方法 havingRaw()/orHavingRaw() 具有处理原始字符串的重载函数。  
如果第二个参数不存在，则将第一个参数作为原始字符串处理。

```c++
	query->table("orders")
	     ->groupBy("customer_id");

	// 以下3行具有相同的结果
	query->havingRaw("SUM(price) > 3000");
//	query->havingRaw("SUM(price) > %d", 3000);
//	query->having("SUM(price)", ">", "%d", 3000);
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
SELECT * FROM users
WHERE city LIKE 'ber%'
LIMIT 20 OFFSET 10
```

使用 C 语言  
  
sq_query_raw() 使用宏来计算参数的数量。如果第三个参数不存在，则将第二个参数作为原始字符串处理。

```c
	// "SELECT * FROM users"
	sq_query_table(query, "users");

	// 因为第三个参数不存在，所以第二个参数是原始字符串。
	sq_query_raw(query, "WHERE city LIKE 'ber%'");

	// 因为第三个参数确实存在，所以第二个参数是 printf 格式字符串。
	sq_query_raw(query, "LIMIT %d OFFSET %d", 20, 10);
```

使用 C++ 语言  
  
C++ 方法 raw() 具有处理原始字符串的重载函数。

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
	sq_query_join(query, "city", "users.id", "=", "%s", "posts.user_id");
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
	sq_query_left_join(query,  "posts", "users.id", "=", "%s", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_right_join(query, "posts", "users.id", "=", "%s", "posts.user_id");

	sq_query_table(query, "users");
	sq_query_full_join(query,  "posts", "users.id", "=", "%s", "posts.user_id");
```

使用 C++ 语言
  
在下面的示例中，三种连接的条件是相同的。

```c++
	query->table("users")
	     ->leftJoin("posts",  "users.id", "posts.user_id");

	query->table("users")
	     ->rightJoin("posts", "users.id", "=", "posts.user_id");

	query->table("users")
	     ->fullJoin("posts",  "users.id", "=", "%s", "posts.user_id");
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
	sq_query_join(query, "posts", "users.id", "=", "%s", "posts.user_id");
	// AND users.id > 120
	sq_query_on_raw(query, "users.id > %d", 120);
```

使用 C++ 语言

```c++
	// SELECT * FROM users
	query->table("users")
	     // JOIN posts ON users.id = posts.user_id
	     ->join("posts", "users.id", "=", "posts.user_id")
	     // AND users.id > 120
	     ->onRaw("users.id > %d", 120);
```

#### 子查询连接 Subquery Joins

join() 和 on() 系列也能使用子查询和括号。  
  
例如: 下面是具有子查询的 SQL 连接。

```sql
SELECT id, age
FROM companies
JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
WHERE age > 5
```

使用 C 语言生成子查询：  
  
* sq_query_join_sub() 和 sq_query_on_sub() 是子查询的开始。
* sq_query_end_sub()  是子查询的结尾。
* sq_query_join()     和 sq_query_on() 在最后一个参数中传递 NULL 也是子查询的开始。

```c
	sq_query_select(query, "id", "age");
	sq_query_from(query, "companies");

	sq_query_join_sub(query);                   // 子查询的开始
//	sq_query_join(query, NULL);                 // 子查询的开始
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "%d", 100);
	sq_query_end_sub(query);                    // 子查询的结束

	sq_query_as(query, "c");
	sq_query_on_raw(query, "c.id = companies.city_id");
	sq_query_where_raw(query, "age > %d", 5);
```

使用 C++ lambda 函数生成子查询：

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

**join() 和 on() 系列的子查询和括号的更多示例：**  
  
使用 C 语言

```c
	// ... JOIN city ON ( city.id = companies.city_id )
	sq_query_join_sub(query, "city");           // 括号的开始
//	sq_query_join(query, "city", NULL);         // 括号的开始
		sq_query_on(query, "city.id", "=", "%s", "companies.city_id");
	sq_query_end_sub(query);                    // 括号的结束

	// ... JOIN city ON city.id = ( SELECT city_id FROM companies )
	sq_query_join_sub(query, "city", "city.id", "=");    // 子查询的开始
//	sq_query_join(query, "city", "city.id", "=", NULL);  // 子查询的开始
		sq_query_from(query, "companies");
		sq_query_select(query, "city_id");
	sq_query_end_sub(query);                             // 子查询的结束

	// ... ON ( city.id < 100 )
	sq_query_on_sub(query);                     // 括号的开始
//	sq_query_on(query, NULL);                   // 括号的开始
		sq_query_on(query, "city.id", "<", "%d", 100);
	sq_query_end_sub(query);                    // 括号的结束

	// ... ON city.id < ( SELECT city_id FROM companies WHERE id = 25 )
	sq_query_on_sub(query, "city.id", "<");     // 子查询的开始
//	sq_query_on(query, "city.id", "<", NULL);   // 子查询的开始
		sq_query_from(query, "companies");
		sq_query_select(query, "city_id");
		sq_query_where(query, "id", "=", "%d", 25);
	sq_query_end_sub(query);                    // 子查询的结束
```

使用 C++ 语言

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

## 联合 Unions

将两个或多个查询 “联合” 在一起。  

```sql
SELECT name1 FROM product1
UNION
SELECT name2 FROM product2
```

用户必须在调用 sq_query_union() 或 sq_query_union_all() 后添加其他查询，并在查询结束时调用 sq_query_end_sub()。  
  
使用 C 语言

```c
	sq_query_select(query, "name1");
	sq_query_from(query, "product1");

	sq_query_union(query);                   // 查询的开始
		sq_query_select(query, "name2");
		sq_query_from(query, "product2");
	sq_query_end_sub(query);                 // 查询的结束
```

C++ 方法 union_() 和 unionAll() 使用 lambda 函数添加其他查询。
* 因为 'union' 是 C/C++ 关键字，所以必须在此方法的尾部附加 '_'。

使用 C++ 语言

```c++
	query->select("name1");
	query->from("product1");

	query->union_([query] {
		query->select("name2");
		query->from("product2");
	});
```

## 子查询和括号 Subquery and Brackets

SqQuery 可以产生子查询或括号。事实上，子查询和括号在程序内部的实现方式相同。  
  
以下 C 函数支持子查询或括号：  
除了 sq_query_where_exists() 系列，这些函数/宏中的最后一个参数必须为 NULL。

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
	注意: 您必须在子查询或括号的末尾调用 sq_query_end_sub()。

下面是上述函数/宏的 C 方便宏：  
这些 C 宏使用可变参数宏在最后一个参数中传递 NULL。

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
	注意: 您必须在子查询或括号的末尾调用 sq_query_end_sub()。

下面的 C++ 方法使用 lambda 函数来支持子查询或括号，用户不需要调用 sq_query_end_sub()  

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

#### 括号 Brackets

括号的用法在 where()、on()、having() 系列函数中基本相同。  
如果您在括号中指定表和列，则括号将成为子查询。  
  
例如: 生成下面的 SQL 语句。

```sql
SELECT * FROM users
WHERE ( salary > 45 AND age < 21 ) OR id > 100
```

使用 C 函数生成括号：

```c
	sq_query_table(query, "users");

	sq_query_where_sub(query);                  // 括号的开始
//	sq_query_where(query, NULL);                // 括号的开始
		sq_query_where(query, "salary", ">", "%d", 45);
		sq_query_where(query, "age", "<", "%d", 21);
	sq_query_end_sub(query);                    // 括号结束

	sq_query_or_where_raw(query, "id > %d", 100);
```

使用 C++ lambda 函数生成括号：

```c++
	query->table("users")
	     ->where([query] {
	         query->where("salary", ">", 45)
	              ->where("age", "<", 21);
	     })
	     ->orWhereRaw("id > %d", 100);
```

#### 子查询 Subquery

子查询的用法在 where()、on()、having() 系列函数中基本相同。  
如果子查询中没有指定表和列，子查询变成括号。  
  
例如: 下面是在条件中有子查询的 SQL 语句。

```sql
SELECT * FROM products
WHERE price < ( SELECT AVG(amount) FROM incomes )
```

使用 C 语言生成在条件中的子查询：

```c
	// SELECT * FROM products
	sq_query_from(query, "products");

	// WHERE price < ( SELECT AVG(amount) FROM incomes )
	sq_query_where_sub(query, "price", "<");    // 子查询的开始
//	sq_query_where(query, "price", "<", NULL);  // 子查询的开始
		sq_query_select_raw(query, "AVG(amount)");
		sq_query_from(query, "incomes");
	sq_query_end_sub(query);                    // 子查询的结束
```

使用 C++ 语言生成在条件中的子查询：

```c
	// SELECT * FROM products
	query->from("products")
	// WHERE price < ( SELECT AVG(amount) FROM incomes )
	     ->where("price", "<", [query] {
	         query->selectRaw("AVG(amount)")
	              ->from("incomes");
	     });
```

## 附录 : 支持 printf 格式字符串的函数

有很多函数可以指定 SQL 条件，它们也支持 printf 格式字符串。请在传递条件值之前传递 printf 格式字符串。如果要在 printf 格式字符串中使用 SQL 通配符 '%'，则必须使用 "%%" 打印 "%"。  
  
以下 C 函数在第 2 个参数中支持 printf 格式字符串：

	sq_query_raw(),
	sq_query_printf(),
	sq_query_on_raw(),        sq_query_or_on_raw(),
	sq_query_where_raw(),     sq_query_or_where_raw(),
	sq_query_where_not_raw(), sq_query_or_where_not_raw(),
	sq_query_having_raw(),    sq_query_or_having_raw(),
	---
	这些 C 函数使用宏来计算参数的数量。
	如果第三个参数不存在，则将第二个参数作为原始字符串处理。

以下 C 函数在第 4 个参数中支持 printf 格式字符串：

	sq_query_on(),            sq_query_or_on(),
	sq_query_where(),         sq_query_or_where(),
	sq_query_where_not(),     sq_query_or_where_not(),
	sq_query_having(),        sq_query_or_having(),

以下 C 函数在第 5 个参数中支持 printf 格式字符串：

	sq_query_join(),
	sq_query_left_join(),
	sq_query_right_join(),
	sq_query_full_join(),

其他支持 printf 格式字符串的 C 函数：

	sq_query_where_between() 系列
	sq_query_where_in() 系列

C 语言示例：

```c
	// --- 第 4 个参数中的 printf 格式字符串 ---
	// WHERE id < 100
	sq_query_where(query, "id", "<", "%d", 100);
	// AND email LIKE 'guest%'
	sq_query_where(query, "email", "LIKE", "'%s'", "guest%");

	// --- 第 2 个参数中的 printf 格式字符串 ---
	// AND city  LIKE 'ber%'
	sq_query_where_raw(query, "city  LIKE '%s'", "ber%");
```

以下 C++ 方法在第 1 个参数中支持 printf 格式字符串：

	raw(),
	printf(),
	onRaw(),       orOnRaw(),
	whereRaw(),    orWhereRaw(),
	whereNotRaw(), orWhereNotRaw(),
	havingRaw(),   orHavingRaw(),

以下 C++ 方法在第 3 个参数中支持 printf 格式字符串：

	on(),          orOn(),
	where(),       orWhere(),
	whereNot(),    orWhereNot(),
	having(),      orHaving(),

以下 C++ 方法在第 4 个参数中支持 printf 格式字符串：

	join(),
	leftJoin(),
	rightJoin(),
	fullJoin(),

其他支持 printf 格式字符串的 C++ 方法：

	whereBetween() 系列
	whereIn() 系列

C++ 语言示例：

```c++
	// --- 第 3 个参数中的 printf 格式字符串 ---
	// WHERE id < 100
	query->where("id", "<", "%d", 100);
	// AND email LIKE 'guest%'
	query->where("email", "LIKE", "'%s'", "guest%");

	// --- 第 1 个参数中的 printf 格式字符串 ---
	// AND city  LIKE 'ber%'
	query->whereRaw("city  LIKE '%s'", "ber%");
```

C++ 方法具有省略 printf 格式字符串的重载函数：

```c++
	// --- 省略第 3 个参数中的 printf 格式字符串 ---
	// WHERE id < 100
	query->where("id", "<", 100);
	// AND email LIKE 'guest%'
	query->where("email", "LIKE", "guest%");
```

如果以下 C++ 方法的第二个参数不存在，则将第一个参数作为原始字符串处理。  
这些 C++ 方法具有处理原始字符串的重载函数：

	onRaw(),       orOnRaw(),
	whereRaw(),    orWhereRaw(),
	whereNotRaw(), orWhereNotRaw(),
	havingRaw(),   orHavingRaw(),
	select(),
	groupBy(),
	orderBy()

C++ 语言示例：

```c++
	// 如果第二个参数不存在，则将第一个参数作为原始字符串处理。
	// WHERE city LIKE 'ber%'
	query->whereRaw("city LIKE 'ber%'");
```
