[中文](SqApp.cn.md)

# SqApp

SqApp is developed based on sqxclib, supports separate migration files for migration, and uses the configuration file (SqApp-config.h) to initialize the database.  
  
Note: SqApp is declared in SqApp.h of the sqxcapp library.  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool is used by command-line program - **sqtool** and **sqtool-cxx**. It use the same configuration values as SqApp.  
  
Both **sqtool** and **sqtool-cxx** can generate separate migration files and use them to do migrate. They can help with the application that using SqApp library. The difference is that sqtool generate C migration file and sqtool-cxx generate C++ migration file.

## 1 Configurations

SqApp-config.h is default configuration file of SqApp. You can also create a SqAppSetting instance and specify settings of SqApp in it.

#### 1.1 Configuration file

SqApp-config.h is configuration file of SqApp, it contains default settings of connection, authentication, etc.  
  
The **first part** of SqApp-config.h can choose Database product:

```c++
// You can enable only one Database product here

// #define DB_SQLITE      1
#define DB_MYSQL       1
// #define DB_POSTGRE     1
```

The **second part** of SqApp-config.h is database configuration values:

	DB_NO_MIGRATION  If you don't need to sync migrations to database, set it to 1.
	DB_DATABASE      is the default name of the database that SqApp will open.
	DB_HOST          is settings of connection.
	DB_PORT          is settings of connection.
	DB_USERNAME      is settings of authentication.
	DB_PASSWORD      is settings of authentication.
	DB_FOLDER        is the folder name. SQLite create or access database files in this folder.
	DB_EXTENSION     is extension name of SQLite database files.

* Please make sure that your app and it's sqtool use the same path of database file if you enable SQLite.

```c++
// common configuration values
#define DB_NO_MIGRATION    0
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

#### 1.2 Replace SqApp-config.h by other config file

User can define macro SQ_APP_CONFIG_FILE to replace SqApp-config.h when compiling.  
  
e.g. use workspace/myapp-config.h to replace default workspace/sqapp/SqApp-config.h  
  
If myapp-config.h place in C include directories, enclose the file name in angle brackets.

```console
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

If the configuration file is in relative path of workspace/sqapp, enclose the file name in double-quotes.

```console
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

#### 1.3 SqAppSetting

SqAppSetting contains all settings required by SqApp.  
SQ_APP_DEFAULT is an instance of SqAppSetting, which uses the values ​​in SqApp-config.h to set its member values.
So developer can edit SqApp-config.h to change values in SQ_APP_DEFAULT.  
  
e.g. create SqAppSetting with default settings.  
  
SQ_APP_DEFAULT_XXXX below are settings in SQ_APP_DEFAULT.

```
	SqAppSetting  settings;

	settings.db_info      = SQ_APP_DEFAULT_DB_INFO;
	settings.db_config    = SQ_APP_DEFAULT_DB_CONFIG;
	settings.db_database  = SQ_APP_DEFAULT_DATABASE;
	settings.migrations   = SQ_APP_DEFAULT_MIGRATIONS
	settings.n_migrations = SQ_APP_DEFAULT_N_MIGRATIONS;
```

## 2 Create SqApp

Each instance of SqApp must specify SqAppSetting when creating.  
SQ_APP_DEFAULT is the default settings as mentioned above.  
  
e.g. create SqApp with default settings  
  
use C language

```c
#include <sqxc/sqxclib.h>      // sqxclib.h includes sqxc/app headers


	SqApp *sqApp;

	// 'SQ_APP_DEFAULT' has database settings and migration data for application.
	sqApp = sq_app_new(SQ_APP_DEFAULT);
```

use C++ language

```c++
#include <sqxc/sqxclib.h>      // sqxclib.h includes sqxc/app headers


	Sq::App *sqApp;

	// 'SQ_APP_DEFAULT' has database settings and migration data for application.
	sqApp = new Sq::App(SQ_APP_DEFAULT);
```

## 3 Open database

C function sq_app_open_database(), C++ method openDatabase() can open database with specified name. If no name is specified, it will use default name defined in SqApp-config.h to open database.  
  
use C language

```c
	// if databaseName = NULL, open database name defined in SqApp-config.h
	char *databaseName = "web";

	if (sq_app_open_database(sqApp, databaseName) != SQCODE_OK)
		return EXIT_FAILURE;
