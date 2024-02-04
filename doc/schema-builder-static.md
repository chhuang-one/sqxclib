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

In the following example,
SQ_TYPE_XXXX are C/C++ data types, they are listed in (SqType)[SqType.md].  
SQB_XXXX     are bit fields that used by (SqColumn)[SqColumn.md].

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

Query-only column names only apply to SQL SELECT query. You can set SqColumn::name to the SELECT query and set SQB_QUERY_ONLY in SqColumn::bit_field to define query-only column. This can store result of special query like 'SELECT length(BlobColumn), * FROM table' to C structure's member.  
  
**Note**: Enable SQ_CONFIG_QUERY_ONLY_COLUMN in SqConfig.h if you want to use this feature.  
  
e.g. Define columns used to store the results of the SQL statement "SELECT length(str), * FROM table" into a C structure's member when querying data.

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  QueryFirst    QueryFirst;

struct QueryFirst
{
	int    id;

	char  *str;

	// length of 'str'
	// SQL statement: SELECT length(str), * FROM table
	int    str_length;
};

static const SqColumn  queryFirstColumns[3] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",          offsetof(QueryFirst, id),         SQB_PRIMARY},

	// Query-only column:
	// 1. column name is SELECT query
	// 2. SqColumn::bit_field has SQB_QUERY_ONLY
	{SQ_TYPE_INT,    "length(str)", offsetof(QueryFirst, str_length), SQB_QUERY_ONLY},

	// VARCHAR
	{SQ_TYPE_STR,    "str",         offsetof(QueryFirst, str),        0},
};
```

**Join multiple tables using the same column name**  
  
If you join multiple tables using the same column name and use query-only column with function, you can add table name as prefix in column name of function parameter.

```c
	// change "length(ColumnName)" to "length(TableName.ColumnName)"
	{SQ_TYPE_INT,    "length(TableName.ColumnName)", offsetof(TableName, ColumnName), SQB_QUERY_ONLY},
```

If you define constant SqType that has query-only column. SqType::bit_field must have SQB_TYPE_QUERY_FIRST.

```c++
// constant pointer array of SqColumn for queryFirstColumns
static const SqColumn *queryFirstColumnPtrs[3] = {
	&queryFirstColumns[0],
	&queryFirstColumns[1],
	&queryFirstColumns[2],
};

// SqType::bit_field must have SQB_TYPE_QUERY_FIRST
const SqType  queryFirstType = SQ_TYPE_INITIALIZER(QueryFirst, queryFirstColumnPtrs, SQB_TYPE_QUERY_FIRST);
```

## Type mapping (static)

Users can specify the SQL data type in SqColumn::sql_type, which will map to the C data type specified in SqColumn::type.
If SqColumn::sql_type is equal to 0, program will try to determine the SQL data type from SqColumn::type.
SQL data types are listed in [SqTable](SqTable.md).  
  
You must use type mapping if you define constant [SqColumn](SqColumn.md) with non-built-in [SqType](SqType.md).
Because some SQL types (such as **BLOB**, **CLOB**, and **TEXT**) do not have default built-in SqType, user must specify which SqType map to these SQL data types.  
  
Example 1: use C SqBuffer to store SQL BLOB data.

```c
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct  Mapping    Mapping;

struct Mapping
{
	int       id;

	// type mapping
	char     *text;

	// type mapping + query-only column
	// specify length of BLOB in SqBuffer::size before parsing 'picture'.
	SqBuffer  picture;
};

static const SqColumn  mappingColumns[4] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",              offsetof(Mapping, id),   SQB_PRIMARY},

	// type mapping: SQ_TYPE_STR map to SQL data type - TEXT
	{SQ_TYPE_STR,    "text",            offsetof(Mapping, text), 0,
		.sql_type = SQ_SQL_TYPE_TEXT},

	// Query-only column: SqColumn::bit_field must has SQB_QUERY_ONLY
	// specify length of BLOB in SqBuffer::size before parsing 'picture'.
	// use this to get length of BLOB data for SQLite or MySQL.
	{SQ_TYPE_INT,    "length(picture)", offsetof(Mapping, picture) + offsetof(SqBuffer, size), SQB_QUERY_ONLY},

	// type mapping: SQ_TYPE_BUFFER map to SQL data type - BLOB
	{SQ_TYPE_BUFFER, "picture",         offsetof(Mapping, picture),    0,
		.sql_type = SQ_SQL_TYPE_BLOB},
};
```

Example 2: use C++ std::vector<char> to store SQL BLOB data.  
  
Note: SQ_TYPE_STD_VECTOR_SIZE will specify size of BLOB by calling std::vector<char>::resize() when parsing integer value.

```c++
struct MappingCpp
{
	int       id;

	// type mapping
	std::string  text;

	// type mapping + query-only column
	// calling std::vector<char>::resize() to specify length of BLOB before parsing 'picture'.
	std::vector<char>  picture;
};

static const SqColumn  mappingCppColumns[4] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,     "id",              offsetof(MappingCpp, id),   SQB_PRIMARY},

	// type mapping: SQ_TYPE_STD_STR map to SQL data type - TEXT
	{SQ_TYPE_STD_STR, "text",            offsetof(MappingCpp, text), 0,
		.sql_type = SQ_SQL_TYPE_TEXT},

	// Query-only column: SqColumn::bit_field must has SQB_QUERY_ONLY
	// calling std::vector<char>::resize() to specify length of BLOB before parsing 'picture'.
	// use this to get length of BLOB data for SQLite or MySQL.
	{SQ_TYPE_STD_VECTOR_SIZE, "length(picture)", offsetof(MappingCpp, picture), SQB_QUERY_ONLY},

	// type mapping: SQ_TYPE_STD_VECTOR map to SQL data type - BLOB
	{SQ_TYPE_STD_VECTOR,      "picture",         offsetof(MappingCpp, picture), 0,
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
