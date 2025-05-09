[中文](Sqxc.cn.md)

# Sqxc

Sqxc convert data between X and C language (X = SQL, JSON, etc.). It contain status, buffer, and input/output arguments in one C structure.  
User can link multiple Sqxc elements to convert different types of data.  

| element name | description           | source file |
| ------------ | --------------------- | ----------- |
| SqxcSql      | convert to SQL (Sqdb) | SqxcSql.c   |
| SqxcJsonc    | convert to/from JSON  | SqxcJsonc.c |
| SqxcValue    | convert to C struct   | SqxcValue.c |
| SqxcFile     | output to file        | SqxcFile.c  |
| SqxcMem      | output to memory      | SqxcMem.c   |

Note: [SqxcFile](SqxcFile.md) and [SqxcMem](SqxcMem.md) are in sqxcsupport library. Sample code is in [xc_json_file.cpp](../examples/xc_json_file.cpp)

**Data type for Sqxc converter**

| Sqxc type name       | description                                |
| -------------------- | ------------------------------------------ |
| SQXC_TYPE_NULL       | corresponds to C value:      NULL          |
| SQXC_TYPE_BOOL       | corresponds to C data type:  bool          |
| SQXC_TYPE_INT        | corresponds to C data type:  int           |
| SQXC_TYPE_UINT       | corresponds to C data type:  unsigned int  |
| SQXC_TYPE_INT64      | corresponds to C data type:  int64_t       |
| SQXC_TYPE_UINT64     | corresponds to C data type:  uint64_t      |
| SQXC_TYPE_TIME       | corresponds to C data type:  time_t        |
| SQXC_TYPE_DOUBLE     | corresponds to C data type:  double        |
| SQXC_TYPE_STR        | corresponds to C data type:  char*         |
| SQXC_TYPE_STRING     | corresponds to C data type:  char*         |
| SQXC_TYPE_RAW        | corresponds to C data type:  char*         |
| SQXC_TYPE_OBJECT     | The beginning of the object                |
| SQXC_TYPE_ARRAY      | The beginning of the array (or container)  |
| SQXC_TYPE_OBJECT_END | The end of object                          |
| SQXC_TYPE_ARRAY_END  | The end of array (or container)            |

Note: SQXC_TYPE_RAW    is raw string, mainly used for SQL data types.  
Note: SQXC_TYPE_STRING is alias of SQXC_TYPE_STR.  
Note: SQXC_TYPE_OBJECT corresponds to SQL row.  
Note: SQXC_TYPE_ARRAY  corresponds to SQL multiple row.  

## Create Sqxc elements
create element to convert data to SQL INSERT/UPDATE statement and add element to convert data to JSON array/object in database column.  
  
use C language

```c
	Sqxc *xcsql;
	Sqxc *xcjson;

	xcsql  = sqxc_new(SQXC_INFO_SQL);
	xcjson = sqxc_new(SQXC_INFO_JSONC_WRITER);
	/* another way to create Sqxc elements */
//	xcsql  = sqxc_sql_new();
//	xcjson = sqxc_jsonc_writer_new();

	// append JSON writer to Sqxc chain
	sqxc_insert(xcsql, xcjson, -1);
```

use C++ language

```c++
	Sq::XcSql         *xcsql  = new Sq::XcSql();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// append JSON writer to Sqxc chain
	xcsql->insert(xcjson);
```

**Default link direction of Sqxc chain:**  
[Sqxc element 1] is head of Sqxc chain, 'peer' is single linked list, 'dest' is data flow.

	                    peer                      peer
	┌────────────────┐  <───  ┌────────────────┐  <───  ┌────────────────┐
	│ Sqxc element 3 │        │ Sqxc element 2 │        │ Sqxc element 1 │
	└────────────────┘        └────────────────┘  ───>  └────────────────┘
	        │                                     dest          ^
	        │           dest                                    │
	        └───────────────────────────────────────────────────┘

Function insert() and steal() only link or unlink 'peer' ('peer' is single linked list),
user may need link 'dest' ('dest' is data flow) by himself in Sqxc chain, especially custom data flow.

## Send data according to data type
sqxc_send() can send data(arguments) between Sqxc elements and change data flow (Sqxc::dest) at runtime.  
  
