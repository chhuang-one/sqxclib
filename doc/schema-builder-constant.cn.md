[English](schema-builder-constant.md)

# Schema Builder (常量)

本文档介绍如何使用 C99 指定初始化器（或 C++ 聚合初始化）来定义表。
* 这可以减少制作架构时的运行时间。
* 架构可以处理动态和常量列/表定义。
* 如果用户修改常量列/表，程序将在修改之前复制列/表。
* 程序不会从内存中释放常量列/表。它只是不使用它们。
* 如果您的 SQL 表是固定的并且以后不会更改，您可以通过使用常量 [SqType](SqType.cn.md) 来定义表来减少更多的运行时间。见文件 [SqColumn.cn.md](SqColumn.cn.md)

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

## 创建表（常量）

要定义常量列，最常用的是 [SqColumn](SqColumn.cn.md) 的前四个字段。它们是 type, name, offset 和 bit_field。  

* 字段 'type' 用于指定 C/C++ 数据类型，一般以 SQ_TYPE 开头，参见 [SqType](SqType.cn.md)。
* 字段 'name' 用于指定列名称。
* 字段 'offset' 用于指定结构体中字段的偏移量。
* 字段 'bit_field' 用于指定主键等属性，一般以 SQB 开头，参见 [SqColumn](SqColumn.cn.md)。
* 字段 'foreign' 是以 NULL 结尾的数组，用于设置引用的 表、列、删除时的操作 和 更新时的操作。用空字符串 "" 来分隔 列 和 删除时的操作。
  
字段 'foreign' 在 **关于字段 SqColumn::foreign** 部分中有更多详细信息。  
  
**C99 指定初始化**  
  
例如: 使用 C99 指定初始化程序在 schema_v1 中定义表和列。

```c
#include <sqxclib.h>

static const SqColumn  userColumns[6] = {
	// 主键 PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),
		.size = 60},

	// 外键 FOREIGN KEY
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id")  ON DELETE CASCADE  ON UPDATE SET DEFAULT
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		.foreign = (char *[]) {"cities", "id",  "",  "CASCADE", "SET DEFAULT", NULL}  },
	// 在上一行中， 列 "id" 和 ON DELETE 的操作 "CASCADE" 必须用空字符串 "" 分隔。

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},
};

	/* 创建架构并指定版本号为 1 */
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// 创建表 "users"
	table = sq_schema_create(schema_v1, "users", User);

	// 将具有 6 个元素的 'userColumns' 添加到表中
	sq_table_add_column(table, userColumns, 6);
```

**C++ 聚合初始化**  
  
如果您的 C++ 编译器无法使用指定初始化程序，您可以使用聚合初始化，如下所示。  
  
例如: 使用 C++ 聚合初始化在 schema_v1 中定义表和列。

```c++
#include <sqxclib.h>

/* 为 C++ STL 定义全局类型
   为 C++ std::vector<int> 创建新的 SqType */
Sq::TypeStl< std::vector<int> > SqTypeIntVector(SQ_TYPE_INT);

/* 此外键约束由下面的 userColumns[] 使用
   在下一行中， 列 "id" 和 ON DELETE 的操作 "CASCADE" 必须用空字符串 "" 分隔。 */
static const char *userForeign[] = {"cities", "id",  "",  "CASCADE", "SET DEFAULT", NULL};

static const SqColumn  userColumns[8] = {
	// 主键 PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),      0,
		NULL,                          // .old_name
		0,                             // .sql_type
		60},                           // .size        // VARCHAR(60)

	// 外键 FOREIGN KEY
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id")  ON DELETE CASCADE  ON UPDATE SET DEFAULT
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		NULL,                          // .old_name,
		0, 0, 0,                       // .sql_type, .size, .digits,
		NULL,                          // .default_value,
		(char **) userForeign},        // .foreign

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// C++ 数据类型 std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ 数据类型 std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* 创建架构并指定版本号为 1 */
	schema_v1 = new Sq::Schema(1, "Ver 1");

	// 创建表 "users"
	table = schema_v1->create<User>("users");
	// 将具有 8 个元素的 'userColumns' 添加到表中
	table->addColumn(userColumns, 8);
```

## 更新表（常量）

* 要重命名列，请将字段 'old_name' 设置为当前列名称，并将 'name' 设置为新列名称。
* 要删除列，  请将字段 'old_name' 设置为当前列名称，并将 'name' 设置为 NULL。
* 要更改列属性，请将 SQB_CHANGED 添加到字段 'bit_field'。

例如: 使用 C99 指定初始化程序更改 schema_v2 中的表和列。  
'columnsChanges' 包含要 添加、更改、删除 和 重命名列 的记录。  

