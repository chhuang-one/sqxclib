[中文](SqTypeRow.cn.md)

# SqTypeRow

SqTypeRow is derived from SqTypeJoint. It can create [SqRow](SqRow.md) and handle unknown (or known) result, table, and column.  
SqTypeRow sample code is in [storage-row.cpp](examples/storage-row.cpp)  
Note1: SqTypeRow can also use with get() and getAll().  
Note2: SqTypeRow is in sqxcsupport library (sqxcsupport.h).  

	SqType
	│
	└─── SqTypeJoint
	     │
	     └─── SqTypeRow

## Directly use without setup

SQ_TYPE_ROW is build-in static constant type for SqTypeRow, user can use it to handle unknown result directly.
In this case, all data type in [SqRow](SqRow.md) is C string because SqTypeRow don't know type of columns.  
  
use C language

```c
	SqRow      *row;
	SqPtrArray *array;

	// specify the table type as SQ_TYPE_ROW
	row = sq_storage_get(storage, "users", SQ_TYPE_ROW, 11);

	// specify the table type as SQ_TYPE_ROW
	// specify the container type of returned data as SQ_TYPE_PTR_ARRAY
	array = sq_storage_query(storage, query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);
```

use C++ language

```c++
	Sq::Row      *row;
	Sq::PtrArray *array;

	// specify the table type as SQ_TYPE_ROW
	row = (Sq::Row*) storage->get("users", SQ_TYPE_ROW, 11);

	// specify the table type as SQ_TYPE_ROW
	// specify the container type of returned data as SQ_TYPE_PTR_ARRAY
	array = (Sq::PtrArray*) storage->query(query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);
```

## Create and setup SqTypeRow

If you want get correct data type from [SqRow](SqRow.md), you must create dynamic SqTypeRow and setup it.
Because the parser of SqTypeRow must know type of columns in table, user must add, remove, clear tables in SqTypeRow.  
  
use C language

```c
	SqTypeRow *type  = sq_type_row_new();
	SqTable   *table = table_instance;

	sq_type_row_add(type, table, "tableAsName");

	sq_type_row_remove(type, table2, "table2AsName");

	sq_type_row_clear(type);
```

use C++ language

```c++
	Sq::TypeRow *type  = new Sq::TypeRow();
	Sq::Table   *table = table_instance;

	type->add(table, "tableAsName");

	type->remove(table2, "table2AsName");

	type->clear();
```

## Works with SqStorage

You can use SqTypeRow to replace default joint type in SqStorage:

```c++
	Sq::TypeRow   *typeRow     = new Sq::TypeRow();
	Sq::TypeJoint *typeDefault = storage->joint_default;
	storage->joint_default = typeRow;
	delete typeDefault;
```

**When SqTypeRow is default joint type, the element type of query() result is SqRow in the following cases:**
1. 'query' has joined multi-table.
2. 'query' has unknown table or unknown result.

```c++
	std::vector<Sq::Row> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query<std::vector<Sq::Row>>(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		// get Sq::Row from std::vector
		Sq::Row &row = vector->at(index);
		// handle columns in Sq::Row
		for (unsigned int nth = 0;  nth < row.length;  nth++) {
			std::cout << row.cols[nth].name << " = ";
			// column value type is decided by 'row.cols[nth].type'
			if (row.cols[nth].type == SQ_TYPE_INT)
				std::cout << row.data[nth].integer << std::endl;
			if (row.cols[nth].type == SQ_TYPE_STRING)
				std::cout << row.data[nth].string  << std::endl;
			// other type...
		}
	}
```

**If you don't want to change default joint type:**
1. call sq_storage_setup_query() to setup 'query' and 'typeRow'.
2. call sq_storage_query() with 'typeRow'.
3. If you skip step 1, all data type in [SqRow](SqRow.md) is C string because SqTypeRow don't know type of column.

function sq_storage_setup_query() declarations:

```c
// C function
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

// C++ method
Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
```

It setup 'query' and 'type_joint' then return SqType for calling sq_storage_query().  
It will add "SELECT table.column AS 'table.column'" in 'query' if 'query' has joined multi-table.  

| Return value  | Description                                                                |
| ------------- | ---------------------------------------------------------------------------|
| NULL          | if table not found and 'type_joint' can NOT handle unknown table type.     |
| 'type_joint'  | if 'query' has joined multi-table. It will setup 'type_joint' and 'query'. |
| type of table | if 'query' has only 1 table. It will setup 'type_joint' but keep 'query' no change. In this case, user can call sq_storage_query() with returned type or 'type_joint'. |

use C functions

```c
	SqType *typeContainer = NULL;

	sq_storage_setup_query(storage, query, typeRow);
	container = sq_storage_query(storage, query, typeRow, typeContainer);
```

use C++ methods

```c++
	Sq::Type *typeContainer = NULL;

	storage->setupQuery(query, typeRow);
	container = storage->query(query, typeRow, typeContainer);
```

use C++ STL

```c++
	storage->setupQuery(query, typeRow);
	vector = storage->query<std::vector<Sq::Row>>(query, typeRow);
```
