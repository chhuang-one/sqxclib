# SqStorage

SqStorage access database by using Sqxc to convert data to/from Sqdb interface.

## create SqStorage

User must specify [Sqdb](Sqdb.md) instance when creating SqStorage.  
  
use C language

```c
	Sqdb      *db;
	SqStorage *storage;

	db      = sqdb_new(SQDB_INFO_SQLITE, NULL);
	storage = sq_storage_new(db);
```

use C++ Language

```c++
	Sq::DbSqlite *db;
	Sq::Storage  *storage;

	db      = new Sq::DbSqlite();
	storage = new Sq::Storage(db);
```

## access database

SqStorage use [Sqdb](Sqdb.md) to access database.  
  
Define a data structure 'User' for SQL table "users".

```c++
typedef struct User  User;

struct User {
	int   id;
	char *name;
};
```

## open database and do migration

use C functions

```c
	// open database "sqxc_local"
	sq_storage_open(storage, "sqxc_local");

	// create table "users" in schema
	table = sq_schema_create(schema, "users", User);
	column = sq_table_int(table, "id", offsetof(User, id));
	column-bit_field |= SQB_PRIMARY;
	column = sq_table_string(table, "name", offsetof(User, name), -1);

	// migrate schema
	sq_storage_migrate(storage, schema);

	// synchronize schema to database and update schema/table status
	// This is mainly used by SQLite
	sq_storage_migrate(storage, NULL);
```

use C++ methods

```c
	// open database "sqxc_local"
	storage->open("sqxc_local");

	// create table "users" in schema
	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary()->autoIncrement();  // PRIMARY KEY
	table->string("name", &User::name);

	// migrate schema
	storage->migrate(schema);

	// synchronize schema to database and update schema/table status
	// This is mainly used by SQLite
	storage->migrate(NULL);
```

Note1: Don't reuse 'schema_next' after migration because data is moved from 'schema_next' to 'schema_current'.  
Note2: If you use SQLite, you must synchronize schema to database after migration.  

## get

get one row from database table "users".

```sql
SELECT * FROM users WHERE id = 3
```

use C language

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

get all rows from database table "users".

```sql
SELECT * FROM users
```

use C language

```c
	SqPtrArray *array;

	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);
```

use C++ language

```c++
	Sq::PtrArray *array;

	array = storage->getAll("users");
	// or
	array = storage->getAll<User>(NULL, NULL);
```

use C++ Standard Template Library (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>();
```

## getAll (Where conditions)

The last parameter in sq_storage_get_all() and Sq::Storage.getAll() is SQL statement that exclude "SELECT * FROM table_name".  
  
Note: SqQuery can generate SQL statement exclude "SELECT * FROM table_name"  
  
get multiple rows from database table "users" with where conditions.

```sql
SELECT * FROM users WHERE id > 10 AND id < 100
```

use C language

```c
	SqPtrArray *array;

	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 10 AND id < 100");
```

use C++ language

```c++
	Sq::PtrArray *array;

	array = storage->getAll("users", NULL, "WHERE id > 10 AND id < 100");
	// or
	array = storage->getAll<User>(NULL, "WHERE id > 10 AND id < 100");
```

use C++ Standard Template Library (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>("WHERE id > 10 AND id < 100");
```

## insert

sq_storage_insert() return inserted row id if primary key has auto increment attribute.  
  
use C functions

```c
	User  user;
	int   inserted_id;

	user.id   = 0;       // primary key set to 0 for auto increment
	user.name = "xname";
	inserted_id = sq_storage_insert(storage, "users", NULL, &user);
```

use C++ methods

```c++
	User  user;
	int   inserted_id;

	user.id   = 0;       // primary key set to 0 for auto increment
	user.name = "xname";
	inserted_id = storage->insert("users", &user);
```

## update

sq_storage_update() return number of rows changed  
  
use C functions

```c
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yname";
	n_changes = sq_storage_update(storage, "users", NULL, &user);
```

use C++ methods

```c++
	User  user;
	int   n_changes;

	user.id   = id;
	user.name = "yname";
	n_changes = storage->update("users", &user);
```

## remove

remove one rows from database table "users".

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

remove all rows from database table "users".

```c++
	// C function
	sq_storage_remove_all(storage, "users", NULL);

	// C++ method
	storage->removeAll("users");
```

## removeAll (Where conditions)

The last parameter in sq_storage_remove_all() and Sq::Storage.removeAll() is SQL statement that exclude "DELETE FROM table_name".  
  
Note: SqQuery can generate SQL statement exclude "DELETE FROM table_name".  
  
remove multiple rows from database table "users" with where conditions.

```c++
	// C function
	sq_storage_remove_all(storage, "users", "WHERE id > 50");

	// C++ method
	storage->removeAll("users", "WHERE id > 50");
```
