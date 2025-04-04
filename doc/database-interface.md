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
			// execute SQL SELECT statement
			// get rows from Xsql and send them to 'xc'
			sqdb_xsql_exec_select(sqdb, sql, (SqxcValue*)xc);
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// execute SQL INSERT statement
			// store id of inserted row in SqxcSql::id
			((SqxcSql*)xc)->id = inserted_id;
			break;

		case 'U':    // UPDATE
		case 'u':    // update
			// execute SQL UPDATE statement
			// store number of changed rows in SqxcSql::changes
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

sqdb_xsql_exec_select() is a function that processes result of SELECT statements. It shows the processing flow of SQL rows and columns.

```c
int  sqdb_xsql_exec_select(SqdbXsql *sqdb, const char *sql, SqxcValue *xc)
{
	// If SqxcValue is prepared to receive multiple rows
	if (xc->container != NULL) {
		// SQL multiple rows corresponds to SQXC_TYPE_ARRAY
		xc->type = SQXC_TYPE_ARRAY;
		xc->name = NULL;
		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
	}

	// get row
	while (row = xsql_get_row(sqdb)) {
		// Special case:
		// Don't send object if user selects only one column and the column type is built-in types (not object).
		if (SQ_TYPE_NOT_BUILTIN(xc->element)) {
			// SQL row corresponds to SQXC_TYPE_OBJECT
			xc->type = SQXC_TYPE_OBJECT;
			xc->name = NULL;
			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}

		// get column
		while (col = xsql_get_column(sqdb)) {
			// send SQL columns to Sqxc chain
			xc->type = SQXC_TYPE_STR;
			xc->name = columnName1;
			xc->value.str = columnValue1;
			xc = sqxc_send(xc);
			if (xc->code != SQCODE_OK) {
				// error occurred
			}
		}

		// Special case:
		// Don't send object if user selects only one column and the column type is built-in types (not object).
		if (SQ_TYPE_NOT_BUILTIN(xc->element)) {
			// SQL row corresponds to SQXC_TYPE_OBJECT
			xc->type = SQXC_TYPE_OBJECT_END;
			xc->name = NULL;
			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}
	}

	// If SqxcValue is prepared to receive multiple rows
	if (xc->container != NULL) {
		// SQL multiple rows corresponds to SQXC_TYPE_ARRAY
		xc->type = SQXC_TYPE_ARRAY_END;
		xc->name = NULL;
//		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
	}
}
```

#### 3.4 migrate

SqdbInfo::sqdb_migrate() use schema's version to decide to migrate or not. It has 2 schema parameters, the first parameter 'schemaCurrent' is the current version of the schema, and the second parameter 'schemaNext' is the next version of the schema. Changes of 'schemaNext' will be applied to 'schemaCurrent'.
This function can move data from 'schemaNext' to 'schemaCurrent', so user can't reuse 'schemaNext' after migration.  
  
To notify the database instance that the migration is completed, call SqdbInfo::migrate() and pass NULL in parameter 'schemaNext'. This will clear unused data, sort tables and columns, and synchronize current schema to database (mainly for SQLite).

```c
static int  sqdb_xsql_migrate(SqdbXsql *db, SqSchema *schemaCurrent, SqSchema *schemaNext)
{
	SqBuffer *buffer;

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
			// Run migrations by records in 'table'.
			// Table related records is in 'table' and column related records is in 'table->type->entry'.

			// Determine the operation to be performed (ALTER, DROP, RENAME, ADD).
			// The processing flow here is the same as for 'column'.
			if (table->bit_field & SQB_CHANGED) {
				// ALTER TABLE
				// table->name is the name of the table to be altered
				sqdb_xsql_alter_table(sqdb, buffer, table);
			}
			else if (table->name == NULL) {
				// DROP TABLE
				// table->old_name is the name of the table to be deleted
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// RENAME TABLE
				// table->old_name is the name of the table to be renamed
				// table->name     is the new name of the table
			}
			else {
				// CREATE TABLE
				// table->name     is the name of the table to be created
				sqdb_xsql_create_table(sqdb, buffer, table);
			}
		}
	}

	// include and apply changes from 'schemaNext'
	sq_schema_update(schemaCurrent, schemaNext);
	schemaCurrent->version = schemaNext->version;
}
```

sqdb_xsql_alter_table() is a function that alters table. It shows the processing flow of records in 'table'.
Please refer to function sqdb_exec_alter_table() in Sqdb.c for more details.

```c
int  sqdb_xsql_alter_table(SqdbXsql *db, SqBuffer *buffer, SqTable *table)
{
	SqPtrArray *columnArray;

	columnArray = sq_type_entry_array(table->type);
	for (unsigned int index = 0;  index < columnArray->length;  index++) {
		SqColumn *column = (SqColumn*)columnArray->data[index];

		// Determine the operation to be performed (ALTER, DROP, RENAME, ADD).
		// The processing flow here is the same as for 'table'.
		if (column->bit_field & SQB_COLUMN_CHANGED) {
			// ALTER COLUMN
			// column->name is the name of the column to be altered
		}
		else if (column->name == NULL) {
			// DROP COLUMN / CONSTRAINT / INDEX / KEY
			// column->old_name is the name of the column to be deleted
		}
		else if (column->old_name && (column->bit_field & SQB_COLUMN_RENAMED) == 0) {
			// RENAME COLUMN
			// column->old_name is the name of the column to be renamed
			// column->name     is the new name of the column
		}
		else {
			// ADD COLUMN / CONSTRAINT / INDEX / KEY
			// column->name     is the name of the column to be added
		}
	}
}
```

sqdb_xsql_create_table() is a function that creates table. It creates SQL table by records in 'table'.
There are many omissions in the following code, please refer to function sqdb_sql_write_column() in Sqdb.c for more details.

```c
int  sqdb_xsql_create_table(SqdbXsql *db, SqBuffer *buffer, SqTable *table)
{
	SqPtrArray *columnArray;

	columnArray = sq_type_entry_array(table->type);
	for (unsigned int index = 0;  index < columnArray->length;  index++) {
		SqColumn *column = (SqColumn*)columnArray->data[index];

		// write column name
		sq_buffer_write(buffer, column->name);

		// write column type
		int sql_type = column->sql_type;
		if (sql_type == SQ_SQL_TYPE_UNKNOWN) {
			// map column->type to SqSqlType
			if (SQ_TYPE_IS_BUILTIN(type))
				sql_type = (int)SQ_TYPE_BUILTIN_INDEX(type) + 1;
			else
				sql_type = SQ_SQL_TYPE_VARCHAR;
		}

		switch (sql_type) {
		case SQ_SQL_TYPE_BOOLEAN:
			if (db->info->column.has_boolean)
				sq_buffer_write(buffer, "BOOLEAN");
			else
				sq_buffer_write(buffer, "TINYINT");
			break;

		case SQ_SQL_TYPE_INT:
		case SQ_SQL_TYPE_INT_UNSIGNED:
			// omitted
			break;

		// omitted
		}

		// write other attributes
		if (column->bit_field & SQB_COLUMN_NULLABLE)
			sq_buffer_write(buffer, " NOT NULL");
		if (column->bit_field & SQB_COLUMN_AUTOINCREMENT)
			sq_buffer_write(buffer, " AUTO_INCREMENT");
		// omitted
	}
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
