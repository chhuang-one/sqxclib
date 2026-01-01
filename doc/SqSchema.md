[中文](SqSchema.cn.md)

# SqSchema

SqSchema defines database schema. It store table and changed record of table. [SqStorage](SqStorage.md) use it to do migration.

	SqEntry
	│
	└─── SqSchema

## 1 Create schema

When program create new schema, it will generate version number automatically. The current way to generate the version number is very primitive, it just add 1 to the counter.  
Because the version number is used to determine whether to do migration. User can manually specify the version number of the schema, it can avoid some problem.  
  
The name of schema can be NULL when creating new schema.  
  
use C language

```c
	SqSchema *schema;
	int       version = 2;

	// Create new schema and automatically assign it's version number.
	schema = sq_schema_new("SchemaName");

	// Create schema and specify it's version number.
	schema = sq_schema_new_ver(version, "SchemaName");
```

use C++ language

```c++
	Sq::Schema *schema;
	int         version = 2;

	// Create new schema and automatically assign it's version number.
	schema = new Sq::Schema("SchemaName");

	// Create schema and specify it's version number.
	schema = new Sq::Schema(version, "SchemaName");
```

## 2 Create table

SqSchema must be used with [SqTable](SqTable.md) and [SqColumn](SqColumn.md) to create a table. You can see below documents to get more information and sample:

1. [database-migrations.md](database-migrations.md)
2. "**Database schema**" section in ../[README.md](../README.md#database-schema)

#### 2.1 Create table for C struct

Define a C struct 'User' to map database table "users".  
  
use C language

```c
	// If you use C language, please use 'typedef' to give a struct type a new name.
	typedef struct  User    User;

	struct  User {
		// ...
	};

	table = sq_schema_create(schema, "users", User);
	// add columns to table...
```

use C++ language

```c++
	struct  User {
		// ...
	};

	table = schema->create<User>("users");
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

#### 2.3 add existing table

```c++
	// C function
	sq_schema_add(schema, table);

	// C++ method
	schema->add(table);
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
	// C function
	sq_schema_drop(schema, "users");

	// C++ method
	schema->drop("users");
```

## 5 Rename table

Like sq_schema_drop(), sq_schema_rename() does not rename the table immediately.

```c++
	// C function
	sq_schema_rename(schema, "old_name", "new_name");

	// C++ method
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
