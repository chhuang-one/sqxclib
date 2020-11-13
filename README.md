[![Donate using PayPal](https://img.shields.io/badge/donate-PayPal-brightgreen.svg)](https://paypal.me/CHHUANGONE)

# sqxc

sqxc is a library to convert SQL (or JSON...etc) data to/from C language.
This is developed using 'C' language. It also provides C++ wrapper.

## Plans
- support more SQL database.
- complete SQL ORM features.
- It can work in low-end hardware.

## Why I develop this?
In new Android system, app must store data in limited time.
uGet for Android usually lost data because it spend much time to save ALL record at one time.
uGet3 use SQL database to solve this problem.

## Current features:
1. It can use C99 designated initializer to declare SQL table/column/migration.
   You can also use C/C++ function to do these.

2. All declared table/column can use to parse JSON object/field.
   Program can also parse JSON object/array from SQL column. (testing)

## Database schema

	struct User {
		int    id;         // primary key
		char*  full_name;
		char*  email;
		int    city_id;    // foreign key
	};

 use C99 designated initializer to declare table/column (static)

	static const SqColumn  *UserColumns[] = {
		&(SqColumn) {SQ_TYPE_INT,    "id",        offsetof(User, id),    SQB_PRIMARY},
		&(SqColumn) {SQ_TYPE_STRING, "full_name", offsetof(User, full_name)  },
		&(SqColumn) {SQ_TYPE_STRING, "email",     offsetof(User, email)      },
		&(SqColumn) {SQ_TYPE_INT,    "city_id",   offsetof(User, city_id),
		             .foreign = &(SqForeign) {"cities", "id", NULL, NULL}    },

		// CONSTRAINT FOREIGN KEY
		&(SqColumn) {.bit_field = SQB_CONSTRAINT | SQB_FOREIGN,
		             .name = "users_city_id_foreign",
		             .foreign = &(SqForeign) {"cities", "id", "no action", "no action"},
		             .composite = (char *[]) {"city_id", NULL} },

		// CREATE INDEX
		&(SqColumn) {.bit_field = SQB_INDEX,
		             .name = "users_id_index",
		             .composite = (char *[]) {"id", NULL} },
	};

 use C function to declare table/column (dynamic)

	SqTable*  table;
	SqColumn* column;

	table = sq_schema_create(schema, "users", User);
	column = sq_table_add_integer(table, "id", offsetof(User, id));
	column->bit_field |= SQB_PRIMARY;
	column = sq_table_add_string(table, "full_name", offsetof(User, full_name), -1);
	column = sq_table_add_string(table, "email", offsetof(User, email), -1);
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	sq_column_reference(column, "cities", "id");
	// CONSTRAINT FOREIGN KEY
	column = sq_table_add_foreign(table, "users_city_id_foreign", "city_id");
	sq_column_reference(column, "cities", "id");
	// CREATE INDEX
	column = sq_table_add_index(table, "users_id_index", "id", NULL);

 use C macro to declare table/column (dynamic)

	SQ_SCHEMA_CREATE(schema, "users", User, {
		SQT_INTEGER("id", User, id);  SQC_PRIMARY();
		SQT_STRING("full_name", User, full_name, -1);
		SQT_STRING("email", User, email, -1);
		SQT_INTEGER("city_id", User, city_id);  SQC_REFERENCE("cities", "id");
		// CONSTRAINT FOREIGN KEY
		SQT_ADD_FOREIGN("users_city_id_foreign", "city_id");  SQC_REFERENCE("cities", "id");
		// CREATE INDEX
		SQT_ADD_INDEX("users_id_index", "id");
	});

 use C++ function to declare table/column (dynamic)

	SqTable* table;

	table = schema->create<User>("users");
	table->integer("id", &User::id)->primary();
	table->string("full_name", &User::full_name);
	table->string("email", &User::email);
	table->integer("city_id", &User::city_id)->reference("cities", "id");
	// CONSTRAINT FOREIGN KEY
	table->addForeign("users_city_id_foreign", "city_id")->reference("cities", "id");
	// CREATE INDEX
	table->addIndex("users_id_index", "id", NULL);


## Migration

 use C99 designated initializer to change table/column (static)

	static const SqColumn  *UserColumnsChange[] = {
		// ADD COLUMN "test_add"
		&(SqColumn) {SQ_TYPE_INT,  "test_add", offsetof(User, test_add)},

		// ALTER COLUMN "city_id"
		&(SqColumn) {SQ_TYPE_INT,  "city_id", offsetof(User, city_id), SQB_CHANGED},

		// DROP COLUMN "full_name"
		&(SqColumn) {.old_name = "full_name", .name = NULL},

		// RENAME COLUMN "email" TO "email2"
		&(SqColumn) {.old_name = "email",     .name = "email2"},
	};

 use C function to change table/column (dynamic)

	table = sq_schema_alter(schema, "users", NULL);
	column = sq_table_add_integer(table, "test_add", offsetof(User, test_add));
	column = sq_table_add_integer(table, "city_id", offsetof(User, city_id));
	column->bit_field |= SQB_CHANGED;
	sq_table_drop_column(table, "full_name");
	sq_table_rename_column(table, "email", "email2");

 use C macro to change table/column (dynamic)

	SQ_SCHEMA_ALTER(schema, "users", User, {
		SQT_INTEGER("test_add", User, test_add);
		SQT_INTEGER("city_id", User, city_id);  SQC_CHANGE();
		SQT_DROP("full_name");
		SQT_RENAME("email", "email2");
	});

 use C++ function to change table/column (dynamic)

	table = schema->alter("users");
	table->integer("test_add", &User::test_add);
	table->integer("city_id", &User::city_id)->change();
	table->drop("full_name");
	table->rename("email", "email2");


## CRUD

 use C function

	User*  user;

	array = sq_storage_get_all(storage, "users", NULL, NULL);
	user  = sq_storage_get(storage, "users", NULL, 2);

	sq_storage_insert(storage, "users", NULL, user);
	sq_storage_update(storage, "users", NULL, user);
	sq_storage_remove(storage, "users", NULL, 5);

 use C++ function

	User*  user;

	array = storage->get_all("users", NULL);
	user  = storage->get("users", 2);

	storage->insert("users", user);
	storage->update("users", user);
	storage->remove("users", 5);

 use C++ template function

	User*  user;

	array = storage->get_all<User>(NULL);
	user  = storage->get<User>(2);

	storage->insert<User>(user);
	storage->update<User>(user);
	storage->remove<User>(5);


## JSON support

- all declared table/column can use to parse JSON object/field
- program can also parse JSON object/array that store in column.


## Query builder

 SQL statement

	SELECT id, age
	FROM companies
	JOIN ( SELECT * FROM city WHERE id < '100' ) AS c ON c.id = companies.city_id
	WHERE age > 5

 use C++ function to produce query

	query->select("id", "age", NULL)
	     ->from("companies")
	     ->join([query] {
	         query->from("city")
	              ->where("id", "<", "100");
	     })->as("c")->on("c.id = companies.city_id")
	     ->where(age > 5);

 use C function to produce query

	sq_query_select(query, "id", "age", NULL);
	sq_query_from(query, "companies");
	sq_query_join(query, NULL);
		sq_query_from(query, "city");
		sq_query_where(query, "id", "<", "100");
	sq_query_pop_nested(query);
	sq_query_as(query, "c");
	sq_query_on(query, "c.id = companies.city_id");
	sq_query_where(query, "age > 5");

 use C macro to produce query

	SQ_QUERY(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "100");
		}); SQQ_AS("c"); SQQ_ON("c.id = companies.city_id");
		SQQ_WHERE("age > 5");
	});


## Sqdb - Database interface
 Sqdb is Database interface for SQLite, MySQL...etc.
 SqdbSqlite.c implement Sqdb interface for SQLite. 


## Sqxc - Converter interface
 Sqxc is interface for data parse and write.
 User can link multiple Sqxc element to convert different types of data.


## Licensing

sqxc is licensed under the Mulan PSL v2.


---

觀察者網觀察觀察者，認同者眾推廣認同者。

观察者网观察观察者，认同者众推广认同者。

