[中文](schema-builder-macro.cn.md)

# Schema Builder (C macro)

This document introduce how to use C macro to define table dynamically.  

Define a C structured data type that maps to your database table "users".

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  User    User;

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;
	int     city_id;     // foreign key

	time_t  created_at;
};
```

use C macro to define table and column in schema_v1 (dynamic)  
  
macro SQ_SCHEMA_CREATE() can create table. The last parameter in macro is like lambda function.  
macro SQT_XXXX() is used to add column to table.  
macro SQC_XXXX() is used to set column properties.

```c
#include <sqxclib.h>
#include <SqSchema-macro.h>    // sqxclib.h doesn't contain special macros

	// create schema and specify version number as 1
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// create table "users"
	SQ_SCHEMA_CREATE(schema_v1, "users", User, {
		// PRIMARY KEY
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();

		// VARCHAR
		SQT_STRING("name", User, name, -1);

		// VARCHAR(60)
		SQT_STRING("email", User, email, 60);

		// DEFAULT CURRENT_TIMESTAMP
		SQT_TIMESTAMP("created_at", User, created_at);  SQC_USE_CURRENT();

		// FOREIGN KEY
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");

		// CONSTRAINT FOREIGN KEY
		SQT_ADD_FOREIGN("users_city_id_foreign", "city_id");
			SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("NO ACTION");  SQC_ON_UPDATE("NO ACTION"):

		// CREATE INDEX
		SQT_ADD_INDEX("users_id_index", "id");
	});
```

use C macro to change table and column in schema_v2 (dynamic)  
  
macro SQ_SCHEMA_ALTER() can alter table. The last parameter in macro is like lambda function.

```c
	// create schema and specify version number as 2
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// alter table "users"
	SQ_SCHEMA_ALTER(schema_v2, "users", User, {
		// add column to table
		SQT_INTEGER("test_add", User, test_add);

		// alter column in table
		SQT_INTEGER("city_id", User, city_id);  SQC_CHANGE();

		// DROP CONSTRAINT FOREIGN KEY
		SQT_DROP_FOREIGN("users_city_id_foreign");

		// drop column
		SQT_DROP("name");

		// rename column
		SQT_RENAME("email", "email2");
	});
```

## Query-only column, Type mapping (C macro)

To define a type mapping column, use macro SQT_MAPPING() to map SQL type and SqType.  
  
To define a query-only column, set column name to the SELECT query and use macro SQC_QUERY_ONLY() to set column properties.
**Note**: Enable SQ_CONFIG_QUERY_ONLY_COLUMN in SqConfig.h if you want to use query-only column.  

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  Demo    Demo;

struct Demo {
	int     id;          // primary key

	char   *text;

	// length of 'text'
	// SQL statement: SELECT length(text), * FROM table
	int     text_length;
};

	// create table "demo"
	SQ_SCHEMA_CREATE(schema_v1, "demo", Demo, {
		// PRIMARY KEY
		SQT_INTEGER("id", Demo, id);  SQC_PRIMARY();

		// type mapping: SQ_TYPE_STR map to SQL data type TEXT
		SQT_MAPPING("text", Demo, text, SQ_TYPE_STR, SQ_SQL_TYPE_TEXT);

		// Query-only column
		SQT_INTEGER("length(text)", Demo, text_length);  SQC_QUERY_ONLY();
	});
```
