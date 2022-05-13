/* This template file is used by sqxcpptool
// migrations-files.cpp has included below headers.
#include <SqStorage.h>
#include <SqMigration.h>
#include <CStructs.h>
 */

const SqMigration alter_users_table_2021_11_26_091532 = {

	// Run the migrations.
//	.up =
	[](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("users");
		table->integer("age", &User::age)->nullable();
	},

	// Reverse the migrations.
//	.down =
	[](SqSchema *schema, SqStorage *storage) {
		SqTable  *table;

		table = schema->alter("users");
		table->dropColumn("age");
	},

#ifdef SQ_APP_TOOL
//	.name =
	"2021_11_26_091532_alter_users_table",
#endif
};
