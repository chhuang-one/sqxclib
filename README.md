[中文](README.cn.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib is a library to convert data between C language and SQL, JSON, etc. It provides ORM features and C++ wrapper.  
Project site: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## Current features:

* User can use C99 designated initializer or C++ aggregate initialization to define constant database table, column, and migration,
   this can reduce running time when making schema, see doc/[schema-builder-constant.md](doc/schema-builder-constant.md).
   You can also use C functions or C++ methods to do these dynamically.

* All defined table and column can use to parse JSON object and field. Program can also parse JSON object and array from database column.

* BLOB support. Supported types are listed in doc/[SqTable.md](doc/SqTable.md).

* It can add comments to tables and columns (only supported by MySQL/MariaDB and PostgreSQL).

* Custom type mapping.

* Query builder that can be used independently. See doc/[SqQuery.md](doc/SqQuery.md)

* It can work in low-end hardware.

* Single header file 〈 **sqxclib.h** 〉  (Note: It doesn't contain special macros and support libraries)

* User can use "no migration mode" if user doesn't need to sync migrations to database.

* Command-line tools can generate migration file and do migrate. See doc/[SqApp.md](doc/SqApp.md)

* Supports SQLite, MySQL / MariaDB, PostgreSQL.

* Provide project template. see directory [project-template](project-template).

## Database schema

Define a C structured data type to map database table "users".

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  User    User;

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;
	int     city_id;     // foreign key

	time_t  created_at;
	time_t  updated_at;

#ifdef __cplusplus       // C++ data type
	std::string       strCpp;
	std::vector<int>  intsCpp;
#endif
};
```

use C++ methods to define table and column in schema_v1 (dynamic):  
You can use create() method of Sq::Schema to create a table in schema.

```c++
/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

	/* create schema and specify version number as 1 */
	schema_v1 = new Sq::Schema(1, "Ver 1");

	// create table "users", then add columns to table.
	table = schema_v1->create<User>("users");

	// PRIMARY KEY
	table->integer("id", &User::id)->primary();
	// VARCHAR
	table->string("name", &User::name);
	// VARCHAR(60)
	table->string("email", &User::email, 60);
	// DEFAULT CURRENT_TIMESTAMP
	table->timestamp("created_at", &User::created_at)->useCurrent();
	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	table->timestamp("updated_at", &User::updated_at)->useCurrent()->useCurrentOnUpdate();
	// C++ types - std::string and std::vector
	table->stdstring("strCpp", &User::strCpp);
	table->custom("intsCpp", &User::intsCpp, &SqTypeIntVector);
	// FOREIGN KEY
	table->integer("city_id", &User::city_id)->reference("cities", "id");
	// CONSTRAINT FOREIGN KEY
	table->foreign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");
	// CREATE INDEX
	table->index("users_id_index", "id");

	/* If you store current time in columns (and members) and they use default name - 'created_at' and 'updated_at',
	   you can use below line to replace above 2 timestamp() methods.
	 */
//	table->timestamps<User>();
```

use C++ methods to change table and column in schema_v2 (dynamic):  
You can use alter() method to alter a table in schema.

```c++
	/* create schema and specify version number as 2 */
	schema_v2 = new Sq::Schema(2, "Ver 2");

	// alter table "users"
	table = schema_v2->alter("users");

	// add column to table
	table->integer("test_add", &User::test_add);
	// alter column in table
	table->integer("city_id", &User::city_id)->change();
	// DROP CONSTRAINT FOREIGN KEY
	table->dropForeign("users_city_id_foreign");
	// drop column
	table->dropColumn("name");
	// rename column
	table->renameColumn("email", "email2");
```

use C functions to define table and column in schema_v1 (dynamic):  
You can use sq_schema_create() function to create a table in schema.

```c
	/* create schema and specify version number as 1 */
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);

	// PRIMARY KEY
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	sq_column_primary(column);

	// VARCHAR
	column = sq_table_add_string(table, "name", offsetof(User, name), -1);
	// VARCHAR(60)
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);

	// DEFAULT CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_use_current(column);

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "updated_at", offset(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);

	// FOREIGN KEY.             NOTE: use NULL-terminated argument list here
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id", NULL);

	// CONSTRAINT FOREIGN KEY.  NOTE: use NULL-terminated argument list here
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id", NULL);
	sq_column_reference(column, "cities", "id", NULL);
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");

	// CREATE INDEX.            NOTE: use NULL-terminated argument list here
	column = sq_table_add_index(table, "users_id_index", "id", NULL);

	/* If you store current time in columns/members and they use default name - 'created_at' and 'updated_at',
	   you can use below line to replace above 2 sq_table_add_timestamp() functions.
	 */
