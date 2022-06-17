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

#include <SqCommandCommon.h>

const SqOption SqOption_CommandCommonHelp_ = {
	SQ_TYPE_BOOL,  "help",      offsetof(SqCommandCommon, help),
	.shortcut = "h",
	.default_value = "true",
	.description = "Display help for the given command.",
};

const SqOption SqOption_CommandCommonQuiet_ = {
	SQ_TYPE_BOOL,  "quiet",     offsetof(SqCommandCommon, quiet),
	.shortcut = "q",
	.default_value = "true",
	.description = "Do not output any message.",
};

const SqOption SqOption_CommandCommonVersion_ = {
	SQ_TYPE_BOOL,  "version",   offsetof(SqCommandCommon, version),
	.shortcut = "V",
	.default_value = "true",
	.description = "Display this application version.",
};
