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

#include "CommandMigrate.h"

static SqOption *migrate_options[] = {
	&(SqOption) {SQ_TYPE_BOOL, "step",  offsetof(CommandMigrate, step),
			.default_value = "true",
			.description = "Force the migrations to be run so they can be rolled back individually",
	}
};

static void migrate_init(CommandMigrate *cmd)
{
	cmd->step = false;
}

static void migrate_final(CommandMigrate *cmd)
{
}

static void migrate_handle(SqCommand *cmd, SqConsole *console, void *data)
{
}

const SqCommandType SqCommandType_Migrate_ = SQ_COMMAND_TYPE_INITIALIZER(
		CommandMigrate, 0, migrate,
		NULL,
		"Run the database migrations");

/*	// Macro expand to
const SqCommandType SqCommandType_Migrate_ = {
	.size  = sizeof(CommandMigrate),
	.init  = (SqTypeFunc) migrate_init,
	.final = (SqTypeFunc) migrate_final,
	.parse = sq_type_command_parse_option,
	.write = NULL,
	.name  = "migrate",
	.entry   = (SqEntry**) migrate_options,
	.n_entry = sizeof(migrate_options) / sizeof(SqOption*),
	.bit_field = 0,
	.ref_count = 0,
	.handle      = (SqCommandFunc) migrate_handle,
	.parameter   = NULL,
	.description = "Run the database migrations",
};
 */
