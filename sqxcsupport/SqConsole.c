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

#include <stdio.h>
#include <string.h>

#include <SqConfig.h>
#if SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif
#include <SqxcValue.h>
#include <SqConsole.h>

#ifdef _MSC_VER
#define strcasecmp   _stricmp
#endif

#define COMMAND_BORDER_WIDTH    2

// SqType SqCompareFunc
static int  sq_command_cmp_name(const void *commandType1, const void *commandType2);
static int  sq_command_cmp_str__name(const void *str,  const void *commandType);

SqConsole *sq_console_new()
{
	SqConsole *console;

	console = malloc(sizeof(SqConsole));
	sq_console_init(console);
	return console;
}

void  sq_console_free(SqConsole *console)
{
	sq_console_final(console);
	free(console);
}

void  sq_console_init(SqConsole *console)
{
	sq_ptr_array_init(&console->commands, 8, (SqDestroyFunc)sq_command_free);
	console->commands_sorted  = false;
	console->command_default  = NULL;

	console->program_name = NULL;
	console->xc_input = sqxc_new(SQXC_INFO_VALUE);
#if SQ_CONFIG_HAVE_JSONC
	// append JSON parser to tail of list
	sqxc_insert(console->xc_input,  sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
#endif
}

void  sq_console_final(SqConsole *console)
{
	sq_ptr_array_final(&console->commands);
	free(console->program_name);
	sqxc_free_chain(console->xc_input);
}

void  sq_console_add(SqConsole *console, const SqCommand *command_type)
{
	if (console->command_default == NULL)
		console->command_default = (SqCommand*)command_type;
	sq_ptr_array_push(&console->commands, (void*)command_type);
}

SqCommand  *sq_console_find(SqConsole *console, const char* command_name)
{
	void  **addr;

	if (console->commands_sorted == false) {
		console->commands_sorted =  true;
		sq_ptr_array_sort(&console->commands, sq_command_cmp_name);
	}
	addr = sq_ptr_array_search(&console->commands,
	               command_name, sq_command_cmp_str__name);
	if (addr)
		return *(SqCommand**)addr;
	return NULL;
}

SqCommandValue *sq_console_parse(SqConsole *console, int argc, char **argv, int argv_has_command)
{
	const SqCommand  *type;
	SqCommandValue   *commandValue;
	Sqxc       *xc;
	char       *arg;
	char       *equ;
	int         n_dash;

	if (argc < 2)
		return NULL;
	if (argv_has_command == -1) {
		// if argv[1] is an option, not a command
		if (argv[1][0] == '-')
			argv_has_command = 0;
		else
			argv_has_command = 1;
	}

	if (argv_has_command == 0) {
		// if argv_has_command == 0, use SqConsole::command_default as command
		type = console->command_default;
		argc -= 1;
		argv += 1;
	}
	else {
		type = sq_console_find(console, argv[1]);
		argc -= 2;
		argv += 2;
	}

	if (type == NULL)
		return NULL;

	commandValue = sq_command_value_new(type);
	sq_command_sort_shortcuts(type, &commandValue->shortcuts);

	xc = console->xc_input;
	sqxc_value_container(xc) = NULL;
	sqxc_value_element(xc) = (SqType*)type;
	sqxc_value_instance(xc) = commandValue;

	sqxc_ready(xc, NULL);
	xc->type = SQXC_TYPE_OBJECT;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);

	for (int i = 0;  i < argc;  i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-')  // option
				n_dash = 2;
			else                    // option shortcut
				n_dash = 1;

			arg = strdup(argv[i]+n_dash);
			if ((equ = strchr(arg, '=')) != NULL)
				*equ = 0;

			xc->type = SQXC_TYPE_STR;
			xc->name = arg;
			xc->value.str = (equ) ? equ + 1 : NULL;
			xc = sqxc_send(xc);
			free(arg);
		}
		else {
			// argument
			sq_ptr_array_push(&commandValue->arguments, argv[i]);
			continue;
		}
	}

	xc->type = SQXC_TYPE_OBJECT_END;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);
	sqxc_finish(xc, NULL);

	return commandValue;
}