//	sq_table_add_timestamps_struct(table, User);
```

use C functions to change table and column in schema_v2 (dynamic):  
You can use sq_schema_alter() function to alter a table in schema.

```c
	/* create schema and specify version number as 2 */
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// add column to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	// alter column in table
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_change(column);
	// DROP CONSTRAINT FOREIGN KEY
	sq_table_drop_foreign(table, "users_city_id_foreign");
	// drop column
	sq_table_drop_column(table, "name");
	// rename column
	sq_table_rename_column(table, "email", "email2");
```

**There are more...**  
  
* You can get more information about schema and migrations in doc/[database-migrations.md](doc/database-migrations.md)
* To use initializer to define (or change) table, see doc/[schema-builder-constant.md](doc/schema-builder-constant.md)
* To use macro to define (or change) table dynamically, see doc/[schema-builder-macro.md](doc/schema-builder-macro.md)

## Database products

**Sqdb** is base structure for Database products such as SQLite, MySQL, etc. You can get more description and example in doc/[Sqdb.md](doc/Sqdb.md)  
If user doesn't need to sync migrations to database, set SQDB_CONFIG_NO_MIGRATION in SqdbConfig::bit_field to use "no migration mode".  
  
e.g. Create Sqdb instance for SQLite database  
  
SQLite will open/create the file in the specified folder of SqdbConfigSqlite when user open database.  
In this example, database file path is "/path/databaseName.db".  
  
use C functions to create SQLite database instance

```c
	// database configuration
	SqdbConfigSqlite  config = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION,
		.folder    = "/path",
		.extension = "db"
	};
	// database instance pointer
	Sqdb  *db;

	db = sqdb_sqlite_new(&config);
//	db = sqdb_sqlite_new(NULL);                // use default setting if config is NULL.
```

use C++ methods to create SQLite database instance

```c++
	// database configuration
	Sq::DbConfigSqlite  config = {0};
//	config.bit_field = SQDB_CONFIG_NO_MIGRATION;
	config.folder    = "/path";
	config.extension = "db";
	// database instance pointer
	Sq::DbMethod  *db;

	db = new Sq::DbSqlite(config);
//	db = new Sq::DbSqlite(NULL);    // use default setting if config is NULL.
```

use C functions to create MySQL database instance  
  
MySQL, PostgreSQL must specify host, port, and authentication, etc. in their SqdbConfig.

```c
	// database configuration
	SqdbConfigMysql  config = {
//		.bit_field = SQDB_CONFIG_NO_MIGRATION,
		.host = "localhost",
		.port = 3306,
		.user = "name",
		.password = "xxx"
	};
	// database instance pointer
	Sqdb  *db;

	db = sqdb_mysql_new(&config);
//	db = sqdb_mysql_new(NULL);               // use default setting if config is NULL.
```

## Open Database

To access database, create [SqStorage](doc/SqStorage.md) and specify database instance [Sqdb](doc/Sqdb.md).  
  
use C functions to open database

```c
	SqStorage *storage;

	storage = sq_storage_new(db);
	sq_storage_open(storage, "databaseName");
```

use C++ methods to open database

```c++
	Sq::Storage *storage;

	storage = new Sq::Storage(db);
	storage->open("databaseName");
```

## Database migration

To do migration, use migrate function of [SqStorage](doc/SqStorage.md). It checks the schema version to decide whether to do.  
You can get more description about migrations and schema in doc/[database-migrations.md](doc/database-migrations.md).  
  
use C functions to migrate schema and synchronize to database

```c
	// migrate 'schema_v1' and 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort schema in SqStorage and synchronize schema to database (mainly for SQLite).
	sq_storage_migrate(storage, NULL);

	// free unused 'schema_v1' and 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```

