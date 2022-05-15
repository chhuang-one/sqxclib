/*
 *   Copyright (C) 2020-2022 by C.H. Huang
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SqUtil.h>
#include <SqStr.h>     // sqxcsupport


void test_name_convention()
{
	const char *src;
	char *name;
	int   len;

	len = sq_snake2camel(NULL, "custom_name", true);
	name = malloc(len +2 +1);  // + plural character + null-terminated
	sq_snake2camel(name, "custom_name", true);
	printf("sq_snake2camel() - %s\n", name);
	assert(strcmp(name, "CustomName") == 0);

	len = sq_singular2plural(name, name);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "CustomNames") == 0);
	free(name);


	len = sq_camel2snake(NULL, "CustomName");
	name = malloc(len +2 +1);  // + plural character + null-terminated
	sq_camel2snake(name, "CustomName");
	printf("sq_camel2snake() - %s\n", name);
	assert(strcmp(name, "custom_name") == 0);

	len = sq_singular2plural(name, name);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "custom_names") == 0);
	free(name);


	src = "boy";
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	len = sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "boys") == 0);

	len = sq_plural2singular(name, name);
	printf("sq_plural2singular() - %d, %s\n", len, name);
	assert(strcmp(name, "boy") == 0);
	free(name);


	src = "Company";
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "Companies") == 0);

	len = sq_plural2singular(name, name);
	printf("sq_plural2singular() - %d, %s\n", len, name);
	assert(strcmp(name, "Company") == 0);
	free(name);


	src = "Watch";
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "Watches") == 0);

	len = sq_plural2singular(name, name);
	printf("sq_plural2singular() - %d, %s\n", len, name);
	assert(strcmp(name, "Watch") == 0);
	free(name);
}

void test_time_string()
{
	char    *str;
	time_t   time;

	time = sq_time_from_string("2002-11-10T15:23:59");
//	time = sq_time_from_string("2012/10/18 17:53:39");
//	time = sq_time_from_string("21:13:30");
	str  = sq_time_to_string(time, 0);
	if (str)
		puts(str);
}

void test_util()
{
	test_name_convention();
	test_time_string();
}

// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	test_util();
	return EXIT_SUCCESS;
}
