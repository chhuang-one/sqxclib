[中文](SqRow.cn.md)

# SqRow

SqRow is created by [SqTypeRow](SqTypeRow.md). If [SqTypeRow](SqTypeRow.md) don't know type of columns, all data type in SqRow is C string.

## Arrays in SqRow

SqRow contain 2 arrays. One is column array, another is data array.
In most cases, these two arrays will be the same length.  
  
SqRow Structure Definition:

```c
struct SqRow {
	// data array
	SqValue      *data;
	int           length;
	int           allocated;

	// column array
	SqRowColumn  *cols;
	int           cols_length;
	int           cols_allocated;
};
```

#### Column array

SqRow use SqRowColumn array to store column name, column type, and column entry.
SqRowColumn.type is always equal to SQ_TYPE_STRING when [SqTypeRow](SqTypeRow.md) does not know the type of the column.  
  
SqRowColumn Structure Definition:

```c
struct SqRowColumn {
	const char    *name;
	const SqType  *type;
	const SqEntry *entry;
};
```

#### Data array

SqRow use SqValue to store values. SqValue is a union type, the type of value is determined by the SqRowColumn.type at the same index.  
  
SqValue Definition:

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

## Result from SQL

For example, there is a row in customers table.

|  id | name | state |
| --- | ---- | ----- |
|  1  | Bob  |  NY   |

If [SqTypeRow](SqTypeRow.md) does not know the type of the columns, arrays in SqRow should look like this:

```c
	SqRow.cols[0].name   = "id";
	SqRow.cols[0].type   = SQ_TYPE_STRING;
	SqRow.cols[0].entry  = NULL;
	SqRow.data[0].string = "1";

	SqRow.cols[1].name   = "name";
	SqRow.cols[1].type   = SQ_TYPE_STRING;
	SqRow.cols[1].entry  = NULL;
	SqRow.data[1].string = "Bob";

	SqRow.cols[2].name   = "state";
	SqRow.cols[2].type   = SQ_TYPE_STRING;
	SqRow.cols[2].entry  = NULL;
	SqRow.data[2].string = "NY";

	// This row has 3 columns
	SqRow.length = 3;
	SqRow.cols_length = 3;
```

Otherwise the first column is this:

```c
	SqRow.cols[0].name    = "id";
	SqRow.cols[0].type    = SQ_TYPE_INT;
	SqRow.cols[0].entry   = (const SqColumn*) pointerToSqColumn;
	SqRow.data[0].integer = 1;
```

## Create and free SqRow

User must specify the pre-allocated length of the array when creating a SqRow.  
  
use C language

```c
	SqRow  *row;
	int     cols_length = 0;    // pre-allocated length of column array
	int     data_length = 0;    // pre-allocated length of data array

	// create new SqRow
	row = sq_row_new(cols_length, data_length);
	// free SqRow
	sq_row_free(row);
```

use C++ language

```c++
	Sq::Row *row;
	int      colsLength = 0;    // pre-allocated length of column array
	int      dataLength = 0;    // pre-allocated length of data array

	// create new Sq::Row
	row = new Sq::Row(colsLength, dataLength);
	// free Sq::Row
	delete row;
```

## Add columns

SqRow can allocate memory for array of SqRowColumn and SqValue separately.  
The C functions sq_row_alloc_column() and sq_row_alloc(), C++ methods allocColumn() and alloc() are used to allocate the column and its value.  
  
use C language

```c
	int  n_columns = 1;
	int  n_values  = 1;

	SqRowColumn *col = sq_row_alloc_column(row, n_columns);
	SqValue     *val = sq_row_alloc(row, n_values);

	// set 'col' and 'val' here
	col->name  = strdup("id");    // It can be NULL if you don't need column name.
	col->type  = SQ_TYPE_INT;     // SqRowColumn.type = SQ_TYPE_INT
	col->entry = NULL;            // It can be NULL.
	val->integer = 1;             // SqRowColumn.type = SQ_TYPE_INT
```

use C++ language

```c++
	int  nColumns = 1;
	int  nValues  = 1;

	Sq::RowColumn *col = row->allocColumn(nColumns);
	Sq::Value     *val = row->alloc(nValues);

	// set 'col' and 'val' here
	col->name  = strdup("id");    // It can be NULL if you don't need column name.
	col->type  = SQ_TYPE_INT;     // SqRowColumn.type = SQ_TYPE_INT
	col->entry = NULL;            // It can be NULL.
	val->integer = 1;             // SqRowColumn.type = SQ_TYPE_INT
```

## Other

The C functions sq_row_free_cols_name(), C++ methods freeColsName() can free all SqRowColumn.name in SqRow. This can save memory because all SqRow has the same column name when you get multiple row from the same table.

```c++
	// C function
	sq_row_free_cols_name(row);

	// C++ method
	row->freeColsName();
```
