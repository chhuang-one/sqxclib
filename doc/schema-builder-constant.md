[中文](schema-builder-constant.cn.md)

# Schema Builder (constant)

This document introduce how to use C99 designated initializer (or C++ aggregate initialization) to define table.
* This can reduce running time when making schema.
* Schema can handle both dynamic and constant column/table definitions.
* If user modify constant column/table, program will copy column/table before modifying it.
* Program will not free constant columns/tables from memory. It just doesn't use them.
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

## Creating Tables (constant)

To define constant column, The first four fields of [SqColumn](SqColumn.md) are the most commonly used. They are type, name, offset, and bit_field.  

* field 'type' is used to specify the C/C++ data type, usually starting with SQ_TYPE, see [SqType](SqType.md).
* field 'name' is used to specify the column name.
* field 'offset' is used to specify the offset of the field in the structure.
* field 'bit_field' is used to specify attributes such as primary key, usually starting with SQB, see [SqColumn](SqColumn.md).
* field 'foreign' is NULL-terminated array for setting referenced table, columns, ON DELETE action and ON UPDATE action. It uses empty string "" to separate columns and ON DELETE action.
  
field 'foreign' has more details in the section **About the field SqColumn::foreign**.  
  
**C99 designated initializer**  
  
e.g. use C99 designated initializer to define table and column in schema_v1.

```c
#include <sqxclib.h>

static const SqColumn  userColumns[6] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// VARCHAR
	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	// VARCHAR(60)
	{SQ_TYPE_STR,    "email",      offsetof(User, email),
		.size = 60},

	// FOREIGN KEY
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id")  ON DELETE CASCADE  ON UPDATE SET DEFAULT
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		.foreign = (char *[]) {"cities", "id",  "",  "CASCADE", "SET DEFAULT", NULL}  },
	// In the previous line, column "id" and ON DELETE action "CASCADE" must be separated by the empty string "".

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},
};

	/* create schema and specify version number as 1 */
	schema_v1 = sq_schema_new_ver(1, "Ver 1");

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);

	// add 'userColumns' that has 6 elements to table
	sq_table_add_column(table, userColumns, 6);
```

**C++ aggregate initialization**  
  
If your C++ compiler can't use designated initializer, you may use aggregate initialization as shown below.  
  
e.g. use C++ aggregate initialization to define table and column in schema_v1.

```c++
#include <sqxclib.h>

/* define global type for C++ STL
   create new SqType  for C++ std::vector<int> */
Sq::TypeStl< std::vector<int> > SqTypeIntVector(SQ_TYPE_INT);

/* this foreign key constraint is used by below userColumns[]
   In the next line, column "id" and ON DELETE action "CASCADE" must be separated by the empty string "". */
static const char *userForeign[] = {"cities", "id",  "",  "CASCADE", "SET DEFAULT", NULL};

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

	// FOREIGN KEY
	// "city_id"  INT  FOREIGN KEY REFERENCES "cities"("id")  ON DELETE CASCADE  ON UPDATE SET DEFAULT
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		NULL,                          // .old_name,
		0, 0, 0,                       // .sql_type, .size, .digits,
		NULL,                          // .default_value,
		(char **) userForeign},        // .foreign

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP  ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// C++ data type std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ data type std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* create schema and specify version number as 1 */
	schema_v1 = new Sq::Schema(1, "Ver 1");

	// create table "users"
	table = schema_v1->create<User>("users");
	// add 'userColumns' that has 8 elements to table
	table->addColumn(userColumns, 8);
```

## Updating Tables (constant)

* To rename a column, set field 'old_name' as current column name and 'name' as new column name.
* To delete a column, set field 'old_name' as current column name and 'name' as NULL.
* To change a column properties, add SQB_CHANGED to field 'bit_field'.

e.g. use C99 designated initializer to change table and column in schema_v2.  
'columnsChanges' contains records to add, alter, drop, and rename columns.  

```c
static const SqColumn  columnsChanges[4] = {
	// ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id",  offsetof(User, city_id), SQB_CHANGED},

	// DROP COLUMN "name"
	{.old_name = "name",      .name = NULL},

	// RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	/* create schema and specify version number as 2 */
	schema_v2 = sq_schema_new_ver(2, "Ver 2");

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);

	// alter table by 'columnsChanges' that has 4 elements
	sq_table_add_column(table, columnsChanges, 4);
```

## Constraints (constant)

set the constraint onto column definition:
* To define primary key column, set SQB_PRIMARY in SqColumn::bit_field.
* To define foreign key column, set referenced table, columns, actions in foreign. SQB_FOREIGN in 'bit_field' can be omitted if 'foreign' is not NULL.
* To define unique column, set SQB_UNIQUE in SqColumn::bit_field.

```c
static const SqColumn  columns[] = {
	// PRIMARY KEY
	{SQ_TYPE_INT,    "id",         offsetof(User, id),         SQB_PRIMARY},

	// FOREIGN KEY:  SQB_FOREIGN in 'bit_field' can be omitted if 'foreign' is not NULL.
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    SQB_FOREIGN,
		.foreign = (char *[]) {"cities", "id",  "",  "CASCADE", "CASCADE", NULL}  },
	//	                                             ^^^^^^^^^
	//	.foreign = (char *[]) {"cities", "id",  "",         "", "CASCADE", NULL}  },
	// FOREIGN KEY:  If no ON DELETE action is specified, set it to empty string "" as shown above.

	// UNIQUE
	{SQ_TYPE_STR,    "name",       offsetof(User, name),       SQB_UNIQUE},
}
```

**Composite Constraints**  

