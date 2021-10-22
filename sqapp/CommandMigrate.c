/*
 *   Copyright (C) 2021 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <stdio.h>

#include <SqError.h>
#include <SqMigration.h>

#include <SqConsole.h>
#include <SqApp.h>
#include <CommandMigrate.h>

// ----------------------------------------------------------------------------
// migrate

static void migrate(SqCommandValue *cmd_value, SqConsole *console, void *data)
{
	SqApp *app = data;
	int    code;

	code = sq_app_migrate(app, ((CommandMigrate*)cmd_value)->step);
	if (code)
		printf("Can't migrate\n");
}

static const SqOption *migrate_options[] = {
	&(SqOption) {SQ_TYPE_BOOL, "step",   offsetof(CommandMigrate, step),
		.default_value = "true",
		.description = "Force the migrations to be run so they can be rolled back individually"},
};

static const SqCommand migrate_command = SQ_COMMAND_INITIALIZER(
	CommandMigrate, 0,                             // StructureType, bit_field
	"migrate",                                     // command string
	migrate_options,                               // SqOption pointer array
	migrate,                                       // handle function
	NULL,                                          // parameter string
	"Run the database migrations"                  // description string
);
/* Macro Expands to
static const SqCommand migrate_command = {
	.size  = sizeof(CommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate",
	.entry   = (SqEntry**) migrate_options,
	.n_entry = sizeof(migrate_options) / sizeof(SqOption*),
	// SqCommand members
	.handle      = (SqCommandFunc) migrate,
	.parameter   = NULL,
	.description = "Run the database migrations",
};
 */

// ----------------------------------------------------------------------------
// migrate:install

static void migrate_install(SqCommandValue *cmd_value, SqConsole *console, void *data)
{
	SqApp *app = data;
	int    code;

	code = sq_migration_install(app->db);
	if (code != SQCODE_OK)
		printf("Can't install migration table\n");
}

static const SqOption *migrate_install_options[] = {
	&(SqOption) {SQ_TYPE_BOOL, "quiet",  offsetof(CommandMigrate, quiet),
		.default_value = "true",
		.description = "Do not output any message"},
};

static const SqCommand migrate_install_command = SQ_COMMAND_INITIALIZER(
	CommandMigrate, 0,                            // StructureType, bit_field
	"migrate:install",                            // command string
	migrate_install_options,                      // SqOption pointer array
	migrate_install,                              // handle function
	NULL,                                         // parameter string
	"Create the migration repository"             // description string
);
/* Macro Expands to
static const SqCommand migrate_install_command = {
	.size  = sizeof(CommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate:install",
	.entry   = (SqEntry**)migrate_install_options,
	.n_entry = sizeof(migrate_install_options) / sizeof(SqOption*),
	// SqCommand members
	.handle      = (SqCommandFunc) migrate_install,
	.parameter   = NULL,
	.description = "Create the migration repository",
};
 */

// ----------------------------------------------------------------------------
// migrate:rollback

static void migrate_rollback(SqCommandValue *cmd_value, SqConsole *console, void *data)
{
	SqApp *app = data;
	int    code;

	code = sq_app_migrate_rollback(app, ((CommandMigrate*)cmd_value)->step);
	if (code != SQCODE_OK)
		printf("Can't install migration table\n");
}

static const SqOption *migrate_rollback_options[] = {
	&(SqOption) {SQ_TYPE_INT,  "step",   offsetof(CommandMigrate, step),
		.default_value = "0",
		.value_description = "[=STEP]",
		.description = "The number of migrations to be reverted"},
};

static const SqCommand migrate_rollback_command = SQ_COMMAND_INITIALIZER(
	CommandMigrate, 0,                            // StructureType, bit_field
	"migrate:rollback",                           // command string
	migrate_rollback_options,                     // SqOption pointer array
	migrate_rollback,                             // handle function
	NULL,                                         // parameter string
	"Rollback the last database migration"        // description string
);
/* Macro Expands to
static const SqCommand migrate_rollback_command = {
	.size  = sizeof(CommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate:rollback",
	.entry   = (SqEntry**)migrate_rollback_options,
	.n_entry = sizeof(migrate_rollback_options) / sizeof(SqOption*),
	// SqCommand members
	.handle      = (SqCommandFunc) migrate_rollback,
	.parameter   = NULL,
	.description = "Rollback the last database migration",
};
 */

// ----------------------------------------------------------------------------
//

void  sq_console_add_command_migrate(SqConsole *console)
{
	// migrate
	sq_console_add(console, &migrate_command);
	// migrate:install
	sq_console_add(console, &migrate_install_command);
	// migrate:rollback
	sq_console_add(console, &migrate_rollback_command);
}
