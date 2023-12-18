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
  
使用 C 语言

```c
	typedef struct  User    User;

	SqTable *table;
	SqType  *tableType = NULL;

	// 使用 SqSchema 的函数创建表（推荐）
	table = sq_schema_create(schema, "users", User);

	// 使用 sq_table_new() 创建表
	table = sq_table_new("users", tableType);
	sq_schema_add(schema, table);
```

使用 C++ 语言

```c++
	typedef struct  User    User;

	Sq::Table *table;
	Sq::Type  *tableType = NULL;

	// 使用 Sq::Schema 的方法创建表（推荐）
	table = schema->create<User>("users");

	// 使用 Sq::Table 的构造函数创建表
	table = new Sq::Table("users", tableType);
	schema->add(table);
```

## 创建列

下面的 C++ 方法 (C 函数) 对应于您可以添加到表中的不同类型的列。  
大多数方法（函数）都会为列设置特定的 [SqType](SqType.cn.md)。

| C++ 方法    | C 函数                 | C 数据类型     | SQL 数据类型      | 指定的 SqType       |
| ----------- | ---------------------- | -------------- | ----------------- | ------------------- |
| boolean     | sq_table_add_boolean   | bool           | BOOLEAN           | SQ_TYPE_BOOL        |
| bool_       | sq_table_add_bool      | bool           | BOOLEAN           | SQ_TYPE_BOOL        |
| integer     | sq_table_add_integer   | int            | INT               | SQ_TYPE_INT         |
| int_        | sq_table_add_int       | int            | INT               | SQ_TYPE_INT         |
| uint        | sq_table_add_uint      | unsigned int   | INT (UNSIGNED)    | SQ_TYPE_UINT        |
| int64       | sq_table_add_int64     | int64_t        | BIGINT            | SQ_TYPE_INT64       |
| uint64      | sq_table_add_uint64    | uint64_t       | BIGINT (UNSIGNED) | SQ_TYPE_UINT64      |
| timestamp   | sq_table_add_timestamp | time_t         | TIMESTAMP         | SQ_TYPE_TIME        |
| timestamps  | sq_table_add_timestamps| time_t &ensp; x 2 | TIMESTAMP &ensp; x 2 | SQ_TYPE_TIME &ensp; x 2 |
| double_     | sq_table_add_double    | double         | DOUBLE            | SQ_TYPE_DOUBLE      |
| str         | sq_table_add_str       | char*          | VARCHAR           | SQ_TYPE_STR         |
| string      | sq_table_add_string    | char*          | VARCHAR           | SQ_TYPE_STR         |
| char_       | sq_table_add_char      | char*          | CHAR              | SQ_TYPE_CHAR        |
| text        | sq_table_add_text      | char* &ensp; 默认值 | TEXT         | SQ_TYPE_STR &ensp; 默认值 |
| clob        | sq_table_add_clob      | char* &ensp; 默认值 | CLOB         | SQ_TYPE_STR &ensp; 默认值 |
| blob        | sq_table_add_blob      | SqBuffer       | BLOB 或 BINARY    | SQ_TYPE_BUFFER      |
| binary      | sq_table_add_binary    | SqBuffer       | BLOB 或 BINARY    | SQ_TYPE_BUFFER      |
| custom      | sq_table_add_custom    | *用户定义*     | VARCHAR           | *用户定义*          |
| mapping     | sq_table_add_mapping   | *用户指定*     | *用户指定*        | *用户指定*          |

* 因为 'bool'、'int'、'double' 和 'char' 是 C/C++ 关键字，所以在这些方法的尾部附加 '_'。
* 某些方法/函数（例如 boolean, integer, string 和 binary）具有较短的别名。
* timestamps() 方法用于添加 2 个常用的时间戳列 - created_at 和 updated_at。
* text() 和 clob() 方法可以由用户指定 SqType，或使用默认的类型 SQ_TYPE_STR。

以下方法仅适用于 C++ 数据类型。

| C++ 方法    | C++ 数据类型      | SQL 数据类型      | 指定的 SqType      |
| ----------- | ----------------- | ----------------- | ------------------ |
| stdstring   | std::string       | VARCHAR           | SQ_TYPE_STD_STRING |
| stdstr      | std::string       | VARCHAR           | SQ_TYPE_STD_STRING |
| stdvector   | std::vector<char> | BLOB 或 BINARY    | SQ_TYPE_STD_VECTOR |
| stdvec      | std::vector<char> | BLOB 或 BINARY    | SQ_TYPE_STD_VECTOR |

