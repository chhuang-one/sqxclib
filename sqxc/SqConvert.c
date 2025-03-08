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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>        // sscanf()
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>       // malloc()
#include <string.h>

#include <SqConvert.h>

#ifdef _MSC_VER
#define snprintf     _snprintf
#endif

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
char   *sq_time_to_string(time_t timeraw, int format_type)
{
	const char *format;
	struct tm  *timeinfo;
	char       *timestr;

#if SQ_CONFIG_CONVERT_TIME_TO_GMT
	timeinfo = gmtime((time_t*) &timeraw);
#else
	timeinfo = localtime((time_t*) &timeraw);
#endif
	if (timeinfo == NULL)
		return NULL;

	switch (format_type) {
	case 'c':
		// output format : "2013_02_05_212515"
		format = "%.4d_%.2d_%.2d_%.2d%.2d%.2d";
		break;

	case 0:
	default:
		// output format : "2013-02-05 21:25:15"
		format = "%.4d-%.2d-%.2d %.2d:%.2d:%.2d";
		break;
	}

	timestr = malloc(32);
	snprintf(timestr, 32, format,
	         timeinfo->tm_year + 1900,
	         timeinfo->tm_mon  + 1,
	         timeinfo->tm_mday,
	         timeinfo->tm_hour,
	         timeinfo->tm_min,
	         timeinfo->tm_sec);
	return timestr;
}

// ----------------------------------------------------------------------------

size_t  sq_bin_to_hex(char *dest_hex, const char *src_bin, size_t src_length)
{
	const char hex[] = {"0123456789ABCDEF"};

	if (dest_hex != NULL) {
		for (size_t idx = 0;  idx < src_length;  idx++) {
			uint8_t ch = *(uint8_t*)src_bin++;
			*dest_hex++ = hex[ch >> 4];
			*dest_hex++ = hex[ch  & 0x0F];
		}
	}

	return src_length * 2;
}

size_t  sq_hex_to_bin(char *dest_bin, const char *src_hex, size_t src_length)
{
//	if (src_length == 0)
//		src_length  = strlen(src_hex);

	if (dest_bin != NULL) {
		for (size_t idx = 0;  idx < src_length;  idx++) {
			uint8_t ch = *(uint8_t*)src_hex++;
			if (ch > 96)
				ch = ch - 'a' + 10;
			else if (ch > 64)
				ch = ch - 'A' + 10;
			else if (ch > 47)
				ch = ch - '0';

			if (idx & 1)     // src_hex[1], src_hex[3], src_hex[5]
				*dest_bin++ |= ch;
			else             // src_hex[0], src_hex[2], src_hex[4]
				*dest_bin    = ch << 4;
		}
	}

	return src_length >> 1;    // src_length / 2;
}

// ----------------------------------------------------------------------------

#if 0

// C string to SQL string
int  sq_cstr2sql(char *sql_string, const char *c_string)
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
int  sq_sql2cstr(char *c_string, char *sql_string)
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

#endif
