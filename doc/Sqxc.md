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

- Note: SQXC_TYPE_OBJECT corresponds to SQL row.
- Note: SQXC_TYPE_ARRAY  corresponds to SQL multiple row.

### Sqxc dataflow

	                 +-> SqxcJsonParser --+
	( input )        |                    |
	Sqdb.exec()    --+--------------------+-> SqxcValue ---> SqType.parse()
	                 |                    |
	                 +--> SqxcXmlParser --+
Note: If SqxcValue can't match current data type, it will forward data to SqxcJsonParser (or other element).


	                 +-> SqxcJsonWriter --+
	( output )       |                    |
	SqType.write() --+--------------------+-> SqxcSql   ---> Sqdb.exec()
	                 |                    |
	                 +--> SqxcXmlWriter --+
Note: If SqxcSql doesn't support current data type, it will forward data to SqxcJsonWriter (or other element).

### Use sqxc_send() to send data to Sqxc elements.

Before you call sqxc_send(), set data type, data name, and data value in Sqxc structure.
These data will process between Sqxc elements.
```c
	// sqxc_send() input arguments
	sqxc->type = SQXC_TYPE_INT;
	sqxc->name = "id";
	sqxc->value.integer = 105;
```

sqxc_send() return current Sqxc element that processing data
```c
	sqxc = sqxc_send(sqxc);
```

get error code from current Sqxc element
```c
	// get result
	if (sqxc->code != SQCODE_OK)
		return;    // error
```

### Use Sqxc elements to convert data
create element to convert data to SQL INSERT/UPDATE statement
```c
	Sqxc *xc = sqxc_sql_new();
```

add element to convert data to JSON array/object in SQL column.
```c
	Sqxc *xc_json = sqxc_jsonc_writer_new();
	sqxc_insert(xc, xc_json, -1);
```

send object data to Sqxc elements

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
