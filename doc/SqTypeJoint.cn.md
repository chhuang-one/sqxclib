[English](SqTypeJoint.md)

# SqTypeJoint

SqTypeJoint 是处理多表连接查询的默认类型。它可以为查询结果创建指针数组。

	SqType
	│
	└─── SqTypeJoint

## 创建和设置

用户必须在使用前设置 SqTypeJoint。
因为 SqTypeJoint 的解析器必须知道表中列的类型，所以用户必须在 SqTypeJoint 中添加、删除、清除表。  
  
使用 C 语言

```c
	SqTypeJoint *type;
	SqTable     *table = table_instance;

	type = sq_type_joint_new();

	sq_type_joint_add(type, table, "tableAsName");

	sq_type_joint_remove(type, table2, "table2AsName");

	sq_type_joint_clear(type);
```

使用 C++ 语言

```c++
	Sq::TypeJoint *type;
	Sq::Table     *table = table_instance;

	type = new Sq::TypeJoint();

	type->add(table, "tableAsName");

	type->remove(table2, "table2AsName");

	type->clear();
```

## 容器元素

因为 SqTypeJoint 创建指针数组，用户可以将指向指针的指针（double pointer）指定为容器的元素。

```c++
	void **element;

	vector = storage->query< std::vector<void**> >(query);
	element = vector->at(index);

	// element[0] = 第一个表结构的实例
	// element[1] = 第二个表结构的实例
```

如果你不想使用指针作为容器的元素，您可以将数组定义为新的数据类型来替换它。  
将数组定义为数据类型时，必须指定数组的大小。例如，如果查询连接 2 个表，则数组的大小必须指定为 2 或更大的数字。  
  
使用 C 语言

```c
	typedef void *Joint2[2];    // 将数组定义为新的数据类型
	SqArray *array;

	// 指定表类型为 NULL（默认值）
	// 指定返回数据的容器类型为 SQ_TYPE_ARRAY
	array = sq_storage_query(storage, query, NULL, SQ_TYPE_ARRAY);
	void **element = sq_array_at(array, Joint2, index);

	// element[0] = 第一个表结构的实例
	// element[1] = 第二个表结构的实例
```

使用 C++ 语言

```c++
	typedef void *Joint2[2];    // 将数组定义为新的数据类型
	Sq::Array<Joint2> *array;

	// 指定返回数据的容器类型为 SQ_TYPE_ARRAY
	array = (Sq::Array<Joint2>*) storage->query(query, SQ_TYPE_ARRAY);
	Joint2 &element = array->at(index);
//	void  **element = array->at(index);    // 這也可行

	// element[0] = 第一个表结构的实例
	// element[1] = 第二个表结构的实例
```

如果您使用 C++ STL 容器或不想定義新的數據類型，你可以使用 Sq::Joint 来代替它。它只是将指针数组包装到结构中。  
使用 Sq::Joint 时，必须指定数组的大小。例如，如果查询连接 2 个表，则 Sq::Joint 数组的大小必须指定为 2 或更大的数字。

```c++
	// Sq::Joint 的数组大小为 2
	std::vector< Sq::Joint<2> > *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query< std::vector< Sq::Joint<2> > >(query);
	Sq::Joint<2> &joint = vector->at(index);

	// joint[0] = 第一个表结构的实例
	// joint[1] = 第二个表结构的实例
```
