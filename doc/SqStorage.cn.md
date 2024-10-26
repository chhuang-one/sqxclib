[English](SqStorage.md)

# SqStorage

SqStorage 使用 [Sqdb](Sqdb.cn.md) 访问数据库。它使用 [Sqxc](Sqxc.cn.md) 在 C 语言和 [Sqdb](Sqdb.cn.md) 实例之间转换数据。

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

## 打开数据库

打开数据库后，您可以从 SqStorage::db.version 获取数据库中当前的架构版本。  
  
使用 C 语言

```c
	// 打开数据库 "local"
	sq_storage_open(storage, "local");

	// 获取数据库 "local" 中的当前架构版本。
	int  schemaVersion = storage->db->version;
```

使用 C++ 语言

```c
	// 打开数据库 "local"
	storage->open("local");

	// 获取数据库 "local" 中的当前架构版本。
	int  schemaVersion = storage->db->version;
```

## 进行迁移

为数据库表 "users" 定义数据结构 'User'。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct User    User;

struct User {
	int   id;
	char *name;
	char *email;
};
```

C 函数 sq_storage_migrate() 和 C++ 方法 migrate() 使用架构的版本来决定是否迁移。它们的 'schema' 参数是下一个版本的架构，'schema' 的更改将应用​​于 SqStorage::schema。  
  
要通知数据库实例迁移已完成，请调用 sq_storage_migrate() 并在最后一个参数中传入 NULL。这将清除未使用的数据、对表和列进行排序，并将当前架构同步到数据库（主要用于 SQLite）。  
  
注意: 迁移后不要重复使用传递给 migrate() 的 'schema'，因为数据会从 'schema' 移动到 SqStorage::schema。  
  
使用 C 语言

```c
	// 在架构中创建表 "users"
	table = sq_schema_create(schema, "users", User);

	column = sq_table_add_int(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);

	// 迁移架构
	sq_storage_migrate(storage, schema);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 SqStorage 中的架构并将架构同步到数据库（主要用于 SQLite）。
	sq_storage_migrate(storage, NULL);
```

使用 C++ 语言

```c
	// 在架构中创建表 "users"
	table = schema->create<User>("users");

	table->integer("id", &User::id)->primary()->autoIncrement();  // 主键
	table->string("name", &User::name);
	table->string("email", &User::email);

	// 迁移架构
	storage->migrate(schema);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 SqStorage 中的架构并将架构同步到数据库（主要用于 SQLite）。
	storage->migrate(NULL);
```

* 您可以在 [database-migrations.cn.md](database-migrations.cn.md) 中获得有关架构和迁移的更多信息
* 要使用初始化器定义（或更改）表，请参阅 [schema-builder-constant.cn.md](schema-builder-constant.cn.md)

## 获取 get

get() 的参数是 表名、表类型 和 id。  
如果表类型为 NULL，SqStorage 将尝试在其架构中查找表类型。  
如果用户同时指定参数 表名 和 表类型，它可以运行得更快一些。  
  
例如: 从数据库表 users 中获取一行。

```sql
SELECT * FROM users WHERE id = 3
```

使用 C 语言  
  
表类型指定为 NULL（在其架构中查找表类型）。

```c
	User *user;

	user = sq_storage_get(storage, "users", NULL, 3);
```

使用 C++ 语言

```c++
	User *user;

	user = storage->get("users", 3);
		// 或
	user = storage->get<User>(3);
```

## 获取所有列 getAll

getAll() 的参数是 表名、表类型、容器类型 和 SQL where 条件。它可以指定返回数据的 表类型 和 容器类型。  
如果程序没有指定容器类型，getAll() 将使用默认的容器类型 [SqPtrArray](SqPtrArray.cn.md)。  
例如: 从数据库表 "users" 中获取所有行。

```sql
SELECT * FROM users
```

使用 C 语言  
  
表类型指定为 NULL（在其架构中查找表类型）。  
容器类型指定为 NULL（使用默认容器类型）。  

```c
	SqPtrArray *array;
	const char *where = NULL;         // SQL WHERE 子句

	array = sq_storage_get_all(storage, "users", NULL, NULL, where);
```

使用 C++ 语言

```c++
	Sq::PtrArray *array;
	const char   *where = NULL;       // SQL WHERE 子句

	array = storage->getAll("users");
		// 或
//	array = storage->getAll("users", NULL, NULL, where);
```

**指定容器类型**  
  
如果不想使用指针数组，可以指定其他容器类型。  
  
例如: 容器类型指定为 SQ_TYPE_ARRAY（参见 [SqArray](SqArray.cn.md)）。

```c++
	SqArray    *array;
	const char *where = NULL;         // SQL WHERE 子句

	// C 函数
	array = sq_storage_get_all(storage, "users", NULL, SQ_TYPE_ARRAY, where);

	// C++ 方法
	array = storage->getAll("users", NULL, SQ_TYPE_ARRAY, where);
