[中文](database-interface.cn.md)

# Database interface

The Sqdb series are basic structures for operating database products. If you want to support a new database product, use the following structure.

| struct name | description                                                       |
| ----------- | ----------------------------------------------------------------- |
| Sqdb        | Base structure for Database product such as SQLite, MySQL, etc.   |
| SqdbInfo    | Interface of Database product.                                    |
| SqdbConfig  | Setting of Database product.                                      |

# Support new database product

Suppose we want to support a new database product Xsql, we need to complete the following steps:

## 0 Define

Add definitions in the header file.

```c
// Define Database product code for Xsql.
#define  SQDB_PRODUCT_XSQL    (SQDB_PRODUCT_CUSTOM + 1)

// Define SqdbInfo interface name for Xsql, global variable 'sqdbInfoXsql' will be defined later.
#define  SQDB_INFO_XSQL       (&sqdbInfoXsql)
```

## 1 Derive SqdbConfig

Use the macro SQDB_CONFIG_MEMBERS to inherit SqdbConfig member variables. The macro SQDB_CONFIG_MEMBERS will expand to two members:

```c
	unsigned int    product;      // reserve. value of enum SqdbProduct
	unsigned int    bit_field;    // reserve. Is the instance of config constant or dynamic?
```

Create SqdbConfigXsql for Xsql, which is derived from SqdbConfig.
Derived structure must conforme C++11 standard-layout.

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct SqdbConfigXsql    SqdbConfigXsql;

// config data structure that derived from SqdbConfig
struct SqdbConfigXsql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. inherit member variables

	int   xsql_setting;                    // <-- 2. Add variable and non-virtual function in derived struct.
};
```

## 2 Derive Sqdb

Use the macro SQDB_MEMBERS to inherit Sqdb member variables. The macro SQDB_MEMBERS will expand to two members:

```c
	const SqdbInfo *info;       // data and function interface
	int             version;    // schema version of the currently opened database
```

Create SqdbXsql for Xsql, which is derived from Sqdb.
Derived structure must conforme C++11 standard-layout.

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct SqdbXsql          SqdbXsql;

// structure that derived from Sqdb
#ifdef __cplusplus
struct SqdbXsql : Sq::DbMethod             // <-- 1. inherit C++ member function(method)
#else
struct SqdbXsql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. inherit member variables

	SqdbConfigXsql *config;                // <-- 3. Add variable and non-virtual function in derived struct.
	int             variable;


#ifdef __cplusplus
	// define C++ constructor and destructor here if you use C++ language.
	SqdbXsql(const SqdbConfigXsql *config) {
		// call Sq::DbMethod::init()
		init(SQDB_INFO_XSQL, (SqdbConfig*)config);
		// or call C function:
		// sqdb_init((Sqdb*)this, SQDB_INFO_XSQL, (SqdbConfig*)config);
	}
	SqdbXsql(const SqdbConfigXsql &config) {
		init(SQDB_INFO_XSQL, (SqdbConfig&)config);
	}
	~SqdbXsql() {
		// call Sq::DbMethod::final()
		final();
		// or call C function:
		// sqdb_final((Sqdb*)this);
	}
#endif
};
```

## 3 Implement SqdbInfo interface

Implement SqdbInfo interface for Xsql:

```c
// declare functions for SqdbInfo interface
static void sqdb_xsql_init(SqdbXsql *sqdb, const SqdbConfigXsql *config);
static void sqdb_xsql_final(SqdbXsql *sqdb);
static int  sqdb_xsql_open(SqdbXsql *sqdb, const char *databaseName);
static int  sqdb_xsql_close(SqdbXsql *sqdb);
static int  sqdb_xsql_exec(SqdbXsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xsql_migrate(SqdbXsql *sqdb, SqSchema *schema, SqSchema *schemaNext);

// Implement SqdbInfo interface
// Global variable 'sqdbInfoXsql' will be used when creating a Sqdb instance.
const SqdbInfo sqdbInfoXsql = {
	.size    = sizeof(SqdbXsql),         // Sqdb instance size
	.product = SQDB_PRODUCT_XSQL,        // Database product code

	.column  = {
		.has_boolean = 1,                // has Boolean Data Type
		.use_alter  = 1,                 // use "ALTER COLUMN" to change column
		.use_modify = 0,                 // use "MODIFY COLUMN" to change column
	},

	// for  Database column and table identifiers
	.quote = {
		.identifier = {'"', '"'}         // ANSI-SQL quote identifier is ""
		                                 // SQLite is "", MySQL is ``, SQL Server is []
	},

	// initialize derived structure of Sqdb
	.init    = sqdb_xsql_init,
	// finalize derived structure of Sqdb
	.final   = sqdb_xsql_final,

	// open a database file or establish a connection to a Database server
	.open    = sqdb_xsql_open,
	// close a previously opened file or connection.
	.close   = sqdb_xsql_close,
	// executes the SQL statement
	.exec    = sqdb_xsql_exec,
	// migrate schema. It apply changes of 'schemaNext' to 'schemaCurrent'
	.migrate = sqdb_xsql_migrate,
};
```