use C++ methods to migrate schema and synchronize to database

```c++
	// migrate 'schema_v1' and 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort schema in SqStorage and synchronize schema to database (mainly for SQLite).
	storage->migrate(NULL);

	// free unused 'schema_v1' and 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

If you want to do this using separate migration files like Laravel, this library provided [SqApp](doc/SqApp.md) for this purpose.  
[SqApp](doc/SqApp.md) will use migration files in workspace/database/migrations. See doc/[SqApp.md](doc/SqApp.md) to get more information.

## CRUD

This library use [SqStorage](doc/SqStorage.md) to do Create, Read, Update, and Delete rows in database. These functions can use with [SqQuery](doc/SqQuery.md) (explain in "Query builder").  
To get more information and sample, you can see doc/[SqStorage.md](doc/SqStorage.md)

#### Get

User can specify the container type of returned data when getting multiple rows. If you does not specify a container type, getAll() and query() will use the default container type - [SqPtrArray](doc/SqPtrArray.md).  
  
use C functions  
  
The container type is specified as NULL (use default container type).

```c
	User       *user;
	SqPtrArray *array;

	// get multiple rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 8 AND id < 20");

	// get multiple rows with SqQuery (explain in "Query builder")
	sq_query_where(query, "id", ">", "%d", 8);
	sq_query_where_raw(query, "id < %d", 20);
	array = sq_storage_get_all(storage, "users", NULL, NULL, query->c());

	// get all rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);

	// get one row (where id = 2)
	user  = sq_storage_get(storage, "users", NULL, 2);
