[English](README.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# 思库可思 sqxclib

sqxclib 是在 C 语言和 SQL、JSON 等之间转换数据的库。它提供 ORM 的功能和 C++ 包装器。  
项目地址: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## 目前的功能:

* 开发者可以使用 C99 指定初始化(designated initializer) 或 C++ 聚合初始化(aggregate initialization) 定义常量数据库 表、列、迁移，
   这可以减少制作架构时的运行时间，请参阅 doc/[schema-builder-constant.cn.md](doc/schema-builder-constant.cn.md)。
   当然也可以使用 C 函数 或 C++ 方法 动态执行这些操作。

* 所有定义的 SQL 表和列 都可以用于解析 JSON 对象和字段。也可以从数据库列 解析 JSON 对象和数组。

* BLOB 支持，支持的类型列表位于 doc/[SqTable.cn.md](doc/SqTable.cn.md) 中。

* 可以为表和列添加注释（仅 MySQL / MariaDB 和 PostgreSQL 支持）。

* 自定义类型映射。

* 可以独立使用的查询生成器。见 doc/[SqQuery.cn.md](doc/SqQuery.cn.md)。

* 可以在低端硬件上工作。

* 单一头文件 〈 **sqxclib.h** 〉 (注意: 不包含特殊宏和支持库)

* 如果开发者不需要将迁移同步到数据库，可以使用 “无迁移模式”。

* 命令行工具可以生成迁移文件并执行迁移。见 doc/[SqApp.cn.md](doc/SqApp.cn.md)。

* 支持使用 cJSON 或 json-c 转换 JSON，开发者可以选择其中一个作为默认的 JSON 转换器。

* 支持 SQLite, MySQL / MariaDB, PostgreSQL。

* 提供项目模板。见目录 [project-template](project-template)。

## 数据库架构

定义映射到数据库表 "users" 的 C 结构化数据类型。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct  User    User;

struct User {
	int     id;          // 主键
	char   *name;
	char   *email;
	int     city_id;     // 外键

	time_t  created_at;
	time_t  updated_at;

#ifdef __cplusplus       // C++ 数据类型
	std::string       strCpp;
	std::vector<int>  intsCpp;
#endif
};
```

使用 C++ 方法在 schema_v1 中定义表和列 （动态）:  
您可以使用 Sq::Schema 的 create() 方法在架构中创建表。

```c++
/* 为 C++ STL 定义全局类型 */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

	/* 创建架构并指定版本号为 1 */
	schema_v1 = new Sq::Schema(1, "Ver 1");

	// 创建表 "users"，然后将列添加到表中。
	table = schema_v1->create<User>("users");

	// 主键 PRIMARY KEY
	table->integer("id", &User::id)->primary();
	// VARCHAR
	table->string("name", &User::name);
	// VARCHAR(60)
	table->string("email", &User::email, 60);
	// DEFAULT CURRENT_TIMESTAMP
	table->timestamp("created_at", &User::created_at)->useCurrent();
	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	table->timestamp("updated_at", &User::updated_at)->useCurrent()->useCurrentOnUpdate();
	// C++ 类型 - std::string 和 std::vector
	table->stdstring("strCpp", &User::strCpp);
	table->custom("intsCpp", &User::intsCpp, &SqTypeIntVector);
	// 外键 FOREIGN KEY
	table->integer("city_id", &User::city_id)->reference("cities", "id");
	// 约束外键 CONSTRAINT FOREIGN KEY
	table->foreign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");
	// 创建索引 CREATE INDEX
	table->index("users_id_index", "id");

	/* 如果您将当前时间存储在列和成员中并且它们使用默认名称 - 'created_at' 和 'updated_at',
	   您可以使用下面的行替换上述 2 个 timestamp() 方法。
	 */
//	table->timestamps<User>();
```

使用 C++ 方法更改 schema_v2 中的表和列 （动态）:  
您可以使用 alter() 方法来改变架构中的表。

```c++
	/* 创建架构并指定版本号为 2 */
	schema_v2 = new Sq::Schema(2, "Ver 2");

	// 更改表 "users"
	table = schema_v2->alter("users");

	// 向表中添加列
	table->integer("test_add", &User::test_add);
	// 更改表中的列
	table->integer("city_id", &User::city_id)->change();
	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	table->dropForeign("users_city_id_foreign");
	// 删除列
	table->dropColumn("name");
	// 重命名列
	table->renameColumn("email", "email2");
```

使用 C 函数在 schema_v1 中定义表和列 （动态）:  
您可以使用 sq_schema_create() 函数在架构中创建表。

```c
	/* 创建架构并指定版本号为 1 */
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// 创建表 "users"
	table = sq_schema_create(schema_v1, "users", User);

	// 主键 PRIMARY KEY
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	// VARCHAR
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	// VARCHAR(60)
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);

	// DEFAULT CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_use_current(column);

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "updated_at", offset(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);

	// 外键 FOREIGN KEY.                 注意：此处使用 NULL 终止的参数列表
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id", NULL);

	// 约束外键 CONSTRAINT FOREIGN KEY.  注意：此处使用 NULL 终止的参数列表
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id", NULL);
	sq_column_reference(column, "cities", "id", NULL);
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");

	// 创建索引 CREATE INDEX.            注意：此处使用 NULL 终止的参数列表
	column = sq_table_add_index(table, "users_id_index", "id", NULL);

	/* 如果您将当前时间存储在列和成员中并且它们使用默认名称 - 'created_at' 和 'updated_at',
	   您可以使用下面的行替换上述 2 个 sq_table_add_timestamp() 函数。
	 */
