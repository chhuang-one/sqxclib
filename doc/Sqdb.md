[中文](Sqdb.cn.md)

# Sqdb

Sqdb is base structure for database product such as SQLite, MySQL, etc.
The following are the implemented database products:

| Derived structure | Database product | Source file    |
| ----------------- | ---------------- | -------------- |
| SqdbSqlite        | SQLite           | SqdbSqlite.c   |
| SqdbMysql         | MySQL            | SqdbMysql.c    |
| SqdbPostgre       | PostgreSQL       | SqdbPostgre.c  |

Refer to the document [database-interface.md](database-interface.md) to support new database products.

## Create and free Sqdb instance

When creating Sqdb instance, the database configuration (SqdbConfig) and interface (SqdbInfo) must be specified.

#### Database configuration

SqdbConfig is base structure for database configuration. The following are the derived database configurations:

| Derived configuration | Database product |
| --------------------- | ---------------- |
| SqdbConfigSqlite      | SQLite           |
| SqdbConfigMysql       | MySQL            |
| SqdbConfigPostgre     | PostgreSQL       |

Sample code: Configure database

```c
	// SQLite database configuration
	// set 'folder' and 'extension' in SqdbConfigSqlite can affect database filename and path.
	SqdbConfigSqlite  sqliteConfig;
	sqliteConfig.folder    = "/home/user";
	sqliteConfig.extension = "db";

	// MySQL database configuration
	SqdbConfigMysql  mysqlConfig;
	mysqlConfig.host = "localhost";
	mysqlConfig.port = 3306;
	mysqlConfig.user = "root";
	mysqlConfig.password = "";

	// PostgreSQL database configuration
	SqdbConfigPostgre  postgresConfig;
	postgresConfig.host = "localhost";
	postgresConfig.port = 5432;
	postgresConfig.user = "postgres";
	postgresConfig.password = "";
```

#### Database interface

SqdbInfo is database interface. The following are the provided database interfaces:

| SqdbInfo interface | Database product |
| ------------------ | ---------------- |
| SQDB_INFO_SQLITE   | SQLite           |
| SQDB_INFO_MYSQL    | MySQL            |
| SQDB_INFO_POSTGRE  | PostgreSQL       |

#### Create Sqdb instance

sqdb_new() will create Sqdb instance using the given interface and configuration. If configuration is NULL, use default setting.  
  
Sample code: Create Sqdb instance using SqdbConfig defined above.  
  
use C language

```c
	// database instance pointer
	Sqdb  *db;

	// method 1: use sqdb_new()
	// create Sqdb for SQLite
	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &sqliteConfig);
	// create Sqdb for MySQL
	db = sqdb_new(SQDB_INFO_MYSQL,  (SqdbConfig*) &mysqlConfig);

	// method 2: use sqdb_xxx_new()
	// create Sqdb for SQLite
	db = sqdb_sqlite_new(&sqliteConfig);
	// create Sqdb for PostgreSQL
	db = sqdb_postgre_new(&postgresConfig);
```

use C++ language

```c++
	// database instance pointer
	Sq::DbMethod  *db;

	// create Sqdb for SQLite
	db = new Sq::DbSqlite(sqliteConfig);

	// create Sqdb for PostgreSQL
	db = new Sq::DbPostgre(postgresConfig);
```

#### Free Sqdb instance

```c
	// C language
	sqdb_free(db);

	// C++ language
	delete db;
```

## Open and close database

sqdb_open() will get current schema version number while opening database.  
  
use C language

```c
	// open database.
	// If 'db' is instance of SqdbSqlite, it will open file - "/home/user/local-base.db"
	sqdb_open(db, "local-base");

	// close database
	sqdb_close(db);
```

use C++ language

```c++
	// open database
	// If 'db' is instance of SqdbSqlite, it will open file - "/home/user/local-base.db"
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
