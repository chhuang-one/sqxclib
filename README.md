[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxclib

sqxclib is a library to convert SQL (or JSON...etc) data to/from C language.
It provides ORM features and C++ wrapper.

## Current features:
1. User can use C99 designated initializer (or C++ aggregate initialization) to define SQL table/column/migration statically.
   You can also use C/C++ function to do these dynamically.

2. All defined table/column can use to parse JSON object/field.
   Program can also parse JSON object/array from SQL column.

3. It can work in low-end hardware.

4. Single header file sqxclib.h  (Note: It doesn't contain special macros)

5. Supports SQLite, MySQL / MariaDB.

## Database schema

```c
struct User {
	int    id;          // primary key
	char  *full_name;
	char  *email;
	int    city_id;     // foreign key

	time_t    created_at;

#ifdef __cplusplus      // C++ Only
	std::string       strCpp;
	std::vector<int>  intsCpp;
#endif
};
```

use C99 designated initializer to define table/column in schema_v1 (static)

```c
#include <sqxclib.h>

static const SqColumn  userColumns[6] = {
	{SQ_TYPE_INT,    "id",         offsetof(User, id),       SQB_PRIMARY},
	{SQ_TYPE_STRING, "full_name",  offsetof(User, full_name)  },
	{SQ_TYPE_STRING, "email",      offsetof(User, email),    .size = 60},    // VARCHAR(60)

	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at),
		.default_value = "CURRENT_TIMESTAMP"},

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

	schema_v1 = sq_schema_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);
	// add static 'userColumns' that has 6 elements to table
	sq_table_add_column(table, userColumns, 6);
```

use C99 designated initializer to change table/column in schema_v2 (static)

```c
static const SqColumn  userColumnsChanged[5] = {
	// ADD COLUMN "test_add"
	{SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

	// ALTER COLUMN "city_id"
	{SQ_TYPE_INT,  "city_id", offsetof(User, city_id), SQB_CHANGED},

	// DROP CONSTRAINT FOREIGN KEY
	{.old_name = "users_city_id_foreign",     .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_FOREIGN },

	// DROP COLUMN "full_name"
	{.old_name = "full_name", .name = NULL},

	// RENAME COLUMN "email" TO "email2"
	{.old_name = "email",     .name = "email2"},
};

	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);
	// add static 'userColumnsChanged' that has 5 elements to table
	sq_table_add_column(table, userColumnsChanged, 5);
```

use C function to define table/column in schema_v1 (dynamic)

```c
	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = sq_schema_create(schema_v1, "users", User);
	// add dynamic columns to table
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;        // set bit in SqColumn.bit_field
	column = sq_table_add_string(table, "full_name", offsetof(User, full_name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), 60);    // VARCHAR(60)
	column = sq_table_add_timestamp(table, "created_at", offset(User, created_at));
	sq_column_default("CURRENT_TIMESTAMP");
	// FOREIGN KEY
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");
	// CONSTRAINT FOREIGN KEY
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	sq_column_on_delete(column, "NO ACTION");
	sq_column_on_update(column, "NO ACTION");
	// CREATE INDEX
	column = sq_table_add_index(table, "users_id_index", "id", NULL);
```

use C function to change table/column in schema_v2 (dynamic)

```c
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = sq_schema_alter(schema_v2, "users", NULL);
	// add dynamic columns/records to table
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	column->bit_field |= SQB_CHANGED;        // set bit in SqColumn.bit_field
	sq_table_drop_foreign(table, "users_city_id_foreign");
	sq_table_drop_column(table, "full_name");
	sq_table_rename_column(table, "email", "email2");
```

use C macro to define table/column in schema_v1 (dynamic)

```c
#include <sqxclib.h>
#include <SqSchema-macro.h>    // sqxclib.h doesn't contain special macros

	schema_v1 = sq_schmea_new("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	SQ_SCHEMA_CREATE(schema_v1, "users", User, {
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();
		SQT_STRING("full_name", User, full_name, -1);
		SQT_STRING("email", User, email, 60);    // VARCHAR(60)
		SQT_TIMESTAMP("created_at", User, created_at);  SQC_DEFAULT("CURRENT_TIMESTAMP");
		// FOREIGN KEY
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");
		// CONSTRAINT FOREIGN KEY
		SQT_ADD_FOREIGN("users_city_id_foreign", "city_id");
			SQC_REFERENCE("cities", "id");  SQC_ON_DELETE("NO ACTION");  SQC_ON_UPDATE("NO ACTION"):
		// CREATE INDEX
		SQT_ADD_INDEX("users_id_index", "id");
	});
```

use C macro to change table/column in schema_v2 (dynamic)

```c
	schema_v2 = sq_schema_new("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	SQ_SCHEMA_ALTER(schema_v2, "users", User, {
		SQT_INTEGER("test_add", User, test_add);
		SQT_INTEGER("city_id", User, city_id);  SQC_CHANGE();
		SQT_DROP_FOREIGN("users_city_id_foreign");
		SQT_DROP("full_name");
		SQT_RENAME("email", "email2");
	});
```

use C++ aggregate initialization to define table/column in schema_v1 (static)

```c++
#include <sqxclib.h>

Sq::TypeStl<std::vector<int>> SqTypeIntVector(SQ_TYPE_INT);    // C++ std::vector

static const SqForeign userForeign = {"cities",  "id",  "CASCADE",  "CASCADE"};

static const SqColumn  userColumns[6] = {
	{SQ_TYPE_INT,    "id",         offsetof(User, id),       SQB_PRIMARY},
	{SQ_TYPE_STRING, "full_name",  offsetof(User, full_name)  },
	{SQ_TYPE_STRING, "email",      offsetof(User, email),    .size = 60},    // VARCHAR(60)
	{SQ_TYPE_TIME,   "created_at", offsetof(User, created_at),
		.default_value = "CURRENT_TIMESTAMP"},

	// FOREIGN KEY
	{SQ_TYPE_INT,    "city_id",    offsetof(User, city_id),
		.foreign = (SqForeign*) &userForeign},
	// C++ std::string
	{SQ_TYPE_STD_STRING, "strCpp", offsetof(User, strCpp)     },
	// C++ std::vector
	{&SqTypeIntVector,  "intsCpp", offsetof(User, intsCpp)    },
};

	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = schema_v1->create<User>("users");
	// add static 'userColumns' that has 6 elements to table
	table->addColumn(userColumns, 6);
```

use C++ function to define table/column in schema_v1 (dynamic)

```c++
	schema_v1 = new Sq::Schema("Ver 1");
	schema_v1->version = 1;    // specify version number or auto generate it

	// create table "users"
	table = schema_v1->create<User>("users");
	// add dynamic columns to table
	table->integer("id", &User::id)->primary();
	table->string("full_name", &User::full_name);
	table->string("email", &User::email, 60);    // VARCHAR(60)
	table->timestamp("created_at", &User::created_at)->default_("CURRENT_TIMESTAMP");
	table->stdstring("strCpp", &User::strCpp);                     // C++ std::string
	table->custom("intsCpp", &User::intsCpp, &SqTypeIntVector);    // C++ std::vector
	// FOREIGN KEY
	table->integer("city_id", &User::city_id)->reference("cities", "id");
	// CONSTRAINT FOREIGN KEY
	table->addForeign("users_city_id_foreign", "city_id")
	     ->reference("cities", "id")->onDelete("NO ACTION")->onUpdate("NO ACTION");
	// CREATE INDEX
	table->addIndex("users_id_index", "id", NULL);
```

use C++ function to change table/column in schema_v2 (dynamic)

```c++
	schema_v2 = new Sq::Schema("Ver 2");
	schema_v2->version = 2;    // specify version number or auto generate it

	// alter table "users"
	table = schema_v2->alter("users");
	// add dynamic columns/records to table
	table->integer("test_add", &User::test_add);
	table->integer("city_id", &User::city_id)->change();
	table->dropForeign("users_city_id_foreign");    // DROP CONSTRAINT FOREIGN KEY
	table->drop("full_name");
	table->rename("email", "email2");
```

Other constraint sample: use C99 designated initializer to change constraint (static)

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
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_PRIMARY },

	// DROP CONSTRAINT UNIQUE
	{.old_name = "other_unique",   .name = NULL,
	 .type = SQ_TYPE_CONSTRAINT,  .bit_field = SQB_UNIQUE },
};
```

Other constraint sample: use C function to change constraint (dynamic)

```c
	// ADD CONSTRAINT UNIQUE
	sq_table_add_unique(table, "other_unique", "column1", "column2", NULL);
	// ADD CONSTRAINT PRIMARY KEY
	sq_table_add_primary(table, "other_primary", "column1", "column2", NULL);

	// DROP CONSTRAINT UNIQUE
	sq_table_drop_unique(table, "other_unique");
	// DROP CONSTRAINT PRIMARY KEY
	sq_table_drop_primary(table, "other_primary");
