[English](SqApp.md)

# SqApp

SqApp 它基于 sqxclib 开发，支持单独的迁移文件进行迁移，并使用配置文件 (SqApp-config.h) 初始化数据库。  
  
注意: SqApp 在 sqxcapp 库的 SqApp.h 中声明。  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool 由命令行程序使用 - **sqtool** 和 **sqtool-cxx**。它使用与 SqApp 相同的配置值。  
  
**sqtool** 和 **sqtool-cxx** 都可以生成单独的迁移文件并使用它们进行迁移。他们可以帮助使用 SqApp 库的应用程序。区别在于 sqtool 生成 C 迁移文件，而 sqtool-cxx 生成 C++ 迁移文件。

## 1 配置

SqApp-config.h 是 SqApp 的默认配置文件。您还可以创建 SqAppSetting 实例并在其中指定 SqApp 的设置。

#### 1.1 配置文件

SqApp-config.h 是 SqApp 的配置文件，它包含连接、身份验证等的默认设置。  
  
SqApp-config.h 的**第一部分**可以选择数据库产品：

```c++
// 此处只能启用一种数据库产品

// #define DB_SQLITE      1
#define DB_MYSQL       1
// #define DB_POSTGRE     1
```

SqApp-config.h 的**第二部分**是数据库配置值：

	DB_NO_MIGRATION 如果不需要将迁移同步到数据库，则将其设置为 1。
	DB_DATABASE     是 SqApp 将打开的数据库的默认名称。
	DB_HOST         是连接的设置。
	DB_PORT         是连接的设置。
	DB_USERNAME     是认证的设置。
	DB_PASSWORD     是认证的设置。
	DB_FOLDER       是文件夹名称。SQLite 在此文件夹中创建或访问数据库文件。
	DB_EXTENSION    是 SQLite 数据库文件的扩展名。

* 如果您启用 SQLite，请确保您的应用程序及其 sqtool 使用相同的数据库文件路径。

```c++
// 通用配置值
#define DB_NO_MIGRATION    0
#define DB_DATABASE    "sqxcapp-example"

// 连接配置值
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""

// --- SQLite ---
// 文件配置值
#define DB_FOLDER      NULL
#define DB_EXTENSION   NULL
```

#### 1.2 用其他配置文件替换 SqApp-config.h

用户可以在编译时定义宏 SQ_APP_CONFIG_FILE 来替换 SqApp-config.h。  
  
例如: 使用 工作区目录/myapp-config.h 替换默认的 工作区目录/sqapp/SqApp-config.h。  
  
如果 myapp-config.h 位于 C 包含目录中，请使用尖括号括住文件名。

```console
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

如果配置文件位于 工作区目录/sqapp 的相对路径中，则将文件名括在双引号中。

```console
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

#### 1.3 SqAppSetting

SqAppSetting 包含 SqApp 所需的所有设置。  
SQ_APP_DEFAULT 是 SqAppSetting 的一个实例，它使用 SqApp-config.h 中的值来设置其成员值。
因此开发者可以编辑 SqApp-config.h 来更改 SQ_APP_DEFAULT 中的值。  
  
例如: 使用默认设置创建 SqAppSetting。  
  
下面的 SQ_APP_DEFAULT_XXXX 是 SQ_APP_DEFAULT 中的设置。

```
	SqAppSetting  settings;

	settings.db_info      = SQ_APP_DEFAULT_DB_INFO;
	settings.db_config    = SQ_APP_DEFAULT_DB_CONFIG;
	settings.db_database  = SQ_APP_DEFAULT_DATABASE;
	settings.migrations   = SQ_APP_DEFAULT_MIGRATIONS
	settings.n_migrations = SQ_APP_DEFAULT_N_MIGRATIONS;
```

## 2 创建 SqApp

SqApp 的每个实例都必须在创建时指定 SqAppSetting。  
SQ_APP_DEFAULT 是上面提到的默认设置。  
  
例如: 使用默认设置创建 SqApp  
  
使用 C 语言

```c
#include <sqxc/sqxclib.h>      // sqxclib.h 包含 sqxc/app 头文件


	SqApp *sqApp;

	// 'SQ_APP_DEFAULT' 具有应用程序的数据库设置和迁移数据。
	sqApp = sq_app_new(SQ_APP_DEFAULT);
```

