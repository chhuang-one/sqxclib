/*
 *   Copyright (C) 2020 by C.H. Huang
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

#include <ctype.h>    // tolower(), toupper()
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>   // malloc()
#include <string.h>

#include <SqUtil.h>

#ifdef SQ_HAVE_NAMING_CONVENTION

// ----------------------------------------------------------------------------
// camel case and snake case

int  sq_camel_from_snake(char* camel_name, const char* snake_name, bool prev_underline)
{
	int  length;

	for (length = 0;  *snake_name;  snake_name++, length++) {
		if (*snake_name == '_') {
			snake_name++;
			prev_underline = true;
		}
		if (camel_name)
			*camel_name++ = (prev_underline) ? toupper(*snake_name) : *snake_name;
		prev_underline = false;
	}
	if (camel_name)
		*camel_name = 0;
	return length;
}

int  sq_snake_from_camel(char* snake_name, const char* camel_name)
{
	bool prev_upper = true;
	int  length;

	for (length = 0;  *camel_name;  camel_name++, length++) {
		if (*camel_name < 'A' || *camel_name > 'Z') {
			// lower case
			prev_upper = false;
			if (snake_name)
				*snake_name++ = *camel_name;
		}
		else {
			// upper case
			if (prev_upper == false) {
				// previous character is lower case
				prev_upper = true;
				length++;
				if (snake_name)
					*snake_name++ = '_';
			}
			if (snake_name)
				*snake_name++ = tolower(*camel_name);
		}
	}
	if (snake_name)
		*snake_name = 0;
	return length;
}

// ----------------------------------------------------------------------------
//	singular and plural

struct {
	int16_t offset;
	int16_t length;
	char*   string;
} plural_[4] = {
	{-1, 3, "ies"},
	{0,  2, "es"},
	{0,  1, "s"},
	{0,  0, ""}
};

// singular to plural
int sq_noun2plural(char* dest, const char* src)
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
int sq_noun2singular(char* dest, const char* src)
{
	int  index;
	int  offset;
	int  length;

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

// ----------------------------------------------------------------------------
//	table name and type name

char* sq_name2table(const char* type_name)
{
	char*  table_name;
	int    length;

	// length = snake case name + plural character + null-terminated
	length = sq_snake_from_camel(NULL, type_name) +2 +1;
	table_name = malloc(length);
	sq_snake_from_camel(table_name, type_name);
	sq_noun2plural(table_name, table_name);
	return table_name;
}

char* sq_name2type(const char* table_name)
{
	char*  type_name;
	int    length;

	// length = snake case name + null-terminated
	length = sq_camel_from_snake(NULL, table_name, true) +1;
	type_name = malloc(length);
	sq_camel_from_snake(type_name, table_name, true);
	sq_noun2singular(type_name, type_name);
	return type_name;
}

#endif  // SQ_HAVE_NAMING_CONVENTION
