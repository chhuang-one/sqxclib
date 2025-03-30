[English](database-interface.md)

Sqdb 系列是用于操作数据库产品的基本结构。如果要支持新的数据库产品，请使用以下结构。

| 结构名称    | 描述                                   |
| ----------- | -------------------------------------- |
| Sqdb        | SQLite、MySQL 等数据库产品的基础结构。 |
| SqdbInfo    | 数据库产品的接口。                     |
| SqdbConfig  | 数据库产品的设置。                     |

假设我们要支持一个新的数据库产品 Xsql：

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

以下是 SqdbInfo 各字段说明。

```c++
struct SqdbInfo
{
	uintptr_t      size;       // Sqdb 实例大小
	SqdbProduct    product;    // 数据库产品代码

	struct {
		unsigned int has_boolean:1;      // 具有布尔数据类型
		unsigned int use_alter:1;        // 使用 "ALTER COLUMN" 更改列
		unsigned int use_modify:1;       // 使用 "MODIFY COLUMN" 更改列
	} column;

	// 用于数据库列和表标识符
	struct {
		char         identifier[2];      // SQLite 使用 "", MySQL 使用 ``, SQL Server 使用 []
	} quote;

	// 初始化 Sqdb 的派生结构
	void (*init)(Sqdb *db, SqdbConfig *config);
	// 终结 Sqdb 的派生结构
	void (*final)(Sqdb *db);

	// 打开数据库文件或建立与数据库服务器的连接
	int  (*open)(Sqdb *db, const char *name);
	// 关闭以前打开的文件或连接。
	int  (*close)(Sqdb *db);
	// 执行 SQL 语句
	int  (*exec)(Sqdb *db, const char *sql, Sqxc *xc, void *reserve);
	// 迁移架构。它将 'schemaNext' 的更改应用于 'schemaCurrent'
	int  (*migrate)(Sqdb *db, SqSchema *schemaCurrent, SqSchema *schemaNext);
};
```

为 Xsql 实现 SqdbInfo。

```c
// 为 SqdbInfo 声明函数
static void sqdb_xsql_init(SqdbXsql *sqdb, const SqdbConfigXsql *config);
static void sqdb_xsql_final(SqdbXsql *sqdb);
static int  sqdb_xsql_open(SqdbXsql *sqdb, const char *databaseName);
static int  sqdb_xsql_close(SqdbXsql *sqdb);
static int  sqdb_xsql_exec(SqdbXsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xsql_migrate(SqdbXsql *sqdb, SqSchema *schema, SqSchema *schemaNext);

// 实现 SqdbInfo，创建 Sqdb 实例时将使用 'sqdbInfoXsql'。
const SqdbInfo sqdbInfoXsql = {
	.size    = sizeof(SqdbXsql),
	.product = SQDB_PRODUCT_XSQL,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}     // ANSI-SQL 引用标识符是 ""
	},

	.init    = sqdb_xsql_init,
	.final   = sqdb_xsql_final,
	.open    = sqdb_xsql_open,
	.close   = sqdb_xsql_close,
	.exec    = sqdb_xsql_exec,
	.migrate = sqdb_xsql_migrate,
};

// 在这里实现 SqdbXsql 的函数

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
				// 根据 'table' 的数据执行 SQL 语句
			}
			else if (table->name == NULL) {
				// 删除表   DROP TABLE
				// 根据 'table' 的数据执行 SQL 语句
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// 重命名表 RENAME TABLE
				// 根据 'table' 的数据执行 SQL 语句
			}
			else {
				// 创建表   CREATE TABLE
				// 根据 'table' 的数据执行 SQL 语句
			}
		}
	}

	// 包含并应用来自 'schemaNext' 的更改
	sq_schema_update(schemaCurrent, schemaNext);
	schemaCurrent->version = schemaNext->version;
}
```

#### 4 使用自定义 Sqdb

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
