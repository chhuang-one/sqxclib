[English](README.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib 用于将 SQL 或 JSON 的资料与 C 语言的资料互相转换并提供 ORM 的功能和 C++ 包装器 (C++ wrapper)  
项目地址: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## 目前的功能:
1. 用户可以使用 C99 指定初始化(designated initializer) 或 C++ 聚合初始化(aggregate initialization) 静态定义 SQL表/列/迁移。
   也可以使用 C 函数 或 C++ 方法 动态执行这些操作。

2. 所有定义的 SQL表/列 都可以用于解析 JSON 对象/字段。也可以从 SQL 列 解析 JSON 对象/数组。

3. 可以在低端硬件上工作。

4. 单一头文件 〈 **sqxclib.h** 〉 (注意：不包含特殊宏和支持库)

5. 命令行工具可以生成迁移并进行迁移。 见 doc/[SqApp.md](doc/SqApp.md)。

6. 支持 SQLite, MySQL / MariaDB。

7. 提供项目模板。 见目录 [project-template](project-template)。

## 数据库架构

定义映射到数据库表 “users” 的 C 结构化数据类型。

```c++
typedef struct  User    User;    // add this line if you use C language

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;
	int     city_id;     // foreign key

	time_t  created_at;
	time_t  updated_at;

#ifdef __cplusplus       // C++ Only
	std::string       strCpp;
	std::vector<int>  intsCpp;
#endif
};
```

使用 C++ 方法（Schema Builder）在 schema_v1 中定义表/列 （动态）

```c++
/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

	/* create schema version 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = schema_v1->create<User>("users");
	// add dynamic columns to table
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
	// FOREIGN KEY
	table->integer("city_id", &User::city_id)->reference("cities", "id");
	// CONSTRAINT FOREIGN KEY
	table->foreign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");
	// CREATE INDEX
	table->index("users_id_index", "id");

	// If columns and members use default names - 'created_at' and 'updated_at',
	// you can use below line to replace above 2 timestamp() methods.
//	table->timestamps<User>();
```

使用 C++ 方法（Schema Builder）更改 schema_v2 中的表/列 （动态）

```c++
	/* create schema version 2 */
	schema_v2 = new Sq::Schema("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = schema_v2->alter("users");
	// add dynamic columns/records to table
	table->integer("test_add", &User::test_add);
	table->integer("city_id", &User::city_id)->change();
	table->dropForeign("users_city_id_foreign");    // DROP CONSTRAINT FOREIGN KEY
	table->drop("name");
	table->rename("email", "email2");
```

使用 C++ 聚合初始化在 schema_v1 中定义表/列 （静态）
* 这可以减少制作 schema 时的运行时间。
* 如果您的 SQL 表是固定的并且以后不会更改，您可以通过使用常量 SqType 来定义表来减少更多的运行时间。见文件 doc/[SqColumn.md](doc/SqColumn.md)

```c++
/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

static const SqForeign userForeign = {"cities",  "id",  "CASCADE",  "CASCADE"};

static const SqColumn  userColumns[8] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	{SQ_TYPE_STRING, "name",       offsetof(User, name)  },

	{SQ_TYPE_STRING, "email",      offsetof(User, email),      0,
		NULL,                          // .old_name
		60},                           // .size    // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    0,
		NULL, 0, 0, NULL, NULL,        // .old_name, .size, .digits, .default_value, .check
		(SqForeign*) &userForeign},    // .foreign

	// C++ std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* create schema version 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = schema_v1->create<User>("users");
	// add static 'userColumns' that has 8 elements to table
	table->addColumn(userColumns, 8);
```

使用 C99 指定初始化程序在 schema_v1 中定义表/列 （静态）
* 这可以减少制作 schema 时的运行时间。
* 如果您的 SQL 表是固定的并且以后不会更改，您可以通过使用常量 SqType 来定义表来减少更多的运行时间。见文件 doc/[SqColumn.md](doc/SqColumn.md)

```c
static const SqColumn  userColumns[8] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	{SQ_TYPE_STRING, "name",       offsetof(User, name)  },

	{SQ_TYPE_STRING, "email",      offsetof(User, email),
		.size = 60},    // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),
		.foreign = &(SqForeign) {"cities", "id", NULL, NULL}    },

	// CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,  "users_city_id_foreign",
		.foreign = &(SqForeign) {"cities", "id", "NO ACTION", "NO ACTION"},
		.composite = (char *[]) {"city_id", NULL} },

	// CREATE INDEX
	{SQ_TYPE_INDEX,       "users_id_index",
		.composite = (char *[]) {"id", NULL} },
};

	/* create schema version 1 */
	schema_v1 = sq_schema_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);

	// add static 'userColumns' that has 8 elements to table
	sq_table_add_column(table, userColumns, 8);
```

使用 C99 指定初始化程序更改 schema_v2 中的表/列 （静态）

```c
static const SqColumn  userColumnsChanged[5] = {
	// ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id",  offsetof(User, city_id), SQB_CHANGED},

	// DROP CONSTRAINT FOREIGN KEY
	{.old_name = "users_city_id_foreign",     .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_FOREIGN },

	// DROP COLUMN "name"
	{.old_name = "name",      .name = NULL},

	// RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	/* create schema version 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// add static 'userColumnsChanged' that has 5 elements to table
	sq_table_add_column(table, userColumnsChanged, 5);
```

使用 C 函数（Schema Builder）在 schema_v1 中定义表/列 （动态）

```c
	/* create schema version 1 */
	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);

	// PRIMARY KEY
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

	// FOREIGN KEY
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");

	// CONSTRAINT FOREIGN KEY
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");

	// CREATE INDEX
	column = sq_table_add_index(table, "users_id_index", "id", NULL);


	// If columns and members use default names - 'created_at' and 'updated_at',
	// you can use below line to replace above 2 sq_table_add_timestamp() functions.
//	sq_table_add_timestamps_struct(table, User);
```

使用 C 函数（Schema Builder）更改 schema_v2 中的表/列 （动态）

```c
	/* create schema version 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// add column to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));

	// alter column in table
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	column->bit_field |= SQB_CHANGED;        // set bit in SqColumn.bit_field

	sq_table_drop_foreign(table, "users_city_id_foreign");
	sq_table_drop_column(table, "name");
	sq_table_rename_column(table, "email", "email2");
```

其他 constraint 示例代码:  
  
使用 C99 指定初始化器更改 constraint（静态）

```c
static const SqColumn  otherSampleChanged_1[] = {
	// CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "other_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"column1", "column2", NULL} },

	// CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "other_unique",  0,  SQB_UNIQUE,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherSampleChanged_2[] = {
	// DROP CONSTRAINT PRIMARY KEY
	{.old_name = "other_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_PRIMARY },

	// DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_UNIQUE },
};
```

其他 constraint 示例代码 (Schema Builder):  
  
使用 C 函数更改 constraint（动态）

```c
	// ADD CONSTRAINT UNIQUE
	sq_table_add_unique(table, "other_unique", "column1", "column2", NULL);
	// ADD CONSTRAINT PRIMARY KEY
	sq_table_add_primary(table, "other_primary", "column1", "column2", NULL);

	// DROP CONSTRAINT UNIQUE
	sq_table_drop_unique(table, "other_unique");
	// DROP CONSTRAINT PRIMARY KEY
	sq_table_drop_primary(table, "other_primary");
```

使用 C++ 方法更改 constraint（动态）

```c++
	// ADD CONSTRAINT UNIQUE
	table->addUnique("other_unique", "column1", "column2");
	// ADD CONSTRAINT PRIMARY KEY
	table->addPrimary("other_primary", "column1", "column2");

	// DROP CONSTRAINT UNIQUE
	table->dropUnique("other_unique");
	// DROP CONSTRAINT PRIMARY KEY
	table->dropPrimary("other_primary");
```

* 要使用宏动态定义（或更改）表，请参阅 doc/[schema-builder-macro.md](doc/schema-builder-macro.md)
* 您可以在 doc/[database-migrations.md](doc/database-migrations.md) 中获得有关架构和迁移的更多信息

## 数据库同步（迁移）

使用 C++ 方法迁移架构并同步到数据库

```c++
	// migrate 'schema_v1' and 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// synchronize schema to database and update schema/table status
	// This is mainly used by SQLite
	storage->migrate(NULL);

	// free unused 'schema_v1' and 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

使用 C 函数迁移架构并同步到数据库

```c
	// migrate 'schema_v1' and 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// synchronize schema to database and update schema/table status
	// This is mainly used by SQLite
	sq_storage_migrate(storage, NULL);

	// free unused 'schema_v1' and 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```

## 增删查改

要获取更多信息和示例，您可以查看 doc/[SqStorage.md](doc/SqStorage.md)  
  
使用 C 函数

```c
	User  *user;

	// get multiple rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 8 AND id < 20");
	// get all rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);
	// get one row
	user  = sq_storage_get(storage, "users", NULL, 2);

	sq_storage_insert(storage, "users", NULL, user);

	// update one row
	sq_storage_update(storage, "users", NULL, user);
	// update specific columns - "name" and "email" in multiple rows.
	sq_storage_update_all(storage, "users", NULL, user, 
	                      "WHERE id > 11 AND id < 28",
	                      "name", "email",
	                      NULL);
	// update specific fields - User::name and User::email in multiple rows.
	sq_storage_update_field(storage, "users", NULL, user, 
	                        "WHERE id > 11 AND id < 28",
	                        offsetof(User, name),
	                        offsetof(User, email),
	                        -1);

	// remove one row
	sq_storage_remove(storage, "users", NULL, 5);
	// remove multiple rows
	sq_storage_remove_all(storage, "users", "WHERE id < 5");
```

使用 C++ 方法

```c++
	User         *user;
	Sq::PtrArray *array;

	// get multiple rows
	array = storage->getAll("users", "WHERE id > 8 AND id < 20");
	// get all rows
	array = storage->getAll("users");
	// get one row
	user  = storage->get("users", 2);

	// insert one row
	storage->insert("users", user);

	// update one row
	storage->update("users", user);
	// update specific columns - "name" and "email" in multiple rows.
	storage->updateAll("users", user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// update specific fields - User::name and User::email in multiple rows.
	storage->updateField("users", user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// remove one row
	storage->remove("users", 5);
	// remove multiple rows
	storage->removeAll("users", "WHERE id < 5");
```

使用 C++ 模板函数

```c++
	User              *user;
	std::vector<User> *vector;

	// get multiple rows
	vector = storage->getAll<std::vector<User>>("WHERE id > 8 AND id < 20");
	// get all rows
	vector = storage->getAll<std::vector<User>>();
	// get one row
	user = storage->get<User>(2);

	// insert one row
	storage->insert<User>(user);
		// or
	storage->insert(user);

	// update one row
	storage->update<User>(user);
		// or
	storage->update(user);

	// update specific columns - "name" and "email" in multiple rows.
	// call updateAll<User>(...)
	storage->updateAll(user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// update specific fields - User::name and User::email in multiple rows.
	// call updateField<User>(...)
	storage->updateField(user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// remove one row
	storage->remove<User>(5);
	// remove multiple rows
	storage->removeAll<User>("WHERE id < 5");
```

## 数据库产品

使用 C 函数打开 SQLite 数据库

```c
	SqdbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_SQLITE, NULL);     // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");    // This will open file "sqxc_local.db"
```

使用 C 函数打开 MySQL 数据库

```c
	SqdbConfigMysql  config = { .host = "localhost", .port = 3306,
	                            .user = "name", .password = "xxx" };

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_MYSQL, NULL);    // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");
```

使用 C++ 方法打开 SQLite 数据库

```c++
	Sq::DbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = new Sq::DbSqlite(&config);
//	db = new Sq::DbSqlite(NULL);    // use default setting if config is NULL.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);    // this also works.

	storage = new Sq::Storage(db);
	storage->open("sqxc_local");    // This will open file "sqxc_local.db"
```

## 查询生成器

SqQuery 可以使用 C 函数或 C++ 方法生成 SQL 语句。
要获取更多信息和示例，您可以查看 doc/[SqQuery.md](doc/SqQuery.md)
  
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
	// C function
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ method
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

方便的 C++ 类  
  
使用 Sq::Where（或 Sq::where）的 operator()

```c++
	Sq::Where  where;

	array = storage->getAll("users",
			where("id > %d", 10).orWhere("city_id < %d", 22).c());
```

使用 Sq::where 的构造函数和运算符

```c++
	// use default constructor and operator()
	array = storage->getAll("users",
			Sq::where()("id > %d", 10).orWhere("city_id < %d", 22).c());

	// use parameter pack constructor (Visual C++ can NOT use this currently)
	array = storage->getAll("users",
			Sq::where("id > %d", 10).orWhere("city_id < %d", 22).c());
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
		// free 'element' before you free 'array'
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
		// free 'element' before you free 'array'
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

	// C function
	row = sq_storage_get(storage, "users", typeRow, 12);

	// C++ method
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

**关于 sq_storage_setup_query()**：  
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);  
它设置 'query' 和 'type_joint' 然后返回 SqType 以调用 sq_storage_query()。  
如果 'query' 已加入多表，它将在 'query' 中添加 "SELECT table.column AS 'table.column'"。  

| 返回值       | 描述                                                                       |
| ------------ | ---------------------------------------------------------------------------|
| NULL         | 如果找不到表 并且 'type_joint' 不能处理未知的表类型。                      |
| 'type_joint' | 如果 'query' 已加入多表。它将设置 'type_joint' 和 'query'。                |
| 其他表类型   | 如果 'query' 只有 1个表。它将设置 'type_joint' 但保持 'query' 不变。在这种情况下，用户可以使用返回的类型或'type_joint' 调用 sq_storage_query()。 |

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
  
sqxclib 在搜索/排序 SQL 列名和 JSON 字段名时默认区分大小写。 用户可以在 sqxc/[SqConfig.h](sqxc/SqConfig.h) 中更改它。

```c
// Common settings in SqConfig.h

/* sqxclib is case-sensitive when searching/sorting SQL column name and JSON field name by default.
   You may disable this for some old SQL product.
   Affected source : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE        1

/* If user doesn't specify SQL string length, program will use it by default.
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191
```

## JSON 支持
- 这个库使用 [json-c](https://github.com/json-c/json-c) 来解析/写入 JSON。
- 所有定义的表/列都可以用来解析 JSON 对象/字段。
- 程序还可以解析存储在列中的 JSON 对象/数组。

## Sqdb
Sqdb 是数据库产品（SQLite、MySQL 等）的基础结构。  
SqdbSqlite.c 为 SQLite 实现 Sqdb 接口。  
SqdbMysql.c 为 MySQL 实现 Sqdb 接口。  
您可以在 doc/[Sqdb.md](doc/Sqdb.md) 中获得更多描述和示例。  

## Sqxc
Sqxc 是数据解析和写入的接口。  
用户可以链接多个 Sqxc 元素来转换不同类型的数据。  
您可以在 doc/[Sqxc.md](doc/Sqxc.md) 中获得更多描述和示例。  

## SqApp
SqApp 使用配置文件（SqApp-config.h）来初始化数据库并为用户的应用程序进行迁移。  
它提供命令行程序来生成迁移并进行迁移。  
请参阅文档 doc/[SqApp.md](doc/SqApp.md)。  

## SqConsole
SqConsole 提供命令行界面（主要用于 SqAppTool）。  
请参阅文档 doc/[SqConsole.md](doc/SqConsole.md)。  

## 其他
SqType 文档: doc/[SqType.md](doc/SqType.md)  
SqEntry （SqColumn 的基类/结构） 文档: doc/[SqEntry.md](doc/SqEntry.md)  
SqColumn 文档: doc/[SqColumn.md](doc/SqColumn.md)  

## sqxc 怎么念

sqxc 中文发音「思库可思」，还可以翻译成白话文。  
備註：中文发音是在 2022年5月14日 凌晨4点左右决定的。

## 许可

sqxclib 在 木兰宽松许可证 第2版 下获得许可。


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

