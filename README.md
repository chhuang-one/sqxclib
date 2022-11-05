[中文](README.cn.md)

[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib is a library to convert data between C language and SQL (or JSON...etc).
It provides ORM features and C++ wrapper.  
Project site: [GitHub](https://github.com/chhuang-one/sqxclib), [Gitee](https://gitee.com/chhuang-one/sqxclib)

## Current features:
1. User can use C99 designated initializer or C++ aggregate initialization to define SQL table, column, and migration statically,
   this can reduce running time when making schema, see doc/[schema-builder-static.md](doc/schema-builder-static.md).
   You can also use C functions or C++ methods to do these dynamically.

2. All defined table and column can use to parse JSON object and field.
   Program can also parse JSON object and array from SQL column.

3. It can work in low-end hardware.

4. Single header file 〈 **sqxclib.h** 〉  (Note: It doesn't contain special macros and support libraries)

5. Command-line tools can generate migration and do migrate. See doc/[SqApp.md](doc/SqApp.md)

6. Supports SQLite, MySQL / MariaDB, PostgreSQL.

7. Provide project template. see directory [project-template](project-template).

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

use C++ methods to define table and column in schema_v1 (dynamic)

```c++
/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

	/* create schema version 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

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
	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
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

use C++ methods to change table and column in schema_v2 (dynamic)

```c++
	/* create schema version 2 */
	schema_v2 = new Sq::Schema("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

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

use C functions to define table and column in schema_v1 (dynamic)

```c
	/* create schema version 1 */
	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

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

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	column = sq_table_add_timestamp(table, "updated_at", offset(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);

	// FOREIGN KEY
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");

	// CONSTRAINT FOREIGN KEY
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");

	// CREATE INDEX
	column = sq_table_add_index(table, "users_id_index", "id", NULL);

	/* If you store current time in columns/members and they use default name - 'created_at' and 'updated_at',
	   you can use below line to replace above 2 sq_table_add_timestamp() functions.
	 */
//	sq_table_add_timestamps_struct(table, User);
```

use C functions to change table and column in schema_v2 (dynamic)

```c
	/* create schema version 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

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
* To use initializer to define (or change) table statically, see doc/[schema-builder-static.md](doc/schema-builder-static.md)
* To use macro to define (or change) table dynamically, see doc/[schema-builder-macro.md](doc/schema-builder-macro.md)

## Database product

**Sqdb** is base structure for database product (SQLite, MySQL...etc). You can get more description and example in doc/[Sqdb.md](doc/Sqdb.md)  
  
use C functions to open SQLite database

```c
	SqdbConfigSqlite  config = {
		.folder    = "/path",
		.extension = "db"
	};

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_SQLITE, NULL);     // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");    // This will open file "sqxc_local.db"
```

use C functions to open MySQL database

```c
	SqdbConfigMysql  config = {
		.host = "localhost",
		.port = 3306,
		.user = "name",
		.password = "xxx"
	};

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_MYSQL, NULL);    // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");
```

use C++ methods to open SQLite database

```c++
	Sq::DbConfigSqlite  config = {
		"/path",        // .folder    = "/path",
		"db",           // .extension = "db",
	};

	db = new Sq::DbSqlite(&config);
//	db = new Sq::DbSqlite(NULL);    // use default setting if config is NULL.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);    // this also works.

	storage = new Sq::Storage(db);
	storage->open("sqxc_local");    // This will open file "sqxc_local.db"
```

## Database migration

You can get more description about migrations and schema in doc/[database-migrations.md](doc/database-migrations.md).  
  
use C++ methods to migrate schema and synchronize to database

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

use C functions to migrate schema and synchronize to database

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

## CRUD

To get more information and sample, you can see doc/[SqStorage.md](doc/SqStorage.md)  
  
use C++ methods

```c++
	User         *user;
	Sq::PtrArray *array;

	// get multiple rows
	array = storage->getAll("users", "WHERE id > 8 AND id < 20");

	// get multiple rows with C++ class 'where' series (explain below "Query builder")
	array = storage->getAll("users", Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// get all rows
	array = storage->getAll("users");
	// get one row
	user  = storage->get("users", 2);

	// insert one row
	storage->insert("users", user);

	// update one row
	storage->update("users", user);
	// update specific columns - "name" and "email" in multiple rows.
	storage->updateAll("users", user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// update specific fields - User::name and User::email in multiple rows.
	storage->updateField("users", user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// remove one row
	storage->remove("users", 5);
	// remove multiple rows
	storage->removeAll("users", "WHERE id < 5");
```

use C++ template functions

```c++
	User              *user;
	std::vector<User> *vector;

	// get multiple rows
	vector = storage->getAll<std::vector<User>>("WHERE id > 8 AND id < 20");

	// get multiple rows with C++ class 'where' series (explain below "Query builder")
	vector = storage->getAll<std::vector<User>>(Sq::where("id", ">", 8).whereRaw("id < %d", 20));

	// get all rows
	vector = storage->getAll<std::vector<User>>();
	// get one row
	user = storage->get<User>(2);

	// insert one row
	storage->insert<User>(user);
		// or
	storage->insert(user);

	// update one row
	storage->update<User>(user);
		// or
	storage->update(user);

	// update specific columns - "name" and "email" in multiple rows.
	// call updateAll<User>(...)
	storage->updateAll(user,
	                   "WHERE id > 11 AND id < 28",
	                   "name", "email");
	// update specific fields - User::name and User::email in multiple rows.
	// call updateField<User>(...)
	storage->updateField(user,
	                     "WHERE id > 11 AND id < 28",
	                     &User::name, &User::email);

	// remove one row
	storage->remove<User>(5);
	// remove multiple rows
	storage->removeAll<User>("WHERE id < 5");
```

use C functions

```c
	User  *user;

	// get multiple rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, "WHERE id > 8 AND id < 20");
	// get all rows
	array = sq_storage_get_all(storage, "users", NULL, NULL, NULL);
	// get one row
	user  = sq_storage_get(storage, "users", NULL, 2);

	sq_storage_insert(storage, "users", NULL, user);

	// update one row
	sq_storage_update(storage, "users", NULL, user);
	// update specific columns - "name" and "email" in multiple rows.
	sq_storage_update_all(storage, "users", NULL, user, 
	                      "WHERE id > 11 AND id < 28",
	                      "name", "email",
	                      NULL);
	// update specific fields - User::name and User::email in multiple rows.
	sq_storage_update_field(storage, "users", NULL, user, 
	                        "WHERE id > 11 AND id < 28",
	                        offsetof(User, name),
	                        offsetof(User, email),
	                        -1);

	// remove one row
	sq_storage_remove(storage, "users", NULL, 5);
	// remove multiple rows
	sq_storage_remove_all(storage, "users", "WHERE id < 5");
```

## Query builder

SqQuery can generate SQL statement by using C functions or C++ methods.
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

* sq_query_join_sub() is start of subquery. It call sq_query_begin_sub()
* sq_query_end_sub()  is end of subquery.

```c
	sq_query_select(query, "id", "age");
	sq_query_from(query, "companies");
	sq_query_join_sub(query);
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "%d", 100);
	sq_query_end_sub(query);
	sq_query_as(query, "c");
	sq_query_on_raw(query, "c.id = companies.city_id");
	sq_query_where_raw(query, "age > %d", 5);
```

#### Using SqQuery with SqStorage

SqStorage provides sq_storage_query() and C++ method query() to handle query.

```c++
	// C function
	array = sq_storage_query(storage, query, NULL, NULL);

	// C++ method
	array = storage->query(query);
```

SqQuery provides sq_query_c() or C++ method c() to generate SQL statement for SqStorage.  
  
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

## JOIN support

SqTypeJoint is the default type for handling query that join multi-table. It creates an array of pointers for the results returned by the query.  
  
e.g. get result from query that join multi-table.  
  
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
  
Sq::Joint is pointer array that used by STL container.

```c++
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query<std::vector< Sq::Joint<2> >>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Joint<2> &joint = vector->at(index);
		city = (City*)joint[0];      // from("cities")
		user = (User*)joint[1];      // join("users")
	}
```

#### use SqTypeRow to get result

SqTypeRow is derived from SqTypeJoint. It create SqRow and handle unknown (or known) result, table, and column.  
SqTypeRow sample code is in [storage-row.cpp](examples/storage-row.cpp)  
Note1: SqTypeRow can also use with get() and getAll().  
Note2: SqTypeRow is in sqxcsupport library (sqxcsupport.h).  

	SqType
	│
	└─── SqTypeJoint
	     │
	     └─── SqTypeRow

**Directly use without setup:**  
In this case, all data type in SqRow is C string because SqTypeRow don't known type of column.

```c
	SqTypeRow  *typeRow;
	SqRow      *row;

	// C function
	row = sq_storage_get(storage, "users", typeRow, 12);

	// C++ method
//	row = storage->get("users", typeRow, 12);

	for (int  index = 0;  index < row->length;  index++)
		if (row->cols[index].type == SQ_TYPE_STRING)
			puts(row->data[index].string);
```

You can use SqTypeRow to replace default joint type in SqStorage:

```c++
	Sq::TypeRow   *typeRow     = new Sq::TypeRow();
	Sq::TypeJoint *typeDefault = storage->joint_default;
	storage->joint_default = typeRow;
	delete typeDefault;
```

**When SqTypeRow is default joint type, the element type of query() result is SqRow in the following cases:**
1. 'query' has joined multi-table.
2. 'query' has unknown table or unknown result.

```c++
	std::vector<Sq::Row> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query<std::vector<Sq::Row>>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Row &row = vector->at(index);
		for (unsigned int nth = 0;  nth < row.length;  nth++) {
			std::cout << row.cols[nth].name << " = ";
			if (row.cols[nth].type == SQ_TYPE_INT)
				std::cout << row.data[nth].integer << std::endl;
			if (row.cols[nth].type == SQ_TYPE_STRING)
				std::cout << row.data[nth].string  << std::endl;
			// other type...
		}
	}
```

**If you don't want to change default joint type:**
1. call sq_storage_setup_query() to setup 'query' and 'typeRow'.
2. call sq_storage_query() with 'typeRow'.
3. If you skip step 1, all data type in SqRow is C string because SqTypeRow don't known type of column.

function sq_storage_setup_query() declarations:

```c
// C function
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

// C++ method
Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
```

It setup 'query' and 'type_joint' then return SqType for calling sq_storage_query().  
It will add "SELECT table.column AS 'table.column'" in 'query' if 'query' has joined multi-table.  

| Return value  | Description                                                                |
| ------------- | ---------------------------------------------------------------------------|
| NULL          | if table not found and 'type_joint' can NOT handle unknown table type.     |
| 'type_joint'  | if 'query' has joined multi-table. It will setup 'type_joint' and 'query'. |
| type of table | if 'query' has only 1 table. It will setup 'type_joint' but keep 'query' no change. In this case, user can call sq_storage_query() with returned type or 'type_joint'. |

use C functions

```c
	SqType *typeContainer = NULL;

	sq_storage_setup_query(storage, query, typeRow);
	container = sq_storage_query(storage, query, typeRow, typeContainer);
```

use C++ methods

```c++
	Sq::Type *typeContainer = NULL;

	storage->setupQuery(query, typeRow);
	container = storage->query(query, typeRow, typeContainer);
```

use C++ STL

```c++
	storage->setupQuery(query, typeRow);
	vector = storage->query<std::vector<Sq::Row>>(query, typeRow);
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

## Configuration header file

change build configuration.  
  
sqxclib is case-sensitive when searching and sorting SQL column name and JSON field name by default. User can change it in sqxc/[SqConfig.h](sqxc/SqConfig.h).

```c
// Common settings in SqConfig.h

/* sqxclib is case-sensitive when searching and sorting SQL column name and JSON field name by default.
   You may disable this for some old SQL product.
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

## Sqdb
Sqdb is base structure for database product (SQLite, MySQL...etc).  
You can get more description and example in doc/[Sqdb.md](doc/Sqdb.md)  

## Sqxc
Sqxc is interface for data parse and write.  
User can link multiple Sqxc element to convert different types of data.  
You can get more description and example in doc/[Sqxc.md](doc/Sqxc.md)  

## SqApp
SqApp use configuration file (SqApp-config.h) to initialize database and do migrations for user's application.  
It provide command-line program to generate migration and do migrate.  
See document in doc/[SqApp.md](doc/SqApp.md)  

## SqConsole
SqConsole provide command-line interface (mainly for SqAppTool).  
See document in doc/[SqConsole.md](doc/SqConsole.md)  

## Others
SqType document: doc/[SqType.md](doc/SqType.md)  
SqEntry document (SqColumn's base class/structure): doc/[SqEntry.md](doc/SqEntry.md)  
SqColumn document: doc/[SqColumn.md](doc/SqColumn.md)  

## Licensing

sqxclib is licensed under the Mulan PSL v2.


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

