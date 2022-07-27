/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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

#ifndef SQ_OPTION_H
#define SQ_OPTION_H

#include <SqEntry.h>
#include <SqType.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqOption          SqOption;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* --- SqOption C functions --- */
SqOption *sq_option_new(const SqType *type);
void      sq_option_free(SqOption *option);

void  sq_option_init(SqOption *option, const SqType *type);
void  sq_option_final(SqOption *option);

// return length of option string
int   sq_option_print(SqOption *option, SqBuffer *buffer, int opt_max_length);

// SqCompareFunc for sorting and finding SqOption by SqOption::shortcut
int   sq_option_cmp_str__shortcut(const char *str,  SqOption **option);
int   sq_option_cmp_shortcut(SqOption **option1, SqOption **option2);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

}  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqOption: define an option for command
	          format : -shortcut, --name[=value_description]  description

	SqEntry
	|
	`--- SqOption

	Because 'const' is used to define string 'char*' and type 'SqType*',
	C++ user can initialize static structure easily.

	SqOption must have no base struct because I need use aggregate initialization with it.
*/

#define SQ_OPTION_MEMBERS           \
	SQ_ENTRY_MEMBERS;               \
	const char *shortcut;           \
	const char *default_value;      \
	const char *value_description;  \
	const char *description

struct SqOption
{
	SQ_OPTION_MEMBERS;
/*	// ------ SqEntry members ------
	const SqType *type;
	const char   *name;
	size_t        offset;
	unsigned int  bit_field;

	// ------ SqOption members ------
	const char *shortcut;
	const char *default_value;
	const char *value_description;
	const char *description;
 */
#ifdef __cplusplus
	/* Note: If you add, remove, or change methods here, do the same things in Sq::OptionMethod. */

	void  init(const SqType *type) {
		sq_option_init((SqOption*)this, type);
	}
	void  init(const Sq::TypeMethod *type) {
		sq_option_init((SqOption*)this, (const SqType*)type);
	}
	void  final() {
		sq_option_final((SqOption*)this);
	}

	int   print(Sq::BufferMethod *buffer, int opt_max_length) {
		return sq_option_print((SqOption*)this, (SqBuffer*)buffer, opt_max_length);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	OptionMethod is used by SqOption's children.

	It's derived struct/class must be C++11 standard-layout and has SqOption members.

	Note: If you add, remove, or change methods here, do the same things in SqOption.
 */
struct OptionMethod {
	void  init(const SqType *type) {
		sq_option_init((SqOption*)this, type);
	}
	void  init(const Sq::TypeMethod *type) {
		sq_option_init((SqOption*)this, (const SqType*)type);
	}
	void  final() {
		sq_option_final((SqOption*)this);
	}

	int   print(Sq::BufferMethod *buffer, int opt_max_length) {
		return sq_option_print((SqOption*)this, (SqBuffer*)buffer, opt_max_length);
	}
};

/* All derived struct/class must be C++11 standard-layout. */

struct Option : SqOption {
	// constructor
	Option() {}
	Option(const SqType *type) {
		sq_option_init(this, type);
	}
	Option(const Sq::TypeMethod *type) {
		sq_option_init(this, (const SqType*)type);
	}
	// destructor
	~Option() {
		sq_option_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_OPTION_H
