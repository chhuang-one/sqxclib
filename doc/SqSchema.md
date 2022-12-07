[中文](SqSchema.cn.md)

# SqSchema

SqSchema defines database schema. It store table and changed record of table.

	SqEntry
	│
	└--- SqSchema

## 1 Create schema

When program create new schema, it will generate version number automatically. The current way to generate the version number is very primitive, it just add 1 to the counter.  
Because the version number is used to determine whether to do migration. User can manually specify the version number of the schema, it can avoid some problem.  
  
The name of schema can be NULL when user create new schema.  
  
use C language

```c
	SqSchema *schema;

	// Create new schema and assign it's name and version number.
	schema = sq_schema_new("SchemaName");
	schema->version = 1;

	// Create schema and assign it's version number only.
//	schema = sq_schema_new(NULL);
//	schema->version = 1;
```

use C++ language

```c++
	Sq::Schema *schema;

	// Create new schema and assign it's name and version number.
	schema = new Sq::Schema("SchemaName");
	schema->version = 1;

	// Create schema and assign it's version number only.
//	schema = new Sq::Schema(1);
```

## 2 Create table

SqSchema must be used with [SqTable](SqTable.md) and [SqColumn](SqColumn.md) to create a table. You can see below documents to get more information and sample:  
1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)
  
You will get different type name from C and C++ source code when you use gcc to compile because gcc's typeid(Type).name() will return strange name.  
**Please create or define type of SqTable in C++ language if your application written in C++ language.**  
  
If SqTable::type defined in C language, you may NOT use below C++ template functions to access SQL table.

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

#### 2.1 Create table for C struct

Define a C struct 'UserStruct' to map database table "users".  
  
use C language

```c
	// If you use C language, please use 'typedef' to give a struct type a new name.
	typedef struct  UserStruct    UserStruct;

	struct  UserStruct {
		// ...
	};

	table = sq_schema_create(schema, "users", UserStruct);
	// add columns to table...
```

use C++ language

```c++
	struct  UserStruct {
		// ...
	};

	table = schema->create<UserStruct>("users");
	// add columns to table...
```

#### 2.2 Create table by existed SqType

You can see documents [SqColumn.md](SqColumn.md) to get more information about creating SqType with SqColumn.  
Note: If 'type' is dynamic SqType, it will be freed when the program frees 'table'.  
  
use C language

```c
	// create table by existed type
	table = sq_schema_create_by_type(schema, "your_table_name", type);
```

use C++ language

```c++
	// create table by existed type
	table = schema->create("your_table_name", type);
```

## 3 Alter table

The usage of alter() is similar to the create().  
  
use C language

```c
	// alter table
	table = sq_schema_alter(schema, "users", NULL);

	// change column "nickname"
	column = sq_table_add_string(table, "nickname", offsetof(User, nickname), 40);
	sq_column_change();
	// add or change columns...
```

use C++ language

```c++
	// alter table
	table = schema->alter("users");

	// change column "nickname"
	table->string("nickname", offsetof(User, nickname), 40)->change();
	// add or change columns...
```

## 4 Drop table

sq_schema_drop() must specify the table name to drop. This does not drop the table before doing migration because it just add a record of "Remove Table" to the 'schema'.

```c++
	// C functions
	sq_schema_drop(schema, "users");

	// C++ methods
	schema->drop("users");
```

## 5 Rename table

Like sq_schema_drop(), sq_schema_rename() does not rename the table immediately.

```c++
	// C functions
	sq_schema_rename(schema, "old_name", "new_name");

	// C++ methods
	schema->rename("old_name", "new_name");
```

## 6 Find table

After creating or migrating table, user can find table by it's name in schema.

```c++
	// C function
	table = sq_schema_find(schema, "users");

	// C++ method
	table = schema->find("users");
```
