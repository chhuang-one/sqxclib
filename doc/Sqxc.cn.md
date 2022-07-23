[English](Sqxc.md)

# Sqxc

Sqxc 在 X 和 C 语言之间转换数据 （X = SQL, JSON...等）  
它在一个 C 结构中包含状态、缓冲区和输入/输出参数。  
用户可以链接多个 Sqxc 元素来转换不同类型的数据。  

| 元素名称     | 描述              | 源代码文件  |
| ------------ | ----------------- | ----------- |
| SqxcSql      | 转换为 SQL (Sqdb) | SqxcSql.c   |
| SqxcJsonc    | 转换   JSON       | SqxcJsonc.c |
| SqxcValue    | 转换为 C 结构     | SqxcValue.c |

Sqxc 转换器的数据类型

| Sqxc 类型名称        | 描述                           |
| -------------------- | ------------------------------ |
| SQXC_TYPE_UNKNOWN    | 未指定                         |
| SQXC_TYPE_NULL       | 对应C值：        NULL          |
| SQXC_TYPE_BOOL       | 对应C数据类型：  bool          |
| SQXC_TYPE_INT        | 对应C数据类型：  int           |
| SQXC_TYPE_UINT       | 对应C数据类型：  unsigned int  |
| SQXC_TYPE_INT64      | 对应C数据类型：  int64_t       |
| SQXC_TYPE_UINT64     | 对应C数据类型：  uint64_t      |
| SQXC_TYPE_TIME       | 对应C数据类型：  time_t        |
| SQXC_TYPE_DOUBLE     | 对应C数据类型：  double        |
| SQXC_TYPE_STRING     | 对应C数据类型：  char*         |
| SQXC_TYPE_OBJECT     | 对象的开头                     |
| SQXC_TYPE_ARRAY      | 数组的开头 (或其他容器)        |
| SQXC_TYPE_OBJECT_END | 对象结束                       |
| SQXC_TYPE_ARRAY_END  | 数组结束 (或其他容器)          |

注意：SQXC_TYPE_OBJECT 对应 SQL 行。  
注意：SQXC_TYPE_ARRAY  对应 SQL 多行。  

## 创建 Sqxc 元素
创建元素以将数据转换为 SQL INSERT/UPDATE 语句，并添加元素以将数据转换为 SQL 列中的 JSON 数组/对象。  
  
使用 C 语言

```c
	Sqxc *xcsql;
	Sqxc *xcjson;

	xcsql  = sqxc_new(SQXC_INFO_SQL);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcsql  = sqxc_sql_new();
//	xcjson = sqxc_jsonc_writer_new();

	// 将 JSON 写入器附加到 Sqxc 链
	sqxc_insert(xcsql, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcSql         *xcsql  = new Sq::XcSql();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// 将 JSON 写入器附加到 Sqxc 链
	xcsql->insert(xcjson);
```

**Sqxc 链的默认链接方向：**  
[Sqxc element 1] 是 Sqxc 链的头，'peer' 是单链表，'dest' 是数据流。

	                    peer                      peer
	┌----------------┐  <---  ┌----------------┐  <---  ┌----------------┐
	| Sqxc element 3 |        | Sqxc element 2 |        | Sqxc element 1 |
	└----------------┘        └----------------┘  --->  └----------------┘
	        |                                     dest          ^
	        |           dest                                    |
	        └---------------------------------------------------┘

函数 insert() 和steal() 仅链接/取消链接 'peer' （'peer' 是单链表），
用户可能需要自己在 Sqxc 链中链接 'dest'（'dest' 是数据流），尤其是自定义数据流。

## 根据数据类型发送数据
sqxc_send() 可以在 Sqxc 元素之间发送数据（参数）并在运行时更改数据流（Sqxc.dest）。  
  
**数据流 1：** sqxc_send() 从 SQL 结果（列有 JSON 数据）发送到 C 值  
如果 SqxcValue 不能匹配当前数据类型，它会将数据转发给 SqxcJsonParser。

	input ─>         ┌─> SqxcJsonParser ──┐
	Sqdb.exec()    ──┴────────────────────┴──> SqxcValue ───> SqType.parse()


