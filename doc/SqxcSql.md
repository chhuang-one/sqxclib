[中文](SqxcSql.cn.md)

# SqxcSql

SqxcSql is derived from [Sqxc](Sqxc.md). It uses SQL to output Sqxc data to a database.

	Sqxc
	│
	└─── SqxcSql

## Create the Sqxc chain

Create the following Sqxc chain to output data to database.

	Sqxc data arguments ────> SqxcSql ────> sqdb_exec()

use C language

```c
	Sqxc *xcsql;

	xcsql = sqxc_new(SQXC_INFO_SQL);
	/* another way to create Sqxc elements */
//	xcsql = sqxc_sql_new();
```

use C++ language

```c++
	Sq::XcSql *xcsql;

	xcsql = new Sq::XcSql();
```

If you want to write JSON object or array to column, modify the above Sqxc chain as follows:

	                      ┌─> SqxcJsoncWriter ─┐
	Sqxc data arguments ──┴────────────────────┴──> SqxcSql   ───> sqdb_exec()

use C language

```c
	Sqxc *xcjson;

	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* another way to create Sqxc elements */
//	xcjson = sqxc_jsonc_writer_new();

	// append JSON writer to Sqxc chain
	sqxc_insert(xcsql, xcjson, -1);
```

use C++ language

```c++
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// append JSON writer to Sqxc chain
	xcsql->insert(xcjson);
```

## Setup SqxcSql

SqxcSql can generate INSERT or UPDATE SQL statements, which can be set using sqxc_ctrl().
In this case, the parameters of sqxc_ctrl() are control code and SQL table name.

| SQL statement | sqxc_ctrl() control code  |
| ------------- | ------------------------- |
| INSERT        | SQXC_SQL_CTRL_INSERT      |
| UPDATE        | SQXC_SQL_CTRL_UPDATE      |

```c++
	// use C language
	// SqxcSql will generate INSERT statement to insert into 'tableName'
	sqxc_ctrl(xcsql, SQXC_SQL_CTRL_INSERT, tableName);

	// use C++ language
	// SqxcSql will generate UPDATE statement to update 'tableName'
	xcsql->ctrl(SQXC_SQL_CTRL_UPDATE, tableName);
```

SqxcSql can specify [Sqdb](Sqdb.md) instance of database and WHERE condition of SQL statement by setting data members.

```c
	// SQL Database
	xcsql->db = sqdb_instance;

	// WHERE condition
	xcsql->condition = "WHERE id < 100";
```

## Pass arguments to Sqxc chain

Using sqxc_send() to pass data arguments can forward data between Sqxc elements. The following is just part of the code, please refer to the document [Sqxc.md](Sqxc.md) for more details.  
  
use C language

```c
	// Because arguments in 'xcsql' will not be used in Sqxc chain,
	// 'xcsql' is used as initial arguments source here.
	Sqxc *xc = (Sqxc*)xcsql;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// Pass the data arguments 'xc' to the Sqxc chain,
	// Return the Sqxc element currently processing the data arguments.
	xc = sqxc_send(xc);

	// continue to set data arguments in 'xc'
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// continue passing data arguments to the Sqxc element that is currently processing data arguments.
	xc = sqxc_send(xc);
```

use C++ language

```c++
	// Because arguments in 'xcsql' will not be used in Sqxc chain,
	// 'xcsql' is used as initial arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcsql;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// Pass the data arguments 'xc' to the Sqxc chain,
	// Return the Sqxc element currently processing the data arguments.
	xc = xc->send();

	// continue to set data arguments in 'xc'
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// continue passing data arguments to the Sqxc element that is currently processing data arguments.
	xc = xc->send();
```

When using SqxcSql and SqxcJsonc at the same time, it is not recommended to use sqxc_send_to() directly to pass data arguments.
Because user must switch between SqxcSql and SqxcJsonc when users send data arguments.

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
