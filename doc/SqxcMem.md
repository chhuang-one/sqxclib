[中文](SqxcMem.cn.md)

# SqxcMem

SqxcMem is derived from [Sqxc](Sqxc.md). It is used to output Sqxc converted data to memory.  
Note: SqxcMem is in sqxcsupport library (sqxcsupport.h).

	Sqxc
	│
	└─── SqxcMem

SqxcMem is usually used with SqxcJsoncWriter.

## Create the Sqxc chain

Create the following Sqxc chain to output JSON data to memory.

	Sqxc data arguments ────> SqxcJsoncWriter ────> SqxcMem

use C language

```c
	Sqxc *xcmem;
	Sqxc *xcjson;

	xcmem  = sqxc_new(SQXC_INFO_MEM_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* another way to create Sqxc elements */
//	xcmem  = sqxc_mem_writer_new();
//	xcjson = sqxc_jsonc_writer_new();

	// append JSON writer to Sqxc chain
	sqxc_insert(xcmem, xcjson, -1);
```

use C++ language

```c++
	Sq::XcMem         *xcmem  = new Sq::XcMemWriter();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// append JSON writer to Sqxc chain
	xcmem->insert(xcjson);
```

## Setup SqxcMem

SqxcMem does not require configuration.

## Pass arguments to Sqxc chain

Using sqxc_send() to pass data arguments can forward data between Sqxc elements. The following is just part of the code, please refer to the document [Sqxc.md](Sqxc.md) for more details.  
  
use C language

```c
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// 'xcmem' is used as initial arguments source here.
	Sqxc *xc = (Sqxc*)xcmem;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// Pass the data arguments 'xc' to the Sqxc chain,
	// Return the Sqxc element currently processing the data arguments.
	xc = sqxc_send(xc);

	// continue to set data arguments in 'xc'
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// continue passing data arguments to the Sqxc element that is currently processing data arguments.
	xc = sqxc_send(xc);
```

use C++ language

```c++
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// 'xcmem' is used as initial arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcmem;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// Pass the data arguments 'xc' to the Sqxc chain,
	// Return the Sqxc element currently processing the data arguments.
	xc = xc->send();

	// continue to set data arguments in 'xc'
	xc->type = SQXC_TYPE_STR;
	xc->name = "name";
	xc->value.str = "Bob";

	// continue passing data arguments to the Sqxc element that is currently processing data arguments.
	xc = xc->send();
```

You can also use sqxc_send_to() to pass data arguments to SqxcJsoncWriter, which then outputs the JSON data to SqxcMem.  
  
use C language

```c
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// 'xcmem' is used as arguments source here.
	Sqxc *xc = (Sqxc*)xcmem;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);
```

use C++ language

```c++
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// 'xcmem' is used as arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcmem;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);
```

## Output

User can get output data in SqxcMem::buf and length of output in SqxcMem::buf_writed.

```c
	char   *output_data;
	size_t  output_len;

	output_data = xcmem->buf;
	output_len  = xcmem->buf_writed;
```