**数据流 2:** sqxc_send() 从 C 值发送到 SQL（列有 JSON 数据）  
如果 SqxcSql 不支持当前数据类型，它会将数据转发给 SqxcJsonWriter。

	output ─>        ┌─> SqxcJsonWriter ──┐
	SqType.write() ──┴────────────────────┴──> SqxcSql   ───> Sqdb.exec()

sqxc_send() 由数据源端调用。 它将数据（参数）发送到 Sqxc 元素并尝试匹配 Sqxc 链中的类型。  
因为差异数据类型是由差异 Sqxc 元素处理的，所以它返回当前的 Sqxc 元素。  
  
在调用 sqxc_send() 之前，在 Sqxc 结构中设置数据类型、数据名称和数据值。
这些数据（参数）将在 Sqxc 元素之间进行处理。  
  
使用 C 语言

```c
	Sqxc *xc = xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// sqxc_send() 在 Sqxc 元素之间传递数据（参数）。
	//             它返回当前处理数据的 Sqxc 元素（参数）
	xc = sqxc_send(xc);

	// 从当前 Sqxc 元素获取错误代码
	if (xc->code != SQCODE_OK)
		return;    // error
```

use C++ language

```c++
	Sq::Xc *xc = (Sq::Xc*)xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// send() 在 Sqxc 元素之间传递数据（参数）。
	//        它返回处理数据的当前 Sqxc 元素（参数）
	xc = xc->send();

	// 从当前 Sqxc 元素获取错误代码
	if (xc->code != SQCODE_OK)
		return;    // error
```

#### 使用C语言向 Sqxc 元素发送对象数据
如果要解析/写入对象或数组并重用 Sqxc 元素：
1. 在发送数据之前调用 sqxc_ready()。
2. 发送数据后调用 sqxc_finish()。

```c
	Sqxc *xcur = xc;   // 当前 Sqxc 元素

	sqxc_ready(xc);    // 通知 Sqxc 元素准备好

	xcur->type = SQXC_TYPE_OBJECT;      // {
	xcur->name = NULL;
	xcur->value.pointer = NULL;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_INT;         // "id": 1
	xcur->name = "id";
	xcur->value.integer = 1;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_ARRAY;       // "int_array": [
	xcur->name = "int_array";
	xcur->value.pointer = NULL;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_INT;         // 2,
	xcur->name = NULL;
	xcur->value.integer = 2;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_INT;         // 4
	xcur->name = NULL;
	xcur->value.integer = 4;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_ARRAY_END;   // ]
	xcur->name = NULL;
	xcur->value.pointer = NULL;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_OBJECT_END;  // }
	xcur->name = NULL;
	xcur->value.pointer = NULL;
	xcur = sqxc_send(xcur);

	sqxc_finish(xc);    // 通知 Sqxc 元素完成
```

SqxcSql 会在上面的例子中输出 SQL 语句：

```sql
INSERT INTO table_name (id, int_array) VALUES (1, '[ 2, 4 ]');
```

SQL 表如下所示：

| id | int_array |
| -- | --------- |
| 1  | [ 2, 4 ]  |

JSON 看起来像这样：

```json
{ "id": 1, "int_array": [ 2, 4] }
```

#### 使用 C++ 语言将数组发送到 Sqxc 元素
如果要解析/写入对象或数组并重用 Sqxc 元素：
1. 在发送数据之前调用 ready()。
2. 发送数据后调用 finish()。

```c++
	Sq::Xc *xcur = xc; // 当前 Sqxc 元素

	xc->ready();       // 通知 Sqxc 元素准备好

	xcur->type = SQXC_TYPE_ARRAY;       // [
	xcur->name = NULL;
	xcur = xcur->send();

	xcur->type = SQXC_TYPE_INT;         // 1,
	xcur->name = NULL;
	xcur->value.integer = 1;
	xcur = xcur->send();

	xcur->type = SQXC_TYPE_INT;         // 3
	xcur->name = NULL;
	xcur->value.integer = 3;
	xcur = xcur->send();

	xcur->type = SQXC_TYPE_ARRAY_END;   // ]
	xcur->name = NULL;
	xcur = xcur->send();

	xc->finish();      // 通知 Sqxc 元素完成
```

