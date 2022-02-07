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

#include <SqError.h>
#include <SqType.h>

static int  sq_type_unknown_parse(void *instance, const SqType *type, Sqxc *src)
{
	return (src->code = SQCODE_ENTRY_NOT_FOUND);
}

static Sqxc *sq_type_unknown_write(void *instance, const SqType *type, Sqxc *dest)
{
	return dest;
}

const struct SqTypeFake SqType_Fake_ = {
	.nth = {0},
	.unknown = {
		0,
		NULL,
		NULL,
		sq_type_unknown_parse,
		sq_type_unknown_write,
	},
};