```

Other constraint sample: use C++ function to change constraint (dynamic)

```c++
	// ADD CONSTRAINT UNIQUE
	table->addUnique("other_unique", "column1", "column2", NULL);
	// ADD CONSTRAINT PRIMARY KEY
	table->addPrimary("other_primary", "column1", "column2", NULL);

	// DROP CONSTRAINT UNIQUE
	table->dropUnique("other_unique");
	// DROP CONSTRAINT PRIMARY KEY
	table->dropPrimary("other_primary");
```

## Migration

use C++ function to migrate schema and synchronize to database

```c++
	storage->migrate(schema_v1); // migrate schema_v1
	storage->migrate(schema_v2); // migrate schema_v2
	storage->migrate(NULL);      // synchronize schema to database. (Mainly used by SQLite)
	delete schema_v1;            // free unused schema_v1
	delete schema_v2;            // free unused schema_v2
```

use C function to migrate schema and synchronize to database

```c
	sq_storage_migrate(storage, schema_v1); // migrate schema_v1
	sq_storage_migrate(storage, schema_v2); // migrate schema_v2
	sq_storage_migrate(storage, NULL);      // synchronize schema to database. (Mainly used by SQLite)
	sq_schema_free(schema_v1);              // free unused schema_v1
	sq_schema_free(schema_v2);              // free unused schema_v2
