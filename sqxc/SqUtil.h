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

#ifndef SQ_UTIL_H
#define SQ_UTIL_H

#include <time.h>        // time_t, struct tm
#include <stdbool.h>     // bool

#include <SqConfig.h>    // SQ_CONFIG_NAMING_CONVENTION

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	time_t convert from/to string
 */

// return -1 if error
time_t  sq_time_from_string(const char* timestr);

// return NULL if error
char*   sq_time_to_string(time_t time);

/* ----------------------------------------------------------------------------
	camel case and snake case

	e.g. snake case       : question_mark
	     lower camel case : questionMark
	     upper camel case : QuestionMark
 */

#ifdef SQ_CONFIG_NAMING_CONVENTION

// pass NULL to "camel_name" to calculate length
// return length of camel_name
int  sq_camel_from_snake(char* camel_name, const char* snake_name, bool upper_camel_case);

// pass NULL to "snake_name" to calculate length
// return length of snake_name
int  sq_snake_from_camel(char* snake_name, const char* camel_name);

// ----------------------------------------------------------------------------
//	singular noun and plural noun

// pass NULL to "dest" to calculate length
// return length of singular noun
int  sq_noun2singular(char* dest, const char* src_plural_noun);

// pass NULL to "dest" to calculate length
// return length of plural noun
int  sq_noun2plural(char* dest, const char* src_singular_noun);

// ----------------------------------------------------------------------------
//	table name (snake case) and type name (upper camel case)

char* sq_name2table(const char* type_name);
char* sq_name2type(const char* table_name);

#endif  // SQ_CONFIG_NAMING_CONVENTION


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_UTIL_H
