﻿[English](SqSchema.md)

# SqSchema

SqSchema 定义数据库架构。它存储表和表的更改记录。[SqStorage](SqStorage.md) 使用它来进行迁移。

	SqEntry
	│
	└─── SqSchema

## 1 创建架构

当程序创建新架构时，它会自动生成版本号。目前生成版本号的方式很原始，就是给计数器加 1。  
因为版本号是用来判断是否做迁移的。用户可以手动指定架构的版本号，这样可以避免一些问题。  
  
当用户创建新架构时，架构名称可以为 NULL。  
  
使用 C 语言

```c
	SqSchema *schema;
	int       version = 2;

	// 创建新架构并自动分配其版本号。
	schema = sq_schema_new("SchemaName");

	// 创建架构并指定其版本号。
	schema = sq_schema_new_ver(version, "SchemaName");
```

使用 C++ 语言

```c++
	Sq::Schema *schema;
	int         version = 2;

	// 创建新架构并自动分配其版本号。
	schema = new Sq::Schema("SchemaName");

	// 创建架构并指定其版本号。
	schema = new Sq::Schema(version, "SchemaName");
```

## 2 创建表

SqSchema 必须与 [SqTable](SqTable.cn.md) 和 [SqColumn](SqColumn.cn.md) 一起使用来创建表。您可以查看以下文档获取更多信息和示例：

1. [database-migrations.cn.md](database-migrations.cn.md)
2. ../[README.cn.md](../README.cn.md#数据库架构) 中的 "**数据库架构**" 部分

#### 2.1 为 C 结构创建表

定义一个 C 结构 'User' 来映射数据库表 "users"。  
  
使用 C 语言

```c
	// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
	typedef struct  User    User;

	struct  User {
		// ...
	};

	table = sq_schema_create(schema, "users", User);
	// 向表中添加列...
```

使用 C++ 语言

```c++
	struct  User {
		// ...
	};

	table = schema->create<User>("users");
	// 向表中添加列...
```

#### 2.2 按现有 SqType 创建表

您可以查看文档 [SqColumn.cn.md](SqColumn.cn.md) 以获取有关使用 SqColumn 创建 SqType 的更多信息。  
注意: 如果 'type' 是动态 SqType，它会在程序释放 'table' 时被一起释放。  
  
使用 C 语言

```c
	// 按现有类型创建表
	table = sq_schema_create_by_type(schema, "your_table_name", type);
```

使用 C++ 语言

```c++
	// 按现有类型创建表
	table = schema->create("your_table_name", type);
```

#### 2.3 添加现有表

```c++
	// C 函数
	sq_schema_add(schema, table);

	// C++ 方法
	schema->add(table);
```

## 3 修改表

alter() 的用法与 create() 类似。  
  
使用 C 语言

```c
	// 修改表
	table = sq_schema_alter(schema, "users", NULL);

	// 更改列 "nickname"
	column = sq_table_add_string(table, "nickname", offsetof(User, nickname), 40);
	sq_column_change();
	// 添加或更改列...
```

使用 C++ 语言

```c++
	// 修改表
	table = schema->alter("users");

	// 更改列 "nickname"
	table->string("nickname", offsetof(User, nickname), 40)->change();
	// 添加或更改列...
```

## 4 删除表

sq_schema_drop() 必须指定要删除的表名。因为它只是向 'schema' 中添加了一条 "删除表" 的记录，所以不会在迁移之前删除该表。

```c++
	// C 函数
	sq_schema_drop(schema, "users");

	// C++ 方法
	schema->drop("users");
```

## 5 重命名表

与 sq_schema_drop() 一样，sq_schema_rename() 不会立即重命名表。

```c++
	// C 函数
	sq_schema_rename(schema, "old_name", "new_name");

	// C++ 方法
	schema->rename("old_name", "new_name");
```

## 6 查找表

创建或迁移表后，用户可以通过表的名称在架构中找到表。

```c++
	// C 函数
	table = sq_schema_find(schema, "users");

	// C++ 方法
	table = schema->find("users");
```
