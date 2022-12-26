[中文](SqTypeJoint.cn.md)

# SqTypeJoint

SqTypeJoint is the default type for handling query that join multi-table. It can create array of pointers for query result.

	SqType
	│
	└─── SqTypeJoint

## Create and setup

User must setup SqTypeJoint before using it.
Because the parser of SqTypeJoint must know type of columns in table, user must add, remove, clear tables in SqTypeJoint.  
  
use C language

```c
	SqTypeJoint *type;
	SqTable     *table = table_instance;

	type = sq_type_joint_new();

	sq_type_joint_add(type, table, "tableAsName");

	sq_type_joint_remove(type, table2, "table2AsName");

	sq_type_joint_clear(type);
```

use C++ language

```c++
	Sq::TypeJoint *type;
	Sq::Table     *table = table_instance;

	type = new Sq::TypeJoint();

	type->add(table, "tableAsName");

	type->remove(table2, "table2AsName");

	type->clear();
```

## Element of container

Because SqTypeJoint create pointer array, user can specify pointer to pointer (double pointer) as element of container.

```c++
	void **element;

	vector = storage->query< std::vector<void**> >(query);
	element = vector->at(index);

	// element[0] = instance of 1st table struct
	// element[1] = instance of 2nd table struct
```

If you don't want to use pointer as element of C++ STL container, you can use Sq::Joint to replace it.  
Sq::Joint just wraps array of pointers into struct. User must use it with C++ STL because C++ STL can NOT directly use array as element of container.  
When Sq::Joint is used, it must specify the size of the array. For example, if query joins 2 tables, the size of Sq::Joint array must specify a number of 2 or greater.

```c++
	// the size of Sq::Joint array is 2
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector< Sq::Joint<2> > >(query);
	Sq::Joint<2> &joint = vector->at(index);

	// joint[0] = instance of 1st table struct
	// joint[1] = instance of 2nd table struct
```
