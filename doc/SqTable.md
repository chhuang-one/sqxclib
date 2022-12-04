[中文](SqTable.cn.md)

# SqTable

SqTable defines SQL table

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

## Create table

SqTable must be used with [SqSchema](SqSchema.md) and [SqColumn](SqColumn.md) to create a table. You can see below documents to get more information and sample:  
1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)

## Create column

Below C++ methods (and C functions) are correspond to the different types of columns that you can add to tables.

| C++ methods | C functions             | C data type   |
| ----------- | ----------------------- | ------------- |
| boolean     | sq_table_add_bool       | bool          |
| bool_       | sq_table_add_bool       | bool          |
| integer     | sq_table_add_int        | int           |
| int_        | sq_table_add_int        | int           |
| uint        | sq_table_add_uint       | unsigned int  |
| int64       | sq_table_add_int64      | int64_t       |
| uint64      | sq_table_add_uint64     | uint64_t      |
| timestamp   | sq_table_add_timestamp  | time_t        |
| double_     | sq_table_add_double     | double        |
| string      | sq_table_add_string     | char*         |
| char_       | sq_table_add_char       | char*         |
| custom      | sq_table_add_custom     | *User define* |

* Because 'bool', 'int', 'double', and 'char' are C/C++ keywords, it must append '_' in tail of these methods.

Below method is for C++ data type only.

| C++ methods | C++ data type |
| ----------- | ------------- |
| stdstring   | std::string   |

**Add custom or JSON type**  
  
Add column that contain JSON object or array.  
  
e.g. Define a C structure that contain struct and array.

```c
struct User {
	// ...

	// MyStructure is C struct
	MyStructure    myStruct;

	// SqIntptrArray is integer array that defined in SqPtrArray.h
	SqIntptrArray  posts;
};
```

use C language

```c
	// JSON object
	column = sq_table_add_custom(table, "myStruct", offsetof(User, myStruct),
	                             SQ_TYPE_MY_STRUCT, 128);

	// JSON integer array
	column = sq_table_add_custom(table, "posts", offsetof(User, posts),
	                             SQ_TYPE_INTPTR_ARRAY, 256);
```

use C++ language

```c++
	// JSON object
	column = table->custom("myStruct", offsetof(User, myStruct),
	                       SQ_TYPE_MY_STRUCT, 128);

	// JSON integer array
	column = table->custom("posts", offsetof(User, posts),
	                       SQ_TYPE_INTPTR_ARRAY, 256);
```

## Add column

C function sq_table_add_column() and sq_table_add_column_ptrs(), C++ method addColumn() can add existed columns. They are typically used to add static SqColumn array.  
  
sq_table_add_column(), addColumn()      can add SqColumn array.  
sq_table_add_column_ptrs(), addColumn() can add SqColumn pointer array.  
  
You can see [schema-builder-static.md](schema-builder-static.md) to get more information and sample.

## Drop column

sq_table_drop_column() must specify the column name to drop. This does not drop the column immediately before migration because it just add a record of "Remove Column" to the 'table'.

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