**Data flow 1:** sqxc_send() send from SQL result (column has JSON data) to C value  
If SqxcValue can't match current data type, it will forward data to SqxcJsoncParser.

	input ─>          ┌─> SqxcJsoncParser ─┐
	sqdb_exec()     ──┴────────────────────┴──> SqxcValue ───> SqType::parse()


**Data flow 2:** sqxc_send() send from C value to SQL (column has JSON data)  
If [SqxcSql](SqxcSql.md) doesn't support current data type, it will forward data to SqxcJsoncWriter.

	output ─>         ┌─> SqxcJsoncWriter ─┐
	SqType::write() ──┴────────────────────┴──> SqxcSql   ───> sqdb_exec()

sqxc_send() is called by data source side. It send data(arguments) to Sqxc element and try to match type in Sqxc chain.  
Because different data type is processed by different Sqxc element, It returns current Sqxc elements.  
  
Before you call sqxc_send(), set data type, data name, and data value in Sqxc structure.
These data(arguments) will be processed between Sqxc elements.  
  
use C language

```c
	// pointer to Sqxc instance
	Sqxc *xc = xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// sqxc_send() pass data(arguments) between Sqxc elements.
	//             It return current Sqxc element that processing data(arguments)
	xc = sqxc_send(xc);

	// get error code from current Sqxc element
	if (xc->code != SQCODE_OK)
		return;    // error
```

use C++ language

```c++
	// pointer to Sqxc instance
	Sq::XcMethod *xc = xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// send() pass data(arguments) between Sqxc elements.
	//        It return current Sqxc element that processing data(arguments)
	xc = xc->send();

	// get error code from current Sqxc element
	if (xc->code != SQCODE_OK)
		return;    // error
```

#### use C language to send object data to Sqxc elements
If you want to parse/write object or array and reuse Sqxc elements:
1. call sqxc_ready() before sending data.
2. call sqxc_finish() after sending data.

```c
	// pointer to current Sqxc instance
	Sqxc *xcur;

	sqxc_ready(xc, NULL);    // notify Sqxc elements to get ready
	xcur = xc;               // current Sqxc element

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

	sqxc_finish(xc, NULL);   // notify Sqxc elements to finish
```

[SqxcSql](SqxcSql.md) will output SQL statement in above example:

```sql
INSERT INTO table_name (id, int_array) VALUES (1, '[ 2, 4 ]');
```

database table look like this:

| id | int_array |
| -- | --------- |
| 1  | [ 2, 4 ]  |

JSON look like this:

```json
{ "id": 1, "int_array": [ 2, 4] }
```

#### use C++ language to send array to Sqxc elements
If you want to parse/write object or array, and reuse Sqxc elements:
1. call ready() before sending data.
2. call finish() after sending data.

```c++
	// pointer to current Sqxc instance
	Sq::XcMethod *xcur;

	xc->ready();       // notify Sqxc elements to get ready
	xcur = xc;         // current Sqxc element

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

	xc->finish();      // notify Sqxc elements to finish
```

JSON look like this:

```json
[ 1, 3]
```

## Send data to user specified Sqxc element

Use sqxc_send_to() to pass data arguments to specified Sqxc elements.  
  
e.g. Pass data(arguments) to specified Sqxc element.  
  
use C language

```c
	// 'xc' is data source.
	Sqxc *xc     = data_src;
	Sqxc *xcjson = json_writer;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	sqxc_send_to(xcjson, xc);
```

use C++ language

```c++
	// 'xc' is data source.
	Sq::Xc *xc     = (Sq::Xc*)data_src;
	Sq::Xc *xcjson = (Sq::Xc*)json_writer;

	// set data arguments in 'xc'
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data arguments 'xc' to 'xcjson'
	xcjson->send(xc);
```

## How to support new format:
User can refer SqxcJsonc.h and SqxcJsonc.c to support new format.  
SqxcFile.h and SqxcFile.c is the simplest sample code, it just write string to file.  
SqxcEmpty.h and SqxcEmpty.c is a workable sample, but it do nothing.  

#### 1 define new structure that derived from Sqxc
All derived structure must conforme C++11 standard-layout

