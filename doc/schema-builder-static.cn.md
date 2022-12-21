[English](schema-builder-static.md)

# Schema Builder (静态)

本文档介绍如何使用 C99 指定初始化器（或 C++ 聚合初始化）来定义表。
* 这可以减少制作架构时的运行时间。
* 架构可以处理动态和静态列/表定义。
* 如果用户修改静态定义的列/表，程序将在修改之前复制列/表。
* 程序不会从内存中释放静态定义的列/表。它只是不使用它们。
* 如果您的 SQL 表是固定的并且以后不会更改，您可以通过使用常量 SqType 来定义表来减少更多的运行时间。见文件 [SqColumn.cn.md](SqColumn.cn.md)

定义 C 结构化数据类型以映射数据库表 "users"。

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

## C99 指定初始化

使用 C99 指定初始化程序在 schema_v1 中定义表和列 （静态）

```c
#include <sqxclib.h>

static const SqColumn  userColumns[8] = {
	// 主键 PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),
		.size = 60},

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// 外键 FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),
		.foreign = &(SqForeign) {"cities", "id", NULL, NULL}    },

	// 约束外键 CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,  "users_city_id_foreign",
		.foreign = &(SqForeign) {"cities", "id", "NO ACTION", "NO ACTION"},
		.composite = (char *[]) {"city_id", NULL} },

	// 创建索引 CREATE INDEX
	{SQ_TYPE_INDEX,       "users_id_index",
		.composite = (char *[]) {"id", NULL} },
};

	/* 创建架构版本 1 */
	schema_v1 = sq_schema_new("Ver 1");
	schema_v1->version = 1;    // 指定版本号或自动生成

	// 创建表 "users"
	table = sq_schema_create(schema_v1, "users", User);

	// 将具有 8 个元素的静态 'userColumns' 添加到表中
	sq_table_add_column(table, userColumns, 8);
```

使用 C99 指定初始化程序更改 schema_v2 中的表和列 （静态）

```c
static const SqColumn  userColumnsChanged[5] = {
	// 向表中添加列 ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// 更改表中的列 ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id",  offsetof(User, city_id), SQB_CHANGED},

	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	{.old_name = "users_city_id_foreign",     .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_FOREIGN },

	// 删除列 DROP COLUMN "name"
	{.old_name = "name",      .name = NULL},

	// 重命名列 RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	/* 创建架构版本 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // 指定版本号或自动生成

	// 更改表 "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// 通过具有 5 个元素的静态 'userColumnsChanged' 更改表
	sq_table_add_column(table, userColumnsChanged, 5);
```

使用 C99 指定初始化器更改 constraint（静态）

```c
static const SqColumn  otherSampleChanged_1[] = {
	// 约束主键 CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "other_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"column1", "column2", NULL} },

	// 约束唯一 CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "other_unique",  0,  SQB_UNIQUE,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherSampleChanged_2[] = {
	// 删除约束主键 DROP CONSTRAINT PRIMARY KEY
	{.old_name = "other_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_PRIMARY },

	// 删除约束唯一 DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_UNIQUE },
};
```

## C++ 聚合初始化

成员中的所有数据与上面的示例代码相同。  
  
使用 C++ 聚合初始化在 schema_v1 中定义表和列 （静态）

```c++
#include <sqxclib.h>

/* 为 C++ STL 定义全局类型 */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

static const SqForeign userForeign = {"cities",  "id",  "CASCADE",  "CASCADE"};

static const SqColumn  userColumns[8] = {
	// 主键 PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	{SQ_TYPE_STR,    "email",      offsetof(User, email),      0,
		NULL,                          // .old_name
		60},                           // .size    // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// 外键 FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    0,
		NULL, 0, 0, NULL,              // .old_name, .size, .digits, .default_value,
		(SqForeign*) &userForeign},    // .foreign

	// C++ std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* 创建架构版本 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // 指定版本号或自动生成

	// 创建表 "users"
	table = schema_v1->create<User>("users");
	// 将具有 8 个元素的静态 'userColumns' 添加到表中
	table->addColumn(userColumns, 8);
```

## 迁移

无论是动态定义还是静态定义，运行迁移的代码都是一样的。  
  
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
