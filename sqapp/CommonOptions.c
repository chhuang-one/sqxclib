/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <CommonOptions.h>

const SqOption common_option_array[] = {
	{SQ_TYPE_BOOL,  "help",      offsetof(CommandCommon, help),
		.shortcut = "h",
		.default_value = "true",
		.description = "Display help for the given command."},

	{SQ_TYPE_BOOL,  "quiet",     offsetof(CommandCommon, quiet),
		.shortcut = "q",
		.default_value = "true",
		.description = "Do not output any message."},

	{SQ_TYPE_BOOL,  "version",   offsetof(CommandCommon, version),
		.shortcut = "V",
		.default_value = "true",
		.description = "Display this application version."},
};

// common option pointer array
const SqOption *common_options[] = {
	&common_option_array[0],
	&common_option_array[1],
	&common_option_array[2],
};

const int n_common_options = sizeof(common_options)/sizeof(void*);
