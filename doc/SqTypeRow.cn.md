[English](SqTypeRow.md)

# SqTypeRow

SqTypeRow 派生自 SqTypeJoint。它可以创建 [SqRow](SqRow.cn.md) 并处理未知（或已知）的结果、表和列。  
SqTypeRow 示例代码在 [storage-row.cpp](examples/storage-row.cpp)  
注意1: SqTypeRow 也可以与 get() 和 getAll() 一起使用。  
注意2: SqTypeRow 在 sqxcsupport 库中 (sqxcsupport.h)。  

	SqType
	│
	└─── SqTypeJoint
	     │
	     └─── SqTypeRow

## 无需设置直接使用

SQ_TYPE_ROW 是 SqTypeRow 内置的静态常量类型，用户可以直接用它来处理未知的结果。
在这种情况下，[SqRow](SqRow.cn.md) 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。  
  
使用 C 语言

```c
	SqRow      *row;
	SqPtrArray *array;

	// 指定返回数据的表类型和容器类型
	// SQ_TYPE_ROW 是表类型。
	// SQ_TYPE_PTR_ARRAY 是返回的容器类型。
	array = sq_storage_query(storage, query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	for (int  nth_row = 0;  nth_row < array->length;  nth_row++) {
		// 从数组中获取 SqRow
		row = array->data[nth_row];
		// 处理 SqRow 中的列
		for (int  index = 0;  index < row->length;  index++) {
			// 列名
			puts(row->cols[index].name);
			// 列值类型由 'row->cols[index].type' 决定
			if (row->cols[index].type == SQ_TYPE_STRING)
				puts(row->data[index].string);
		}
	}
```

使用 C++ 语言

```c++
	Sq::Row      *row;
	Sq::PtrArray *array;

	// 指定返回数据的表类型和容器类型
	// SQ_TYPE_ROW 是表类型。
	// SQ_TYPE_PTR_ARRAY 是返回的容器类型。
	array = (Sq::PtrArray*) storage->query(query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);

	for (int  nthRow = 0;  nthRow < array->length;  nthRow++) {
		// 从数组中获取 Sq::Row
		row = (Sq::Row*) array->data[nthRow];
		// 处理 Sq::Row 中的列
		for (int  index = 0;  index < row->length;  index++) {
			// 列名
			std::cout << row->cols[index].name << std::endl;
			// 列值类型由 'row->cols[index].type' 决定
			if (row->cols[index].type == SQ_TYPE_STRING)
				std::cout << row->data[index].string << std::endl;
		}
	}
```

## 创建和设置 SqTypeRow

如果你想从 [SqRow](SqRow.cn.md) 获得正确的数据类型，你必须创建动态 SqTypeRow 并设置它。
因为 SqTypeRow 的解析器必须知道表中列的类型，所以用户必须在 SqTypeRow 中添加、删除、清除表。  
  
使用 C 语言

```c
	SqTypeRow *type  = sq_type_row_new();
	SqTable   *table = table_instance;

	sq_type_row_add(type, table, "tableAsName");

	sq_type_row_remove(type, table2, "table2AsName");

	sq_type_row_clear(type);
```

使用 C++ 语言

```c++
	Sq::TypeRow *type  = new Sq::TypeRow();
	Sq::Table   *table = table_instance;

	type->add(table, "tableAsName");

	type->remove(table2, "table2AsName");

	type->clear();
```

## 与 SqStorage 一起使用

您可以使用 SqTypeRow 替换 SqStorage 中的默认联合类型:

```c++
	Sq::TypeRow   *typeRow     = new Sq::TypeRow();
	Sq::TypeJoint *typeDefault = storage->joint_default;
	storage->joint_default = typeRow;
	delete typeDefault;
```

**当 SqTypeRow 为默认联合类型时，在以下情况下 query() 结果的元素类型为 SqRow：**
1. 'query' 已经加入了多表。
2. 'query' 有未知表或未知结果。

```c++
	std::vector<Sq::Row> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query<std::vector<Sq::Row>>(query);
	for (unsigned int index = 0;  index < vector->size();  index++) {
		Sq::Row &row = vector->at(index);
		for (unsigned int nth = 0;  nth < row.length;  nth++) {
			std::cout << row.cols[nth].name << " = ";
			if (row.cols[nth].type == SQ_TYPE_INT)
				std::cout << row.data[nth].integer << std::endl;
			if (row.cols[nth].type == SQ_TYPE_STRING)
				std::cout << row.data[nth].string  << std::endl;
			// other type...
		}
	}
```

**如果您不想更改默认联合类型：**
1. 调用 sq_storage_setup_query() 来设置 'query' 和 'typeRow'。
2. 调用 sq_storage_query() 時指定使用 'typeRow'。
3. 如果您跳过第 1 步，则 [SqRow](SqRow.cn.md) 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。

函数 sq_storage_setup_query() 声明：

```c++
// C 函数
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *type_joint);

// C++ 方法
Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *jointType);
```

它设置 'query' 和 'type_joint' 然后返回 SqType 以调用 sq_storage_query()。  
如果 'query' 已加入多表，它将在 'query' 中添加 "SELECT table.column AS 'table.column'"。  

| 返回值       | 描述                                                                       |
| ------------ | ---------------------------------------------------------------------------|
| NULL         | 如果找不到表 并且 'type_joint' 不能处理未知的表类型。                      |
| 'type_joint' | 如果 'query' 已加入多表。它将设置 'type_joint' 和 'query'。                |
| 其他表类型   | 如果 'query' 只有 1个表。它将设置 'type_joint' 但保持 'query' 不变。在这种情况下，用户可以使用返回的类型或'type_joint' 调用 sq_storage_query()。 |

使用 C 函数

```c
	SqType *typeContainer = NULL;

	sq_storage_setup_query(storage, query, typeRow);
	container = sq_storage_query(storage, query, typeRow, typeContainer);
```

使用 C++ 方法

```c++
	Sq::Type *typeContainer = NULL;

	storage->setupQuery(query, typeRow);
	container = storage->query(query, typeRow, typeContainer);
```

使用 C++ STL

```c++
	storage->setupQuery(query, typeRow);
	vector = storage->query<std::vector<Sq::Row>>(query, typeRow);
```
