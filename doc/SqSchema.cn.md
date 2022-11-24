[English](SqSchema.md)

# SqSchema
SqSchema 定义数据库架构

	SqEntry
	│
	├--- SqSchema
	│
	└--- SqReentry
	     │
	     ├--- SqTable
	     │
	     └--- SqColumn

SqSchema、SqTable 和 SqColumn 的关系。

	SqSchema ---┬--- SqTable 1 ---┬--- SqColumn 1
	            │                 │
	            │                 └--- SqColumn n
	            │
	            └--- SqTable n ---  ...

# SqTable
SqTable 定义 SQL 表

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

# SqColumn
[SqColumn](SqColumn.cn.md) 定义 SQL 表中的列.

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

## 1 通过函数创建表和列（动态）

建议使用 C++ 方法或 C 函数创建动态表。  
要获取更多信息和示例，您可以查看以下文档：  
1. [database-migrations.cn.md](database-migrations.cn.md)
2. ../[README.cn.md](../README.cn.md#数据库架构) 中的“**数据库架构**”部分

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

C++ 示例代码：

```c++
	// 定义一个 C 结构来映射数据库表 "areas".
	struct Area {
		int     id;          // 主键
		char   *name;
	};

	// 创建架构版本 1
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;        // 指定版本号或自动生成

	// 创建表 "areas"
	table = schema_v1->create<Area>("areas");

	// 向表中添加列
	table->integer("id", &Area::id)->primary();  // 主键
	table->string("name", &Area::name);

	// 做迁移
	storage->migrate(schema_v1);   // 迁移 'schema_v1'

	// 将 schema_v1 同步到数据库并更新 'storage' 中的架构
	// 这主要由 SQLite 使用
	storage->migrate(NULL);

	// 释放未使用的 'schema_v1'
	delete schema_v1;
```

## 2 按现有 SqType 创建表（静态或动态）
您可以查看文档 [SqColumn.cn.md](SqColumn.cn.md) 以获取有关使用 SqColumn 创建 SqType 的更多信息。  
注意: 如果 'type' 是动态 SqType，它会在程序释放 'table' 时被一起释放。

```c++
	// C 函数
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// C++ 方法
	table = schema->create("your_table_name", type);
```

## 3 查找表
创建或迁移表后，用户可以通过表的名称在架构中找到表。

```c++
	// C 函数
	table = sq_schema_find(schema, "users");

	// C++ 方法
	table = schema->find("users");
```
