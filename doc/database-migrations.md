[中文](database-migrations.cn.md)

# Database: Migrations

This document describe how to define database table, column, and migration dynamically. Migration function can handle both dynamic and static column/table definitions.
You can also use SqApp to run and reverse migrations with command-line tool, see [SqApp.md](SqApp.md)  

Note: Because many users have used Laravel, there are many sqxclib C++ method names are similar to it.
Actually the design of sqxclib is different from Laravel, so the usage cannot be the same.  

## Running Migrations

migrate schema and synchronize to database.  
  
use C++ language

```c++
	// migrate 'schema_v1' and 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	storage->migrate(NULL);

	// free unused 'schema_v1' and 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

use C language

```c
	// migrate 'schema_v1' and 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	sq_storage_migrate(storage, NULL);

	// free unused 'schema_v1' and 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```

## Tables

First, we define a C structured data type that maps to your database table "users".

```c
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  User    User;

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;

	time_t  created_at;

	int     test_add;
};
```

#### Creating Tables (dynamic)

Create a new table and it's columns in database.  
  
use C++ language  
  
The create() method requires specifying the structure data type and table name.

```c++
	// create table "users"
	table = schema->create<User>("users");

	// add columns to table
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->useCurrent();
```

use C language  
  
The sq_schema_create() function accepts two arguments: first argument is the name of table, second is structure data type.  

```c
	// create table "users"
	table = sq_schema_create(schema, "users", User);

	// add columns to table
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_use_current(column);
```

If your table is defined in C language but application is written in C++ language, you may NOT use below C++ template functions to access database table.

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

Because gcc's typeid(Type).name() will return strange name, you will get different type name from C and C++ source code. In this case, you must create or define type of SqTable in C++ language, or use setName() method to set name of SqTable::type created in C language.

```c++
	// 'cUserTable' is created in C language and is not a constant.
	SqTable *table = cUserTable;

	// change C type name to C++ type name
	table->type->setName(typeid(User).name());
		// or
	table->type->setName(SQ_GET_TYPE_NAME(User));
```

#### Checking For Table Existence

You may check for the existence of a table using the find function:
* This function doesn't look for table in the database, it looks up table in instance of SqSchema.

```c++
	/* C++ sample code */
	if (schema->find("users") == NULL) {
		// The "users" table doesn't exists in 'schema'.
	}

	/* C sample code */
	if (sq_schema_find(schema, "users") == NULL) {
		// The "users" table doesn't exists in 'schema'.
	}
```

#### Updating Tables (dynamic)

use the alter function to update existing tables.  
  
use C++ language

```c++
	// alter table "users"
	table = schema->alter("users");

	// add column to table
	table->integer("test_add", &User::test_add);

	// alter column "email" in table
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

use C language

```c
	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// add columns to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));

	// alter column "email" in table
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	sq_column_change(column);
```

#### Renaming / Dropping Tables (dynamic)

use the rename function to rename an existing database table.

```c++
	/* C++ sample code */
	schema->rename("old table name", "new table name");

	/* C sample code */
	sq_schema_rename(schema, "old table name", "new table name");
```

You can use the drop function to drop an existing table.

```c++
	/* C++ sample code */
	schema->drop("users");

	/* C sample code */
	sq_schema_drop(schema, "users");
```

## Columns

#### Creating Columns (dynamic)

To add columns to the table, you can use functions of SqTable.
You will get instance of SqTable after calling alter or create function in schema.  
  
All available column types are listed in [SqTable](SqTable.md).  
  
use C++ language

```c++
	// alter table "users"
	table = schema->alter("users");

	// add column to table
	column = table->integer("test_add", &User::test_add);
```

use C language

```c
	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// add columns to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
```

#### Column Modifiers (dynamic)

There are several "modifiers" you may use when adding a column to table or a entry to structure.  
  
All available column modifiers are listed in [SqColumn](SqColumn.md).  
  
For example, to make the column "nullable":

```c++
	/* C++ sample code */
	table->string("name", &User::name)->nullable();

	/* C sample code */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	sq_column_nullable(column);