//	sq_table_add_timestamps_struct(table, User);
```

使用 C 函数更改 schema_v2 中的表和列 （动态）:  
您可以使用 sq_schema_alter() 函数来改变架构中的表。

```c
	/* 创建架构并指定版本号为 2 */
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// 更改表 "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// 将列添加到表中
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	// 更改表中的列
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_change(column);
	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	sq_table_drop_foreign(table, "users_city_id_foreign");
	// 删除列
	sq_table_drop_column(table, "name");
	// 重命名列
	sq_table_rename_column(table, "email", "email2");
```

**还有更多...**  
  
* 您可以在 doc/[database-migrations.cn.md](doc/database-migrations.cn.md) 中获得有关架构和迁移的更多信息
* 要使用初始化器定义（或更改）表，请参阅 doc/[schema-builder-constant.cn.md](doc/schema-builder-constant.cn.md)
* 要使用宏动态定义（或更改）表，请参阅 doc/[schema-builder-macro.cn.md](doc/schema-builder-macro.cn.md)

## 数据库产品

**Sqdb** 是 SQLite、MySQL 等数据库产品的基础结构。您可以在 doc/[Sqdb.cn.md](doc/Sqdb.cn.md) 中获得更多描述和示例。  
如果用户不需要将迁移同步到数据库，请在 SqdbConfig::bit_field 中设置 SQDB_CONFIG_NO_MIGRATION 以使用 “无迁移模式”。  
  
例如: 为 SQLite 数据库创建 Sqdb 实例  
  
当用户打开数据库时，SQLite 将在 Sq::DbConfigSqlite 指定的文件夹中打开/创建文件。  
在此示例中，数据库文件路径为 "/path/databaseName.db"。  
  
使用 C 函数创建 SQLite 数据库实例

```c
	// 数据库配置
	SqdbConfigSqlite  config = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION,
		.folder    = "/path",
		.extension = "db"
	};
	// 数据库实例的指针
	Sqdb  *db;

	db = sqdb_sqlite_new(&config);
//	db = sqdb_sqlite_new(NULL);                // 如果 config 为 NULL，则使用默认设置。
```

使用 C++ 方法创建 SQLite 数据库实例

```c++
	// 数据库配置
	Sq::DbConfigSqlite  config = {0};
//	config.bit_field = SQDB_CONFIG_NO_MIGRATION;
	config.folder    = "/path";
	config.extension = "db";
	// 数据库实例的指针
	Sq::DbMethod  *db;

	db = new Sq::DbSqlite(config);
