# SqSchema
SqSchema defines database schema

	SqEntry
	|
	├--- SqSchema
	|
	└--- SqReentry
	     │
	     ├--- SqTable
	     │
	     └--- SqColumn

Relationship of SqSchema, SqTable, and SqColumn.

	SqSchema ---┐
	            ├--- SqTable 1 ---  ...
	            │
	            └--- SqTable n ---┐
	                              ├--- SqColumn 1
	                              │
	                              └--- SqColumn n

# SqTable
SqTable defines SQL table

	SqEntry
	│
	└─── SqReentry
	     │
	     └─── SqTable

# SqColumn
SqColumn defines column in SQL table.

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

## 2. Create table by existed SqType (static or dynamic)
To get more information about SqType + SqColumn, you can see documents [SqColumn.md](SqColumn.md)

```c++
	// C function
	table = sq_schema_create_by_type(schema, "your_table_name", type);

	// C++ method
	table = schema->create("your_table_name", type);
```
