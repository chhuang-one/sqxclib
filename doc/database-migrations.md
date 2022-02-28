# Database: Migrations (Draft)

This document describe how to define SQL table/column/migration dynamically.
You can also use SqApp to run and reverse migrations with command-line tool, see [SqApp.md](SqApp.md)  

Note: Because many users have used Laravel, there are many sqxclib C++ method names are similar to it.
Actually the design of sqxclib is different from Laravel, so the usage cannot be the same.  

## Running Migrations
You can use method/function to migrate schema and synchronize to database.

```c++
	/* C++ sample code */

	storage->migrate(schema_v1); // migrate schema_v1
	storage->migrate(schema_v2); // migrate schema_v2
	storage->migrate(NULL);      // synchronize schema to database. (Mainly used by SQLite)
	delete schema_v1;            // free unused schema_v1
	delete schema_v2;            // free unused schema_v2
```

```c
	/* C sample code */

	sq_storage_migrate(storage, schema_v1); // migrate schema_v1
	sq_storage_migrate(storage, schema_v2); // migrate schema_v2
	sq_storage_migrate(storage, NULL);      // synchronize schema to database. (Mainly used by SQLite)
	sq_schema_free(schema_v1);              // free unused schema_v1
	sq_schema_free(schema_v2);              // free unused schema_v2
```

## Tables

First, we define a C structured data type that mappings to your database table "users".

```c
typedef struct  User    User;    // add this line if you use C language

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;

	time_t  created_at;

	int     test_add;
};
```

### Creating Tables (dynamic)
 
You will get difference type name from C and C++ source code when you use gcc to compile because gcc's typeid(Type).name() will return strange name.  
**Please create or define type of SqTable in C++ language if your application written in C++ language.**  
  
If SqTable::type defined in C language, you may NOT use below C++ template functions to access SQL table.

	storage->insert<StructType>(...)
	storage->update<StructType>(...)
	storage->updateAll<StructType>(...)
	storage->remove<StructType>(...)
	storage->removeAll<StructType>(...)
	storage->get<StructType>(...)
	storage->getAll<StructType>(...)

use the create method/function in the schema to create a new database table.  
The method/function accepts two arguments: one argument is the name of table, another is structured data type.  

```c++
	/* C++ sample code */

	// create table "users"
	table = schema->create<User>("users");

	// add dynamic columns to table
	table->integer("id", &User::id)->primary();
	table->string("name", &User::name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->default_("CURRENT_TIMESTAMP");
```

```c
	/* C sample code */

	// create table "users"
	table = sq_schema_create(schema, "users", User);

	// add dynamic columns to table
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;        // set bit in SqColumn.bit_field

	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)

	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_default("CURRENT_TIMESTAMP");
```

#### Checking For Table Existence

You may check for the existence of a table using the find method/function:
* This method/function doesn't find table in database, it find table in instance of schema.

```c++
	/* C++ sample code */
	if (schema->find("users") == NULL) {
		// The "users" table exists...
	}

	/* C sample code */
	if (sq_schema_find(schema, "users") == NULL) {
		// The "users" table exists...
	}
```

### Updating Tables (dynamic)

use the alter method/function to update existing tables.

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
	column->bit_field |= SQB_CHANGED;
```

### Renaming / Dropping Tables (dynamic)

use the rename method/function to rename an existing database table.

```c++
	/* C++ sample code */
	schema->rename("old table name", "new table name");

	/* C sample code */
	sq_schema_rename(schema, "old table name", "new table name");
```

You can use the drop method/function to drop an existing table

```c++
	/* C++ sample code */
	schema->drop("users");

	/* C sample code */
	sq_schema_drop(schema, "users");
