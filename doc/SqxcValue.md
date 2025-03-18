[中文](SqxcValue.cn.md)

# SqxcValue

SqxcValue is derived from [Sqxc](Sqxc.md). It converts Sqxc data to C data types.

	Sqxc
	│
	└─── SqxcValue

## Create the Sqxc chain

Create the following Sqxc chain to convert data to C data types.

	Sqxc data arguments ────> SqxcValue ────> SqType::parse()

use C language

```c
	Sqxc *xcvalue;

	xcvalue = sqxc_new(SQXC_INFO_VALUE);
	/* another way to create Sqxc elements */
//	xcvalue = sqxc_value_new();
```

use C++ language

```c++
	Sq::XcValue *xcvalue;

	xcvalue = new Sq::XcValue();
```

If some of C string is JSON data, modify the Sqxc chain above.

	                      ┌─> SqxcJsoncParser ─┐
	Sqxc data arguments ──┴────────────────────┴──> SqxcValue   ───> SqType::parse()

use C language

```c
	Sqxc *xcjson;

	xcjson = sqxc_new(SQXC_INFO_JSONC_PARSER);
	/* another way to create Sqxc elements */
//	xcjson = sqxc_jsonc_parser_new();

	// append JSON parser to Sqxc chain
	sqxc_insert(xcvalue, xcjson, -1);
```

use C++ language

```c++
	Sq::XcJsoncParser *xcjson = new Sq::XcJsoncParser();

	// append JSON parser to Sqxc chain
	xcvalue->insert(xcjson);
```

## Setup SqxcValue

SqxcValue can set 3 fields, at least 2 of which must be set.

#### SqType of instance (must be set)

If type of C value is not container (array, list, etc), then set SqType of instance in SqxcValue::element and set NULL in SqxcValue::container.

```c++
	xcvalue->element   = SQ_TYPE_DOUBLE;
	xcvalue->container = NULL;
```

If type of C value is container (array, list, etc), then set SqType of element in SqxcValue::element and set SqType of container in SqxcValue::container.

```c++
	// SqIntArray  or  Sq::Array<int>
	xcvalue->element   = SQ_TYPE_INT;
	xcvalue->container = SQ_TYPE_ARRAY;
```

#### Instance (can be set to NULL)

Allocate memory by SqxcValue:  
If SqxcValue::instance is set to NULL, it will allocate memory using SqType::size before converting.

```c++
	xcvalue->instance = NULL;
```

Allocate memory yourself before conversion:  
If no container, allocate memory of instance of SqType.

```c++
	xcvalue->instance = malloc(sizeof(double));
```

Allocate memory yourself before conversion:  
If it is container, allocate memory of container of SqType.

```c++
	xcvalue->instance = new Sq::Array<int>;
```

## Pass arguments to Sqxc chain

Using sqxc_send() to pass data arguments can forward data between Sqxc elements. The following is just part of the code, please refer to the document [Sqxc.md](Sqxc.md) for more details.  
  
use C language

```c
	// Because arguments in 'xcvalue' will not be used in Sqxc chain,
	// 'xcvalue' is used as initial arguments source here.
	Sqxc *xc = (Sqxc*)xcvalue;

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
	// Because arguments in 'xcvalue' will not be used in Sqxc chain,
	// 'xcvalue' is used as initial arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcvalue;

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

If you are just converting JSON data types to C data types, you can use sqxc_send_to() to pass JSON data to SqxcJsoncParser, which then outputs data arguments to SqxcValue. The data flow looks like:

	Sqxc data arguments ───── SqxcJsoncParser ────> SqxcValue   ───> SqType::parse()

use C language

```c
	// Because arguments in 'xcvalue' will not be used in Sqxc chain,
	// 'xcvalue' is used as arguments source here.
	Sqxc *xc = (Sqxc*)xcvalue;

	// set data arguments in 'xc'.
	// Below is a C string containing a JSON array of integers.
	xc->type = SQXC_TYPE_STR;
	xc->name = "intArray";
	xc->value.str = "[3, 5, 7]";

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);
```

use C++ language

```c++
	// Because arguments in 'xcvalue' will not be used in Sqxc chain,
	// 'xcvalue' is used as arguments source here.
	Sq::Xc *xc = (Sq::Xc*)xcvalue;

	// set data arguments in 'xc'
	// Below is a C string containing a JSON array of integers.
	xc->type = SQXC_TYPE_STR;
	xc->name = "intArray";
	xc->value.str = "[3, 5, 7]";

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);
```

## Output

SqxcValue::instance is the conversion result and must be set to NULL before next conversion.

```c
	// get conversion result in SqxcValue::instance
	instance = xcvalue->instance;

	// reset SqxcValue::instance before next converting
	xcvalue->instance = NULL;
```
