# SqSchema
SqSchema defines database schema

	SqEntry
	│
	├--- SqSchema
	│
	└--- SqReentry
	     │
	     ├--- SqTable
	     │
	     └--- SqColumn

Relationship of SqSchema, SqTable, and SqColumn.

	SqSchema ---┬--- SqTable 1 ---┬--- SqColumn 1
	            │                 │
	            │                 └--- SqColumn n
	            │
	            └--- SqTable n ---  ...

# SqTable
SqTable defines SQL table

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

# SqColumn
[SqColumn](SqColumn.md) defines column in SQL table.

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqColumn

## 1. Create table and column by schema builder (dynamic)

It is recommended to use schema builder to create dynamic table.  
To get more information and sample, you can see below documents:  
1. [database-migrations.md](database-migrations.md)
2. **(Schema Builder)** parts of "**Database schema**" in ../[README.md](../README.md#database-schema)
  
schema builder C++ sample code:

```c++
	// define a C struct to map database table "areas".
	struct Area {
		int     id;          // primary key
		char   *name;
	};

	// create schema version 1
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;        // specify version number or auto generate it

	// create table "areas"
	table = schema_v1->create<Area>("areas");

	// add dynamic columns to table
	table->integer("id", &Area::id)->primary();  // PRIMARY KEY
	table->string("name", &Area::name);

	// do migration
	storage->migrate(schema_v1);   // migrate schema_v1

	// synchronize schema_v1 to database and update schema/table status
	// This is mainly used by SQLite
	storage->migrate(NULL);

	// free unused 'schema_v1'
	delete schema_v1;
```

## 2. Create table by existed SqType (static or dynamic)
You can see documents [SqColumn.md](SqColumn.md) to get more information about creating SqType with SqColumn.

```c++
	// C function
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// C++ method
	table = schema->create("your_table_name", type);
```