```c++
// This is header file - SqxcText.h
#include <Sqxc.h>

// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct SqxcText   SqxcText;

#ifdef __cplusplus    // mix C and C++
extern "C" {
#endif

// define in SqxcText.c
extern const SqxcInfo               sqxcInfo_TextParser;
#define SQXC_INFO_TEXT_PARSER     (&sqxcInfo_TextParser)

#ifdef __cplusplus    // mix C and C++
}  // extern "C"
#endif

#ifdef __cplusplus
struct SqxcText : Sq::XcMethod           // <-- 1. inherit C++ member function(method)
#else
struct SqxcText
#endif
{
	SQXC_MEMBERS;                        // <-- 2. inherit member variable

	int    other_data;                   // <-- 3. Add variable and non-virtual function in derived struct.
	int    status;

#ifdef __cplusplus
	SqxcText() {
		// call Sq::XcMethod::init()
		init(SQXC_INFO_TEXT_PARSER);
		// or call C function:
		// sqxc_init((Sqxc*)this, SQXC_INFO_TEXT_PARSER);
	}
	~SqxcText() {
		// call Sq::XcMethod::final()
		final();
		// or call C function:
		// sqxc_final((Sqxc*)this);
	}
#endif
};

```

#### 2 implement SqxcInfo interface

```c
// This is source file - SqxcText.c
#include <SqxcText.h>

// declare functions for SqxcInfo
static void sqxc_text_parser_init(SqxcText *xctext);
static void sqxc_text_parser_final(SqxcText *xctext);
static int  sqxc_text_parser_ctrl(SqxcText *xctext, int id, void *data);
static int  sqxc_text_parser_send(SqxcText *xctext, Sqxc *src);

// used by SqxcText.h
const SqxcInfo sqxcInfo_TextParser =
{
	.size  = sizeof(SqxcText),
	.init  = (SqInitFunc)   sqxc_text_parser_init,
	.final = (SqFinalFunc)  sqxc_text_parser_final,
	.ctrl  = (SqxcCtrlFunc) sqxc_text_parser_ctrl,
	.send  = (SqxcSendFunc) sqxc_text_parser_send,
};

// implement sqxc_text_parser_xxxx() functions here

static void sqxc_text_parser_init(SqxcText *xctext)
{
	// initialize SqxcText instance
}

static void sqxc_text_parser_final(SqxcText *xctext)
{
	// finalize SqxcText instance
}

static int  sqxc_text_parser_ctrl(SqxcText *xctext, int id, void *data)
{
	switch(id) {
	case SQXC_CTRL_READY:     // notify 'xctext' to get ready
		break;

	case SQXC_CTRL_FINISH:    // notify 'xctext' to finish
		break;

	default:
		return SQCODE_NOT_SUPPORTED;
	}
	return SQCODE_OK;
}

static int  sqxc_text_parser_send(SqxcText *xctext, Sqxc *src)
{
	Sqxc *xc_dest = xctext->dest;

	// parse arguments in 'src'

	// set parsed data to arguments of 'xctext'
	xctext->type = src->type;
	xctext->name = src->name;
	xctext->value.str = src->value.str;

	// send arguments of 'xctext' to 'xc_dest'
	sqxc_send_to(xc_dest, (Sqxc*)xctext);

	// set return code in src->code
	return (src->code = SQCODE_OK);
}
```

**Handle nested data types:**

When Sqxc handles nested data types like object or array, you may need to store/restore some value.
This library defines structure SqxcNested to store values and provides functions to push/pop SqxcNested:

| C++ methods  | C functions            | description                   |
| ------------ | ---------------------- | ----------------------------- |
| clearNested  | sqxc_clear_nested      | clear all SqxcNested in stack |
| eraseNested  | sqxc_erase_nested      | remove SqxcNested from stack  |
| removeNested | sqxc_remove_nested     | alias of eraseNested          |
| pushNested   | sqxc_push_nested       | push a SqxcNested to stack    |
| popNested    | sqxc_pop_nested        | pop a SqxcNested from stack   |

The C++ method pushNested() and the C function sqxc_push_nested() return a newly created SqxcNested that has been pushed onto the stack.

```c++
	Sqxc       *xc;
	SqxcNested *nested;

	// C++ method
//	nested = xc->pushNested();

	// C function
	nested = sqxc_push_nested(xc);

	// SqxcNested has 3 pointer to store values.
	nested->data  = pointer1;
	nested->data2 = pointer2;
	nested->data3 = pointer3;
```

