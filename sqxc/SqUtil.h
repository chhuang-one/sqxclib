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

#ifndef SQ_UTIL_H
#define SQ_UTIL_H

#include <time.h>        // time_t, struct tm

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

#if 0
/* ----------------------------------------------------------------------------
	C string to/from SQL string

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

#endif  // SQ_UTIL_H
