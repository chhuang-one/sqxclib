/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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

#include <stdlib.h>
#include <SqOption.h>

#define OPTION_BORDER_WIDTH    2

SqOption *sq_option_new(const SqType *type)
{
	SqOption *option;

	option = malloc(sizeof(SqOption));
	sq_option_init(option, type);
	return option;
}

void  sq_option_free(SqOption *option)
{
	sq_option_final(option);
	free(option);
}

void  sq_option_init(SqOption *option, const SqType *type)
{
	sq_entry_init((SqEntry*)option, type);
	// --- SqOption members ---
	option->shortcut = NULL;
	option->default_value = NULL;
	option->value_description = NULL;
	option->description = NULL;
}

void  sq_option_final(SqOption *option)
{
	if (option->bit_field & SQB_DYNAMIC) {
		sq_entry_final((SqEntry*)option);
		// --- SqOption members ---
		free((char*)option->shortcut);
		free((char*)option->default_value);
		free((char*)option->value_description);
		free((char*)option->description);
	}
}

int   sq_option_print(SqOption *option, SqBuffer *buffer, int opt_max_length)
{
	int   length;

	if (buffer)
		memset(sq_buffer_alloc(buffer,2), ' ', OPTION_BORDER_WIDTH);
	length = OPTION_BORDER_WIDTH;

	if (option->shortcut) {
		if (buffer) {
			sq_buffer_write_c(buffer, '-');
			sq_buffer_write(buffer, option->shortcut);
			sq_buffer_write_n(buffer, ", ", 2);
		}
		length += (int)strlen(option->shortcut) + 2 + 1;
	}
	if (option->name) {
		if (buffer) {
			sq_buffer_write_n(buffer, "--", 2);
			sq_buffer_write(buffer, option->name);
		}
		length += (int)strlen(option->name) + 2;
	}

	if (option->value_description) {
		if (buffer)
			sq_buffer_write(buffer, option->value_description);
		length += (int)strlen(option->value_description);
	}

	if (opt_max_length < length)
		opt_max_length = OPTION_BORDER_WIDTH;
	else
		opt_max_length = OPTION_BORDER_WIDTH + opt_max_length - length;
	if (buffer)
		memset(sq_buffer_alloc(buffer, opt_max_length), ' ', opt_max_length);

	if (option->description  &&  buffer)
		sq_buffer_write(buffer, option->description);

	return length;
}

// ------------------------------------
// SqOption SqCompareFunc

// This function is used by find(). Its actual parameter type:
//int sq_option_cmp_str__shortcut(const char *str, SqOption  **option);
int   sq_option_cmp_str__shortcut(const void *str, const void *option)
{
	const char *shortcut;

	shortcut = (*(SqOption**)option) ? (*(SqOption**)option)->shortcut : "";
	return strcmp(str, shortcut);
}

// This function is used by sort(). Its actual parameter type:
//int sq_option_cmp_shortcut(SqOption  **option1, SqOption  **option2);
int   sq_option_cmp_shortcut(const void *option1, const void *option2)
{
	const char *shortcut1;
	const char *shortcut2;

	shortcut1 = (*(SqOption**)option1) ? (*(SqOption**)option1)->shortcut : "";
	shortcut2 = (*(SqOption**)option2) ? (*(SqOption**)option2)->shortcut : "";
	return strcmp(shortcut1, shortcut2);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqOption.h

#else   // __STDC_VERSION__
// define C/C++ functions here if compiler does NOT support inline function.

void  sq_option_set_name(SqOption *option, const char *name)
{
	SQ_OPTION_SET_NAME(option, name);
}

void  sq_option_set_shortcut(SqOption *option, const char *shortcut)
{
	SQ_OPTION_SET_SHORTCUT(option, shortcut);
}

void  sq_option_set_default(SqOption *option, const char *default_value)
{
	SQ_OPTION_SET_DEFAULT(option, default_value);
}

void  sq_option_set_value_description(SqOption *option, const char *value_description)
{
	SQ_OPTION_SET_VALUE_DESCRIPTION(option, value_description);
}

void  sq_option_set_description(SqOption *option, const char *description)
{
	SQ_OPTION_SET_DESCRIPTION(option, description);
}

#endif  // __STDC_VERSION__
