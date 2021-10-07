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

#include <sqxclib.h>

#include <SqConsole.h>
#include <SqCommand.h>
#include "CommandMigrate.h"

const char *test_argv[] = {
    "sqtool", "migrate", "--step", "testarg",
};

int main(int argc, char **argv)
{
	SqConsole *console;
	SqCommand *command;

	console = sq_console_new();
	sq_console_add(console, SQ_COMMAND_TYPE_MIGRATE);
	command = sq_console_parse(console, 4, (char**)test_argv, true);

	sq_console_print_help(console, "migrate", "sqtool");
	sq_command_free(command);

	return EXIT_SUCCESS;
}