```c
static const SqColumn  columnsChanges[4] = {
	// 向表中添加列 ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// 更改表中的列 ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id",  offsetof(User, city_id), SQB_CHANGED},

	// 删除列 DROP COLUMN "name"
	{.old_name = "name",      .name = NULL},

	// 重命名列 RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	/* 创建架构并指定版本号为 2 */
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// 更改表 "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// 通过具有 4 个元素的 'columnsChanges' 更改表
	sq_table_add_column(table, columnsChanges, 4);
```

## 约束 Constraints （常量）

将约束设置到列定义上：
* 要定义主键列，请在 SqColumn::bit_field 中设置 SQB_PRIMARY。
* 要定义外键列，请在 SqColumn::foreign 中设置引用的 表、列、操作。 如果 'foreign' 不为 NULL，则 'bit_field' 中的 SQB_FOREIGN 可以省略。
* 要定义唯一列，请在 SqColumn::bit_field 中设置 SQB_UNIQUE。

```c
static const SqColumn  columns[] = {
	// 主键 PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// 外键 FOREIGN KEY:  如果 'foreign' 不为 NULL，则 'bit_field' 中的 SQB_FOREIGN 可以省略。
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		.foreign = (char *[]) {"cities", "id",  "",  "CASCADE", "CASCADE", NULL}  },
	//	                                             ^^^^^^^^^
	//	.foreign = (char *[]) {"cities", "id",  "",         "", "CASCADE", NULL}  },
	// 外键 FOREIGN KEY:  如果未指定 ON DELETE 操作，请将其设为空字符串 "" ，如上所示。

	// 唯一 UNIQUE
	{SQ_TYPE_STR,    "name",       offsetof(User, name),       SQB_UNIQUE},
}
```

**复合约束**  

* 字段 'type' 必须设置为 SQ_TYPE_CONSTRAINT。 SQ_TYPE_CONSTRAINT 是迁移使用的假数据类型。
* 字段 'composite' 是以 NULL 结尾的数组，用于设置复合约束的 列。
* 字段 'foreign'   是以 NULL 结尾的数组，用于设置引用的 表、列、删除时的操作 和 更新时的操作。用空字符串 "" 来分隔 列 和 删除时的操作。
  
由于外键中的列数必须与引用表中的列数匹配，因此 'foreign' 和 'composite' 字段中的列数必须匹配。  
  
例如: 使用 C99 指定初始化器添加/删除约束。  
'otherChanges1' 添加约束（主键、外键和唯一）。  
'otherChanges2' 删除约束（主键、外键和唯一）。  

```c
static const SqColumn  otherChanges1[] = {
	// 约束主键 CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "other_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"column1", "column2", NULL} },

	// 约束外键 CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,  "other_foreign", 0,  SQB_FOREIGN,
		.foreign   = (char *[]) {"table",  "column1", "column2",  "",  "NO ACTION", "NO ACTION",  NULL},
		.composite = (char *[]) {          "column1", "column2",  NULL} },
	// 由于外键中的列数必须与引用表中的列数匹配，
	// 因此 'foreign' 和 'composite' 字段中的列数必须匹配。

	// 约束唯一 CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "other_unique",  0,  SQB_UNIQUE,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherChanges2[] = {
	// 删除约束主键 DROP CONSTRAINT PRIMARY KEY
	{.old_name = "other_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_PRIMARY },

	// 删除约束外键 DROP CONSTRAINT FOREIGN KEY
	{.old_name = "other_foreign",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_FOREIGN },

	// 删除约束唯一 DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_UNIQUE },
};
```

**关于字段 SqColumn::foreign**

* 它是 NULL 结尾的数组。
* 用于设置引用的 表、列、删除时的操作 (ON DELETE) 和 更新时的操作 (ON UPDATE)。
* 列 和 删除时的操作 (ON DELETE) 必须用空字符串 "" 分隔。
* 如果未指定 删除时的操作 (ON DELETE) 或 更新时的操作 (ON UPDATE)，可以设置为空字符串 ""。

