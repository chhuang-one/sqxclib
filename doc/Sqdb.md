[中文](Sqdb.cn.md)

# Sqdb

Sqdb is a base structure for Database product such as SQLite, MySQL, etc.  

| derived structure | Database product | source file    |
| ----------------- | ---------------- | -------------- |
| SqdbSqlite        | SQLite           | SqdbSqlite.c   |
| SqdbMysql         | MySQL            | SqdbMysql.c    |
| SqdbPostgre       | PostgreSQL       | SqdbPostgre.c  |

```c
struct Sqdb
{
	// you can use SQDB_MEMBERS to define below members
	const SqdbInfo *info;       // data and function interface
	int             version;    // schema version of the currently opened database
};
```

# SqdbInfo

SqdbInfo is interface for Database product.

```c
struct SqdbInfo
{
	uintptr_t      size;       // Sqdb instance size
	SqdbProduct    product;    // Database product code

	struct {
		unsigned int has_boolean:1;      // has Boolean Data Type
		unsigned int use_alter:1;        // use "ALTER COLUMN" to change column
		unsigned int use_modify:1;       // use "MODIFY COLUMN" to change column
	} column;

	// for  Database column and table identifiers
	struct {
		char         identifier[2];      // SQLite is "", MySQL is ``, SQL Server is []
	} quote;

	// initialize derived structure of Sqdb
	void (*init)(Sqdb *db, SqdbConfig *config);
	// finalize derived structure of Sqdb
	void (*final)(Sqdb *db);

	// open a database file or establish a connection to a Database server
	int  (*open)(Sqdb *db, const char *name);
	// close a previously opened file or connection.
	int  (*close)(Sqdb *db);
	// executes the SQL statement
	int  (*exec)(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
	// migrate schema. It apply changes of 'schema_next' to 'schema_current'
	int  (*migrate)(Sqdb *db, SqSchema *schema_current, SqSchema *schema_next);
};
```

# SqdbConfig

SqdbConfig is setting of Database product.

```c
struct SqdbConfig
{
	// you can use SQDB_CONFIG_MEMBERS to define below members
	unsigned int    product;      // reserve. value of enum SqdbProduct
	unsigned int    bit_field;    // reserve. Is the instance of config constant or dynamic?
};
```

## open and close database

sqdb_open() will get current schema version number while opening database.  
* SQLite user can set 'folder' and 'extension' in SqdbConfigSqlite, these affect database filename and path.

use C functions

```c
	// database configuration
	SqdbConfigSqlite config = {
		.folder    = "/home/dir",
		.extension = "db"
	};
	// database instance pointer
	Sqdb  *db;

	// create SqdbSqlite with 'config'. use default setting if config is NULL.
	db = sqdb_sqlite_new(&config);
	// result is the same as the previous line.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);

	// open database file - "/home/dir/local-base.db"
	sqdb_open(db, "local-base");

	// close database
	sqdb_close(db);
```

use C++ methods

```c++
	// database configuration
	Sq::DbConfigSqlite config = {
		"/home/dir",   // .folder    = "/home/dir",
		"db"           // .extension = "db",
	};
	// database instance pointer
	Sq::DbMethod  *db;

	// create Sq::DbSqlite with 'config'. use default setting if config is NULL.
	db = new Sq::DbSqlite(config);

	// open database file - "/home/dir/local-base.db"
	db->open("local-base");

	// close database
	db->close();
```

## migrate

sqdb_migrate() use schema's version to decide to migrate or not. It has 2 schema parameters, the first 'schema_current' parameter is the current version of the schema, and the second 'schema_next' parameter is the next version of the schema. Changes of 'schema_next' will be applied to 'schema_current'.  
You can't reuse 'schema_next' after migration because this function may move data from 'schema_next' to 'schema_current'.  
  
Please do not alter, rename, and drop tables directly in the first 'schema_current' parameter, but instead do these operations in the second 'schema_next' parameter then run migrate() to apply the changes to 'schema_current'.  
  
To notify the database instance that the migration is completed, call sqdb_migrate() and pass NULL in the last parameter. This will clear unused data, sort tables and columns, and synchronize current schema to database (mainly for SQLite).  
  
use C functions

```c
	// apply changes of 'schema_next' to 'schema_current'
	sqdb_migrate(db, schema_current, schema_next);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort 'schema_current' and synchronize 'schema_current' to database (mainly for SQLite).
	sqdb_migrate(db, schema_current, NULL);
```

