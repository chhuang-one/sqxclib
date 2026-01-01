[中文](SqxcFile.cn.md)

# SqxcFile

SqxcFile is derived from [Sqxc](Sqxc.md). It is used to output Sqxc converted data to a file.  
Note: SqxcFile is in sqxcsupport library.

	Sqxc
	│
	└─── SqxcFile

SqxcFile is usually used with SqxcJsonWriter.

## Create the Sqxc chain

Create the following Sqxc chain to output JSON data to a file.

	Sqxc data arguments ────> SqxcJsonWriter ────> SqxcFile ────> fwrite()

use C language

```c
	Sqxc *xcfile;
	Sqxc *xcjson;

	xcfile = sqxc_new(SQXC_INFO_FILE_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSON_WRITER);
	/* another way to create Sqxc elements */
//	xcfile = sqxc_file_writer_new();
//	xcjson = sqxc_json_writer_new();

	// append JSON writer to Sqxc chain
	sqxc_insert(xcfile, xcjson, -1);
```

use C++ language

```c++
	Sq::XcFile        *xcfile = new Sq::XcFileWriter();
	Sq::XcJsonWriter  *xcjson = new Sq::XcJsonWriter();

	// append JSON writer to Sqxc chain
	xcfile->insert(xcjson);
```

## Setup SqxcFile

Set the output file name in SqxcFile::filename.

```c
	// set filename
	xcfile->filename = "filename.json";
```

## Pass arguments to Sqxc chain

Using sqxc_send() to pass data arguments can forward data between Sqxc elements. The following is just part of the code, please refer to the document [Sqxc.md](Sqxc.md) for more details.  
  
use C language

```c
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// 'xcfile' is used as initial arguments source here.
	Sqxc *xc = (Sqxc*)xcfile;

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
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// 'xcfile' is used as initial arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcfile;

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

You can also use sqxc_send_to() to pass data arguments to SqxcJsonWriter, which then outputs the JSON data to SqxcFile.  
  
use C language

```c
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// 'xcfile' is used as arguments source here.
	Sqxc *xc = (Sqxc*)xcfile;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);
```

use C++ language

```c++
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// 'xcfile' is used as arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcfile;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);
```

## Output

User can get output data in a file (filename is SqxcFile::filename).
