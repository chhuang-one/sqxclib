# Sqxc

Sqxc convert X to/from C value (X = SQL, JSON...etc)  
It contain status, buffer, and input/output arguments in one C structure.  
User can link multiple Sqxc elements to convert different types of data.  

| element name | description           | source file |
| ------------ | --------------------- | ----------- |
| SqxcSql      | convert to SQL (Sqdb) | SqxcSql.c   |
| SqxcJsonc    | convert to/from JSON  | SqxcJsonc.c |
| SqxcValue    | convert to C struct   | SqxcValue.c |

data type for Sqxc converter

| Sqxc type name   | description                                |
| ---------------- | ------------------------------------------ |
| SQXC_TYPE_BOOL   | corresponds to C data type:  bool          |
| SQXC_TYPE_INT    | corresponds to C data type:  int           |
| SQXC_TYPE_UINT   | corresponds to C data type:  unsigned int  |
| SQXC_TYPE_INT64  | corresponds to C data type:  int64_t       |
| SQXC_TYPE_UINT64 | corresponds to C data type:  uint64_t      |
| SQXC_TYPE_TIME   | corresponds to C data type:  time_t        |
| SQXC_TYPE_DOUBLE | corresponds to C data type:  double        |
| SQXC_TYPE_STRING | corresponds to C data type:  char*         |
| SQXC_TYPE_OBJECT | Start of object                            |
| SQXC_TYPE_ARRAY  | Start of array (or other container)        |
| SQXC_TYPE_STREAM | Text stream must be null-terminated string |
| SQXC_TYPE_OBJECT_END | End of object                          |
| SQXC_TYPE_ARRAY_END  | End of array (or other container)      |
| SQXC_TYPE_STREAM_END | End of stream (reserve, unused now)    |

Note: SQXC_TYPE_OBJECT corresponds to SQL row.  
Note: SQXC_TYPE_ARRAY  corresponds to SQL multiple row.  

## Create Sqxc elements
create element to convert data to SQL INSERT/UPDATE statement and add element to convert data to JSON array/object in SQL column.

* use C Language
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

* use C++ Language
```c++
	Sq::XcSql         *xcsql  = new Sq::XcSql();
	Sq::XcJsoncWriter *xcjson = new Sq::XcJsoncWriter();

	// append JSON writer to Sqxc chain
	xcsql->insert(xcjson);
```

## Send data according to data type
  Use sqxc_send() to send data(arguments) between Sqxc elements.  

	input ->         ┌─> SqxcJsonParser ──┐
	Sqdb.exec()    ──┴────────────────────┴──> SqxcValue ───> SqType.parse()

Note: If SqxcValue can't match current data type, it will forward data to SqxcJsonParser (or other element).  


	output ->        ┌─> SqxcJsonWriter ──┐
	SqType.write() ──┴────────────────────┴──> SqxcSql   ───> Sqdb.exec()

Note: If SqxcSql doesn't support current data type, it will forward data to SqxcJsonWriter (or other element).  

sqxc_send() is called by data source side. It send data(arguments) to Sqxc element and try to match type in Sqxc chain.  
Because difference data type is processed by difference Sqxc element, It returns current Sqxc elements.  
  
Before you call sqxc_send(), set data type, data name, and data value in Sqxc structure.
These data(arguments) will be processed between Sqxc elements.

* use C Language
```c
	Sqxc *xc = xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// sqxc_send() pass data(arguments) between Sqxc elements and
	//             return current Sqxc element that processing data(arguments)
	xc = sqxc_send(xc);

	// get error code from current Sqxc element
	if (xc->code != SQCODE_OK)
		return;    // error
```

* use C++ Language
```c++
	Sq::Xc *xc = (Sq::Xc*)xcsql;

	// set Sqxc arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;

	// send() pass data(arguments) between Sqxc elements and
	//        return current Sqxc element that processing data(arguments)
	xc = xc->send();

	// get error code from current Sqxc element
	if (xc->code != SQCODE_OK)
		return;    // error
```

#### use C function to send object data to Sqxc elements
- If you want to parse/write object or array and reuse Sqxc elements:
1. call sqxc_ready() before sending data.
2. call sqxc_finish() after sending data.

```c
	Sqxc *xcur = xc;   // current Sqxc element

	sqxc_ready(xc);    // notify Sqxc elements to get ready

	xcur->type = SQXC_TYPE_OBJECT;      // {
	xcur->name = NULL;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_INT;         // "id": 1
	xcur->name = "id";
	xcur->value.integer = 1;
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_ARRAY;       // "int_array": [
	xcur->name = "int_array";
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
	xcur = sqxc_send(xcur);

	xcur->type = SQXC_TYPE_OBJECT_END;  // }
	xcur->name = NULL;
	xcur = sqxc_send(xcur);

	sqxc_finish(xc);    // notify Sqxc elements to finish
```

SqxcSql will output SQL statement:
```sql
INSERT INTO table_name (id, int_array) VALUES (1, '[ 2, 4 ]');
```

SQL table look like this:
| id | int_array |
| -- | --------- |
| 1  | [ 2, 4 ]  |

JSON look like this:
```json
{ "id": 1, "int_array": [ 2, 4] }
```

#### use C++ function to send array to Sqxc elements
- If you want to parse/write object or array and reuse Sqxc elements:
1. call xc->ready() before sending data.
2. call xc->finish() after sending data.

