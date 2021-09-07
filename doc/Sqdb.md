# Sqdb
  Sqdb is a base structure for database product (SQLite, MySQL...etc).  

| derived structure | Database name | source file  |
| ----------------- | ------------- | ------------ |
| SqdbSqlite        | SQLite        | SqdbSqlite.c |
| SqdbMysql         | MySQL         | SqdbMysql.c  |

```c
struct Sqdb
{
	// you can use SQDB_MEMBERS to define below members
	const SqdbInfo *info;       // data and function interface
	int             version;    // schema version in SQL database
};
```

## SqdbInfo

 SqdbInfo is data and function interface for database product.

```c
struct SqdbInfo
{
	uintptr_t      size;       // Sqdb instance size
	SqdbProduct    product;    // SqdbProduct product = SQLite, MySQL...etc

	struct {
		unsigned int has_boolean:1;      // has Boolean Data Type
		unsigned int use_alter:1;        // ALTER COLUMN
		unsigned int use_modify:1;       // MODIFY COLUMN
	} column;

	// for  Database column/table identifiers
	struct {
		char         identifier[2];      // SQLite is "", MySQL is ``, SQL Server is []
	} quote;

	// initialize derived structure of Sqdb
	void (*init)(Sqdb *db, SqdbConfig *config);
	// finalize derived structure of Sqdb
	void (*final)(Sqdb *db);

	// open a database file or establish a connection to a database server
	int  (*open)(Sqdb *db, const char *name);
	// close a previously opened file or connection.
	int  (*close)(Sqdb *db);
	// executes the SQL statement
	int  (*exec)(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
	// migrate schema from 'schema_next' to 'schema_cur'
	int  (*migrate)(Sqdb *db, SqSchema *schema_cur, SqSchema *schema_next);
};
```

## SqdbConfig

 SqdbConfig is setting of SQL product

```c
struct SqdbConfig
{
	// you can use SQDB_CONFIG_MEMBERS to define below members
	unsigned int    product;
	unsigned int    bit_field;    // reserve. constant or dynamic config data
};
```

## Get result from SQL query

User can use sqdb_exec() to execute a query and get result by Sqxc elements.  
- If you want to parse object or array and reuse Sqxc elements:
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

#### Get a row of "migrations" table

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

#### Get multiple rows of "migrations" table

Use C function to get multiple rows of "migrations" table

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

Use C++ function to get multiple rows of "migrations" table

```c++
	Sq::XcValue    *xc_input = new Sq::XcValue();
	Sq::PtrArray<> *array = new Sq::PtrArray<>();
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

## How to support new SQL product:
 User can refer SqdbMysql.h and SqdbMysql.c to support new SQL product.  
 SqdbEmpty.h and SqdbEmpty.c is a workable sample, but it do nothing.  

#### 1. define new structure that derived from SqdbConfig and Sqdb
 All derived structure must conforme C++11 standard-layout

```c++
// This is header file - SqdbXxsql.h
#include <Sqdb.h>

// define types - SqdbXxsql and SqdbConfigXxsql for C Language
typedef struct SqdbXxsql          SqdbXxsql;
typedef struct SqdbConfigXxsql    SqdbConfigXxsql;

// define SQL product id
#define  SQDB_PRODUCT_XXSQL    (SQDB_PRODUCT_CUSTOM + 1)

// define in SqdbXxsql.c
extern const SqdbInfo    *SQDB_INFO_XXSQL;

// ------------------------------------
// config data structure that derived from SqdbConfig
struct SqdbConfigXxsql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variable

	int   xxsql_setting1;                  // <-- 2. Add variable and non-virtual function in derived struct.
	int   xxsql_setting2;
};


// ------------------------------------
// structure that derived from Sqdb
#ifdef __cplusplus
struct SqdbXxsql : Sq::DbMethod            // <-- 1. inherit C++ member function(method)
#else
struct SqdbXxsql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variable

	SqdbConfigXxsql *config;               // <-- 3. Add variable and non-virtual function in derived struct.
	int    others;
};
```

#### 2. implement SqdbInfo interface

```c
// This is source file - SqdbXxsql.c
#include <SqdbXxsql.h>

// declare functions for SqdbInfo
static void sqdb_xxsql_init(SqdbXxsql *sqdb, SqdbConfigXxsql *config);
static void sqdb_xxsql_final(SqdbXxsql *sqdb);
static int  sqdb_xxsql_open(SqdbXxsql *sqdb, const char *database_name);
static int  sqdb_xxsql_close(SqdbXxsql *sqdb);
static int  sqdb_xxsql_exec(SqdbXxsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xxsql_migrate(SqdbXxsql *sqdb, SqSchema *schema, SqSchema *schema_next);

static const SqdbInfo dbinfo = {
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

// used by SqdbXxsql.h
const SqdbInfo *SQDB_INFO_XXSQL = &dbinfo;

// implement sqdb_xxsql_xxxx() functions here
//
```

#### 3. use custom Sqdb

```c++
	Sqdb       *db;
	SqdbConfigXxsql  config = { .xxsql_setting1 = 0};
	SqStorage       *storage;

	// create custom Sqdb object with config data
	db = sqdb_new(SQDB_INFO_XXSQL, (SqdbConfig*) &config);

	// create storage object that use new Sqdb
	/* C function */
	storage = sq_storage_new(db);
	/* C++ function */
//	storage = new Sq::Storage(db);
```

