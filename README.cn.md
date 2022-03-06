[English](README.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib 用于将 SQL 或 JSON 的资料与 C 语言的资料互相转换并提供 ORM 的功能和 C++ 包装器 (C++ wrapper)  
项目地址: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## 目前的功能:
1. 用户可以使用 C99 指定初始化器(designated initializer) 或 C++ 聚合初始化(aggregate initialization) 静态定义 SQL表/列/迁移。
   也可以使用 C 函数 或 C++ 方法 动态执行这些操作。

2. 所有定义的 SQL表/列 都可以用于解析 JSON 对象/字段。也可以从 SQL 列 解析 JSON 对象/数组。

3. 可以在低端硬件上工作.

4. 单一头文件 〈 **sqxclib.h** 〉 (注意：不包含特殊宏)

5. 命令行工具可以生成迁移并进行迁移. 见 doc/[SqApp.md](doc/SqApp.md)

6. 支持 SQLite, MySQL / MariaDB.

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
	table->index("users_id_index", "id", NULL);

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
* 这可以减少制作模式时的运行时间。
* 如果您的 SQL 表是固定的并且以后不会更改，您可以通过使用常量 SqType 来定义表来减少更多的运行时间。见文件 doc/[SqColumn.md](doc/SqColumn.md)

```c++
/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

static const SqForeign userForeign = {"cities",  "id",  "CASCADE",  "CASCADE"};

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
		.foreign = (SqForeign*) &userForeign},

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
* 这可以减少制作模式时的运行时间。
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
use C99 designated initializer to change constraint (static)

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
  
use C functions to change constraint (dynamic)

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

use C++ methods to change constraint (dynamic)

```c++
	// ADD CONSTRAINT UNIQUE
	table->addUnique("other_unique", "column1", "column2", NULL);
	// ADD CONSTRAINT PRIMARY KEY
	table->addPrimary("other_primary", "column1", "column2", NULL);

	// DROP CONSTRAINT UNIQUE
	table->dropUnique("other_unique");
	// DROP CONSTRAINT PRIMARY KEY
	table->dropPrimary("other_primary");
```

* To use C macro to define (or change) table dynamically, see doc/[schema-builder-macro.md](doc/schema-builder-macro.md)
* You can get more information about schema and migrations in doc/[database-migrations.md](doc/database-migrations.md)

## Database synchronization (Migration)

use C++ methods to migrate schema and synchronize to database

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

use C functions to migrate schema and synchronize to database

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
  
use C functions

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

use C++ methods

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

use C++ template functions

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

## Database support

use C functions to open SQLite database

```c
	SqdbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_SQLITE, NULL);     // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");    // This will open file "sqxc_local.db"
```

use C functions to open MySQL database

```c
	SqdbConfigMysql  config = { .host = "localhost", .port = 3306,
	                            .user = "name", .password = "xxx" };

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_MYSQL, NULL);    // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");
```

use C++ methods to open SQLite database

```c++
	Sq::DbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = new Sq::DbSqlite(&config);
//	db = new Sq::DbSqlite(NULL);    // use default setting if config is NULL.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);    // this also works.

	storage = new Sq::Storage(db);
	storage->open("sqxc_local");    // This will open file "sqxc_local.db"
```

## 查询生成器

SqQuery can generate SQL statement by using C functions or C++ methods.
To get more information and sample, you can see doc/[SqQuery.md](doc/SqQuery.md)  
  
SQL statement

```sql
	SELECT id, age
	FROM companies
	JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
	WHERE age > 5
```

use C++ methods to produce query

```c++
	query->select("id", "age")
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > %d", 5);
```

use C functions to produce query

* sq_query_join(query, NULL) is start of Subquery. It call sq_query_push_nested()
* sq_query_pop_nested(query) is end of Subquery.

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

use C macro to produce query

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

To use SqQuery with SqStorage, call C function sq_storage_query() or C++ method query()

```c++
	// C function
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ method
	array = storage->query(query);
```

## JOIN support

use C functions

```c
	SqPtrArray *array;

	sq_query_from(query, "cities");
	sq_query_join(query, "users",  "cities.id", "users.city_id");

	array = sq_storage_query(storage, query, NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);
		// free 'element' before you free 'array'
		// free(element);
	}
```

use C++ methods

```c++
	query->from("cities")->join("users",  "cities.id", "users.city_id");

	typedef  Sq::Joint<2>  SqJoint2;
	vector = storage->query<std::vector<SqJoint2>>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		SqJoint2 *joint = vector->at(index);
		city = (City*)joint.t[0];
		user = (User*)joint.t[1];
	}
	// or
	SqPtrArray *array = (Sq::PtrArray*) storage->query(query);
	for (int i = 0;  i < array->length;  i++) {
		element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")
		// free 'element' before you free 'array'
		// free(element);
	}
```

## 交易 Transaction

use C functions

```c
	User  *user;

	sq_storage_begin_trans(storage);
	sq_storage_insert(storage, "users", NULL, user);
	if (abort)
		sq_storage_rollback_trans(storage);
	else
		sq_storage_commit_trans(storage);
```

use C++ methods

```c++
	User  *user;

	storage->beginTrans();
	storage->insert(user);
	if (abort)
		storage->rollbackTrans();
	else
		storage->commitTrans();
```

## Configuration Headers
change build configuration.

sqxclib is case-sensitive when searching/sorting SQL column name and JSON field name by default. User can change it in sqxc/[SqConfig.h](sqxc/SqConfig.h).

```c
// Common settings in SqConfig.h

/* Enable "SQL_table_name" <-> "C struct type_name" converting.
   When calling sq_schema_create_xxx():
   If user only specify "SQL_table_name", program generate "C struct type_name".
   If user only specify "C struct type_name", program generate "SQL_table_name".
   Affected source : SqSchema, SqUtil, SqAppTool
 */
#define SQ_CONFIG_NAMING_CONVENTION

/* sqxclib is case-sensitive when searching/sorting SQL column name and JSON field name by default.
   You may disable this for some old SQL product.
   Affected source : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE

/* If user doesn't specify SQL string length, program will use it by default.
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191
```

## JSON support
- This library use [json-c](https://github.com/json-c/json-c) to parse/write JSON.
- all defined table/column can use to parse JSON object/field
- program can also parse JSON object/array that store in column.

## Sqdb
Sqdb is base structure for database product (SQLite, MySQL...etc).  
SqdbSqlite.c implement Sqdb interface for SQLite.  
SqdbMysql.c implement Sqdb interface for MySQL.  
You can get more description and example in doc/[Sqdb.md](doc/Sqdb.md)  

## Sqxc
Sqxc is interface for data parse and write.  
User can link multiple Sqxc element to convert different types of data.  
You can get more description and example in doc/[Sqxc.md](doc/Sqxc.md)  

## SqApp
SqApp use configuration file (SqApp-config.h) to initialize database and do migrations for user's application.  
It provide command-line program to generate migration and do migrate.  
See document in doc/[SqApp.md](doc/SqApp.md)  

## SqConsole
SqConsole provide command-line interface (mainly for SqAppTool).  
Document for SqConsole in doc/[SqConsole.md](doc/SqConsole.md)  

## Others
Document for SqEntry (SqColumn's base class/structure) in doc/[SqEntry.md](doc/SqEntry.md)  
Document for SqColumn in doc/[SqColumn.md](doc/SqColumn.md)  
Document for SqType in doc/[SqType.md](doc/SqType.md)  

## Licensing

sqxc is licensed under the Mulan PSL v2.


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