void  sq_console_print_options(const SqConsole *console, SqOption **options, int  n_options)
{
	SqBuffer   buffer = {0};
	SqOption  *option;
	int        option_max_length = 0;
	int        length;

	// count max length
	for (int i = 0;  i < n_options;  i++) {
		option = options[i];
		length = sq_option_print(option, NULL, 0);
		if (option_max_length < length)
			option_max_length = length;
	}

	for (int j = 0;  j < n_options;  j++) {
		option = options[j];
		buffer.writed = 0;
		sq_option_print(option, &buffer, option_max_length);
		puts(buffer.mem);
	}
	sq_buffer_final(&buffer);
}

void  sq_console_print_help(const SqConsole *console, const SqCommand *commandType)
{
	bool  has_command = true;

	if (commandType == NULL) {
		commandType = console->command_default;
		has_command = false;
	}

	printf("\n");
	if (commandType == NULL) {
		printf("Unknown command"  "\n");
		return;
	}

	if (commandType->description) {
		printf("Description:"  "\n"
		       "%*c%s"         "\n\n",
		       2, ' ', commandType->description);
	}

	printf("Usage:"  "\n"  "%*c", 2, ' ');
	if (console->program_name)
		printf("%s ", console->program_name);
	if (commandType->name && has_command)
		printf("%s ", commandType->name);
	printf("[options] ");
	if (commandType->parameter)
		printf("%s ", commandType->parameter);
	printf("\n\n");

	printf("Options:"  "\n");
	sq_console_print_options(console, (SqOption**)commandType->entry, commandType->n_entry);
	printf("\n");
}

// print command list
void  sq_console_print_list(const SqConsole *console, const char *program_description)
{
	const SqCommand *commandType;
	int    command_max_length = 0;
	int    length;

	for (int i = 0;  i < console->commands.length;  i++) {
		commandType = console->commands.data[i];
		length = (int)strlen(commandType->name);
		if (command_max_length < length)
			command_max_length = length;
	}
	command_max_length += COMMAND_BORDER_WIDTH;

	if (program_description) {
		puts("");
		puts(program_description);
	}

	puts("\n" "Usage:");
	printf("%*c", COMMAND_BORDER_WIDTH, ' ');
	if (console->program_name)
		printf("%s ", console->program_name);
	puts("command [options] [arguments]\n");

	puts("Available commands:");
	for (int i = 0;  i < console->commands.length;  i++) {
		commandType = console->commands.data[i];
		printf("%*c", COMMAND_BORDER_WIDTH, ' ');
		printf("%s%*c", commandType->name,
		       command_max_length - (int)strlen(commandType->name), ' ');
		puts(commandType->description);
	}
	puts("");
}

// ------------------------------------
// SqType SqCompareFunc

// This function is used by sort(). Its actual parameter type:
//     int  sq_command_cmp_name(SqCommand **commandType1, SqCommand **commandType2);
static int  sq_command_cmp_name(const void *commandType1, const void *commandType2)
{
	const char *name1;
	const char *name2;

	name1 = (*(SqCommand**)commandType1) ? (*(SqCommand**)commandType1)->name : "";
	name2 = (*(SqCommand**)commandType2) ? (*(SqCommand**)commandType2)->name : "";
#if SQ_CONFIG_COMMAND_CASE_SENSITIVE
	return strcmp(name1, name2);
#else
	return strcasecmp(name1, name2);
#endif
}

// This function is used by find(). Its actual parameter type:
//     int  sq_command_cmp_str__name(const char *str, SqCommand **commandType);
static int  sq_command_cmp_str__name(const void *str, const void *commandType)
{
	const char *name;

	name = (*(SqCommand**)commandType) ? (*(SqCommand**)commandType)->name : "";
#if SQ_CONFIG_COMMAND_CASE_SENSITIVE
	return strcmp(str, name);
#else
	return strcasecmp(str, name);
#endif
}
