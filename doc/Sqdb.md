[中文](Sqdb.cn.md)

# Sqdb

Sqdb is base structure for Database product such as SQLite, MySQL, etc.
The following are the implemented database products:

| derived structure | Database product | source file    |
| ----------------- | ---------------- | -------------- |
| SqdbSqlite        | SQLite           | SqdbSqlite.c   |
| SqdbMysql         | MySQL            | SqdbMysql.c    |
| SqdbPostgre       | PostgreSQL       | SqdbPostgre.c  |

## How to support new Database product:

Please refer to the document [database-interface.md](database-interface.md).

## Open and close database

sqdb_open() will get current schema version number while opening database.  
* SQLite user can set 'folder' and 'extension' in SqdbConfigSqlite, these affect database filename and path.

use C functions

```c
	// database configuration
	SqdbConfigSqlite config = {
		.folder    = "/home/someone",
		.extension = "db"
	};
	// database instance pointer
	Sqdb  *db;

	// create SqdbSqlite with 'config'. use default setting if config is NULL.
	db = sqdb_sqlite_new(&config);
	// result is the same as the previous line.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);

	// open database file - "/home/someone/local-base.db"
	sqdb_open(db, "local-base");

	// close database
	sqdb_close(db);
```

use C++ methods

```c++
	// database configuration
	Sq::DbConfigSqlite config = {
		"/home/someone",   // .folder    = "/home/someone",
		"db"               // .extension = "db",
	};
	// database instance pointer
	Sq::DbMethod  *db;

	// create Sq::DbSqlite with 'config'. use default setting if config is NULL.
	db = new Sq::DbSqlite(config);

	// open database file - "/home/someone/local-base.db"
	db->open("local-base");

	// close database
	db->close();
```

## Migrate

sqdb_migrate() use schema's version to decide to migrate or not. It has 2 schema parameters, the first parameter 'schemaCurrent' is the current version of the schema, and the second parameter 'schemaNext' is the next version of the schema. Changes of 'schemaNext' will be applied to 'schemaCurrent'.
You can't reuse 'schemaNext' after migration because this function may move data from 'schemaNext' to 'schemaCurrent'.  
  
Please do not alter, rename, and drop tables directly in 'schemaCurrent', but instead do these operations in 'schemaNext' then run migrate() to apply the changes to 'schemaCurrent'.  
  
To notify the database instance that the migration is completed, call sqdb_migrate() and pass NULL in parameter 'schemaNext'. This will clear unused data, sort tables and columns, and synchronize current schema to database (mainly for SQLite).  
  
use C functions

```c
	// apply changes of 'schemaNext' to 'schemaCurrent'
	sqdb_migrate(db, schemaCurrent, schemaNext);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort 'schemaCurrent' and synchronize 'schemaCurrent' to database (mainly for SQLite).
	sqdb_migrate(db, schemaCurrent, NULL);
```

use C++ methods

```c++
	// apply changes of 'schemaNext' to 'schemaCurrent'
	db->migrate(schemaCurrent, schemaNext);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort 'schemaCurrent' and synchronize 'schemaCurrent' to database (mainly for SQLite).
	db->migrate(schemaCurrent, NULL);
```

## Get result from SQL query

User can use sqdb_exec() to execute a query and get result by [Sqxc](Sqxc.md) elements.  
If you want to parse object or array and reuse [Sqxc](Sqxc.md) elements:
1. call sqxc_ready() before sqdb_exec().
2. call sqxc_finish() after sqdb_exec().

#### Get an integer value

```c
	SqxcValue *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	int   integer = -1;
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_INT;
	xc_input->instance  = &integer;

	code = sqdb_exec(db, "SELECT max(id) FROM users", (Sqxc*)xc_input, NULL);

	if (code == SQCODE_OK)
		return integer;
```

#### Get a row from "users" table

```c
	SqxcValue *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	UserTable *utable = calloc(1, sizeof(UserTable));
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = utable;

	// call sqxc_ready() before sqdb_exec() if you want to reuse Sqxc elements
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM users WHERE id = 1", (Sqxc*)xc_input, NULL);

	// call sqxc_finish() after sqdb_exec() if you want to reuse Sqxc elements
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return utable;
	else {
		free(utable);
		return NULL;
	}
```

#### Get multiple rows from "users" table

Use C language to get multiple rows from "users" table

```c
	SqxcValue  *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqPtrArray *array = sq_ptr_array_new(32, NULL);
	int         code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = array;

	// call sqxc_ready() before sqdb_exec() if you want to reuse Sqxc elements
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM users", (Sqxc*)xc_input, NULL);

	// call sqxc_finish() after sqdb_exec() if you want to reuse Sqxc elements
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return array;
	else {
		sq_ptr_array_free(array);
		return NULL;
	}
```

Use C++ language to get multiple rows from "users" table

```c++
	Sq::XcValue   *xc_input = new Sq::XcValue();
	Sq::PtrArray  *array    = new Sq::PtrArray();
	int   code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = array;

	xc_input->ready();
	code = db->exec("SELECT * FROM users", xc_input);
	xc_input->finish();

	if (code == SQCODE_OK)
		return array;
	else {
		delete array;
		return NULL;
	}
```
