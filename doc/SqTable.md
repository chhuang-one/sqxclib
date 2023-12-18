[中文](SqTable.cn.md)

# SqTable

SqTable derives from [SqEntry](SqEntry.md). It defines SQL table.

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

## Create table

SqTable must be used with [SqSchema](SqSchema.md) and [SqColumn](SqColumn.md) to create a table. You can see below documents to get more information and sample:  
1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)
  
Use C language

```c
	typedef struct  User    User;

	SqTable *table;
	SqType  *tableType = NULL;

	// Using functions of SqSchema to create tables (Recommend)
	table = sq_schema_create(schema, "users", User);

	// Using sq_table_new() to create tables
	table = sq_table_new("users", tableType);
	sq_schema_add(schema, table);
```

Use C++ language

```c++
	typedef struct  User    User;

	Sq::Table *table;
	Sq::Type  *tableType = NULL;

	// Using methods of Sq::Schema to create tables (Recommend)
	table = schema->create<User>("users");

	// Using constructor of Sq::Table to create tables
	table = new Sq::Table("users", tableType);
	schema->add(table);
```

## Create column

Below C++ methods (C functions) are correspond to the different types of columns that you can add to table.  
Most methods (functions) set a specific [SqType](SqType.md) to the column.

| C++ methods | C functions            | C data type    | SQL data type     | specified SqType    |
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
| text        | sq_table_add_text      | char* &ensp; default | TEXT        | SQ_TYPE_STR &ensp; default |
| clob        | sq_table_add_clob      | char* &ensp; default | CLOB        | SQ_TYPE_STR &ensp; default |
| blob        | sq_table_add_blob      | SqBuffer       | BLOB or BINARY    | SQ_TYPE_BUFFER      |
| binary      | sq_table_add_binary    | SqBuffer       | BLOB or BINARY    | SQ_TYPE_BUFFER      |
| custom      | sq_table_add_custom    | *User define*  | VARCHAR           | *User define*       |
| mapping     | sq_table_add_mapping   | *User specify* | *User specify*    | *User specify*      |

* Because 'bool', 'int', 'double', and 'char' are C/C++ keywords, it must append '_' in tail of these methods.
* Some methods/functions (such as boolean, integer, string, and binary) have shorter aliases.
* timestamps() method is used to add 2 commonly used timestamp columns - created_at and updated_at.
* text() and clob() methods can specify SqType by user, or use the default type SQ_TYPE_STR.

Below method is for C++ data type only.

| C++ methods | C++ data type     | SQL data type     | specified SqType   |
| ----------- | ----------------- | ----------------- | ------------------ |
| stdstring   | std::string       | VARCHAR           | SQ_TYPE_STD_STRING |
| stdstr      | std::string       | VARCHAR           | SQ_TYPE_STD_STRING |
| stdvector   | std::vector<char> | BLOB or BINARY    | SQ_TYPE_STD_VECTOR |
| stdvec      | std::vector<char> | BLOB or BINARY    | SQ_TYPE_STD_VECTOR |

* stdstr and stdvec are shorter aliases of stdstring and stdvector.

**Use custom or JSON type**  
  
If you want to store JSON object or array in SQL column, you can use sq_table_add_custom() or C++ method custom() to create column.  
When adding a column that uses a custom type, you must specify [SqType](SqType.md).  
  
Example: define a C structure that contain struct and array.

```c
struct DemoTable {
	// ...

	// MyStructure is user-defined C struct
	// This will store JSON object in SQL column.
	MyStructure    myStruct;

	// SqIntArray is integer array that defined in SqArray.h
	// This will store JSON integer array in SQL column.
	SqIntArray     intArray;
};
```

In the following example,
SQ_TYPE_MY_STRUCT is the user-defined SqType of MyStructure.  
SQ_TYPE_INT_ARRAY is declared in SqType.h and parses a JSON integer array from value of SQL column.  
  
use C language

```c
	// JSON object will be stored in SQL VARCHAR column.
	column = sq_table_add_custom(table, "myStruct", offsetof(DemoTable, myStruct),
	                             SQ_TYPE_MY_STRUCT, 128);

	// JSON integer array will be stored in SQL VARCHAR column.
	column = sq_table_add_custom(table, "intArray", offsetof(DemoTable, intArray),
	                             SQ_TYPE_INT_ARRAY, 96);
```

use C++ language

```c++
	// JSON object will be stored in SQL VARCHAR column.
	column = table->custom("myStruct", offsetof(DemoTable, myStruct),
	                       SQ_TYPE_MY_STRUCT, 128);

	// JSON integer array will be stored in SQL VARCHAR column.
	column = table->custom("intArray", offsetof(DemoTable, intArray),
	                       SQ_TYPE_INT_ARRAY, 96);
```

You can also use pointer to member if you don't want to use offsetof().

```c++
	// use '&DemoTable::myStruct' to replace 'offsetof(DemoTable, myStruct)'
	column = table->custom("myStruct", &DemoTable::myStruct,
	                       SQ_TYPE_MY_STRUCT, 128);

	// use '&DemoTable::intArray' to replace 'offsetof(DemoTable, intArray)'
	column = table->custom("intArray", &DemoTable::intArray,
	                       SQ_TYPE_INT_ARRAY, 96);
```

**Use type mapping**  
  
To use type mapping, you must map [SqType](SqType.md) to the SQL data type when creating column.
Of course you can also map to user-defined [SqType](SqType.md).  
  
