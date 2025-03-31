[English](database-interface.md)

# 数据库接口

Sqdb 系列是用于操作数据库产品的基本结构。如果要支持新的数据库产品，请使用以下结构。

| 结构名称    | 描述                                   |
| ----------- | -------------------------------------- |
| Sqdb        | SQLite、MySQL 等数据库产品的基础结构。 |
| SqdbInfo    | 数据库产品的接口。                     |
| SqdbConfig  | 数据库产品的设置。                     |

# 支持新的数据库产品

假设要支持一个新的数据库产品 Xsql，需要完成以下步骤：

## 0 定义

在头文件中添加定义。

```c
// 为 Xsql 定义数据库产品代码。
#define  SQDB_PRODUCT_XSQL    (SQDB_PRODUCT_CUSTOM + 1)

// 为 Xsql 定义 SqdbInfo 接口名，全局变量 'sqdbInfoXsql' 将稍后定义。
#define  SQDB_INFO_XSQL       (&sqdbInfoXsql)
```

## 1 派生 SqdbConfig

使用宏 SQDB_CONFIG_MEMBERS 继承 SqdbConfig 成员变量。宏 SQDB_CONFIG_MEMBERS 将扩展为两个成员：

```c
	unsigned int    product;      // 保留。枚举 SqdbProduct 的值
	unsigned int    bit_field;    // 保留。config 的实例是常量还是动态的？
```

为 Xsql 创建 SqdbConfigXsql，它派生自 SqdbConfig。
派生结构必须符合 C++11 标准布局。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct SqdbConfigXsql    SqdbConfigXsql;

// 从 SqdbConfig 派生的配置数据结构
struct SqdbConfigXsql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. 继承成员变量

	int   xsql_setting;                    // <-- 2. 在派生结构中添加变量和非虚函数。
};
```

## 2 派生 Sqdb

使用宏 SQDB_MEMBERS 继承 Sqdb 成员变量。宏 SQDB_MEMBERS 将扩展为两个成员：

```c
	const SqdbInfo *info;       // 数据与功能接口
	int             version;    // 当前打开的数据库的架构版本
```

为 Xsql 创建 SqdbXsql，它派生自 Sqdb。
派生结构必须符合 C++11 标准布局。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct SqdbXsql          SqdbXsql;

// 源自 Sqdb 的结构
#ifdef __cplusplus
struct SqdbXsql : Sq::DbMethod             // <-- 1. 继承 C++ 成员函数 (方法)
#else
struct SqdbXsql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. 继承成员变量

	SqdbConfigXsql *config;                // <-- 3. 在派生结构中添加变量和非虚函数。
	int             variable;


#ifdef __cplusplus
	// 如果您使用 C++ 语言，请在此处定义 C++ 构造函数和析构函数。
	SqdbXsql(const SqdbConfigXsql *config) {
		// 调用 Sq::DbMethod::init()
		init(SQDB_INFO_XSQL, (SqdbConfig*)config);
		// 或调用 C 函数：
		// sqdb_init((Sqdb*)this, SQDB_INFO_XSQL, (SqdbConfig*)config);
	}
	SqdbXsql(const SqdbConfigXsql &config) {
		init(SQDB_INFO_XSQL, (SqdbConfig&)config);
	}
	~SqdbXsql() {
		// 调用 Sq::DbMethod::final()
		final();
		// 或调用 C 函数：
		// sqdb_final((Sqdb*)this);
	}
#endif
};
```

## 3 实现 SqdbInfo 接口

为 Xsql 实现 SqdbInfo 接口：

```c
// 声明 SqdbInfo 接口的函数
static void sqdb_xsql_init(SqdbXsql *sqdb, const SqdbConfigXsql *config);
static void sqdb_xsql_final(SqdbXsql *sqdb);
static int  sqdb_xsql_open(SqdbXsql *sqdb, const char *databaseName);
static int  sqdb_xsql_close(SqdbXsql *sqdb);
static int  sqdb_xsql_exec(SqdbXsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xsql_migrate(SqdbXsql *sqdb, SqSchema *schema, SqSchema *schemaNext);

// 实现 SqdbInfo 接口
// 创建 Sqdb 实例时将使用全局变量 'sqdbInfoXsql'。
const SqdbInfo sqdbInfoXsql = {
	.size    = sizeof(SqdbXsql),         // Sqdb 实例大小
	.product = SQDB_PRODUCT_XSQL,        // 数据库产品代码

	.column  = {
		.has_boolean = 1,                // 具有布尔数据类型
		.use_alter  = 1,                 // 使用 "ALTER COLUMN" 更改列
		.use_modify = 0,                 // 使用 "MODIFY COLUMN" 更改列
	},

	// 用于数据库列和表标识符
	.quote = {
		.identifier = {'"', '"'}         // ANSI-SQL 引用标识符是 ""
		                                 // SQLite 使用 "", MySQL 使用 ``, SQL Server 使用 []
	},

	// 初始化 Sqdb 的派生结构
	.init    = sqdb_xsql_init,
	// 终结 Sqdb 的派生结构
	.final   = sqdb_xsql_final,

	// 打开数据库文件或建立与数据库服务器的连接
	.open    = sqdb_xsql_open,
	// 关闭以前打开的文件或连接。
	.close   = sqdb_xsql_close,
	// 执行 SQL 语句
	.exec    = sqdb_xsql_exec,
	// 迁移架构。它将 'schemaNext' 的更改应用于 'schemaCurrent'
	.migrate = sqdb_xsql_migrate,
};
```

