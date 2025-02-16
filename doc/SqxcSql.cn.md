[English](SqxcSql.md)

# SqxcSql

SqxcSql 派生自 [Sqxc](Sqxc.cn.md)。它使用 SQL 将 Sqxc 数据输出到数据库。

	Sqxc
	│
	└─── SqxcSql

## 设置 SqxcSql

SqxcSql 可以生成 INSERT 或 UPDATE SQL 语句，可以使用 sqxc_ctrl() 进行切换。

| SQL 语句      | sqxc_ctrl() 控制码        |
| ------------- | ------------------------- |
| INSERT        | SQXC_SQL_CTRL_INSERT      |
| UPDATE        | SQXC_SQL_CTRL_UPDATE      |

```c++
    // 使用 C 语言
	sqxc_ctrl(xcsql, SQXC_SQL_CTRL_INSERT, table_name);

    // 使用 C++ 语言
    xcsql->ctrl(SQXC_SQL_CTRL_UPDATE, table_name);
```

SqxcSql 可以通过设置数据成员来指定数据库的 [Sqdb](Sqdb.cn.md) 实例 以及 SQL 语句的 WHERE 条件。

```c
    // SQL 数据库
    xcsql->db = sqdb_instance;

    // WHERE 条件
    xcsql->condition = "WHERE id < 100";
```

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
