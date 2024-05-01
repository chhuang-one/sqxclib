[English](database-migrations.md)

# 数据库：迁移

本文档描述了如何动态定义 SQL 表、列、迁移。迁移功能可以处理动态和静态列/表定义。
您还可以使用 SqApp 通过命令行工具运行和反向迁移，请参阅 [SqApp.cn.md](SqApp.cn.md)  

注意: 因为很多用户使用过 Laravel，所以有很多 sqxclib C++ 方法名和它类似。
实际上 sqxclib 的设计与 Laravel 不同，所以用法不可能相同。  

## 运行迁移

迁移架构并同步到数据库。  
  
使用 C++ 语言

```c++
	// 迁移 'schema_v1' 和 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// 将架构同步到数据库并更新 'storage' 中的架构
	// 这主要由 SQLite 使用
	storage->migrate(NULL);

	// 释放未使用的 'schema_v1' 和 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

使用 C 语言

```c
	// 迁移 'schema_v1' 和 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// 将架构同步到数据库并更新 'storage' 中的架构
	// 这主要由 SQLite 使用
	sq_storage_migrate(storage, NULL);

	// 释放未使用的 'schema_v1' 和 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```

## 表

首先，我们定义一个映射到您的数据库表 "users" 的 C 结构化数据类型。

```c
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct  User    User;

struct User {
	int     id;          // 主键
	char   *name;
	char   *email;

	time_t  created_at;

	int     test_add;
};
```

#### 创建表（动态）

在数据库中创建一个新表及其列。  
  
使用 C++ 语言  
  
create() 方法需要指定 结构数据类型 和 表名称。

```c++
	// 创建表 "users"
	table = schema->create<User>("users");

	// 向表中添加列
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->useCurrent();
```

使用 C 语言  
  
sq_schema_create() 函数接受两个参数：第一个参数是表的名称，第二个参数是结构化数据类型。

```c
	// 创建表 "users"
	table = sq_schema_create(schema, "users", User);

	// 向表中添加列
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_use_current(column);
```

如果您的表是用 C 语言定义的，但应用程序是用 C++ 语言编写的，则不能使用以下 C++ 模板函数来访问 SQL 表。

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

因为 gcc 的 typeid(Type).name() 会返回奇怪的名称，所以你会从 C 和 C++ 源代码中得到不同的类型名称。 在这种情况下，您必须用 C++ 语言创建或定义 SqTable 的类型，或者使用 setName() 方法设置 C 语言创建的 SqTable::type 的名称。

```c++
	// 'cUserTable' 是用 C 语言创建的并且不是常量。
	SqTable *table = cUserTable;

	// 将 C 类型名称更改为 C++ 类型名称
	table->type->setName(typeid(User).name());
		// 或
	table->type->setName(SQ_GET_TYPE_NAME(User));
```

#### 检查表是否存在

您可以使用 find 函数检查表是否存在:
* 此函数不在数据库找表，它在 SqSchema 的实例中找表。

```c++
	/* C++ 示例代码 */
	if (schema->find("users") == NULL) {
		// 'schema' 中不存在 "users" 表.
	}

	/* C 示例代码 */
	if (sq_schema_find(schema, "users") == NULL) {
		// 'schema' 中不存在 "users" 表.
	}
```

#### 更新表（动态）

使用 alter 函数更新现有表。  
  
使用 C++ 语言

```c++
	// 更改表 "users"
	table = schema->alter("users");

	// 将列添加到表中
	table->integer("test_add", &User::test_add);

	// 更改表中的 "email" 列
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

使用 C 语言

```c
	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 向表中添加列
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));

	// 更改表中的 "email" 列
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	sq_column_change(column);
```

#### 重命名/删除表（动态）

使用 rename 函数来重命名现有的数据库表。

```c++
	/* C++ 示例代码 */
	schema->rename("old table name", "new table name");

	/* C 示例代码 */
	sq_schema_rename(schema, "old table name", "new table name");
```

您可以使用 drop 函数来删除现有的表。

```c++
	/* C++ 示例代码 */
	schema->drop("users");

	/* C 示例代码 */
	sq_schema_drop(schema, "users");
```

## 列

#### 创建列（动态）

要将列添加到表中，您可以使用 SqTable 中的函数。
在 'schema' 中调用 alter 或 create 函数后，您将获得 SqTable 的实例。  
  
[SqTable](SqTable.cn.md) 中列出了所有可用的列类型。  
  
使用 C++ 语言

```c++
	// 更改表 "users"
	table = schema->alter("users");

	// 将列添加到表中
	column = table->integer("test_add", &User::test_add);
```

使用 C 语言

```c
	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 将列添加到表中
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
```

#### 列修饰符（动态）

在将列添加到表或将条目添加到结构时，您可以使用几个 "修饰符"。  
  
[SqColumn](SqColumn.cn.md) 中列出了所有可用的列修饰符。  
  
例如，使列 "nullable":

```c++
	/* C++ 示例代码 */
	table->string("name", &User::name)->nullable();

	/* C 示例代码 */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	sq_column_nullable(column);