```


## Columns

### Creating Columns (dynamic)

To add columns to the table, you can use methods/functions in the SqTable.
You will get instance of SqTable after calling alter or create method/function in schema.

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

### Available Column Types (dynamic)

Below methods/functions are correspond to the different types of columns that you can add to your database tables.

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
| custom      | sq_table_add_custom     | *User define* |

* Because 'bool', 'int', and 'double' are C/C++ keywords, I must append '_' in tail of these method.

Below method is for C++ data type only.

| C++ methods | C++ data type |
| ----------- | ------------- |
| stdstring   | std::string   |

### Column Modifiers (dynamic)

There are several "modifiers" you may use when adding a column to table or a entry to structure.
For example, to make the column "nullable":

```c++
	/* C++ sample code */
	table->string("name", &User::name)->nullable();

	/* C sample code */
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column->bit_field |= SQB_NULLABLE;
```

Below methods/functions are correspond to Column Modifiers

| C++ methods          | C functions         | C bit field name      |
| -------------------- | ------------------- | --------------------- |
| primary()            |                     | SQB_PRIMARY           |
| unique()             |                     | SQB_UNIQUE            |
| primary()            |                     | SQB_PRIMARY           |
| autoIncrement()      |                     | SQB_AUTOINCREMENT     |
| nullable()           |                     | SQB_NULLABLE          |
| useCurrent()         |                     | SQB_CURRENT           |
| useCurrentOnUpdate() |                     | SQB_CURRENT_ON_UPDATE |
| default_(string)     | sq_column_default() |                       |

* Because 'default' is C/C++ keywords, I must append '_' in tail of this method.

Special methods/functions for structured data type.

| C++ methods      | C bit field name  | Description                                                  |
| ---------------- | ----------------- | ------------------------------------------------------------ |
| pointer()        | SQB_POINTER       | This data member is a pointer.                               |
| hidden()         | SQB_HIDDEN        | Don't output this data member to JSON.                       |
| hiddenNull()     | SQB_HIDDEN_NULL   | Don't output this data member to JSON if it's value is NULL. |

### Updating Column Attributes

C++ Language: The change method allows you to modify the type and attributes of existing columns.

```c++
	/* C++ sample code */

	// alter table "users"
	table = schema->alter("users");

	// alter column "email" in table
	table->string("email", &User::email, 100)->change();    // VARCHAR(100)
```

C language: set SQB_CHANGE in bit_field allows you to modify the type and attributes of existing columns.

```c
	/* C sample code */

	// alter table "users"
	table = sq_schema_alter(schema, "users", NULL);

	// alter column "email" in table
	column = sq_table_add_string(table, "email", offsetof(User, email), 100);    // VARCHAR(100)
	column->bit_field |= SQB_CHANGED;
```

### Renaming and Dropping Columns

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

### Creating Indexes

use the unique method onto the column definition:

```c++
	/* C++ sample code */
	table->string("email", &User::email)->unique();

	/* C sample code */
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	column->bit_field |= SQB_UNIQUE;
```

use method/function to create composite unique/index/primary key.  
The first argument specify the unique/index/primary name, others are list of column name that must be null-terminated.

```c++
	/* C++ sample code */

	table->index("index_email_account_id", "email", "account_id", NULL);

	table->unique("unique_email_account_id", "email", "account_id", NULL);

	table->primary("primary_email_account_id", "email", "account_id", NULL);

	/* C sample code */

	column = sq_table_add_index(table, "index_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_unique(table, "unique_email_account_id", "email", "account_id", NULL);

	column = sq_table_add_primary(table, "primary_email_account_id", "email", "account_id", NULL);
```

### Dropping Indexes

User must specify the index's name to drop an index/unique/primary.

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

### Foreign Key Constraints

use method/function to create composite unique/index/primary key.  
The first argument specify the foreign name, second is column name.

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

passing the name of the foreign key constraint to delete it.

```c++
	/* C++ sample code */
	table->dropForeign("users_city_id_foreign");

	/* C sample code */
	sq_table_drop_foreign(table, "users_city_id_foreign");
```
