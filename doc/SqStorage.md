[中文](SqStorage.cn.md)

# SqStorage

SqStorage use [Sqdb](Sqdb.md) to access database. It using Sqxc to convert data between C language and Sqdb interface.

## create storage

User must specify [Sqdb](Sqdb.md) instance when creating SqStorage.  
  
use C language

```c
	Sqdb      *db;
	SqStorage *storage;

	db      = sqdb_new(SQDB_INFO_SQLITE, NULL);
	storage = sq_storage_new(db);
```

use C++ language

```c++
	Sq::DbSqlite *db;
	Sq::Storage  *storage;

	db      = new Sq::DbSqlite();
	storage = new Sq::Storage(db);
```

## open database

use C functions

```c
	// open database "sqxc_local"
	sq_storage_open(storage, "sqxc_local");
```

use C++ methods

```c
	// open database "sqxc_local"
	storage->open("sqxc_local");
```

## do migration

Define a data structure 'User' for SQL table "users".

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct User    User;

struct User {
	int   id;
	char *name;
	char *email;
};
```

Here we use functions or methods for running migration dynamically.
* You can get more information about schema and migrations in [database-migrations.md](database-migrations.md)
* To use initializer to define (or change) table statically, see [schema-builder-static.md](schema-builder-static.md)
  
use C functions

```c
	// create table "users" in schema
	table = sq_schema_create(schema, "users", User);
	column = sq_table_int(table, "id", offsetof(User, id));
	sq_column_primary(column);
	column = sq_table_string(table, "name", offsetof(User, name), -1);
	column = sq_table_string(table, "email", offsetof(User, email), -1);

	// migrate schema
	sq_storage_migrate(storage, schema);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	sq_storage_migrate(storage, NULL);
```

use C++ methods

```c
	// create table "users" in schema
	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary()->autoIncrement();  // PRIMARY KEY
	table->string("name", &User::name);
	table->string("email", &User::email);

	// migrate schema
	storage->migrate(schema);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	storage->migrate(NULL);
```

Note1: Don't reuse 'schema' after migration because data is moved from 'schema' to 'storage->schema'.  
Note2: If you use SQLite, you must synchronize schema to database after migration.  

## get

The arguments of get() are table name, table type, and id.  
If table type is NULL, SqStorage will try to find table type in its schema.  
It can run a bit faster if user specify parameter table name and table type at the same time.  
  
e.g. get one row from database table "users".

```sql
SELECT * FROM users WHERE id = 3
```

use C language  
  
The table type is specified as NULL (find table type in its schema).

```c
	User *user;

	user = sq_storage_get(storage, "users", NULL, 3);
```

use C++ language

```c++
	User *user;

	user = storage->get("users", 3);
		// or
	user = storage->get<User>(3);
```

## getAll

The arguments of getAll() are table name, table type, container type, and SQL where conditions. It can specify table type and container type of returned data.  
If the program does not specify a container type, getAll() will use the default container type [SqPtrArray](SqPtrArray.md).  
e.g. get all rows from database table "users".

```sql
SELECT * FROM users
```

use C language  
  
The table type is specified as NULL (find table type in its schema).  
The container type is specified as NULL (use default container type).  

```c
	SqPtrArray *array;
	const char *where = NULL;         // SQL WHERE Clause

	array = sq_storage_get_all(storage, "users", NULL, NULL, where);
```

use C++ language

```c++
	Sq::PtrArray *array;
	const char   *where = NULL;       // SQL WHERE Clause

	array = storage->getAll("users");
		// or
//	array = storage->getAll("users", NULL, NULL, where);
```

use C++ Standard Template Library (STL)  
  
The container type is specified as std::list<User>.

```c++
	std::list<User> *list;
	const char      *where = NULL;    // SQL WHERE Clause

	list = storage->getAll<std::list<User>>();
		// or