Below are Library-defined common SQL data type for type mapping.

| Library-defined SQL data types  | Library-provided SqType             |
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

The following SQL data types are aliases:

| Library-defined SQL data types  | alias of SQ_SQL_XXXX                    |
| ------------------------------- | --------------------------------------- |
| SQ_SQL_TYPE_BINARY              | alias of SQ_SQL_TYPE_BLOB               |
| SQ_SQL_TYPE_UNSIGNED_INT        | alias of SQ_SQL_TYPE_INT_UNSIGNED       |
| SQ_SQL_TYPE_UNSIGNED_BIGINT     | alias of SQ_SQL_TYPE_BIGINT_UNSIGNED    |
| SQ_SQL_TYPE_UNSIGNED_TINYINT    | alias of SQ_SQL_TYPE_TINYINT_UNSIGNED   |
| SQ_SQL_TYPE_UNSIGNED_SMALLINT   | alias of SQ_SQL_TYPE_SMALLINT_UNSIGNED  |
| SQ_SQL_TYPE_UNSIGNED_MEDIUMINT  | alias of SQ_SQL_TYPE_MEDIUMINT_UNSIGNED |

Example: map SqType SQ_TYPE_INT_ARRAY to SQL data type TEXT  
  
use C language

```c
	// JSON integer array will be stored in SQL TEXT column.
	column = sq_table_add_mapping(table, "intArray", offsetof(DemoTable, intArray),
	                              SQ_TYPE_INT_ARRAY,
	                              SQ_SQL_TYPE_TEXT);
```

use C++ language

```c++
	// JSON integer array will be stored in SQL TEXT column.
	column = table->mapping("intArray", offsetof(DemoTable, intArray),
	                        SQ_TYPE_INT_ARRAY,
	                        SQ_SQL_TYPE_TEXT);
```

## Add column

C function sq_table_add_column() and sq_table_add_column_ptrs(), C++ method addColumn() can add existed columns.  
  
sq_table_add_column(), addColumn()      can add SqColumn array.  
sq_table_add_column_ptrs(), addColumn() can add SqColumn pointer array.  
  
You can see [schema-builder-static.md](schema-builder-static.md) to get more information and sample.

## Drop column

sq_table_drop_column() must specify the column name to drop. This does not drop the column before doing migration because it just add a record of "Remove Column" to the 'table'.

```c++
	// C function
	sq_table_drop_column(table, "columnName");

	// C++ method
	table->dropColumn("columnName");
```

## Rename Column

Like sq_schema_drop(), sq_schema_rename() does not rename the table immediately.

```c++
	// C function
	sq_table_rename_column(table, "old_name", "new_name");

	// C++ method
	table->renameColumn("old_name", "new_name");
```

## Get Column

C function sq_table_find_column(), C++ method findColumn() can find column by name.

```c++
	// C function
	column = sq_table_find_column(table, "columnName");

	// C++ method
	column = table->findColumn("columnName");
```

C function sq_table_get_primary(), C++ method getPrimary() can get primary key.

```c++
	// C function
	column = sq_table_get_primary(table, NULL);

	// C++ method
	column = table->getPrimary();
```

## Index (composite)

**Create index**  
  
To create a composite index, you can pass multiple columns to index() method.
If you use C function sq_table_add_index() to create index, the last argument must be NULL.

```c++
	// C function
	column = sq_table_add_index(table, "indexName", "columnName01", "columnName02", NULL);

	// C++ method
	column = table->index("indexName", "columnName01", "columnName02");
```

**Drop index**  
  
To drop an index, you must specify the index's name.

```c++
	// C function
	column = sq_table_drop_index(table, "indexName");

	// C++ method
	column = table->dropIndex("indexName");
```

## Unique (composite)

**Create unique**  
  
To create a composite unique, you can pass multiple columns to unique() method.
If you use C function sq_table_add_unique() to create unique, the last argument must be NULL.

```c++
	// C function
	column = sq_table_add_unique(table, "uniqueName", "columnName01", "columnName02", NULL);

	// C++ method
	column = table->unique("uniqueName", "columnName01", "columnName02");
```

**Drop unique**  
  
To drop an unique, you must specify the unique's name.

```c++
	// C function
	column = sq_table_drop_unique(table, "uniqueName");

	// C++ method
	column = table->dropUnique("uniqueName");
```

## Primary key (composite)

**Create primary key**  
  
To create a composite primary key, you can pass multiple columns to primary() method.
If you use C function sq_table_add_primary() to create primary key, the last argument must be NULL.

```c++
	// C function
	column = sq_table_add_primary(table, "primaryName", "columnName01", "columnName02", NULL);

	// C++ method
	column = table->primary("primaryName", "columnName01", "columnName02");
```

**Drop primary key**  
  
To drop an primary key, you must specify the primary key's name.

```c++
	// C function
	column = sq_table_drop_primary(table, "primaryName");

	// C++ method
	column = table->dropPrimary("primaryName");
```

## Foreign Key

**Create foreign key**  
  
To create a foreign key, you can pass a column to foreign() method.

```c++
	// C function
	column = sq_table_add_foreign(table, "foreignName", "columnName");

	// C++ method
	column = table->foreign("foreignName", "columnName");
```

**Drop foreign key**  
  
To drop an foreign key, you must specify the foreign key's name.

```c++
	// C function
	column = sq_table_drop_foreign(table, "foreignName");

	// C++ method
	column = table->dropForeign("foreignName");
```