//	db = new Sq::DbSqlite(NULL);    // 如果 config 为 NULL，则使用默认设置。
```

使用 C 函数创建 MySQL 数据库实例  
  
MySQL、PostgreSQL 必须在其 SqdbConfig 中指定主机、端口和身份验证等。

```c
	// 数据库配置
	SqdbConfigMysql  config = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION,
		.host = "localhost",
		.port = 3306,
		.user = "name",
		.password = "xxx"
	};
	// 数据库实例的指针
	Sqdb  *db;

	db = sqdb_mysql_new(&config);
//	db = sqdb_mysql_new(NULL);               // 如果 config 为 NULL，则使用默认设置。
```

## 打开数据库

要访问数据库，请创建 [SqStorage](doc/SqStorage.cn.md) 并指定数据库实例 [Sqdb](doc/Sqdb.cn.md)。  
  
使用 C 函数打开数据库

```c
	SqStorage *storage;

	storage = sq_storage_new(db);
	sq_storage_open(storage, "databaseName");
```

使用 C++ 方法打开数据库

```c++
	Sq::Storage *storage;

	storage = new Sq::Storage(db);
	storage->open("databaseName");
```

## 数据库迁移

要进行迁移，请使用 [SqStorage](doc/SqStorage.cn.md) 的迁移功能。它会检查架构版本来决定是否执行。  
您可以在 doc/[database-migrations.cn.md](doc/database-migrations.cn.md) 中获得有关迁移和架构的更多描述。  
  
使用 C 函数迁移架构并同步到数据库

```c
	// 迁移 'schema_v1' 和 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 SqStorage 中的架构并将架构同步到数据库（主要用于 SQLite）。
	sq_storage_migrate(storage, NULL);

	// 释放未使用的 'schema_v1' 和 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```

使用 C++ 方法迁移架构并同步到数据库

```c++
	// 迁移 'schema_v1' 和 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 SqStorage 中的架构并将架构同步到数据库（主要用于 SQLite）。
	storage->migrate(NULL);

	// 释放未使用的 'schema_v1' 和 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

如果你想使用像 Laravel 这样的单独迁移文件来执行此操作，此库为此提供了 [SqApp](doc/SqApp.cn.md)。  
[SqApp](doc/SqApp.cn.md) 将使用 Workspace/database/migrations 中的迁移文件。请参阅 doc/[SqApp.cn.md](doc/SqApp.cn.md) 以获取更多信息。

## 增删查改 CRUD

此库使用 [SqStorage](doc/SqStorage.cn.md) 在数据库中创建、读取、更新和删除行。这些函数可与 [SqQuery](doc/SqQuery.cn.md) 一起使用 （在下面的 "查询生成器" 中说明）。  
要获取更多信息和示例，您可以查看 doc/[SqStorage.cn.md](doc/SqStorage.cn.md)

#### 获取 Get

获取多行时用户可以指定返回数据的容器类型。如果您没有指定容器类型，getAll() 和 query() 将使用默认容器类型 - [SqPtrArray](doc/SqPtrArray.cn.md)。  
  
使用 C 函数  
  
容器类型指定为 NULL（使用默认容器类型）。

```c
	User       *user;
	SqPtrArray *array;

	// 获取多行
	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 8 AND id < 20");

	// 使用 SqQuery 获取多行 （在下面的 "查询生成器" 中说明）
	sq_query_where(query, "id", ">", "%d", 8);
	sq_query_where_raw(query, "id < %d", 20);
	array = sq_storage_get_all(storage, "users", NULL, NULL, query->c());

	// 获取所有行
	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);

	// 获取一行 (其中 id 等于 2)
	user  = sq_storage_get(storage, "users", NULL, 2);
