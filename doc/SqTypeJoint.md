[中文](SqTypeJoint.cn.md)

# SqTypeJoint

SqTypeJoint is the default type for handling query that join multi-table. It can create array of pointers for query result.

	SqType
	│
	└─── SqTypeJoint

## Create and setup

Because SqStorage will set built-in SqTypeJoint when querying, it generally does not need to be set.  
But when used alone, user must setup SqTypeJoint before using it.
Because the parser of SqTypeJoint must know type of columns in table, user must add, remove, clear tables in SqTypeJoint.  
You can specify table alias name (used by SQL AS keyword) in the last argument when you add or remove tables. If alias name is NULL, Sqxc use table name directly.  
  
use C language

```c
	SqTypeJoint *type;
	SqTable     *table = table_instance;

	// Create
	type = sq_type_joint_new();

	// Setup
	sq_type_joint_add(type, table, "tableAsName");
	sq_type_joint_remove(type, table2, "table2AsName");
	sq_type_joint_clear(type);
```

use C++ language

```c++
	Sq::TypeJoint *type;
	Sq::Table     *table = table_instance;

	// Create
	type = new Sq::TypeJoint();

	// Setup
	type->add(table, "tableAsName");
	type->remove(table2, "table2AsName");
	type->clear();
```

## Element of container

Because SqTypeJoint create pointer array, user can specify pointer to pointer (double pointer) as element of container.
The value of the element is determined by the query statement.  
  
Example: Build query and get result:  
The first table in the query is "cities" and the second table is "users".

```sql
SELECT * FROM cities JOIN users ON cities.id = users.city_id
```

use C language

```c
	// SQL statement:
	// SELECT * FROM cities JOIN users ON cities.id = users.city_id
	sq_query_from(query, "cities");
	sq_query_join(query, "users", "cities.id", "=", "%s", "users.city_id");

	SqPtrArray *array = sq_storage_query(storage, query, NULL, NULL);

	void **element = (void**)array->data[index];
	City  *city = (City*)element[0];    // instance of 1st table
	User  *user = (User*)element[1];    // instance of 2nd table

	// Because SqPtrArray doesn't free elements by default, free elements before freeing array.
//	free(element);
```

use C++ STL

```c++
	// SQL statement:
	// SELECT * FROM cities JOIN users ON cities.id = users.city_id
	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	std::vector<void**> *vector = storage->query< std::vector<void**> >(query);

	void **element = vector->at(index);
	City  *city = (City*)element[0];    // instance of 1st table
	User  *user = (User*)element[1];    // instance of 2nd table
```

If you don't want to use pointer as element of container, you can define array as new data type to replace it.  
When you define array as data type, it must specify the size of the array.
For example, if query joins 2 tables, the size of array must specify a number of 2 or greater.  
  
use C language

```c
	typedef void *Joint2[2];    // define array as new data type
	SqArray *array;

	// specify the table type as NULL (default value)
	// specify the container type of returned data as SQ_TYPE_ARRAY
	array = sq_storage_query(storage, query, NULL, SQ_TYPE_ARRAY);

	void **element = sq_array_at(array, Joint2, index);
	// element[0] = instance of 1st table
	// element[1] = instance of 2nd table
```

use C++ language

```c++
	typedef void *Joint2[2];    // define array as new data type
	Sq::Array<Joint2> *array;

	// specify the container type of returned data as SQ_TYPE_ARRAY
	array = (Sq::Array<Joint2>*) storage->query(query, SQ_TYPE_ARRAY);

	Joint2 &element = array->at(index);
//	void  **element = vector->at(index);    // this also workable
	// element[0] = instance of 1st table
	// element[1] = instance of 2nd table
```

If you use C++ STL container or do not want to define new data type, you can use Sq::Joint instead. It just wraps array of pointers into struct.  
When Sq::Joint is used, it must specify the size of the array.
For example, if query joins 2 tables, the size of Sq::Joint array must specify a number of 2 or greater.

```c++
	// the size of Sq::Joint array is 2
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector< Sq::Joint<2> > >(query);

	Sq::Joint<2> &joint = vector->at(index);
	// joint[0] = instance of 1st table
	// joint[1] = instance of 2nd table
```
