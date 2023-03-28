[中文](SqColumn.cn.md)

# SqColumn

SqColumn derives from [SqEntry](SqEntry.md). It defines columns in SQL table and works with SqType.

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

## Create column (dynamic)

SqColumn must be used with [SqTable](SqTable.md) and [SqSchema](SqSchema.md) to create a table. It use C++ methods or C functions to create dynamic table and column.  
To get more information and sample, you can see below documents:  
1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)
  
Use C language

```c
	SqColumn *column;

	// Using functions of SqTable to create columns (Recommend)
	column = sq_table_add_string(table, "column", offsetof(MyStruct, column), 191);

	// Using sq_column_new() to create columns
	column = sq_column_new("id", SQ_TYPE_INT);
	column->offset = offsetof(MyStruct, id);
	sq_table_add_column(table, column, 1);
```

Use C++ language

```c++
	Sq::Column *column;

	// Using methods of Sq::Table to create columns (Recommend)
	column = table->string("column", &MyStruct::column, 191);

	// Using constructor of Sq::Column to create columns
	column = new Sq::Column("id", SQ_TYPE_INT);
	column->offset = offsetof(MyStruct, id);
	table->addColumn(column);
```

## Column Modifiers

There are several "modifiers" you may use when adding a column to table or a entry to structure.  
Below C++ methods (and C functions) are correspond to Column Modifiers:

| C++ methods          | C functions                        | C bit field name      |
| -------------------- | ---------------------------------- | --------------------- |
| primary()            | sq_column_primary()                | SQB_PRIMARY           |
| unique()             | sq_column_unique()                 | SQB_UNIQUE            |
| autoIncrement()      | sq_column_auto_increment()         | SQB_AUTOINCREMENT     |
| nullable()           | sq_column_nullable()               | SQB_NULLABLE          |
| useCurrent()         | sq_column_use_current()            | SQB_CURRENT           |
| useCurrentOnUpdate() | sq_column_use_current_on_update()  | SQB_CURRENT_ON_UPDATE |
| default_(string)     | sq_column_default()                |                       |

* Because 'default' is C/C++ keywords, it must append '_' in tail of this method.

Special methods for structured data type.

| C++ methods      | C bit field name  | Description                                                  |
| ---------------- | ----------------- | ------------------------------------------------------------ |
| pointer()        | SQB_POINTER       | This data member is a pointer.                               |
| hidden()         | SQB_HIDDEN        | Don't output this data member to JSON.                       |
| hiddenNull()     | SQB_HIDDEN_NULL   | Don't output this data member to JSON if it's value is NULL. |

For example, to make the column "nullable":

```c++
	/* C++ sample code */
	table->string("name", &User::name)->nullable();

	/* C sample code */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	sq_column_nullable(column);
```

## Change column

C function sq_column_change(), C++ method change() allow you to modify the type and attributes of existing columns. They do not change the column before doing migration.  
  
use C language

```c
	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// alter column "email" in table
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	sq_column_change(column);
```

use C++ language

```c++
	// alter table "users"
	table = schema->alter("users");

	// alter column "email" in table
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

## use SqColumn with SqType

To define constant SqColumn, user must know SqColumn Structure Definition:

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

	SqForeign    *foreign;          // foreign key
	char        **composite;        // Null-terminated (column-name) string array

	const char   *reserve;          // reserve
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

#### Define constant SqColumn that used by constant SqType (static)

This can reduce running time when making schema if your SQL table is fixed and not changed in future.  
* Note: If you define constant SqType for structure, it must use with **pointer array** of SqColumn.

```c++
static const SqColumn  columnArray[2] = {
	{SQ_TYPE_UINT,   "id",         offsetof(YourStruct, id),         0},
	{SQ_TYPE_STR,    "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

static const SqColumn *columnPointerArray[2] = {
	& columnArray[0],
	& columnArray[1],
};

/* If 'columnPointerArray' is NOT sorted by name, pass 0 to the last argument.
   Otherwise use SQB_TYPE_SORTED to replace 0 in the last argument.
 */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, columnPointerArray, 0);
```

#### Define constant SqColumn that used by dynamic SqType

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

#### Create dynamic SqColumn that used by dynamic SqType 

add one dynamic column to type

* Note: It is **not recommended** to use this way.

Use C language

```c
	SqColumn *column = sq_column_new("your_column_name", SQ_TYPE_STR);
	column->offset = offsetof(YourStruct, FieldName);
	sq_column_primary(column);           // set PRIMARY KEY

	/* add column to SqType */
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_column_free);
	sq_type_add_entry(type, (const SqEntry*)column, 1, sizeof(SqColumn));
```

Use C++ language

```c++
	Sq::Column *column = new Sq::Column("your_column_name", SQ_TYPE_STR);
	column->offset = offsetof(YourStruct, FieldName);
	column->primary();                   // set PRIMARY KEY

	/* add column to Sq::Type */
	Sq::Type *type = new Sq::Type(0, (SqDestroyFunc)sq_column_free);
	type->addEntry((const SqEntry*)column, 1, sizeof(SqColumn));
```