```c
	// ON DELETE 操作 和 列 必须用空字符串 "" 分隔。
	// ON DELETE 操作为 "CASCADE"
	// ON UPDATE 操作为 "SET DEFAULT"
	.foreign   = (char *[]) {"table", "column",  "",  "CASCADE", "SET DEFAULT", NULL},

	// ON DELETE 操作 和 列 必须用空字符串 "" 分隔。
	// 未指定 ON DELETE 操作，因为它设置为空字符串 ""。
	// ON UPDATE 操作为 "SET DEFAULT"
	.foreign   = (char *[]) {"table", "column",  "",         "", "SET DEFAULT", NULL},

	// ON DELETE 操作 "CASCADE" 和 列 必须用空字符串 "" 分隔。
	// 未指定 ON UPDATE 操作，因为最后一个元素为 NULL。
	.foreign   = (char *[]) {"table", "column",  "",  "CASCADE", NULL},

	// 仅指定 表 和 列
	// 未指定 ON DELETE 操作 和 ON UPDATE 操作，因为最后一个元素为 NULL。
	.foreign   = (char *[]) {"table", "column", NULL},

	// 指定 表 和 多列
	// 未指定 ON DELETE 操作 和 ON UPDATE 操作，因为最后一个元素为 NULL。
	.foreign   = (char *[]) {"table", "column1", "column2", NULL},
```

## 索引 Index （常量）

* 字段 'type' 必须设置为 SQ_TYPE_INDEX。 SQ_TYPE_INDEX 是迁移使用的假数据类型。
* 字段 'composite' 是以 NULL 结尾的数组，用于设置复合索引。

例如: 使用 C99 指定的初始值设定项来添加/删除索引  
'otherChanges3' 添加索引。  
'otherChanges4' 删除索引。  

```c
static const SqColumn  otherChanges3[] = {
	// 创建索引 CREATE INDEX
	{SQ_TYPE_INDEX,  "other_index", 0,  0,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherChanges4[] = {
	// 删除索引 DROP INDEX
	{.old_name = "other_index",  .name = NULL,
	 .type = SQ_TYPE_INDEX },
};
```

## 使用自订或 JSON 型态 (常量)

如果要在 SQL 列中存储 JSON 对象或数组，必须指定 [SqType](SqType.cn.md)。  
  
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

static const SqColumn  demoTableColumns[] = {
	// ...

	// SQ_TYPE_MY_STRUCT 是用户定义的 MyStructure 的 SqType。
	// 如果未指定 SQL 类型，JSON 对象将存储在 SQL VARCHAR 列中。
	{SQ_TYPE_MY_STRUCT,    "myStruct",       offsetof(DemoTable, myStruct)},

	// SQ_TYPE_INT_ARRAY 在 SqType.h 中声明，并从 SQL 列的值解析 JSON 整数数组。
	// 如果未指定 SQL 类型，JSON 整数数组将存储在 SQL VARCHAR 列中。
	{SQ_TYPE_INT_ARRAY,    "intArray",       offsetof(DemoTable, intArray)},
};
```

## 仅查询列 (常量)

仅查询列名仅适用于 SQL SELECT 查询。您可以将 SqColumn::name 设置为 SELECT 查询，并在 SqColumn::bit_field 中设置 SQB_QUERY_ONLY 以定义仅查询列。这可以将特殊查询的结果（如“SELECT length(BlobColumn), * FROM table”）存储到 C 结构的成员中。  
  
**注意**：如果要使用此功能，请在 SqConfig.h 中启用 SQ_CONFIG_QUERY_ONLY_COLUMN。  
  
例如: 定义列，用于在查询数据时将 SQL 语句 'SELECT length(str), * FROM table' 的结果存储到 C 结构的成员中。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct  QueryFirst    QueryFirst;

struct QueryFirst
{
	int    id;

	char  *str;

	// 'str' 的长度
	// SQL 语句: SELECT length(str), * FROM table
	int    str_length;
};

static const SqColumn  queryFirstColumns[3] = {
	// 主键
	{SQ_TYPE_INT,    "id",          offsetof(QueryFirst, id),         SQB_PRIMARY},

	// 仅查询列：
	// 1. 列名称是 SELECT 查询
	// 2. SqColumn::bit_field 具有 SQB_QUERY_ONLY
	{SQ_TYPE_INT,    "length(str)", offsetof(QueryFirst, str_length), SQB_QUERY_ONLY},

	// VARCHAR
	{SQ_TYPE_STR,    "str",         offsetof(QueryFirst, str),        0},
};
```

**使用相同的列名连接多个表**  
  
如果使用相同的列名连接多个表并使用仅查询列和函数，则可以在函数参数的列名中添加表名作为前缀。

```c
	// 将 "length(ColumnName)" 更改为 "length(TableName.ColumnName)"
	{SQ_TYPE_INT,    "length(TableName.ColumnName)", offsetof(TableName, ColumnName), SQB_QUERY_ONLY},
```

如果您定义具有仅查询列的常量 SqType。 SqType::bit_field 必须具有 SQB_TYPE_QUERY_FIRST。

