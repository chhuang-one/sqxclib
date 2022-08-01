[中文](SqStorage.cn.md)

# SqStorage

SqStorage access database. It using Sqxc to convert data between C language and Sqdb interface.

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
// If you use C language, please use 'typedef' to to give a struct type a new name.
typedef struct User    User;

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

	// synchronize schema to database and update schema in 'storage'
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

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	storage->migrate(NULL);
```

Note1: Don't reuse 'schema_next' after migration because data is moved from 'schema_next' to 'schema_current'.  
Note2: If you use SQLite, you must synchronize schema to database after migration.  

## access database with user defined data type

below C functions and C++ methods can return instance of user defined data type ( [SqType](SqType.md) ):

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

## get

e.g. get one row from database table "users".

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

e.g. get all rows from database table "users".

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
	sq_query_where_raw(query, "id > 10");
	sq_query_where(query, "id < %d", 99);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

use C++ language

```c++
	array = storage->getAll("users",
			Sq::Query().whereRaw("id > 10").where("id < %d", 99).c());
```

#### convenient C++ class 'where'

use C++ Sq::where and Sq::whereRaw to generate SQL statement

```c++
	array = storage->getAll("users",
			Sq::whereRaw("id > 10").where("id < %d", 99));
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

SqStorage provides sq_storage_query() and C++ method query() to running database queries.  
  
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