The C++ method popNested() and the C function sqxc_pop_nested() can remove SqxcNested from the top of the stack.

```c++
	Sqxc       *xc;
	SqxcNested *nested;

	// C++ method
//	xc->popNested();

	// C function
	sqxc_pop_nested(xc);

	// get SqxcNested from the top of the stack.
	nested = xc->nested;

	// restore values from SqxcNested.
	pointer1 = nested->data;
	pointer2 = nested->data2;
	pointer3 = nested->data3;
```

If new Sqxc element want to parse/write data in database column, it must:  
1. support SQXC_TYPE_ARRAY or SQXC_TYPE_OBJECT.
2. send converted data to dest (or next) element. see below:

```c++
	Sqxc       *xc_dest;
	SqxcNested *nested;

	// handle nested type - object
	if (xc_src->type == SQXC_TYPE_OBJECT) {
		// store current values
		nested = sqxc_push_nested(xc_text);
		nested->data  = current_value1;
		nested->data2 = current_value2;
		nested->data3 = current_value3;
		// maybe do something here
		xc_src->code = SQCODE_OK;
		return SQCODE_OK;
	}
	else if (xc_src->type == SQXC_TYPE_OBJECT_END) {
		// restore previous values
		sqxc_pop_nested(xc_text);
		nested = xc_text->nested;
		current_value1 = nested->data;
		current_value2 = nested->data2;
		current_value3 = nested->data3;
		// maybe do something here
		xc_src->code = SQCODE_OK;
		return SQCODE_OK;
	}

	// converting data...

	// set converted data in 'xc_text'
	xc_text->type = SQXC_TYPE_INT;
	xc_text->name = NULL;
	xc_text->value.integer = 3;

	// send data from 'xc_text' to 'xc_dest' element
	xc_dest = xc_text->dest;
	/* C function */
	sqxc_send_to(xc_dest, xc_text);

	/* C++ method */
//	xc_dest->send(xc_text);
```

#### 3 use new Sqxc element

create custom Sqxc element and insert it to SqStorage::xc_input list.  
  
use C language

```c++
	Sqxc *xc_text;
	Sqxc *xc_json;

	// create custom Sqxc element
	xc_text = sqxc_new(SQXC_INFO_TEXT_PARSER);

	// append 'xc_text' parser to 'xc_input' list
//	sqxc_insert(storage->xc_input, xc_text, -1);

	// insert 'xc_text' parser to the 2nd element of 'xc_input' list
	sqxc_insert(storage->xc_input, xc_text, 1);

	// remove JSON parser from list because it is replaced by new one.
	xc_json = sqxc_find(storage->xc_input, SQXC_INFO_JSONC_PARSER);
	if (xc_json) {
		sqxc_steal(storage->xc_input, xc_json);
		// free 'xc_json' if no longer needed
		sqxc_free(xc_json);
	}
```

use C++ language

```c++
	SqxcText  *xc_text;
	Sq::Xc    *xc_json;

	// create custom Sqxc element
	xc_text = new SqxcText();

	// append 'xc_text' parser to 'xc_input' list
//	storage->xc_input->insert(xc_text, -1);

	// insert 'xc_text' parser to the 2nd element of 'xc_input' list
	storage->xc_input->insert(xc_text, 1);

	// remove JSON parser from list because it is replaced by new one.
	xc_json = storage->xc_input->find(SQXC_INFO_JSONC_PARSER);
	if (xc_json) {
		storage->xc_input->steal(xc_json);
		// free 'xc_json' if no longer needed
		delete xc_json;
	}
```

The Sqxc input data flow in your [SqStorage](SqStorage.md) object will look like this:

	input ->         ┌─> SqxcTextParser ──┐
	sqdb_exec()    ──┴────────────────────┴─> SqxcValue ───> SqType::parse()

Note: You also need replace SqxcJsoncWriter by SqxcTextWriter in SqStorage::xc_output.

## Processing (skip) unknown object & array

There is a JSON object defined below.

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

If no 'undefinedObject' defined by SqEntry (or SqColumn), SqxcValue will ignore 'undefinedObject' when parsing above JSON object.