JSON 看起来像这样：

```json
[ 1, 3]
```

## 将数据发送到用户指定的 Sqxc 元素

使用 sqxc_send_to() 将数据（参数）传递给指定的 Sqxc 元素。

	user output ────> SqxcJsonWriter ────> SqxcFile ────> fwrite()

注意：SqxcFile 在 sqxcsupport 库中。示例代码在 [xc_json_file.cpp](../examples/xc_json_file.cpp)  
  
使用 C 语言

```c
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据（参数）'xc' 传递给 'xcjson'（类型为 SqxcJsonWriter）
	sqxc_send_to(xcjson, xc);
```

使用 C++ 语言

```c++
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据（参数）'xc' 传递给 'xcjson'（类型为 SqxcJsonWriter）
	xcjson->send(xc);
```

## 如何支持新格式：
用户可以参考 SqxcJsonc.h 和 SqxcJsonc.c 来支持新的格式。  
SqxcFile.h 和 SqxcFile.c 是最简单的示例代码，它只是将字符串写入文件。  
SqxcEmpty.h 和 SqxcEmpty.c 是一个可行的示例，但它什么也不做。  

#### 1. 定义从 Sqxc 派生的新结构
所有派生结构必须符合 C++11 标准布局

```c++
// 这是头文件 - SqxcText.h
#include <Sqxc.h>

// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct SqxcText   SqxcText;

#ifdef __cplusplus    // 混合 C 和 C++
extern "C" {
#endif

// 在 SqxcText.c 中定义
extern const SqxcInfo               SqxcInfo_TextParser_;
#define SQXC_INFO_TEXT_PARSER     (&SqxcInfo_TextParser_)

#ifdef __cplusplus    // 混合 C 和 C++
}  // extern "C"
#endif

#ifdef __cplusplus
struct SqxcText : Sq::XcMethod           // <-- 1. 继承 C++ 成员函数（方法）
#else
struct SqxcText
#endif
{
	SQXC_MEMBERS;                        // <-- 2. 继承成员变量

	int    other_data;                   // <-- 3. 在派生结构中添加变量和非虚函数。
	int    status;

#ifdef __cplusplus
	SqxcText() {
		sqxc_init((Sqxc*)this, SQXC_INFO_TEXT_PARSER);
	}
	~SqxcText() {
		sqxc_final((Sqxc*)this);
	}
#endif
};

```

#### 2. 实现 SqxcInfo 接口

```c
// 这是源代码文件 - SqxcText.c
#include <SqxcText.h>

// 为 SqxcInfo 声明函数
static void sqxc_text_parser_init(SqxcText *xctext);
static void sqxc_text_parser_final(SqxcText *xctext);
static int  sqxc_text_parser_ctrl(SqxcText *xctext, int id, void *data);
static int  sqxc_text_parser_send(SqxcText *xctext, Sqxc *src);

// 由 SqxcText.h 使用
const SqxcInfo SqxcInfo_TextParser_ =
{
	.size  = sizeof(SqxcText),
	.init  = (SqInitFunc)   sqxc_text_parser_init,
	.final = (SqFinalFunc)  sqxc_text_parser_final,
	.ctrl  = (SqxcCtrlFunc) sqxc_text_parser_ctrl,
	.send  = (SqxcSendFunc) sqxc_text_parser_send,
};

// 在这里实现 sqxc_text_parser_xxxx() 函数

static void sqxc_text_parser_init(SqxcText *xctext)
{
	// 初始化 SqxcText 实例
}

static void sqxc_text_parser_final(SqxcText *xctext)
{
	// 终结 SqxcText 实例
}

static int  sqxc_text_parser_ctrl(SqxcText *xctext, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:     // 通知 'xctext' 做好准备
		break;

	case SQXC_CTRL_FINISH:    // 通知 'xctext' 完成
		break;

	default:
		return SQCODE_NOT_SUPPORT;
	}
	return SQCODE_OK;
}

static int  sqxc_text_parser_send(SqxcText *xctext, Sqxc *src)
{
	Sqxc *xc_dest = xctext->dest;

	// 解析 'src' 中的参数

	// 将解析的数据设置为 'xctext' 的参数
	xctext->type = src->type;
	xctext->name = src->name;
	xctext->value.string = src->value.string;

	// 将 'xctext' 的参数发送到 'xc_dest'
	sqxc_send_to(xc_dest, (Sqxc*)xctext);

	// 在 src->code 中设置返回码
	return (src->code = SQCODE_OK);
}
```