```

use C++ language

```c++
	// if databaseName = NULL, open database name defined in SqApp-config.h
	char *databaseName = "web";

	if (sqApp->openDatabase(databaseName) != SQCODE_OK)
		return EXIT_FAILURE;
```

After opening the database, you can get current schema version in the database from SqApp::db.version.

```c
	int  schemaVersion;

	schemaVersion = sqApp->db->version;
```

## 4 Migrations

C function sq_app_make_schema(), C++ method makeSchema() can produce schema by using migration files.  
Developer can specify which version of the schema to generate. If version 0 is specified, program will use default version (version of schema in database).

	Return value of C function sq_app_make_schema() and C++ makeSchema():

	SQCODE_DB_SCHEMA_VERSION_0 : if the version of schema in database is 0 (no migrations have been done).
	SQCODE_DB_WRONG_MIGRATIONS : if these migrations are not for this database.

use C language

```c
	int  version = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sq_app_make_schema(sqApp, version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

use C++ language

```c++
	int  version = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sqApp->makeSchema(version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

### 4.1 create migration files

C migration files are created by sqtool, C++ migration files are created by sqtool-cxx. Each migration file has a instance of SqMigration for migration.

	To create new database table:

	1. Define structure 'Newbie' for database table "newbies" in header file sqapp/CStructs.h

	2. Run following command in the console to generate migration file:
	   $ sqtool      make:migration  create_newbies_table  (generate C   migration file)
	   $ sqtool-cxx  make:migration  create_newbies_table  (generate C++ migration file)

	   This command will:
	   append declaration of migration to     sqapp/migrations-declarations
	   append element of migrations array to  sqapp/migrations-elements

	   append relative path of migration file to  sqapp/migrations-files.c        by sqtool
	                                          or  sqapp/migrations-files-cxx.cpp  by sqtool-cxx

	3. Edit generated migration file  yyyy_MM_dd_HHmmss_create_newbies_table.c    (generated by sqtool)
	                              or  yyyy_MM_dd_HHmmss_create_newbies_table.cpp  (generated by sqtool-cxx)
	   in folder  database/migrations/

Finally, you must recompile migration code after defining table.

#### 4.1.0 migration files

Each migration file in the directory database/migrations defines a SqMigration instance. SqMigration::name is the description of the migration.  
If SqMigration::name does NOT contain string, the column 'migrations.name' in database will be empty string.  
  
You can set SQ_APP_HAS_MIGRATION_NAME to 0 in "sqapp/migrations.h", SqApp will NOT contain string in SqMigration::name.
This can reduce application binary size.  

#### 4.1.1 create table by sqtool (C language)

e.g. generate C migration file to create "companies" table

```console
sqtool  make:migration  create_companies_table
```

Above command will create file yyyy_MM_dd_HHmmss_create_companies_table.c in workspace/database/migrations.  
It is recommended to define structure 'Company' in workspace/sqapp/CStructs.h in this case.  
The file looks like below:

```c
/* This template file is used by sqtool
 * Please define structure 'Company' in workspace/sqapp/CStructs.h
 *
 * Normally this file should be included in migrations-files.c if you use sqtool to make migration file.
 * migrations-files.c has included following headers.
 * #include <sqxc/SqStorage.h>
 * #include <sqxc/app/SqMigration.h>
 * #include "CStructs.h"
 */


// Run the migrations.
static void up_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "companies", Company);

	column = sq_table_add_integer(table, "id", offsetof(Company, id));
	sq_column_primary(column);
}

// Reverse the migrations.
static void down_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	sq_schema_drop(schema, "companies");
}

