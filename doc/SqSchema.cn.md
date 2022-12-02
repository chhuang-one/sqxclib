[English](SqSchema.md)

# SqSchema

SqSchema 定义数据库架构。它存储表和表的更改记录。

	SqEntry
	│
	└--- SqSchema

## 1 创建架构

当程序创建新架构时，它会自动生成版本号。目前生成版本号的方式很原始，就是给计数器加1。  
因为版本号是用来判断是否做迁移的。用户可以手动指定架构的版本号，这样可以避免一些问题。  
  
当用户创建新架构时，架构名称可以为 NULL。  
  
使用 C 语言

```c
	SqSchema *schema;

	// 创建新架构并分配它的名称和版本号
	schema = sq_schema_new("SchemaName");
	schema->version = 1;

	// 创建架构并仅分配它的版本号
//	schema = sq_schema_new(NULL);
//	schema->version = 1;
```

使用 C++ 语言

```c++
	Sq::Schema *schema;

	// 创建新架构并分配它的名称和版本号
	schema = new Sq::Schema("SchemaName");
	schema->version = 1;

	// 创建架构并仅分配它的版本号
//	schema = new Sq::Schema(1);
```

## 2 创建表

SqSchema 必须与 SqTable 和 [SqColumn](SqColumn.cn.md) 一起使用来创建表。您可以查看以下文档获取更多信息和示例：  
1. [database-migrations.cn.md](database-migrations.cn.md)
2. ../[README.cn.md](../README.cn.md#数据库架构) 中的 "**数据库架构**" 部分
  
当你使用 gcc 编译时，你会从 C 和 C++ 源代码中得到不同的类型名称，因为 gcc 的 typeid(Type).name() 会返回奇怪的名称。  
**如果您的应用程序是用 C++ 语言编写的，请用 C++ 语言创建或定义 SqTable 的类型。**  
  
如果 SqTable::type 是用 C 语言定义的，你不能使用下面的 C++ 模板函数来访问 SQL 表。

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

#### 2.1 为 C 结构创建表

定义一个 C 结构 'UserStruct' 来映射数据库表 "users"。  
  
使用 C 语言

```c
	// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
	typedef struct  UserStruct    UserStruct;

	struct  UserStruct {
		// ...
	};

	table = sq_schema_create(schema, "users", UserStruct);
	// 向表中添加列...
```

使用 C++ 语言

```c++
	struct  UserStruct {
		// ...
	};

	table = schema->create<UserStruct>("users");
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

```c++
	// C 函数
	sq_schema_drop(schema, "users");

	// C++ 方法
	schema->drop("users");
```

## 5 重命名表

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
