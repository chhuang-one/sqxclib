[English](schema-builder-macro.md)

# Schema Builder (C 宏)

本文档介绍如何使用 C 宏动态定义表。  

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
};
```

使用 C 宏定义 schema_v1 中的表和列（动态）  
  
宏 SQ_SCHEMA_CREATE() 可以创建表。宏中的最后一个参数类似于 lambda 函数。  
宏 SQT_XXXX() 用于向表中添加列。  
宏 SQC_XXXX() 用于设置列属性。

```c
#include <sqxclib.h>
#include <SqSchema-macro.h>    // sqxclib.h 不包含特殊宏

	// 创建架构并指定版本号为 1
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// 创建表 "users"
	SQ_SCHEMA_CREATE(schema_v1, "users", User, {
		// 主键 PRIMARY KEY
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();

		// VARCHAR
		SQT_STRING("name", User, name, -1);

		// VARCHAR(60)
		SQT_STRING("email", User, email, 60);

		// DEFAULT CURRENT_TIMESTAMP
		SQT_TIMESTAMP("created_at", User, created_at);  SQC_USE_CURRENT();

		// 外键 FOREIGN KEY
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");

		// 约束外键 CONSTRAINT FOREIGN KEY
		SQT_ADD_FOREIGN("users_city_id_foreign", "city_id");
			SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("NO ACTION");  SQC_ON_UPDATE("NO ACTION"):

		// 创建索引 CREATE INDEX
		SQT_ADD_INDEX("users_id_index", "id");
	});
```

使用 C 宏更改 schema_v2 中的表和列（动态）  
  
宏 SQ_SCHEMA_ALTER() 可以改变表。宏中的最后一个参数类似于 lambda 函数。

```c
	// 创建架构并指定版本号为 2
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// 更改表 "users"
	SQ_SCHEMA_ALTER(schema_v2, "users", User, {
		// 将列添加到表中
		SQT_INTEGER("test_add", User, test_add);

		// 更改表中的列
		SQT_INTEGER("city_id", User, city_id);  SQC_CHANGE();

		// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
		SQT_DROP_FOREIGN("users_city_id_foreign");

		// 删除列
		SQT_DROP("name");

		// 重命名列
		SQT_RENAME("email", "email2");
	});
```

## 仅查询列, 类型映射 (C 宏)

要定义类型映射列，请使用宏 SQT_MAPPING() 来映射 SQL 类型和 SqType。  
  
要定义仅查询列，请将列名称设置为 SELECT 查询并使用宏 SQC_QUERY_ONLY() 设置列属性。
**注意**：如果要使用仅查询列，请在 SqConfig.h 中启用 SQ_CONFIG_QUERY_ONLY_COLUMN。  

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct  Demo    Demo;

struct Demo {
	int     id;          // 主键

	char   *text;

	// 'text' 的长度
	// SQL 语句: SELECT length(text), * FROM table
	int     text_length;
};

	// 创建表 "demo"
	SQ_SCHEMA_CREATE(schema_v1, "demo", Demo, {
		// 主键 PRIMARY KEY
		SQT_INTEGER("id", Demo, id);  SQC_PRIMARY();

		// 类型映射：SQ_TYPE_STR 映射到 SQL 数据类型 TEXT
		SQT_MAPPING("text", Demo, text, SQ_TYPE_STR, SQ_SQL_TYPE_TEXT);

		// 仅查询列
		SQT_INTEGER("length(text)", Demo, text_length);  SQC_QUERY_ONLY();
	});
```
