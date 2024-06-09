[English](SqApp.md)

# SqApp

SqApp 它基于 sqxclib 开发，支持单独的迁移文件进行迁移，并使用配置文件 (SqApp-config.h) 初始化数据库。  
  
注意: SqApp 在 sqxcapp 库的 SqApp.h 中声明。  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool 由命令行程序使用 - **sqxctool** 和 **sqxcpptool**。它使用与 SqApp 相同的配置值。  
  
**sqxctool** 和 **sqxcpptool** 都可以生成单独的迁移文件并使用它们进行迁移。他们可以帮助使用 SqApp 库的用户应用程序。区别在于 sqxctool 生成 C 迁移文件，而 sqxcpptool 生成 C++ 迁移文件。

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
  
DB_DATABASE  是 SqApp 将打开的数据库的默认名称。  
DB_HOST      是连接的设置。  
DB_PORT      是连接的设置。  
DB_USERNAME  是认证的设置。  
DB_PASSWORD  是认证的设置。  
DB_FOLDER    是文件夹名称。SQLite 在此文件夹中创建或访问数据库文件。  
DB_EXTENSION 是 SQLite 数据库文件的扩展名。  

* 如果您启用 SQLite，请确保您的应用程序及其 sqxctool 使用相同的数据库文件路径。

```c++
// 通用配置值
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
  
例如: 使用 工作区目录/myapp-config.h 替换默认的 工作区目录/sqxcapp/SqApp-config.h。  
  
如果 myapp-config.h 放在 C 包含目录中...

```console
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

或使用 工作区目录/sqxcapp 的相对路径

```console
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

#### 1.3 SqAppSetting

SqAppSetting 包含 SqApp 所需的所有设置。  
SQ_APP_DEFAULT 是 SqAppSetting 的一个实例，它使用 SqApp-config.h 中的值来设置其成员值。
因此用户可以编辑 SqApp-config.h 来更改 SQ_APP_DEFAULT 中的值。  
  
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
#include <SqApp.h>    // sqxclib.h 不包含 sqxcapp 库


	SqApp *sqApp;

	// 'SQ_APP_DEFAULT' 具有用户应用程序的数据库设置和迁移数据。
	sqApp = sq_app_new(SQ_APP_DEFAULT);
```

使用 C++ 语言

```c++
#include <SqApp.h>    // sqxclib.h 不包含 sqxcapp 库


	Sq::App *sqApp;

	// 'SQ_APP_DEFAULT' 具有用户应用程序的数据库设置和迁移数据。
	sqApp = new Sq::App(SQ_APP_DEFAULT);
```

## 3 打开数据库

C 函数 sq_app_open_database()，C++ 方法 openDatabase() 可以打开指定名称的数据库。如果用户没有指定名称，它将使用默认名称打开数据库。  
  
使用 C 语言

```c
	// 打开在 SqApp-config.h 中定义的数据库
	if (sq_app_open_database(sqApp, NULL) != SQCODE_OK)
		return EXIT_FAILURE;
```

使用 C++ 语言

```c++
	// 打开在 SqApp-config.h 中定义的数据库
	if (sqApp->openDatabase(NULL) != SQCODE_OK)
		return EXIT_FAILURE;
```

## 4 迁移

C 函数 sq_app_make_schema()，C++ 方法 makeSchema() 可以使用迁移文件生成架构。  
用户可以指定要生成的架构版本。如果用户将版本指定为 0，程序将使用默认版本（数据库中架构的版本）。  
  
函数 sq_app_make_schema() 的返回值：  
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

通过命令行程序生成 C 迁移文件

```console
sqxctool  make:migration  migration_name
```

该命令将：
1. 生成迁移文件 - 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. 将迁移文件的相对路径附加到 工作区目录/sqxcapp/migrations-files.c
3. 将迁移声明附加到 工作区目录/sqxcapp/migrations-declarations
4. 将迁移数组的元素附加到 工作区目录/sqxcapp/migrations-elements

如果使用 C++ 进行迁移，可以将 sqxctool 替换为 sqxcpptool。不同的是
**sqxcpptool** 可以生成 C++ 迁移文件并将路径附加到 工作区目录/sqxcapp/migrations-files.cpp  
  
最后，您必须在定义表后重新编译迁移代码。

#### 4.1.1 sqxctool 建表 (C 语言)

例如: 生成 C 迁移文件以创建 "companies" 表

```console
sqxctool  make:migration  create_companies_table
```

上面的命令将创建文件 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_create_companies_table.c  
在这种情况下，建议用户在 工作区目录/sqxcapp/CStructs.h 中定义结构 'Company'。  
该文件如下所示：

```c
/* 此模板文件由 sqxctool 使用
 * 请在 工作区/sqxcapp/CStructs.h 中定义结构 'Company'
 *
 * 通常，如果您使用 sqxctool 制作迁移文件，则此文件应包含在 migrations-files.c 中。
 * migrations-files.c 已包含以下标头。
 * #include <SqStorage.h>
 * #include <SqMigration.h>
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

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
	.name = "2021_12_12_180000_create_companies_table",
#endif
};
```

#### 4.1.2 通过 sqxcpptool 更改表（C++ 语言）

例如: 生成 C++ 迁移文件以更改 "companies" 表

```console
sqxcpptool  make:migration  --table=companies  alter_companies_table
```

上面的命令将在 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_alter_companies_table.cpp 中创建文件  
该文件如下所示：

```c++
/* 此模板文件由 sqxcpptool 使用
 *
 * 如果您使用 sqxcpptool 制作迁移文件，则通常应将此文件包含在 migrations-files.cpp 中。
 * migrations-files.cpp 已包含以下标头。
 * #include <SqStorage.h>
 * #include <SqMigration.h>
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

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
//	.name =
	"2021_12_26_191532_alter_companies_table",
#endif
};

```

#### 4.1.3 通过 sqxctool（或 sqxcpptool）迁移

运行所有未完成的迁移

```console
sqxctool  migrate
```

回滚最后一批迁移

```console
sqxctool  migrate:rollback
```

您可以通过向 rollback 命令提供 step 选项来回滚有限数量的迁移。

```console
sqxctool  migrate:rollback --step=5
```

### 4.2 在运行时迁移

因为 SqApp 默认不包含 SqMigration.name 字符串，所以用户在运行时迁移时，数据库中的 'migrations.name' 列将为空字符串。  
在 "migrations.h" 中启用 SQ_APP_HAS_MIGRATION_NAME 以更改默认设置。  

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

1. 删除迁移文件 - 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_migration_name.c
2. 移除 工作区目录/sqxcapp/migrations-files.c 中迁移文件的相对路径
3. 移除 工作区目录/sqxcapp/migrations-declarations 中的迁移声明
4. 在 工作区目录/sqxcapp/migrations-elements 中将迁移元素设置为 NULL
  
第 2 点特别说明： C++ 用户必须删除 工作区目录/sqxcapp/migrations-files.cpp 中 C++ 迁移文件的相对路  
  
第 4 点的示例： 编辑 工作区目录/sqxcapp/migrations-elements 以删除迁移

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
  
**第一步：** 为 SQLite 和 PostgreSQL 准备两个 SqAppSetting  
  
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

**第 2 步：** 使用其设置创建两个 SqApp  
  
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

**Step 3:** 运行迁移  
  
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