```

## CRUD

use C function

```c
	User  *user;

	array = sq_storage_get_all(storage, "users", NULL, NULL);
	user  = sq_storage_get(storage, "users", NULL, 2);

	sq_storage_insert(storage, "users", NULL, user);
	sq_storage_update(storage, "users", NULL, user);
	sq_storage_remove(storage, "users", NULL, 5);
```

use C++ function

```c++
	User  *user;

	array = storage->getAll("users", NULL);
	user  = storage->get("users", 2);

	storage->insert("users", user);
	storage->update("users", user);
	storage->remove("users", 5);
```

use C++ template function

```c++
	User  *user;

	vector = storage->getAll<std::vector<User>>();
	// or
	array  = storage->getAll<User>(NULL);

	user = storage->get<User>(2);

	storage->remove<User>(5);

	storage->insert<User>(user);
	storage->update<User>(user);
	// or
	storage->insert(user);
	storage->update(user);
```

## Database support

use C function to open SQLite database

```c
	SqdbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_SQLITE, NULL);     // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");    // This will open file "sqxc_local.db"
```

use C function to open MySQL database

```c
	SqdbConfigMysql  config = { .host = "localhost", .port = 3306,
	                            .user = "name", .password = "xxx" };

	db = sqdb_new(SQDB_INFO_MYSQL, (SqdbConfig*) &config);
//	db = sqdb_new(SQDB_INFO_MYSQL, NULL);    // use default setting if config is NULL.

	storage = sq_storage_new(db);
	sq_storage_open(storage, "sqxc_local");