//	list = storage->getAll<std::list<User>>("users", NULL, where);
```

## getAll (Where conditions)

The last parameter in sq_storage_get_all() is SQL statement that exclude "SELECT * FROM table_name".  
  
e.g. get multiple rows from database table "users" with where conditions.

```sql
SELECT * FROM users WHERE id > 10 AND id < 99
```

use C language

```c
	SqPtrArray *array;

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           "WHERE id > 10 AND id < 99");
```

use C++ language

```c++
	Sq::PtrArray *array;

	array = storage->getAll("users",
	                        "WHERE id > 10 AND id < 99");
	// or specify container type
	array = storage->getAll<User>(SQ_TYPE_PTR_ARRAY,
	                              "WHERE id > 10 AND id < 99");
```

use C++ Standard Template Library (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>("WHERE id > 10 AND id < 99");
```

## getAll (with SqQuery)

SqQuery can generate SQL statement that exclude "SELECT * FROM table_name"  
  
use C language

```c
	SqQuery *query = sq_query_new(NULL);
	sq_query_where_raw(query, "id > %d", 10);
	sq_query_where(query, "id", "<", "%d", 99);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

use C++ language

```c++
	Sq::Query *query = new Sq::Query();
	query->whereRaw("id > %d", 10);
	     ->where("id", "<", 99);

	array = storage->getAll("users", query->c());
```

#### convenient C++ class 'where'

use C++ Sq::where and Sq::whereRaw to generate SQL statement

```c++
	array = storage->getAll("users",
			Sq::whereRaw("id > 10").where("id", "<", 99));
```

## insert

sq_storage_insert() is used to insert a new record in a table and return inserted row id.  
  
use C functions

```c
	User  user;
	int   inserted_id;

	user.id   = 0;       // primary key set to 0 for auto increment
	user.name = "xman";
	inserted_id = sq_storage_insert(storage, "users", NULL, &user);
```

use C++ methods

```c++
	User  user;
	int   inserted_id;

	user.id   = 0;       // primary key set to 0 for auto increment
	user.name = "xman";
	inserted_id = storage->insert("users", &user);
	// or call template function: insert<User>(...)
	inserted_id = storage->insert(user);
```

## update

sq_storage_update() is used to modify an existing record in a table and return number of rows changed.  
  
use C functions

```c
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yael";
	n_changes = sq_storage_update(storage, "users", NULL, &user);
```

use C++ methods

```c++
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yael";
	n_changes = storage->update("users", &user);
	// or call template function: update<User>(...)
	n_changes = storage->update(user);
```

## updateAll (Where conditions)

sq_storage_update_all() is used to modify the existing records in a table and return number of rows changed.  
Parameter 'SQL statement' must exclude "UPDATE table_name SET column=value". Then append list of column name, the last argument must be NULL.  
  
Note: SqQuery can generate SQL statement exclude "UPDATE table_name SET column=value". Please see above "getAll (with SqQuery)".  
  
e.g. update specific columns in rows.

```sql
UPDATE "users" SET "name"='yael',"email"='user@server' WHERE id > 10
```

use C functions

```c
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = sq_storage_update_all(storage, "users", NULL, &user,
	                                   "WHERE id > 10",
	                                   "name", "email",
	                                   NULL);
```

Because C++ method updateAll() use parameter pack, the last argument can pass (or not) NULL.  
  
use C++ methods

```c++
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = storage->updateAll("users", &user,
	                                "WHERE id > 10",
	                                "name", "email");
	// or call template function: updateAll<User>(...)
	n_changes  = storage->updateAll(user,
	                                "WHERE id > 10",
	                                "name", "email");
```

## updateField (Where conditions)

sq_storage_update_field() is similar to sq_storage_update_all(). User must append list of field_offset after parameter 'SQL statement' and the last argument must be -1.  
  
use C functions

```c
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = sq_storage_update_field(storage, "users", NULL, &user,
	                                     "WHERE id > 10",
	                                     offsetof(User, name),
	                                     offsetof(User, email),
	                                     -1);
