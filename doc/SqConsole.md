# SqConsole

SqConsole provide command-line interface (mainly for SqAppTool). It must use with SqCommand and SqOption.  
Note: SqConsole is in sqxcsupport library.  
  
Relationship of SqConsole, SqCommand, and SqOption.

	SqConsole ---┬--- SqCommand 1 ---┬--- SqOption 1
	             │                   │
	             │                   └--- SqOption n
	             │
	             └--- SqCommand n ---  ...

# SqCommand

User can use SqCommand to define command and it's options statically (or dynamically).
SqConsole use this to parse data from command-line and store parsed data in SqCommandValue.

	SqType
	│
	└--- SqCommand

# SqOption
define an option for command

	SqEntry
	│
	└--- SqOption

## 1 Define a new command statically

define 'mycommand' that has two options - '--help' and '--quiet'

#### 1.1 define value of command

	SqCommandValue
	│
	└--- MyCommandValue

```c++
// add this line if you use C language
typedef struct MyCommandValue    MyCommandValue;

#ifdef __cplusplus
struct MyCommandValue : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
struct MyCommandValue
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                      // <-- 2. inherit member variable

	bool    help;                                  // <-- 3. Add variable and non-virtual function in derived struct.
	bool    quiet;
};
```

#### 1.2 define options of command statically
* Note: If you define constant SqCommand, it must use with **pointer array** of SqOption.

```c
static const SqOption  mycommand_option_array[] = {
	{SQ_TYPE_BOOL,  "help",      offsetof(MyCommandValue, help),
		.shortcut = "h",  .default_value = "true",
		.description = "Display help for the given command."},

	{SQ_TYPE_BOOL,  "quiet",     offsetof(MyCommandValue, quiet),
		.shortcut = "q",  .default_value = "true",
		.description = "Do not output any message."},
};

static const SqOption *mycommand_options[] = {
	& mycommand_option_array[0],
	& mycommand_option_array[1],
};
```

#### 1.3 define function of command handler

```c++
static void mycommand_handle(MyCommandValue *cmd_value, SqConsole *console, void *data)
{
	// The function will be called when your command is executed.
}
```

#### 1.4 define command statically

```c++
static const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandValue,                                // StructureType
	0,                                             // bit_field
	"mycommand",                                   // command string
	mycommand_options,                             // pointer array of SqOption
	mycommand_handle,                              // handle function
	"mycommand parameterName",                     // parameter string
	"mycommand description"                        // description string
);

/* above SQ_COMMAND_INITIALIZER() Macro Expands to
static const SqCommand mycommand = {
	// --- SqType members ---
	.size  = sizeof(MyCommandValue),
	.parse = sq_command_parse_option,
	.name  = "mycommand",
	.entry   = (SqEntry**) mycommand_options,
	.n_entry = sizeof(mycommand_options) / sizeof(SqOption*),
	.bit_field = 0,

	// --- SqCommand members ---
	.handle      = (SqCommandFunc) mycommand_handle,
	.parameter   = "mycommand parameterName",
	.description = "mycommand description",
};
 */
```

## 2 Define a new command dynamically

use C language

```c
	SqOption  *option;
	SqCommand *mycommand;

	mycommand = sq_command_new("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	mycommand->parameter   = strdup("mycommand parameterName");
	mycommand->description = strdup("mycommand description");

	option = sq_option_new(SQ_TYPE_BOOL);
	option->offset        = offsetof(MyCommandValue, help);
	option->name          = strdup("help");
	option->shortcut      = strdup("h");
	option->default_value = strdup("true");
	option->description   = strdup("Display help for the given command.");

	sq_command_add_option(mycommand, option);
```

use C++ language

```c++
	Sq::Option  *option;
	Sq::Command *mycommand;

	mycommand = new Sq::Command("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	mycommand->parameter   = strdup("mycommand parameterName");
	mycommand->description = strdup("mycommand description");

	option = new Sq::Option(SQ_TYPE_BOOL);
	option->offset        = offsetof(MyCommandValue, help);
	option->name          = strdup("help");
	option->shortcut      = strdup("h");
	option->default_value = strdup("true");
	option->description   = strdup("Display help for the given command.");

	mycommand->addOption(option);
```

## 3 add command to SqConsole

```c
	// C function
	sq_console_add(console, &mycommand);

	// C++ method
	console->add(&mycommand)
```

