[English](SqApp.md)

# SqApp

SqApp 使用配置文件 (SqApp-config.h) 来初始化数据库并进行迁移。它由应用程序使用。  
注意: SqApp 在 sqxcapp 库中。  

	SqApp
	│
	└─── SqAppTool

# SqAppTool

SqAppTool 由命令行程序使用 - **sqxctool** 和 **sqxcpptool**。它使用与 SqApp 相同的配置值。  
  
**sqxctool** 和 **sqxcpptool** 都可以生成迁移并执行迁移。他们可以帮助使用 SqApp 库的用户应用程序。区别在于 sqxctool 生成 C 迁移文件，而 sqxcpptool 生成 C++ 迁移文件。

## 1 创建

SqApp 的每个实例都必须在创建时指定设置。SqAppSetting 包含 SqApp 所需的所有设置。  
SQ_APP_DEFAULT 是 SqAppSetting 的内置默认设置。用户可以通过编辑文件 (SqApp-config.h) 来更改它。  
  
使用 C 语言

```c
	// 'SQ_APP_DEFAULT' 具有用户应用程序的数据库设置和迁移数据。
	SqApp *sqapp = sq_app_new(SQ_APP_DEFAULT);
```

使用 C++ 语言

```c++
	// 'SQ_APP_DEFAULT' 具有用户应用程序的数据库设置和迁移数据。
	Sq::App *sqapp = new Sq::App(SQ_APP_DEFAULT);
```

## 2 默认配置

用户可以编辑 SqApp-config.h 来更改 SQ_APP_DEFAULT 使用的默认配置。

### 2.1 选择 SQL 产品

用户在这里只能使用一种 SQL 产品（例如使用 MySQL）

```c++
// 此处只能启用一种 SQL 产品

// #define DB_SQLITE      1
#define DB_MYSQL       1
// #define DB_POSTGRE     1
```

### 2.2 数据库配置值

DB_DATABASE 是 SqApp 将打开的数据库的默认名称。

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

* 如果您启用 SQLite，请确保您的应用程序及其 sqxctool 使用相同的数据库文件路径。

### 2.3 用其他配置文件替换 SqApp-config.h

用户可以在编译时定义宏 SQ_APP_CONFIG_FILE 来替换 SqApp-config.h。  
  
例如: 使用 工作区目录/myapp-config.h 替换默认的 工作区目录/sqxcapp/SqApp-config.h  
  
如果 myapp-config.h 放在 C 包含目录中...

```
gcc -DSQ_APP_CONFIG_FILE="<myapp-config.h>"
```

或使用工作空间/sqxcapp 的相对路径

```
gcc -DSQ_APP_CONFIG_FILE="\"../myapp-config.h\""
```

## 3 打开数据库

C 函数 sq_app_open_database()，C++ 方法 openDatabase() 可以打开指定名称的数据库。如果用户没有指定名称，它将使用默认名称打开数据库。  
  
使用 C 语言

```c
	// 打开在 SqApp-config.h 中定义的数据库
	if (sq_app_open_database(sqapp, NULL) != SQCODE_OK)
		return EXIT_FAILURE;
```

使用 C++ 语言

```c++
	// 打开在 SqApp-config.h 中定义的数据库
	if (sqapp->openDatabase(NULL) != SQCODE_OK)
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
	if (sq_app_make_schema(sqapp, version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

使用 C++ 语言

```c++
	int  version = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sqapp->makeSchema(version) == SQCODE_DB_SCHEMA_VERSION_0)
		return EXIT_FAILURE;
```

### 4.1 创建迁移文件

通过命令行程序生成 C 迁移文件

```
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

```
sqxctool  make:migration  create_companies_table
```

上面的命令将创建文件 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_create_companies_table.c  
在这种情况下，建议用户在 工作区目录/sqxcapp/CStructs.h 中定义结构 "Company"。  
该文件如下所示：

```c
/* migrations-files.c 包含以下标题。
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"        // 在 CStructs.h 中定义结构 "Company"
 */

// 运行迁移。
static void up_2021_12_12_180000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "companies", Company);
}

// 反转迁移。
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

#### 4.1.2 通过 sqxcpptool 更改表（C++ 语言）

例如: 生成 C++ 迁移文件以更改 "companies" 表

```
sqxcpptool  make:migration  --table=companies  alter_companies_table
```

上面的命令将在 工作区目录/database/migrations/yyyy_MM_dd_HHmmss_alter_companies_table.cpp 中创建文件  
该文件如下所示：

```c++
/* 该模板文件由 sqxcpptool 使用
// migrations-files.cpp 已包含以下头文件。
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"
 */

const SqMigration alter_companies_table_2021_12_26_191532 = {

	// 运行迁移。
//	.up = 
	[](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("companies");
	},

	// 反转迁移。
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

#### 4.1.3 通过 sqxctool（或 sqxcpptool）迁移

运行所有未完成的迁移

```
sqxctool  migrate
```

回滚上次数据库迁移

```
sqxctool  migrate:rollback
```

您可以通过向 rollback 命令提供 step 选项来回滚有限数量的迁移。

```
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
	if (sq_app_make_schema(sqapp, migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sq_app_migrate(sqapp, step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

使用 C++ 语言

```c++
	int  step = 0;
	int  migration_id = 0;

	// 如果数据库中的架构版本为 0 (未进行任何迁移)
	if (sqapp->makeSchema(migration_id) == SQCODE_DB_SCHEMA_VERSION_0) {
		// 运行在 ../database/migrations 中定义的迁移
		if (sqapp->migrate(step) != SQCODE_OK)
			return EXIT_FAILURE;
	}
```

#### 4.2.2 回滚上次数据库迁移

如果 'step' 为 0，sq_app_rollback() 将回滚最新的迁移操作。  
  
使用 C 语言

```c
	int  step = 0;

	sq_app_rollback(app, step);
```

使用 C++ 语言

```c++
	int  step = 0;

	sqapp->rollback(step);
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
& CreateUsersTable_2021_10_12_000000,

// 编辑后
NULL,
```

## 5 多个 SqApp 实例

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
