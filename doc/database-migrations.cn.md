[English](database-migrations.md)

# 数据库：迁移

本文档描述了如何动态定义 SQL 表、列、迁移。
您还可以使用 SqApp 通过命令行工具运行和反向迁移，请参阅 [SqApp.cn.md](SqApp.cn.md)  

注意：因为很多用户使用过 Laravel，所以有很多 sqxclib C++ 方法名和它类似。
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

### 创建表（动态）

因为 gcc 的 typeid(Type).name() 会返回奇怪的名称，你会从 C 和 C++ 源代码中得到不同的类型名称。  
**如果您的应用程序是用 C++ 语言编写的，请用 C++ 语言创建或定义 SqTable 的类型。**  
  
如果 SqTable::type 用 C 语言定义，你不能使用下面的 C++ 模板函数来访问 SQL 表.

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

使用 Schema 的 create 函数来创建一个新的数据库表。  
该函数接受两个参数：一个参数是表的名称，另一个是结构类型。  

```c++
	/* C++ 示例代码 */

	// 创建表 "users"
	table = schema->create<User>("users");

	// 向表中添加列
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->default_("CURRENT_TIMESTAMP");
```

```c
	/* C 示例代码 */

	// 创建表 "users"
	table = sq_schema_create(schema, "users", User);

	// 向表中添加列
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;        // 设置 SqColumn.bit_field

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_default("CURRENT_TIMESTAMP");
```

#### 检查表是否存在

您可以使用 find 函数检查表是否存在:
* 此函数不在数据库找表，它在 SqSchema 的实例中找表.

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

### 更新表（动态）

使用 alter 函数更新现有表.

```c++
	/* C++ 示例代码 */

	// 更改表 "users"
	table = schema->alter("users");

	// 将列添加到表中
	table->integer("test_add", &User::test_add);
	// 更改表中的 "email" 列
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

```c
	/* C 示例代码 */

	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 向表中添加列
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	// 更改表中的 "email" 列
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	column->bit_field |= SQB_CHANGED;
```

### 重命名/删除表（动态）

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

### 创建列（动态）

要将列添加到表中，您可以使用 SqTable 中的函数。
在 'schema' 中调用 alter 或 create 函数后，您将获得 SqTable 的实例。

```c++
	/* C++ 示例代码 */

	// 更改表 "users"
	table = schema->alter("users");

	// 将列添加到表中
	column = table->integer("test_add", &User::test_add);
```

```c
	/* C 示例代码 */

	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 将列添加到表中
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
```

### 可用的列类型（动态）

下面的 C++ 方法 (和 C 函数)对应于您可以添加到数据库表中的不同类型的列。

| C++ 方法    | C 函数                  | C 数据类型    |
| ----------- | ----------------------- | ------------- |
| boolean     | sq_table_add_bool       | bool          |
| bool_       | sq_table_add_bool       | bool          |
| integer     | sq_table_add_int        | int           |
| int_        | sq_table_add_int        | int           |
| uint        | sq_table_add_uint       | unsigned int  |
| int64       | sq_table_add_int64      | int64_t       |
| uint64      | sq_table_add_uint64     | uint64_t      |
| timestamp   | sq_table_add_timestamp  | time_t        |
| double_     | sq_table_add_double     | double        |
| string      | sq_table_add_string     | char*         |
| char_       | sq_table_add_char       | char*         |
| custom      | sq_table_add_custom     | *User define* |

* 因为 'bool'、'int'、'double' 和 'char' 是 C/C++ 关键字，所以在这些方法的尾部附加 '_'。

以下方法仅适用于 C++ 数据类型。

| C++ 方法    | C 数据类型    |
| ----------- | ------------- |
| stdstring   | std::string   |

### 列修饰符（动态）

在将列添加到表或将条目添加到结构时，您可以使用几个“修饰符”。
例如，使列 "nullable":

```c++
	/* C++ 示例代码 */
	table->string("name", &User::name)->nullable();

	/* C 示例代码 */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column->bit_field |= SQB_NULLABLE;
