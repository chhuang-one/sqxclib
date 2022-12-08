[English](SqRow.md)

# SqRow

SqRow 由 (SqTypeRow)[SqTypeRow.cn.md] 创建。如果 (SqTypeRow)[SqTypeRow.md] 不知道列的类型，则 SqRow 中的所有数据类型都是 C 字符串。

## 数组

SqRow 包含 2 个数组。一个是列数组，另一个是数据数组。
在大多数情况下，这两个数组的长度相同。  
  
SqRow 结构定义：

```c
struct SqRow {
	// 数据数组
	SqValue      *data;
	int           length;
	int           allocated;

	// 列数组
	SqRowColumn  *cols;
	int           cols_length;
	int           cols_allocated;
};
```

#### 列数组

SqRow 使用 SqRowColumn 数组来存储列名、列类型和列条目。
当 (SqTypeRow)[SqTypeRow.md] 不知道列的类型时，SqRowColumn.type 总是等于 SQ_TYPE_STRING。  
  
SqRowColumn 结构定义:

```c
struct SqRowColumn {
	const char    *name;
	const SqType  *type;
	const SqEntry *entry;
};
```

#### 数据数组

SqRow 使用 SqValue 来存储值。SqValue 是联合类型，值的类型由相同索引处的 SqRowColumn.type 决定。  
  
SqValue 定义:

```c
union SqValue {
	bool          boolean;     // SQ_TYPE_BOOL
	int           integer;     // SQ_TYPE_INT
	int           int_;        // SQ_TYPE_INT
	unsigned int  uinteger;    // SQ_TYPE_UINT
	unsigned int  uint;        // SQ_TYPE_UINT
	int64_t       int64;       // SQ_TYPE_INT64
	uint64_t      uint64;      // SQ_TYPE_UINT64
	time_t        rawtime;     // SQ_TYPE_TIME
	double        fraction;    // SQ_TYPE_DOUBLE
	double        double_;     // SQ_TYPE_DOUBLE
	const char   *string;      // SQ_TYPE_STRING
	const char   *stream;      // Text stream must be null-terminated string
	void         *pointer;     // for user defined type
};
```

## 创建和释放 SqRow

创建 SqRow 时，用户必须指定数组的预分配长度。  
  
使用 C 语言

```c
	SqRow  *row;
	int     cols_length = 0;    // 列数组的预分配长度
	int     data_length = 0;    // 数据数组的预分配长度

	// 创建新的 SqRow
	row = sq_row_new(cols_length, data_length);
	// 释放 SqRow
	sq_row_free(row);
```

使用 C++ 语言

```c++
	Sq::Row *row;
	int      colsLength = 0;    // 列数组的预分配长度
	int      dataLength = 0;    // 数据数组的预分配长度

	// 创建新的 Sq::Row
	row = new Sq::Row(colsLength, dataLength);
	// 释放 Sq::Row
	delete row;
```

## 添加列

SqRow 可以分别为 SqRowColumn 和 SqValue 数组分配内存。  
C 函数 sq_row_alloc_column() 和 sq_row_alloc()，C++ 方法 allocColumn() 和 alloc() 用于分配列及其值。  
  
使用 C 语言

```c
	int  n_columns = 1;
	int  n_values  = 1;

	SqRowColumn *col = sq_row_alloc_column(row, n_columns);
	SqValue     *val = sq_row_alloc(row, n_values);
	// 在这里设置 'col' 和 'val'
```

使用 C++ 语言

```c++
	int  nColumns = 1;
	int  nValues  = 1;

	Sq::RowColumn *col = row->allocColumn(nColumns);
	Sq::Value     *val = row->alloc(nValues);
	// 在这里设置 'col' 和 'val'
```

## 其他

C 函数 sq_row_free_cols_name()、C++ 方法 freeColsName() 可以释放 SqRow 中的所有 SqRowColumn.name。这可以节省内存，因为当您从同一个表中获取多行时，所有 SqRow 都具有相同的列名。

```c++
	// C 函数
	sq_row_free_cols_name(row);

	// C++ 方法
	row->freeColsName();
```
