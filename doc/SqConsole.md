[中文](SqConsole.cn.md)

# SqConsole

SqConsole provide command-line interface (mainly for SqAppTool). It must use with SqCommand and SqOption.  
Note: SqConsole is in sqxcsupport library.  
  
Relationship of SqConsole, SqCommand, and SqOption.

	           1 : N               1 : N
	SqConsole ---┬--- SqCommand 1 ---┬--- SqOption 1-1
	             │                   │
	             │                   └--- SqOption 1-2
	             │
	             └--- SqCommand 2 ---┬--- SqOption 2-1
	                                 │
	                                 └--- SqOption 2-2

# SqCommand

User can use SqCommand to define constant (or dynamic) command and it's options.
SqConsole use this to parse data from command-line and store parsed data in SqCommandValue.

	SqType
	│
	└--- SqCommand

# SqOption

It defines option in command. Because it derives from [SqEntry](SqEntry.md), you can set SQB_POINTER in SqOption::bit_field if the option value in C/C++ language is a pointer type.

	SqEntry
	│
	└--- SqOption

# SqCommandValue

SqCommandValue stores option values and arguments from command-line.

## Define New Command

e.g. define 'mycommand' that has 3 options - 'help', 'quiet', and 'step'.  
  
First define structure MyCommandOptions for options of 'mycommand'.

```c
struct MyCommandOptions
{
	bool    help;
	bool    quiet;
	int     step;
};
```

#### define function of command handler

```c++
// The function will be called when your command is executed.
void  mycommand_handle(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	MyCommandOptions *options = (MyCommandOptions*)commandValue->options;

	// do something here...
}
```

#### 1.1 define constant options of command

If you define constant SqCommand, it must use with **pointer array** of SqOption.

```c
static const SqOption  mycommand_option_array[] = {
	{SQ_TYPE_BOOL,  "help",      offsetof(MyCommandOptions, help),
		.shortcut = "h",  .default_value = "true",
		.description = "Display help for the given command."},

	{SQ_TYPE_BOOL,  "quiet",     offsetof(MyCommandOptions, quiet),
		.shortcut = "q",  .default_value = "true",
		.description = "Do not output any message."},

	{SQ_TYPE_INT,   "step",      offsetof(MyCommandOptions, step),
		.shortcut = "s",  .default_value = "1",
		.description = "Take step."},
};

static const SqOption *mycommand_options[] = {
	& mycommand_option_array[0],
	& mycommand_option_array[1],
	& mycommand_option_array[2],
};
```

Setting SQB_POINTER in SqOption::bit_field if the option value in C/C++ language is a pointer type.

```c
const SqOption  options[] = {
	{SQ_TYPE_MY_OBJECT,  "obj",  offsetof(MyCommandOptions, obj),  SQB_POINTER},
	//                                                             ^^^^^^^^^^^

	// Omitted
}
```

#### 1.2 define constant command

```c++
const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandOptions,                              // StructureType
	0,                                             // bit_field
	"mycommand",                                   // command name
	mycommand_options,                             // pointer array of SqOption
	mycommand_handle,                              // handle function
	"mycommand parameterName",                     // parameter string
	"mycommand description"                        // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
const SqCommand mycommand = {
	// --- SqType members ---
	.size  = sizeof(MyCommandOptions),
	.parse = sq_command_parse_option,
	.name  = "mycommand",
	.entry   = (SqEntry**) mycommand_options,
	.n_entry = sizeof(mycommand_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,

	// --- SqCommand members ---
	.handle      = (SqCommandFunc) mycommand_handle,
	.parameter   = "mycommand parameterName",
	.description = "mycommand description",
};
 */
```

#### 2.1 define a new command dynamically

e.g. create 'mycommand' by function.  
  
use C language

```c
	SqCommand *mycommand;

	mycommand = sq_command_new("mycommand");
	mycommand->size   = sizeof(MyCommandOptions);
	mycommand->handle = mycommand_handle;
	sq_command_set_parameter(mycommand, "mycommand parameterName");
	sq_command_set_description(mycommand, "mycommand description");
```

use C++ language

