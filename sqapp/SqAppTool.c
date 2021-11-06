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

#include <SqAppTool.h>
#include <CommonOptions.h>

// ----------------------------------------------------------------------------
// CommandList

typedef struct CommandList        CommandList;

/*	CommandList: migrate command

	SqCommandValue
	|
	`--- CommandCommon    (CommonOptions.h)
	     |
	     `--- CommandList
 */

#ifdef __cplusplus
struct CommandList : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
struct CommandList
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                   // <-- 2. inherit member variable
/*	// ------ SqCommandValue members ------
	const SqCommand  *type;

	// shortcuts is an array that sorted by SqOption.shortcut
	SqPtrArray     shortcuts;
	SqPtrArray     arguments;
 */

	COMMON_OPTION_MEMBERS;
/*	// ------ CommandCommon members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */

	// ------ CommandList members ------        // <-- 3. Add variable and non-virtual function in derived struct.
};

// ----------------------------------------------------------------------------
// list

static void list(SqCommandValue *cmd_value, SqConsole *console, void *data)
{
//	SqAppTool *app = data;
//	console = app->console;

	sq_console_print_list(console, NULL);
}

static const SqOption *list_options[] = {
};

static const SqCommand list_command = SQ_COMMAND_INITIALIZER(
	CommandList, 0,                                // StructureType, bit_field
	"list",                                        // command string
	list_options,                                  // SqOption pointer array
	list,                                          // handle function
	NULL,                                          // parameter string
	"lists all commands"                           // description string
);
/* Macro Expands to
static const SqCommand list_command = {
	// --- SqType members ---
	.size  = sizeof(CommandList),
	.parse = sq_command_parse_option,
	.name  = "list",
	.entry   = (SqEntry**) list_options,
	.n_entry = sizeof(list_options) / sizeof(SqOption*),
	.bit_field = 0,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) list,
	.parameter   = NULL,
	.description = "lists all commands",
};
 */

// ----------------------------------------------------------------------------
// SqAppTool functions

void    sq_app_tool_init(SqAppTool *app, const char *program_name)
{
	sq_app_init((SqApp*)app);
	// console
	app->console = sq_console_new();
	app->console->program_name = strdup(program_name);
	sq_console_add(app->console, &list_command);
	sq_console_add_command_migrate(app->console);
}

void    sq_app_tool_final(SqAppTool *app)
{
	sq_console_free(app->console);
	sq_app_final((SqApp*)app);
}
