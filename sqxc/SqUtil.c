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

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#define strtoll		_strtoi64
#define snprintf	_snprintf
#endif

#include <ctype.h>    // tolower(), toupper()
#include <stdio.h>    // sscanf()
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>   // malloc()
#include <string.h>

#include <SqUtil.h>

/* ----------------------------------------------------------------------------
	Time string converter

	YYYY/MM/DD HH:MM:SS

	SQLite time string can be in any of the following formats:

	YYYY-MM-DD
	YYYY-MM-DD HH:MM
	YYYY-MM-DD HH:MM:SS
	YYYY-MM-DD HH:MM:SS.SSS
	YYYY-MM-DDTHH:MM
	YYYY-MM-DDTHH:MM:SS
	YYYY-MM-DDTHH:MM:SS.SSS
	HH:MM
	HH:MM:SS
	HH:MM:SS.SSS
	now                           // 
	DDDDDDDDDD                    // Julian day number expressed as a floating point value.
 */

static int sq_tm_hms(struct tm *timeinfo, const char *timestr)
{
	int  hour, minute, second = 0;
	int  result;

	result = sscanf(timestr, "%d:%d:%d", &hour, &minute, &second);
	if (result < 2)
		return 0;
	timeinfo->tm_hour = hour;
	timeinfo->tm_min  = minute;
	timeinfo->tm_sec  = second;
	return result;
}

static int sq_tm_ymd(struct tm *timeinfo, const char *timestr)
{
	int  year, month, day;

	if (sscanf(timestr, "%d-%d-%d", &year, &month, &day) != 3) {
		if (sscanf(timestr, "%d/%d/%d", &year, &month, &day) != 3)
			return 0;
	}
	timeinfo->tm_year = year  - 1900;
	timeinfo->tm_mon  = month - 1;
	timeinfo->tm_mday = day;
	return 3;
}

// return UTC time
// return -1 if error
time_t  sq_time_from_string(const char *timestr)
{
	struct tm  timeinfo = {0};
	char  *cur;

	cur = strpbrk(timestr, " T:n.");
	if (cur == NULL)
		cur = strpbrk(timestr, "-/");    // YYYY-MM-DD  or  YYYY/MM/DD
	if (cur == NULL)
		return -1;

	switch(cur[0]) {
	case ' ':
	case 'T':
		if (sq_tm_hms(&timeinfo, cur +1) == 0)
			return -1;
	case '-':
	case '/':
		if (sq_tm_ymd(&timeinfo, timestr) == 0)
			return -1;
		break;

	case ':':    // HH:MM  or  HH:MM:SS
		// 2000-01-01
		timeinfo.tm_year = 2000 - 1900;
		timeinfo.tm_mon  = 1    - 1;
		timeinfo.tm_mday = 1;
		if (sq_tm_hms(&timeinfo, timestr) == 0)
			return -1;
		break;

	case 'n':    // now
		return -1;

	case '.':    // Julian day number
	default:
		return -1;
	}

	return mktime(&timeinfo);
}
// return NULL if error
char   *sq_time_to_string(time_t timeraw)
{
	struct tm  *timeinfo;
	char       *timestr;

	timeinfo = localtime((time_t*) &timeraw);
//	timeinfo = gmtime((time_t*) &timeraw);
	if (timeinfo == NULL)
		return NULL;

	timestr = malloc(32);
	// output format : "2013-02-05 21:25:15"
	snprintf(timestr, 32, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
	         timeinfo->tm_year + 1900,
	         timeinfo->tm_mon  + 1,
	         timeinfo->tm_mday,
	         timeinfo->tm_hour,
	         timeinfo->tm_min,
	         timeinfo->tm_sec);
	return timestr;
}

// ----------------------------------------------------------------------------

// C string to SQL string
int  sq_str_c2sql(char *sql_string, const char *c_string)
{
	int    length = 0;

	if (*c_string) {
		*sql_string++ = '\'';

		for (;  *c_string;  c_string++) {
			if (*c_string == '\'') {
				if (sql_string)
					*sql_string++ = '\'';
				length++;
			}
			if (sql_string)
				*sql_string++ = *c_string;
			length++;
		};

		*sql_string++ = '\'';
		*sql_string   = 0;
		length += 2;
	}

	return length;
}

// SQL string to C string
int  sq_str_sql2c(char *c_string, char *sql_string)
{
	int    length = 0;
	int    prev_quote = 0;

	for (;  *sql_string;  sql_string++) {
		if (*sql_string == '\'' && prev_quote == 0) {
			prev_quote = 1;
			continue;
		}
		prev_quote = 0;
		if (c_string)
			*c_string++ = *sql_string;
		length++;
	}
	if (c_string)
		*c_string++ = 0;
	return length;
}

#ifdef SQ_CONFIG_NAMING_CONVENTION

// ----------------------------------------------------------------------------
// Naming convention

// camel case form snake case
int  sq_camel_from_snake(char *camel_name, const char *snake_name, bool prev_underline)
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

// snake case from camel case
int  sq_snake_from_camel(char *snake_name, const char *camel_name)
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
int sq_noun2plural(char *dest, const char *src)
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
int sq_noun2singular(char *dest, const char *src)
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

// ------------------------------------
//	table name and type name

char *sq_name2table(const char *type_name)
{
	char  *table_name;
	int    length;

	// length = snake case name + plural character + null-terminated
	length = sq_snake_from_camel(NULL, type_name) +2 +1;
	table_name = malloc(length);
	sq_snake_from_camel(table_name, type_name);
	sq_noun2plural(table_name, table_name);
	return table_name;
}

char *sq_name2type(const char *table_name)
{
	char  *type_name;
	int    length;

	// length = snake case name + null-terminated
	length = sq_camel_from_snake(NULL, table_name, true) +1;
	type_name = malloc(length);
	sq_camel_from_snake(type_name, table_name, true);
	sq_noun2singular(type_name, type_name);
	return type_name;
}

#endif  // SQ_CONFIG_NAMING_CONVENTION
