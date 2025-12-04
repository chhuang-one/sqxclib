/* This template file is used by sqtool-cpp
 * Please define structure 'User' in workspace/sqxcapp/CStructs.h
 *
 * Normally this file should be included in migrations-files-cxx.cpp if you use sqtool-cpp to make migration file.
 * migrations-files-cxx.cpp has included following headers.
 * #include <SqStorage.h>
 * #include <SqMigration.h>
 * #include "CStructs.h"
 */

/* This is example of C++ migration file for structure 'User' defined in CStructs.h
   To enable this file, please edit following files in ../../sqapp/
     migrations-declarations
     migrations-elements
     migrations-files-cxx.cpp
 */

const SqMigration createUsersTable_2021_10_12_000000 = {

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

#if SQ_APP_HAS_MIGRATION_NAME
//	.name =
	"2021_10_12_000000_create_users_table",
#endif
};
