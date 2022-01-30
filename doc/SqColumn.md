# SqColumn

SqColumn derives from [SqEntry](SqEntry.md) and defines column in SQL table.
When it works with SqType is very similar to SqEntry.

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

| name                   | description                                   | 
| ---------------------- | --------------------------------------------- |
| SQB_PRIMARY            | SQL property: PRIMARY KEY                     |
| SQB_FOREIGN            | SQL property: FOREIGN KEY                     |
| SQB_UNIQUE             | SQL property: UNIQUE                          |
| SQB_INCREMENT          | SQL property: AUTOINCREMENT                   |
| SQB_AUTOINCREMENT      | SQL property: the same as SQB_INCREMENT       |
| SQB_NULLABLE           | SQL property: remove "NOT NULL"               |
| SQB_CURRENT            | SQL property: DEFAULT CURRENT_TIMESTAMP       |
| SQB_CURRENT_ON_UPDATE  | SQL property: use CURRENT_TIMESTAMP when a record is updated. |
| SQB_RENAMED            | column/table has been renamed.                |
| SQB_CHANGED            | alter/modify. column/table has been altered.  |

* All items that need attention are the same as the [SqEntry](SqEntry.md)

## 1. Create table by schema builder (dynamic)

It is recommended to use schema builder to create dynamic table.  
To get more information and sample, you can see below documents:  
1. [database-migrations.md](database-migrations.md)
2. **(Schema Builder)** parts of "**Database schema**" in ../[README.md](../README.md#database-schema)

## 2. Create table by SqType (static or dynamic)
Because SqColumn is derived from SqEntry, you can see [SqType](SqType.md) to get more sample.

```c++
	// C function
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// C++ method
	table = schema->create("your_table_name", type);
```

#### 2.1. Define constant SqType that using constant SqColumn (static)
This can reduce running time when making schema if your SQL table is fixed and not changed in future.  
* Note: If you define constant SqType for structure, it must use with SqColumn **pointer array**.

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

#### 2.2. Create dynamic SqType that using constant SqColumn

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

#### 2.3. Create dynamic SqType that using dynamic SqColumn

add one dynamic column to type

* Note: It is **not recommended** to use this way.

```c++
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STRING);
	column->offset = offsetof(YourStruct, name);
	column->bit_field |= SQB_PRIMARY;    // set bit in SqColumn.bit_field
//	column->bit_field &= ~SQB_PRIMARY;   // clear bit in SqColumn.bit_field

	/* use C functions to add column */
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));

	/* use C++ methods to add column */
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);
	type->addEntry((const SqEntry*)column, 1, sizeof(SqColumn));
```