```

use C++ methods

```c++
	User         *user;
	Sq::PtrArray *array;

	// get multiple rows
	array = storage->getAll("users", "WHERE id > 8 AND id < 20");

	// get multiple rows with C++ class 'where' series (explain in "Query builder")
	array = storage->getAll("users", Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// get all rows
	array = storage->getAll("users");

	// get one row (where id = 2)
	user  = storage->get("users", 2);
```

use C++ template functions  
  
The container type is specified as std::vector<User>.

```c++
	User              *user;
	std::vector<User> *vector;

	// get multiple rows
	vector = storage->getAll< std::vector<User> >("WHERE id > 8 AND id < 20");

	// get multiple rows with C++ class 'where' series (explain in "Query builder")
	vector = storage->getAll< std::vector<User> >(Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// get all rows
	vector = storage->getAll< std::vector<User> >();

	// get one row (where id = 2)
	user = storage->get<User>(2);
```

#### Insert

The insert() can insert a row into table and return inserted row ID. It must specify the table name and struct instance. If the primary key is auto-incremented, its value can be set to 0.  
  
use C functions

```c
	User  user = {10, "Bob", "bob@server"};

	// insert one row
	id = sq_storage_insert(storage, "users", NULL, &user);
```

use C++ methods

```c++
	User  user = {10, "Bob", "bob@server"};

	// insert one row
	id = storage->insert("users", &user);
```

use C++ template functions

```c++
	User  user = {10, "Bob", "bob@server"};

	// insert one row
	storage->insert<User>(&user);
		// or
	storage->insert(&user);
```

#### Update

updateAll() is used to modify the existing records in a table and return number of rows changed. It can update specific columns by appending column names to its arguments.  
updateField() is similar to updateAll(). It can update specific columns by appending field's offset to its arguments.  
  
use C functions

```c
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// update one row
	n_changes = sq_storage_update(storage, "users", NULL, &user);

	// update specific columns - "name" and "email" in multiple rows.
	n_changes = sq_storage_update_all(storage, "users", NULL, &user, 
	                                  "WHERE id > 11 AND id < 28",
	                                  "name", "email",
	                                  NULL);

	// update specific fields - User::name and User::email in multiple rows.
	n_changes = sq_storage_update_field(storage, "users", NULL, &user, 
	                                    "WHERE id > 11 AND id < 28",
	                                    offsetof(User, name),
	                                    offsetof(User, email),
	                                    -1);
```

use C++ methods

```c++
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// update one row
	n_changes = storage->update("users", &user);

	// update specific columns - "name" and "email" in multiple rows.
	n_changes = storage->updateAll("users", &user,
	                               "WHERE id > 11 AND id < 28",
	                               "name", "email");

	// update specific fields - User::name and User::email in multiple rows.
	n_changes = storage->updateField("users", &user,
	                                 "WHERE id > 11 AND id < 28",
	                                 &User::name, &User::email);
```

use C++ template functions

```c++
	User  user = {10, "Bob2", "bob2@server"};
	int   n_changes;

	// update one row
	n_changes = storage->update<User>(&user);
		// or
	n_changes = storage->update(&user);

	// update specific columns - "name" and "email" in multiple rows.
	// call updateAll<User>(...)
	n_changes = storage->updateAll(&user,
	                               "WHERE id > 11 AND id < 28",
	                               "name", "email");

	// update specific fields - User::name and User::email in multiple rows.
	// call updateField<User>(...)
	n_changes = storage->updateField(&user,
	                                 "WHERE id > 11 AND id < 28",
	                                 &User::name, &User::email);
```

#### Remove

remove() can delete a row in table.  
removeAll() can delete multiple rows by condition. If no condition is specified, all rows in the table are deleted.  
  
use C functions

```c
	// remove one row (where id = 5)
	sq_storage_remove(storage, "users", NULL, 5);

	// remove multiple rows
	sq_storage_remove_all(storage, "users", "WHERE id < 5");
```

use C++ methods

```c++
	// remove one row (where id = 5)
	storage->remove("users", 5);

	// remove multiple rows
	storage->removeAll("users", "WHERE id < 5");
```

use C++ template functions

```c++
	// remove one row (where id = 5)
	storage->remove<User>(5);

	// remove multiple rows
	storage->removeAll<User>("WHERE id < 5");
```

## Query with raw string

sq_storage_query_raw() can query with raw string, program must specify data type and container type.

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
	table = sq_storage_query_raw(storage, "SELECT * FROM table WHERE id=1", tableType, NULL);
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
	table = storage->query("SELECT * FROM table WHERE id=1", tableType, NULL);
```

## Query builder

[SqQuery](doc/SqQuery.md) can generate SQL statement by using C functions or C++ methods, and provide where(), join(), on(), and having() series functions that support printf format.  
To get more information and sample, you can see doc/[SqQuery.md](doc/SqQuery.md)  
  
SQL statement

```sql
	SELECT id, age
	FROM companies
	JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
	WHERE age > 5
```

use C++ methods to produce query

```c++
	query->select("id", "age")
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", 100);
	     })
	     ->as("c")
	     ->onRaw("c.id = companies.city_id")
	     ->whereRaw("age > %d", 5);
```

use C functions to produce query

* sq_query_join_sub() is start of subquery.
* sq_query_end_sub()  is end of subquery.
* sq_query_join()     passing NULL in the last parameter is also the start of subquery.

```c
	sq_query_select(query, "id", "age");
	sq_query_from(query, "companies");

	sq_query_join_sub(query);      // start of subquery
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "%d", 100);
	sq_query_end_sub(query);       // end of subquery

	sq_query_as(query, "c");
	sq_query_on_raw(query, "c.id = companies.city_id");
	sq_query_where_raw(query, "age > %d", 5);
```

#### Using SqQuery with SqStorage

[SqStorage](doc/SqStorage.md) provides sq_storage_query() and C++ method query() to handle query.

```c++
	// C function
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ method
	array = storage->query(query);
```

[SqQuery](doc/SqQuery.md) provides sq_query_c() or C++ method c() to generate SQL statement for [SqStorage](doc/SqStorage.md).  
  
use C functions

```c
	// SQL statement exclude "SELECT * FROM ..."
	sq_query_clear(query);
	sq_query_where(query, "id", ">", "%d", 10);
	sq_query_or_where_raw(query, "city_id < %d", 22);

	array = sq_storage_get_all(storage, "users", NULL, NULL,
	                           sq_query_c(query));
```

use C++ methods

```c++
	// SQL statement exclude "SELECT * FROM ..."
	query->clear()
	     ->where("id", ">", 10)
	     ->orWhereRaw("city_id < %d", 22);

	array = storage->getAll("users", query->c());

	// overloaded function of getAll() can pass 'query' directly.
//	array = storage->getAll("users", query);
```

**convenient C++ class 'where' series**  
  
use operator() of Sq::Where (or Sq::where)

```c++
	Sq::Where  where;

	array = storage->getAll("users",
			where("id", ">", 10).orWhereRaw("city_id < %d", 22));
```

use constructor and operator of Sq::where

```c++
	// use parameter pack constructor
	array = storage->getAll("users",
			Sq::where("id", ">", 10).orWhereRaw("city_id < %d", 22));

	// use default constructor and operator()
	array = storage->getAll("users",
			Sq::where()("id", ">", 10).orWhereRaw("city_id < %d", 22));
```

Below is currently provided convenient C++ class:  

```
	Sq::Where,        Sq::WhereNot,
	Sq::WhereRaw,     Sq::WhereNotRaw,
	Sq::WhereExists,  Sq::WhereNotExists,
	Sq::WhereBetween, Sq::WhereNotBetween,
	Sq::WhereIn,      Sq::WhereNotIn,
	Sq::WhereNull,    Sq::WhereNotNull,

	'Where' class series use 'typedef' to give them new names: lower case 'where' class series.

	Sq::where,        Sq::whereNot,
	Sq::whereRaw,     Sq::whereNotRaw,
	Sq::whereExists,  Sq::whereNotExists,
	Sq::whereBetween, Sq::whereNotBetween,
	Sq::whereIn,      Sq::whereNotIn,
	Sq::whereNull,    Sq::whereNotNull,
```

**convenient C++ class 'select' and 'from'**

use C++ Sq::select or Sq::from to run database queries.

```c++
	// use Sq::select with query method
	array = storage->query(Sq::select("email").from("users").whereRaw("city_id > 5"));

	// use Sq::from with query method
	array = storage->query(Sq::from("users").whereRaw("city_id > %d", 5));
```

## JOIN support

[SqTypeJoint](doc/SqTypeJoint.md) is the default type for handling query that join multi-table. It can create array of pointers for query result.  
  
e.g. get result from query that join multi-table.  
  
use C functions

```c
	sq_query_from(query, "cities");
	sq_query_join(query, "users", "cities.id", "=", "%s", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);

	for (unsigned int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);

		// Because SqPtrArray doesn't free elements by default, free elements before freeing array.
//		free(element);
	}
```

use C++ methods

```c++
	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	Sq::PtrArray *array = (Sq::PtrArray*) storage->query(query);

	for (unsigned int i = 0;  i < array->length;  i++) {
		void **element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")

		// Because Sq::PtrArray doesn't free elements by default, free elements before freeing array.
//		free(element);
	}
```

use C++ STL  
  
User can specify pointer to pointer (double pointer) as element of STL container.

```c++
	std::vector<void**> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector<void**> >(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		void **element = vector->at(index);
		city = (City*)element[0];      // from("cities")
		user = (User*)element[1];      // join("users")
	}
```

If you don't want to use pointer as element of container:
1. use Sq::Joint as element of C++ STL container.
2. use typedef to define element type of [SqArray](doc/SqArray.md) for C language.

Refering [SqTypeJoint](doc/SqTypeJoint.md) to get more information.  
Here is one of the C++ STL examples:

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

## Parse unknown result

[SqTypeRow](doc/SqTypeRow.md) is derived from [SqTypeJoint](doc/SqTypeJoint.md). It create instance of [SqRow](doc/SqRow.md) and parse unknown (or known) result.  
SQ_TYPE_ROW is a built-in static constant type of SqTypeRow. Both [SqTypeRow](doc/SqTypeRow.md) and SQ_TYPE_ROW are in sqxcsupport library (sqxcsupport.h).  
  
use C functions

```c
	SqRow      *row;
	SqPtrArray *array;

	// specify the table type as SQ_TYPE_ROW
	// specify the container type of returned data as SQ_TYPE_PTR_ARRAY
	array = sq_storage_query(storage, query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	array = sq_storage_get_all(storage, "users", SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY, NULL);

	// specify the table type as SQ_TYPE_ROW
	row = sq_storage_get(storage, "users", SQ_TYPE_ROW, 11);
```

use C++ methods

```c++
	Sq::Row      *row;
	Sq::PtrArray *array;

	// specify the table type as SQ_TYPE_ROW
	// specify the container type of returned data as SQ_TYPE_PTR_ARRAY
	array = (Sq::PtrArray*) storage->query(query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	array = (Sq::PtrArray*) storage->getAll("users", SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY, NULL);

	// specify the table type as SQ_TYPE_ROW
	row = (Sq::Row*) storage->get("users", SQ_TYPE_ROW, 11);
```

use C++ STL

```c++
	Sq::Row               *row;
	std::vector<Sq::Row*> *rowVector;

	// specify the table type as SQ_TYPE_ROW
	// specify the container type of returned data as std::vector<Sq::Row*>
	rowVector = storage->query< std::vector<Sq::Row*> >(query, SQ_TYPE_ROW);

	rowVector = storage->getAll< std::vector<Sq::Row*> >("users", SQ_TYPE_ROW, NULL);

	// get first row
	row = rowVector->at(0);
```

[SqRow](doc/SqRow.md) contain 2 arrays. One is column array, another is data array.

```c
	// name of first column
	char   *columnName = row->cols[0].name;

	// data type of first column (columnType equal SQ_TYPE_STR in this example)
	SqType *columnType = row->cols[0].type;

	// value of first column (if columnType equal SQ_TYPE_STR)
	char   *columnValue = row->data[0].str;
```

## Transaction

	beginTrans():    Starts a new transaction.
	commitTrans():   Saves any changes made during the current transaction and ends the transaction.
	rollbackTrans(): Cancels any changes made during the current transaction and ends the transaction.

use C functions

```c
	sq_storage_begin_trans(storage);

	// do something to database here...

	if (abort)
		sq_storage_rollback_trans(storage);
	else
		sq_storage_commit_trans(storage);
```

use C++ methods

```c++
	storage->beginTrans();

	// do something to database here...

	if (abort)
		storage->rollbackTrans();
	else
		storage->commitTrans();
```

## Configuration header file

change build configuration.  
  
sqxclib is case-sensitive when searching and sorting database column name and JSON field name by default. User can change it in sqxc/[SqConfig.h](sqxc/SqConfig.h).

```c
// Common settings in SqConfig.h

/* sqxclib is case-sensitive when searching and sorting database column name and JSON field name by default.
   You may disable this for some old Database product.
   Affected source : SqEntry, SqRelation-migration
 */
#define SQ_CONFIG_ENTRY_NAME_CASE_SENSITIVE        1

/* If user doesn't specify SQL string length, program will use it by default.
   SQL_STRING_LENGTH_DEFAULT
 */
#define SQ_CONFIG_SQL_STRING_LENGTH_DEFAULT      191
```

## JSON support
- This library use [json-c](https://github.com/json-c/json-c) to parse/write JSON.
- all defined table and column can use to parse JSON object and field.
- program can also parse JSON object and array that store in column.

## Sqxc
Sqxc is used for data parsing and writing.  
User can link multiple Sqxc element to convert different types of data.  
You can get more description and example in doc/[Sqxc.md](doc/Sqxc.md)

## SqType
It define how to initialize, finalize, and convert C data type.  
*Sqxc* use it to convert data between C language and SQL, JSON, etc.  
You can get more description and example in doc/[SqType.md](doc/SqType.md)

## SqSchema
SqSchema defines database schema. It store table and changed record of table.  
You can get more description and example in doc/[SqSchema.md](doc/SqSchema.md)

## SqApp
SqApp use configuration file (SqApp-config.h) to initialize database and do migrations for user's application.  
It provide command-line program to generate migration and do migrate.  
See document in doc/[SqApp.md](doc/SqApp.md)

## SqConsole
SqConsole provide command-line interface (mainly for SqAppTool).  
See document in doc/[SqConsole.md](doc/SqConsole.md)

## Licensing

sqxclib is licensed under the Mulan PSL v2.


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

