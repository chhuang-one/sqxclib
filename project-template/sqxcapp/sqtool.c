/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

#include <stdio.h>        // printf()

#include <SqAppTool.h>

#define TEST_ARGV    0

#if TEST_ARGV
const char *test_argv[] = {
	"sqtool", "migrate",
//	"sqtool", "migrate", "--step", "testarg",
//	"sqtool", "migrate:install", "testarg",
//	"sqtool", "migrate:rollback", "--step", "testarg",
//	"sqtool", "make:migration", "create_companies_table",
//	"sqtool", "make:migration", "--table=companies", "alter_companies_table",
//	"sqtool",
};
const int   test_argc = sizeof(test_argv) / sizeof(char*);
#endif

int  main(int argc, char **argv)
{
	SqAppTool *apptool;
	char      *program_name;

	// get 'program_name'
#if defined(_WIN32) || defined(_WIN64)
	program_name = strrchr(argv[0], '\\');
#else
	program_name = strrchr(argv[0], '/');
#endif
	if (program_name)
		program_name++;
	else
		program_name = argv[0];

	// check program_name
	if (strncmp(program_name, "sqxc", 4) == 0)
		printf("\n" "* Warning: %s is deprecated. Please use sqtool or sqtool-cpp instead." "\n", program_name);

	// create SqAppTool with 'program_name'
	apptool = malloc(sizeof(SqAppTool));
	// 'SQ_APP_DEFAULT' has database settings and migration data for user application.
	sq_app_tool_init(apptool, program_name, SQ_APP_DEFAULT);

	// if 'program_name' contains "cpp" or "cxx", program outputs cpp files.
	if (strstr(program_name, "cpp") || strstr(program_name, "cxx")) {
		apptool->template_extension = ".cpp.txt";
		apptool->migrations_files_name = "migrations-files-cxx.cpp";
	}
	else {
		apptool->template_extension = ".c.txt";
		apptool->migrations_files_name = "migrations-files.c";
	}

#if TEST_ARGV
	argc = test_argc;
	argv = (char**)test_argv;
#endif

	sq_app_tool_run(apptool, argc, argv);

	sq_app_tool_final(apptool);
	free(apptool);

	return EXIT_SUCCESS;
}