```

使用 C++ 方法

```c++
	User         *user;
	Sq::PtrArray *array;

	// 获取多行
	array = storage->getAll("users", "WHERE id > 8 AND id < 20");

	// 使用 C++ 类 'where' 系列获取多行 （在下面的 "查询生成器" 中说明）
	array = storage->getAll("users", Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// 获取所有行
	array = storage->getAll("users");

	// 获取一行 (其中 id 等于 2)
	user  = storage->get("users", 2);
```

使用 C++ 模板函数  
  
容器类型指定为 std::vector<User>。

```c++
	User              *user;
	std::vector<User> *vector;

	// 获取多行
	vector = storage->getAll< std::vector<User> >("WHERE id > 8 AND id < 20");

	// 使用 C++ 类 'where' 系列获取多行 （在下面的 "查询生成器" 中说明）
	vector = storage->getAll< std::vector<User> >(Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// 获取所有行
	vector = storage->getAll< std::vector<User> >();

	// 获取一行 (其中 id 等于 2)
	user = storage->get<User>(2);
```

#### 插入 Insert

insert() 可以将一行插入到表中并返回插入的行 ID。它必须指定表名和结构实例。如果主键是自动增加的，则可以将其值设置为 0。  
  
使用 C 函数

```c
	User  user = {10, "Bob", "bob@server"};

	// 插入一行
	id = sq_storage_insert(storage, "users", NULL, &user);
```

使用 C++ 方法

```c++
	User  user = {10, "Bob", "bob@server"};

	// 插入一行
	id = storage->insert("users", &user);
```

使用 C++ 模板函数

```c++
	User  user = {10, "Bob", "bob@server"};

	// 插入一行
	storage->insert<User>(&user);
		// 或
	storage->insert(&user);
```

#### 更新 Update

updateAll() 用于修改表中的多条现有记录并返回更改的行数。它可以通过将列名附加到其参数来更新特定的列。  
updateField() 类似于 updateAll()。它可以通过将字段偏移量附加到其参数来更新特定列。  
  
使用 C 函数

```c
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// 更新一行
	n_changes = sq_storage_update(storage, "users", NULL, &user);

	// 更新特定列 - 多行中的 "name" 和 "email"。
	n_changes = sq_storage_update_all(storage, "users", NULL, &user, 
	                                  "WHERE id > 11 AND id < 28",
	                                  "name", "email",
	                                  NULL);

	// 更新特定字段 - 多行中的 User::name 和 User::email。
	n_changes = sq_storage_update_field(storage, "users", NULL, &user, 
	                                    "WHERE id > 11 AND id < 28",
	                                    offsetof(User, name),
	                                    offsetof(User, email),
	                                    -1);
```

使用 C++ 方法

```c++
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// 更新一行
	n_changes = storage->update("users", &user);

	// 更新特定列 - 多行中的 "name" 和 "email"。
	n_changes = storage->updateAll("users", &user,
	                               "WHERE id > 11 AND id < 28",
	                               "name", "email");

	// 更新特定字段 - 多行中的 User::name 和 User::email。
	n_changes = storage->updateField("users", &user,
	                                 "WHERE id > 11 AND id < 28",
	                                 &User::name, &User::email);
```

使用 C++ 模板函数

```c++
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// 更新一行
	n_changes = storage->update<User>(&user);
		// 或
	n_changes = storage->update(&user);

	// 更新特定列 - 多行中的 "name" 和 "email"。
	// 调用 updateAll<User>(...)
	n_changes = storage->updateAll(&user,
	                               "WHERE id > 11 AND id < 28",
	                               "name", "email");

	// 更新特定字段 - 多行中的 User::name 和 User::email。
	// 调用 updateField<User>(...)
	n_changes = storage->updateField(&user,
	                                 "WHERE id > 11 AND id < 28",
	                                 &User::name, &User::email);
```

#### 删除 Remove

remove() 可以删除表中的一行。  
removeAll() 可以根据条件删除多行。如果没有指定条件，则删除表中的所有行。  
  
使用 C 函数

```c
	// 删除一行 (其中 id 等于 5)
	sq_storage_remove(storage, "users", NULL, 5);

	// 删除多行
	sq_storage_remove_all(storage, "users", "WHERE id < 5");
```

使用 C++ 方法

```c++
	// 删除一行 (其中 id 等于 5)
	storage->remove("users", 5);

	// 删除多行
	storage->removeAll("users", "WHERE id < 5");
```

使用 C++ 模板函数

```c++
	// 删除一行 (其中 id 等于 5)
	storage->remove<User>(5);

	// 删除多行
	storage->removeAll<User>("WHERE id < 5");
```

## 使用原始字符串进行查询

sq_storage_query_raw() 使用原始字符串进行查询。程序必须指定数据类型和容器类型。

使用 C 函数

```c
	int  *p2integer;
	int   max_id;

	// 如果只查询 MAX(id)，会得到一个整数。
	// 因此指定表类型为 SQ_TYPE_INT，容器类型为 NULL。
	p2integer = sq_storage_query_raw(storage, "SELECT MAX(id) FROM table", SQ_TYPE_INT, NULL);
	// 返回整数指针
	max_id = *p2integer;
	// 不需要时释放整数指针。
	free(p2integer);

	// 如果只查询一行，则不需要容器。
	// 因此指定容器类型为 NULL。
	table = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id=1", tableType, NULL);
```

使用 C++ 方法

```c++
	int  *p2integer;
	int   max_id;

	// 如果只查询 MAX(id)，会得到一个整数。
	// 因此指定表类型为 SQ_TYPE_INT，容器类型为 NULL。
	p2integer = storage->query("SELECT MAX(id) FROM table", SQ_TYPE_INT, NULL);
	// 返回整数指针
	max_id = *p2integer;
	// 不需要时释放整数指针。
	free(p2integer);

	// 如果只查询一行，则不需要容器。
	// 因此指定容器类型为 NULL。
	table = storage->query("SELECT * FROM table WHERE id=1", tableType, NULL);
```

## 查询生成器

[SqQuery](doc/SqQuery.cn.md) 可以使用 C 函数或 C++ 方法生成 SQL 语句，并提供了支持 printf 格式的 where()、join()、on()、having() 系列函数。  
要获取更多信息和示例，您可以查看 doc/[SqQuery.cn.md](doc/SqQuery.cn.md)  
  
SQL 语句

```sql
	SELECT id, age
	FROM companies
	JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
	WHERE age > 5
```

使用 C++ 方法生成查询

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

使用 C 函数生成查询

* sq_query_join_sub() 是子查询的开始。
* sq_query_end_sub()  是子查询的结尾。
* sq_query_join()     在最后一个参数中传递 NULL 也是子查询的开始。

```c
	sq_query_select(query, "id", "age");
	sq_query_from(query, "companies");

	sq_query_join_sub(query);      // 子查询的开始
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "%d", 100);
	sq_query_end_sub(query);       // 子查询的结尾

	sq_query_as(query, "c");
	sq_query_on_raw(query, "c.id = companies.city_id");
	sq_query_where_raw(query, "age > %d", 5);
```

#### 将 SqQuery 与 SqStorage 一起使用

[SqStorage](doc/SqStorage.cn.md) 提供 sq_storage_query() 和 C++ 方法 query() 来处理查询。

```c++
	// C 函数
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ 方法
	array = storage->query(query);
```

[SqQuery](doc/SqQuery.cn.md) 提供 sq_query_c() 或 C++ 方法 c() 来为 [SqStorage](doc/SqStorage.cn.md) 生成 SQL 语句。  
  
使用 C 函数

```c
	// SQL 语句排除 "SELECT * FROM ..."
	sq_query_clear(query);
	sq_query_where(query, "id", ">", "%d", 10);
	sq_query_or_where_raw(query, "city_id < %d", 22);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

使用 C++ 方法

```c++
	// SQL 语句排除 "SELECT * FROM ..."
	query->clear()
	     ->where("id", ">", 10)
	     ->orWhereRaw("city_id < %d", 22);

	array = storage->getAll("users", query->c());

	// getAll() 重载函数可以直接传递 'query'
//	array = storage->getAll("users", query);
```

**方便的 C++ 类 'where' 系列**  
  
使用 Sq::Where（或 Sq::where）的 operator()

```c++
	Sq::Where  where;

	array = storage->getAll("users",
			where("id", ">", 10).orWhereRaw("city_id < %d", 22));
```

使用 Sq::where 的构造函数和运算符

```c++
	// 使用参数包构造函数
	array = storage->getAll("users",
			Sq::where("id", ">", 10).orWhereRaw("city_id < %d", 22));

	// 使用默认构造函数和 operator()
	array = storage->getAll("users",
			Sq::where()("id", ">", 10).orWhereRaw("city_id < %d", 22));
```

下面是目前提供的方便的 C++ 类：

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

**方便的 C++ 类 'select' 和 'from'**

使用 C++ Sq::select 或 Sq::from 来运行数据库查询。

```c++
	// 将 Sq::select 与 query 方法一起使用
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));

	// 将 Sq::from 与 query 方法一起使用
	array = storage->query(Sq::from("users").whereRaw("city_id > %d", 5));
```

## JOIN 支持

[SqTypeJoint](doc/SqTypeJoint.cn.md) 是处理多表连接查询的默认类型。它可以为查询结果创建指针数组。  
  
例如: 从连接多表的查询中获取结果。  
  
使用 C 函数

```c
	sq_query_from(query, "cities");
	sq_query_join(query, "users", "cities.id", "=", "%s", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);

	for (unsigned int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);

		// 因为 SqPtrArray 默认不释放元素，所以在释放数组之前先释放元素。
//		free(element);
	}
```

使用 C++ 方法

```c++
	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	Sq::PtrArray *array = (Sq::PtrArray*) storage->query(query);

	for (unsigned int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")

		// 因为 Sq::PtrArray 默认不释放元素，所以在释放数组之前先释放元素。
//		free(element);
	}
```

使用 C++ STL  
  
用户可以将指向指针的指针（double pointer）指定为 STL 容器的元素。

```c++
	std::vector<void**> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector<void**> >(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		void **element = vector->at(index);
		city = (City*)element[0];      // from("cities")
		user = (User*)element[1];      // join("users")
	}
```

如果你不想使用指针作为容器的元素：
1. 使用 Sq::Joint 作为 C++ STL 容器的元素。
2. 使用 typedef 为 C 语言定义 [SqArray](doc/SqArray.cn.md) 的元素类型。

参考 [SqTypeJoint](doc/SqTypeJoint.cn.md) 以获取更多信息。  
以下是 C++ STL 示例之一：

```c++
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector< Sq::Joint<2> > >(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Joint<2> &joint = vector->at(index);
		city = (City*)joint[0];      // from("cities")
		user = (User*)joint[1];      // join("users")
	}
```

## 解析未知结果

[SqTypeRow](doc/SqTypeRow.cn.md) 派生自 [SqTypeJoint](doc/SqTypeJoint.cn.md)。它创建 [SqRow](doc/SqRow.cn.md) 的实例并解析未知（或已知）的结果。  
SQ_TYPE_ROW 是 SqTypeRow 的内置静态常量类型。[SqTypeRow](doc/SqTypeRow.cn.md) 和 SQ_TYPE_ROW 都在 sqxcsupport 库中 (sqxcsupport.h)。  
  
使用 C 函数

```c
	SqRow      *row;
	SqPtrArray *array;

	// 指定表类型为 SQ_TYPE_ROW
	// 指定返回数据的容器类型为 SQ_TYPE_PTR_ARRAY
	array = sq_storage_query(storage, query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	array = sq_storage_get_all(storage, "users", SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY, NULL);

	// 指定表类型为 SQ_TYPE_ROW
	row = sq_storage_get(storage, "users", SQ_TYPE_ROW, 11);
```

使用 C++ 方法

```c++
	Sq::Row      *row;
	Sq::PtrArray *array;

	// 指定表类型为 SQ_TYPE_ROW
	// 指定返回数据的容器类型为 SQ_TYPE_PTR_ARRAY
	array = (Sq::PtrArray*) storage->query(query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	array = (Sq::PtrArray*) storage->getAll("users", SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY, NULL);

	// 指定表类型为 SQ_TYPE_ROW
	row = (Sq::Row*) storage->get("users", SQ_TYPE_ROW, 11);
```

使用 C++ STL

```c++
	Sq::Row               *row;
	std::vector<Sq::Row*> *rowVector;

	// 指定表类型为 SQ_TYPE_ROW
	// 指定返回数据的容器类型为 std::vector<Sq::Row*>
	rowVector = storage->query< std::vector<Sq::Row*> >(query, SQ_TYPE_ROW);

	rowVector = storage->getAll< std::vector<Sq::Row*> >("users", SQ_TYPE_ROW, NULL);

	// 获取第一行
	row = rowVector->at(0);
```

[SqRow](doc/SqRow.cn.md) 包含 2 个数组。一个是列数组，另一个是数据数组。

```c
	// 第一列的名称
	char   *columnName = row->cols[0].name;

	// 第一列的数据类型 (在此示例中，columnType 等于 SQ_TYPE_STR)
	SqType *columnType = row->cols[0].type;

	// 第一列的值 (如果 columnType 等于 SQ_TYPE_STR)
	char   *columnValue = row->data[0].str;
```

## 交易 Transaction

	beginTrans()：   开始新交易。
	commitTrans()：  保存当前交易期间所做的任何更改并结束交易。
	rollbackTrans()：取消当前交易期间所做的任何更改并结束交易。

使用 C 函数

```c
	sq_storage_begin_trans(storage);

	// 在这里对数据库做一些事情...

	if (abort)
		sq_storage_rollback_trans(storage);
	else
		sq_storage_commit_trans(storage);
```

使用 C++ 方法

```c++
	storage->beginTrans();

	// 在这里对数据库做一些事情...

	if (abort)
		storage->rollbackTrans();
	else
		storage->commitTrans();
```

## 配置头文件

更改构建配置。  
  
sqxclib 在搜索和排序数据库列名和 JSON 字段名时默认区分大小写。用户可以在 sqxc/[SqConfig.h](sqxc/SqConfig.h) 中更改。

```c
// SqConfig.h 中的常用设置

/* sqxclib 在搜索和排序数据库列名和 JSON 字段名时默认区分大小写。
   某些旧的数据库产品可能需要禁用此功能。
   受影响的源代码 : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE        1

/* 如果用户没有指定 SQL 字符串长度，程序将使用默认值。
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191
```

## JSON 支持
- 此库使用 [json-c](https://github.com/json-c/json-c) 来解析/写入 JSON。
- 所有定义的表和列都可以用来解析 JSON 对象和字段。
- 程序还可以解析存储在列中的 JSON 对象和数组。

## Sqxc
Sqxc 用于数据解析和写入。  
用户可以链接多个 Sqxc 元素来转换不同类型的数据。  
您可以在 doc/[Sqxc.cn.md](doc/Sqxc.cn.md) 中获得更多描述和示例。

## SqType
它定义了如何初始化、终结和转换 C 数据类型。  
*Sqxc* 使用它在 C 语言和 SQL、JSON 等之间转换数据。  
您可以在 doc/[SqType.cn.md](doc/SqType.cn.md) 中获得更多描述和示例。

## SqSchema
SqSchema 定义数据库架构。它存储表和表的更改记录。  
您可以在 doc/[SqSchema.cn.md](doc/SqSchema.cn.md) 中获得更多描述和示例。

## SqApp
SqApp 使用配置文件（SqApp-config.h）来初始化数据库并为用户的应用程序进行迁移。  
它提供命令行程序来生成迁移并进行迁移。  
请参阅文档 doc/[SqApp.cn.md](doc/SqApp.cn.md)。

## SqConsole
SqConsole 提供命令行界面（主要用于 SqAppTool）。  
请参阅文档 doc/[SqConsole.cn.md](doc/SqConsole.cn.md)。

## sqxc 怎么念

sqxc 中文发音「思库可思」，还可以翻译成白话文。  
备注：中文发音是在 2022年5月14日 凌晨4点左右决定的。

## 许可

sqxclib 在 木兰宽松许可证 第2版 下获得许可。


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

