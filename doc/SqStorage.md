# SqStorage

SqStorage access database by using Sqxc to convert data to/from Sqdb interface.

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

## access database

SqStorage use [Sqdb](Sqdb.md) to access database.  
  
Define a data structure 'User' for SQL table "users".

```c++
typedef struct User  User;

struct User {
	int   id;
	char *name;
	char *email;
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
	column = sq_table_string(table, "email", offsetof(User, email), -1);

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
	table->string("email", &User::email);

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
```

use C++ Standard Template Library (STL)

```c++
	std::list<User> *list;

	list = storage->getAll<std::list<User>>();
```

## getAll (Where conditions)

The last parameter in sq_storage_get_all() and Sq::Storage.getAll() is SQL statement that exclude "SELECT * FROM table_name".  
  
get multiple rows from database table "users" with where conditions.

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

Note: SqQuery can generate SQL statement exclude "SELECT * FROM table_name"  
  
use C language

```c
	SqQuery *query = sq_query_new(NULL);
	sq_query_where(query, "id > %d", 10);
	sq_query_where(query, "id < %d", 99);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

use C++ language

```c++
	array = storage->getAll("users",
			Sq::Query().where("id > %d", 10).where("id < %d", 99).c());
```

use C++ language with convenient structure/function

```c++
	// use Sq::where to generate SQL statement
	array = storage->getAll("users",
			Sq::where("id > %d", 10)->where("id < %d", 99)->c());
```

## insert

sq_storage_insert() return inserted row id if primary key has auto increment attribute.  
  
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

sq_storage_update() update one row in database and return number of rows changed.  
  
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

update specific columns in multiple rows.

```sql
UPDATE "users" SET "name"='yael',"email"='user@server' WHERE id > 10
```

sq_storage_update_all() return number of rows changed. They have parameter for SQL statement that exclude "UPDATE table_name SET column=value". User must pass column_name list after parameter 'SQL statement' and the last argument must be NULL.  
  
Note: SqQuery can generate SQL statement exclude "UPDATE table_name SET column=value". Please see above "getAll (with SqQuery)".  
  
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

update specific fields in multiple rows.  
  
sq_storage_update_field() is similar to sq_storage_update_all(). User must pass field_offset list after parameter 'SQL statement' and the last argument must be -1.  
  
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

remove multiple rows from database table "users" with where conditions.

```sql
DELETE FROM users WHERE id > 50
```

The last parameter in sq_storage_remove_all() and Sq::Storage.removeAll() is SQL statement that exclude "DELETE FROM table_name".  
  
Note: SqQuery can generate SQL statement exclude "DELETE FROM table_name". Please see above "getAll (with SqQuery)".  
  
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