#### 3.1 init / final

调用 SqdbInfo::init()  时，它应该初始化 Sqdb 实例。  
调用 SqdbInfo::final() 时，它应该终结   Sqdb 实例。

```c
static void sqdb_xsql_init(SqdbXsql *sqdb, const SqdbConfigXsql *config)
{
	// 初始化 SqdbXsql 实例
	sqdb->version  = 0;
	sqdb->config   = config;
}

static void sqdb_xsql_final(SqdbXsql *sqdb)
{
	// 终结 SqdbXsql 实例
}
```

#### 3.2 open / close

调用 SqdbInfo::open()  时，它应该执行连接、登录、打开文件或数据库，并从 SQL 表获取架构版本。  
调用 SqdbInfo::close() 时，它应该执行断开连接、登出、关闭文件或数据库。

```c
static int  sqdb_xsql_open(SqdbXsql *sqdb, const char *databaseName)
{
	// 打开数据库并获取它的架构版本
	sqdb->version = schemaVersion;

	return SQCODE_OK;
}

static int  sqdb_xsql_close(SqdbXsql *sqdb)
{
	// 关闭数据库
	return SQCODE_OK;
}
```

#### 3.3 exec

如果参数 'xc' 为 NULL，则直接执行 SQL 语句 'sql'，否则将 SQL 语句的结果发送给 'xc' 进行转换。

```c
static int  sqdb_xsql_exec(SqdbXsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
{
	if (xc == NULL) {
		// 执行查询
	}
	else {
		// 根据 SQL 语句中的第一个字符确定命令。
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
			// 从 Xsql 获取行并将它们发送到 'xc'
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// 将插入的行 id 设置为 SqxcSql::id
			((SqxcSql*)xc)->id = inserted_id;
			break;

		case 'U':    // UPDATE
		case 'u':    // update
			// 设置 SqxcSql::changes 为更改行数
			((SqxcSql*)xc)->changes = number_of_rows_changed;
			break;

		default:
			// 执行查询
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

SqdbInfo::migrate() 使用架构的版本来决定是否迁移。它有 2 个 schema 参数，第一个 'schemaCurrent' 参数是当前版本的架构，第二个 'schemaNext' 参数是下一个版本的架构。'schemaNext' 的更改将应用​​于 'schemaCurrent'。  
迁移后您无法重用 'schemaNext'，因为此功能可能会将数据从 'schemaNext' 移动到 'schemaCurrent'。  
  
请不要在第一个 'schemaCurrent' 参数中直接更改、重命名和删除表，而是在第二个 'schemaNext' 参数中执行这些操作然后运行 migrate() 将更改应用于 'schemaCurrent'。  
  
要通知数据库实例迁移已完成，请调用 SqdbInfo::migrate() 并在最后一个参数中传入 NULL。这将清除未使用的数据、对表和列进行排序，并将当前架构同步到数据库（主要用于 SQLite）。

```c
static int  sqdb_xsql_migrate(SqdbXsql *db, SqSchema *schemaCurrent, SqSchema *schemaNext)
{
	// 如果 'schemaNext' 为 NULL，则更新并排序 'schemaCurrent'，并
	// 将 'schemaCurrent' 同步到数据库（主要用于 SQLite）。
	if (schemaNext == NULL) {
		// 按名称对表和列进行排序
		sq_schema_sort_table_column(schema);
		return SQCODE_OK;
	}

	if (db->version < schemaNext->version) {
		// 通过 'schemaNext' 进行迁移
		for (unsigned int index = 0;  index < schemaNext->type->n_entry;  index++) {
			SqTable *table = (SqTable*)schemaNext->type->entry[index];
			if (table->bit_field & SQB_CHANGED) {
				// 更改表   ALTER TABLE
				// 根据 'table' 中的记录运行迁移。
				// 表相关记录在 'table' 中，列相关记录在 'table->type->entry' 中。
			}
			else if (table->name == NULL) {
				// 删除表   DROP TABLE
				// 根据 'table' 中的记录运行迁移。
				// 表相关记录在 'table' 中，列相关记录在 'table->type->entry' 中。
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// 重命名表 RENAME TABLE
				// 根据 'table' 中的记录运行迁移。
				// 表相关记录在 'table' 中，列相关记录在 'table->type->entry' 中。
			}
			else {
				// 创建表   CREATE TABLE
				// 根据 'table' 中的记录运行迁移。
				// 表相关记录在 'table' 中，列相关记录在 'table->type->entry' 中。
			}
		}
	}

	// 包含并应用来自 'schemaNext' 的更改
	sq_schema_update(schemaCurrent, schemaNext);
	schemaCurrent->version = schemaNext->version;
}
```

## 4 使用自定义 Sqdb

使用 C 语言

```c++
	SqdbConfigXsql  config = {
		.xsql_setting = 0,
	};
	Sqdb           *db;
	SqStorage      *storage;

	// 使用配置数据创建自定义 Sqdb 对象
	db = sqdb_new(SQDB_INFO_XSQL, (SqdbConfig*) &config);

	// 创建使用新 Sqdb 的存储对象
	storage = sq_storage_new(db);
```

使用 C++ 语言

```c++
	SqdbConfigXsql  config = {
		.xsql_setting = 0,
	};
	SqdbXsql       *db;
	Sq::Storage    *storage;

	// 使用配置数据创建自定义 Sqdb 对象
	db = new SqdbXsql(config);

	// 创建使用新 Sqdb 的存储对象
	storage = new Sq::Storage(db);
```
