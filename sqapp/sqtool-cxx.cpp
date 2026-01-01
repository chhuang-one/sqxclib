/*
 *   Copyright (C) 2021-2026 by C.H. Huang
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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>             // printf()

#include <sqxc/app/SqAppTool.h>

#define TEST_ARGV    0

#if TEST_ARGV
const char *test_argv[] = {
	"sqtool-cxx", "migrate",
//	"sqtool-cxx", "migrate", "--step", "testarg",
//	"sqtool-cxx", "migrate:install", "testarg",
//	"sqtool-cxx", "migrate:rollback", "--step", "testarg",
//	"sqtool-cxx", "make:migration", "create_companies_table",
//	"sqtool-cxx", "make:migration", "--table=companies", "alter_companies_table",
//	"sqtool-cxx",
};
const int   test_argc = sizeof(test_argv) / sizeof(char*);
#endif

int  main(int argc, char **argv)
{
	Sq::AppTool *apptool;
	char        *program_name;

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
		printf("\n" "* Warning: %s is deprecated. Please use sqtool or sqtool-cxx instead." "\n", program_name);
	if (strstr(program_name, "cpp"))
		printf("\n" "* Warning: %s is deprecated. Please use sqtool or sqtool-cxx instead." "\n", program_name);

	// create Sq::AppTool with 'program_name'
	apptool = new Sq::AppTool(program_name);

	// if 'program_name' contains "cxx" or "cpp", program outputs cpp files.
	if (strstr(program_name, "cxx") || strstr(program_name, "cpp")) {
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

	apptool->run(argc, argv);

	delete apptool;

	return EXIT_SUCCESS;
}
