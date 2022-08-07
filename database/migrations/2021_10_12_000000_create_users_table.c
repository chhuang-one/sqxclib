/* This template file is used by sqxctool
// migrations-files.c has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"        // define struct 'User' in CStructs.h
 */

// Run the migrations.
static void up_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
	SqColumn *column;

	table  = sq_schema_create(schema, "users", User);
	column = sq_table_add_uint(table, "id", offsetof(User, id));
	sq_column_primary(column);
	sq_column_auto_increment(column);

	column = sq_table_add_string(table, "name", offsetof(User, name), 0);
	column = sq_table_add_string(table, "email", offsetof(User, email), 0);

	sq_table_add_timestamps_struct(table, User);

/*	// sq_table_add_timestamps_struct(table, User) will do these
	column = sq_table_add_timestamp(table, "created_at", offsetof(User, created_at));
	sq_column_use_current(column);

	column = sq_table_add_timestamp(table, "updated_at", offsetof(User, updated_at));
	sq_column_use_current(column);
	sq_column_use_current_on_update(column);
 */
}

// Reverse the migrations.
static void down_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
	sq_schema_drop(schema, "users");
}

const SqMigration CreateUsersTable_2021_10_12_000000 = {
	.up   = up_2021_10_12_000000,
	.down = down_2021_10_12_000000,

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
	.name = "2021_10_12_000000_create_user_table",
#endif
};
