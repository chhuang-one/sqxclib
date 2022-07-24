[English](README.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib 是在 C 语言和 SQL（或 JSON ...等）之间转换数据的库。它提供 ORM 的功能和 C++ 包装器 (C++ wrapper)  
项目地址: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## 目前的功能:
1. 用户可以使用 C99 指定初始化(designated initializer) 或 C++ 聚合初始化(aggregate initialization) 静态定义 SQL 表、列、迁移，
   这可以减少制作架构时的运行时间，请参阅 doc/[schema-builder-static.cn.md](doc/schema-builder-static.cn.md)。
   当然也可以使用 C 函数 或 C++ 方法 动态执行这些操作。

2. 所有定义的 SQL表和列 都可以用于解析 JSON 对象和字段。也可以从 SQL 列 解析 JSON 对象和数组。

3. 可以在低端硬件上工作。

4. 单一头文件 〈 **sqxclib.h** 〉 (注意：不包含特殊宏和支持库)

5. 命令行工具可以生成迁移并进行迁移。 见 doc/[SqApp.cn.md](doc/SqApp.cn.md)。

6. 支持 SQLite, MySQL / MariaDB。

7. 提供项目模板。 见目录 [project-template](project-template)。

## 数据库架构

定义映射到数据库表 "users" 的 C 结构化数据类型。

```c++
typedef struct  User    User;    // 如果您使用 C 语言，请添加此行

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

使用 C++ 方法在 schema_v1 中定义表和列 （动态）

```c++
/* 为 C++ STL 定义全局类型 */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

	/* 创建架构版本 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // 指定版本号或自动生成

	// 创建表 "users"
	table = schema_v1->create<User>("users");
	// 向表中添加列
	table->integer("id", &User::id)->primary();  // PRIMARY KEY
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	// DEFAULT CURRENT_TIMESTAMP
	table->timestamp("created_at", &User::created_at)->useCurrent();
	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	table->timestamp("updated_at", &User::updated_at)->useCurrent()->useCurrentOnUpdate();
	// C++ types - std::string and std::vector
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

使用 C++ 方法更改 schema_v2 中的表和列 （动态）

```c++
	/* 创建架构版本 2 */
	schema_v2 = new Sq::Schema("Ver 2");
	schema_v2->version = 2;    // 指定版本号或自动生成

	// 更改表 "users"
	table = schema_v2->alter("users");
	// 向表中添加列
	table->integer("test_add", &User::test_add);
	// 更改表中的列
	table->integer("city_id", &User::city_id)->change();
	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	table->dropForeign("users_city_id_foreign");

	table->dropColumn("name");
	table->renameColumn("email", "email2");
```

使用 C 函数在 schema_v1 中定义表和列 （动态）

```c
	/* 创建架构版本 1 */
	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // 指定版本号或自动生成

	// 创建表 "users"
	table = sq_schema_create(schema_v1, "users", User);

	// 主键 PRIMARY KEY
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;        // set bit in SqColumn.bit_field

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);

	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	column->bit_field |= SQB_CURRENT;

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "updated_at", offset(User, updated_at));
	column->bit_field |= SQB_CURRENT | SQB_CURRENT_ON_UPDATE;

	// 外键 FOREIGN KEY
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");

	// 约束外键 CONSTRAINT FOREIGN KEY
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");

	// 创建索引 CREATE INDEX
	column = sq_table_add_index(table, "users_id_index", "id", NULL);

	/* 如果您将当前时间存储在列和成员中并且它们使用默认名称 - 'created_at' 和 'updated_at',
	   您可以使用下面的行替换上述 2 个 sq_table_add_timestamp() 函数。
	 */
