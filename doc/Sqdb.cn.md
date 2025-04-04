[English](Sqdb.md)

# Sqdb

Sqdb 是 SQLite、MySQL 等数据库产品的基础结构。
以下是已实现的数据库产品：

| 派生结构      | 数据库产品 | 源代码文件     |
| ------------- | ---------- | -------------- |
| SqdbSqlite    | SQLite     | SqdbSqlite.c   |
| SqdbMysql     | MySQL      | SqdbMysql.c    |
| SqdbPostgre   | PostgreSQL | SqdbPostgre.c  |

## 如何支持新的数据库产品：

请参阅文档 [database-interface.cn.md](database-interface.cn.md)。

## 打开和关闭数据库

sqdb_open() 将在打开数据库时获取当前架构版本号。  
* SQLite 用户可以在 SqdbConfigSqlite 中设置 '文件夹' 和 '扩展名'，这些会影响数据库文件名和路径。

使用 C 函数

```c
	// 数据库配置
	SqdbConfigSqlite config = {
		.folder    = "/home/someone",
		.extension = "db"
	};
	// 数据库实例的指针
	Sqdb  *db;

	// 使用 'config' 创建 SqdbSqlite。如果 config 为 NULL，则使用默认设置。
	db = sqdb_sqlite_new(&config);
	// 结果与上一行相同。
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);

	// 打开数据库文件 - "/home/someone/local-base.db"
	sqdb_open(db, "local-base");

	// 关闭数据库
	sqdb_close(db);
```

使用 C++ 方法

```c++
	// 数据库配置
	Sq::DbConfigSqlite config = {
		"/home/someone",   // .folder    = "/home/someone",
		"db"               // .extension = "db",
	};
	// 数据库实例的指针
	Sq::DbMethod  *db;

	// 使用 'config' 创建 Sq::DbSqlite。如果 config 为 NULL，则使用默认设置。
	db = new Sq::DbSqlite(config);

	// 打开数据库文件 - "/home/someone/local-base.db"
	db->open("local-base");

	// 关闭数据库
	db->close();
```

## 迁移

sqdb_migrate() 使用架构的版本来决定是否迁移。它有 2 个 schema 参数，第一个参数 'schemaCurrent' 是当前版本的架构，第二个参数 'schemaNext' 是下一个版本的架构。'schemaNext' 的更改将应用​​于 'schemaCurrent'。
迁移后您无法重用 'schemaNext'，因为此功能可能会将数据从 'schemaNext' 移动到 'schemaCurrent'。  
  
请不要在 'schemaCurrent' 中直接更改、重命名和删除表，而是在 'schemaNext' 中执行这些操作然后运行 migrate() 将更改应用于 'schemaCurrent'。  
  
要通知数据库实例迁移已完成，请调用 sqdb_migrate() 并在参数 'schemaNext' 中传入 NULL。这将清除未使用的数据、对表和列进行排序，并将当前架构同步到数据库（主要用于 SQLite）。  
  
使用 C 函数

```c
	// 将 'schemaNext' 的更改应用于 'schemaCurrent'
	sqdb_migrate(db, schemaCurrent, schemaNext);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 'schemaCurrent' 并且将 'schemaCurrent' 同步到数据库（主要用于 SQLite）。
	sqdb_migrate(db, schemaCurrent, NULL);
```

使用 C++ 方法

```c++
	// 将 'schemaNext' 的更改应用于 'schemaCurrent'
	db->migrate(schemaCurrent, schemaNext);

	// 要通知数据库实例迁移已完成，传递 NULL 给最后一个参数。
	// 这将更新和排序 'schemaCurrent' 并且将 'schemaCurrent' 同步到数据库（主要用于 SQLite）。
	db->migrate(schemaCurrent, NULL);
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

	code = sqdb_exec(db, "SELECT max(id) FROM users", (Sqxc*)xc_input, NULL);

	if (code == SQCODE_OK)
		return integer;
```

#### 从 "users" 表中获取一行

```c
	SqxcValue *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	UserTable *utable = calloc(1, sizeof(UserTable));
	int   code;

	xc_input->container = NULL;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = utable;

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之前调用 sqxc_ready()
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM users WHERE id = 1", (Sqxc*)xc_input, NULL);

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之后调用 sqxc_finish()
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return utable;
	else {
		free(utable);
		return NULL;
	}
```

#### 从 "users" 表中获取多行

使用 C 语言从 "users" 表中获取多行

```c
	SqxcValue  *xc_input = (SqxcValue*)sqxc_new(SQXC_INFO_VALUE);
	SqPtrArray *array = sq_ptr_array_new(32, NULL);
	int         code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = array;

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之前调用 sqxc_ready()
	sqxc_ready(xc_input, NULL);

	code = sqdb_exec(db, "SELECT * FROM users", (Sqxc*)xc_input, NULL);

	// 如果要重用 Sqxc 元素，请在 sqdb_exec() 之后调用 sqxc_finish()
	sqxc_finish(xc_input, NULL);

	if (code == SQCODE_OK)
		return array;
	else {
		sq_ptr_array_free(array);
		return NULL;
	}
```

使用 C++ 语言从 "users" 表中获取多行

```c++
	Sq::XcValue   *xc_input = new Sq::XcValue();
	Sq::PtrArray  *array    = new Sq::PtrArray();
	int   code;

	xc_input->container = SQ_TYPE_PTR_ARRAY;
	xc_input->element   = SQ_TYPE_USER_TABLE;
	xc_input->instance  = array;

	xc_input->ready();
	code = db->exec("SELECT * FROM users", xc_input);
	xc_input->finish();

	if (code == SQCODE_OK)
		return array;
	else {
		delete array;
		return NULL;
	}
```
