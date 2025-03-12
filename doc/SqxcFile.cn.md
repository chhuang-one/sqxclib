[English](SqxcFile.md)

# SqxcFile

SqxcFile 派生自 [Sqxc](Sqxc.cn.md)。它用于将 Sqxc 转换的数据输出到文件。  
注意: SqxcFile 在 sqxcsupport 库中 (sqxcsupport.h)。

	Sqxc
	│
	└─── SqxcFile

SqxcFile 通常与 SqxcJsoncWriter 一起使用。

## 创建 Sqxc 链

创建以下 Sqxc 链，将 JSON 数据输出到文件。

	Sqxc 数据参数 ────> SqxcJsoncWriter ────> SqxcFile ────> fwrite()

使用 C 语言

```c
	Sqxc *xcfile;
	Sqxc *xcjson;

	xcfile = sqxc_new(SQXC_INFO_FILE_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* 另一种创建 Sqxc 元素的方法 */
//	xcfile = sqxc_file_writer_new();
//	xcjson = sqxc_jsonc_writer_new();

	// 将 JSON 写入器附加到 Sqxc 链
	sqxc_insert(xcfile, xcjson, -1);
```

使用 C++ 语言

```c++
	Sq::XcFile        *xcfile = new Sq::XcFileWriter();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// 将 JSON 写入器附加到 Sqxc 链
	xcfile->insert(xcjson);
```

## 设置 SqxcFile

在 SqxcFile::filename 中设置输出文件名。

```c
	// 设置文件名
	xcfile->filename = "filename.json";
```

## 将参数传递给 Sqxc 链

使用 sqxc_send() 传递数据参数可以在 Sqxc 元素之间转发数据。以下只是部分代码，更多详细信息请参阅文档 [Sqxc.cn.md](Sqxc.cn.md)。  
  
使用 C 语言

```c
	// 因为 'xcfile' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcfile' 作为初始参数源。
	Sqxc *xc = (Sqxc*)xcfile;

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
	// 因为 'xcfile' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcfile' 作为初始参数源。
	Sq::Xc *xc = (Sq::Xc*)xcfile;

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

您还可以使用 sqxc_send_to() 将数据参数传递给 SqxcJsoncWriter，然后 SqxcJsoncWriter 再输出 JSON 数据到 SqxcFile。  
  
使用 C 语言

```c
	// 因为 'xcfile' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcfile' 作为参数源。
	Sqxc *xc = (Sqxc*)xcfile;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	sqxc_send_to(xcjson, xc);
```

使用 C++ 语言

```c++
	// 因为 'xcfile' 中的参数不会在 Sqxc 链中使用，
	// 这里使用 'xcfile' 作为参数源。
	Sq::Xc *xc = (Sq::Xc*)xcfile;

	// 在 'xc' 中设置数据参数
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// 将数据参数 'xc' 传递给 'xcjson'
	xcjson->send(xc);
```

## 输出

用户可以在文件中获取输出数据（文件名为 SqxcFile::filename）。
