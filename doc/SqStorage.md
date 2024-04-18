[中文](SqStorage.cn.md)

# SqStorage

SqStorage use [Sqdb](Sqdb.md) to access database. It using [Sqxc](Sqxc.md) to convert data between C language and [Sqdb](Sqdb.md) interface.

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
* To use initializer to define (or change) table, see [schema-builder-constant.md](schema-builder-constant.md)
  
use C functions

```c
	// create table "users" in schema
	table = sq_schema_create(schema, "users", User);
	column = sq_table_add_int(table, "id", offsetof(User, id));
	sq_column_primary(column);
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);

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

**Specify container type**  
  
If you don't want to use pointer array, you can specify other container type.  
  
e.g. The container type is specified as SQ_TYPE_ARRAY (see [SqArray](SqArray.md)).

```c++
	SqArray    *array;
	const char *where = NULL;         // SQL WHERE Clause

	// C function
	array = sq_storage_get_all(storage, "users", NULL, SQ_TYPE_ARRAY, where);

	// C++ method
	array = storage->getAll("users", NULL, SQ_TYPE_ARRAY, where);
```

use C++ Standard Template Library (STL)  
  
The container type is specified as std::list<User>.

```c++
	std::list<User> *list;
	const char      *where = NULL;    // SQL WHERE Clause

	list = storage->getAll< std::list<User> >();
		// or
//	list = storage->getAll< std::list<User> >("users", NULL, where);
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

	list = storage->getAll< std::list<User> >("WHERE id > 10 AND id < 99");
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

use C++ methods  
  
Because C++ method updateAll() use parameter pack, the last argument can pass (or not) NULL.  

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
	// or use convenient C++ class 'where'
	n_changes  = storage->updateAll(user,
	                                Sq::where("id", ">", 10),
	                                "name", "email");
```

## updateField (Where conditions)

sq_storage_update_field() is similar to sq_storage_update_all(). User must append list of field's offset after parameter 'SQL statement' and the last argument must be -1.  
  
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

use C++ methods  
  
Because C++ method updateField() use parameter pack, the last argument can pass (or not) -1.  

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
	// or use convenient C++ class 'where'
	n_changes  = storage->updateField("users", &user,
	                                  Sq::where("id", ">", 10),
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
		// or use convenient C++ class 'where'
	storage->removeAll("users", Sq::where("id", ">", 50));
```

## Transaction

use C functions

```c
	User  *user;

	sq_storage_begin_trans(storage);
	sq_storage_insert(storage, "users", NULL, user);
	if (abort)
		sq_storage_rollback_trans(storage);
	else
		sq_storage_commit_trans(storage);
```

use C++ methods

```c++
	User  *user;

	storage->beginTrans();
	storage->insert(user);
	if (abort)
		storage->rollbackTrans();
	else
		storage->commitTrans();
```

## Custom query

SqStorage provides sq_storage_query() and C++ method query() to query with SqQuery. Like getAll(), If the program does not specify a container type, they will use the default container type [SqPtrArray](SqPtrArray.md).  

#### query without JOIN clause

In this case, the usage of table type and container type is basically the same as getAll().  
  
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

#### query with JOIN clause

If user executes a query that joins multiple tables without specifying table type, program will use [SqTypeJoint](SqTypeJoint.md) as table type by default. [SqTypeJoint](SqTypeJoint.md) can create array of pointers for query result.  
  
use C functions

```c
	sq_query_from(query, "cities");
	sq_query_join(query, "users", "cities.id", "=", "%s", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);

	for (int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);
		// free 'element' before you free 'array'
		// free(element);
	}
```

use C++ methods

```c++
	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	Sq::PtrArray *array = (Sq::PtrArray*) storage->query(query);

	for (int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")
		// free 'element' before you free 'array'
		// free(element);
	}
```

use C++ STL  
  
User can specify pointer to pointer (double pointer) as element of STL container.

```c++
	std::vector<void**> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector<void**> >(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		void **element = = vector->at(index);
		city = (City*)element[0];      // from("cities")
		user = (User*)element[1];      // join("users")
	}
```

If you don't want to use pointer as element of C++ STL container, you can use Sq::Joint to replace it.  
Sq::Joint just wraps array of pointers into struct. User must use it with C++ STL because C++ STL can NOT directly use array as element of container.

```c++
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector< Sq::Joint<2> > >(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Joint<2> &joint = vector->at(index);
		city = (City*)joint[0];      // from("cities")
		user = (User*)joint[1];      // join("users")
	}
```

#### convenient C++ class 'select' and 'from'

use C++ Sq::select or Sq::from to run database queries.

```c++
	// use Sq::select with query method
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));

	// use Sq::from with query method
	array = storage->query(Sq::from("users").whereRaw("city_id > 5"));
```

#### query with raw string

SqStorage provides sq_storage_query_raw() to query with raw string. Unlike getAll(), program must specify data type and container type because it will not use default container type when container type is NULL.  

use C function

```c
	int  *p2integer;
	int   max_id;

	// If you just query MAX(id), it will get an integer.
	// Therefore specify the table type as SQ_TYPE_INT and the container type as NULL.
	p2integer = sq_storage_query_raw(storage, "SELECT MAX(id) FROM table", SQ_TYPE_INT, NULL);
	// return integer pointer
	max_id = *p2integer;
	// free the integer pointer when no longer needed
	free(p2integer);

	// If you just query a row, it doesn't need a container.
	// Therefore specify the container type as NULL.
	table = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id = 1", tableType, NULL);

	// If you query multiple rows, it needs container.
	container = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id < 25", tableType, containerType);
```

use C++ method

```c++
	int  *p2integer;
	int   max_id;

	// If you just query MAX(id), it will get an integer.
	// Therefore specify the table type as SQ_TYPE_INT and the container type as NULL.
	p2integer = storage->query("SELECT MAX(id) FROM table", SQ_TYPE_INT, NULL);
	// return integer pointer
	max_id = *p2integer;
	// free the integer pointer when no longer needed
	free(p2integer);

	// If you just query a row, it doesn't need a container.
	// Therefore specify the container type as NULL.
	table = storage->query("SELECT * FROM table WHERE id = 1", tableType, NULL);

	// If you query multiple rows, it needs container.
	container = storage->query("SELECT * FROM table WHERE id < 25", tableType, containerType);
```

## use custom data type

Below C functions and C++ methods can return instance of custom data type and container type:  
See the documentation [SqType](SqType.md) to learn how to customize types.

| C functions               | C++ methods   |
| ------------------------- | ------------- |
| sq_storage_get()          | get()         |
| sq_storage_get_all()      | getAll()      |
| sq_storage_query()        | query()       |
| sq_storage_query_raw()    | query()       |

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

Note: SqStorage will try to find matched type if user does NOT specify object type. (except sq_storage_query_raw)  
Note: SqStorage will use default container type if user does NOT specify container type. (except sq_storage_query_raw)  
  
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
