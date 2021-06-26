# SqColumn

SqColumn derives from [SqEntry](SqEntry.md) and defines column in SQL table.
When it works with SqType is very similar to SqEntry.

	SqEntry
	|
	`--- SqReentry
	     |
	     `--- SqColumn

Structure Definition:

```c
struct SqColumn
{
	// ------ SqEntry members ------
	const SqType *type;        // field type
	const char   *name;        // field name
	size_t        offset;      // offset of field in structure/class
	unsigned int  bit_field;   // declare below


	// ------ SqReentry members ------
	const char   *old_name;    // rename or drop


	// ------ SqColumn members ------
	int16_t      size;             // total digits or length of string
	int16_t      digits;           // decimal digits

	const char  *default_value;    // DEFAULT
	const char  *check;            // CHECK (condition)

	SqForeign   *foreign;          // foreign key
	char       **composite;        // Null-terminated (column-name) string array

	const char  *raw;              // raw SQL column property
};
```

Declaring bit_field in SqColumn

| name              | description                                   | 
| ----------------- | --------------------------------------------- |
| SQB_PRIMARY       | SQL property: PRIMARY KEY                     |
| SQB_FOREIGN       | SQL property: FOREIGN KEY                     |
| SQB_UNIQUE        | SQL property: UNIQUE                          |
| SQB_INCREMENT     | SQL property: AUTOINCREMENT                   |
| SQB_AUTOINCREMENT | SQL property: the same as SQB_INCREMENT       |
| SQB_NULLABLE      | SQL property: remove "NOT NULL"               |
| SQB_RENAMED       | column/table has been renamed.                |
| SQB_CHANGED       | alter/modify. column/table has been altered.  |

* All items that need attention are the same as the [SqEntry](SqEntry.md)


## 1. Create table by SqType
Because SqColumn is derived from SqEntry, you can see [SqType](SqType.md) to get more sample.

#### 1.1. define constant SqColumn pointer array that used by constant SqType
This can reduce running time when making schema if your SQL table is fixed and not changed in future.

* Note: This is SqColumn pointer array. If you define constant SqType for structure, it must use with SqColumn pointer array.

```c++
static const SqColumn *columnPointers[2] = {
	&(SqColumn) {SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	&(SqColumn) {SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

// If your columnPointers are not sorted, pass 0 to last argument.
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, columnPointers, 0);

// If your columnPointers are sorted by name, pass SQB_TYPE_SORTED to last argument.
// const SqType type = SQ_TYPE_INITIALIZER(YourStruct, columnPointers, SQB_TYPE_SORTED);


	// use C function to create table by type
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// use C++ function to create table by type
	table = schema->create("your_table_name", type);
```

#### 1.2. define constant SqColumn array that used by dynamic SqType
* Note: This is SqColumn array (NOT pointer array)

```c++
static const SqColumn columns[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	{SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

	// use C function:
	// add 2 columns from array
	sq_type_add_entry(type, (const SqEntry*)columns, 2, sizeof(SqColumn));
	// create table by type
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// use C++ function:
	// add 2 columns from array
	type->addEntry((const SqEntry*)columns, 2, sizeof(SqColumn));
	// create table by type
	table = schema->create("your_table_name", type);
```

#### 1.3. define dynamic SqColumn that used by dynamic SqType

use C function to add one dynamic column 

* Note: It is **not recommended** to use this way because it may deprecated or changed in future.

```c++
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STRING);
	column->offset = offsetof(YourStruct, name);
	column->bit_field |= SQB_PRIMARY;    // set bit in SqColumn.bit_field
//	column->bit_field &= ~SQB_PRIMARY;   // clear bit in SqColumn.bit_field

	// use C function to add column
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));

	// use C++ function to add column
	type->addEntry((const SqEntry*)column);
```

## 2. Create table by schema builder

It is recommended to use schema builder to create dynamic table.
You can see **(Schema Builder)** parts of "**Database schema**" in ../[README.md](../README.md) to get more sample.