use C++ methods

```c++
	// apply changes of 'schema_next' to 'schema_current'
	db->migrate(schema_current, schema_next);

	// To notify database instance that migration is completed, pass NULL to the last parameter.
	// This will update and sort 'schema_current' and synchronize 'schema_current' to database (mainly for SQLite).
	db->migrate(schema_current, NULL);
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

	code = sqdb_exec(db, "SELECT max(id) FROM migrations", (Sqxc*)xc_input, NULL);

	if (code == SQCODE_OK)
		return integer;
```

#### Get a row from "migrations" table

```c
	SqxcValue        *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqMigrationTable *mtable = calloc(1, sizeof(SqMigrationTable));
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = mtable;

	// call sqxc_ready() before sqdb_exec() if you want to reuse Sqxc elements
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM migrations WHERE id = 1", (Sqxc*)xc_input, NULL);

	// call sqxc_finish() after sqdb_exec() if you want to reuse Sqxc elements
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return mtable;
	else {
		free(mtable);
		return NULL;
	}
```

#### Get multiple rows from "migrations" table

Use C language to get multiple rows from "migrations" table

```c
	SqxcValue  *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqPtrArray *array = sq_ptr_array_new(32, NULL);
	int         code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = array;

	// call sqxc_ready() before sqdb_exec() if you want to reuse Sqxc elements
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM migrations", (Sqxc*)xc_input, NULL);

	// call sqxc_finish() after sqdb_exec() if you want to reuse Sqxc elements
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return array;
	else {
		sq_ptr_array_free(array);
		return NULL;
	}
```

Use C++ language to get multiple rows from "migrations" table

```c++
	Sq::XcValue   *xc_input = new Sq::XcValue();
	Sq::PtrArray  *array    = new Sq::PtrArray();
	int   code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = array;

	xc_input->ready();
	code = db->exec("SELECT * FROM migrations", xc_input);
	xc_input->finish();

	if (code == SQCODE_OK)
		return array;
	else {
		delete array;
		return NULL;
	}
```

## How to support new Database product:

User can refer SqdbMysql.h and SqdbMysql.c to support new Database product.  
SqdbEmpty.h and SqdbEmpty.c is a workable sample, but it do nothing.  

#### 1 define new structure that derived from SqdbConfig and Sqdb

All derived structure must conforme C++11 standard-layout

```c++
// This is header file - SqdbXxsql.h
#include <Sqdb.h>

// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct SqdbXxsql          SqdbXxsql;
typedef struct SqdbConfigXxsql    SqdbConfigXxsql;

// define Database product code
#define  SQDB_PRODUCT_XXSQL    (SQDB_PRODUCT_CUSTOM + 1)

#ifdef __cplusplus    // mix C and C++
extern "C" {
#endif

// define in SqdbXxsql.c
extern const SqdbInfo        sqdbInfo_XXSQL;
#define SQDB_INFO_XXSQL    (&sqdbInfo_XXSQL)

#ifdef __cplusplus    // mix C and C++
}  // extern "C"
#endif

// config data structure that derived from SqdbConfig
struct SqdbConfigXxsql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable

	int   xxsql_setting;                   // <-- 2. Add variable and non-virtual function in derived struct.
};

// structure that derived from Sqdb
#ifdef __cplusplus
struct SqdbXxsql : Sq::DbMethod            // <-- 1. inherit C++ member function(method)
#else
struct SqdbXxsql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable

	SqdbConfigXxsql *config;               // <-- 3. Add variable and non-virtual function in derived struct.
	int            variable;


#ifdef __cplusplus
	// define C++ constructor and destructor here if you use C++ language.
	SqdbXxsql(const SqdbConfigXxsql *config) {
		// call Sq::DbMethod::init()
		init(SQDB_INFO_XXSQL, (SqdbConfig*)config);
		// or call C function:
		// sqdb_init((Sqdb*)this, SQDB_INFO_XXSQL, (SqdbConfig*)config);
	}
	SqdbXxsql(const SqdbConfigXxsql &config) {
		init(SQDB_INFO_XXSQL, (SqdbConfig&)config);
	}
	~SqdbXxsql() {
		// call Sq::DbMethod::final()
		final();
		// or call C function:
		// sqdb_final((Sqdb*)this);
	}
#endif
};
```

#### 2 implement SqdbInfo interface

