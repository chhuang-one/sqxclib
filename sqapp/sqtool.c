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
#include <sqxclib.h>

#include <SqAppTool.h>
#include <CommandMigrate.h>

//#define TEST_ARGV

#ifdef TEST_ARGV
const char *test_argv[] = {
//	"sqtool", "migrate", "--step", "testarg",
//	"sqtool", "migrate:install", "testarg",
//	"sqtool", "migrate:rollback", "--step", "testarg",
	"sqtool", "make:migration", "create_companies_table",
//	"sqtool",
};
const int   test_argc = sizeof(test_argv) / sizeof(char*);
#endif

int main(int argc, char **argv)
{
	SqAppTool *apptool;
	SqConsole *console;
	SqCommandValue *cmd_value;

	apptool = malloc(sizeof(SqAppTool));
	sq_app_tool_init(apptool, "sqtool");
	if (sq_app_open_database((SqApp*)apptool, NULL) != SQCODE_OK) {
		puts("Can't open database");
		return EXIT_FAILURE;
	}

#ifdef TEST_ARGV
	argc = test_argc;
	argv = (char**)test_argv;
#endif

	console = apptool->console;
	if (argc > 1) {
		cmd_value = sq_console_parse(console, argc, (char**)argv, true);
		if (cmd_value == NULL)
			puts("unknown command");
		else {
			cmd_value->type->handle(cmd_value, console, apptool);
			sq_command_value_free(cmd_value);
		}
	}
	else {
		sq_console_print_list(console, NULL);
	}

	sq_storage_close(apptool->storage);
	return EXIT_SUCCESS;
}