```

使用 C++ 标准模板库 (STL)  
  
容器类型指定为 std::list<User>。

```c++
	std::list<User> *list;
	const char      *where = NULL;    // SQL WHERE 子句

	list = storage->getAll< std::list<User> >();
		// 或
//	list = storage->getAll< std::list<User> >("users", NULL, where);
```

## 获取所有列 getAll (Where 条件)

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

	list = storage->getAll< std::list<User> >("WHERE id > 10 AND id < 99");
```

## 获取所有列 getAll (配合 SqQuery)

[SqQuery](SqQuery.cn.md) 可以生成排除 "SELECT * FROM table_name" 的 SQL 语句  
  
使用 C 语言

```c
	SqQuery *query = sq_query_new(NULL);
	sq_query_where_raw(query, "id > %d", 10);
	sq_query_where(query, "id", "<", "%d", 99);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

使用 C++ 语言

```c++
	Sq::Query *query = new Sq::Query();
	query->whereRaw("id > %d", 10)
	     ->where("id", "<", 99);

	array = storage->getAll("users", query);
```

#### 方便的 C++ 类 'where'

使用 C++ Sq::where 和 Sq::whereRaw 生成 SQL 语句

```c++
	array = storage->getAll("users",
			Sq::whereRaw("id > 10").where("id", "<", 99));
```

## 插入 insert

sq_storage_insert() 用于在表中插入一个新记录并返回插入的行 ID。如果主键是自动增加的，则可以将其值设置为 0。  
  
使用 C 函数

```c
	User  user;
	int   row_id;

	user.id   = 0;       // 主键设置为 0 用于自动递增
	user.name = "xman";

	row_id = sq_storage_insert(storage, "users", NULL, &user);
```

使用 C++ 方法

```c++
	User  user;
	int   row_id;

	user.id   = 0;       // 主键设置为 0 用于自动递增
	user.name = "xman";

	row_id = storage->insert("users", &user);
	// 或调用模板函数： insert<User>(...)
	row_id = storage->insert(user);
```

## 更新 update

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

## 更新所有列 updateAll (Where 条件)

sq_storage_update_all() 用于修改表中的多条现有记录并返回更改的行数。它可以通过将列名附加到其参数来更新特定列，但最后一个参数必须为 NULL。  
参数 'SQL 语句' 必须排除 "UPDATE table_name SET column=value"。  
  
注意: [SqQuery](SqQuery.cn.md) 可以生成排除 "UPDATE table_name SET column=value" 的 SQL 语句。请参阅上面的 "getAll (配合 SqQuery)"。  
  
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

	// 或与 SqQuery 一起使用
	sq_query_where(query, "id", ">", 10);
	n_changes  = sq_storage_update_all(storage, "users", NULL, &user,
	                                   sq_query_c(query),
	                                   "name", "email",
	                                   NULL);
```

使用 C++ 方法  
  
因为 C++ 方法 updateAll() 使用参数包，所以最后一个参数可以传递（或不传递）NULL。  

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

	// 或用方便的 C++ 类 'where'
	n_changes  = storage->updateAll(user,
	                                Sq::where("id", ">", 10),
	                                "name", "email");
```

## 更新字段 updateField (Where 条件)

sq_storage_update_field() 类似于 sq_storage_update_all()。它可以通过将字段偏移量附加到其参数来更新特定列，但最后一个参数必须为 -1。  
  
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

使用 C++ 方法  
  
因为 C++ 方法 updateField() 使用参数包，所以最后一个参数可以传递（或不传递）-1。  

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

	// 或用方便的 C++ 类 'where'
	n_changes  = storage->updateField("users", &user,
	                                  Sq::where("id", ">", 10),
	                                  &User::name,
	                                  &User::email);
```

## 删除 remove

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
		// 或
	storage->remove<User>(3);
```

## 删除所有列 removeAll

如果最后一个参数为 NULL，sq_storage_remove_all() 将删除表中的所有行。  
  
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

## 删除所有列 removeAll (Where 条件)

sq_storage_remove_all() 可以根据条件删除表中多个现有记录。如果不指定条件，则删除表中的所有行。  
最后一个参数是排除 "DELETE FROM table_name" 的 SQL 语句。  
  
注意: [SqQuery](SqQuery.cn.md) 可以生成排除 "DELETE FROM table_name" 的 SQL 语句。请参阅上面的 "getAll (配合 SqQuery)"。  
  
例如: 使用 where 条件从数据库表 "users" 中删除多行。

```sql
DELETE FROM users WHERE id > 50
```

使用 C 函数

```c
	sq_storage_remove_all(storage, "users", "WHERE id > 50");
		// 或与 SqQuery 一起使用
	sq_query_where_raw(query, "WHERE id > %d", 50);
	sq_storage_remove_all(storage, "users", sq_query_c(query));
