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
#include <SqAppTool.h>
#include <CommandMake.h>

// ----------------------------------------------------------------------------
// make:migration

static void make_migration(SqCommandValue *cmd_value, SqConsole *console, void *data)
{
	CommandMake *value = (CommandMake*)cmd_value;
	SqAppTool   *app = data;
	const char  *template_file = NULL;
	int          code;

	if (value->help || value->arguments.length == 0) {
		sq_console_print_help(console, value->type);
		return;
	}

	if (value->table_to_migrate) {
		template_file = "migration-alter";
		sq_pairs_add(&app->pairs, "table_name", value->table_to_migrate);
	}
	if (value->table_to_create) {
		template_file = "migration-create";
		if (value->table_to_migrate == NULL)
			sq_pairs_add(&app->pairs, "table_name", value->table_to_create);
	}
	if (template_file == NULL)
		template_file = "migration-create";    // default

	code = sq_app_tool_make_migration(app, template_file,
			value->arguments.data[0], &app->pairs);
	if (code != SQCODE_OK)
		puts("make:migration error");

	sq_pairs_steal(&app->pairs, "table_name");
}

static const SqOption *make_migration_options[] = {
	// --- CommandCommon options ---
	COMMAND_COMMON_OPTION_HELP,
//	COMMAND_COMMON_OPTION_QUIET,

	// --- CommandMake options ---
	&(SqOption) {SQ_TYPE_STRING, "create",  offsetof(CommandMake, table_to_create),
		.value_description = "[=CREATE]",
		.description = "The table to be created"},
	&(SqOption) {SQ_TYPE_STRING, "table",   offsetof(CommandMake, table_to_migrate),
		.value_description = "[=TABLE]",
		.description = "The table to migrate"},
};

static const SqCommand make_migration_command = SQ_COMMAND_INITIALIZER(
	CommandMake, 0,                                // StructureType, bit_field
	"make:migration",                              // command string
	make_migration_options,                        // SqOption pointer array
	make_migration,                                // handle function
	"<name>",                                      // parameter string
	"Create a new migration file"                  // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand make_migration_command = {
	// --- SqType members ---
	.size  = sizeof(CommandMake),
	.parse = sq_command_parse_option,
	.name  = "make:migration",
	.entry   = (SqEntry**) make_migration_options,
	.n_entry = sizeof(make_migration_options) / sizeof(SqOption*),
	.bit_field = 0,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) make_migration,
	.parameter   = "<name>",
	.description = "Create a new migration file",
};
 */

void  sq_console_add_command_make(SqConsole *console)
{
	// make:migration
	sq_console_add(console, &make_migration_command);
}
