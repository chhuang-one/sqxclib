/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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
#include <time.h>             // time_t, struct tm
#include <stdio.h>            // FILE
#include <ctype.h>            // toupper()
#include <string.h>           // strchr()
#include <SqError.h>
#include <SqConvert.h>        // sq_time_to_string()
#include <SqHelpers.h>        // sq_str_type_name()
#include <SqBuffer.h>
#include <SqAppTool.h>
#include <SqAppTool-config.h>
#include <SqAppOptions.h>
#include <SqAppCommands.h>

#ifdef _MSC_VER
#define strdup       _strdup
#endif

// ----------------------------------------------------------------------------
// SqListOptions

typedef struct SqListOptions       SqListOptions;

/*	SqListOptions:

	SqCommonOptions
	|
	`--- SqListOptions
 */

struct SqListOptions
{
	SQ_COMMON_OPTIONS_MEMBERS;
/*	// ------ SqCommonOptions members ------
	bool           help;
	bool           quiet;
	bool           version;    // Display this application version
 */

	// ------ SqListOptions members ------
	// none
};

// ----------------------------------------------------------------------------
// list

static void list(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	SqListOptions *options = (SqListOptions*)commandValue->options;
//	SqAppTool *app = data;
//	console = app->console;

	if (options->help) {
		sq_console_print_help(console, commandValue->type);
		return;
	}

	sq_console_print_list(console, NULL);
}

static const SqOption *list_options[] = {
	SQ_COMMON_OPTIONS_HELP,
};

