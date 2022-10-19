[English](SqStorage.md)

# SqStorage

SqStorage 访问数据库。它使用 Sqxc 在 C 语言和 Sqdb 接口之间转换数据。

## 创建 storage

用户在创建 SqStorage 时必须指定 [Sqdb](Sqdb.cn.md) 实例。  
  
使用 C 语言

```c
	Sqdb      *db;
	SqStorage *storage;

	db      = sqdb_new(SQDB_INFO_SQLITE, NULL);
	storage = sq_storage_new(db);
```

使用 C++ 语言

```c++
	Sq::DbSqlite *db;
	Sq::Storage  *storage;

	db      = new Sq::DbSqlite();
	storage = new Sq::Storage(db);
```

## 访问数据库

SqStorage 使用 [Sqdb](Sqdb.cn.md) 访问数据库。  
  
为 SQL 表 "users" 定义数据结构 'User'。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct User    User;

struct User {
	int   id;
	char *name;
	char *email;
};
```

## 打开数据库并进行迁移

使用 C 函数

```c
	// 打开数据库 "sqxc_local"
	sq_storage_open(storage, "sqxc_local");

	// 在架构中创建表 "users"
	table = sq_schema_create(schema, "users", User);
	column = sq_table_int(table, "id", offsetof(User, id));
	sq_column_primary(column);
	column = sq_table_string(table, "name", offsetof(User, name), -1);
	column = sq_table_string(table, "email", offsetof(User, email), -1);

	// 迁移架构
	sq_storage_migrate(storage, schema);

	// 将架构同步到数据库并更新 'storage' 中的架构
	// 这主要由 SQLite 使用
	sq_storage_migrate(storage, NULL);
```

使用 C++ 方法

```c
	// 打开数据库 "sqxc_local"
	storage->open("sqxc_local");

	// 在架构中创建表 "users"
	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary()->autoIncrement();  // 主键
	table->string("name", &User::name);
	table->string("email", &User::email);

	// 迁移架构
	storage->migrate(schema);

	// 将架构同步到数据库并更新 'storage' 中的架构
	// 这主要由 SQLite 使用
	storage->migrate(NULL);
```

注意1: 迁移后不要重复使用 'schema_next'，因为数据会从 'schema_next' 移动到 'schema_current'。  
注意2: 如果使用 SQLite，迁移后必须将架构同步到数据库。  

## 使用用户定义的数据类型访问数据库

以下 C 函数和 C++ 方法可以返回用户定义数据类型 ( [SqType](SqType.cn.md) ) 的实例：

| C 函数                    | C++ 方法      |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |

如果用户同时指定 'table_name' 和 'table_type'，下面的函数可以运行得更快一些。

| C 函数                    | C++ 方法      |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |
| sq_storage_insert()       | insert()      |
| sq_storage_update()       | update()      |
| sq_storage_update_all()   | updateAll()   |
| sq_storage_update_field() | updateField() |

注意: 如果用户未指定对象类型，SqStorage 将尝试查找匹配的类型。  
注意: 如果用户未指定容器类型，SqStorage 将使用默认容器类型。  

## get

例如: 从数据库表 "users" 中获取一行。

```sql
SELECT * FROM users WHERE id = 3
```

使用 C 语言

```c
	User *user;

	user = sq_storage_get(storage, "users", NULL, 3);
```

使用 C++ 语言

```c++
	User *user;

	user = storage->get("users", 3);
	// or
	user = storage->get<User>(3);
```

## getAll

例如: 从数据库表 "users" 中获取所有行。

```sql
SELECT * FROM users
```

使用 C 语言

```c
	SqPtrArray *array;

	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);
```

使用 C++ 语言

```c++
	Sq::PtrArray *array;

	array = storage->getAll("users");
```

使用 C++ 标准模板库 (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>();
```

## getAll (Where 条件)

sq_storage_get_all() 中的最后一个参数是排除 "SELECT * FROM table_name" 的 SQL 语句。  
  
例如: 从具有 where 条件的数据库表 "users" 中获取多行。

```sql
SELECT * FROM users WHERE id > 10 AND id < 99
```

使用 C 语言

```c
	SqPtrArray *array;

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           "WHERE id > 10 AND id < 99");
```

使用 C++ 语言

```c++
	Sq::PtrArray *array;

	array = storage->getAll("users",
	                        "WHERE id > 10 AND id < 99");
	// 或指定容器类型
	array = storage->getAll<User>(SQ_TYPE_PTR_ARRAY,
	                              "WHERE id > 10 AND id < 99");
```