* stdstr 和 stdvec 是 stdstring 和 stdvector 的较短别名。

**使用自订或 JSON 型态**  
  
如果要在 SQL 列中存储 JSON 对象或数组，可以使用 sq_table_add_custom() 或 C++ 方法 custom() 来创建列。  
添加使用自定义类型的列时，必须指定 [SqType](SqType.cn.md)。
  
例如: 定义一个包含结构和数组的 C 结构。

```c
struct DemoTable {
	// ...

	// MyStructure 是用户定义的 C 结构
	// 这会将 JSON 对象存储在 SQL 列中。
	MyStructure    myStruct;

	// SqIntArray 是在 SqArray.h 中定义的整数数组
	// 这会将 JSON 整数数组存储在 SQL 列中。
	SqIntArray     intArray;
};
```

在下面的示例中，
SQ_TYPE_MY_STRUCT 是用户定义的 MyStructure 的 SqType。  
SQ_TYPE_INT_ARRAY 在 SqType.h 中声明，并从 SQL 列的值解析 JSON 整数数组。  
  
使用 C 语言

```c
	// JSON 对象将存储在 SQL VARCHAR 列中。
	column = sq_table_add_custom(table, "myStruct", offsetof(DemoTable, myStruct),
	                             SQ_TYPE_MY_STRUCT, 128);

	// JSON 整数数组将存储在 SQL VARCHAR 列中。
	column = sq_table_add_custom(table, "intArray", offsetof(DemoTable, intArray),
	                             SQ_TYPE_INT_ARRAY, 96);
```

使用 C++ 语言

```c++
	// JSON 对象将存储在 SQL VARCHAR 列中。
	column = table->custom("myStruct", offsetof(DemoTable, myStruct),
	                       SQ_TYPE_MY_STRUCT, 128);

	// JSON 整数数组将存储在 SQL VARCHAR 列中。
	column = table->custom("intArray", offsetof(DemoTable, intArray),
	                       SQ_TYPE_INT_ARRAY, 96);
```

如果您不想使用 offsetof()，也可以使用指向成员的指针。

```c++
	// 使用 '&DemoTable::myStruct' 替换 'offsetof(DemoTable, myStruct)'
	column = table->custom("myStruct", &DemoTable::myStruct,
	                       SQ_TYPE_MY_STRUCT, 128);

	// 使用 '&DemoTable::intArray' 替换 'offsetof(DemoTable, intArray)'
	column = table->custom("intArray", &DemoTable::intArray,
	                       SQ_TYPE_INT_ARRAY, 96);
```

**使用类型映射**  
  
要使用类型映射，您必须在创建列时将 [SqType](SqType.cn.md) 映射到 SQL 数据类型。
当然你也可以映射到用户自定义的 [SqType](SqType.cn.md)。  
  
下面是库定义的用于类型映射的常见 SQL 数据类型。

| 库定义的 SQL 数据类型           | 库提供的 SqType                     |
| ------------------------------- | ----------------------------------- |
| SQ_SQL_TYPE_BOOLEAN             | SQ_TYPE_BOOL                        |
| SQ_SQL_TYPE_INT                 | SQ_TYPE_INT                         |
| SQ_SQL_TYPE_INT_UNSIGNED        | SQ_TYPE_UINT                        |
| SQ_SQL_TYPE_BIGINT              | SQ_TYPE_INT64                       |
| SQ_SQL_TYPE_BIGINT_UNSIGNED     | SQ_TYPE_UINT64                      |
| SQ_SQL_TYPE_TIMESTAMP           | SQ_TYPE_TIME                        |
| SQ_SQL_TYPE_DOUBLE              | SQ_TYPE_DOUBLE                      |
| SQ_SQL_TYPE_VARCHAR             | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_CHAR                | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_TEXT                | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_CLOB                | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_BLOB                | SQ_TYPE_BUFFER, SQ_TYPE_STD_VECTOR  |
| SQ_SQL_TYPE_DECIMAL             | SQ_TYPE_DOUBLE, SQ_TYPE_INT         |
| SQ_SQL_TYPE_TINYINT             | SQ_TYPE_INT                         |
| SQ_SQL_TYPE_TINYINT_UNSIGNED    | SQ_TYPE_UINT                        |
| SQ_SQL_TYPE_SMALLINT            | SQ_TYPE_INT                         |
| SQ_SQL_TYPE_SMALLINT_UNSIGNED   | SQ_TYPE_UINT                        |
| SQ_SQL_TYPE_MEDIUMINT           | SQ_TYPE_INT                         |
| SQ_SQL_TYPE_MEDIUMINT_UNSIGNED  | SQ_TYPE_UINT                        |
| SQ_SQL_TYPE_TINYTEXT            | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_MEDIUMTEXT          | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |
| SQ_SQL_TYPE_LONGTEXT            | SQ_TYPE_STRING, SQ_TYPE_STD_STRING  |

