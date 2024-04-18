[中文](SqApp.cn.md)

# SqApp

SqApp use configuration file (SqApp-config.h) to initialize database and do migrations. It used by application.  
Note: SqApp is declared in SqApp.h of the sqxcapp library.  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool is used by command-line program - **sqxctool** and **sqxcpptool**. It use the same configuration values as SqApp.  
  
Both **sqxctool** and **sqxcpptool** can generate migration and do migrate. They can help with the user's application that using SqApp library. The difference is that sqxctool generate C migration file and sqxcpptool generate C++ migration file.

## 1 Create

Each instance of SqApp must specify setting when creating. SqAppSetting contains all settings required by SqApp.  
SQ_APP_DEFAULT is build-in default setting for SqAppSetting, user can change it by editing file (SqApp-config.h).  
  
use C language

```c
// sqxclib.h doesn't contain sqxcapp library
#include <SqApp.h>

	// 'SQ_APP_DEFAULT' has database settings and migration data for user application.
	SqApp *sqapp = sq_app_new(SQ_APP_DEFAULT);
```

use C++ language

```c++
// sqxclib.h doesn't contain sqxcapp library
#include <SqApp.h>

	// 'SQ_APP_DEFAULT' has database settings and migration data for user application.
	Sq::App *sqapp = new Sq::App(SQ_APP_DEFAULT);
```

## 2 Default Configurations

User can edit SqApp-config.h to change the default configuration used by SQ_APP_DEFAULT.

### 2.1 choose SQL product

User can use only one SQL products here (e.g. use MySQL)

```c++
// You can enable only one SQL products here

// #define DB_SQLITE      1
#define DB_MYSQL       1
// #define DB_POSTGRE     1
```

### 2.2 Database configuration values

DB_DATABASE is the default name of the database that SqApp will open.

```c++
// common configuration values
#define DB_DATABASE    "sqxcapp-example"

// connection configuration values
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""

// --- SQLite ---
// file configuration values
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL
```

* Please make sure that your app and it's sqxctool use the same path of database file if you enable SQLite.

### 2.3 replace SqApp-config.h by other config file

User can define macro SQ_APP_CONFIG_FILE to replace SqApp-config.h when compiling.  
  
e.g. use workspace/myapp-config.h to replace default workspace/sqxcapp/SqApp-config.h  
  
If myapp-config.h place in C include directories...

```
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

OR use relative path of workspace/sqxcapp

```
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

## 3 Open database

C function sq_app_open_database(), C++ method openDatabase() can open database with specified name. If user does not specify name, it will open database with default name.  
  
use C language

```c
	// open database that defined in SqApp-config.h
	if (sq_app_open_database(sqapp, NULL) != SQCODE_OK)
		return EXIT_FAILURE;
```

use C++ language

```c++
	// open database that defined in SqApp-config.h
	if (sqapp->openDatabase(NULL) != SQCODE_OK)
		return EXIT_FAILURE;
```

## 4 Migrations

C function sq_app_make_schema(), C++ method makeSchema() can produce schema by using migration files.  
User can specify which version of the schema to generate. If user specify version as 0, program will use default version (version of schema in database).  
  
Return values of function sq_app_make_schema():  
SQCODE_DB_SCHEMA_VERSION_0 : if the version of schema in database is 0 (no migrations have been done).  
SQCODE_DB_WRONG_MIGRATIONS : if these migrations are not for this database.  
  
use C language

