[中文](schema-builder-static.cn.md)

# Schema Builder (static)

This document introduce how to use C99 designated initializer (or C++ aggregate initialization) to define table.
* This can reduce running time when making schema.
* Schema can handle both dynamic and static column/table definitions.
* If user modify static defined column/table, program will copy column/table before modifying it.
* Program will not free static defined columns/tables from memory. It just doesn't use them.
* If your SQL table is fixed and not changed in future, you can reduce more running time by using constant [SqType](SqType.md) to define table. see [SqColumn.md](SqColumn.md)

Define a C structured data type to map database table "users".

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  User    User;

struct User {
	int     id;          // primary key
	char   *name;
	char   *email;
	int     city_id;     // foreign key

	time_t  created_at;
	time_t  updated_at;

#ifdef __cplusplus       // C++ data type
	std::string       strCpp;
	std::vector<int>  intsCpp;
#endif
};
```

## C99 designated initializer

use C99 designated initializer to define table and column in schema_v1 (static)

```c
#include <sqxclib.h>

static const SqColumn  userColumns[8] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),
		.size = 60},

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),
		.foreign = &(SqForeign) {"cities", "id", NULL, NULL}    },

	// CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,  "users_city_id_foreign",
		.foreign = &(SqForeign) {"cities", "id", "NO ACTION", "NO ACTION"},
		.composite = (char *[]) {"city_id", NULL} },

	// CREATE INDEX
	{SQ_TYPE_INDEX,       "users_id_index",
		.composite = (char *[]) {"id", NULL} },
};

	/* create schema and specify version number as 1 */
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);

	// add static 'userColumns' that has 8 elements to table
	sq_table_add_column(table, userColumns, 8);
```

use C99 designated initializer to change table and column in schema_v2 (static)

```c
static const SqColumn  userColumnsChanged[5] = {
	// ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id",  offsetof(User, city_id), SQB_CHANGED},

	// DROP CONSTRAINT FOREIGN KEY
	{.old_name = "users_city_id_foreign",     .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_FOREIGN },

	// DROP COLUMN "name"
	{.old_name = "name",      .name = NULL},

	// RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	/* create schema and specify version number as 2 */
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// alter table by static 'userColumnsChanged' that has 5 elements
	sq_table_add_column(table, userColumnsChanged, 5);
```

use C99 designated initializer to change constraint (static)

```c
static const SqColumn  otherSampleChanged_1[] = {
	// CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "other_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"column1", "column2", NULL} },

	// CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "other_unique",  0,  SQB_UNIQUE,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherSampleChanged_2[] = {
	// DROP CONSTRAINT PRIMARY KEY
	{.old_name = "other_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_PRIMARY },

	// DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_UNIQUE },
};
```

## C++ aggregate initialization
All data in members are the same as above sample code.  
  
use C++ aggregate initialization to define table and column in schema_v1 (static)

```c++
#include <sqxclib.h>

/* define global type for C++ STL */
Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

static const SqForeign userForeign = {"cities",  "id",  "CASCADE",  "CASCADE"};

static const SqColumn  userColumns[8] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),      0,
		NULL,                          // .old_name
		0,                             // .sql_type
		60},                           // .size        // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    0,
		NULL,                          // .old_name,
		0, 0, 0,                       // .sql_type, .size, .digits,
		NULL,                          // .default_value,
		(SqForeign*) &userForeign},    // .foreign

	// C++ std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* create schema and specify version number as 1 */
	schema_v1 = new Sq::Schema(1, "Ver 1");

	// create table "users"
	table = schema_v1->create<User>("users");
	// add static 'userColumns' that has 8 elements to table
	table->addColumn(userColumns, 8);
```

## Query-only column (static)

Enable SQ_CONFIG_QUERY_ONLY_COLUMN in SqConfig.h if you want store result of special query like 'SELECT length(BlobColumn), * FROM table' in C struct.
To define a query-only column, set SqColumn.name to the SELECT query and SqColumn.bit_field have SQB_QUERY_ONLY.  
  
e.g. Define columns used to store the results of the SQL command "SELECT length(str), * FROM table" into a C structure when querying data.

```c++
struct QueryFirst
{
	int    id;

	char  *str;

	// length of 'str'
	// SQL command: SELECT length(str), * FROM table
	int    str_length;
};

static const SqColumn  queryFirstColumns[3] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",          offsetof(QueryFirst, id),         SQB_PRIMARY},

	// query-only column: SqColumn.bit_field has SQB_QUERY_ONLY
	// column name is SELECT queries
	{SQ_TYPE_INT,    "length(str)", offsetof(QueryFirst, str_length), SQB_QUERY_ONLY},

	// VARCHAR
	{SQ_TYPE_STR,    "str",         offsetof(QueryFirst, str),        0},
};
```

If you define constant SqType that has query-only column. SqType.bit_field must have SQB_TYPE_QUERY_FIRST.

```c++
// constant pointer array of SqColumn for queryFirstColumns
static const SqColumn *queryFirstColumnPtrs[3] = {
	&QueryFirstColumns[0],
	&QueryFirstColumns[1],
	&QueryFirstColumns[2],
};

// SqType.bit_field must have SQB_TYPE_QUERY_FIRST
const SqType  queryFirstType = SQ_TYPE_INITIALIZER(QueryFirst, queryFirstColumnPtrs, SQB_TYPE_QUERY_FIRST);
```

## Type mapping (static)

If you define constant SqColumn with SQL type BLOB and TEXT, you must use type mapping.

```c
struct Mapping
{
	int       id;

	// type mapping
	char     *text;

	// type mapping + query-only column
	// Assign length of BLOB in SqBuffer.size before parsing
	SqBuffer  picture;
};

static const SqColumn  mappingColumns[4] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",              offsetof(Mapping, id),         SQB_PRIMARY},

	// type mapping: SQ_TYPE_STR map to SQL data type - TEXT
	{SQ_TYPE_STR,    "text",            offsetof(Mapping, text),
		.sql_type = SQ_SQL_TYPE_TEXT},

	// query-only column: SqColumn.bit_field must have SQB_QUERY_ONLY
	// Assign length of BLOB in SqBuffer.size before parsing 'picture', This is mainly for SQLite or MySQL.
	{SQ_TYPE_INT,    "length(picture)", offsetof(Mapping, picture) + offsetof(SqBuffer, size), SQB_QUERY_ONLY},

	// type mapping: SQ_TYPE_BUFFER map to SQL data type - BLOB
	{SQ_TYPE_BUFFER, "picture",         offsetof(Mapping, picture),    0,
		.sql_type = SQ_SQL_TYPE_BLOB},
};
```

## Migrations

Whether dynamic and static definitions, the code that running migrations is the same.  
  
use C++ methods to migrate schema and synchronize to database

```c++
	// migrate 'schema_v1' and 'schema_v2'
	storage->migrate(schema_v1);
	storage->migrate(schema_v2);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	storage->migrate(NULL);

	// free unused 'schema_v1' and 'schema_v2'
	delete schema_v1;
	delete schema_v2;
```

use C functions to migrate schema and synchronize to database

```c
	// migrate 'schema_v1' and 'schema_v2'
	sq_storage_migrate(storage, schema_v1);
	sq_storage_migrate(storage, schema_v2);

	// synchronize schema to database and update schema in 'storage'
	// This is mainly used by SQLite
	sq_storage_migrate(storage, NULL);

	// free unused 'schema_v1' and 'schema_v2'
	sq_schema_free(schema_v1);
	sq_schema_free(schema_v2);
```
