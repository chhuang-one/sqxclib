/* This template file is used by sqxcpptool
// define struct 'User' in workspace/sqxcapp/CStructs.h
// migrations-files.cpp has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include "CStructs.h"
 */

/*  This file is a rewrite of 2021_10_12_000000_create_users_table.c in C++.
    Normally it should be included in migrations-files.cpp if you use sqxcpptool to make migration file.
 */

const SqMigration create_users_table_2021_10_12_111111 = {

	// Run the migrations.
//	.up =
	[](SqSchema *schema, SqStorage *storage) {
		Sq::Table  *table;

		table = schema->create<User>("users");
		table->integer("id", &User::id)->primary()->autoIncrement();
		table->string("name", &User::name);
		table->string("email", &User::email);
#if 1
		table->timestamp("created_at", &User::created_at)->useCurrent();
		table->timestamp("updated_at", &User::updated_at)->useCurrent()->useCurrentOnUpdate();
#elif 1
		table->timestamps("created_at", &User::created_at,
		                  "updated_at", &User::updated_at);
#else
		table->timestamps(&User::created_at, &User::updated_at);
#endif
	},

	// Reverse the migrations.
//	.down =
	[](SqSchema *schema, SqStorage *storage) {
		schema->drop("users");
	},

#if defined(SQ_APP_TOOL) || SQ_APP_HAS_MIGRATION_NAME
//	.name =
	"2021_10_12_111111_create_users_table",
#endif
};