使用 C++ 语言

```c++
#include <sqxc/sqxclib.h>      // sqxclib.h 包含 sqxc/app 头文件


	Sq::App *sqApp;

	// 'SQ_APP_DEFAULT' 具有应用程序的数据库设置和迁移数据。
	sqApp = new Sq::App(SQ_APP_DEFAULT);
```

## 3 打开数据库

C 函数 sq_app_open_database()，C++ 方法 openDatabase() 可以打开指定名称的数据库。如果没有指定名称，它将使用 SqApp-config.h 中定义的默认名称来打开数据库。  
  
使用 C 语言

```c
	// 如果 databaseName = NULL，则打开 SqApp-config.h 中定义的数据库名称
	char   *databaseName = "web";

	if (sq_app_open_database(sqApp, databaseName) != SQCODE_OK)
		return EXIT_FAILURE;
```

使用 C++ 语言

```c++
	// 如果 databaseName = NULL，则打开 SqApp-config.h 中定义的数据库名称
	char   *databaseName = "web";

	if (sqApp->openDatabase(databaseName) != SQCODE_OK)
		return EXIT_FAILURE;
```

打开数据库后，您可以从 SqApp::db.version 获取数据库中当前的架构版本。

```c
	int  schemaVersion;

	schemaVersion = sqApp->db->version;
```

## 4 迁移

C 函数 sq_app_make_schema()，C++ 方法 makeSchema() 可以使用迁移文件生成架构。  
开发者可以指定要生成的架构版本。如果将版本指定为 0，程序将使用默认版本（数据库中架构的版本）。

	C 函数 sq_app_make_schema()、C++ makeSchema() 的返回值：

	SQCODE_DB_SCHEMA_VERSION_0 : 如果数据库中的架构版本为 0（未完成任何迁移）。
	SQCODE_DB_WRONG_MIGRATIONS : 如果这些迁移不是此数据库的。

使用 C 语言

