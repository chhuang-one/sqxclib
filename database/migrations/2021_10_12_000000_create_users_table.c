/* This template file is used by sqxctool
 * Please define structure 'User' in workspace/sqxcapp/CStructs.h
 *
 * Normally this file should be included in migrations-files.c if you use sqxctool to make migration file.
 * migrations-files.c has included following headers.
 * #include <SqStorage.h>
 * #include <SqMigration.h>
 * #include "CStructs.h"
 */

/* This is example of C migration file for structure 'User' defined in CStructs.h
   To enable this file, please edit following files in ../../sqapp/
     migrations-declarations
     migrations-elements
     migrations-files.c
 */

// Run the migrations.
static void up_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "users", User);
	column = sq_table_add_int(table, "id", offsetof(User, id));
	sq_column_primary(column);
	sq_column_auto_increment(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), 0);
	column = sq_table_add_string(table, "email", offsetof(User, email), 0);

#if 1
	sq_table_add_timestamps_struct(table, User);
#else
	// sq_table_add_timestamps_struct(table, User) will do these
	column = sq_table_add_timestamp(table, "created_at", offsetof(User, created_at));
	sq_column_use_current(column);

	column = sq_table_add_timestamp(table, "updated_at", offsetof(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);
#endif
}

// Reverse the migrations.
static void down_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
	sq_schema_drop(schema, "users");
}

const SqMigration createUsersTable_2021_10_12_000000 = {
	.up   = up_2021_10_12_000000,
	.down = down_2021_10_12_000000,

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
	.name = "2021_10_12_000000_create_user_table",
#endif
};
