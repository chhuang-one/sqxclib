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

It stores value of option from command-line.

## 1 Define a constant command

e.g. define 'mycommand' that has two options - '--help' and '--quiet'

#### 1.1 define value of command option

Define MyCommandValue that is derived from SqCommandValue.

	SqCommandValue
	│
	└--- MyCommandValue

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct MyCommandValue    MyCommandValue;

#ifdef __cplusplus
struct MyCommandValue : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
struct MyCommandValue
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                      // <-- 2. inherit member variable

	// The following are option values.

	// ------ MyCommandValue members ------        // <-- 3. Add variable and non-virtual function in derived struct.
	bool    help;
	bool    quiet;

	int     step;
};
```

#### 1.2 define constant options of command

If you define constant SqCommand, it must use with **pointer array** of SqOption.

```c
static const SqOption  mycommand_option_array[] = {
	{SQ_TYPE_BOOL,  "help",      offsetof(MyCommandValue, help),
		.shortcut = "h",  .default_value = "true",
		.description = "Display help for the given command."},

	{SQ_TYPE_BOOL,  "quiet",     offsetof(MyCommandValue, quiet),
		.shortcut = "q",  .default_value = "true",
		.description = "Do not output any message."},

	{SQ_TYPE_INT,   "step",      offsetof(MyCommandValue, step),
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
	{SQ_TYPE_MY_OBJECT,  "obj",      offsetof(MyCommandValue, obj),  SQB_POINTER},
	//                                                               ^^^^^^^^^^^

	// Omitted
}
```

#### 1.3 define function of command handler

```c++
static void mycommand_handle(MyCommandValue *commandValue, SqConsole *console, void *data)
{
	// The function will be called when your command is executed.
}
```

#### 1.4 define constant command

```c++
const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandValue,                                // StructureType
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
	.size  = sizeof(MyCommandValue),
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

## 2 Define a new command dynamically

e.g. create 'mycommand' by function.  
  
use C language

```c
	SqCommand *mycommand;

	mycommand = sq_command_new("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	sq_command_set_parameter(mycommand, "mycommand parameterName");
	sq_command_set_description(mycommand, "mycommand description");
```

use C++ language

```c++
	Sq::Command *mycommand;

	mycommand = new Sq::Command("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	mycommand->setParameter("mycommand parameterName");
	mycommand->setDescription("mycommand description");
```

#### 2.1 dynamic SqCommand use constant array of SqOption

e.g. add array that has 2 options.

```c++
	// C function
	sq_command_add_option(mycommand, mycommand_option_array, 2);

	// C++ method
	mycommand->addOption(mycommand_option_array, 2);
```

#### 2.2 dynamic SqCommand use dynamic SqOption

use C language

```c
	SqOption  *option;

	option = sq_option_new(SQ_TYPE_BOOL);
	option->offset = offsetof(MyCommandValue, help);
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
	option->offset = offsetof(MyCommandValue, help);
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

## 3 add command to SqConsole

```c
	// C function
	sq_console_add(console, &mycommand);

	// C++ method
	console->add(&mycommand)
```

## 4 Parse command-line

e.g. execute program with specified command, options, and arguments.

```console
program  mycommand  --step=5  argument1  argument2
```

Calling parse() to parse command-line arguments.

```c++
int  main(int argc, char **argv)
{
	MyCommandValue *commandValue;
	bool            command_in_argv = true;

	// C function
	commandValue = sq_console_parse(console, argc, argv, command_in_argv);

	// C++ method
	commandValue = console->parse(argc, argv, command_in_argv);
}
```

values of 'commandValue' should look like this:

```c
	commandValue->type = mycommand;

	commandValue->arguments.data[0] = "argument1";
	commandValue->arguments.data[1] = "argument2";
	commandValue->arguments.length  = 2;

	commandValue->help  = false;
	commandValue->quiet = false;
	commandValue->step  = 5;
```

e.g. execute program without specified command. In this case, SqConsole use first added command by default.

```console
program  --step=5  argument1  argument2
```

Call parse() and specify the last argument as false to parse command line arguments without a command.

```c++
int  main(int argc, char **argv)
{
	// C function
	commandValue = sq_console_parse(console, argc, argv, false);

	// C++ method
	commandValue = console->parse(argc, argv, false);
}
```

**release memory of 'commandValue'**  
  
use C language

```c
	sq_command_value_free(commandValue);
```

use C++ language  
  
If 'MyCommandValue' has defined destructor, you can use C++ keywords "delete" to release memory.  
The following example does not derive methods from Sq::CommandValueMethod, but it still works.

```c++
struct MyCommandValue
{
	// inherit member variable from SqCommandValue
	SQ_COMMAND_VALUE_MEMBERS;

	// member variable (option values)
	bool    help;
	bool    quiet;
	int     step;

	// destructor
	~MyCommandValue() {
		sq_command_value_final((SqCommandValue*)this);
	}
};

int  main(int argc, char **argv)
{
	MyCommandValue *commandValue;

	// parse values from command-line
	commandValue = (MyCommandValue*)console->parse(argc, argv, true);
	// delete MyCommandValue instance
	delete commandValue;
}
```

If 'MyCommandValue' does not have a destructor defined, the variable must be cast to Sq::CommandValue and then delete instance, or call Sq::CommandValueMethod::free() method to free instance memory.

```c++
	// cast to Sq::CommandValue and then delete instance
	delete (Sq::CommandValue*)commandValue;
	// or call Sq::CommandValueMethod::free()
	commandValue->free();
```