#### 3.1 init / final

When calling SqdbInfo::init(),  it should initialize Sqdb instance.  
When calling SqdbInfo::final(), it should finalize   Sqdb instance.

```c
static void sqdb_xsql_init(SqdbXsql *sqdb, const SqdbConfigXsql *config)
{
	// initialize SqdbXsql instance
	sqdb->version  = 0;
	sqdb->config   = config;
}

static void sqdb_xsql_final(SqdbXsql *sqdb)
{
	// finalize SqdbXsql instance
}
```

#### 3.2 open / close

When calling SqdbInfo::open(),  it should do connect, login, open file or database, and get schema version from SQL table.  
When calling SqdbInfo::close(), it should do disconnect, logout, close file or database.

```c
static int  sqdb_xsql_open(SqdbXsql *sqdb, const char *databaseName)
{
	// open database and get it's schema version
	sqdb->version = schemaVersion;

	return SQCODE_OK;
}

static int  sqdb_xsql_close(SqdbXsql *sqdb)
{
	// close database
	return SQCODE_OK;
}
```

#### 3.3 exec

If the parameter 'xc' is NULL, SQL statement 'sql' is executed directly, otherwise result of SQL statement is sent to 'xc' for conversion.

```c
static int  sqdb_xsql_exec(SqdbXsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
{
	if (xc == NULL) {
		// execute query
	}
	else {
		// Determines command based on the first character in SQL statement.
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
			// get rows from Xsql and send them to 'xc'
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
```

#### 3.4 migrate

SqdbInfo::migrate() use schema's version to decide to migrate or not. It has 2 schema parameters, the first 'schemaCurrent' parameter is the current version of the schema, and the second 'schemaNext' parameter is the next version of the schema. Changes of 'schemaNext' will be applied to 'schemaCurrent'.  
You can't reuse 'schemaNext' after migration because this function may move data from 'schemaNext' to 'schemaCurrent'.  
  
Please do not alter, rename, and drop tables directly in the first 'schemaCurrent' parameter, but instead do these operations in the second 'schemaNext' parameter then run migrate() to apply the changes to 'schemaCurrent'.  
  
To notify the database instance that the migration is completed, call SqdbInfo::migrate() and pass NULL in the last parameter. This will clear unused data, sort tables and columns, and synchronize current schema to database (mainly for SQLite).

```c
static int  sqdb_xsql_migrate(SqdbXsql *db, SqSchema *schemaCurrent, SqSchema *schemaNext)
{
	// If 'schemaNext' is NULL, update and sort 'schemaCurrent' and
	// synchronize 'schemaCurrent' to database (mainly for SQLite).
	if (schemaNext == NULL) {
		// sort tables and columns by their name
		sq_schema_sort_table_column(schema);
		return SQCODE_OK;
	}

	if (db->version < schemaNext->version) {
		// do migrations by 'schemaNext'
		for (unsigned int index = 0;  index < schemaNext->type->n_entry;  index++) {
			SqTable *table = (SqTable*)schemaNext->type->entry[index];
			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
				// Run migrations by records in 'table'.
				// Table related records is in 'table' and column related records is in 'table->type->entry'.
			}
			else if (table->name == NULL) {
				// DROP TABLE
				// Run migrations by records in 'table'.
				// Table related records is in 'table' and column related records is in 'table->type->entry'.
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
				// Run migrations by records in 'table'.
				// Table related records is in 'table' and column related records is in 'table->type->entry'.
			}
			else {
				// CREATE TABLE
				// Run migrations by records in 'table'.
				// Table related records is in 'table' and column related records is in 'table->type->entry'.
			}
		}
	}

	// include and apply changes from 'schemaNext'
	sq_schema_update(schemaCurrent, schemaNext);
	schemaCurrent->version = schemaNext->version;
}
```

## 4 Use custom Sqdb

use C language

```c++
	SqdbConfigXsql  config = {
		.xsql_setting = 0,
	};
	Sqdb           *db;
	SqStorage      *storage;

	// create custom Sqdb object with config data
	db = sqdb_new(SQDB_INFO_XSQL, (SqdbConfig*) &config);

	// create storage object that use new Sqdb
	storage = sq_storage_new(db);
```

use C++ language

```c++
	SqdbConfigXsql  config = {
		.xsql_setting = 0,
	};
	SqdbXsql       *db;
	Sq::Storage    *storage;

	// create custom Sqdb object with config data
	db = new SqdbXsql(config);

	// create storage object that use new Sqdb
	storage = new Sq::Storage(db);
```
