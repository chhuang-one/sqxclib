[English](SqColumn.md)

# SqColumn

SqColumn 派生自 [SqEntry](SqEntry.cn.md)，它定义 SQL 表中的列。
当它与 SqType 一起工作时，它与 SqEntry 非常相似。

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

结构定义:

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
	const char   *check;            // CHECK (condition)

	SqForeign    *foreign;          // 外键
	char        **composite;        // Null-terminated （列名）字符串数组

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

* 所有需要注意的事项与 [SqEntry](SqEntry.cn.md) 相同。

## 1. 通过方法和函数创建表和列（动态）

建议使用 C++ 方法或 C 函数来创建动态表和列。  
要获取更多信息和示例，您可以查看以下文档：  
1. [database-migrations.md](database-migrations.md)
2. ../[README.md](../README.md#database-schema) 中的 "**数据库架构**" 部分

## 2. 定义由常量 SqType 使用的常量 SqColumn（静态）

如果您的 SQL 表是固定的并且将来不会更改，这可以减少创架构时的运行时间。  
* 注意：如果为结构定义常量 SqType，它必须与 SqColumn 的**指针数组**一起使用。

```c++
static const SqColumn  columnArray[2] = {
	{SQ_TYPE_UINT,   "id",         offsetof(YourStruct, id),         0},
	{SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
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

## 3. 定义由动态 SqType 使用的常量 SqColumn

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

## 4 创建由动态 SqType 使用的动态 SqColumn

添加一个动态列来输入

* 注意：**不推荐**使用这种方式。

```c++
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STRING);
	column->offset = offsetof(YourStruct, name);
	column->bit_field |= SQB_PRIMARY;    // 设置 SqColumn.bit_field
//	column->bit_field &= ~SQB_PRIMARY;   // 清除 SqColumn.bit_field

	/* 使用 C 函数添加列 */
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));

	/* 使用 C++ 方法添加列 */
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);
	type->addEntry((const SqEntry*)column, 1, sizeof(SqColumn));
```