```

use C++ function to open SQLite database

```c++
	Sq::DbConfigSqlite  config = { .folder = "/path", .extension = "db" };

	db = new Sq::DbSqlite(&config);
//	db = new Sq::DbSqlite(NULL);    // use default setting if config is NULL.
//	db = sqdb_new(SQDB_INFO_SQLITE, (SqdbConfig*) &config);    // this also works.

	storage = new Sq::Storage(db);
	storage->open("sqxc_local");    // This will open file "sqxc_local.db"
```

## Query builder

SQL statement

```sql
	SELECT id, age
	FROM companies
	JOIN ( SELECT * FROM city WHERE id < 100 ) AS c ON c.id = companies.city_id
	WHERE age > 5
```

use C++ function to produce query

```c++
	query->select("id", "age", NULL)
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where("age > 5");
```

use C function to produce query

```c
	sq_query_select(query, "id", "age", NULL);
	sq_query_from(query, "companies");
	sq_query_join(query, NULL);
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "100");
	sq_query_pop_nested(query);
	sq_query_as(query, "c");
	sq_query_on(query, "c.id = companies.city_id");
	sq_query_where(query, "age > 5");
```

use C macro to produce query

```c
#include <sqxclib.h>
#include <SqQuery-macro.h>    // sqxclib.h doesn't contain special macros

	SQ_QUERY_DO(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "100");
		}); SQQ_AS("c"); SQQ_ON("c.id = companies.city_id");
		SQQ_WHERE("age > 5");
	});
```

## JOIN support

use C function

```c
	SqPtrArray *array;

	sq_query_from(query, "cities");
	sq_query_join(query, "users",  "cities.id", "users.city_id");

	array = sq_storage_query(storage, query, NULL, NULL);
	for (int i = 0;  i < array->length;  i++) {
		element = (void**)array->data[i];
		city = (City*)element[0];    // sq_query_from(query, "cities");
		user = (User*)element[1];    // sq_query_join(query, "users", ...);
		// free 'element' before you free 'array'
		// free(element);
	}
```

use C++ function

```c++
	query->from("cities")->join("users",  "cities.id", "users.city_id");

	typedef  Sq::Joint<2>  SqJoint2;
	vector = storage->query<std::vector<SqJoint2>>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		SqJoint2 *joint = vector->at(index);
		city = (City*)joint.t[0];
		user = (User*)joint.t[1];
	}
	// or
	SqPtrArray *array = (Sq::PtrArray*) storage->query(query);
	for (int i = 0;  i < array->length;  i++) {
		element = (void**)array->data[i];
		city = (City*)element[0];    // from("cities")
		user = (User*)element[1];    // join("users")
		// free 'element' before you free 'array'
		// free(element);
	}
```

## Transaction

use C function

```c
	User  *user;

	sq_storage_begin(storage);
	sq_storage_insert(storage, "users", NULL, user);
	if (abort)
		sq_storage_rollback(storage);
	else
		sq_storage_commit(storage);
```

use C++ function

```c++
	User  *user;

	storage->begin();
	storage->insert<User>(user);
	if (abort)
		storage->rollback();
	else
		storage->commit();
```

## JSON support

- all defined table/column can use to parse JSON object/field
- program can also parse JSON object/array that store in column.

## Sqdb
 Sqdb is base structure for database product (SQLite, MySQL...etc).  
 SqdbSqlite.c implement Sqdb interface for SQLite.  
 SqdbMysql.c implement Sqdb interface for MySQL.  
 You can get more description and example in doc/[Sqdb.md](doc/Sqdb.md)  

## Sqxc
 Sqxc is interface for data parse and write.  
 User can link multiple Sqxc element to convert different types of data.  
 You can get more description and example in doc/[Sqxc.md](doc/Sqxc.md)  

## Others
 Document for SqEntry (SqColumn's base class/structure) in doc/[SqEntry.md](doc/SqEntry.md)  
 Document for SqType in doc/[SqType.md](doc/SqType.md)  

## Licensing

sqxc is licensed under the Mulan PSL v2.


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

