# SqApp

SqApp use configuration file (SqApp-config.h) to initialize database and do migrations. It used by user's application.  
Note: SqApp is in sqxcapp library.  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool is used by command-line program - **sqxctool** and **sqxcpptool**.  
  
Both **sqxctool** and **sqxcpptool** can generate migration and do migrate. They can help with the user's application that using SqApp library. The difference is that sqxctool generate C migration file and sqxcpptool generate C++ migration file.

## Configuration (SqApp-config.h)

### replace SqApp-config.h by other config file

user can define macro SQ_APP_CONFIG_FILE to replace SqApp-config.h when compiling.  
  
e.g. use workspace/myapp-config.h to replace default workspace/sqxcapp/SqApp-config.h  
if myapp-config.h place in C include directories...

```
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

OR use relative path of workspace/sqxcapp

```
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

### choose SQL product

User can use only one SQL products here (e.g. use MySQL)

```c++
// You can enable only one SQL products here

// #define DB_SQLITE      1
#define DB_MYSQL       1
```

### Database configuration values

```c++
// connection configuration values
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""

// common configuration values
#define DB_DATABASE    "sqxcapp-example"

// SQLite configuration values
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL
```

* Please make sure that your app and it's sqxctool use the same database file if you enable SQLite.

## Migrations

### use sqxctool (or sqxcpptool)

sqxctool (or sqxcpptool) use the same configuration values as library sqxcapp.  

```
sqxctool  make:migration  migration_name
```
* This command will:
1. generate migration file - workspace/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. append relative path of migration file in workspace/sqxcapp/migrations-files.c
3. append declaration of migration to workspace/sqxcapp/migrations-declarations
4. append element of migrations array to workspace/sqxcapp/migrations-elements

* If you use C++ to do migration, you can replace sqxctool by sqxcpptool. It will create workspace/database/migrations/yyyy_MM_dd_HHmmss_migration_name.cpp and append it to workspace/sqxcapp/migrations-files.cpp

#### create table by sqxctool (C language)

generate C migration file to create companies table

```
sqxctool  make:migration  create_companies_table
```

Above command will create file in workspace/database/migrations/yyyy_MM_dd_HHmmss_create_companies_table.c  
It is suggested that user define struct Company in workspace/sqxcapp/CStructs.h in this case.  
The file looks like below:

```c
/* migrations-files.c has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include <CStructs.h>        // define struct Company in CStructs.h
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

#### alter table by sqxcpptool (C++ language)

generate C++ migration file to alter companies table

```
sqxcpptool  make:migration  --table=companies  alter_companies_table
```

Above command will create file in workspace/database/migrations/yyyy_MM_dd_HHmmss_alter_companies_table.cpp  
The file looks like below:

```c++
/* This template file is used by sqxcpptool
// migrations-files.cpp has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include <CStructs.h>
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

#### migrate by sqxctool (or sqxcpptool)

Run all of your outstanding migrations

```
sqxctool  migrate
```

Rollback the last database migration

```
sqxctool  migrate:rollback
```

### migrate at runtime
When user migrate at runtime, column 'migrations.name' in database will be empty string because SqApp does NOT contain SqMigration.name string by default.  
Enable SQ_APP_HAS_MIGRATION_NAME in "migrations.h" to change default setting.  
  
**Run all of your outstanding migrations**  
  
use C functions

```c
	SqApp *sqapp = sq_app_new();

	// open database that defined in SqApp-config.h
	if (sq_app_open_database(sqapp, NULL) != SQCODE_OK)
		return EXIT_FAILURE;

	// if the database vesion is 0 (no migrations have been done)
	if (sq_app_make_schema(sqapp, 0) == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(sqapp, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

use C++ methods

```c++
	Sq::App *sqapp = new Sq::App;

	// open database that defined in SqApp-config.h
	if (sqapp->openDatabase(NULL) != SQCODE_OK)
		return EXIT_FAILURE;

	// if the database vesion is 0 (no migrations have been done)
	if (sqapp->makeSchema() == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sqapp->migrate() != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

**Rollback the last database migration**  
  
use C functions

```c
	int  step = 0;

	sq_app_rollback(app, step);
```

use C++ methods

```c++
	int  step = 0;

	sqapp->rollback(step);
```