```c++
// queryFirstColumns 的 SqColumn 常量指针数组
static const SqColumn *queryFirstColumnPtrs[3] = {
	&queryFirstColumns[0],
	&queryFirstColumns[1],
	&queryFirstColumns[2],
};

// SqType::bit_field 必须具有 SQB_TYPE_QUERY_FIRST
const SqType  queryFirstType = SQ_TYPE_INITIALIZER(QueryFirst, queryFirstColumnPtrs, SQB_TYPE_QUERY_FIRST);
```

## 类型映射（常量）

用户可以在 SqColumn::sql_type 中指定 SQL 数据类型，该数据类型将映射到 SqColumn::type 中指定的 C 数据类型。
如果 SqColumn::sql_type 等于 0，程序将尝试从 SqColumn::type 确定 SQL 数据类型。
SQL 数据类型在 [SqTable](SqTable.cn.md) 中列出。  
  
如果使用非内置 [SqType](SqType.cn.md) 定义常量 [SqColumn](SqColumn.cn.md)，则必须使用类型映射。
由于某些 SQL 类型（例如 **BLOB**、**CLOB** 和 **TEXT**）没有默认的内置 SqType，因此用户必须指定哪个 SqType 映射到这些 SQL 数据类型。  
  
示例1: 使用 C SqBuffer 来存储 SQL BLOB 数据。

```c
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct  Mapping    Mapping;

struct Mapping
{
	int       id;

	// 类型映射
	char     *text;

	// 类型映射 + 仅查询列
	// 在解析 'picture' 之前在 SqBuffer::size 中指定 BLOB 的长度
	SqBuffer  picture;
};

static const SqColumn  mappingColumns[4] = {
	// 主键
	{SQ_TYPE_INT,    "id",              offsetof(Mapping, id),   SQB_PRIMARY},

	// 类型映射：SQ_TYPE_STR 映射到 SQL 数据类型 - TEXT
	{SQ_TYPE_STR,    "text",            offsetof(Mapping, text), 0,
		.sql_type = SQ_SQL_TYPE_TEXT},

	// 仅查询列：SqColumn::bit_field 必须具有 SQB_QUERY_ONLY
	// 在解析 'picture' 之前在 SqBuffer::size 中指定 BLOB 的长度。
	// 使用它来获取 SQLite 或 MySQL 的 BLOB 数据的长度。
	{SQ_TYPE_INT,    "length(picture)", offsetof(Mapping, picture) + offsetof(SqBuffer, size), SQB_QUERY_ONLY},

	// 类型映射：SQ_TYPE_BUFFER 映射到 SQL 数据类型 - BLOB
	{SQ_TYPE_BUFFER, "picture",         offsetof(Mapping, picture),    0,
		.sql_type = SQ_SQL_TYPE_BLOB},
};
```

示例2: 使用 C++ std::vector<char> 来存储 SQL BLOB 数据。  
  
注意: SQ_TYPE_STD_VECTOR_SIZE 在解析整数值时将通过调用 std::vector<char>::resize() 来指定 BLOB 的大小。

```c++
struct MappingCpp
{
	int       id;

	// 类型映射
	std::string  text;

	// 类型映射 + 仅查询列
	// 在解析 'picture' 之前调用 std::vector<char>::resize() 来指定 BLOB 的长度。
	std::vector<char>  picture;
};

static const SqColumn  mappingCppColumns[4] = {
	// 主键
	{SQ_TYPE_INT,     "id",              offsetof(MappingCpp, id),   SQB_PRIMARY},

	// 类型映射：SQ_TYPE_STD_STR 映射到 SQL 数据类型 - TEXT
	{SQ_TYPE_STD_STR, "text",            offsetof(MappingCpp, text), 0,
		.sql_type = SQ_SQL_TYPE_TEXT},

	// 仅查询列：SqColumn::bit_field 必须具有 SQB_QUERY_ONLY
	// 在解析 'picture' 之前调用 std::vector<char>::resize() 来指定 BLOB 的长度。
	// 使用它来获取 SQLite 或 MySQL 的 BLOB 数据的长度。
	{SQ_TYPE_STD_VECTOR_SIZE, "length(picture)", offsetof(MappingCpp, picture), SQB_QUERY_ONLY},

	// 类型映射：SQ_TYPE_STD_VECTOR 映射到 SQL 数据类型 - BLOB
	{SQ_TYPE_STD_VECTOR,      "picture",         offsetof(MappingCpp, picture), 0,
		.sql_type = SQ_SQL_TYPE_BLOB},
};
```

## 迁移

无论是动态定义还是常量定义，运行迁移的代码都是一样的。  
  
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