```c
	int  version = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sq_app_make_schema(sqApp, version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

使用 C++ 语言

```c++
	int  version = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sqApp->makeSchema(version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

### 4.1 创建迁移文件

C 迁移文件由 sqtool 创建，C++ 迁移文件由 sqtool-cxx 创建。每个迁移文件都有一个 SqMigration 实例用于迁移。

	要创建新的数据库表：

	1. 在头文件 sqapp/CStructs.h 中为数据库表 "newbies" 定义结构 'Newbie'。

	2. 在控制台中运行以下命令以生成迁移文件：
	   $ sqtool      make:migration  create_newbies_table  (生成 C   迁移文件)
	   $ sqtool-cxx  make:migration  create_newbies_table  (生成 C++ 迁移文件)

	   该命令将：
	   将迁移声明附加到       sqapp/migrations-declarations
	   将迁移数组的元素附加到 sqapp/migrations-elements

	   将迁移文件的相对路径通过 sqtool     附加到 sqapp/migrations-files.c
	                     或通过 sqtool-cxx 附加到 sqapp/migrations-files-cxx.cpp

	3. 编辑生成的迁移文件 yyyy_MM_dd_HHmmss_create_newbies_table.c    (由 sqtool     生成)
	                   或 yyyy_MM_dd_HHmmss_create_newbies_table.cpp  (由 sqtool-cxx 生成)
	   位于文件夹 database/migrations/

最后，您必须在定义表后重新编译迁移代码。

#### 4.1.0 迁移文件

在 database/migrations 目录中的每个迁移文件都定义了一个 SqMigration 实例。SqMigration::name 是迁移的描述。  
如果 SqMigration::name 不包含字符串，数据库中的列 'migrations.name' 将为空字符串。
  
您可以在 sqapp/migrations.h 中将 SQ_APP_HAS_MIGRATION_NAME 设置为 0，这样 SqApp 将不会在 SqMigration::name 中包含字符串。
这可以减小应用程序二进制文件的大小。  

#### 4.1.1 使用 sqtool 建表 (C 语言)

例如: 生成 C 迁移文件以创建 "companies" 表

```console
sqtool  make:migration  create_companies_table
```

上述命令将在 工作区目录/database/migrations 中创建文件 yyyy_MM_dd_HHmmss_create_companies_table.c。  
在这种情况下，建议在 工作区目录/sqapp/CStructs.h 中定义结构 'Company'。  
该文件如下所示：

```c
/* 此模板文件由 sqtool 使用
 * 请在 工作区/sqapp/CStructs.h 中定义结构 'Company'
 *
 * 通常，如果您使用 sqtool 制作迁移文件，则此文件应包含在 migrations-files.c 中。
 * migrations-files.c 已包含以下标头。
 * #include <sqxc/SqStorage.h>
 * #include <sqxc/app/SqMigration.h>
 * #include "CStructs.h"
 */


// 运行迁移。
static void up_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "companies", Company);

	column = sq_table_add_integer(table, "id", offsetof(Company, id));
	sq_column_primary(column);
}

// 反向迁移。
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

#### 4.1.2 通过 sqtool-cxx 更改表（C++ 语言）

要更改表，您必须使用 sqtool 或 sqtool-cxx 的 --table 选项指定表名。  
  
例如: 生成 C++ 迁移文件以更改 "companies" 表

```console
sqtool-cxx  make:migration  --table=companies  alter_companies_table
```

上述命令将在 工作区目录/database/migrations 中创建文件 yyyy_MM_dd_HHmmss_alter_companies_table.cpp。  
该文件如下所示：

```c++
/* 此模板文件由 sqtool-cxx 使用
 *
 * 如果您使用 sqtool-cxx 制作迁移文件，则通常应将此文件包含在 migrations-files-cxx.cpp 中。
 * migrations-files-cxx.cpp 已包含以下标头。
 * #include <sqxc/SqStorage.h>
 * #include <sqxc/app/SqMigration.h>
 * #include "CStructs.h"
 */


const SqMigration alterCompaniesTable_2021_12_26_191532 = {

	// 运行迁移。
//	.up =
	[](SqSchema *schema, SqStorage *storage) {
		Sq::Table  *table;

		table = schema->alter("companies");

		// 修改表中的列
	},

	// 反向迁移。
//	.down =
	[](SqSchema *schema, SqStorage *storage) {
		Sq::Table  *table;

		table = schema->alter("companies");

		// 修改表中的列
	},

#if SQ_APP_HAS_MIGRATION_NAME
//	.name =
	"2021_12_26_191532_alter_companies_table",
#endif
};

```

#### 4.1.3 通过 sqtool（或 sqtool-cxx）迁移

运行所有未完成的迁移

```console
sqtool  migrate
```

回滚最后一批迁移

```console
sqtool  migrate:rollback
```

您可以通过向 rollback 命令提供 step 选项来回滚有限数量的迁移。

```console
sqtool  migrate:rollback  --step=5
```

### 4.2 在运行时迁移

开发者可以使用 migrate() 和 rollback() 在运行时执行迁移。

#### 4.2.1 运行所有未完成的迁移

sq_app_migrate() 的 'step' 参数如果为 0，将运行所有未完成的迁移。  
  
使用 C 语言

```c
	int  step = 0;
	int  migration_id = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sq_app_make_schema(sqApp, migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sq_app_migrate(sqApp, step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

使用 C++ 语言

```c++
	int  step = 0;
	int  migration_id = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sqApp->makeSchema(migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sqApp->migrate(step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

#### 4.2.2 回滚迁移

如果 'step' 为 0，sq_app_rollback() 将回滚最后一批迁移。  
  
使用 C 语言

```c
	int  step = 0;

	sq_app_rollback(sqApp, step);
```

使用 C++ 语言

```c++
	int  step = 0;

	sqApp->rollback(step);
```

#### 4.3 删除迁移

要删除 sqtool 创建的迁移，您必须：

	1. 删除迁移文件 yyyy_MM_dd_HHmmss_migration_name.c    (由 sqtool     生成)
	             或 yyyy_MM_dd_HHmmss_migration_name.cpp  (由 sqtool-cxx 生成)
	   位于文件夹 database/migrations/

	2. 移除 sqapp/migrations-files.c   中迁移文件的相对路径      (由 sqtool     添加)
	     或 sqapp/migrations-files-cxx.cpp 中迁移文件的相对路径  (由 sqtool-cxx 添加)

	3. 移除 sqapp/migrations-declarations 中的迁移声明

	4. 在   sqapp/migrations-elements 中将迁移元素设置为 NULL

第 4 点的示例： 编辑 sqapp/migrations-elements 以删除迁移

```c
// 编辑前
& createUsersTable_2021_10_12_000000,

// 编辑后
NULL,
```

## 5 访问数据库

完成上述步骤后，SqApp::storage 就可以工作了。现在您可以使用 [SqStorage](SqStorage.cn.md) 访问数据库。  
  
使用 C 语言

```c
	SqStorage *storage;

	storage = sqApp->storage;

	// 使用 'storage' 访问数据库
	User *user;
	user = sq_storage_get(storage, "users", NULL, 3);
```

使用 C++ 语言

```c++
	Sq::Storage *storage;

	storage = sqApp->storage;

	// 使用 'storage' 访问数据库
	User *user;
	user = storage->get("users", 3);
```

## 6 多个 SqApp 实例

例如，创建两个 SqApp 来同步 SQLite 和 PostgreSQL 的架构。  
  
**步骤 1**： 为 SQLite 和 PostgreSQL 准备两个 SqAppSetting  
  
SQ_APP_DEFAULT_xxx 系列是 'SQ_APP_DEFAULT' 中的默认设置。  

| 名称                        | 描述            |
| ----------------------------| --------------- |
| SQ_APP_DEFAULT_DATABASE     | 默认数据库名称  |
| SQ_APP_DEFAULT_MIGRATIONS   | 默认迁移数组    |
| SQ_APP_DEFAULT_N_MIGRATIONS | 默认迁移总数    |
  
使用 C 语言

```c
SqdbConfigSqlite  configSQLite = {0};        // 省略内容

SqAppSetting  forSQLite = {
	SQDB_INFO_SQLITE,                  // .db_info
	(SqdbConfig*) &configSQLite,       // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};


SqdbConfigPostgre configPostgreSQL = {0};    // 省略内容

SqAppSetting  forPostgreSQL = {
	SQDB_INFO_POSTGRE,                 // .db_info
	(SqdbConfig*) &configPostgreSQL,   // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};
```

使用 C++ 语言

```c++
Sq::DbConfigSqlite  configSQLite = {0};      // 省略内容

Sq::AppSetting  forSQLite = {
	SQDB_INFO_SQLITE,                  // .db_info
	(SqdbConfig*) &configSQLite,       // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};


Sq::DbConfigPostgre configPostgreSQL = {0};  // 省略内容

Sq::AppSetting  forPostgreSQL = {
	SQDB_INFO_POSTGRE,                 // .db_info
	(SqdbConfig*) &configPostgreSQL,   // .db_config
	SQ_APP_DEFAULT_DATABASE,           // .db_database
	SQ_APP_DEFAULT_MIGRATIONS,         // .migrations
	SQ_APP_DEFAULT_N_MIGRATIONS,       // .n_migrations
};
```

**步骤 2**: 使用上面定义的 SqAppSetting 创建两个 SqApp 实例  
  
使用 C 语言

```c
	SqApp *appSQLite = sq_app_new(&forSQLite);

	SqApp *appPostgreSQL = sq_app_new(&forPostgreSQL);
```

使用 C++ 语言

```c++
	Sq::App *appSQLite = new Sq::App(forSQLite);

	Sq::App *appPostgreSQL = new Sq::App(forPostgreSQL);
```

**步骤 3**: 使用两个 SqApp 运行迁移  
  
使用 C 语言

```c
	if (sq_app_make_schema(appSQLite, 0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sq_app_migrate(appSQLite, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	if (sq_app_make_schema(appPostgreSQL, 0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sq_app_migrate(appPostgreSQL, 0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

使用 C++ 语言

```c++
	if (appSQLite->makeSchema(0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (appSQLite->migrate(0) != SQCODE_OK)
			return EXIT_FAILURE;
	}

	if (appPostgreSQL->makeSchema(0) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (appPostgreSQL->migrate(0) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```