```c++
	Sq::Command *mycommand;

	mycommand = new Sq::Command("mycommand");
	mycommand->size   = sizeof(MyCommandOptions);
	mycommand->handle = mycommand_handle;
	mycommand->setParameter("mycommand parameterName");
	mycommand->setDescription("mycommand description");
```

#### 2.2 dynamic SqCommand use constant array of SqOption

e.g. add array that has 2 options.

```c++
	// C function
	sq_command_add_option(mycommand, mycommand_option_array, 2);

	// C++ method
	mycommand->addOption(mycommand_option_array, 2);
```

#### 2.3 dynamic SqCommand use dynamic SqOption

use C language

```c
	SqOption  *option;

	option = sq_option_new(SQ_TYPE_BOOL);
	option->offset = offsetof(MyCommandOptions, help);
	sq_option_set_name(option, "help");
	sq_option_set_shortcut(option, "h");
	sq_option_set_default_value(option, "true");
	sq_option_set_description(option, "Display help for the given command.");

	sq_command_add_option(mycommand, option, 1);
```

use C++ language

```c++
	Sq::Option  *option;

	option = new Sq::Option(SQ_TYPE_BOOL);
	option->offset = offsetof(MyCommandOptions, help);
	option->setName("help");
	option->setShortcut("h");
	option->setDefault("true");
	option->setDescription("Display help for the given command.");

	mycommand->addOption(option);
```

Calling pointer() if the option value in C/C++ language is a pointer type.

```c++
	// C function
	sq_option_pointer(option);

	// C++ method
	option->pointer();
```

## Add command to SqConsole

```c
	// C function
	sq_console_add(console, &mycommand);

	// C++ method
	console->add(&mycommand)
```

## Parse command-line

e.g. execute program with specified command, options, and arguments.

```console
program  mycommand  --step=5  argument1  argument2
```

Calling parse() to parse command-line arguments.  
  
use C language

```c
int  main(int argc, char **argv)
{
	SqCommandValue *commandValue;
	bool            command_in_argv = true;

	commandValue = sq_console_parse(console, argc, argv, command_in_argv);
}
```

use C++ language

```c++
int  main(int argc, char **argv)
{
	Sq::CommandValue *commandValue;
	bool              command_in_argv = true;

	commandValue = console->parse(argc, argv, command_in_argv);
}
```

The return value of sq_console_parse() is 'commandValue', it should look like this:

```c
	commandValue->type = mycommand;

	commandValue->arguments.data[0] = "argument1";
	commandValue->arguments.data[1] = "argument2";
	commandValue->arguments.length  = 2;

	MyCommandOptions *options = commandValue->options;
	options->help  = false;
	options->quiet = false;
	options->step  = 5;
```

If you only need options and arguments and no command, calling parse() and specify the last argument as false.
In this case, SqConsole use first added command by default.  
  
e.g. Execute the program with only options and parameters, no command specified.

```console
program  --step=5  argument1  argument2
```

Call parse() and specify the last argument as false to parse command line arguments without a command.

```c++
	// C function
	commandValue = sq_console_parse(console, argc, argv, false);

	// C++ method
	commandValue = console->parse(argc, argv, false);
```

**release memory of 'commandValue'**  
  
use C language

```c
	SqCommandValue *commandValue;

	sq_command_value_free(commandValue);
```

use C++ language  
  
C++ Sq::CommandValue has defined destructor, so user can use 'delete' keyword to free memory.

```c++
	Sq::CommandValue *commandValue;

	delete commandValue;
	// or
	// commandValue->free();
```

## Print help message

To print help message, you can use C function sq_console_print_help(), C++ method printHelp().  
If 'command' is NULL, they use first added SqCommand to print help message without name of command.

```c++
	SqCommand*  command = mycommand;

	// C function
	sq_console_print_help(console, command);

	// C++ method
	console->printHelp(command);
```

When your program has multiple command, you can use printList() to list all commands added in SqConsole.

```c++
	const char *program_description = "Program description, which is printed before other help messages.";

	// C function
	q_console_print_list(console, program_description);

	// C++ method
	console->printList(program_description);
```
