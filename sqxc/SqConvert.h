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

#ifndef SQ_CONVERT_H
#define SQ_CONVERT_H

#include <time.h>         // time_t, struct tm

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	time_t convert from/to string
 */

// return -1 if error
time_t  sq_time_from_string(const char *timestr);

// return NULL if error
char   *sq_time_to_string(time_t time, int format_type);

/* ----------------------------------------------------------------------------
	binary convert from/to hex
 */

// pass NULL to 'dest_hex' to calculate length
// return length of dest_hex
size_t  sq_bin_to_hex(char *dest_hex, const char *src_bin, size_t src_length);

// 'src_hex' can be the same as 'dest_bin'
// pass NULL to 'dest_bin' to calculate length
// return length of dest_bin
size_t  sq_hex_to_bin(char *dest_bin, const char *src_hex, size_t src_length);

/* ----------------------------------------------------------------------------
	binary convert from/to base64
 */

#if 0
/* ----------------------------------------------------------------------------
	convert string between C and SQL

	e.g.   C string:   "I'm worker."
	       SQL string: 'I''m worker.'
*/

// C string to SQL string
// parameter: 'c_string' input
// parameter: 'dest' output
// pass NULL to 'dest' to calculate length
// return length of SQL string
int  sq_cstr2sql(char *dest, const char *c_string);

// SQL string to C string
// parameter: 'sql_string' input
// parameter: 'dest' output
// pass NULL to 'dest' to calculate length
// return length of C string
int  sq_sql2cstr(char *dest, char *sql_string);

#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_CONVERT_H