```

以下 C++ 方法 (和 C 函数) 对应于列修饰符：

| C++ 方法             | C 函数              | C 位字段名             |
| -------------------- | ------------------- | --------------------- |
| primary()            |                     | SQB_PRIMARY           |
| unique()             |                     | SQB_UNIQUE            |
| autoIncrement()      |                     | SQB_AUTOINCREMENT     |
| nullable()           |                     | SQB_NULLABLE          |
| useCurrent()         |                     | SQB_CURRENT           |
| useCurrentOnUpdate() |                     | SQB_CURRENT_ON_UPDATE |
| default_(string)     | sq_column_default() |                       |

* 因为 "default" 是 C/C++ 关键字，所以在此方法的尾部附加 "_"。

结构类型的特殊方法。

| C++ 方法         | C 位字段名        | 描述                                               |
| ---------------- | ----------------- | -------------------------------------------------- |
| pointer()        | SQB_POINTER       | 这个数据成员是一个指针。                           |
| hidden()         | SQB_HIDDEN        | 不要将此数据成员输出到 JSON。                      |
| hiddenNull()     | SQB_HIDDEN_NULL   | 如果它的值为 NULL，则不要将此数据成员输出到 JSON。 |

### 更新列属性

C++ 语言：change 方法允许您修改现有列的类型和属性。

```c++
	/* C++ 示例代码 */

	// 更改表 "users"
	table = schema->alter("users");

	// 更改表中的 "email" 列
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

C 语言：在 bit_field 中设置 SQB_CHANGE 允许您修改现有列的类型和属性。

```c
	/* C 示例代码 */

	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 更改表中的 "email" 列
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	column->bit_field |= SQB_CHANGED;
```

### 重命名和删除列

使用 C++ 方法 renameColumn 和 C 函数 sq_table_rename_column 重命名列。  
使用 C++ 方法 dropColumn 和 C 函数 sq_table_drop_column 删除列。

```c++
	/* C++ 示例代码 */

	// 更改表 "users"
	table = schema->alter("users");
	// 重命名列
	table->renameColumn("from", "to");
	// 删除列
	table->dropColumn("columnName");

	/* C 示例代码 */

	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);
	// 重命名列
	sq_table_rename_column(table, "from", "to");
	// 删除列
	sq_table_drop_column(table, "columnName");
```

## 索引

### 创建索引

在列定义上使用 unique 方法：

```c++
	/* C++ 示例代码 */
	table->string("email", &User::email)->unique();

	/* C 示例代码 */
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	column->bit_field |= SQB_UNIQUE;
```

要使用 C 函数创建复合 唯一、索引、主键，
第二个参数指定 唯一、索引、主键的名称，其他是必须以空值结尾的列名列表。

```c
	column = sq_table_add_index(table, "index_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_unique(table, "unique_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_primary(table, "primary_email_account_id", "email", "account_id", NULL);
```

要使用 C++ 方法创建复合 唯一、索引、主键，
第一个参数指定 唯一、索引、主键的名称，其他是列名列表。  
因为 C++ 方法使用参数包，所以最后一个参数可以传递（或不传递）NULL。

```c++
	table->index("index_email_account_id", "email", "account_id");

	table->unique("unique_email_account_id", "email", "account_id");

	table->primary("primary_email_account_id", "email", "account_id");
```

### 删除索引

用户必须指定要删除的 索引、唯一、主键的名称。

```c++
	/* C++ 示例代码 */

	table->dropIndex("index_email_account_id");

	table->dropUnique("unique_email_account_id");

	table->dropPrimary("primary_email_account_id");

	/* C 示例代码 */

	sq_table_drop_index(table, "index_email_account_id");

	sq_table_drop_unique(table, "unique_email_account_id");

	sq_table_drop_primary(table, "primary_email_account_id");
```

### 外键约束

使用 foreign 函数创建外键。  
第一个参数指定外键名称，第二个是列名。

```c++
	/* C++ 示例代码 */

	table->foreign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");

	/* C 示例代码 */

	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");
```

#### 删除外键

使用外键约束的名称来删除。

```c++
	/* C++ 示例代码 */
	table->dropForeign("users_city_id_foreign");

	/* C 示例代码 */
	sq_table_drop_foreign(table, "users_city_id_foreign");
```
