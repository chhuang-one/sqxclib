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

#ifndef SQ_HELPERS_H
#define SQ_HELPERS_H

#include <stdbool.h>

#include <SqConfig.h>
#include <SqType.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------------
	camel case and snake case

	e.g. snake case       : question_mark
	     lower camel case : questionMark
	     upper camel case : QuestionMark
 */

// pass NULL to 'dest_camel' to calculate length
// return length of dest_camel
int   sq_snake2camel(char *dest_camel, const char *src_snake_name, bool upper_camel_case);

// pass NULL to 'dest_snake' to calculate length
// return length of dest_snake
int   sq_camel2snake(char *dest_snake, const char *src_camel_name);

char* sq_str_camel(const char *snake_name, bool upper_camel_case);
char* sq_str_snake(const char *camel_name);

// ----------------------------------------------------------------------------
//	singular noun and plural noun

// pass NULL to 'dest' to calculate length
// return length of singular noun
int   sq_plural2singular(char *dest, const char *src_plural_noun);

// pass NULL to 'dest' to calculate length
// return length of plural noun
int   sq_singular2plural(char *dest, const char *src_singular_noun);

char* sq_str_singular(const char *plural);
char* sq_str_plural(const char *singular);

/* ----------------------------------------------------------------------------
	table name (snake case) and type name (upper camel case)

	If source string is NULL, these functions return NULL for convenience.
 */

char *sq_str_table_name(const char *src_type_name);
char *sq_str_type_name(const char *src_table_name);

/* ----------------------------------------------------------------------------
	use Sqxc elements to output JSON data to memory or file.

	these functions return error code.
 */

#if defined(SQ_CONFIG_HAVE_JSONC) && (SQ_CONFIG_HAVE_JSONC == 1)
int  sq_write_json_mem(void *instance, const SqType *type, char **buf, size_t *len);
int  sq_write_json_file(void *instance, const SqType *type, const char *filename);
#endif

#ifdef __cplusplus
}  // extern "C"
#endif


// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

#if defined(SQ_CONFIG_HAVE_JSONC) && (SQ_CONFIG_HAVE_JSONC == 1)
inline int  writeJson(void *instance, const SqType *type, char **buf, size_t *len) {
	return sq_write_json_mem(instance, type, buf, len);
}

inline int  writeJson(void *instance, const SqType *type, const char *filename) {
	return sq_write_json_file(instance, type, filename);
}
#endif

namespace Str {

	// naming convention
	inline char* camel(const char *snakeName, bool upperCamelCase = true) {
		return sq_str_camel(snakeName, upperCamelCase);
	}
	inline char* snake(const char *camelName) {
		return sq_str_snake(camelName);
	}
	inline char* singular(const char *plural) {
		return sq_str_singular(plural);
	}
	inline char* plural(const char *singular) {
		return sq_str_plural(singular);
	}

	inline char *tableName(const char *typeName) {
		return sq_str_table_name(typeName);
	}
	inline char *typeName(const char *tableName) {
		return sq_str_type_name(tableName);
	}

};  // namespace Str

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_HELPERS_H
