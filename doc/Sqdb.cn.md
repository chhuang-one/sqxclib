[English](Sqdb.md)

# Sqdb
Sqdb 是数据库产品（SQLite、MySQL...等）的基础结构。  

| 派生结构      | 数据库产品 | 源代码文件     |
| ------------- | ---------- | -------------- |
| SqdbSqlite    | SQLite     | SqdbSqlite.c   |
| SqdbMysql     | MySQL      | SqdbMysql.c    |
| SqdbPostgre   | PostgreSQL | SqdbPostgre.c  |

```c
struct Sqdb
{
	// 您可以使用 SQDB_MEMBERS 定义以下成员
	const SqdbInfo *info;       // 数据与功能接口
	int             version;    // SQL 数据库中的架构 (schema) 版本
};
```

# SqdbInfo

SqdbInfo 是数据库产品的接口。

```c
struct SqdbInfo
{
	uintptr_t      size;       // Sqdb 实例大小
	SqdbProduct    product;    // SQL  产品代码

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
	// 迁移架构。它将 'schema_next' 的更改应用于 'schema_current'
	int  (*migrate)(Sqdb *db, SqSchema *schema_current, SqSchema *schema_next);
};
```

# SqdbConfig

SqdbConfig 是 SQL 产品的设置

```c
struct SqdbConfig
{
	// 你可以使用 SQDB_CONFIG_MEMBERS 来定义下面的成员
	unsigned int    product;
	unsigned int    bit_field;    // 保留。config 的实例是常量还是动态的？
};
```

## 打开和关闭数据库

sqdb_open() 将在打开数据库时获取当前架构版本号。  
* SQLite 用户可以在 SqdbConfigSqlite 中设置 '文件夹' 和 '扩展名'，这些会影响数据库文件名和路径。

```c
	SqdbConfigSqlite config = { .folder = "/home/dir", .extension = "db" };
	Sqdb  *db;

	// 使用 'config' 创建 SqdbSqlite
	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);

	// 打开数据库文件 - "/home/dir/local-base.db"
	sqdb_open(db, "local-base");

	// 关闭数据库
	sqdb_close(db);
```

## 迁移

sqdb_migrate() 使用架构的版本来决定是否迁移。它将 'schema_next' 的更改应用于 'schema_current'。  
此函数可能会将数据从 'schema_next' 移动到 'schema_current'，迁移后您不能重用 'schema_next'。

使用 C 函数

```c
	// 将 'schema_next' 的更改应用于 'schema_current'
	sqdb_migrate(db, schema_current, schema_next);

	// 将 'schema_current' 同步到数据库并更新 'schema_current'
	// 这主要由 SQLite 使用
	sqdb_migrate(db, schema_current, NULL);
```

使用 C++ 方法

```c++
	// 将 'schema_next' 的更改应用于 'schema_current'
	db->migrate(schema_current, schema_next);

	// 将 'schema_current' 同步到数据库并更新 'schema_current'
	// 这主要由 SQLite 使用
	db->migrate(schema_current, NULL);
```

## 从 SQL 查询中获取结果

用户可以使用 sqdb_exec() 执行查询并通过 [Sqxc](Sqxc.cn.md) 元素获取结果。  
如果要解析对象或数组并重用 [Sqxc](Sqxc.cn.md) 元素：
1. 在 sqdb_exec() 之前调用 sqxc_ready()。
2. 在 sqdb_exec() 之后调用 sqxc_finish()。

#### 获取一个整数值

```c
	SqxcValue *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	int   integer = -1;
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_INT;
	xc_input->instance  = &integer;

	code = sqdb_exec(db, "SELECT max(id) FROM migrations", (Sqxc*)xc_input, NULL);

	if (code == SQCODE_OK)
		return integer;
```

#### 从 "migrations" 表中获取一行

```c
	SqxcValue        *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqMigrationTable *mtable = calloc(1, sizeof(SqMigrationTable));
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = mtable;

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之前调用 sqxc_ready()
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM migrations WHERE id = 1", (Sqxc*)xc_input, NULL);

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之后调用 sqxc_finish()
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return mtable;
	else {
		free(mtable);
		return NULL;
	}
```

