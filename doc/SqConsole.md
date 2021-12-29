# SqConsole

SqConsole provide command-line interface. It must use with SqCommand and SqOption.

	Relationship of SqConsole, SqCommand, and SqOption.

	SqConsole ---┐
	             |
	             ├--- SqCommand 1 ---  ...
	             |
	             └--- SqCommand n ---┐
	                                 |
	                                 ├--- SqOption 1
	                                 |
	                                 └--- SqOption n

# SqCommand

  User can use SqCommand to define command and it's options statically (or dynamically).
SqConsole use this to parse data from command-line and store parsed data in SqCommandValue.

	SqType
	|
	`--- SqCommand

# SqOption
  define an option for command

	SqEntry
	|
	`--- SqOption

## Define a new command

define MyCommandValue that has two options - '--help' and '--quiet'

#### 1. define value of command

	SqCommandValue
	|
	`--- MyCommandValue

```c++
#ifdef __cplusplus
typedef struct MyCommandValue : Sq::CommandValueMethod     // <-- 1. inherit C++ member function(method)
#else
typedef struct MyCommandValue
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                              // <-- 2. inherit member variable

	bool    help;                                          // <-- 3. Add variable and non-virtual function in derived struct.
	bool    quiet;
};
```

#### 2. define options of command

```c
static const SqOption *mycommand_options[] = {
	&(SqOption) {SQ_TYPE_BOOL,  "help",      offsetof(MyCommandValue, help),
	             .shortcut = "h",  .default_value = "true",
	             .description = "Display help for the given command."},

	&(SqOption) {SQ_TYPE_BOOL,  "quiet",     offsetof(MyCommandValue, quiet),
	             .shortcut = "q",  .default_value = "true",
	             .description = "Do not output any message."},
};
```

#### 3. define command handler function

```c++
static void mycommand_handle(MyCommandValue *cmd_value, SqConsole *console, void *data)
{
	// The function will be called when your command is executed.
}
```

#### 4. define command

```c++
static const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandValue, 0,                             // StructureType, bit_field
	"mycommand",                                   // command string
	mycommand_options,                             // SqOption pointer array
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

#### 5. add command to SqConsole

```c
	// C function
	sq_console_add(console, &mycommand);

	// C++ method
	console->add(&mycommand)
```
