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
#include <string.h>

#include <SqError.h>
#include <SqxcValue.h>
#include <SqCommand.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

SqCommandValue  *sq_command_value_new(const SqCommand *cmd_type)
{
	SqCommandValue *cmd_value;

	cmd_value = calloc(1, cmd_type->size);
	sq_command_value_init(cmd_value, cmd_type);
	return cmd_value;
}

void  sq_command_value_free(SqCommandValue *cmd_value)
{
	sq_command_value_final(cmd_value);
	free(cmd_value);
}

void  sq_command_value_init(SqCommandValue *cmd_value, const SqCommand *cmd_type)
{
	if (cmd_type) {
		sq_command_ref((SqCommand*)cmd_type);
		sq_type_init_instance((SqType*)cmd_type, cmd_value, 0);
	}
	cmd_value->type = cmd_type;
	sq_ptr_array_init(&cmd_value->shortcuts, 8, NULL);
	sq_ptr_array_init(&cmd_value->arguments, 8, NULL);
//	sq_ptr_array_init(&cmd->arguments, 8, (SqDestroyFunc)free);
}

void  sq_command_value_final(SqCommandValue *cmd_value)
{
	const SqCommand *cmd_type = cmd_value->type;

	sq_ptr_array_final(&cmd_value->arguments);
	sq_ptr_array_final(&cmd_value->shortcuts);
	if (cmd_type) {
		sq_type_final_instance((SqType*)cmd_value->type, cmd_value, 0);
		sq_command_unref((SqCommand*)cmd_value->type);
	}
}

// ----------------------------------------------------------------------------
// --- SqCommand C functions ---

SqCommand *sq_command_new(const char *cmd_name)
{
	SqCommand *cmd_type;

	cmd_type = malloc(sizeof(SqCommand));
	// set SqType members
	sq_type_init_self((SqType*)cmd_type, 0, (SqDestroyFunc)sq_option_free);
	cmd_type->parse = sq_command_parse_option;
	cmd_type->write = NULL;
	cmd_type->name  = strdup(cmd_name);
	// set SqCommand members
	cmd_type->handle = NULL;
	cmd_type->parameter = NULL;
	cmd_type->description = NULL;

	return cmd_type;
}

void sq_command_ref(SqCommand *cmd_type)
{
	// below code is the same as sq_type_ref()
	if (cmd_type->bit_field & SQB_TYPE_DYNAMIC)
		cmd_type->ref_count++;
}

void sq_command_unref(SqCommand *cmd_type)
{
	if (cmd_type->bit_field & SQB_TYPE_DYNAMIC) {
		cmd_type->ref_count--;
		if (cmd_type->ref_count == 0) {
			// free SqType members
			sq_type_final_self((SqType*)cmd_type);
			// free SqCommand members
			free((char*)cmd_type->parameter);
			free((char*)cmd_type->description);
			// free SqCommand struct
			free(cmd_type);
		}
	}
}

SqCommand *sq_command_copy_static(SqCommand       *type_dest,
                                  const SqCommand *static_type_src,
                                  SqDestroyFunc    option_free_func)
{
	if (type_dest == NULL)
		type_dest = malloc(sizeof(SqCommand));
	if (option_free_func == NULL)
		option_free_func = (SqDestroyFunc)sq_option_free;

	// copy SqType members
	sq_type_copy_static((SqType*)type_dest, (SqType*)static_type_src, option_free_func);
	// copy SqCommand members
	type_dest->handle = static_type_src->handle;
	type_dest->parameter = strdup(static_type_src->parameter);
	type_dest->description = strdup(static_type_src->description);

	return type_dest;
}

void  sq_command_sort_shortcuts(const SqCommand *cmd_type, SqPtrArray *array)
{
	SqOption *option;

	array->length = 0;
	for (int i = 0;  i < cmd_type->n_entry;  i++) {
		option = (SqOption*)cmd_type->entry[i];
        if (option->shortcut)
            sq_ptr_array_append(array, option);
	}
	sq_ptr_array_sort(array, (SqCompareFunc)sq_option_cmp_shortcut);
}

/* SqCommand parse function */
int  sq_command_parse_option(void *instance, const SqType *type, Sqxc *src)
{
	SqxcValue  *xc_value = (SqxcValue*)src->dest;
	SqxcNested *nested;
	SqOption   *option;

	// Start of Object
	nested = xc_value->nested;
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// Frist time to call this function to parse object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data  = instance;
			nested->data2 = (SqType*)type;
			nested->data3 = NULL;
		}
		if (src->type != SQXC_TYPE_OBJECT) {
//			src->required_type = SQXC_TYPE_OBJECT;    // set required type if return SQCODE_TYPE_NOT_MATCH
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		}
		// ready to parse object
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// option
	option = (SqOption*)sq_type_find_entry(type, src->name, NULL);
	if (option == NULL) {
		// option shortcut
		option = sq_ptr_array_search(&((SqCommandValue*)instance)->shortcuts,
				src->name, (SqCompareFunc)sq_option_cmp_str__shortcut);
	}

	// parse entries in type
	if (option) {
		option = *(SqOption**)option;
		type = option->type;
		if (type->parse == NULL)  // don't parse anything if function pointer is NULL
			return (src->code = SQCODE_OK);
		instance = (char*)instance + option->offset;
		// default_value
		if (src->value.string == NULL || *src->value.string == 0)
			src->value.string = (char*)option->default_value;
		// special case : pointer to instance
		if (option->bit_field & SQB_POINTER) {
			// try to use existed instance
			if (*(void**)instance)
				instance = *(void**)instance;
//			else if (src->type != SQXC_TYPE_STRING || src->value.string != NULL)
//				instance = sq_type_init_instance(type, instance, true);
			else
				return (src->code = SQCODE_OK);
		}
		return type->parse(instance, type, src);
	}
	return (src->code = SQCODE_ENTRY_NOT_FOUND);
}

