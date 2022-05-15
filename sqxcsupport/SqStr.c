/*
 *   Copyright (C) 2022 by C.H. Huang
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

#include <ctype.h>    // tolower(), toupper()
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SqStr.h>

// ----------------------------------------------------------------------------
// Naming convention

#define PLURAL_LENGTH    2

// camel case form snake case
int  sq_snake2camel(char *dest_camel, const char *src_snake_name, bool prev_underline)
{
	int  length;

	for (length = 0;  *src_snake_name;  src_snake_name++, length++) {
		if (*src_snake_name == '_') {
			src_snake_name++;
			prev_underline = true;
		}
		if (dest_camel)
			*dest_camel++ = (prev_underline) ? toupper(*src_snake_name) : *src_snake_name;
		prev_underline = false;
	}
	if (dest_camel)
		*dest_camel = 0;
	return length;
}

// snake case from camel case
int  sq_camel2snake(char *dest_snake, const char *src_camel_name)
{
	bool prev_upper = true;
	int  length;

	for (length = 0;  *src_camel_name;  src_camel_name++, length++) {
		if (*src_camel_name < 'A' || *src_camel_name > 'Z') {
			// lower case
			prev_upper = false;
			if (dest_snake)
				*dest_snake++ = *src_camel_name;
		}
		else {
			// upper case
			if (prev_upper == false) {
				// previous character is lower case
				prev_upper = true;
				length++;
				if (dest_snake)
					*dest_snake++ = '_';
			}
			if (dest_snake)
				*dest_snake++ = tolower(*src_camel_name);
		}
	}
	if (dest_snake)
		*dest_snake = 0;
	return length;
}

char* sq_str_camel(const char *snake_name, bool upper_camel_case)
{
    char *camel;

    camel = malloc(sq_snake2camel(NULL, snake_name, upper_camel_case));
    sq_snake2camel(camel, snake_name, upper_camel_case);
    return camel;
}

char* sq_str_snake(const char *camel_name)
{
    char *snake;

    snake = malloc(sq_camel2snake(NULL, camel_name));
    sq_camel2snake(snake, camel_name);
    return snake;
}

// ------------------------------------
//	singular and plural

struct
{
	int16_t offset;
	int16_t length;
	char   *string;
} plural_[4] =
{
	{-1, 3, "ies"},
	{0,  2, "es"},
	{0,  1, "s"},
	{0,  0, ""}
};

// singular to plural
int sq_singular2plural(char *dest, const char *src)
{
	const char *tail;
	int  index;
	int  length;

	length = strlen(src);
	if (length == 0)
		return 0;
	tail = src + length -1;

	index = 2;
	// "s", "x"
	if (*tail == 's' || *tail == 'x')
		index = 1;
	else if (tail != src) {
		// "ch", "sh"
		if (*tail == 'h' && (*(tail-1) == 'c' || *(tail-1) == 's'))
			index = 1;
		// "y"
		if (*tail == 'y' && *(tail-1) != 'o')
			index = 0;
	}
	// "y"
	else if (*tail == 'y')
		index = 0;

	if (dest) {
		if (dest != src)
			strcpy(dest, src);
		dest += length + plural_[index].offset;
		strcpy(dest, plural_[index].string);
	}
	//     length + (change in length)
	return length + plural_[index].length + plural_[index].offset;
}

// plural to singular
int sq_plural2singular(char *dest, const char *src)
{
	int  index;
	int  offset;
	int  length;

	offset = 0;
	length = strlen(src);

	for (index = 0;  index < 3;  index++) {
		if (length < plural_[index].length)
			continue;
		offset = length - plural_[index].length;
		if (strcasecmp(src + offset, plural_[index].string) == 0)
			break;
	}

	if (dest) {
		if (dest != src)
			strcpy(dest, src);
		if (index == 0)
			dest[offset++] = 'y';
		if (index != 3)
			dest[offset] = 0;
	}
	//     length - (change in length)
	return length - plural_[index].length - plural_[index].offset;
}

char* sq_str_singular(const char *plural)
{
    char *singular;

    singular = malloc(sq_plural2singular(NULL, plural));
    sq_plural2singular(singular, plural);
    return singular;
}

char* sq_str_plural(const char *singular)
{
    char *plural;

    plural = malloc(sq_singular2plural(NULL, singular));
    sq_singular2plural(plural, singular);
    return plural;
}

// ------------------------------------
//	table name and type name

char *sq_str_table_name(const char *src_type_name)
{
	char  *table_name;
	int    length;

	if (src_type_name == NULL)
		return NULL;
	// length = snake case name + plural character + null-terminated
	length = sq_camel2snake(NULL, src_type_name) +PLURAL_LENGTH +1;
	table_name = malloc(length);
	sq_camel2snake(table_name, src_type_name);
	sq_singular2plural(table_name, table_name);
	return table_name;
}

char *sq_str_type_name(const char *src_table_name)
{
	char  *type_name;
	int    length;

	if (src_table_name == NULL)
		return NULL;
	// length = snake case name + null-terminated
	length = sq_snake2camel(NULL, src_table_name, true) +1;
	type_name = malloc(length);
	sq_snake2camel(type_name, src_table_name, true);
	sq_plural2singular(type_name, type_name);
	return type_name;
}
