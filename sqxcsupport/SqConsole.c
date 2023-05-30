/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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
static int  sq_command_cmp_name(SqCommand **cmd_type1, SqCommand **cmd_type2);
static int  sq_command_cmp_str__name(const char *str,  SqCommand **cmd_type);

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
	sq_buffer_init(&console->buf);
	sq_ptr_array_init(&console->commands, 8, (SqDestroyFunc)sq_command_free);
	console->commands.data[0] = NULL;    // default command
	console->commands_sorted  = false;

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
	sq_buffer_final(&console->buf);
}

void  sq_console_add(SqConsole *console, const SqCommand *command_type)
{
	sq_ptr_array_push(&console->commands, (void*)command_type);
}

SqCommand  *sq_console_find(SqConsole *console, const char* command_name)
{
	void  **addr;

	if (console->commands_sorted == false) {
		console->commands_sorted =  true;
		sq_ptr_array_sort(&console->commands, (SqCompareFunc)sq_command_cmp_name);
	}
	addr = sq_ptr_array_search(&console->commands, command_name,
	                           (SqCompareFunc)sq_command_cmp_str__name);
	if (addr)
		return *(SqCommand**)addr;
	return NULL;
}

SqCommandValue *sq_console_parse(SqConsole *console, int argc, char **argv, bool argv_has_command)
{
    const SqCommand  *type;
	SqCommandValue   *cmd_value;
	Sqxc       *xc;
	char       *equ;
	int         n_dash;

	if (argc < 2)
		return NULL;
	if (argv_has_command) {
		type = sq_console_find(console, argv[1]);
		argc -= 2;
		argv += 2;
	}
	else {
		type = console->commands.data[0];    // default command
		argc -= 1;
		argv += 1;
	}
	if (type == NULL)
		return NULL;

	cmd_value = sq_command_value_new(type);
	sq_command_sort_shortcuts(type, &cmd_value->shortcuts);

	xc = console->xc_input;
	sqxc_value_container(xc) = NULL;
	sqxc_value_element(xc) = (SqType*)type;
	sqxc_value_instance(xc) = cmd_value;

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

			sq_buffer_write(&console->buf, argv[i]+n_dash);
			if ((equ = strchr(console->buf.mem, '=')) != NULL)
				*equ = 0;

			xc->type = SQXC_TYPE_STR;
			xc->name = console->buf.mem;
			xc->value.str = (equ) ? equ + 1 : NULL;
			xc = sqxc_send(xc);
		}
		else {
			// argument
			sq_ptr_array_push(&cmd_value->arguments, argv[i]);
			continue;
		}
	}

	xc->type = SQXC_TYPE_OBJECT_END;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);
	sqxc_finish(xc, NULL);

	return cmd_value;
}

void  sq_console_print_options(SqConsole *console, SqOption **options, int  n_options)
{
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
		console->buf.writed = 0;
		sq_option_print(option, &console->buf, option_max_length);
		puts(console->buf.mem);
	}
}

void  sq_console_print_help(SqConsole  *console, const SqCommand *cmd_type)
{
	if (cmd_type == NULL)
		cmd_type = (SqCommand*)console->commands.data[0];    // default command

	puts("");
	if (cmd_type == NULL) {
		puts("Unknown command");
		return;
	}

	if (cmd_type->description) {
		puts("Description:");
		printf("%*c%s\n\n", 2, ' ', cmd_type->description);
	}

	puts("Usage:");
	printf("%*c", 2, ' ');
	if (console->program_name)
		printf("%s ", console->program_name);
	if (cmd_type->name)
		printf("%s ", cmd_type->name);
	if (cmd_type->parameter)
		printf("%s ", cmd_type->parameter);
	puts("[options]" "\n");

	puts("Options:");
	sq_console_print_options(console, (SqOption**)cmd_type->entry, cmd_type->n_entry);
	puts("\n");
}

// print command list
void  sq_console_print_list(SqConsole  *console, const char *program_description)
{
	const SqCommand *cmd_type;
	int    command_max_length = 0;
	int    length;

	for (int i = 0;  i < console->commands.length;  i++) {
		cmd_type = console->commands.data[i];
		length = (int)strlen(cmd_type->name);
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
		cmd_type = console->commands.data[i];
		printf("%*c", COMMAND_BORDER_WIDTH, ' ');
		printf("%s%*c", cmd_type->name,
		       command_max_length - (int)strlen(cmd_type->name), ' ');
		puts(cmd_type->description);
	}
}

// ------------------------------------
// SqType SqCompareFunc

static int  sq_command_cmp_name(SqCommand **cmd_type1, SqCommand **cmd_type2)
{
	const char *name1;
	const char *name2;

	name1 = (*cmd_type1) ? (*cmd_type1)->name : "";
	name2 = (*cmd_type2) ? (*cmd_type2)->name : "";
#if SQ_CONFIG_COMMAND_CASE_SENSITIVE
	return strcmp(name1, name2);
#else
	return strcasecmp(name1, name2);
#endif
}

static int  sq_command_cmp_str__name(const char *str,  SqCommand **cmd_type)
{
	const char *name;

	name = (*cmd_type) ? (*cmd_type)->name : "";
#if SQ_CONFIG_COMMAND_CASE_SENSITIVE
	return strcmp(str, name);
#else
	return strcasecmp(str, name);
#endif
}
