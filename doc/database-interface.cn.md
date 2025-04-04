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
			// 执行 SQL SELECT 语句
			// 从 Xsql 获取行并将它们发送到 'xc'
			sqdb_xsql_exec_select(sqdb, sql, (SqxcValue*)xc);
			break;

		case 'I':    // INSERT
		case 'i':    // insert
			// 执行 SQL INSERT 语句
			// 将插入行的 ID 存储在 SqxcSql::id 中
			((SqxcSql*)xc)->id = inserted_id;
			break;

		case 'U':    // UPDATE
		case 'u':    // update
			// 执行 SQL UPDATE 语句
			// 将更改的行数存储在 SqxcSql::changes 中
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

sqdb_xsql_exec_select() 是一個處理 SELECT 语句结果的函式。展示了 SQL 行和列的處理流程。

```c
int  sqdb_xsql_exec_select(SqdbXsql *sqdb, const char *sql, SqxcValue *xc)
{
	// 如果 SqxcValue 准备接收多行
	if (xc->container != NULL) {
		// SQL 多行对应 SQXC_TYPE_ARRAY
		xc->type = SQXC_TYPE_ARRAY;
		xc->name = NULL;
		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
	}

	// 获取行
	while (row = xsql_get_row(sqdb)) {
		// 特殊情况：
		// 如果用户只选择一列并且该列类型是内置类型（不是对象），则不要发送对象。
		if (SQ_TYPE_NOT_BUILTIN(xc->element)) {
			// SQL 行对应于 SQXC_TYPE_OBJECT
			xc->type = SQXC_TYPE_OBJECT;
			xc->name = NULL;
			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}

		// 获取列
		while (col = xsql_get_column(sqdb)) {
			// 将 SQL 列发送到 Sqxc 链
			xc->type = SQXC_TYPE_STR;
			xc->name = columnName1;
			xc->value.str = columnValue1;
			xc = sqxc_send(xc);
			if (xc->code != SQCODE_OK) {
				// 发生错误
			}
		}

		// 特殊情况：
		// 如果用户只选择一列并且该列类型是内置类型（不是对象），则不要发送对象。
		if (SQ_TYPE_NOT_BUILTIN(xc->element)) {
			// SQL 行对应于 SQXC_TYPE_OBJECT
			xc->type = SQXC_TYPE_OBJECT_END;
			xc->name = NULL;
			xc->value.pointer = NULL;
			xc = sqxc_send(xc);
		}
	}

	// 如果 SqxcValue 准备接收多行
	if (xc->container != NULL) {
		// SQL 多行对应 SQXC_TYPE_ARRAY
		xc->type = SQXC_TYPE_ARRAY_END;
		xc->name = NULL;
//		xc->value.pointer = NULL;
		xc = sqxc_send(xc);
	}
}
```

#### 3.4 migrate

SqdbInfo::migrate() 使用架构的版本来决定是否迁移。它有 2 个 schema 参数，第一个参数 'schemaCurrent' 是当前版本的架构，第二个参数 'schemaNext' 是下一个版本的架构。'schemaNext' 的更改将应用​​于 'schemaCurrent'。
此功能可以将数据从 'schemaNext' 移动到 'schemaCurrent'，因此用户在迁移后无法重复使用 'schemaNext'。  
  
要通知数据库实例迁移已完成，请调用 SqdbInfo::migrate() 并在参数 'schemaNext' 中传入 NULL。这将清除未使用的数据、对表和列进行排序，并将当前架构同步到数据库（主要用于 SQLite）。

