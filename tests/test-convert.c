/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#include <sqxc/SqConvert.h>
#include <sqxc/support/SqHelpers.h>        // sqxcsupport


void test_name_convention()
{
	const char *src;
	char *name;
	char *noun;
	int   len;

	src = "custom_name";
	// camel case name
	len = sq_snake2camel(NULL, src, true);
	name = malloc(len +1);  // + null-terminated
	sq_snake2camel(name, src, true);
	printf("sq_snake2camel() - %s\n", name);
	assert(strcmp(name, "CustomName") == 0);
	// plural noun
	len = sq_singular2plural(NULL, name);
	noun = malloc(len +1);  // + null-terminated
	sq_singular2plural(noun, name);
	printf("sq_singular2plural() - %d, %s\n", len, noun);
	assert(strcmp(noun, "CustomNames") == 0);
	// free
	free(name);
	free(noun);
 
	src = "CustomName";
	// snake case name
	len = sq_camel2snake(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_camel2snake(name, src);
	printf("sq_camel2snake() - %s\n", name);
	assert(strcmp(name, "custom_name") == 0);
	// plural noun
	len = sq_singular2plural(NULL, name);
	noun = malloc(len +1);  // + null-terminated
	sq_singular2plural(noun, name);
	printf("sq_singular2plural() - %d, %s\n", len, noun);
	assert(strcmp(noun, "custom_names") == 0);
	// free
	free(name);
	free(noun);

	src = "boy";
	// plural noun
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "boys") == 0);
	// singular noun
	len = sq_plural2singular(NULL, name);
	noun = malloc(len +1);  // + null-terminated
	sq_plural2singular(noun, name);
	printf("sq_plural2singular() - %d, %s\n", len, noun);
	assert(strcmp(noun, "boy") == 0);
	// free
	free(name);
	free(noun);

	src = "COMPANY";
	// plural noun
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "COMPANIES") == 0);
	// singular noun
	len = sq_plural2singular(NULL, name);
	noun = malloc(len +1);  // + null-terminated
	sq_plural2singular(noun, name);
	printf("sq_plural2singular() - %d, %s\n", len, noun);
	assert(strcmp(noun, "COMPANY") == 0);
	// free
	free(name);
	free(noun);

	src = "Watch";
	// plural noun
	len = sq_singular2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_singular2plural(name, src);
	printf("sq_singular2plural() - %d, %s\n", len, name);
	assert(strcmp(name, "Watches") == 0);
	// singular noun
	len = sq_plural2singular(NULL, name);
	noun = malloc(len +1);  // + null-terminated
	sq_plural2singular(noun, name);
	printf("sq_plural2singular() - %d, %s\n", len, noun);
	assert(strcmp(noun, "Watch") == 0);
	// free
	free(name);
	free(noun);
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

// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	test_name_convention();
	test_time_string();
	return EXIT_SUCCESS;
}
