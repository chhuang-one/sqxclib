/*
 *   Copyright (C) 2021-2024 by C.H. Huang
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

#include <SqAppOptions.h>

const SqOption sqCommonOptions_help = {
	SQ_TYPE_BOOL,  "help",      offsetof(SqCommonOptions, help),
	.shortcut = "h",
	.default_value = "true",
	.description = "Display help for the given command.",
};

const SqOption sqCommonOptions_quiet = {
	SQ_TYPE_BOOL,  "quiet",     offsetof(SqCommonOptions, quiet),
	.shortcut = "q",
	.default_value = "true",
	.description = "Do not output any message.",
};

const SqOption sqCommonOptions_version = {
	SQ_TYPE_BOOL,  "version",   offsetof(SqCommonOptions, version),
	.shortcut = "V",
	.default_value = "true",
	.description = "Display this application version.",
};