```c
static int  sqdb_xsql_migrate(SqdbXsql *db, SqSchema *schemaCurrent, SqSchema *schemaNext)
{
	SqBuffer *buffer;

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
			// 根据 'table' 中的记录运行迁移。
			// 表相关记录在 'table' 中，列相关记录在 'table->type->entry' 中。

			// 确定要执行的操作（ALTER、DROP、RENAME、ADD）。
			// 这里的处理流程与 'column' 相同。
			if (table->bit_field & SQB_CHANGED) {
				// 更改表   ALTER TABLE
				// table->name 是需要修改的表的名称
				sqdb_xsql_alter_table(sqdb, buffer, table);
			}
			else if (table->name == NULL) {
				// 删除表   DROP TABLE
				// table->old_name 是要删除的表的名称
			}
			else if (table->old_name && (table->bit_field & SQB_RENAMED) == 0) {
				// 重命名表 RENAME TABLE
				// table->old_name 是要重命名的表的名称
				// table->name     是表的新名称
			}
			else {
				// 创建表   CREATE TABLE
				// table->name 是要创建的表的名称
				sqdb_xsql_create_table(sqdb, buffer, table);
			}
		}
	}

	// 包含并应用来自 'schemaNext' 的更改
	sq_schema_update(schemaCurrent, schemaNext);
	schemaCurrent->version = schemaNext->version;
}
```

sqdb_xsql_alter_table() 是一个修改表的函数。它显示了 'table' 中记录的处理流程。
请参考 Sqdb.c 中的函数 sqdb_exec_alter_table() 获取更多详细信息。

```c
int  sqdb_xsql_alter_table(SqdbXsql *db, SqBuffer *buffer, SqTable *table)
{
	SqPtrArray *columnArray;

	columnArray = sq_type_entry_array(table->type);
	for (unsigned int index = 0;  index < columnArray->length;  index++) {
		SqColumn *column = (SqColumn*)columnArray->data[index];

		// 确定要执行的操作（ALTER、DROP、RENAME、ADD）。
		// 此处的处理流程与 'table' 相同。
		if (column->bit_field & SQB_COLUMN_CHANGED) {
			// 更改列   ALTER COLUMN
			// column->name 是要修改的列的名称
		}
		else if (column->name == NULL) {
			// 删除列   DROP COLUMN / CONSTRAINT / INDEX / KEY
			// column->old_name 是要删除的列的名称
		}
		else if (column->old_name && (column->bit_field & SQB_COLUMN_RENAMED) == 0) {
			// 重命名列 RENAME COLUMN
			// column->old_name 是要重命名的列的名称
			// column->name     是列的新名称
		}
		else {
			// 添加列   ADD COLUMN / CONSTRAINT / INDEX / KEY
			// column->name     是要添加的列的名称
		}
	}
}
```

sqdb_xsql_create_table() 是一个创建表的函数。它根据 'table' 中的记录创建 SQL 表。
下面的代码有很多遗漏，请参考 Sqdb.c 中的函数 sqdb_sql_write_column() 获取更多详细信息。

```c
int  sqdb_xsql_create_table(SqdbXsql *db, SqBuffer *buffer, SqTable *table)
{
	SqPtrArray *columnArray;

	columnArray = sq_type_entry_array(table->type);
	for (unsigned int index = 0;  index < columnArray->length;  index++) {
		SqColumn *column = (SqColumn*)columnArray->data[index];

		// 写入列名
		sq_buffer_write(buffer, column->name);

		// 写入列类型
		int sql_type = column->sql_type;
		if (sql_type == SQ_SQL_TYPE_UNKNOWN) {
			// 将 column->type 映射到 SqSqlType
			if (SQ_TYPE_IS_BUILTIN(type))
				sql_type = (int)SQ_TYPE_BUILTIN_INDEX(type) + 1;
			else
				sql_type = SQ_SQL_TYPE_VARCHAR;
		}

		switch (sql_type) {
		case SQ_SQL_TYPE_BOOLEAN:
			if (db->info->column.has_boolean)
				sq_buffer_write(buffer, "BOOLEAN");
			else
				sq_buffer_write(buffer, "TINYINT");
			break;

		case SQ_SQL_TYPE_INT:
		case SQ_SQL_TYPE_INT_UNSIGNED:
			// 省略
			break;

		// 省略
		}

		// 写入其他属性
		if (column->bit_field & SQB_COLUMN_NULLABLE)
			sq_buffer_write(buffer, " NOT NULL");
		if (column->bit_field & SQB_COLUMN_AUTOINCREMENT)
			sq_buffer_write(buffer, " AUTO_INCREMENT");
		// 省略
	}
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
