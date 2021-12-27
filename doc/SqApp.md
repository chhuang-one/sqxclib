# SqApp

SqApp use configuration file (SqApp-config.h) to initialize database and do migrations for user's application.

	SqApp
	│
	└─── SqAppTool

SqApp is used by user's application.
SqAppTool is used by command-line program - sqtool and sqtoolcpp.

## Configuration (SqApp-config.h)

### replace SqApp-config.h by other config file

user can define macro SQ_APP_CONFIG_FILE to replace SqApp-config.h when compiling.  
  
e.g. use workspace/myapp-config.h to replace default workspace/sqapp/SqApp-config.h  
if myapp-config.h place in C include directories...

```
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

OR use relative path of workspace/sqapp

```
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

### choose SQL product

User can use only one SQL products here (e.g. use MySQL)

```c++
// You can enable only one SQL products here

// #define DB_SQLITE
#define DB_MYSQL
```

### Database configuration values

```c++
// connection configuration values
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""

// common configuration values
#define DB_DATABASE    "sqapp-example"

// SQLite configuration values
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL
```

* Please make sure that your app and it's sqtool use the same database file if you enable SQLite.

## Migrations

### use sqtool (or sqtoolcpp)

sqtool (or sqtoolcpp) use the same configuration values as library sqapp.  

```
sqtool  make:migration  migration_name
```
* This command will:
1. generate migration file - workspace/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. append relative path of migration file in workspace/sqapp/migrations.c
3. append declaration of migration to workspace/sqapp/migrations-declarations
4. append element of migrations array to workspace/sqapp/migrations-elements

* If you use C++ to do migration, you can replace sqtool by sqtoolcpp. It will create yyyy_MM_dd_HHmmss_migration_name.cpp and append it to migrations.cpp

#### create table by sqtool (C language)

generate C migration file to create componies table

```
sqtool  make:migration  create_componies_table
```

Above command will create file in workspace/database/migrations/yyyy_MM_dd_HHmmss_create_componies_table.c  
The file looks like below:

```c
// Run the migrations.
static void up_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "componies", Componies);
}

// Reverse the migrations.
static void down_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	sq_schema_drop(schema, "componies");
}

const SqMigration create_componies_table_2021_12_12_180000 = {
	.up   = up_2021_12_12_180000,
	.down = down_2021_12_12_180000,

#ifdef SQ_APP_TOOL
	.name = "2021_12_12_180000_create_componies_table",
#endif
};
```

#### alter table by sqtoolcpp (C++ Language)

generate C++ migration file to alter componies table

```
sqtoolcpp  make:migration  --table=componies  alter_componies_table
```

Above command will create file in workspace/database/migrations/yyyy_MM_dd_HHmmss_alter_componies_table.cpp  
The file looks like below:

```c++
const SqMigration alter_componies_table_2021_12_26_191532 = {

	// Run the migrations.
	.up = [](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("componies");
	},

	// Reverse the migrations.
	.down = [](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("componies");
	},

#ifdef SQ_APP_TOOL
	.name = "2021_12_26_191532_alter_componies_table",
#endif
};

```

#### migrate by sqtool (or sqtoolcpp)

Run all of your outstanding migrations

```
sqtool  migrate
```

Rollback the last database migration

```
sqtool  migrate:rollback
```

#### migrate by C Language

Run all of your outstanding migrations

```c
	// if the database vesion is 0 (no migrations have been done)
	if (sq_app_make_schema(sqapp, 0) == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sq_app_migrate(sqapp, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

Rollback the last database migration

```c
	int  step = 0;

	sq_app_rollback(app, step);
```

#### migrate by C++ Language

Run all of your outstanding migrations

```c++
	// if the database vesion is 0 (no migrations have been done)
	if (sqapp->makeSchema() == SQCODE_DB_VERSION_0) {
		// run migrations that defined in ../database/migrations
		if (sqapp->migrate() != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

Rollback the last database migration

```c++
	int  step = 0;

	sqapp->rollback(step);
```