使用 C++ 标准模板库 (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>("WHERE id > 10 AND id < 99");
```

## getAll (配合 SqQuery)

SqQuery 可以生成排除 "SELECT * FROM table_name" 的 SQL 语句  
  
使用 C 语言

```c
	SqQuery *query = sq_query_new(NULL);
	sq_query_where_raw(query, "id > 10");
	sq_query_where(query, "id", "<", "%d", 99);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

使用 C++ 语言

```c++
	Sq::Query *query = new Sq::Query();
	query->whereRaw("id > 10")
	     ->where("id", "<", "%d", 99);

	array = storage->getAll("users", query->c());
```

#### 方便的 C++ 类 'where'

使用 C++ Sq::where 和 Sq::whereRaw 生成 SQL 语句

```c++
	array = storage->getAll("users",
			Sq::whereRaw("id > 10").where("id", "<", "%d", 99));
```

## insert

sq_storage_insert() 用于在表中插入一个新记录并返回插入的行 ID。  
  
使用 C 函数

```c
	User  user;
	int   inserted_id;

	user.id   = 0;       // 主键设置为 0 用于自动递增
	user.name = "xman";
	inserted_id = sq_storage_insert(storage, "users", NULL, &user);
```

使用 C++ 方法

```c++
	User  user;
	int   inserted_id;

	user.id   = 0;       // 主键设置为 0 用于自动递增
	user.name = "xman";
	inserted_id = storage->insert("users", &user);
	// 或调用模板函数： insert<User>(...)
	inserted_id = storage->insert(user);
```

## update

sq_storage_update() 用于修改表中的一个现有记录并返回更改的行数。  
  
使用 C 函数

```c
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yael";
	n_changes = sq_storage_update(storage, "users", NULL, &user);
```

使用 C++ 方法

```c++
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yael";
	n_changes = storage->update("users", &user);
	// 或调用模板函数： update<User>(...)
	n_changes = storage->update(user);
```

## updateAll (Where 条件)

sq_storage_update_all() 用于修改表中的多条现有记录并返回更改的行数。  
参数 'SQL 语句' 必须排除 "UPDATE table_name SET column=value"。然后追加列名列表，最后一个参数必须为 NULL。  
  
注意: SqQuery 可以生成排除 "UPDATE table_name SET column=value" 的 SQL 语句。请参阅上面的 "getAll (配合 SqQuery)"。  
  
例如: 更新行中的特定列。

```sql
UPDATE "users" SET "name"='yael',"email"='user@server' WHERE id > 10
```

使用 C 函数

```c
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = sq_storage_update_all(storage, "users", NULL, &user,
	                                   "WHERE id > 10",
	                                   "name", "email",
	                                   NULL);
```

因为 C++ 方法 updateAll() 使用参数包，所以最后一个参数可以传递（或不传递）NULL。  
  
使用 C++ 方法

```c++
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = storage->updateAll("users", &user,
	                                "WHERE id > 10",
	                                "name", "email");
	// 或调用模板函数： updateAll<User>(...)
	n_changes  = storage->updateAll(user,
	                                "WHERE id > 10",
	                                "name", "email");
```

## updateField (Where 条件)

sq_storage_update_field() 类似于 sq_storage_update_all()。用户必须在参数 'SQL 语句' 之后附加 field_offset 列表，最后一个参数必须为 -1。  
  
使用 C 函数

```c
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = sq_storage_update_field(storage, "users", NULL, &user,
	                                     "WHERE id > 10",
	                                     offsetof(User, name),
	                                     offsetof(User, email),
	                                     -1);
```

因为 C++ 方法 updateField() 使用参数包，所以最后一个参数可以传递（或不传递）-1。  
  
使用 C++ 方法

```c++
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = storage->updateField("users", &user,
	                                  "WHERE id > 10",
	                                  &User::name,
	                                  &User::email);
	// 或调用模板函数： updateField<User>(...)
	n_changes  = storage->updateField(user,
	                                  "WHERE id > 10",
	                                  &User::name,
	                                  &User::email);
```

## remove

sq_storage_remove() 用于删除表中的一个现有记录。  
  
例如: 从数据库表 "users" 中删除一行。

```sql
DELETE FROM users WHERE id = 3
```

使用 C 函数

```c
	sq_storage_remove(storage, "users", NULL, 3);
```

使用 C++ 方法

```c++
	storage->remove("users", 3);
	// or
	storage->remove<User>(3);
```

## removeAll

例如: 从数据库表 "users" 中删除所有行。

```sql
DELETE FROM users
```

使用 C 函数

```c
	sq_storage_remove_all(storage, "users", NULL);
```

使用 C++ 方法

```c++
	storage->removeAll("users");
	// 或
	storage->removeAll<User>();
```

## removeAll (Where 条件)

sq_storage_remove_all() 用于删除表中的多条现有记录。  
最后一个参数是排除 "DELETE FROM table_name" 的 SQL 语句。  
  
注意: SqQuery 可以生成排除 "DELETE FROM table_name" 的 SQL 语句。 请参阅上面的 "getAll (配合 SqQuery)"。  
  
例如: 使用 where 条件从数据库表 "users" 中删除多行。

```sql
DELETE FROM users WHERE id > 50
```

使用 C 函数

```c
	sq_storage_remove_all(storage, "users", "WHERE id > 50");
```

使用 C++ 方法

```c++
	storage->removeAll("users", "WHERE id > 50");
	// or
	storage->removeAll<User>("WHERE id > 50");
```

## 运行自定义查询 (使用 SqQuery)

SqStorage 提供 sq_storage_query() 和 C++ 方法 query() 来运行数据库查询。  
  
使用 C 函数

```c
	SqType *userType;
	SqType *containerType;

	// 查找匹配类型并使用默认容器
	array = sq_storage_query(storage, query, NULL, NULL);

	// 返回用户定义的数据类型
	container = sq_storage_query(storage, query, userType, containerType);
```

使用 C++ 方法

```c++
	// 查找匹配类型并使用默认容器
	array = storage->query(query);

	// 返回用户定义的数据类型
	container = storage->query(query, userType, containerType);
```
