[English](SqxcMem.md)

# SqxcMem

SqxcMem 派生自 [Sqxc](Sqxc.cn.md)。它用于将 Sqxc 转换的数据输出到内存。  
注意: SqxcMem 在 sqxcsupport 库中 (sqxcsupport.h)。

	Sqxc
	│
	└─── SqxcMem

## 输出 JSON 数据到内存

创建以下 Sqxc 链，将 JSON 数据输出到内存。

	Sqxc 数据参数 ────> SqxcJsoncWriter ────> SqxcMem

#### 创建 Sqxc 链

使用 C 语言

```c
	Sqxc *xcmem;
	Sqxc *xcjson;

	xcmem  = sqxc_new(SQXC_INFO_MEM_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcmem  = sqxc_mem_writer_new();
//	xcjson = sqxc_jsonc_writer_new();

	// 将 JSON 写入器附加到 Sqxc 链
	sqxc_insert(xcmem, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcMem         *xcmem  = new Sq::XcMemWriter();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// 将 JSON 写入器附加到 Sqxc 链
	xcmem->insert(xcjson);
```

#### 设置 SqxcMem

SqxcMem 无需设置。

#### 将参数传递给 SqxcJsoncWriter

使用 sqxc_send_to() 将数据参数传递给指定的 Sqxc 元素 - SqxcJsoncWriter。  
  
使用 C 语言

```c
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 我在这里使用 'xcmem' 作为参数源。
	Sqxc *xc = (Sqxc*)xcmem;

	// 通知 Sqxc 元素准备好。
	sqxc_ready(xc, NULL);

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	sqxc_send_to(xcjson, xc);

	// 通知 Sqxc 元素完成
	sqxc_finish(xc, NULL);
```

使用 C++ 语言

```c++
	// 因为 'xcmem' 中的参数不会在 Sqxc 链中使用，
	// 我在这里使用 'xcmem' 作为参数源。
	Sq::Xc *xc = (Sq::Xc*)xcmem;

	// 通知 Sqxc 元素准备好
	xc->ready();

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	xcjson->send(xc);

	// 通知 Sqxc 元素完成
	xc->finish();
```

#### 输出

用户可以在 SqxcMem::buf 中获取输出数据，并在 SqxcMem::buf_writed 中获取输出长度。

```c
	char   *output_data;
	size_t  output_len;

	output_data = xcmem->buf;
	output_len  = xcmem->buf_writed;
```
