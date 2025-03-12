[English](SqxcSql.md)

# SqxcSql

SqxcSql 派生自 [Sqxc](Sqxc.cn.md)。它使用 SQL 将 Sqxc 数据输出到数据库。

	Sqxc
	│
	└─── SqxcSql

## 创建 Sqxc 链

创建以下 Sqxc 链，将数据输出到数据库。

	Sqxc 数据参数 ────> SqxcSql ────> sqdb_exec()

使用 C 语言

```c
	Sqxc *xcsql;

	xcsql = sqxc_new(SQXC_INFO_SQL);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcsql = sqxc_sql_new();
```

使用 C++ 语言

```c++
	Sq::XcSql *xcsql;

	xcsql = new Sq::XcSql();
```

如果列有 JSON 数据，请修改上面的 Sqxc 链。

	               ┌─> SqxcJsoncWriter ─┐
	Sqxc 数据参数 ──┴────────────────────┴──> SqxcSql   ───> sqdb_exec()

使用 C 语言

```c
	Sqxc *xcjson;

	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcjson = sqxc_jsonc_writer_new();

	// 将 JSON 写入器附加到 Sqxc 链
	sqxc_insert(xcsql, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// 将 JSON 写入器附加到 Sqxc 链
	xcsql->insert(xcjson);
```

## 设置 SqxcSql

SqxcSql 可以生成 INSERT 或 UPDATE SQL 语句，可以使用 sqxc_ctrl() 进行设置。
在这种情况下，sqxc_ctrl() 的参数是控制代码和 SQL 表名。

| SQL 语句      | sqxc_ctrl() 控制码        |
| ------------- | ------------------------- |
| INSERT        | SQXC_SQL_CTRL_INSERT      |
| UPDATE        | SQXC_SQL_CTRL_UPDATE      |

```c++
	// 使用 C 语言
	// SqxcSql 将生成 INSERT 语句以插入到 'tableName' 中
	sqxc_ctrl(xcsql, SQXC_SQL_CTRL_INSERT, tableName);

	// 使用 C++ 语言
	// SqxcSql 将生成 UPDATE 语句来更新 'tableName'
	xcsql->ctrl(SQXC_SQL_CTRL_UPDATE, tableName);
```

SqxcSql 可以通过设置数据成员来指定数据库的 [Sqdb](Sqdb.cn.md) 实例 以及 SQL 语句的 WHERE 条件。

```c
	// SQL 数据库
	xcsql->db = sqdb_instance;

	// WHERE 条件
	xcsql->condition = "WHERE id < 100";
```

## 将参数传递给 Sqxc 链

使用 sqxc_send() 传递数据参数可以在 Sqxc 元素之间转发数据。以下只是部分代码，更多详细信息请参阅文档 [Sqxc.cn.md](Sqxc.cn.md)。  
  
使用 C 语言

```c
	// 因为 'xcsql' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcsql' 作为初始参数源。
	Sqxc *xc = (Sqxc*)xcsql;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 Sqxc 链，
	// 返回当前处理数据参数的 Sqxc 元素。
	xc = sqxc_send(xc);

	// 继续在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// 继续传递数据参数给当前正在处理数据参数的 Sqxc 元素。
	xc = sqxc_send(xc);
```

使用 C++ 语言

```c++
	// 因为 'xcsql' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcsql' 作为初始参数源。
	Sq::Xc *xc = (Sq::Xc*)xcsql;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 Sqxc 链，
	// 返回当前处理数据参数的 Sqxc 元素。
	xc = xc->send();

	// 继续在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// 继续传递数据参数给当前正在处理数据参数的 Sqxc 元素。
	xc = xc->send();
```

使用 SqxcSql 时，不建议直接使用 sqxc_send_to() 传递数据参数。

## 输出

SqxcSql 将在指定的 Sqdb 实例中执行生成的 SQL 语句。一些特殊结果（如插入行的 id 和更改的行数）在 SqxcSql 的数据成员中设置。

```c
	int64_t  inserted_id;
	int64_t  n_changes;

	// 插入行的 id
	inserted_id = xcsql->id;

	// 更改的行数
	n_changes   = xcsql->changes;
```
