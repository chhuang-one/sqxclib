/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <stdio.h>

#include <SqConfig.h>
#include <SqxcValue.h>
#include <SqxcJsonc.h>
#include <SqConsole.h>

// SqType SqCompareFunc
static int  sq_type_cmp_name(SqType **type1, SqType **type2);
static int  sq_type_cmp_str__name(const char *str,  SqType **type);

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
	sq_ptr_array_init(console, 8, NULL);
	sq_buffer_init(&console->buf);

	console->xc_input = sqxc_new(SQXC_INFO_VALUE);
#ifdef SQ_CONFIG_HAVE_JSONC
	// append JSON parser to tail of list
	sqxc_insert(console->xc_input,  sqxc_new(SQXC_INFO_JSONC_PARSER), -1);
#endif
}

void  sq_console_final(SqConsole *console)
{
	sq_ptr_array_final(console);
	sq_buffer_final(&console->buf);
	sqxc_free_chain(console->xc_input);
}

void  sq_console_add(SqConsole *console, const SqCommandType *command_type)
{
	sq_ptr_array_append(console, (void*)command_type);
}

SqCommandType  *sq_console_find(SqConsole *console, const char* name)
{
	SqCommandType *type;

	if (console->sorted == false) {
		console->sorted =  true;
		sq_ptr_array_sort(console, (SqCompareFunc)sq_type_cmp_name);
	}
	type = sq_ptr_array_search(console, name, (SqCompareFunc)sq_type_cmp_str__name);
	if (type)
		type = *(SqCommandType**)type;
	return type;
}

SqCommand *sq_console_parse(SqConsole *console, int argc, char **argv, bool argv_has_command)
{
    const SqCommandType  *type;
	SqCommand  *cmd;
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
		type = console->commands[0];
		argc -= 1;
		argv += 1;
	}
	if (type == NULL)
		return NULL;

	cmd = sq_command_new(type);
	sq_command_sort_shortcuts(cmd);

	xc = console->xc_input;
	sqxc_value_container(xc) = NULL;
	sqxc_value_element(xc) = (SqType*)type;
	sqxc_value_instance(xc) = cmd;

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

			xc->type = SQXC_TYPE_STRING;
			xc->name = console->buf.mem;
			xc->value.string = (equ) ? equ + 1 : NULL;
			xc = sqxc_send(xc);
		}
		else {
			// argument
			sq_ptr_array_append(&cmd->arguments, argv[i]);
			continue;
		}
	}

	xc->type = SQXC_TYPE_OBJECT_END;
	xc->name = NULL;
	xc->value.pointer = NULL;
	xc = sqxc_send(xc);
	sqxc_finish(xc, NULL);

	return cmd;
}

void  sq_console_print_help(SqConsole  *console,
                            const char *command_name,
                            const char *program_name)
{
	const SqCommandType *cmdtype;
	SqOption  *option;

	if (command_name)
		cmdtype = sq_console_find(console, command_name);
	else
		cmdtype = console->commands[0];
	if (cmdtype == NULL)
		return;

	puts("");

	if (cmdtype->description) {
		puts("Description:");
		printf("  %s\n\n", cmdtype->description);
	}

	printf("Usage:\n" "  ");
	if (program_name)
		printf("%s ", program_name);
	if (command_name)
		printf("%s ", command_name);
	if (cmdtype->parameter)
		printf("%s ", cmdtype->parameter);
	printf("[options]" "\n\n");

	puts("Options:");
	for (int i = 0;  i < console->n_commands;  i++) {
		cmdtype = console->commands[i];
		for (int j = 0;  j < cmdtype->n_entry;  j++) {
			option = (SqOption*)cmdtype->entry[j];
			console->buf.writed = 0;
			sq_option_print(option, &console->buf, 0);
			puts(console->buf.mem);
		}
	}
	printf("\n\n");
}

// ------------------------------------
// SqType SqCompareFunc

static int  sq_type_cmp_name(SqType **type1, SqType **type2)
{
	const char *name1;
	const char *name2;

	name1 = (*type1) ? (*type1)->name : "";
	name2 = (*type2) ? (*type2)->name : "";
#if defined(SQ_CONFIG_COMMAND_CASE_SENSITIVE)
	return strcmp(name1, name2);
#else
	return strcasecmp(name1, name2);
#endif
}

static int  sq_type_cmp_str__name(const char *str,  SqType **type)
{
	const char *name;

	name = (*type) ? (*type)->name : "";
#if defined(SQ_CONFIG_COMMAND_CASE_SENSITIVE)
	return strcmp(name1, name2);
#else
	return strcasecmp(str, name);
#endif
}