```c++
	Sq::Xc *xcur = xc; // current Sqxc element

	xc->ready();       // notify Sqxc elements to get ready

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
  Use sqxc_send_to() to pass data(arguments) to specified Sqxc elements.  

	user output ────> SqxcJsonWriter ────> SqxcFile ────> fwrite()

Note: SqxcFile is in sqxctest library. Sample code is in [xc_json_file.cpp](../examples/xc_json_file.cpp)

* use C Language
```c
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data(arguments) 'xc' to 'xcjson' (type is SqxcJsonWriter)
	sqxc_send_to(xcjson, xc);
```

* use C++ Language
```c++
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 100;

	// pass data(arguments) 'xc' to 'xcjson' (type is Sq::XcJsonWriter)
	xcjson->send(xc);
```

## How to support new format:
 User can refer SqxcJsonc.h and SqxcJsonc.c to support new format.  
 SqxcEmpty.h and SqxcEmpty.c is a workable sample, but it do nothing.  

#### 1. define new structure that derived from Sqxc
 All derived structure must conforme C++11 standard-layout

```c++
// This is header file - SqxcText.h
#include <Sqxc.h>

// define type - SqxcText for C Language
typedef struct SqxcText   SqxcText;

// define in SqxcText.c
extern const SqxcInfo    *SQXC_INFO_TEXT_PARSER;

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
		sqxc_init((Sqxc*)this, SQXC_INFO_TEXT_PARSER);
	}
	~SqxcText() {
		sqxc_final((Sqxc*)this);
	}
#endif
};

```

#### 2. implement SqxcInfo interface

```c
// This is source file - SqxcText.c
#include <SqxcText.h>

// declare functions for SqxcInfo
static void sqxc_text_parser_init(SqxcText *xctext);
static void sqxc_text_parser_final(SqxcText *xctext);
static int  sqxc_text_parser_ctrl(SqxcText *xctext, int id, void *data);
static int  sqxc_text_parser_send(SqxcText *xctext, Sqxc *src);

static const SqxcInfo sqxc_text_parser =
{
	.size  = sizeof(SqxcText),
	.init  = (SqInitFunc)   sqxc_text_parser_init,
	.final = (SqFinalFunc)  sqxc_text_parser_final,
	.ctrl  = (SqxcCtrlFunc) sqxc_text_parser_ctrl,
	.send  = (SqxcSendFunc) sqxc_text_parser_send,
};

// used by SqxcText.h
const SqxcInfo *SQXC_INFO_TEXT_PARSER = &sqxc_text_parser;

// implement sqxc_text_parser_xxxx() functions here
//
```

Note: If new Sqxc element want to parse/write data in SQL column, it must:  
1. support SQXC_TYPE_ARRAY or SQXC_TYPE_OBJECT.
2. send converted data to dest (next) element. see below:

```c++
	Sqxc *xc_dest;

	// set converted data in xc_text
	xc_text->type = SQXC_TYPE_INT;
	xc_text->name = NULL;
	xc_text->value.integer = 3;

	// send data from xc_text to xc_dest element
	xc_dest = xc_text->dest;
	/* C function */
	sqxc_send_to(xc_dest, xc_text);

	/* C++ function */
//	xc_dest->send(xc_text);
```

#### 3. use new Sqxc element

append custom Sqxc element to tail of list.

```c++
	Sqxc *xc_text;

	// create custom Sqxc element
	xc_text = sqxc_new(SQXC_INFO_TEXT_PARSER);

	// append xc_text parser to tail of list
	/* C function */
	sqxc_insert(xc, xc_text, -1);
	/* C++ function */
//	xc->insert(xc_text, -1);
```

insert custom Sqxc element to input elements in SqStorage object.

```c++
	Sqxc *xc_text;
	Sqxc *xc_json;

	// create custom Sqxc element
	xc_text = sqxc_new(SQXC_INFO_TEXT_PARSER);

	// insert xc_text parser after storage->xc_input
	/* C function */
	sqxc_insert(storage->xc_input, xc_text, 1);
	/* C++ function */
//	storage->xc_input->insert(xc_text, 1);

	// remove JSON parser from list because it is replaced by new one.
	/* C function */
	xc_json = sqxc_find(storage->xc_input, SQXC_INFO_JSONC_PARSER);
	/* C++ function */
//	xc_json = storage->xc_input->find(SQXC_INFO_JSONC_PARSER);

	if (xc_json) {
		/* C function */
		sqxc_steal(storage->xc_input, xc_json);
		/* C++ function */
//		storage->xc_input->steal(xc_json);

//		sqxc_free(xc_json);
	}

	// You may also need:
	// insert xc_text writer after storage->xc_output
	// remove JSON writer from list because it is replaced by new one.
```

The Sqxc input dataflow in your SqStorage object will look like this:

	input ->         ┌-> SqxcTextParser --┐
	Sqdb.exec()    --┴--------------------┴-> SqxcValue ---> SqType.parse()

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

In this case, it will cause an error when Sqxc parsing above JSON object if no 'undefinedObject' defined by SqEntry/SqColumn.  
If you want to skip unknown(undefine) object & array like 'undefinedObject', please enable SQ_CONFIG_SQXC_UNKNOWN_SKIP in SqConfig.h
