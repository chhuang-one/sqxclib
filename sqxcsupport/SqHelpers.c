/*
 *   Copyright (C) 2022-2025 by C.H. Huang
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
#include <ctype.h>        // tolower(), toupper()
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SqHelpers.h>

#ifdef _MSC_VER
#define strcasecmp   _stricmp
#endif

// ----------------------------------------------------------------------------
// camel case and snake case conversion

// camel case form snake case
int   sq_snake2camel(char *dest_camel, const char *src_snake_name, bool prev_underline)
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
int   sq_camel2snake(char *dest_snake, const char *src_camel_name)
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

    camel = malloc(sq_snake2camel(NULL, snake_name, upper_camel_case) +1);    // + '\0'
    sq_snake2camel(camel, snake_name, upper_camel_case);
    return camel;
}

char* sq_str_snake(const char *camel_name)
{
    char *snake;

    snake = malloc(sq_camel2snake(NULL, camel_name) +1);    // + '\0'
    sq_camel2snake(snake, camel_name);
    return snake;
}

// ----------------------------------------------------------------------------
// singular and plural conversion

struct
{
	int16_t offset;
	int16_t length;
	char   *str;
} plural_[4] =
{
	{-1, 3, "ies"},    // To change word to plural form by replacing "y" to "ies"
	{0,  2, "es"},     // To change word to plural form by adding "es"
	{0,  1, "s"},      // To change word to plural form by adding "s"
	{0,  0, ""}        // sq_plural2singular() need this element
};

// singular to plural
int   sq_singular2plural(char *dest, const char *src)
{
	char tail[2];   // last 2 characters in src string
	int  index;
	int  length;

	length = (int)strlen(src);
	if (length == 0)
		return 0;
	tail[1] = tolower(src[length-1]);

	index = 2;
	// "s", "x"
	if (tail[1] == 's' || tail[1] == 'x')
		index = 1;
	else if (length > 2) {
		tail[0] = tolower(src[length-2]);
		// "ch", "sh"
		if (tail[1] == 'h' && (tail[0] == 'c' || tail[0] == 's'))
			index = 1;
		// "y"
		if (tail[1] == 'y' &&  tail[0] != 'o')
			index = 0;
	}
	// "y"
	else if (tail[1] == 'y')
		index = 0;

	if (dest) {
		if (dest != src)
			strcpy(dest, src);
		dest += length + plural_[index].offset;
		strcpy(dest, plural_[index].str);
		// to upper case
		if (isupper(src[length-1]))
			for (;  *dest;  dest++)
				*dest = toupper(*dest);
	}
	//     length + (change in length)
	return length + plural_[index].length + plural_[index].offset;
}

// plural to singular
int   sq_plural2singular(char *dest, const char *src)
{
	int  index;
	int  diff_pos;
	int  length;

	length = (int)strlen(src);

	for (index = 0;  ;  index++) {
		diff_pos = length - plural_[index].length;
		if (length < plural_[index].length)
			continue;
		if (strcasecmp(src + diff_pos, plural_[index].str) == 0)
			break;
	}
	//     = length - plural_[index].length - plural_[index].offset;
	length = diff_pos - plural_[index].offset;

	if (dest) {
		if (dest != src)
			strncpy(dest, src, length);
		if (index == 0) {
			dest[diff_pos] = isupper(src[diff_pos]) ? 'Y' : 'y';
			diff_pos++;
		}
		dest[diff_pos] = 0;
	}

	return length;
}

char* sq_str_singular(const char *plural)
{
    char *singular;

    singular = malloc(sq_plural2singular(NULL, plural) +1);    // + '\0'
    sq_plural2singular(singular, plural);
    return singular;
}

char* sq_str_plural(const char *singular)
{
    char *plural;

    plural = malloc(sq_singular2plural(NULL, singular) +1);    // + '\0'
    sq_singular2plural(plural, singular);
    return plural;
}

// ----------------------------------------------------------------------------
// table name and type name

char *sq_str_table_name(const char *src_type_name)
{
	char  *plural_noun;
	char  *snake_case;

	if (src_type_name == NULL)
		return NULL;
	snake_case  = sq_str_snake(src_type_name);
	plural_noun = sq_str_plural(snake_case);
	free(snake_case);
	return plural_noun;
}

char *sq_str_type_name(const char *src_table_name)
{
	char  *singular_noun;
	char  *camel_case;

	if (src_table_name == NULL)
		return NULL;
	camel_case    = sq_str_camel(src_table_name, true);
	singular_noun = sq_str_singular(camel_case);
	free(camel_case);
	return singular_noun;
}
