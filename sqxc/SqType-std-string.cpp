/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#include <string>
#include <SqError.h>
#include <SqType.h>

/* ----------------------------------------------------------------------------
    std::string
 */

static void sq_type_std_string_init(void* instance, const SqType *type)
{
    new (&(*(std::string*)instance)) std::string();
}

static void sq_type_std_string_final(void* instance, const SqType *type)
{
    ((std::string*)instance)->std::string::~string();
}

static int  sq_type_std_string_parse(void* instance, const SqType *type, Sqxc* src)
{
	if (src->type == SQXC_TYPE_STRING) {
		if (src->value.string)
            ((std::string*)instance)->assign(src->value.string);
		else
            ((std::string*)instance)->resize(0);
	}
	else {
		// TODO: convert to string
//		src->required_type = SQXC_TYPE_STRING;    // set required type if return SQCODE_TYPE_NOT_MATCH
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

static Sqxc* sq_type_std_string_write(void* instance, const SqType *type, Sqxc* dest)
{
	dest->type = SQXC_TYPE_STRING;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.string = (char*)((std::string*)instance)->c_str();
	return sqxc_send(dest);
}

// extern
const SqType SqType_StdString_ =
{
	sizeof(std::string),
	sq_type_std_string_init,
	sq_type_std_string_final,
	sq_type_std_string_parse,
	sq_type_std_string_write,
};