const SqMigration createCompaniesTable_2021_12_12_180000 = {
	.up   = up_2021_12_12_180000,
	.down = down_2021_12_12_180000,

#if SQ_APP_HAS_MIGRATION_NAME
	.name = "2021_12_12_180000_create_companies_table",
#endif
};
```

#### 4.1.2 alter table by sqtool-cxx (C++ language)

To alter a table, you must specify the table name using the --table option of sqtool or sqtool-cxx.  
  
e.g. generate C++ migration file to alter "companies" table

```console
sqtool-cxx  make:migration  --table=companies  alter_companies_table
```

Above command will create file yyyy_MM_dd_HHmmss_alter_companies_table.cpp in workspace/database/migrations.  
The file looks like below:

```c++
/* This template file is used by sqtool-cxx
 *
 * Normally this file should be included in migrations-files-cxx.cpp if you use sqtool-cxx to make migration file.
 * migrations-files-cxx.cpp has included following headers.
 * #include <sqxc/SqStorage.h>
 * #include <sqxc/app/SqMigration.h>
 * #include "CStructs.h"
 */


const SqMigration alterCompaniesTable_2021_12_26_191532 = {

	// Run the migrations.
//	.up =
	[](SqSchema *schema, SqStorage *storage) {
		Sq::Table  *table;

		table = schema->alter("companies");

		// alter columns in table
	},

	// Reverse the migrations.
//	.down =
	[](SqSchema *schema, SqStorage *storage) {
		Sq::Table  *table;

		table = schema->alter("companies");

		// alter columns in table
	},

#if SQ_APP_HAS_MIGRATION_NAME
//	.name =
	"2021_12_26_191532_alter_companies_table",
#endif
};

```

#### 4.1.3 migrate by sqtool (or sqtool-cxx)

Run all of your outstanding migrations

```console
sqtool  migrate
```

rolls back the last "batch" of migrations

```console
sqtool  migrate:rollback
```

You may roll back a limited number of migrations by providing the step option to the rollback command.

```console
sqtool  migrate:rollback  --step=5
```

### 4.2 migrate at runtime

Developer can use migrate() and rollback() to do migration at runtime.

#### 4.2.1 Run all of your outstanding migrations

sq_app_migrate() will run all of your outstanding migrations if 'step' is 0.  
  
use C language

```c
	int  step = 0;
	int  migration_id = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sq_app_make_schema(sqApp, migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(sqApp, step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

use C++ language

```c++
	int  step = 0;
	int  migration_id = 0;

	// if the version of schema in database is 0 (no migrations have been done)
	if (sqApp->makeSchema(migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sqApp->migrate(step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

#### 4.2.2 Rolling Back Migrations

sq_app_rollback() will roll back the last "batch" of migrations if 'step' is 0.  
  
use C language

```c
	int  step = 0;

	sq_app_rollback(sqApp, step);
```

use C++ language

```c++
	int  step = 0;

	sqApp->rollback(step);
```

#### 4.3 Delete migration

To delete migration that created by sqtool, you must:

	1. delete migration file  yyyy_MM_dd_HHmmss_migration_name.c    (generated by sqtool)
	                      or  yyyy_MM_dd_HHmmss_migration_name.cpp  (generated by sqtool-cxx)
	   in folder  database/migrations/

	2. remove relative path of migration file in  sqapp/migrations-files.c        (added by sqtool)
	                                          or  sqapp/migrations-files-cxx.cpp  (added by sqtool-cxx)

	3. remove declaration of migration in    sqapp/migrations-declarations

	4. set element of migrations as NULL in  sqapp/migrations-elements

Example for point 4: edit  sqapp/migrations-elements  to delete migration.

```c
// Before editing
& createUsersTable_2021_10_12_000000,

// After editing
NULL,
```

## 5 Access Databasse

After completing the above steps, SqApp::storage should work. Now you can use [SqStorage](SqStorage.md) to access database.  
  
use C language

```c
	SqStorage *storage;

	storage = sqApp->storage;

	// use 'storage' to access database
	User *user;
	user = sq_storage_get(storage, "users", NULL, 3);
```

use C++ language

```c++
	Sq::Storage *storage;

	storage = sqApp->storage;

	// use 'storage' to access database
	User *user;
	user = storage->get("users", 3);
```

## 6 Multiple SqApp instance

For example, create two SqApps to synchronize the schema of SQLite and PostgreSQL.  
  
**Step 1**: Prepare two SqAppSetting for SQLite and PostgreSQL  
  
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

**Step 2**: Create two SqApp instance using the SqAppSetting defined above  
  
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

**Step 3**: run migrations using two SqApp  
  
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
