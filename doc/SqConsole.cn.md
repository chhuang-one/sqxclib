[English](SqConsole.md)

# SqConsole

SqConsole 提供命令行界面（主要用于 SqAppTool）。 它必须与 SqCommand 和 SqOption 一起使用。  
注意: SqConsole 在 sqxcsupport 库中。  
  
SqConsole、SqCommand 和 SqOption 的关系。

	SqConsole ---┬--- SqCommand 1 ---┬--- SqOption 1
	             │                   │
	             │                   └--- SqOption n
	             │
	             └--- SqCommand n ---  ...

# SqCommand

用户可以使用 SqCommand 静态（或动态）定义命令及其选项。
SqConsole 使用它来解析来自命令行的数据并将解析的数据存储在 SqCommandValue 中。

	SqType
	│
	└--- SqCommand

# SqOption

它在命令中定义选项。

	SqEntry
	│
	└--- SqOption

# SqCommandValue

它存储来自命令行的选项值。

## 1 静态定义新命令

例如: 定义 'mycommand' 有两个选项 - '--help' 和 '--quiet'

#### 1.1 定义命令选项的值

定义派生自 SqCommandValue 的 MyCommandValue。

	SqCommandValue
	│
	└--- MyCommandValue

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct MyCommandValue    MyCommandValue;

#ifdef __cplusplus
struct MyCommandValue : Sq::CommandValueMethod     // <-- 1. 继承 C++ 成员函数 (方法)
#else
struct MyCommandValue
#endif
{
	SQ_COMMAND_VALUE_MEMBERS;                      // <-- 2. 继承成员变量

	bool    help;                                  // <-- 3. 在派生结构中添加变量和非虚函数。
	bool    quiet;
};
```

#### 1.2 静态定义命令选项

如果定义常量 SqCommand，它必须与 SqOption 的**指针数组**一起使用。

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

#### 1.3 定义命令处理程序的功能

```c++
static void mycommand_handle(MyCommandValue *cmd_value, SqConsole *console, void *data)
{
	// 执行命令时将调用该函数。
}
```

#### 1.4 静态定义命令

```c++
static const SqCommand mycommand = SQ_COMMAND_INITIALIZER(
	MyCommandValue,                                // 结构类型
	0,                                             // bit_field
	"mycommand",                                   // 命令名称
	mycommand_options,                             // SqOption 的指针数组
	mycommand_handle,                              // 处理函数
	"mycommand parameterName",                     // 参数字符串
	"mycommand description"                        // 描述字符串
);

/* 以上 SQ_COMMAND_INITIALIZER() 宏扩展为
static const SqCommand mycommand = {
	// --- SqType 成员 ---
	.size  = sizeof(MyCommandValue),
	.parse = sq_command_parse_option,
	.name  = "mycommand",
	.entry   = (SqEntry**) mycommand_options,
	.n_entry = sizeof(mycommand_options) / sizeof(SqOption*),
	.bit_field = 0,

	// --- SqCommand 成员 ---
	.handle      = (SqCommandFunc) mycommand_handle,
	.parameter   = "mycommand parameterName",
	.description = "mycommand description",
};
 */
```

## 2 动态定义新命令

例如: 通过函数创建 "mycommand"。  
  
使用 C 语言

```c
	SqCommand *mycommand;

	mycommand = sq_command_new("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	mycommand->parameter   = strdup("mycommand parameterName");
	mycommand->description = strdup("mycommand description");
```

使用 C++ 语言

```c++
	Sq::Command *mycommand;

	mycommand = new Sq::Command("mycommand");
	mycommand->size   = sizeof(MyCommandValue);
	mycommand->handle = mycommand_handle;
	mycommand->parameter   = strdup("mycommand parameterName");
	mycommand->description = strdup("mycommand description");
```

#### 2.1 动态 SqCommand 使用 SqOption 的常量数组

```c++
	// C 函数
	sq_command_add_option(mycommand, mycommand_option_array, 2);

	// C++ 方法
	mycommand->addOption(mycommand_option_array, 2);
```

#### 2.2 动态 SqCommand 使用动态 SqOption

使用 C 语言

```c
	SqOption  *option;

	option = sq_option_new(SQ_TYPE_BOOL);
	option->offset        = offsetof(MyCommandValue, help);
	option->name          = strdup("help");
	option->shortcut      = strdup("h");
	option->default_value = strdup("true");
	option->description   = strdup("Display help for the given command.");

	sq_command_add_option(mycommand, option, 1);
```

使用 C++ 语言

```c++
	Sq::Option  *option;

	option = new Sq::Option(SQ_TYPE_BOOL);
	option->offset        = offsetof(MyCommandValue, help);
	option->name          = strdup("help");
	option->shortcut      = strdup("h");
	option->default_value = strdup("true");
	option->description   = strdup("Display help for the given command.");

	mycommand->addOption(option);
```

## 3 将命令添加到 SqConsole

```c
	// C 函数
	sq_console_add(console, &mycommand);

	// C++ 方法
	console->add(&mycommand)
```