```

使用 C++ 方法

```c++
	storage->removeAll("users", "WHERE id > 50");
		// 或
	storage->removeAll<User>("WHERE id > 50");
		// 或用方便的 C++ 类 'where'
	storage->removeAll("users", Sq::where("id", ">", 50));
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

## 自定义查询

SqStorage 提供 sq_storage_query() 和 C++ 方法 query() 来使用 [SqQuery](SqQuery.cn.md) 进行查询。和 getAll() 一样，如果程序没有指定容器类型，它们将使用默认容器类型 [SqPtrArray](SqPtrArray.cn.md)。  

#### 没有 JOIN 子句的查询

在这种情况下，表类型和容器类型的用法与 getAll() 基本相同。  
  
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

#### 使用 JOIN 子句的查询

如果用户在没有指定表类型的情况下执行连接多个表的查询，程序将默认使用 [SqTypeJoint](SqTypeJoint.cn.md) 作为表类型。[SqTypeJoint](SqTypeJoint.cn.md) 可以为查询结果创建指针数组。  
  
使用 C 函数

```c
	sq_query_from(query, "cities");
	sq_query_join(query, "users", "cities.id", "=", "%s", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);

	for (unsigned int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);
		// 在释放 'array' 之前释放 'element'
		// free(element);
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
		// 在释放 'array' 之前释放 'element'
		// free(element);
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

如果你不想使用指针作为 C++ STL 容器的元素，你可以使用 Sq::Joint 来代替它。  
Sq::Joint 只是将指针数组包装到结构中。因为 C++ STL 不能直接将数组用作容器的元素，所以用户必须将它与 C++ STL 一起使用。

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

#### 方便的 C++ 类 'select' 和 'from'

使用 C++ Sq::select 或 Sq::from 来运行数据库查询。

```c++
	// 将 Sq::select 与 query 方法一起使用
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));

	// 将 Sq::from 与 query 方法一起使用
	array = storage->query(Sq::from("users").whereRaw("city_id > 5"));
```

#### 使用原始字符串进行查询

SqStorage 提供 sq_storage_query_raw() 来使用原始字符串进行查询。与 getAll() 不同，程序必须指定数据类型和容器类型，因为当容器类型为 NULL 时程序不会使用默认容器类型。  

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
	table = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id = 1", tableType, NULL);

	// 如果查询多行，则需要容器。
	container = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id < 25", tableType, containerType);
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
	table = storage->query("SELECT * FROM table WHERE id = 1", tableType, NULL);

	// 如果查询多行，则需要容器。
	container = storage->query("SELECT * FROM table WHERE id < 25", tableType, containerType);
```

## 使用自定义数据类型

下面的 C 函数和 C++ 方法可以返回自定义数据类型和容器类型的实例：  
请参阅文档 [SqType](SqType.cn.md) 以了解如何自定义类型。

| C 函数                    | C++ 方法      |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |
| sq_storage_query_raw()    | query()       |

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

注意: 如果用户未指定对象类型，SqStorage 将尝试查找匹配的类型。（sq_storage_query_raw 除外）  
注意: 如果用户未指定容器类型，SqStorage 将使用默认容器类型。（sq_storage_query_raw 除外）  

使用 C 函数  
  
SQ_TYPE_ROW       是内置的类型，位于 sqxcsupport 库 (sqxcsupport.h) 中。  
SQ_TYPE_PTR_ARRAY 是内置的容器类型。  

```c
	SqType     *rowType   = SQ_TYPE_ROW;
	SqRow      *row;
	SqType     *arrayType = SQ_TYPE_PTR_ARRAY;
	SqPtrArray *array;

	row = sq_stoarge_get(storage, "users", rowType, 10);

	array = sq_storage_get_all(storage, "users", rowType, arrayType, NULL);
	for (unsigned int i = 0;  i < array->length;  i++) {
		row = array->data[i];
		// 在这里做点什么
	}
```

使用 C++ 方法

```c++
	Sq::Type     *rowType   = SQ_TYPE_ROW;
	Sq::Row      *row;
	Sq::Type     *arrayType = SQ_TYPE_PTR_ARRAY;
	Sq::PtrArray *array;

	row = (Sq::Row*)storage->get("users", rowType, 10);

	array = (Sq::PtrArray*)storage->getAll("users", rowType, arrayType, NULL);
	for (unsigned int i = 0;  i < array->length;  i++) {
		row = array->data[i];
		// 在这里做点什么
	}
```