static const SqCommand list_command = SQ_COMMAND_INITIALIZER(
	SqListOptions,                                 // StructureType
	0,                                             // bit_field
	"list",                                        // command string
	list_options,                                  // pointer array of SqOption
	list,                                          // handle function
	NULL,                                          // parameter string
	"lists all commands"                           // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand list_command = {
	// --- SqType members ---
	.size  = sizeof(SqListOptions),
	.parse = sq_command_parse_option,
	.write = sq_type_object_write,
	.name  = "list",
	.entry   = (SqEntry**) list_options,
	.n_entry = sizeof(list_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,
	// --- SqCommand members ---
	.handle      = (SqCommandFunc) list,
	.parameter   = NULL,
	.description = "lists all commands",
};
 */

// ----------------------------------------------------------------------------
/* --- SqAppTool functions --- */

void   sq_app_tool_init(SqAppTool *app, const char *program_name, const struct SqAppSetting *setting)
{
	sq_app_init((SqApp*)app, setting);

	// console
	app->console = sq_console_new();
	app->console->program_name = strdup(program_name);
	sq_console_add(app->console, &list_command);
	sq_console_add_command_migrate(app->console);
	sq_console_add_command_make(app->console);
	// Key-Value pairs for temporary use
	sq_pairs_init(&app->pairs, sq_pairs_cmp_string);
	app->pairs.key_destroy_func   = NULL;
	app->pairs.value_destroy_func = free;
	// buffer for temporary use
	sq_buffer_init(&app->buffer);

	app->template_extension = ".c.txt";
	app->path = NULL;
}

void   sq_app_tool_final(SqAppTool *app)
{
	sq_console_free(app->console);
	sq_pairs_final(&app->pairs);
	sq_buffer_final(&app->buffer);
	free(app->path);    // free workspace folder

	sq_app_final((SqApp*)app);
}

int    sq_app_tool_run(SqAppTool *app, int argc, char **argv)
{
	SqConsole      *console;
	SqCommandValue *commandValue;

	console = app->console;
	// if there is only program name in 'argv'
	if (argc < 2) {
		sq_console_print_list(console, NULL);
		return SQCODE_OK;
	}

	commandValue = sq_console_parse(console, argc, argv, SQ_CONSOLE_PARSE_ALL);
	if (commandValue == NULL) {
		printf("\n"
		       "Unknown command '%s'\n\n",
		       (argc > 1) ? argv[1] : NULL);
		return SQCODE_ERROR;
	}

	// decide workspace folder
	sq_app_tool_decide_path(app);
	// handle command
	commandValue->type->handle(commandValue, console, app);
	sq_command_value_free(commandValue);
	return SQCODE_OK;
}

void   sq_app_tool_set_path(SqAppTool *app, const char *path)
{
	free(app->path);
	app->path = strdup(path);
}

int    sq_app_tool_decide_path(SqAppTool *app)
{
	SqBuffer *buf = &app->buffer;
	FILE     *file = NULL;
	char     *array[] = {".", "..", "../..", SQ_APP_TOOL_PATH_BASE, NULL};

	buf->writed = 0;
	// workspace folder
	for (int i = 0;  array[i];  i++) {
		sq_buffer_write(buf, array[i]);
		sq_buffer_write(buf, SQ_APP_TOOL_PATH_TEMPLATES);
		sq_buffer_write_c(buf, '/');
		sq_buffer_write(buf, "migration-create.c.txt");
		file = fopen(buf->mem, "r");
		buf->writed = 0;    // clear app->buffer
		if (file) {
			app->path = strdup(array[i]);
			fclose(file);
			break;
		}
	}
	if (app->path == NULL) {
		app->path = strdup(array[0]);
		puts("workspace folder not found");
		return SQCODE_ERROR;
	}
	printf("workspace folder = %s\n", app->path);
	return SQCODE_OK;
}

int    sq_app_tool_make_migration(SqAppTool  *app,
                                  const char *template_filename,
                                  const char *migration_name,
                                  SqPairs    *pairs)
{
	SqBuffer *buf = &app->buffer;
	int       code = SQCODE_OK;
	char     *table_name;
	char     *struct_name;
	char     *migration_name_camel;
	struct {
		unsigned int  table_name:1;
		unsigned int  struct_name:1;
		unsigned int  migration_name:1;
		unsigned int  migration_name_camel:1;
		unsigned int  timestr:1;
	} added = {0};
	union {
//		FILE *file;
		char *path;
	} in;
	union {
		FILE *file;
		char *path;
	} out;
	union {
		char *snake_case;
		char *timestr;
		size_t  len;
	} temp;


	if (sq_pairs_get(pairs, "migration_name") == NULL) {
		sq_pairs_add(pairs, "migration_name", strdup(migration_name));
		added.migration_name = 1;
	}
	migration_name_camel = sq_pairs_get(pairs, "migration_name_camel");
	if (migration_name_camel == NULL) {
		temp.snake_case = sq_pairs_get(pairs, "migration_name");
		migration_name_camel = sq_str_camel(temp.snake_case, false);
		sq_pairs_add(pairs, "migration_name_camel", migration_name_camel);
		added.migration_name_camel = 1;
	}

	table_name = sq_pairs_get(pairs, "table_name");
	if (table_name == NULL) {
		table_name = strchr(migration_name, '_');
		// if 'table_name' found and 'table_name' is NOT empty string
		if (table_name && table_name[1]) {
			table_name++;
			temp.len = strcspn(table_name, "_");
			// handle long table name
			if (table_name[temp.len] != 0) {
				for (;;) {
					size_t cur_len = strcspn(table_name + temp.len + 1, "_");
					if (table_name[temp.len + 1 + cur_len] == 0)
						break;
					temp.len = temp.len + 1 + cur_len;    // temp.len + '_' + cur_len
				}
			}
			table_name = strncpy(malloc(temp.len+1), table_name, temp.len);
			table_name[temp.len] = 0;
		}
		else
			table_name = strdup("your_table_name");
		sq_pairs_add(pairs, "table_name", table_name);
		added.table_name = 1;
	}

	struct_name = sq_pairs_get(pairs, "struct_name");
	if (struct_name == NULL) {
		if (table_name)
			struct_name = sq_str_type_name(table_name);
		else
			struct_name = strdup("YourStructName");
		sq_pairs_add(pairs, "struct_name", struct_name);
		added.struct_name = 1;
	}

	temp.timestr = sq_pairs_get(pairs, "timestamp");
	if (temp.timestr == NULL) {
		temp.timestr = sq_time_to_string(time(NULL), 'c');
		sq_pairs_add(pairs, "timestamp", temp.timestr);
		added.timestr = 1;
	}

	buf->writed = 0;
	sq_buffer_write(buf, app->path);    // workspace folder
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_MIGRATIONS);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, temp.timestr);
	sq_buffer_write_c(buf, '_');
	sq_buffer_write(buf, migration_name);
	// strcspn() count length of template file extension
	sq_buffer_write_len(buf, app->template_extension,
	                    strcspn(app->template_extension+1, ".") + 1);
	buf->mem[buf->writed] = 0;
	out.path = strdup(buf->mem);

	buf->writed = 0;
	sq_buffer_write(buf, app->path);    // workspace folder
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_TEMPLATES);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, template_filename);
	sq_buffer_write(buf, app->template_extension);
	buf->mem[buf->writed] = 0;
	in.path = strdup(buf->mem);

	code = sq_template_write_file(in.path, pairs, out.path);
	free(in.path);
	free(out.path);
	if (code != SQCODE_OK)
		goto exit;

	// relative path of migration file for SqApp
	buf->writed = 0;
	sq_buffer_write(buf, "..");    // current dir of sqxcapp is "workspace folder/sqxcapp"
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_MIGRATIONS);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, temp.timestr);
	sq_buffer_write_c(buf, '_');
	sq_buffer_write(buf, migration_name);
	// strcspn() count length of template file extension
	sq_buffer_write_len(buf, app->template_extension,
	                    strcspn(app->template_extension+1, ".") + 1);
	buf->mem[buf->writed] = 0;
	in.path = strdup(buf->mem);

	// append filename in migrations-files.c (or .cpp)
	buf->writed = 0;
	sq_buffer_write(buf, app->path);    // workspace folder
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_SOURCE);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, "migrations-files");
	// strcspn() count length of template file extension
	sq_buffer_write_len(buf, app->template_extension,
	                    strcspn(app->template_extension+1, ".") + 1);
	buf->mem[buf->writed] = 0;
	out.file = fopen(buf->mem, "a");
	if (out.file == NULL) {
		printf("\n" "Can't open file - %s" "\n", buf->mem);
		code = SQCODE_ERROR;
	}
	else {
		fprintf(out.file, "#include \"%s\"" "\n", in.path);
		fclose(out.file);
	}

	// append element in migrations-declarations
	buf->writed = 0;
	sq_buffer_write(buf, app->path);    // workspace folder
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_SOURCE);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, "migrations-declarations");
	buf->mem[buf->writed] = 0;
	out.file = fopen(buf->mem, "a");
	if (out.file == NULL) {
		printf("\n" "Can't open file - %s" "\n", buf->mem);
		code = SQCODE_ERROR;
	}
	else {
		// print comment and declaration
		fprintf(out.file,
		        "\n"
		        "/* defined in %s */" "\n"
		        "extern const SqMigration  %s_%s;" "\n",
		        in.path,
		        migration_name_camel, temp.timestr);
		fclose(out.file);
	}

	// append element in migrations-elements
	buf->writed = 0;
	sq_buffer_write(buf, app->path);    // workspace folder
	sq_buffer_write(buf, SQ_APP_TOOL_PATH_SOURCE);
	sq_buffer_write_c(buf, '/');
	sq_buffer_write(buf, "migrations-elements");
	buf->mem[buf->writed] = 0;
	out.file = fopen(buf->mem, "a");
	if (out.file == NULL) {
		printf("\n" "Can't open file - %s" "\n", buf->mem);
		code = SQCODE_ERROR;
	}
	else {
		// print comment and element
		fprintf(out.file,
		        "\n"
		        "/* defined in %s */" "\n"
		        "& %s_%s," "\n",
		        in.path,
		        migration_name_camel, temp.timestr);
		fclose(out.file);
	}

	free(in.path);

