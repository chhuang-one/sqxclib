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

#define OPTION_STEP     option_array +0
#define OPTION_QUIET    option_array +1

// ----------------------------------------------------------------------------
// All options

static const SqOption option_array[] = {
	{SQ_TYPE_BOOL, "step",   offsetof(CommandMigrate, step),
		.default_value = "true",
		.description = "Force the migrations to be run so they can be rolled back individually"},
	{SQ_TYPE_BOOL, "quiet",  offsetof(CommandMigrate, quiet),
		.default_value = "true",
		.description = "Do not output any message"},
};

// ----------------------------------------------------------------------------
// migrate

static void migrate_handle(SqCommand *cmd, SqConsole *console, void *data)
{
}

static const SqOption *migrate_options[] = {
	OPTION_STEP,
};

const SqCommandType SqCommandType_Migrate = {
	.size  = sizeof(CommandMigrate),
	.parse = sq_type_command_parse_option,
	.name  = "migrate",
	.entry   = (SqEntry**) migrate_options,
	.n_entry = sizeof(migrate_options) / sizeof(SqOption*),
	// SqCommandType members
	.handle      = (SqCommandFunc) migrate_handle,
	.parameter   = NULL,
	.description = "Run the database migrations",
};

// ----------------------------------------------------------------------------
// migrate:install

static void migrate_install(SqCommand *cmd, SqConsole *console, void *data)
{
	SqApp *app = data;
	int    code;

	code = sq_migration_install(app->db);
	if (code != SQCODE_OK)
		printf("Can't install migration table\n");
}

static const SqOption *migrate_install_options[] = {
	OPTION_QUIET,
};

const SqCommandType SqCommandType_Migrate_Install = {
	.size  = sizeof(CommandMigrate),
	.parse = sq_type_command_parse_option,
	.name  = "migrate:install",
	.entry   = (SqEntry**)migrate_install_options,
	.n_entry = sizeof(migrate_install_options) / sizeof(SqOption*),
	// SqCommandType members
	.handle      = (SqCommandFunc) migrate_install,
	.parameter   = NULL,
	.description = "Create the migration repository",
};

// ----------------------------------------------------------------------------
//

void  sq_console_add_command_migrate(SqConsole *console)
{
	// migrate
	sq_console_add(console, &SqCommandType_Migrate);
	// migrate:install
	sq_console_add(console, &SqCommandType_Migrate_Install);
}
