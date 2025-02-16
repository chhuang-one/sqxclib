[中文](SqxcFile.cn.md)

# SqxcFile

SqxcFile is derived from [Sqxc](Sqxc.md). It is used to output Sqxc converted data to a file.  
Note: SqxcFile is in sqxcsupport library (sqxcsupport.h).

	Sqxc
	│
	└─── SqxcFile

## Output JSON data to file

Create the following Sqxc chain to output JSON data to a file.

	Sqxc data arguments ────> SqxcJsoncWriter ────> SqxcFile ────> fwrite()

#### create the Sqxc chain

use C language

```c
	Sqxc *xcfile;
	Sqxc *xcjson;

	xcfile = sqxc_new(SQXC_INFO_FILE_WRITER);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* another way to create Sqxc elements */
//	xcfile = sqxc_file_writer_new();
//	xcjson = sqxc_jsonc_writer_new();

	// append JSON writer to Sqxc chain
	sqxc_insert(xcfile, xcjson, -1);
```

use C++ language

```c++
	Sq::XcFile        *xcfile = new Sq::XcFileWriter();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// append JSON writer to Sqxc chain
	xcfile->insert(xcjson);
```

#### Setup SqxcFile

Set the output file name in SqxcFile::filename.

```c
	// set filename
	xcfile->filename = "filename.json";
```

#### pass arguments to SqxcJsoncWriter

Use sqxc_send_to() to pass data arguments to specified Sqxc elements - SqxcJsoncWriter.  
  
use C language

```c
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// I use 'xcfile' as arguments source here.
	Sqxc *xc = (Sqxc*)xcfile;

	// notify Sqxc elements to get ready
	// 'xcfile' will open the specified file for writing.
	sqxc_ready(xc, NULL);

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);

	// notify Sqxc elements to finish
	// 'xcfile' will close the file being written to.
	sqxc_finish(xc, NULL);
```

use C++ language

```c++
	// Because arguments in 'xcfile' will not be used in Sqxc chain,
	// I use 'xcfile' as arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcfile;

	// notify Sqxc elements to get ready
	// 'xcfile' will open the specified file for writing.
	xc->ready();

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);

	// notify Sqxc elements to finish
	// 'xcfile' will close the file being written to.
	xc->finish();
```

#### Output

User can get output data in a file (filename is SqxcFile::filename).
