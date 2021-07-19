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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include <SqError.h>
#include <SqType.h>

/* ----------------------------------------------------------------------------
	SqType for std::string

	We can call C++ constructor/destructor in init()/final()

	// ==== C++ constructor + C malloc()
	buffer = malloc(size);
	object = new (buffer) MyClass();

	// ==== C++ destructor  + C free()
	object->~MyClass();
	free(buffer);
 */

static void sq_type_std_string_init(void *instance, const SqType *type)
{
	new (&(*(std::string*)instance)) std::string();
}

static void sq_type_std_string_final(void *instance, const SqType *type)
{
#if 1    // defined(__APPLE__)    // clang
	std::string &stdstring = *((std::string*)instance);
	// call basic_string destructor.
	stdstring.~basic_string();
#else
	((std::string*)instance)->std::string::~string();
#endif
}

static int  sq_type_std_string_parse(void *instance, const SqType *type, Sqxc *src)
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

static Sqxc *sq_type_std_string_write(void *instance, const SqType *type, Sqxc *dest)
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
