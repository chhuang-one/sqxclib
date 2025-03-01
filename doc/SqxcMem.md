[中文](SqxcMem.cn.md)

# SqxcMem

SqxcMem is derived from [Sqxc](Sqxc.md). It is used to output Sqxc converted data to memory.  
Note: SqxcMem is in sqxcsupport library (sqxcsupport.h).

	Sqxc
	│
	└─── SqxcMem

## Output JSON data to memory

Create the following Sqxc chain to output JSON data to memory.

	Sqxc data arguments ────> SqxcJsoncWriter ────> SqxcMem

#### create the Sqxc chain

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

#### Setup SqxcMem

SqxcMem does not require configuration.

#### pass arguments to SqxcJsoncWriter

Use sqxc_send_to() to pass data arguments to specified Sqxc elements - SqxcJsoncWriter.  
  
use C language

```c
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// I use 'xcmem' as arguments source here.
	Sqxc *xc = (Sqxc*)xcmem;

	// notify Sqxc elements to get ready
	sqxc_ready(xc, NULL);

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);

	// notify Sqxc elements to finish
	sqxc_finish(xc, NULL);
```

use C++ language

```c++
	// Because arguments in 'xcmem' will not be used in Sqxc chain,
	// I use 'xcmem' as arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcmem;

	// notify Sqxc elements to get ready
	xc->ready();

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);

	// notify Sqxc elements to finish
	xc->finish();
```

#### Output

User can get output data in SqxcMem::buf and length of output in SqxcMem::buf_writed.

```c
	char   *output_data;
	size_t  output_len;

	output_data = xcmem->buf;
	output_len  = xcmem->buf_writed;
```