```c
	int  version = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sq_app_make_schema(sqapp, version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

use C++ language

```c++
	int  version = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sqapp->makeSchema(version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

### 4.1 create migration files

generate C migration file by command-line program

```
sqxctool  make:migration  migration_name
```

This command will:
1. generate migration file - workspace/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. append relative path of migration file to workspace/sqxcapp/migrations-files.c
3. append declaration of migration to workspace/sqxcapp/migrations-declarations
4. append element of migrations array to workspace/sqxcapp/migrations-elements

If you use C++ to do migration, you can replace sqxctool by sqxcpptool. The difference is
**sqxcpptool** can generate C++ migration file and append path to workspace/sqxcapp/migrations-files.cpp  
  
Finally, you must recompile migration code after defining table.

#### 4.1.1 create table by sqxctool (C language)

e.g. generate C migration file to create "companies" table

```
sqxctool  make:migration  create_companies_table
```

Above command will create file workspace/database/migrations/yyyy_MM_dd_HHmmss_create_companies_table.c  
It is suggested that user define structure "Company" in workspace/sqxcapp/CStructs.h in this case.  
The file looks like below:

```c
/* migrations-files.c has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"        // define struct "Company" in CStructs.h
 */

// Run the migrations.
static void up_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "companies", Company);
}

// Reverse the migrations.
static void down_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	sq_schema_drop(schema, "companies");
}

const SqMigration create_companies_table_2021_12_12_180000 = {
	.up   = up_2021_12_12_180000,
	.down = down_2021_12_12_180000,

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
	.name = "2021_12_12_180000_create_companies_table",
#endif
};
```

#### 4.1.2 alter table by sqxcpptool (C++ language)

e.g. generate C++ migration file to alter "companies" table

```
sqxcpptool  make:migration  --table=companies  alter_companies_table
```

Above command will create file in workspace/database/migrations/yyyy_MM_dd_HHmmss_alter_companies_table.cpp  
The file looks like below:

```c++
/* This template file is used by sqxcpptool
// migrations-files.cpp has included below header files.
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"
 */

const SqMigration alter_companies_table_2021_12_26_191532 = {

	// Run the migrations.
//	.up = 
	[](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("companies");
	},

	// Reverse the migrations.
//	.down =
	[](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("companies");
	},

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
//	.name = 
	"2021_12_26_191532_alter_companies_table",
#endif
};

```

#### 4.1.3 migrate by sqxctool (or sqxcpptool)

Run all of your outstanding migrations

```
sqxctool  migrate
```

Rollback the last database migration

```
sqxctool  migrate:rollback
```

You may roll back a limited number of migrations by providing the step option to the rollback command.

```
sqxctool  migrate:rollback --step=5
```

### 4.2 migrate at runtime

When user migrate at runtime, column 'migrations.name' in database will be empty string because SqApp does NOT contain SqMigration.name string by default.  
Enable SQ_APP_HAS_MIGRATION_NAME in "migrations.h" to change default setting.  

#### 4.2.1 Run all of your outstanding migrations

sq_app_migrate() will run all of your outstanding migrations if 'step' is 0.  
  
use C language

```c
	int  step = 0;
	int  migration_id = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sq_app_make_schema(sqapp, migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(sqapp, step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

use C++ language

```c++
	int  step = 0;
	int  migration_id = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sqapp->makeSchema(migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sqapp->migrate(step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

#### 4.2.2 Rollback the last database migration

sq_app_rollback() will roll back the latest migration operation if 'step' is 0.  
  
use C language

```c
	int  step = 0;

	sq_app_rollback(app, step);
```

use C++ language

```c++
	int  step = 0;

	sqapp->rollback(step);
```

#### 4.3 Delete migration

1. delete migration file - workspace/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. remove relative path of migration file in workspace/sqxcapp/migrations-files.c
3. remove declaration of migration in workspace/sqxcapp/migrations-declarations
4. set element of migrations as NULL in workspace/sqxcapp/migrations-elements
  
Special note for point 2: C++ user must remove relative path of C++ migration file in workspace/sqxcapp/migrations-files.cpp  
  
Example for point 4: edit workspace/sqxcapp/migrations-elements to delete migration.

```c
// Before editing
& CreateUsersTable_2021_10_12_000000,

// After editing
NULL,
```

## 5 Multiple SqApp instance

For example, create two SqApps to synchronize the schema of SQLite and PostgreSQL.  
  
**Step 1:** Prepare two SqAppSetting for SQLite and PostgreSQL  
  
SQ_APP_DEFAULT_xxx series are default setting in 'SQ_APP_DEFAULT'.  

| name                        | description                  |
| ----------------------------| ---------------------------- |
| SQ_APP_DEFAULT_DATABASE     | default database name        |
| SQ_APP_DEFAULT_MIGRATIONS   | default migration array      |
| SQ_APP_DEFAULT_N_MIGRATIONS | total of default migrations  |
  
use C language

```c
SqdbConfigSqlite  configSQLite = {0};        // omitted

SqAppSetting  forSQLite = {
	SQDB_INFO_SQLITE,                  // .db_info
	(SqdbConfig*) &configSQLite,       // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};


SqdbConfigPostgre configPostgreSQL = {0};    // omitted

SqAppSetting  forPostgreSQL = {
	SQDB_INFO_POSTGRE,                 // .db_info
	(SqdbConfig*) &configPostgreSQL,   // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};
```

use C++ language

```c++
Sq::DbConfigSqlite  configSQLite = {0};      // omitted

Sq::AppSetting  forSQLite = {
	SQDB_INFO_SQLITE,                  // .db_info
	(SqdbConfig*) &configSQLite,       // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};


Sq::DbConfigPostgre configPostgreSQL = {0};  // omitted

Sq::AppSetting  forPostgreSQL = {
	SQDB_INFO_POSTGRE,                 // .db_info
	(SqdbConfig*) &configPostgreSQL,   // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};
```

**Step 2:** Create two SqApp with their settings  
  
use C language

```c
	SqApp *appSQLite = sq_app_new(&forSQLite);

	SqApp *appPostgreSQL = sq_app_new(&forPostgreSQL);
```

use C++ language

```c++
	Sq::App *appSQLite = new Sq::App(forSQLite);

	Sq::App *appPostgreSQL = new Sq::App(forPostgreSQL);
```

**Step 3:** run migrations  
  
use C language

```c
	if (sq_app_make_schema(appSQLite, 0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(appSQLite, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	if (sq_app_make_schema(appPostgreSQL, 0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(appPostgreSQL, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

use C++ language

```c++
	if (appSQLite->makeSchema(0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (appSQLite->migrate(0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	if (appPostgreSQL->makeSchema(0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (appPostgreSQL->migrate(0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```