```

#### 更新列属性

C 函数 sq_column_change()、C++ 方法 change() 允许您修改现有列的类型和属性。  
  
使用 C 语言

```c
	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 更改表中的 "email" 列
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	sq_column_change(column);
```

使用 C++ 语言

```c++
	// 更改表 "users"
	table = schema->alter("users");

	// 更改表中的 "email" 列
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

#### 重命名和删除列

使用 C++ 方法 renameColumn() 重命名列。  
使用 C++ 方法 dropColumn() 删除列。

```c++
	// 更改表 "users"
	table = schema->alter("users");

	// 重命名列
	table->renameColumn("from", "to");

	// 删除列
	table->dropColumn("columnName");
```

使用 C 函数 sq_table_rename_column() 重命名列。  
使用 C 函数 sq_table_drop_column() 删除列。

```c
	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 重命名列
	sq_table_rename_column(table, "from", "to");

	// 删除列
	sq_table_drop_column(table, "columnName");
```

## 主键约束

要定义主键列，请在列定义上使用 primary() 方法：

```c++
	/* C 示例代码 */
	column = sq_table_add_int(table, "id", offsetof(User, id));
	sq_column_primary(column);

	/* C++ 示例代码 */
	table->integer("id", &User::id)
	     ->primary();
```

#### 创建主键

C 函数 sq_table_add_primary() 和 C++ 方法 primary() 可以创建主键约束。
参数是主键约束的名称，其他参数是以 NULL 结尾的列名参数列表。  
由于 C++ 方法使用参数包，因此最后一个参数可以传递 NULL，也可以不传递。

```c++
	/* C 示例代码 */
	column = sq_table_add_primary(table, "primary_email_account_id", "email", "account_id", NULL);

	/* C++ 示例代码 */
	table->primary("primary_email_account_id", "email", "account_id");
```

#### 删除主键

用户必须指定主键约束的名称才能删除它。

```c++
	/* C 示例代码 */
	sq_table_drop_primary(table, "primary_email_account_id");

	/* C++ 示例代码 */
	table->dropPrimary("primary_email_account_id");
```

## 外键约束

C 函数 sq_column_reference() 和 C++ 方法 reference() 用于设置外键引用的 表、列。
参数是外部表的名称，其他是以 NULL 结尾的列名参数列表。  
  
要定义外键列，请在列定义上使用 reference()：

```c++
	/* C 示例代码 */
	column = sq_table_add_int(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id", NULL);

	/* C++ 示例代码 */
	table->integer("city_id", &User::city_id)
	     ->reference("cities", "id");
```

#### 创建外键

C 函数 sq_table_add_foreign() 和 C++ 方法 foreign() 可以创建外键约束。
参数是外键约束的名称，其他是以 NULL 结尾的列名参数列表。  
  
由于外键中的列数必须与引用表中的列数匹配，因此 foreign() 和 reference() 参数中的列数必须匹配。  
  
使用 C 语言

```c
	column = sq_table_add_foreign(table, "foreignName", "column1", "column2", NULL);
	sq_column_reference(column, "foreignTableName",     "column1", "column2", NULL);
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");
```

使用 C++ 语言  
  
由于 C++ 方法使用参数包，因此最后一个参数可以传递 NULL，也可以不传递。

```c++
	table->foreign("foreignName", "column1", "column2")
	     ->reference("tableName", "column1", "column2")
	     ->onDelete("NO ACTION")
	     ->onUpdate("NO ACTION");
```

#### 删除外键

使用外键的约束名称来删除。

```c++
	/* C 示例代码 */
	sq_table_drop_foreign(table, "users_city_id_foreign");

	/* C++ 示例代码 */
	table->dropForeign("users_city_id_foreign");
```

## 索引

要定义唯一列，请在列定义上使用 unique() 方法：

```c++
	/* C 示例代码 */
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	sq_column_unique(column);

	/* C++ 示例代码 */
	table->string("email", &User::email)
	     ->unique();
```

#### 创建索引

要使用 C 函数创建复合 索引 和 唯一 的约束，
第二个参数指定 索引 和 唯一 的约束名称，其他是以 NULL 结尾的列名参数列表。

```c
	column = sq_table_add_index(table, "index_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_unique(table, "unique_email_account_id", "email", "account_id", NULL);
```

要使用 C++ 方法创建复合 索引 和 唯一 的约束，
第一个参数指定 索引 和 唯一 的约束名称，其他是列名的参数列表。  
由于 C++ 方法使用参数包，因此最后一个参数可以传递 NULL，也可以不传递。

```c++
	table->index("index_email_account_id", "email", "account_id");

	table->unique("unique_email_account_id", "email", "account_id");
```

#### 删除索引

用户必须指定要删除的 索引 和 唯一 的约束名称。

```c++
	/* C 示例代码 */
	sq_table_drop_index(table, "index_email_account_id");
	sq_table_drop_unique(table, "unique_email_account_id");

	/* C++ 示例代码 */
	table->dropIndex("index_email_account_id");
	table->dropUnique("unique_email_account_id");
```
