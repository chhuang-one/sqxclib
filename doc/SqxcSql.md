[中文](SqxcSql.cn.md)

# SqxcSql

SqxcSql is derived from [Sqxc](Sqxc.md). It uses SQL to output Sqxc data to a database.

	Sqxc
	│
	└─── SqxcSql

## Setup SqxcSql

SqxcSql can generate INSERT or UPDATE SQL statements, which can be switched using sqxc_ctrl().

| SQL statement | sqxc_ctrl() control code  |
| ------------- | ------------------------- |
| INSERT        | SQXC_SQL_CTRL_INSERT      |
| UPDATE        | SQXC_SQL_CTRL_UPDATE      |

```c++
    // use C language
	sqxc_ctrl(xcsql, SQXC_SQL_CTRL_INSERT, table_name);

    // use C++ language
    xcsql->ctrl(SQXC_SQL_CTRL_UPDATE, table_name);
```

SqxcSql can specify [Sqdb](Sqdb.md) instance of database and WHERE condition of SQL statement by setting data members.

```c
    // SQL Database
    xcsql->db = sqdb_instance;

    // WHERE condition
    xcsql->condition = "WHERE id < 100";
```

## Output

SqxcSql will execute generated SQL statement in specified Sqdb instance. Some special result like id of inserted row and number of rows changed are set in data members of SqxcSql.

```c
    int64_t  inserted_id;
    int64_t  n_changes;

    // id of inserted row
	inserted_id = xcsql->id;

    // number of rows changed
	n_changes   = xcsql->changes;
```