#### 从 "migrations" 表中获取多行

使用 C 语言从 "migrations" 表中获取多行

```c
	SqxcValue  *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqPtrArray *array = sq_ptr_array_new(32, NULL);
	int         code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = array;

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之前调用 sqxc_ready()
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM migrations", (Sqxc*)xc_input, NULL);

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之后调用 sqxc_finish()
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return array;
	else {
		sq_ptr_array_free(array);
		return NULL;
	}
```

使用 C++ 语言从 "migrations" 表中获取多行

```c++
	Sq::XcValue   *xc_input = new Sq::XcValue();
	Sq::PtrArray  *array    = new Sq::PtrArray();
	int   code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_MIGRATION_TABLE;
	xc_input->instance  = array;

	xc_input->ready();
	code = db->exec("SELECT * FROM migrations", xc_input);
	xc_input->finish();

	if (code == SQCODE_OK)
		return array;
	else {
		delete array;
		return NULL;
	}
```

## 如何支持新的 SQL 产品：

用户可以参考 SqdbMysql.h 和 SqdbMysql.c 来支持新的 SQL 产品。  
SqdbEmpty.h 和 SqdbEmpty.c 是一个可行的示例，但它什么也不做。

#### 1 定义从 SqdbConfig 和 Sqdb 派生的新结构

所有派生结构必须符合 C++11 标准布局

```c++
// 这是头文件 - SqdbXxsql.h
#include <Sqdb.h>

// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct SqdbXxsql          SqdbXxsql;
typedef struct SqdbConfigXxsql    SqdbConfigXxsql;

// 定义 SQL 产品代码
#define  SQDB_PRODUCT_XXSQL    (SQDB_PRODUCT_CUSTOM + 1)

#ifdef __cplusplus    // 混合 C 和 C++
extern "C" {
#endif

// 在 SqdbXxsql.c 中定义
extern const SqdbInfo        SqdbInfo_XXSQL_;
#define SQDB_INFO_XXSQL    (&SqdbInfo_XXSQL_)

#ifdef __cplusplus    // 混合 C 和 C++
}  // extern "C"
#endif

// 从 SqdbConfig 派生的配置数据结构
struct SqdbConfigXxsql
{
	SQDB_CONFIG_MEMBERS;                   // <-- 1. 继承成员变量

	int   xxsql_setting;                   // <-- 2. 在派生结构中添加变量和非虚函数。
};

// 源自 Sqdb 的结构
#ifdef __cplusplus
struct SqdbXxsql : Sq::DbMethod            // <-- 1. 继承 C++ 成员函数 (方法)
#else
struct SqdbXxsql
#endif
{
	SQDB_MEMBERS;                          // <-- 2. 继承成员变量

	SqdbConfigXxsql *config;               // <-- 3. 在派生结构中添加变量和非虚函数。
	int            variable;


#ifdef __cplusplus
	// 如果您使用 C++ 语言，请在此处定义 C++ 构造函数和析构函数。
	SqdbXxsql(const SqdbConfigXxsql *config) {
		// 调用 Sq::DbMethod::init()
		init(SQDB_INFO_XXSQL, (SqdbConfig*)config);
	}
	~SqdbXxsql() {
		// 调用 Sq::DbMethod::final()
		final();
	}
#endif
};
```

#### 2 实现 SqdbInfo 接口

