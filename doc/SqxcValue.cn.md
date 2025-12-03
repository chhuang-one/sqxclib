[English](SqxcValue.md)

# SqxcValue

SqxcValue 派生自 [Sqxc](Sqxc.cn.md)。它将 Sqxc 数据转换为 C 数据类型。

	Sqxc
	│
	└─── SqxcValue

## 创建 Sqxc 链

创建以下 Sqxc 链，将数据转换为 C 数据类型。

	Sqxc 数据参数 ────> SqxcValue ────> SqType::parse()

使用 C 语言

```c
	Sqxc *xcvalue;

	xcvalue = sqxc_new(SQXC_INFO_VALUE);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcvalue = sqxc_value_new();
```

使用 C++ 语言

```c++
	Sq::XcValue *xcvalue;

	xcvalue = new Sq::XcValue();
```

如果某些字段/成员以 JSON 对象或数组形式输入，则修改上面的 Sqxc 链如下:

	                ┌─> SqxcJsonParser ──┐
	Sqxc 数据参数 ──┴────────────────────┴──> SqxcValue   ───> SqType::parse()

使用 C 语言

```c
	Sqxc *xcjson;

	xcjson = sqxc_new(SQXC_INFO_JSON_PARSER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcjson = sqxc_json_parser_new();

	// 将 JSON 解析器附加到 Sqxc 链
	sqxc_insert(xcvalue, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcJsonParser  *xcjson = new Sq::XcJsonParser();

	// 将 JSON 解析器附加到 Sqxc 链
	xcvalue->insert(xcjson);
```

## 设置 SqxcValue

SqxcValue 可以设置 3 个字段，其中至少 2 个必须设置。

#### 实例的 SqType (必须设置)

如果 C 值的类型不是容器（数组、列表...等），则在 SqxcValue::element 中设置实例的 SqType，并在 SqxcValue::container 中设置 NULL。

```c++
	xcvalue->element   = SQ_TYPE_DOUBLE;
	xcvalue->container = NULL;
```

如果 C 值的类型是容器（数组、列表...等），则在 SqxcValue::element 中设置元素的 SqType，并在 SqxcValue::container 中设置容器的 SqType。

```c++
	// SqIntArray  或者  Sq::Array<int>
	xcvalue->element   = SQ_TYPE_INT;
	xcvalue->container = SQ_TYPE_ARRAY;
```

#### 实例 (可以设置为 NULL)

通过 SqxcValue 分配内存:  
如果 SqxcValue::instance 设置为 NULL，它将在转换之前使用 SqType::size 分配内存。

```c++
	xcvalue->instance = NULL;
```

转换前自行分配内存:  
如果没有容器，则分配 SqType 实例的内存。

```c++
	xcvalue->instance = malloc(sizeof(double));
```

转换前自行分配内存:  
如果是容器，则分配 SqType 容器的内存。

```c++
	xcvalue->instance = new Sq::Array<int>;
```

## 将参数传递给 Sqxc 链

使用 sqxc_send() 传递数据参数可以在 Sqxc 元素之间转发数据。以下只是部分代码，更多详细信息请参阅文档 [Sqxc.cn.md](Sqxc.cn.md)。  
  
使用 C 语言

```c
	// 因为 'xcvalue' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcvalue' 作为初始参数源。
	Sqxc *xc = (Sqxc*)xcvalue;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 Sqxc 链，
	// 返回当前处理数据参数的 Sqxc 元素。
	xc = sqxc_send(xc);

	// 继续在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// 继续传递数据参数给当前正在处理数据参数的 Sqxc 元素。
	xc = sqxc_send(xc);
```

使用 C++ 语言

```c++
	// 因为 'xcvalue' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcvalue' 作为初始参数源。
	Sq::Xc *xc = (Sq::Xc*)xcvalue;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 Sqxc 链，
	// 返回当前处理数据参数的 Sqxc 元素。
	xc = xc->send();

	// 继续在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// 继续传递数据参数给当前正在处理数据参数的 Sqxc 元素。
	xc = xc->send();
```

如果您只是将 JSON 数据类型转换为 C 数据类型，则可以使用 sqxc_send_to() 将 JSON 数据传递给 SqxcJsonParser，然后 SqxcJsonParser 再输出数据参数到 SqxcValue。数据流如下所示：

	Sqxc 数据参数 ───── SqxcJsonParser ────> SqxcValue   ───> SqType::parse()

使用 C 语言

```c
	// 因为 'xcvalue' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcvalue' 作为参数源。
	Sqxc *xc = (Sqxc*)xcvalue;

	// 在 'xc' 中设置数据参数
	// 下面是一个包含整数 JSON 数组的 C 字符串。
	xc->type = SQXC_TYPE_STR;
	xc->name = "intArray";
	xc->value.str = "[3, 5, 7]";

	// 将数据参数 'xc' 传递给 'xcjson'
	sqxc_send_to(xcjson, xc);
```

使用 C++ 语言

```c++
	// 因为 'xcvalue' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcvalue' 作为参数源。
	Sq::Xc *xc = (Sq::Xc*)xcvalue;

	// 在 'xc' 中设置数据参数
	// 下面是一个包含整数 JSON 数组的 C 字符串。
	xc->type = SQXC_TYPE_STR;
	xc->name = "intArray";
	xc->value.str = "[3, 5, 7]";

	// 将数据参数 'xc' 传递给 'xcjson'
	xcjson->send(xc);
```

## 输出

SqxcValue::instance 是转换结果，下次转换之前必须设置为 NULL。

```c
	// 在 SqxcValue::instance 中获取转换结果
	instance = xcvalue->instance;

	// 下次转换前重置 SqxcValue::instance
	xcvalue->instance = NULL;
```
