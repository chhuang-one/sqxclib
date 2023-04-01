[English](SqColumn.md)

# SqColumn

SqColumn 派生自 [SqEntry](SqEntry.cn.md)。它定义 SQL 表中的列并与 SqType 一起使用。

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

## 创建列（动态）

SqColumn 必须与 [SqTable](SqTable.cn.md) and [SqSchema](SqSchema.cn.md) 一起使用来创建表。
它使用 [SqType](SqType.cn.md) 指定数据类型。  
要获取更多信息和示例，您可以查看以下文档：  
1. [database-migrations.cn.md](database-migrations.cn.md)
2. ../[README.cn.md](../README.cn.md#数据库架构) 中的 "**数据库架构**" 部分
3. 所有内置类型都在 [SqType.cn.md](SqType.cn.md) 中列出
  
使用 C 语言

```c
	SqColumn *column;

	// 使用 SqTable 的函数创建列（推荐）
	column = sq_table_add_string(table, "column", offsetof(MyStruct, column), 191);

	// 使用 sq_column_new() 创建列 （这里必须指定 SqType）
	column = sq_column_new("id", SQ_TYPE_INT);
	column->offset = offsetof(MyStruct, id);
	sq_table_add_column(table, column, 1);
```

使用 C++ 语言

```c++
	Sq::Column *column;

	// 使用 Sq::Table 的方法创建列（推荐）
	column = table->string("column", &MyStruct::column, 191);

	// 使用 Sq::Column 的构造函数创建列 （这里必须指定 SqType）
	column = new Sq::Column("id", SQ_TYPE_INT);
	column->offset = offsetof(MyStruct, id);
	table->addColumn(column);
```

## 列修饰符

在将列添加到表或将条目添加到结构时，您可以使用几个 "修饰符"。  
以下 C++ 方法 (和 C 函数) 对应于列修饰符：

| C++ 方法             | C 函数                             | C 位字段名            |
| -------------------- | ---------------------------------- | --------------------- |
| primary()            | sq_column_primary()                | SQB_PRIMARY           |
| unique()             | sq_column_unique()                 | SQB_UNIQUE            |
| autoIncrement()      | sq_column_auto_increment()         | SQB_AUTOINCREMENT     |
| nullable()           | sq_column_nullable()               | SQB_NULLABLE          |
| useCurrent()         | sq_column_use_current()            | SQB_CURRENT           |
| useCurrentOnUpdate() | sq_column_use_current_on_update()  | SQB_CURRENT_ON_UPDATE |
| default_(string)     | sq_column_default()                |                       |

* 因为 "default" 是 C/C++ 关键字，所以在此方法的尾部附加 "_"。

结构类型的特殊方法。

| C++ 方法         | C 位字段名        | 描述                                               |
| ---------------- | ----------------- | -------------------------------------------------- |
| pointer()        | SQB_POINTER       | 这个数据成员是一个指针。                           |
| hidden()         | SQB_HIDDEN        | 不要将此数据成员输出到 JSON。                      |
| hiddenNull()     | SQB_HIDDEN_NULL   | 如果它的值为 NULL，则不要将此数据成员输出到 JSON。 |

例如，使列 "nullable":

```c++
	/* C++ 示例代码 */
	table->string("name", &User::name)->nullable();

	/* C 示例代码 */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	sq_column_nullable(column);
```

## 更改列

C 函数 sq_column_change()、C++ 方法 change() 允许您修改现有列的类型和属性。他们在迁移之前不会更改列。  
  
使用 C 语言

```c
	// 更改表 "users"
	table = sq_schema_alter(schema, "users", NULL);

	// 更改表中的 "email" 列
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	sq_column_change(column);
```

使用 C++ 语言

```c++
	// 更改表 "users"
	table = schema->alter("users");

	// 更改表中的 "email" 列
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

## 将 SqColumn 与 SqType 一起使用

要定义常量 SqColumn，用户必须了解 SqColumn 结构定义：

```c
struct SqColumn
{
	// ------ SqEntry 成员 ------
	const SqType *type;             // 字段类型
	const char   *name;             // 列名称
	size_t        offset;           // 结构中字段的偏移量
	unsigned int  bit_field;        // 下面解释


	// ------ SqReentry 成员 ------
	const char   *old_name;         // 重命名或删除时使用这个


	// ------ SqColumn 成员 ------

	// size  : 总位数以大小或字符串长度
	int16_t       size;
	// digits: 小数点后的位数。
	int16_t       digits;

	const char   *default_value;    // DEFAULT

	SqForeign    *foreign;          // 外键
	char        **composite;        // Null-terminated （列名）字符串数组

	const char   *reserve;          // 保留
	const char   *raw;              // 原始 SQL 列属性
};
```

声明 SqColumn 使用的 bit_field：

| 名称                   | 描述                                          | 
| ---------------------- | --------------------------------------------- |
| SQB_PRIMARY            | SQL 属性: PRIMARY KEY                         |
| SQB_FOREIGN            | SQL 属性: FOREIGN KEY                         |
| SQB_UNIQUE             | SQL 属性: UNIQUE                              |
| SQB_INCREMENT          | SQL 属性: AUTOINCREMENT                       |
| SQB_AUTOINCREMENT      | SQL 属性: 和 SQB_INCREMENT 一样               |
| SQB_NULLABLE           | SQL 属性: 不要使用 "NOT NULL"                 |
| SQB_CURRENT            | SQL 属性: DEFAULT CURRENT_TIMESTAMP           |
| SQB_CURRENT_ON_UPDATE  | SQL 属性: 更新记录时使用 CURRENT_TIMESTAMP    |
| SQB_RENAMED            | 列或表已重命名。                              |
| SQB_CHANGED            | 列或表已更改。                                |

* SqColumn 也继承了 [SqEntry](SqEntry.cn.md) 中 bit_field 的定义。

#### 定义由常量 SqType 使用的常量 SqColumn（静态）

如果您的 SQL 表是固定的并且将来不会更改，这可以减少创架构时的运行时间。  
* 注意: 如果为结构定义常量 SqType，它必须与 SqColumn 的**指针数组**一起使用。

```c++
static const SqColumn  columnArray[2] = {
	{SQ_TYPE_UINT,   "id",         offsetof(YourStruct, id),         0},
	{SQ_TYPE_STR,    "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

static const SqColumn *columnPointerArray[2] = {
	& columnArray[0],
	& columnArray[1],
};

/* 如果 'columnPointerArray' 未按名称排序，则将 0 传递给最后一个参数。
   否则使用 SQB_TYPE_SORTED 替换最后一个参数中的 0。
 */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, columnPointerArray, 0);
```

#### 定义由动态 SqType 使用的常量 SqColumn

使用 C 语言

```c
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);

	// 从指针数组中添加 2 列
	sq_type_add_entry_ptrs(type, (const SqEntry**)columnPointerArray, 2);

	// 从数组中添加 2 列（不是指针数组）
//	sq_type_add_entry(type, (const SqEntry*)columnArray, 2, sizeof(SqColumn));
```

使用 C++ 语言

```c++
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);

	// 从指针数组中添加 2 列
	type->addEntry((const SqEntry**)columnPointerArray, 2);

	// 从数组中添加 2 列（不是指针数组）
//	type->addEntry((const SqEntry*)columnArray, 2, sizeof(SqColumn));
```

#### 创建由动态 SqType 使用的动态 SqColumn

添加一个动态列来输入

* 注意: **不推荐**使用这种方式。

使用 C 语言

```c
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STR);
	column->offset = offsetof(YourStruct, FieldName);
	sq_column_primary(column);           // 设置主键 PRIMARY KEY

	/* 將列添加到 SqType */
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));
```

使用 C++ 语言

```c++
	Sq::Column *column = new Sq::Column("your_column_name", SQ_TYPE_STR);
	column->offset = offsetof(YourStruct, FieldName);
	column->primary();                   // 设置主键 PRIMARY KEY

	/* 將列添加到 Sq::Type */
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);
	type->addEntry((const SqEntry*)column, 1, sizeof(SqColumn));
```