```

Because C++ method updateField() use parameter pack, the last argument can pass (or not) -1.  
  
use C++ methods

```c++
	User  user;
	int   n_changes;

	user.name  = "yael";
	user.email = "user@server";
	n_changes  = storage->updateField("users", &user,
	                                  "WHERE id > 10",
	                                  &User::name,
	                                  &User::email);
	// or call template function: updateField<User>(...)
	n_changes  = storage->updateField(user,
	                                  "WHERE id > 10",
	                                  &User::name,
	                                  &User::email);
```

## remove

sq_storage_remove() is used to delete an existing record in a table.  
  
e.g. remove one rows from database table "users".

```sql
DELETE FROM users WHERE id = 3
```

use C functions

```c
	sq_storage_remove(storage, "users", NULL, 3);
```

use C++ methods

```c++
	storage->remove("users", 3);
	// or
	storage->remove<User>(3);
```

## removeAll

e.g. remove all rows from database table "users".

```sql
DELETE FROM users
```

use C functions

```c
	sq_storage_remove_all(storage, "users", NULL);
```

use C++ methods

```c++
	storage->removeAll("users");
	// or
	storage->removeAll<User>();
```

## removeAll (Where conditions)

sq_storage_remove_all() is used to delete existing records in a table.  
The last parameter is SQL statement that exclude "DELETE FROM table_name".  
  
Note: SqQuery can generate SQL statement exclude "DELETE FROM table_name". Please see above "getAll (with SqQuery)".  
  
e.g. remove multiple rows from database table "users" with where conditions.

```sql
DELETE FROM users WHERE id > 50
```

use C functions

```c
	sq_storage_remove_all(storage, "users", "WHERE id > 50");
```

use C++ methods

```c++
	storage->removeAll("users", "WHERE id > 50");
	// or
	storage->removeAll<User>("WHERE id > 50");
```

## run custom query (with SqQuery)

SqStorage provides sq_storage_query() and C++ method query() to running database queries. Like getAll(), If the program does not specify a container type, they will use the default container type [SqPtrArray](SqPtrArray.md).  
  
use C function

```c
	SqType *userType;
	SqType *containerType;

	// find matched type and use default container
	array = sq_storage_query(storage, query, NULL, NULL);

	// return user defined data type
	container = sq_storage_query(storage, query, userType, containerType);
```

use C++ method

```c++
	// find matched type and use default container
	array = storage->query(query);

	// return user defined data type
	container = storage->query(query, userType, containerType);
```

## access database with user defined data type

Below C functions and C++ methods can return instance of user defined data type or container type:  
See the documentation [SqType](SqType.md) to learn how to customize types.

| C functions               | C++ methods   |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |

below functions can run a bit faster if user specify 'table_name' and 'table_type' at the same time.

| C functions               | C++ methods   |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |
| sq_storage_insert()       | insert()      |
| sq_storage_update()       | update()      |
| sq_storage_update_all()   | updateAll()   |
| sq_storage_update_field() | updateField() |

Note: SqStorage will try to find matched type if user does NOT specify object type.  
Note: SqStorage will use default container type if user does NOT specify container type.  
  
use C function  
  
SQ_TYPE_ROW       is built-in type in sqxcsupport library (sqxcsupport.h).  
SQ_TYPE_PTR_ARRAY is built-in container type.  

```c
	SqType     *rowType   = SQ_TYPE_ROW;
	SqRow      *row;
	SqType     *arrayType = SQ_TYPE_PTR_ARRAY;
	SqPtrArray *array;

	row = sq_stoarge_get(storage, "users", rowType, 10);

	array = sq_storage_get_all(storage, "users", rowType, arrayType, NULL);
	for (int i = 0;  i < array.length;  i++) {
		row = array->data[i];
		// do something here
	}
```

use C++ method

```c++
	Sq::Type     *rowType   = SQ_TYPE_ROW;
	Sq::Row      *row;
	Sq::Type     *arrayType = SQ_TYPE_PTR_ARRAY;
	Sq::PtrArray *array;

	row = (Sq::Row*)storage->get("users", rowType, 10);

	array = (Sq::PtrArray*)storage->getAll("users", rowType, arrayType, NULL);
	for (int i = 0;  i < array.length;  i++) {
		row = array->data[i];
		// do something here
	}
```
