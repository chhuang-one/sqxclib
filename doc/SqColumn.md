[中文](SqColumn.cn.md)

# SqColumn

SqColumn derives from [SqEntry](SqEntry.md). It defines columns in SQL table and works with SqType.

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

Structure Definition:

```c
struct SqColumn
{
	// ------ SqEntry members ------
	const SqType *type;             // field type
	const char   *name;             // column name
	size_t        offset;           // offset of field in structure
	unsigned int  bit_field;        // explain below


	// ------ SqReentry members ------
	const char   *old_name;         // use this when renaming or dropping


	// ------ SqColumn members ------

	// size  : total number of digits is specified in size, or length of string
	int16_t       size;
	// digits: number of digits after the decimal point.
	int16_t       digits;

	const char   *default_value;    // DEFAULT
	const char   *check;            // CHECK (condition)

	SqForeign    *foreign;          // foreign key
	char        **composite;        // Null-terminated (column-name) string array

	const char   *raw;              // raw SQL column property
};
```

Declaring bit_field that used by SqColumn:

| name                   | description                                     | 
| ---------------------- | ----------------------------------------------- |
| SQB_PRIMARY            | SQL property: PRIMARY KEY                       |
| SQB_FOREIGN            | SQL property: FOREIGN KEY                       |
| SQB_UNIQUE             | SQL property: UNIQUE                            |
| SQB_INCREMENT          | SQL property: AUTOINCREMENT                     |
| SQB_AUTOINCREMENT      | SQL property: the same as SQB_INCREMENT         |
| SQB_NULLABLE           | SQL property: remove "NOT NULL"                 |
| SQB_CURRENT            | SQL property: DEFAULT CURRENT_TIMESTAMP         |
| SQB_CURRENT_ON_UPDATE  | SQL property: use CURRENT_TIMESTAMP when a record is updated. |
| SQB_RENAMED            | column or table has been renamed.               |
| SQB_CHANGED            | column or table has been altered.               |

* SqColumn also inherits the definition of bit_field in [SqEntry](SqEntry.md).

## 1 Create table and column by methods and functions (dynamic)

It use C++ methods or C functions to create dynamic table and column.  
To get more information and sample, you can see below documents:  
1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)

## 2 Define constant SqColumn that used by constant SqType (static)

This can reduce running time when making schema if your SQL table is fixed and not changed in future.  
* Note: If you define constant SqType for structure, it must use with **pointer array** of SqColumn.

```c++
static const SqColumn  columnArray[2] = {
	{SQ_TYPE_UINT,   "id",         offsetof(YourStruct, id),         0},
	{SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

static const SqColumn *columnPointerArray[2] = {
	& columnArray[0],
	& columnArray[1],
};

/* If 'columnPointerArray' is NOT sorted by name, pass 0 to last argument.
   Otherwise use SQB_TYPE_SORTED to replace 0 in last argument.
 */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, columnPointerArray, 0);
```

## 3 Define constant SqColumn that used by dynamic SqType

Use C language

```c
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);

	// add 2 columns from pointer array
	sq_type_add_entry_ptrs(type, (const SqEntry**)columnPointerArray, 2);

	// add 2 columns from array (NOT pointer array)
//	sq_type_add_entry(type, (const SqEntry*)columnArray, 2, sizeof(SqColumn));
```

Use C++ language

```c++
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);

	// add 2 columns from pointer array
	type->addEntry((const SqEntry**)columnPointerArray, 2);

	// add 2 columns from array (NOT pointer array)
//	type->addEntry((const SqEntry*)columnArray, 2, sizeof(SqColumn));
```

## 4 Create dynamic SqColumn that used by dynamic SqType 

add one dynamic column to type

* Note: It is **not recommended** to use this way.

```c++
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STRING);
	column->offset = offsetof(YourStruct, name);
	sq_column_primary(column);           // set PRIMARY KEY

	/* use C functions to add column */
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));

	/* use C++ methods to add column */
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);
	type->addEntry((const SqEntry*)column, 1, sizeof(SqColumn));
```
