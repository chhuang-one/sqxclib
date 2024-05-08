[English](SqConsole.md)

# SqConsole

SqConsole 提供命令行界面（主要用于 SqAppTool）。它必须与 SqCommand 和 SqOption 一起使用。  
注意: SqConsole 在 sqxcsupport 库中。  
  
SqConsole、SqCommand 和 SqOption 的关系。

	           1 : N               1 : N
	SqConsole ---┬--- SqCommand 1 ---┬--- SqOption 1-1
	             │                   │
	             │                   └--- SqOption 1-2
	             │
	             └--- SqCommand 2 ---┬--- SqOption 2-1
	                                 │
	                                 └--- SqOption 2-2

# SqCommand

用户可以使用 SqCommand 定义常量（或动态）命令及其选项。
SqConsole 使用它来解析来自命令行的数据并将解析的数据存储在 SqCommandValue 中。

	SqType
	│
	└--- SqCommand

# SqOption

它在命令中定义选项。由于它派生自 [SqEntry](SqEntry.cn.md)，因此如果 C/C++ 语言中的选项值为指针类型，则可以在 SqOption::bit_field 中设置 SQB_POINTER。

	SqEntry
	│
	└--- SqOption

# SqCommandValue

SqCommandValue 存储来自命令行的选项值和参数。

## 定义新命令

例如: 定义具有 3 个选项的 'mycommand' - 'help', 'quiet' 和 'step'。  
  
首先为 'mycommand' 的选项定义结构体 MyCommandOptions。

```c
struct MyCommandOptions
{
	bool    help;
	bool    quiet;
	int     step;
};
```

#### 定义命令处理程序的功能

```c++
// 执行命令时将调用该函数。
void  mycommand_handle(SqCommandValue *commandValue, SqConsole *console, void *data)
{
	MyCommandOptions *options = (MyCommandOptions*)commandValue->options;

	// 在这里做点什么...
}
```

#### 1.1 定义常量命令选项

如果定义常量 SqCommand，它必须与 SqOption 的**指针数组**一起使用。

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

如果 C/C++ 语言中的选项值是指针类型，则在 SqOption::bit_field 中设置 SQB_POINTER。

```c
const SqOption  options[] = {
	{SQ_TYPE_MY_OBJECT,  "obj",  offsetof(MyCommandOptions, obj),  SQB_POINTER},
	//                                                             ^^^^^^^^^^^

	// 省略
}
```

#### 1.2 定义常量命令

```c++
const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandOptions,                              // 结构类型
	0,                                             // bit_field
	"mycommand",                                   // 命令名称
	mycommand_options,                             // SqOption 的指针数组
	mycommand_handle,                              // 处理函数
	"mycommand parameterName",                     // 参数字符串
	"mycommand description"                        // 描述字符串
);

/* 以上 SQ_COMMAND_INITIALIZER() 宏扩展为
const SqCommand mycommand = {
	// --- SqType 成员 ---
	.size  = sizeof(MyCommandOptions),
	.parse = sq_command_parse_option,
	.name  = "mycommand",
	.entry   = (SqEntry**) mycommand_options,
	.n_entry = sizeof(mycommand_options) / sizeof(SqOption*),
	.bit_field = 0,
	.on_destroy = NULL,

	// --- SqCommand 成员 ---
	.handle      = (SqCommandFunc) mycommand_handle,
	.parameter   = "mycommand parameterName",
	.description = "mycommand description",
};
 */
```

#### 2.1 动态定义新命令

例如: 通过函数创建 "mycommand"。  
  
使用 C 语言

```c
	SqCommand *mycommand;

	mycommand = sq_command_new("mycommand");
	mycommand->size   = sizeof(MyCommandOptions);
	mycommand->handle = mycommand_handle;
	sq_command_set_parameter(mycommand, "mycommand parameterName");
	sq_command_set_description(mycommand, "mycommand description");
```

使用 C++ 语言

```c++
	Sq::Command *mycommand;

	mycommand = new Sq::Command("mycommand");
	mycommand->size   = sizeof(MyCommandOptions);
	mycommand->handle = mycommand_handle;
	mycommand->setParameter("mycommand parameterName");
	mycommand->setDescription("mycommand description");
```

#### 2.2 动态 SqCommand 使用 SqOption 的常量数组

例如: 添加具有 2 个选项的数组。

```c++
	// C 函数
	sq_command_add_option(mycommand, mycommand_option_array, 2);

	// C++ 方法
	mycommand->addOption(mycommand_option_array, 2);
```

#### 2.3 动态 SqCommand 使用动态 SqOption

使用 C 语言

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

使用 C++ 语言

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

如果 C/C++ 语言中的选项值是指针类型，则调用 pointer()。

```c++
	// C 函数
	sq_option_pointer(option);

	// C++ 方法
	option->pointer();
```

## 将命令添加到 SqConsole

```c
	// C 函数
	sq_console_add(console, &mycommand);

	// C++ 方法
	console->add(&mycommand)
```

## 解析命令行

例如: 使用指定的命令、选项和参数执行程序。

```console
program  mycommand  --step=5  argument1  argument2
```

调用 parse() 来解析命令行参数。  
  
使用 C 语言

```c
int  main(int argc, char **argv)
{
	SqCommandValue *commandValue;
	bool            command_in_argv = true;

	commandValue = sq_console_parse(console, argc, argv, command_in_argv);
}
```

使用 C++ 语言

```c++
int  main(int argc, char **argv)
{
	Sq::CommandValue *commandValue;
	bool              command_in_argv = true;

	commandValue = console->parse(argc, argv, command_in_argv);
}
```

sq_console_parse() 的返回值是 'commandValue'，它应该如下所示：

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

如果您只需要选项和参数而不需要命令，请调用 parse() 并将最后一个参数指定为 false。
在这种情况下，SqConsole 默认使用第一个添加的命令。  
  
例如: 仅使用选项和参数执行程序，不指定命令。

```console
program  --step=5  argument1  argument2
```

调用 parse() 并将最后一个参数指定为 false 以解析没有命令的命令行参数。

```c++
	// C 函数
	commandValue = sq_console_parse(console, argc, argv, false);

	// C++ 方法
	commandValue = console->parse(argc, argv, false);
```

**释放 'commandValue' 的内存**  
  
使用 C 语言

```c
	SqCommandValue *commandValue;

	sq_command_value_free(commandValue);
```

使用 C++ 语言  
  
C++ Sq::CommandValue 已定义析构函数，因此用户可以使用 'delete' 关键字来释放内存。

```c++
	Sq::CommandValue *commandValue;

	delete commandValue;
	// 或者
	// commandValue->free();
```

## 打印帮助信息

要打印帮助信息，可以使用 C 函数 sq_console_print_help()、C++ 方法 printHelp()。  
如果 'command' 为 NULL，则它们使用第一个添加的 SqCommand 来打印不带命令名称的帮助信息。

```c++
	SqCommand*  command = mycommand;

	// C 函数
	sq_console_print_help(console, command);

	// C++ 方法
	console->printHelp(command);
```

当您的程序有多个命令时，您可以使用 printList() 列出 SqConsole 中添加的所有命令。

```c++
	const char *program_description = "程序描述，在其他帮助信息之前打印。";

	// C 函数
	q_console_print_list(console, program_description);

	// C++ 方法
	console->printList(program_description);
```