```c
// 这是源文件 - SqdbXxsql.c
#include <SqdbXxsql.h>

// 为 SqdbInfo 声明函数
static void sqdb_xxsql_init(SqdbXxsql *sqdb, const SqdbConfigXxsql *config);
static void sqdb_xxsql_final(SqdbXxsql *sqdb);
static int  sqdb_xxsql_open(SqdbXxsql *sqdb, const char *database_name);
static int  sqdb_xxsql_close(SqdbXxsql *sqdb);
static int  sqdb_xxsql_exec(SqdbXxsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
static int  sqdb_xxsql_migrate(SqdbXxsql *sqdb, SqSchema *schema, SqSchema *schema_next);

// 由 SqdbXxsql.h 使用
const SqdbInfo SqdbInfo_XXSQL_ = {
	.size    = sizeof(SqdbXxsql),
	.product = SQDB_PRODUCT_XXSQL,
	.column  = {
		.has_boolean = 1,
		.use_alter  = 1,
		.use_modify = 0,
	},
	.quote = {
		.identifier = {'"', '"'}     // ANSI-SQL 引用标识符是 ""
	},

	.init    = (void*) sqdb_xxsql_init,
	.final   = (void*) sqdb_xxsql_final,
	.open    = (void*) sqdb_xxsql_open,
	.close   = (void*) sqdb_xxsql_close,
	.exec    = (void*) sqdb_xxsql_exec,
	.migrate = (void*) sqdb_xxsql_migrate,
};

// 在这里实现 SqdbXxsql 的函数

static void sqdb_xxsql_init(SqdbXxsql *sqdb, const SqdbConfigXxsql *config)
{
	// 初始化 SqdbXxsql 实例
	sqdb->version  = 0;
	sqdb->config   = config;
}

static void sqdb_xxsql_final(SqdbXxsql *sqdb)
{
	// 终结 SqdbXxsql 实例
}

static int  sqdb_xxsql_open(SqdbXxsql *sqdb, const char *database_name)
{
	// 打开数据库并获取它的架构版本
	sqdb->version = db_schema_version;
	return SQCODE_OK;
}

static int  sqdb_xxsql_close(SqdbXxsql *sqdb)
{
	// 关闭数据库
	return SQCODE_OK;
}

static int  sqdb_xxsql_exec(SqdbXxsql *sqdb, const char *sql, Sqxc *xc, void *reserve);
{
	if (xc == NULL) {
		// 执行查询
	}
	else {
		switch (sql[0]) {
		case 'S':    // SELECT
		case 's':    // select
			// 从 xxsql 获取行并将它们发送到 'xc'
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// 将插入的行 id 设置为 SqxcSql.id
			((SqxcSql*)xc)->id = inserted_id;
			break;

		case 'U':    // UPDATE
		case 'u':    // update
			// 设置 SqxcSql.changes 为更改行数
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

static int  sqdb_xxsql_migrate(SqdbXxsql *db, SqSchema *schema_current, SqSchema *schema_next)
{
	if (schema_next == NULL) {
		// 将 'schema_current' 同步到数据库。这主要由 SQLite 使用
		return SQCODE_OK;
	}

	if (db->version < schema_next->version) {
		// 通过 'schema_next' 进行迁移
		for (int index = 0;  index < schema_next->type->n_entry;  index++) {
			SqTable *table = (SqTable*)schema_next->type->entry[index];
			if (table->bit_field & SQB_CHANGED) {
				// 更改表   ALTER TABLE
			}
			else if (table->name == NULL) {
				// 删除表   DROP TABLE
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// 重命名表 RENAME TABLE
			}
			else {
				// 创建表   CREATE TABLE
			}
		}
	}

	// 包含并应用来自 'schema_next' 的更改
	sq_schema_update(schema_current, schema_next);
	schema_current->version = schema_next->version;
}
```

#### 3 使用自定义 Sqdb

使用 C 语言

```c++
	SqdbConfigXxsql  config = {
		.xxsql_setting = 0,
	};
	Sqdb            *db;
	SqStorage       *storage;

	// 使用配置数据创建自定义 Sqdb 对象
	db = sqdb_new(SQDB_INFO_XXSQL, (SqdbConfig*) &config);

	// 创建使用新 Sqdb 的存储对象
	storage = sq_storage_new(db);
```

使用 C++ 语言

```c++
	SqdbConfigXxsql  config = {
		.xxsql_setting = 0,
	};
	SqdbXxsql       *db;
	Sq::Storage     *storage;

	// 使用配置数据创建自定义 Sqdb 对象
	db = new SqdbXxsql(&config);

	// 创建使用新 Sqdb 的存储对象
	storage = new Sq::Storage(db);
```