```c
// This is source file - SqdbXxsql.c
#include <SqdbXxsql.h>

// declare functions for SqdbInfo
static void sqdb_xxsql_init(SqdbXxsql *sqdb, const SqdbConfigXxsql *config);
static void sqdb_xxsql_final(SqdbXxsql *sqdb);
static int  sqdb_xxsql_open(SqdbXxsql *sqdb, const char *database_name);
static int  sqdb_xxsql_close(SqdbXxsql *sqdb);
static int  sqdb_xxsql_exec(SqdbXxsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xxsql_migrate(SqdbXxsql *sqdb, SqSchema *schema, SqSchema *schema_next);

// used by SqdbXxsql.h
const SqdbInfo sqdbInfo_XXSQL = {
	.size    = sizeof(SqdbXxsql),
	.product = SQDB_PRODUCT_XXSQL,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}     // ANSI-SQL quote identifier is ""
	},

	.init    = (void*) sqdb_xxsql_init,
	.final   = (void*) sqdb_xxsql_final,
	.open    = (void*) sqdb_xxsql_open,
	.close   = (void*) sqdb_xxsql_close,
	.exec    = (void*) sqdb_xxsql_exec,
	.migrate = (void*) sqdb_xxsql_migrate,
};

// implement functions of SqdbXxsql here

static void sqdb_xxsql_init(SqdbXxsql *sqdb, const SqdbConfigXxsql *config)
{
	// initialize SqdbXxsql instance
	sqdb->version  = 0;
	sqdb->config   = config;
}

static void sqdb_xxsql_final(SqdbXxsql *sqdb)
{
	// finalize SqdbXxsql instance
}

static int  sqdb_xxsql_open(SqdbXxsql *sqdb, const char *database_name)
{
	// open database and get it's schema version
	sqdb->version = schemaVersion;

	return SQCODE_OK;
}

static int  sqdb_xxsql_close(SqdbXxsql *sqdb)
{
	// close database
	return SQCODE_OK;
}

static int  sqdb_xxsql_exec(SqdbXxsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
{
	if (xc == NULL) {
		// execute query
	}
	else {
		// Determines command based on the first character in SQL statement.
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
			// get rows from xxsql and send them to 'xc'
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// set inserted row id to SqxcSql::id
			((SqxcSql*)xc)->id = inserted_id;
			break;

		case 'U':    // UPDATE
		case 'u':    // update
			// set number of rows changed to SqxcSql::changes
			((SqxcSql*)xc)->changes = number_of_rows_changed;
			break;

		default:
			// execute query
			break;
		}
	}

	if (error_occurred)
		return SQCODE_EXEC_ERROR;
	else
		return SQCODE_OK;
}

static int  sqdb_xxsql_migrate(SqdbXxsql *db, SqSchema *schema_current, SqSchema *schema_next)
{
	// If 'schema_next' is NULL, update and sort 'schema_current' and
	// synchronize 'schema_current' to database (mainly for SQLite).
	if (schema_next == NULL) {
		// sort tables and columns by their name
		sq_schema_sort_table_column(schema);
		return SQCODE_OK;
	}

	if (db->version < schema_next->version) {
		// do migrations by 'schema_next'
		for (unsigned int index = 0;  index < schema_next->type->n_entry;  index++) {
			SqTable *table = (SqTable*)schema_next->type->entry[index];
			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
			}
			else if (table->name == NULL) {
				// DROP TABLE
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
			}
			else {
				// CREATE TABLE
			}
		}
	}

	// include and apply changes from 'schema_next'
	sq_schema_update(schema_current, schema_next);
	schema_current->version = schema_next->version;
}
```

#### 3 use custom Sqdb

use C language

```c++
	SqdbConfigXxsql  config = {
		.xxsql_setting = 0,
	};
	Sqdb            *db;
	SqStorage       *storage;

	// create custom Sqdb object with config data
	db = sqdb_new(SQDB_INFO_XXSQL, (SqdbConfig*) &config);

	// create storage object that use new Sqdb
	storage = sq_storage_new(db);
```

use C++ language

```c++
	SqdbConfigXxsql  config = {
		.xxsql_setting = 0,
	};
	SqdbXxsql       *db;
	Sq::Storage     *storage;

	// create custom Sqdb object with config data
	db = new SqdbXxsql(config);

	// create storage object that use new Sqdb
	storage = new Sq::Storage(db);
```
