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

```c
#include <sqxclib.h>
#include <SqSchema-macro.h>    // sqxclib.h 不包含特殊宏

	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // 指定版本号或自动生成

	// 创建表 "users"
	SQ_SCHEMA_CREATE(schema_v1, "users", User, {
		// 主键 PRIMARY KEY
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();

		SQT_STRING("name", User, name, -1);

		SQT_STRING("email", User, email, 60);    // VARCHAR(60)

		SQT_TIMESTAMP("created_at", User, created_at);  SQC_DEFAULT("CURRENT_TIMESTAMP");

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

```c
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // 指定版本号或自动生成

	// 更改表 "users"
	SQ_SCHEMA_ALTER(schema_v2, "users", User, {
		SQT_INTEGER("test_add", User, test_add);
		SQT_INTEGER("city_id", User, city_id);  SQC_CHANGE();
		SQT_DROP_FOREIGN("users_city_id_foreign");
		SQT_DROP("name");
		SQT_RENAME("email", "email2");
	});
```
