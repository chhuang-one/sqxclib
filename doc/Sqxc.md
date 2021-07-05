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
| SQXC_TYPE_STREAM_END | End of stream                          |

Note: SQXC_TYPE_OBJECT corresponds to SQL row.  
Note: SQXC_TYPE_ARRAY  corresponds to SQL multiple row.  

## Sqxc dataflow

	                 +-> SqxcJsonParser --+
	( input )        |                    |
	Sqdb.exec()    --+--------------------+-> SqxcValue ---> SqType.parse()
	                 |                    |
	                 +--> SqxcXmlParser --+
Note: If SqxcValue can't match current data type, it will forward data to SqxcJsonParser (or other element).  
Note: SqxcXmlParser doesn't implement yet because it is rarely used.  


	                 +-> SqxcJsonWriter --+
	( output )       |                    |
	SqType.write() --+--------------------+-> SqxcSql   ---> Sqdb.exec()
	                 |                    |
	                 +--> SqxcXmlWriter --+
Note: If SqxcSql doesn't support current data type, it will forward data to SqxcJsonWriter (or other element).  
Note: SqxcXmlWriter doesn't implement yet because it is rarely used.  

## Use sqxc_send() to send data to Sqxc elements.

Before you call sqxc_send(), set data type, data name, and data value in Sqxc structure.
These data will process between Sqxc elements.
```c
	// sqxc_send() input arguments
	xc->type = SQXC_TYPE_INT;
	xc->name = "id";
	xc->value.integer = 105;
```

sqxc_send() return current Sqxc element that processing data
```c++
	/* C function */
	xc = sqxc_send(xc);
	/* C++ function */
//	xc = xc->send();
```

get error code from current Sqxc element
```c
	// get result
	if (xc->code != SQCODE_OK)
		return;    // error
```

## Use Sqxc elements to convert data
create element to convert data to SQL INSERT/UPDATE statement
```c
	Sqxc *xc;

	xc = sqxc_new(SQXC_INFO_SQL);    // suggest using this
//	xc = sqxc_sql_new();
```

add element to convert data to JSON array/object in SQL column.
```c++
	Sqxc *xc_json;

	xc_json = sqxc_new(SQXC_INFO_JSONC_WRITER);    // suggest using this
//	xc_json = sqxc_jsonc_writer_new();

	// append xc_json to tail of list
	/* C function */
	sqxc_insert(xc, xc_json, -1);
	/* C++ function */
//	xc->insert(xc_json, -1);
```

#### use C function to send object data to Sqxc elements

```c
	Sqxc *dest = xc;   // current Sqxc element

	sqxc_ready(xc);    // notify Sqxc elements to get ready

	dest->type = SQXC_TYPE_OBJECT;      // {
	dest->name = NULL;
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_INT;         // "id": 1
	dest->name = "id";
	dest->value.integer = 1;
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_ARRAY;       // "ints": [
	dest->name = "int_array";
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_INT;         // 2,
	dest->name = NULL;
	dest->value.integer = 2;
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_INT;         // 4
	dest->name = NULL;
	dest->value.integer = 4;
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_ARRAY_END;   // ]
	dest->name = NULL;
	dest = sqxc_send(dest);

	dest->type = SQXC_TYPE_OBJECT_END;  // }
	dest->name = NULL;
	dest = sqxc_send(dest);

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

```c++
	Sq::Xc *dest = xc; // current Sqxc element

	xc->ready();       // notify Sqxc elements to get ready

	dest->type = SQXC_TYPE_ARRAY;       // [
	dest->name = NULL;
	dest = dest->send();

	dest->type = SQXC_TYPE_INT;         // 1,
	dest->name = NULL;
	dest->value.integer = 1;
	dest = dest->send();

	dest->type = SQXC_TYPE_INT;         // 3
	dest->name = NULL;
	dest->value.integer = 3;
	dest = dest->send();

	dest->type = SQXC_TYPE_ARRAY_END;   // ]
	dest->name = NULL;
	dest = dest->send();

	xc->finish();      // notify Sqxc elements to finish
```

JSON look like this:
```json
[ 1, 3]
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
	xc_dest->info->send(xc_dest, xc_text);
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

	                 +-> SqxcTextParser --+
	( input )        |                    |
	Sqdb.exec()    --+--------------------+-> SqxcValue ---> SqType.parse()

