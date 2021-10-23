/* migrations.c has included below headers.
#include <SqStorage.h>
#include <CStructs.h>
 */

// Run the migrations.
void up_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
	SqTable  *table;
    SqColumn *column;

	table  = sq_schema_create(schema, "users", User);
    column = sq_table_add_uint(table, "id", offsetof(User, id));
    column->bit_field |= SQB_PRIMARY;

    column = sq_table_add_string(table, "name", offsetof(User, name), 0);
    column = sq_table_add_string(table, "email", offsetof(User, email), 0);

    sq_table_add_timestamps_struct(table, User);

/* sq_table_add_timestamps_struct(table, User) will do these
	column = sq_table_add_timestamp(table, "created_at", offsetof(User, created_at));
	column->bit_field |= SQB_CURRENT;
	column = sq_table_add_timestamp(table, "updated_at", offsetof(User, updated_at));
	column->bit_field |= SQB_CURRENT | SQB_CURRENT_ON_UPDATE;
 */
}

// Reverse the migrations.
void down_2021_10_12_000000(SqSchema *schema, SqStorage *storage)
{
    sq_schema_drop(schema, "users");
}

static const SqMigration CreateUsersTable_2021_10_12_000000 = {
	.up   = up_2021_10_12_000000,
	.down = down_2021_10_12_000000,

#ifdef SQ_APP_TOOL
	.name = "2021_10_12_000000_create_user_table",
#endif
};
