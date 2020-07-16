/*
 *   Copyright (C) 2020 by C.H. Huang
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

#include <SqError.h>
#include <SqField.h>

// ----------------------------------------------------------------------------
// SqField

SqField* sq_field_new(const SqType* type_info)
{
	SqField* field;

	field = malloc(sizeof(SqField));
	sq_field_init(field, type_info);
	return field;
}

void  sq_field_free(SqField* field)
{
	if (field->bit_field & SQB_DYNAMIC) {
		sq_field_final(field);
		free(field);
	}
}

void  sq_field_init(SqField* field, const SqType* type_info)
{
	field->name = NULL;
	field->offset = 0;
	field->bit_field = SQB_DYNAMIC;

	if (type_info)
		field->type = (SqType*)type_info;
	else {
		// use dynamic type if type_info == NULL
		field->type = sq_type_new(8, (SqDestroyFunc)sq_field_free);
	}
}

void  sq_field_final(SqField* field)
{
	if (field->bit_field & SQB_DYNAMIC) {
		if (field->type)
			sq_type_free(field->type);
		free(field->name);
	}
}

// ----------------------------------------------------------------------------
// SqCompareFunc

// used by find()
int  sq_field_cmp_str__name(const char* str, SqField** field)
{
	const char* name1;
	const char* name2;

	name1 = (str) ? str : "";
	name2 = (*field) ? (*field)->name : "";
	return strcasecmp(name1, name2);
}

// used by sort()
int  sq_field_cmp_name(SqField** field1, SqField** field2)
{
	const char* name1;
	const char* name2;

	name1 = (*field1) ? (*field1)->name : "";
	name2 = (*field2) ? (*field2)->name : "";
	return strcasecmp(name1, name2);
}

int  sq_field_cmp_str__type_name(const char* str,  SqField** field)
{
	const char* name1;
	const char* name2;

	name1 = (str) ? str : "";
	name2 = (*field) ? (*field)->type->name : "";
	return strcmp(name1, name2);
}

int  sq_field_cmp_type_name(SqField** field1, SqField** field2)
{
	const char* name1;
	const char* name2;

	name1 = (*field1) ? (*field1)->type->name : "";
	name2 = (*field2) ? (*field2)->type->name : "";
	return strcmp(name1, name2);
}
