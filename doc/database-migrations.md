[中文](database-migrations.cn.md)

# Database: Migrations

This document describe how to define SQL table, column, and migration dynamically. Migration function can handle both dynamic and static column/table definitions.
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

First, we define a C structured data type that mappings to your database table "users".

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

use the create function in the schema to create a new database table.  
The function accepts two arguments: one argument is the name of table, another is structured data type.  

```c++
	/* C++ sample code */

	// create table "users"
	table = schema->create<User>("users");

	// add columns to table
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->default_("CURRENT_TIMESTAMP");
```

```c
	/* C sample code */

	// create table "users"
	table = sq_schema_create(schema, "users", User);

	// add columns to table
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_default("CURRENT_TIMESTAMP");
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

```c++
	/* C++ sample code */

	// alter table "users"
	table = schema->alter("users");

	// add column to table
	table->integer("test_add", &User::test_add);
	// alter column "email" in table
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

```c
	/* C sample code */

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

```c++
	/* C++ sample code */

	// alter table "users"
	table = schema->alter("users");

	// add column to table
	column = table->integer("test_add", &User::test_add);
```

```c
	/* C sample code */

	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// add columns to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
```

#### Available Column Types (dynamic)

Below C++ methods (and C functions) are correspond to the different types of columns that you can add to database tables.

| C++ methods | C functions            | C data type   | SQL data type     |
| ----------- | ---------------------- | ------------- | ----------------- |
| boolean     | sq_table_add_bool      | bool          | BOOLEAN           |
| bool_       | sq_table_add_bool      | bool          | BOOLEAN           |
| integer     | sq_table_add_integer   | int           | INT               |
| int_        | sq_table_add_int       | int           | INT               |
| uint        | sq_table_add_uint      | unsigned int  | INT (UNSIGNED)    |
| int64       | sq_table_add_int64     | int64_t       | BIGINT            |
| uint64      | sq_table_add_uint64    | uint64_t      | BIGINT (UNSIGNED) |
| timestamp   | sq_table_add_timestamp | time_t        | TIMESTAMP         |
| double_     | sq_table_add_double    | double        | DOUBLE            |
| str         | sq_table_add_str       | char*         | VARCHAR           |
| string      | sq_table_add_string    | char*         | VARCHAR           |
| char_       | sq_table_add_char      | char*         | CHAR              |
| custom      | sq_table_add_custom    | *User define* | VARCHAR           |

* Because 'bool', 'int', 'double', and 'char' are C/C++ keywords, it must append '_' in tail of these methods.

Below method is for C++ data type only.

| C++ methods | C++ data type |
| ----------- | ------------- |
| stdstr      | std::string   |
| stdstring   | std::string   |

#### Column Modifiers (dynamic)

There are several "modifiers" you may use when adding a column to table or a entry to structure.
For example, to make the column "nullable":

```c++
	/* C++ sample code */
	table->string("name", &User::name)->nullable();

	/* C sample code */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	sq_column_nullable(column);
```

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

use C++ method renameColumn and C function sq_table_rename_column to rename a column.  
use C++ method dropColumn and C function sq_table_drop_column to drop a column.

```c++
	/* C++ sample code */

	// alter table "users"
	table = schema->alter("users");
	// rename column
	table->renameColumn("from", "to");
	// drop column
	table->dropColumn("columnName");

	/* C sample code */

	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);
	// rename column
	sq_table_rename_column(table, "from", "to");
	// drop column
	sq_table_drop_column(table, "columnName");
```

## Indexes

#### Creating Indexes

use the unique method onto the column definition:

```c++
	/* C++ sample code */
	table->string("email", &User::email)->unique();

	/* C sample code */
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	sq_column_unique(column);
```

To use C functions to create composite unique, index, and primary key,
the 2nd argument specify the name of unique, index, primary key, others are list of column name that must be null-terminated.

```c
	column = sq_table_add_index(table, "index_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_unique(table, "unique_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_primary(table, "primary_email_account_id", "email", "account_id", NULL);
```

To use C++ methods to create composite unique, index, and primary key,
the 1st argument specify the name of unique, index, primary key, others are list of column name.  
Because C++ methods use parameter pack, the last argument can pass (or not) NULL.

```c++
	table->index("index_email_account_id", "email", "account_id");

	table->unique("unique_email_account_id", "email", "account_id");

	table->primary("primary_email_account_id", "email", "account_id");
```

#### Dropping Indexes

User must specify name of index, unique, and primary key to drop them.

```c++
	/* C++ sample code */

	table->dropIndex("index_email_account_id");

	table->dropUnique("unique_email_account_id");

	table->dropPrimary("primary_email_account_id");

	/* C sample code */

	sq_table_drop_index(table, "index_email_account_id");

	sq_table_drop_unique(table, "unique_email_account_id");

	sq_table_drop_primary(table, "primary_email_account_id");
```

## Foreign Key Constraints

use foreign function to create foreign key.  
The first argument specify the foreign key name, second is column name.

```c++
	/* C++ sample code */

	table->foreign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");

	/* C sample code */

	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");
```

#### Dropping Foreign Keys

use the name of the foreign key constraint to delete it.

```c++
	/* C++ sample code */
	table->dropForeign("users_city_id_foreign");

	/* C sample code */
	sq_table_drop_foreign(table, "users_city_id_foreign");
```
