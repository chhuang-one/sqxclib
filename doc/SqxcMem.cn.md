[English](SqxcMem.md)

# SqxcMem

SqxcMem 派生自 [Sqxc](Sqxc.cn.md)。它用于将 Sqxc 转换的数据输出到内存。  
注意: SqxcMem 在 sqxcsupport 库。

	Sqxc
	│
	└─── SqxcMem

SqxcMem 通常与 SqxcJsonWriter 一起使用。

## 创建 Sqxc 链

创建以下 Sqxc 链，将 JSON 数据输出到内存。

	Sqxc 数据参数 ────> SqxcJsonWriter ────> SqxcMem

使用 C 语言

```c
	Sqxc *xcmem;
	Sqxc *xcjson;

	xcmem  = sqxc_new(SQXC_INFO_MEM_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSON_WRITER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcmem  = sqxc_mem_writer_new();
//	xcjson = sqxc_json_writer_new();

	// 将 JSON 写入器附加到 Sqxc 链
	sqxc_insert(xcmem, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcMem         *xcmem  = new Sq::XcMemWriter();
	Sq::XcJsonWriter  *xcjson = new Sq::XcJsonWriter();

	// 将 JSON 写入器附加到 Sqxc 链
	xcmem->insert(xcjson);
```

## 设置 SqxcMem

SqxcMem 无需设置。

## 将参数传递给 Sqxc 链

使用 sqxc_send() 传递数据参数可以在 Sqxc 元素之间转发数据。以下只是部分代码，更多详细信息请参阅文档 [Sqxc.cn.md](Sqxc.cn.md)。  
  
使用 C 语言

```c
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcmem' 作为初始参数源。
	Sqxc *xc = (Sqxc*)xcmem;

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
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcmem' 作为初始参数源。
	Sq::Xc *xc = (Sq::Xc*)xcmem;

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

您还可以使用 sqxc_send_to() 将数据参数传递给 SqxcJsonWriter，然后 SqxcJsonWriter 再输出 JSON 数据到 SqxcMem。  
  
使用 C 语言

```c
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcmem' 作为参数源。
	Sqxc *xc = (Sqxc*)xcmem;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	sqxc_send_to(xcjson, xc);
```

使用 C++ 语言

```c++
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcmem' 作为参数源。
	Sq::Xc *xc = (Sq::Xc*)xcmem;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	xcjson->send(xc);
```

## 输出

用户可以在 SqxcMem::buf 中获取输出数据，并在 SqxcMem::buf_writed 中获取输出长度。

```c
	char   *output_data;
	size_t  output_len;

	output_data = xcmem->buf;
	output_len  = xcmem->buf_writed;
```
