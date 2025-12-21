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

#include <stdlib.h>
#include <string.h>

#include <SqConfig.h>
#include <SqError.h>
#include <SqxcValue.h>
#include <SqCommand.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

SqCommandValue *sq_command_value_new(const SqCommand *commandType)
{
	SqCommandValue *commandValue;

	commandValue = malloc(sizeof(SqCommandValue));
	sq_command_value_init(commandValue, commandType);
	return commandValue;
}

void  sq_command_value_free(SqCommandValue *commandValue)
{
	sq_command_value_final(commandValue);
	free(commandValue);
}

void  sq_command_value_init(SqCommandValue *commandValue, const SqCommand *commandType)
{
	commandValue->type = commandType;
	sq_ptr_array_init(&commandValue->shortcuts, 8, NULL);
	sq_ptr_array_init(&commandValue->arguments, 8, NULL);
//	sq_ptr_array_init(&cmd->arguments, 8, (SqDestroyFunc)free);

	commandValue->options = calloc(1, commandType->size);
	sq_type_init_instance((SqType*)commandValue->type, commandValue->options, false);
}

void  sq_command_value_final(SqCommandValue *commandValue)
{
	sq_ptr_array_final(&commandValue->arguments);
	sq_ptr_array_final(&commandValue->shortcuts);

	sq_type_final_instance((SqType*)commandValue->type, commandValue->options, 0);
	free(commandValue->options);
}

// ----------------------------------------------------------------------------
// --- SqCommand C functions ---

SqCommand *sq_command_new(const char *commandName)
{
	SqCommand *commandType;

	commandType = malloc(sizeof(SqCommand));
	sq_command_init_self(commandType, commandName);
	return commandType;
}

void  sq_command_free(SqCommand *commandType)
{
	if (commandType->bit_field & SQB_TYPE_DYNAMIC) {
		sq_command_final_self(commandType);
		free(commandType);
	}
}

void  sq_command_init_self(SqCommand *commandType, const char *commandName)
{
	// set SqType members
	sq_type_init_self((SqType*)commandType, 0, (SqDestroyFunc)sq_option_free);
	commandType->parse = sq_command_parse_option;
	commandType->write = sq_type_object_write;
	commandType->name  = (commandName) ? strdup(commandName) : NULL;
	// set SqCommand members
	commandType->handle = NULL;
	commandType->parameter = NULL;
	commandType->description = NULL;
}

void  sq_command_final_self(SqCommand *commandType)
{
	// free SqType members
	sq_type_final_self((SqType*)commandType);
	// free SqCommand members
	free((char*)commandType->parameter);
	free((char*)commandType->description);
}

SqCommand *sq_command_copy(SqCommand       *commandType_dest,
                           const SqCommand *commandType_src,
                           SqDestroyFunc    option_free_func,
                           SqCopyFunc       option_copy_func)
{
	if (commandType_dest == NULL)
		commandType_dest = malloc(sizeof(SqCommand));
	if (option_free_func == NULL)
		option_free_func = (SqDestroyFunc)sq_option_free;

	// copy SqType members
	sq_type_copy((SqType*)commandType_dest, (SqType*)commandType_src,
	             option_free_func, option_copy_func);
	// copy SqCommand members
	commandType_dest->handle = commandType_src->handle;
	commandType_dest->parameter = strdup(commandType_src->parameter);
	commandType_dest->description = strdup(commandType_src->description);

	return commandType_dest;
}

void  sq_command_add_option(SqCommand *commandType, const SqOption *option, unsigned int n_option)
{
	sq_type_add_entry((SqType*)commandType, (SqEntry*)option, n_option, sizeof(SqOption));
}

void  sq_command_sort_shortcuts(const SqCommand *commandType, SqPtrArray *array)
{
	SqOption *option;

	array->length = 0;
	for (unsigned int i = 0;  i < commandType->n_entry;  i++) {
		option = (SqOption*)commandType->entry[i];
        if (option->shortcut)
            sq_ptr_array_push(array, option);
	}
	sq_ptr_array_sort(array, sq_option_cmp_shortcut);
}

/* SqCommand parse function */
int  sq_command_parse_option(void *instance, const SqType *type, Sqxc *src)
{
	SqxcValue  *xc_value = (SqxcValue*)src->dest;
	SqxcNested *nested;
	union {
		SqOption   *option;
		void      **addr;
	} temp;

	// Start of Object
	nested = xc_value->nested;
#if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	if (nested->data3 != instance) {
		if (nested->data != instance) {
			// First time to call this function to parse object
			nested = sqxc_push_nested((Sqxc*)xc_value);
			nested->data  = instance;
			nested->data2 = (SqType*)type;
			nested->data3 = xc_value;    // SqxcNested is NOT ready to parse, it is doing type match.
		}
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// SqxcNested is ready to parse object, type has been matched.
		nested->data3 = instance;
		return (src->code = SQCODE_OK);
	}
#else
	if (nested->data != instance) {
		// do type match
		if (src->type != SQXC_TYPE_OBJECT) {
			/* set required type if return SQCODE_TYPE_NOT_MATCHED
			src->required_type = SQXC_TYPE_OBJECT;
			*/
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		}
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = instance;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
#endif  // SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
	/*
	// End of Object : sqxc_value_send() have done it.
	else if (src->type == SQXC_TYPE_OBJECT_END) {
		sqxc_pop_nested(src);
		return (src->code = SQCODE_OK);
	}
	 */

	// find option by name or shortcut name
	temp.addr = sq_type_find_entry(type, src->name, NULL);
	if (temp.addr == NULL) {
		// option shortcut
		temp.addr = sq_ptr_array_search(&((SqCommandValue*)instance)->shortcuts,
				src->name, sq_option_cmp_str__shortcut);
	}

	// 'instance' pointer to structure of option values
	instance = ((SqCommandValue*)instance)->options;
	// parse entries in type
	if (temp.addr) {
		temp.option = *(SqOption**)temp.addr;
		type = temp.option->type;
		if (type->parse == NULL)  // don't parse anything if function pointer is NULL
			return (src->code = SQCODE_OK);
		instance = (char*)instance + temp.option->offset;
		// default_value
		if (src->value.str == NULL || *src->value.str == 0)
			src->value.str = (char*)temp.option->default_value;
		// special case : pointer to instance
		if (temp.option->bit_field & SQB_POINTER) {
			// try to use existed instance
			if (*(void**)instance)
				instance = *(void**)instance;
			// allocate & initialize instance if source is not NULL
			else if (src->type != SQXC_TYPE_NULL)
				instance = sq_type_init_instance(type, instance, true);
			else
				return (src->code = SQCODE_OK);
		}
		return type->parse(instance, type, src);
	}
	return (src->code = SQCODE_ENTRY_NOT_FOUND);
}

// ----------------------------------------------------------------------------
// If C compiler doesn't support C99 inline functions

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// C99 or C++ inline functions has defined in SqCommand.h

#else   // __STDC_VERSION__
// define C/C++ functions here if compiler does NOT support inline function.

void  sq_command_set_parameter(SqCommand *command, const char *parameter)
{
	SQ_COMMAND_SET_PARAMETER(command, parameter);
}

void  sq_command_set_description(SqCommand *command, const char *description)
{
	SQ_COMMAND_SET_DESCRIPTION(command, description);
}

#endif  // __STDC_VERSION__
