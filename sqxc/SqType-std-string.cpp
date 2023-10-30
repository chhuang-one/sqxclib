/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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
	memory = malloc(size);
	object = new (memory) MyClass();

	// ==== C++ destructor  + C free()
	object->~MyClass();
	free(memory);
 */

static void sq_type_std_string_init(void *instance, const SqType *type)
{
#if 1
	new (&(*(std::string*)instance)) std::string();
#else
	new (instance) std::string();
#endif
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
	if (src->type == SQXC_TYPE_STR) {
		if (src->value.str)
			((std::string*)instance)->assign(src->value.str);
		else
			((std::string*)instance)->resize(0);
	}
	else {
		// TODO: convert to string
		/* set required type if return SQCODE_TYPE_NOT_MATCH
		src->required_type = SQXC_TYPE_STR;
		*/
		return (src->code = SQCODE_TYPE_NOT_MATCH);
	}

	return (src->code = SQCODE_OK);
}

static Sqxc *sq_type_std_string_write(void *instance, const SqType *type, Sqxc *dest)
{
	dest->type = SQXC_TYPE_STR;
//	dest->name = dest->name;    // "name" was set by caller of this function
	dest->value.str = (char*)((std::string*)instance)->c_str();
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