//	sq_table_add_timestamps_struct(table, User);
```

使用 C 函数更改 schema_v2 中的表和列 （动态）

```c
	/* 创建架构版本 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // 指定版本号或自动生成

	// 更改表 "users"
	table = sq_schema_alter(schema_v2, "users", NULL);
	// 将列添加到表中
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	// 更改表中的列
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	column->bit_field |= SQB_CHANGED;        // set bit in SqColumn.bit_field
	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	sq_table_drop_foreign(table, "users_city_id_foreign");

	sq_table_drop_column(table, "name");
	sq_table_rename_column(table, "email", "email2");
```

* 您可以在 doc/[database-migrations.cn.md](doc/database-migrations.cn.md) 中获得有关架构和迁移的更多信息
* 要使用初始化器静态定义（或更改）表，请参阅 doc/[schema-builder-static.cn.md](doc/schema-builder-static.cn.md)
* 要使用宏动态定义（或更改）表，请参阅 doc/[schema-builder-macro.cn.md](doc/schema-builder-macro.cn.md)

## 数据库产品

**Sqdb** 是数据库产品（SQLite、MySQL 等）的基础结构。 您可以在 doc/[Sqdb.cn.md](doc/Sqdb.cn.md) 中获得更多描述和示例。  
  
使用 C 函数打开 SQLite 数据库

```c
	SqdbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_SQLITE, NULL);     // 如果 config 为 NULL，则使用默认设置。

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");    // 这将打开文件 "sqxc_local.db"
```

使用 C 函数打开 MySQL 数据库

```c
	SqdbConfigMysql  config = { .host = "localhost", .port = 3306,
	                            .user = "name", .password = "xxx" };

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_MYSQL, NULL);    // 如果 config 为 NULL，则使用默认设置。

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");
```

使用 C++ 方法打开 SQLite 数据库

```c++
	Sq::DbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = new Sq::DbSqlite(&config);
//	db = new Sq::DbSqlite(NULL);    // 如果 config 为 NULL，则使用默认设置。
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);    // this also works.

	storage = new Sq::Storage(db);
	storage->open("sqxc_local");    // 这将打开文件 "sqxc_local.db"
```

## 数据库同步（迁移）

使用 C++ 方法迁移架构并同步到数据库

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

使用 C 函数迁移架构并同步到数据库

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

## 增删查改

要获取更多信息和示例，您可以查看 doc/[SqStorage.cn.md](doc/SqStorage.cn.md)  
  
使用 C++ 方法

```c++
	User         *user;
	Sq::PtrArray *array;

	// 获取多行
	array = storage->getAll("users", "WHERE id > 8 AND id < 20");

	// 使用 C++ 类 'where' 获取多行（在下面的 "查询生成器" 中說明）
	array = storage->getAll("users", Sq::where("id > 8").where("id < %d", 20));

	// 获取所有行
	array = storage->getAll("users");
	// 获取一行
	user  = storage->get("users", 2);

	// 插入一行
	storage->insert("users", user);

	// 更新一行
	storage->update("users", user);
	// 更新特定列 - 多行中的 "name" 和 "email"。
	storage->updateAll("users", user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// 更新特定字段 - 多行中的 User::name 和 User::email。
	storage->updateField("users", user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// 删除一行
	storage->remove("users", 5);
	// 删除多行
	storage->removeAll("users", "WHERE id < 5");
```

使用 C++ 模板函数

```c++
	User              *user;
	std::vector<User> *vector;

	// 获取多行
	vector = storage->getAll<std::vector<User>>("WHERE id > 8 AND id < 20");
	// 获取所有行
	vector = storage->getAll<std::vector<User>>();
	// 获取一行
	user = storage->get<User>(2);

	// 插入一行
	storage->insert<User>(user);
		// 或
	storage->insert(user);

	// 更新一行
	storage->update<User>(user);
		// 或
	storage->update(user);

	// 更新特定列 - 多行中的 "name" 和 "email"。
	// 调用 updateAll<User>(...)
	storage->updateAll(user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// 更新特定字段 - 多行中的 User::name 和 User::email。
	// 调用 updateField<User>(...)
	storage->updateField(user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// 删除一行
	storage->remove<User>(5);
	// 删除多行
	storage->removeAll<User>("WHERE id < 5");
```

使用 C 函数

```c
	User  *user;

	// 获取多行
	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 8 AND id < 20");
	// 获取所有行
	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);
	// 获取一行
	user  = sq_storage_get(storage, "users", NULL, 2);

	sq_storage_insert(storage, "users", NULL, user);

	// 更新一行
	sq_storage_update(storage, "users", NULL, user);
	// 更新特定列 - 多行中的 "name" 和 "email"。
	sq_storage_update_all(storage, "users", NULL, user, 
	                      "WHERE id > 11 AND id < 28",
	                      "name", "email",
	                      NULL);
	// 更新特定字段 - 多行中的 User::name 和 User::email。
	sq_storage_update_field(storage, "users", NULL, user, 
	                        "WHERE id > 11 AND id < 28",
	                        offsetof(User, name),
	                        offsetof(User, email),
	                        -1);

	// 删除一行
	sq_storage_remove(storage, "users", NULL, 5);
	// 删除多行
	sq_storage_remove_all(storage, "users", "WHERE id < 5");
```

## 查询生成器

SqQuery 可以使用 C 函数或 C++ 方法生成 SQL 语句。
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
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > %d", 5);
```

使用 C 函数生成查询

* sq_query_join(query, NULL) 是子查询的开始。 它调用 sq_query_push_nested()
* sq_query_pop_nested(query) 是子查询的结尾。

```c
	sq_query_select(query, "id", "age", NULL);
	sq_query_from(query, "companies");
	sq_query_join(query, NULL);
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "100");
	sq_query_pop_nested(query);
	sq_query_as(query, "c");
	sq_query_on(query, "c.id = companies.city_id");
	sq_query_where(query, "age > %d", 5);
```

使用宏生成查询

```c
#include <sqxclib.h>
#include <SqQuery-macro.h>    // sqxclib.h doesn't contain special macros

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

#### 将 SqQuery 与 SqStorage 一起使用

SqStorage 提供 sq_storage_query() 和 C++ 方法 query() 来处理查询。

```c++
	// C 函数
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ 方法
	array = storage->query(query);
```

SqQuery 提供 sq_query_c() 或 C++ 方法 c() 来为 SqStorage 生成 SQL 语句。  
  
使用 C 函数

```c
	// SQL statement exclude "SELECT * FROM table_name"
	sq_query_clear(query);
	sq_query_where(query, "id > %d", 10);
	sq_query_or_where(query, "city_id < %d", 22);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

使用 C++ 方法

```c++
	// SQL statement exclude "SELECT * FROM table_name"
	query->clear()
	     ->where("id > %d", 10)
	     ->orWhere("city_id < %d", 22);

	array = storage->getAll("users", query->c());
```

**方便的 C++ 类**  
  
使用 Sq::Where（或 Sq::where）的 operator()

```c++
	Sq::Where  where;

	array = storage->getAll("users",
			where("id > %d", 10).orWhere("city_id < %d", 22).c());
```

使用 Sq::where 的构造函数和运算符

```c++
	// use parameter pack constructor
	array = storage->getAll("users",
			Sq::where("id > %d", 10).orWhere("city_id < %d", 22).c());

	// use default constructor 和 operator()
	array = storage->getAll("users",
			Sq::where()("id > %d", 10).orWhere("city_id < %d", 22).c());
```

## JOIN 支持

SqTypeJoint 是处理多表连接查询的默认类型。它为查询返回的结果创建指针数组。  
  
例如 从连接多表的查询中获取结果。  
  
使用 C 函数

```c
	sq_query_from(query, "cities");
	sq_query_join(query, "users",  "cities.id", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);
		// 在释放 'array' 之前释放 'element'
		// free(element);
	}
```

使用 C++ 方法

```c++
	query->from("cities")->join("users",  "cities.id", "users.city_id");

	Sq::PtrArray *array = (Sq::PtrArray*) storage->query(query);
	for (int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")
		// 在释放 'array' 之前释放 'element'
		// free(element);
	}
```

使用 C++ STL  
  
Sq::Joint 是 STL 容器使用的指针数组。

```c++
	query->from("cities")->join("users",  "cities.id", "users.city_id");

	std::vector< Sq::Joint<2> > *vector;
	vector = storage->query<std::vector< Sq::Joint<2> >>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Joint<2> &joint = vector->at(index);
		city = (City*)joint[0];      // from("cities")
		user = (User*)joint[1];      // join("users")
	}
```

#### 使用 SqTypeRow 获取结果

SqTypeRow 派生自 SqTypeJoint。它创建 SqRow 并处理未知（或已知）的结果、表和列。  
SqTypeRow 示例代码在 [storage-row.cpp](examples/storage-row.cpp)  
注意1：SqTypeRow 也可以与 get() 和 getAll() 一起使用。  
注意2：SqTypeRow 在 sqxcsupport 库中 (sqxcsupport.h)。  

	SqType
	│
	└─── SqTypeJoint
	     │
	     └─── SqTypeRow

**无需设置直接使用：**  
在这种情况下，SqRow 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。

```c
	SqTypeRow  *typeRow;
	SqRow      *row;

	// C 函数
	row = sq_storage_get(storage, "users", typeRow, 12);

	// C++ 方法
//	row = storage->get("users", typeRow, 12);

	for (int  index = 0;  index < row->length;  index++)
		if (row->cols[index].type == SQ_TYPE_STRING)
			puts(row->data[index].string);
```

您可以使用 SqTypeRow 替换 SqStorage 中的默认联合类型:

```c++
	Sq::TypeRow   *typeRow     = new Sq::TypeRow();
	Sq::TypeJoint *typeDefault = storage->joint_default;
	storage->joint_default = typeRow;
	delete typeDefault;
```

**当 SqTypeRow 为默认联合类型时，在以下情况下 query() 结果的元素类型为 SqRow：**
1. 'query' 已经加入了多表。
2. 'query' 有未知表或未知结果。

```c++
	std::vector<Sq::Row> *vector;

	query->from("cities")->join("users",  "cities.id", "users.city_id");

	vector = storage->query<std::vector<Sq::Row>>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Row &row = vector->at(index);
		for (unsigned int nth = 0;  nth < row.length;  nth++) {
			std::cout << row.cols[nth].name << " = ";
			if (row.cols[nth].type == SQ_TYPE_INT)
				std::cout << row.data[nth].integer << std::endl;
			if (row.cols[nth].type == SQ_TYPE_STRING)
				std::cout << row.data[nth].string  << std::endl;
			// other type...
		}
	}
```

**如果您不想更改默认联合类型：**
1. 调用 sq_storage_setup_query() 来设置 'query' 和 'typeRow'。
2. 调用 sq_storage_query() 時指定使用 'typeRow'。
3. 如果您跳过第 1 步，则 SqRow 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。

函数 sq_storage_setup_query() 声明：

```c++
// C 函数
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

// C++ 方法
Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
```

它设置 'query' 和 'type_joint' 然后返回 SqType 以调用 sq_storage_query()。  
如果 'query' 已加入多表，它将在 'query' 中添加 "SELECT table.column AS 'table.column'"。  

| 返回值       | 描述                                                                       |
| ------------ | ---------------------------------------------------------------------------|
| NULL         | 如果找不到表 并且 'type_joint' 不能处理未知的表类型。                      |
| 'type_joint' | 如果 'query' 已加入多表。它将设置 'type_joint' 和 'query'。                |
| 其他表类型   | 如果 'query' 只有 1个表。它将设置 'type_joint' 但保持 'query' 不变。在这种情况下，用户可以使用返回的类型或'type_joint' 调用 sq_storage_query()。 |

使用 C 函数

```c
	SqType *typeContainer = NULL;

	sq_storage_setup_query(storage, query, typeRow);
	vector = sq_storage_query(storage, query, typeRow, typeContainer);
```

使用 C++ 方法

```c++
	Sq::Type *typeContainer = NULL;

	storage->setupQuery(query, typeRow);
	container = storage->query(query, typeRow, typeContainer);
```

使用 C++ STL

```c++
	storage->setupQuery(query, typeRow);
	vector = storage->query<std::vector<Sq::Row>>(query, typeRow);
```

## 交易 Transaction

使用 C 函数

```c
	User  *user;

	sq_storage_begin_trans(storage);
	sq_storage_insert(storage, "users", NULL, user);
	if (abort)
		sq_storage_rollback_trans(storage);
	else
		sq_storage_commit_trans(storage);
```

使用 C++ 方法

```c++
	User  *user;

	storage->beginTrans();
	storage->insert(user);
	if (abort)
		storage->rollbackTrans();
	else
		storage->commitTrans();
```

## 配置头文件

更改构建配置。  
  
sqxclib 在搜索和排序 SQL 列名和 JSON 字段名时默认区分大小写。 用户可以在 sqxc/[SqConfig.h](sqxc/SqConfig.h) 中更改。

```c
// SqConfig.h 中的常用设置

/* sqxclib 在搜索和排序 SQL 列名和 JSON 字段名时默认区分大小写。
   某些旧的 SQL 产品可能需要禁用此功能。
   受影响的源代码 : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE        1

/* 如果用户没有指定 SQL 字符串长度，程序将使用默认值。
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191
```

## JSON 支持
- 这个库使用 [json-c](https://github.com/json-c/json-c) 来解析/写入 JSON。
- 所有定义的表和列都可以用来解析 JSON 对象和字段。
- 程序还可以解析存储在列中的 JSON 对象和数组。

## Sqdb
Sqdb 是数据库产品（SQLite、MySQL 等）的基础结构。  
您可以在 doc/[Sqdb.cn.md](doc/Sqdb.cn.md) 中获得更多描述和示例。  

## Sqxc
Sqxc 是数据解析和写入的接口。  
用户可以链接多个 Sqxc 元素来转换不同类型的数据。  
您可以在 doc/[Sqxc.cn.md](doc/Sqxc.cn.md) 中获得更多描述和示例。  

## SqApp
SqApp 使用配置文件（SqApp-config.h）来初始化数据库并为用户的应用程序进行迁移。  
它提供命令行程序来生成迁移并进行迁移。  
请参阅文档 doc/[SqApp.cn.md](doc/SqApp.cn.md)。  

## SqConsole
SqConsole 提供命令行界面（主要用于 SqAppTool）。  
请参阅文档 doc/[SqConsole.cn.md](doc/SqConsole.cn.md)。  

## 其他
SqType 文档: doc/[SqType.cn.md](doc/SqType.cn.md)  
SqEntry （SqColumn 的基类/结构） 文档: doc/[SqEntry.cn.md](doc/SqEntry.cn.md)  
SqColumn 文档: doc/[SqColumn.cn.md](doc/SqColumn.cn.md)  

## sqxc 怎么念

sqxc 中文发音「思库可思」，还可以翻译成白话文。  
備註：中文发音是在 2022年5月14日 凌晨4点左右决定的。

## 许可

sqxclib 在 木兰宽松许可证 第2版 下获得许可。


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