* field 'type' must be set as SQ_TYPE_CONSTRAINT. SQ_TYPE_CONSTRAINT is a fake data type used by migrations.
* field 'composite' is NULL-terminated array for setting columns of composite constraint.
* field 'foreign'   is NULL-terminated array for setting referenced table, columns, ON DELETE action and ON UPDATE action. It uses empty string "" to separate columns and ON DELETE action.
  
Because number of columns in foreign key must match the number of columns in the referenced table, number of columns in 'foreign' and 'composite' field must match.  
  
e.g. use C99 designated initializer to add/remove constraint.  
'otherChanges1' add  constraint (primary key, foreign key, and unique).  
'otherChanges2' drop constraint (primary key, foreign key, and unique).  

```c
static const SqColumn  otherChanges1[] = {
	// CONSTRAINT PRIMARY KEY
	{SQ_TYPE_CONSTRAINT,  "other_primary", 0,  SQB_PRIMARY,
		.composite = (char *[]) {"column1", "column2", NULL} },

	// CONSTRAINT FOREIGN KEY
	{SQ_TYPE_CONSTRAINT,  "other_foreign", 0,  SQB_FOREIGN,
		.foreign   = (char *[]) {"table",  "column1", "column2",  "",  "NO ACTION", "NO ACTION",  NULL},
		.composite = (char *[]) {          "column1", "column2",  NULL} },
	// Because number of columns in foreign key must match the number of columns in the referenced table,
	// number of columns in 'foreign' and 'composite' field must match.

	// CONSTRAINT UNIQUE
	{SQ_TYPE_CONSTRAINT,  "other_unique",  0,  SQB_UNIQUE,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherChanges2[] = {
	// DROP CONSTRAINT PRIMARY KEY
	{.old_name = "other_primary",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_PRIMARY },

	// DROP CONSTRAINT FOREIGN KEY
	{.old_name = "other_foreign",  .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_FOREIGN },

	// DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,   .bit_field = SQB_UNIQUE },
};
```

**About the field SqColumn::foreign**

* It is NULL-terminated array.
* It is used to setting referenced table, columns, ON DELETE action and ON UPDATE action.
* ON DELETE action and columns must be separated by an empty string "".
* ON DELETE action or ON UPDATE action can be set to empty string "" if not specified.

```c
	// ON DELETE action and column must be separated by an empty string "".
	// ON DELETE action is "CASCADE"
	// ON UPDATE action is "SET DEFAULT"
	.foreign   = (char *[]) {"table", "column",  "",  "CASCADE", "SET DEFAULT", NULL},

	// ON DELETE action and column must be separated by an empty string "".
	// ON DELETE action is not specified because it set to empty string "".
	// ON UPDATE action is "SET DEFAULT"
	.foreign   = (char *[]) {"table", "column",  "",         "", "SET DEFAULT", NULL},

	// ON DELETE action "CASCADE" and column must be separated by the empty string "".
	// ON UPDATE action is not specified because the last element is NULL.
	.foreign   = (char *[]) {"table", "column",  "",  "CASCADE", NULL},

	// specify table and column only
	// ON DELETE action and ON UPDATE action are not specified because the last element is NULL.
	.foreign   = (char *[]) {"table", "column", NULL},

	// specify table and multiple columns
	// ON DELETE action and ON UPDATE action are not specified because the last element is NULL.
	.foreign   = (char *[]) {"table", "column1", "column2", NULL},
```

## Index (constant)

* field 'type' must be set as SQ_TYPE_INDEX. SQ_TYPE_INDEX is a fake data type used by migrations.
* field 'composite' is NULL-terminated array for setting composite index.

e.g. use C99 designated initializer to add/remove index.  
'otherChanges3' add  index.  
'otherChanges4' drop index.  

```c
static const SqColumn  otherChanges3[] = {
	// CREATE INDEX
	{SQ_TYPE_INDEX,  "other_index", 0,  0,
		.composite = (char *[]) {"column1", "column2", NULL} },
};

static const SqColumn  otherChanges4[] = {
	// DROP INDEX
	{.old_name = "other_index",  .name = NULL,
	 .type = SQ_TYPE_INDEX },
};
```

## Use custom or JSON type (constant)

If you want to store JSON object or array in SQL column, you must specify [SqType](SqType.md).  
  
Example: define a C structure that contain struct and array.

```c
struct DemoTable {
	// ...

	// MyStructure is user-defined C struct
	// This will store JSON object in SQL column.
	MyStructure    myStruct;

	// SqIntArray is integer array that defined in SqArray.h
	// This will store JSON integer array in SQL column.
	SqIntArray     intArray;
};

static const SqColumn  demoTableColumns[] = {
	// ...

	// SQ_TYPE_MY_STRUCT is the user-defined SqType of MyStructure.
	// JSON object will be stored in SQL VARCHAR column if SQL type is not specified.
	{SQ_TYPE_MY_STRUCT,    "myStruct",       offsetof(DemoTable, myStruct)},

	// SQ_TYPE_INT_ARRAY is declared in SqType.h and parses JSON integer array from value of SQL column.
	// JSON integer array will be stored in SQL VARCHAR column if SQL type is not specified.
	{SQ_TYPE_INT_ARRAY,    "intArray",       offsetof(DemoTable, intArray)},
};
```

## Query-only column (constant)

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

## Type mapping (constant)

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
	{SQ_TYPE_INT,    "length(picture)", offsetof(Mapping, picture.size), SQB_QUERY_ONLY},

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

Whether dynamic and constant definitions, the code that running migrations is the same.  
  
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
