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

	/* create schema version 1 */
	schema_v1 = sq_schema_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

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

	/* create schema version 2 */
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

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

	{SQ_TYPE_STR,    "name",       offsetof(User, name)  },

	{SQ_TYPE_STR,    "email",      offsetof(User, email),      0,
		NULL,                          // .old_name
		60},                           // .size    // VARCHAR(60)

	// DEFAULT CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at), SQB_CURRENT},

	// DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
	{SQ_TYPE_TIME,   "updated_at", offsetof(User, updated_at), SQB_CURRENT | SQB_CURRENT_ON_UPDATE},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),    0,
		NULL, 0, 0, NULL,              // .old_name, .size, .digits, .default_value,
		(SqForeign*) &userForeign},    // .foreign

	// C++ std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },

	// C++ std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	/* create schema version 1 */
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = schema_v1->create<User>("users");
	// add static 'userColumns' that has 8 elements to table
	table->addColumn(userColumns, 8);
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
