/*
 *   Copyright (C) 2021-2024 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
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
#include <SqCommandMigrate.h>

// ----------------------------------------------------------------------------
static const SqOption option_database = {
	SQ_TYPE_STR,     "database",      offsetof(SqCommandMigrate, database),
		.shortcut = NULL,
		.default_value = NULL,
		.value_description = "[=DATABASE]",
		.description = "The database connection to use.",
};

#define SQ_OPTION_COMMAND_MIGRATE_DATABASE    (&option_database)

// ----------------------------------------------------------------------------
// migrate

static void migrate(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqCommandMigrate *value = (SqCommandMigrate*)commandValue;
	SqApp *app = data;
	int    code;

	if (value->help) {
		sq_console_print_help(console, value->type);
		return;
	}

	// open database
	if (sq_app_open_database((SqApp*)app, value->database) != SQCODE_OK) {
		puts("Can't open database");
		return;
	}
	// make current schema in database
	sq_app_make_schema(app, 0);

	code = sq_app_migrate(app, value->step);
	if (code)
		printf("Can't migrate\n");

	// close database
	sq_app_close_database(app);
}

static const SqOption *migrate_options[] = {
	// --- SqCommandCommon options ---
	SQ_OPTION_COMMAND_COMMON_HELP,
//	SQ_OPTION_COMMAND_COMMON_QUIET,

	// --- SqCommandMigrate options ---
	SQ_OPTION_COMMAND_MIGRATE_DATABASE,

	&(SqOption) {SQ_TYPE_INT, "step",   offsetof(SqCommandMigrate, step),
		.default_value = "1",
		.description = "Force the migrations to be run so they can be rolled back individually"},
};

static const SqCommand migrate_command = SQ_COMMAND_INITIALIZER(
	SqCommandMigrate,                              // StructureType
	0,                                             // bit_field
	"migrate",                                     // command string
	migrate_options,                               // pointer array of SqOption
	migrate,                                       // handle function
	NULL,                                          // parameter string
	"Run the database migrations"                  // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand migrate_command = {
	// --- SqType members ---
	.size  = sizeof(SqCommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate",
	.entry   = (SqEntry**) migrate_options,
	.n_entry = sizeof(migrate_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) migrate,
	.parameter   = NULL,
	.description = "Run the database migrations",
};
 */

// ----------------------------------------------------------------------------
// migrate:install

static void migrate_install(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqCommandMigrate *value = (SqCommandMigrate*)commandValue;
	SqApp *app = data;
	int    code;

	if (value->help) {
		sq_console_print_help(console, value->type);
		return;
	}

	// open database
	if (sq_app_open_database((SqApp*)app, value->database) != SQCODE_OK) {
		puts("Can't open database");
		return;
	}

	code = sq_migration_install(app->db);
	if (code != SQCODE_OK)
		printf("Can't install migration table\n");

	// close database
	sq_app_close_database(app);
}

static const SqOption *migrate_install_options[] = {
	// --- SqCommandCommon options ---
	SQ_OPTION_COMMAND_COMMON_HELP,
//	SQ_OPTION_COMMAND_COMMON_QUIET,

	// --- SqCommandMigrate options ---
	SQ_OPTION_COMMAND_MIGRATE_DATABASE,
};

