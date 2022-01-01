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

void sq_option_free(SqOption *option)
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

int  sq_option_print(SqOption *option, SqBuffer *buffer, int opt_max_length)
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
		length += strlen(option->shortcut) + 2 + 1;
	}
	if (option->name) {
		if (buffer) {
			sq_buffer_write_n(buffer, "--", 2);
			sq_buffer_write(buffer, option->name);
		}
		length += strlen(option->name) + 2;
	}

	if (option->value_description) {
		if (buffer)
			sq_buffer_write(buffer, option->value_description);
		length += strlen(option->value_description);
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

int  sq_option_cmp_str__shortcut(const char *str,  SqOption **option)
{
	const char *shortcut;

	shortcut = (*option) ? (*option)->shortcut : "";
	return strcmp(str, shortcut);
}

int  sq_option_cmp_shortcut(SqOption **option1, SqOption **option2)
{
	const char *shortcut1;
	const char *shortcut2;

	shortcut1 = (*option1) ? (*option1)->shortcut : "";
	shortcut2 = (*option2) ? (*option2)->shortcut : "";
	return strcmp(shortcut1, shortcut2);
}
