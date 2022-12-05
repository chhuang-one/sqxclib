[English](SqTable.md)

# SqTable

SqTable 派生自 [SqEntry](SqEntry.cn.md)。它定义 SQL 表。

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

## 创建表

SqTable 必须与 [SqSchema](SqSchema.cn.md) 和 [SqColumn](SqColumn.cn.md) 一起使用来创建表。您可以查看以下文档获取更多信息和示例：  
1. [database-migrations.cn.md](database-migrations.cn.md)
2. ../[README.cn.md](../README.cn.md#数据库架构) 中的 "**数据库架构**" 部分

## 创建列

下面的 C++ 方法 (和 C 函数)对应于您可以添加到表中的不同类型的列。

| C++ 方法    | C 函数                  | C 数据类型    |
| ----------- | ----------------------- | ------------- |
| boolean     | sq_table_add_bool       | bool          |
| bool_       | sq_table_add_bool       | bool          |
| integer     | sq_table_add_int        | int           |
| int_        | sq_table_add_int        | int           |
| uint        | sq_table_add_uint       | unsigned int  |
| int64       | sq_table_add_int64      | int64_t       |
| uint64      | sq_table_add_uint64     | uint64_t      |
| timestamp   | sq_table_add_timestamp  | time_t        |
| double_     | sq_table_add_double     | double        |
| string      | sq_table_add_string     | char*         |
| char_       | sq_table_add_char       | char*         |
| custom      | sq_table_add_custom     | *User define* |

* 因为 'bool'、'int'、'double' 和 'char' 是 C/C++ 关键字，所以在这些方法的尾部附加 '_'。

以下方法仅适用于 C++ 数据类型。

| C++ 方法    | C 数据类型    |
| ----------- | ------------- |
| stdstring   | std::string   |

**加入自訂或 JSON 型態**  
  
添加包含 JSON 对象或数组的列。  
  
例如：定义一个包含结构和数组的 C 结构。

```c
struct User {
	// ...

	// MyStructure 是 C 结构
	MyStructure    myStruct;

	// SqIntptrArray 是在 SqPtrArray.h 中定义的整数数组
	SqIntptrArray  posts;
};
```

使用 C 语言

```c
	// JSON 对象
	column = sq_table_add_custom(table, "myStruct", offsetof(User, myStruct),
	                             SQ_TYPE_MY_STRUCT, 128);

	// JSON 整数数组
	column = sq_table_add_custom(table, "posts", offsetof(User, posts),
	                             SQ_TYPE_INTPTR_ARRAY, 256);
```

使用 C++ 语言

```c++
	// JSON 对象
	column = table->custom("myStruct", offsetof(User, myStruct),
	                       SQ_TYPE_MY_STRUCT, 128);

	// JSON 整数数组
	column = table->custom("posts", offsetof(User, posts),
	                       SQ_TYPE_INTPTR_ARRAY, 256);
```

## 添加列

C 函数 sq_table_add_column() 和 sq_table_add_column_ptrs(), C++ 方法 addColumn() 可以添加现有列。它们通常用于添加静态 SqColumn 数组。  
  
sq_table_add_column(), addColumn()      可以添加 SqColumn 数组。  
sq_table_add_column_ptrs(), addColumn() 可以添加 SqColumn 指针数组。  
  
您可以查看 [schema-builder-static.cn.md](schema-builder-static.cn.md) 以获取更多信息和示例。

## 删除列

sq_table_drop_column() 必须指定要删除的列名。这不会在迁移前删除该列，因为它只是向 'table' 中添加了一条 "删除列" 的记录。

```c++
	// C 函数
	sq_table_drop_column(table, "columnName");

	// C++ 方法
	table->dropColumn("columnName");
```

## 重命名列

与 sq_table_drop_column() 一样， sq_table_rename_column() 不会立即重命名列。

```c++
	// C 函数
	sq_table_rename_column(table, "old_name", "new_name");

	// C++ 方法
	table->renameColumn("old_name", "new_name");
```

## 获取列

C 函数 sq_table_find_column()， C++ 方法 findColumn() 可以按名称查找列。

```c++
	// C 函数
	column = sq_table_find_column(table, "columnName");

	// C++ 方法
	column = table->findColumn("columnName");
```

C 函数 sq_table_get_primary()， C++ 方法 getPrimary() 可以获取主键。

```c++
	// C 函数
	column = sq_table_get_primary(table, NULL);

	// C++ 方法
	column = table->getPrimary();
```

## 索引（复合） Index (composite)

**创建索引**  
  
要创建复合索引，您可以将多个列传递给 index() 方法。
如果使用 C 函数 sq_table_add_index() 创建索引，则最后一个参数必须为 NULL。

```c++
	// C 函数
	column = sq_table_add_index(table, "indexName", "columnName01", "columnName02", NULL);

	// C++ 方法
	column = table->index("indexName", "columnName01", "columnName02");
```

**删除索引**  
  
要删除索引，您必须指定索引的名称。

```c++
	// C 函数
	column = sq_table_drop_index(table, "indexName");

	// C++ 方法
	column = table->dropIndex("indexName");
```

## 唯一（复合） Unique (composite)

**创建唯一约束 Create unique**  
  
要创建复合唯一，您可以将多个列传递给 unique() 方法。
如果使用 C 函数 sq_table_add_unique() 创建 unique，则最后一个参数必须为 NULL。

```c++
	// C 函数
	column = sq_table_add_unique(table, "uniqueName", "columnName01", "columnName02", NULL);

	// C++ 方法
	column = table->unique("uniqueName", "columnName01", "columnName02");
```

**删除唯一约束 Drop unique**  
  
要删除唯一项，您必须指定唯一项的名称。

```c++
	// C 函数
	column = sq_table_drop_unique(table, "uniqueName");

	// C++ 方法
	column = table->dropUnique("uniqueName");
```

## 主键（复合） Primary key (composite)

**创建主键 Create primary key**  
  
要创建复合主键，您可以将多个列传递给 primary() 方法。
如果使用 C 函数 sq_table_add_primary() 创建主键，则最后一个参数必须为 NULL。

```c++
	// C 函数
	column = sq_table_add_primary(table, "primaryName", "columnName01", "columnName02", NULL);

	// C++ 方法
	column = table->primary("primaryName", "columnName01", "columnName02");
```

**删除主键 Drop primary key**  
  
要删除主键，您必须指定主键的名称。

```c++
	// C 函数
	column = sq_table_drop_primary(table, "primaryName");

	// C++ 方法
	column = table->dropPrimary("primaryName");
```

## 外键 Foreign Key

**创建外键 Create foreign key**  
  
要创建外键，您可以将列传递给 foreign() 方法。

```c++
	// C 函数
	column = sq_table_add_foreign(table, "foreignName", "columnName");

	// C++ 方法
	column = table->foreign("foreignName", "columnName");
```

***删除外键 Drop foreign key**  
  
要删除外键，您必须指定外键的名称。

```c++
	// C 函数
	column = sq_table_drop_foreign(table, "foreignName");

	// C++ 方法
	column = table->dropForeign("foreignName");
```