static const SqCommand migrate_install_command = SQ_COMMAND_INITIALIZER(
	SqCommandMigrate,                             // StructureType
	0,                                            // bit_field
	"migrate:install",                            // command string
	migrate_install_options,                      // pointer array of SqOption
	migrate_install,                              // handle function
	NULL,                                         // parameter string
	"Create the migration repository"             // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand migrate_install_command = {
	// --- SqType members ---
	.size  = sizeof(SqCommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate:install",
	.entry   = (SqEntry**)migrate_install_options,
	.n_entry = sizeof(migrate_install_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) migrate_install,
	.parameter   = NULL,
	.description = "Create the migration repository",
};
 */

// ----------------------------------------------------------------------------
// migrate:rollback

static void migrate_rollback(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqCommandMigrate *value = (SqCommandMigrate*)commandValue;
	SqApp *app = data;
	int    code;

	if (value->help) {
		sq_console_print_help(console, value->type);
		return;
	}

	// open database
	if (sq_app_open_database((SqApp*)app, value->database) != SQCODE_OK) {
		puts("Can't open database");
		return;
	}
	// make current schema in database
	sq_app_make_schema(app, 0);

	code = sq_app_rollback(app, value->step);
	if (code != SQCODE_OK)
		printf("Can't rollback\n");

	// close database
	sq_app_close_database(app);
}

static const SqOption *migrate_rollback_options[] = {
	// --- SqCommandCommon options ---
	SQ_OPTION_COMMAND_COMMON_HELP,
//	SQ_OPTION_COMMAND_COMMON_QUIET,

	// --- SqCommandMigrate options ---
	SQ_OPTION_COMMAND_MIGRATE_DATABASE,

	&(SqOption) {SQ_TYPE_INT,  "step",   offsetof(SqCommandMigrate, step),
		.default_value = "0",
		.value_description = "[=STEP]",
		.description = "The number of migrations to be reverted"},
};

static const SqCommand migrate_rollback_command = SQ_COMMAND_INITIALIZER(
	SqCommandMigrate,                             // StructureType
	0,                                            // bit_field
	"migrate:rollback",                           // command string
	migrate_rollback_options,                     // pointer array of SqOption
	migrate_rollback,                             // handle function
	NULL,                                         // parameter string
	"Rollback the last database migration"        // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand migrate_rollback_command = {
	// --- SqType members ---
	.size  = sizeof(SqCommandMigrate),
	.parse = sq_command_parse_option,
	.name  = "migrate:rollback",
	.entry   = (SqEntry**)migrate_rollback_options,
	.n_entry = sizeof(migrate_rollback_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) migrate_rollback,
	.parameter   = NULL,
	.description = "Rollback the last database migration",
};
 */

// ----------------------------------------------------------------------------
// migrate:reset

static void migrate_reset(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqCommandMigrate *value = (SqCommandMigrate*)commandValue;
	SqApp *app = data;
	int    code;

	if (value->help) {
		sq_console_print_help(console, value->type);
		return;
	}

	// open database
	if (sq_app_open_database((SqApp*)app, value->database) != SQCODE_OK) {
		puts("Can't open database");
		return;
	}
	// make current schema in database
	sq_app_make_schema(app, 0);

	code = sq_app_rollback(app, app->db->version);
	if (code != SQCODE_OK)
		printf("Can't reset\n");

	// close database
	sq_app_close_database(app);
}

static const SqOption *migrate_reset_options[] = {
	// --- SqCommandCommon options ---
	SQ_OPTION_COMMAND_COMMON_HELP,
//	SQ_OPTION_COMMAND_COMMON_QUIET,

	// --- SqCommandMigrate options ---
	SQ_OPTION_COMMAND_MIGRATE_DATABASE,
};

static const SqCommand migrate_reset_command = SQ_COMMAND_INITIALIZER(
	SqCommandMigrate,                             // StructureType
	0,                                            // bit_field
	"migrate:reset",                              // command string
	migrate_reset_options,                        // pointer array of SqOption
	migrate_reset,                                // handle function
	NULL,                                         // parameter string
	"Rollback all database migrations"            // description string
);

// ----------------------------------------------------------------------------
// migrate:refresh

static void migrate_refresh(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqCommandMigrate *value = (SqCommandMigrate*)commandValue;
	SqApp *app = data;
	int    code;

	if (value->help) {
		sq_console_print_help(console, value->type);
		return;
	}

	// open database
	if (sq_app_open_database((SqApp*)app, value->database) != SQCODE_OK) {
		puts("Can't open database");
		return;
	}
	// make current schema in database
	sq_app_make_schema(app, 0);

	code = sq_app_rollback(app, app->db->version);
	if (code != SQCODE_OK)
		printf("Can't refresh\n");

	// synchronize schema to database and update schema in 'storage'
	// This Mainly used by SQLite
	sq_storage_migrate(app->storage, NULL);

	code = sq_app_migrate(app, 0);
	if (code)
		printf("Can't refresh\n");

	// close database
	sq_app_close_database(app);
}

static const SqOption *migrate_refresh_options[] = {
	// --- SqCommandCommon options ---
	SQ_OPTION_COMMAND_COMMON_HELP,
//	SQ_OPTION_COMMAND_COMMON_QUIET,

	// --- SqCommandMigrate options ---
	SQ_OPTION_COMMAND_MIGRATE_DATABASE,
};

static const SqCommand migrate_refresh_command = SQ_COMMAND_INITIALIZER(
	SqCommandMigrate,                             // StructureType
	0,                                            // bit_field
	"migrate:refresh",                            // command string
	migrate_refresh_options,                      // pointer array of SqOption
	migrate_refresh,                              // handle function
	NULL,                                         // parameter string
	"Reset and re-run all migrations"             // description string
);

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
	// migrate:reset
	sq_console_add(console, &migrate_reset_command);
	// migrate:refresh
	sq_console_add(console, &migrate_refresh_command);
}
