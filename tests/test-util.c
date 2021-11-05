/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#include <stdlib.h>
#include <stdio.h>

#include <SqUtil.h>


void test_name_convention()
{
#ifdef SQ_CONFIG_NAMING_CONVENTION
	const char *src;
	char *name;
	int   len;

	len = sq_camel_from_snake(NULL, "custom_name", true);
	name = malloc(len +2 +1);  // + plural character + null-terminated
	sq_camel_from_snake(name, "custom_name", true);
	printf("sq_camel_from_snake() - %s\n", name);
	len = sq_noun2plural(name, name);
	printf("sq_noun2plural() - %d, %s\n", len, name);
	free(name);

	len = sq_snake_from_camel(NULL, "CustomName");
	name = malloc(len +2 +1);  // + plural character + null-terminated
	sq_snake_from_camel(name, "CustomName");
	printf("sq_snake_from_camel() - %s\n", name);
	len = sq_noun2plural(name, name);
	printf("sq_noun2plural() - %d, %s\n", len, name);
	free(name);

	src = "boy";
	len = sq_noun2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	len = sq_noun2plural(name, src);
	printf("sq_noun2plural() - %d, %s\n", len, name);
	len = sq_noun2singular(name, name);
	printf("sq_noun2singular() - %d, %s\n", len, name);
	free(name);

	src = "Company";
	len = sq_noun2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_noun2plural(name, src);
	printf("sq_noun2plural() - %d, %s\n", len, name);
	len = sq_noun2singular(name, name);
	printf("sq_noun2singular() - %d, %s\n", len, name);
	free(name);

	src = "Watch";
	len = sq_noun2plural(NULL, src);
	name = malloc(len +1);  // + null-terminated
	sq_noun2plural(name, src);
	printf("sq_noun2plural() - %d, %s\n", len, name);
	len = sq_noun2singular(name, name);
	printf("sq_noun2singular() - %d, %s\n", len, name);
	free(name);
#endif  // SQ_CONFIG_NAMING_CONVENTION
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