```

#### Updating Column Attributes

C function sq_column_change(), C++ method change() allow you to modify the type and attributes of existing columns.  
  
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

#### Renaming and Dropping Columns

use C++ method renameColumn() to rename a column.  
use C++ method dropColumn() to drop a column.

```c++
	// alter table "users"
	table = schema->alter("users");

	// rename column
	table->renameColumn("from", "to");

	// drop column
	table->dropColumn("columnName");
```

use C function sq_table_rename_column() to rename a column.  
use C function sq_table_drop_column() to drop a column.

```c
	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// rename column
	sq_table_rename_column(table, "from", "to");

	// drop column
	sq_table_drop_column(table, "columnName");
```

## Primary Key Constraints

To define a primary key column, use the primary() method onto the column definition:

```c++
	/* C sample code */
	column = sq_table_add_int(table, "id", offsetof(User, id));
	sq_column_primary(column);

	/* C++ sample code */
	table->integer("id", &User::id)
	     ->primary();
```

#### Creating Primary Keys

C function sq_table_add_primary() and C++ method primary() can create primary key constraint.
Arguments are the name of primary key constraint, others are NULL-terminated column name argument list.  
Because C++ methods use parameter pack, the last argument can pass NULL or not.

```c++
	/* C sample code */
	column = sq_table_add_primary(table, "primary_email_account_id", "email", "account_id", NULL);

	/* C++ sample code */
	table->primary("primary_email_account_id", "email", "account_id");
```

#### Dropping Primary Keys

User must specify name of primary key constraint to drop it.

```c++
	/* C sample code */
	sq_table_drop_primary(table, "primary_email_account_id");

	/* C++ sample code */
	table->dropPrimary("primary_email_account_id");
```

## Foreign Key Constraints

C function sq_column_reference() and C++ method reference() are used to set foreign key referenced table, columns.
Arguments are the name of foreign table, others are NULL-terminated column name argument list.  
  
To define a foreign key column, use reference() onto the column definition:

```c++
	/* C sample code */
	column = sq_table_add_int(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id", NULL);

	/* C++ sample code */
	table->integer("city_id", &User::city_id)
	     ->reference("cities", "id");
```

#### Creating Foreign Keys

C function sq_table_add_foreign() and C++ method foreign() can create foreign key constraint.
Arguments are the name of foreign key constraint, others are NULL-terminated column name argument list.  
  
Because number of columns in foreign key must match the number of columns in the referenced table, number of columns in foreign() and reference() arguments must match.  
  
use C language

```c
	column = sq_table_add_foreign(table, "foreignName", "column1", "column2", NULL);
	sq_column_reference(column, "foreignTableName",     "column1", "column2", NULL);
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");
```

use C++ language  
  
Because C++ methods use parameter pack, the last argument can pass NULL or not.

```c++
	table->foreign("foreignName", "column1", "column2")
	     ->reference("tableName", "column1", "column2")
	     ->onDelete("NO ACTION")
	     ->onUpdate("NO ACTION");
```

#### Dropping Foreign Keys

use the name of the foreign key constraint to delete it.

```c++
	/* C sample code */
	sq_table_drop_foreign(table, "users_city_id_foreign");

	/* C++ sample code */
	table->dropForeign("users_city_id_foreign");
```

## Indexes

To define a unique column, use the unique() method onto the column definition:

```c++
	/* C sample code */
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	sq_column_unique(column);

	/* C++ sample code */
	table->string("email", &User::email)
	     ->unique();
```

#### Creating Indexes

To use C functions to create composite index and unique constraint,
The 2nd argument is the name of index and unique constraint, others are NULL-terminated column name argument list.

```c
	column = sq_table_add_index(table, "index_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_unique(table, "unique_email_account_id", "email", "account_id", NULL);
```

To use C++ methods to create composite index and unique constraint,
The 1st Argument is the name of index and unique constraint, others are column name argument list.  
Because C++ methods use parameter pack, the last argument can pass NULL or not.

```c++
	table->index("index_email_account_id", "email", "account_id");

	table->unique("unique_email_account_id", "email", "account_id");
```

#### Dropping Indexes

User must specify name of index and unique constraint to drop them.

```c++
	/* C sample code */
	sq_table_drop_index(table, "index_email_account_id");
	sq_table_drop_unique(table, "unique_email_account_id");

	/* C++ sample code */
	table->dropIndex("index_email_account_id");
	table->dropUnique("unique_email_account_id");
```