以下是 SQL 数据类型的别名:

| 库定义的 SQL 数据类型           | SQ_SQL_XXXX 的别名                      |
| ------------------------------- | --------------------------------------- |
| SQ_SQL_TYPE_BINARY              | SQ_SQL_TYPE_BLOB 的别名                 |
| SQ_SQL_TYPE_UNSIGNED_INT        | SQ_SQL_TYPE_INT_UNSIGNED 的别名         |
| SQ_SQL_TYPE_UNSIGNED_BIGINT     | SQ_SQL_TYPE_BIGINT_UNSIGNED 的别名      |
| SQ_SQL_TYPE_UNSIGNED_TINYINT    | SQ_SQL_TYPE_TINYINT_UNSIGNED 的别名     |
| SQ_SQL_TYPE_UNSIGNED_SMALLINT   | SQ_SQL_TYPE_SMALLINT_UNSIGNED 的别名    |
| SQ_SQL_TYPE_UNSIGNED_MEDIUMINT  | SQ_SQL_TYPE_MEDIUMINT_UNSIGNED 的别名   |

示例: 将 SqType SQ_TYPE_INT_ARRAY 映射到 SQL 数据类型 TEXT  
  
使用 C 语言

```c
	// JSON 整数数组将存储在 SQL TEXT 列中。
	column = sq_table_add_mapping(table, "intArray", offsetof(DemoTable, intArray),
	                              SQ_TYPE_INT_ARRAY,
	                              SQ_SQL_TYPE_TEXT);
```

使用 C++ 语言

```c++
	// JSON 整数数组将存储在 SQL TEXT 列中。
	column = table->mapping("intArray", offsetof(DemoTable, intArray),
	                        SQ_TYPE_INT_ARRAY,
	                        SQ_SQL_TYPE_TEXT);
```

## 添加列

C 函数 sq_table_add_column() 和 sq_table_add_column_ptrs(), C++ 方法 addColumn() 可以添加现有列。  
  
sq_table_add_column(), addColumn()      可以添加 SqColumn 数组。  
sq_table_add_column_ptrs(), addColumn() 可以添加 SqColumn 指针数组。  
  
您可以查看 [schema-builder-static.cn.md](schema-builder-static.cn.md) 以获取更多信息和示例。

## 删除列

sq_table_drop_column() 必须指定要删除的列名。这不会在迁移之前删除该列，因为它只是向 'table' 中添加了一条 "删除列" 的记录。

```c++
	// C 函数
	sq_table_drop_column(table, "columnName");

	// C++ 方法
	table->dropColumn("columnName");
```

## 重命名列

与 sq_table_drop_column() 一样，sq_table_rename_column() 不会立即重命名列。

```c++
	// C 函数
	sq_table_rename_column(table, "old_name", "new_name");

	// C++ 方法
	table->renameColumn("old_name", "new_name");
```

## 获取列

C 函数 sq_table_find_column()，C++ 方法 findColumn() 可以按名称查找列。

```c++
	// C 函数
	column = sq_table_find_column(table, "columnName");

	// C++ 方法
	column = table->findColumn("columnName");
```

C 函数 sq_table_get_primary()，C++ 方法 getPrimary() 可以获取主键。

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

**删除外键 Drop foreign key**  
  
要删除外键，您必须指定外键的名称。

```c++
	// C 函数
	column = sq_table_drop_foreign(table, "foreignName");

	// C++ 方法
	column = table->dropForeign("foreignName");
```