如果新的 Sqxc 元素要解析/写入 SQL 列中的数据，它必须：  
1. 支持 SQXC_TYPE_ARRAY 或 SQXC_TYPE_OBJECT。
2. 将转换后的数据发送到 dest（或下一个）元素。 见下文：

```c++
	Sqxc *xc_dest;

	// 在 'xc_text' 中设置转换后的数据
	xc_text->type = SQXC_TYPE_INT;
	xc_text->name = NULL;
	xc_text->value.integer = 3;

	// 将数据从 'xc_text' 发送到 'xc_dest' 元素
	xc_dest = xc_text->dest;
	/* C 函数 */
	sqxc_send_to(xc_dest, xc_text);

	/* C++ 方法 */
//	xc_dest->send(xc_text);
```

#### 3. 使用新的 Sqxc 元素

创建自定义 Sqxc 元素并将其插入 SqStorage::xc_input 列表。  
  
使用 C 语言

```c++
	Sqxc *xc_text;
	Sqxc *xc_json;

	// 创建自定义 Sqxc 元素
	xc_text = sqxc_new(SQXC_INFO_TEXT_PARSER);

	// 将 'xc_text' 解析器附加到 'xc_input' 列表
//	sqxc_insert(storage->xc_input, xc_text, -1);

	// 将 'xc_text' 解析器插入 'xc_input' 列表的第二个元素
	sqxc_insert(storage->xc_input, xc_text, 1);

	// 从列表中删除 JSON 解析器，因为它已被新解析器替换。
	xc_json = sqxc_find(storage->xc_input, SQXC_INFO_JSONC_PARSER);
	if (xc_json) {
		sqxc_steal(storage->xc_input, xc_json);
		// 如果不再需要，释放 'xc_json'
		sqxc_free(xc_json);
	}
```

使用 C++ 语言

```c++
	SqxcText  *xc_text;
	Sq::Xc    *xc_json;

	// 创建自定义 Sqxc 元素
	xc_text = new SqxcText();

	// 将 'xc_text' 解析器附加到 'xc_input' 列表
//	storage->xc_input->insert(xc_text, -1);

	// 将 'xc_text' 解析器插入 'xc_input' 列表的第二个元素
	storage->xc_input->insert(xc_text, 1);

	// 从列表中删除 JSON 解析器，因为它已被新解析器替换。
	xc_json = storage->xc_input->find(SQXC_INFO_JSONC_PARSER);
	if (xc_json) {
		storage->xc_input->steal(xc_json);
		// 如果不再需要，释放 'xc_json'
		delete xc_json;
	}
```

SqStorage 对象中的 Sqxc 输入数据流如下所示：

	input ->         ┌-> SqxcTextParser --┐
	Sqdb.exec()    --┴--------------------┴-> SqxcValue ---> SqType.parse()

注意：您还需要在 SqStorage::xc_output 中将 SqxcJsoncWriter 替换为 SqxcTextWriter。

## 处理（跳过）未知对象和数组

下面定义了一个 JSON 对象。

```json
{
	"id" : 123,
	"name" : "Joe",
	"undefinedObject" : {
		"id" : 456,
		"name" : "Alex"
	}
}
```

如果 SqEntry/SqColumn 没有定义“undefinedObject”，则 SqxcValue 在解析上述 JSON 对象时将忽略“undefinedObject”。