exit:
	if (added.migration_name)
		sq_pairs_erase(pairs, "migration_name");
	if (added.migration_name_camel)
		sq_pairs_erase(pairs, "migration_name_camel");
	if (added.struct_name)
		sq_pairs_erase(pairs, "struct_name");
	if (added.table_name)
		sq_pairs_erase(pairs, "table_name");
	if (added.timestr)
		sq_pairs_erase(pairs, "timestamp");
	return code;
}

// ----------------------------------------------------------------------------
/* --- template functions --- */

char *sq_template_write_buffer(const char *template_string, SqPairs *pairs, SqBuffer *buffer)
{
	char *temp_beg;
	char *temp_end;
	char *str;
	size_t  writed_pos;
	size_t  len;

	writed_pos = buffer->writed;

	for (;;) {
		// find template
		temp_beg = strstr(template_string, "{{");
		if (temp_beg)
			temp_end = strstr(temp_beg, "}}");
		else
			temp_end = NULL;

		// if template not found
		if (temp_end == NULL) {
			sq_buffer_write(buffer, template_string);
			break;
		}

		sq_buffer_write_len(buffer, template_string, temp_beg - template_string);
		// + "{{"
		temp_beg += 2;
		// get key
		while(temp_beg[0] == ' ')
			temp_beg++;
		len = strcspn(temp_beg, " }");
		str = sq_buffer_write_len(buffer, temp_beg, len);
		str[len] = 0;
		buffer->writed -= len;
		// get value
		str = sq_pairs_get(pairs, str);
		if (str)
			sq_buffer_write(buffer, str);
		// + "}}"
		template_string = temp_end + 2;
	}

	buffer->mem[buffer->writed] = 0;    // NULL-terminated is not counted in length
	return buffer->mem + writed_pos;
}

int   sq_template_write_file(const char *template_file, SqPairs *pairs, const char *result_file)
{
	FILE     *file_in;
	FILE     *file_out;
	SqBuffer  buffer;
	char     *temp;

	file_in  = fopen(template_file, "r");
	if (file_in == NULL)
		return SQCODE_ERROR;
	file_out = fopen(result_file, "w");
	if (file_out == NULL) {
		fclose(file_in);
		return SQCODE_ERROR;
	}

	temp = malloc(512);
	sq_buffer_init(&buffer);
	sq_buffer_resize(&buffer, 1024);

	while(fgets(temp, 512, file_in) != NULL) {
		sq_template_write_buffer(temp, pairs, &buffer);
		fwrite(buffer.mem, buffer.writed, 1, file_out);
		buffer.writed = 0;
	}

	sq_buffer_final(&buffer);
	free(temp);
	fclose(file_in);
	fclose(file_out);
	return SQCODE_OK;
}
