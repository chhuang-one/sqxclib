[English](SqTypeRow.md)

# SqTypeRow

SqTypeRow 派生自 [SqTypeJoint](SqTypeJoint.cn.md)。它可以创建 [SqRow](SqRow.cn.md) 的实例并解析未知（或已知）的结果、表和列。  
SqTypeRow 示例代码在 [storage-row.cpp](examples/storage-row.cpp)  
注意: SqTypeRow 在 sqxcsupport 库中 (sqxcsupport.h)。  

	SqType
	│
	└─── SqTypeJoint
	     │
	     └─── SqTypeRow

## 无需设置直接使用

SQ_TYPE_ROW 是 SqTypeRow 内置的静态常量类型，用户可以直接用它来解析未知的结果。
在这种情况下，[SqRow](SqRow.cn.md) 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。  
  
使用 C 语言

```c
	SqRow      *row;
	SqPtrArray *array;

	// 指定表类型为 SQ_TYPE_ROW
	row = sq_storage_get(storage, "users", SQ_TYPE_ROW, 11);

	// 指定表类型为 SQ_TYPE_ROW
	// 指定返回数据的容器类型为 SQ_TYPE_PTR_ARRAY
	array = sq_storage_query(storage, query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);
```

使用 C++ 语言

```c++
	Sq::Row      *row;
	Sq::PtrArray *array;

	// 指定表类型为 SQ_TYPE_ROW
	row = (Sq::Row*) storage->get("users", SQ_TYPE_ROW, 11);

	// 指定表类型为 SQ_TYPE_ROW
	// 指定返回数据的容器类型为 SQ_TYPE_PTR_ARRAY
	array = (Sq::PtrArray*) storage->query(query, SQ_TYPE_ROW, SQ_TYPE_PTR_ARRAY);
```

## 创建和设置

如果你想从 [SqRow](SqRow.cn.md) 获得正确的数据类型，你必须创建动态 SqTypeRow 并设置它。
因为 SqTypeRow 的解析器必须知道表中列的类型，所以用户必须在 SqTypeRow 中添加、删除、清除表。  
添加或删除表时，您可以在最后一个参数中指定表别名（由 SQL AS 关键字使用）。如果别名为 NULL，Sqxc 将直接使用表名。  
  
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

您可以使用 SqTypeRow 替换 [SqStorage](SqStorage.cn.md) 中的默认联合类型:

```c++
	Sq::TypeRow   *typeRow     = new Sq::TypeRow();
	Sq::TypeJoint *typeDefault = storage->joint_default;
	storage->joint_default = typeRow;
	delete typeDefault;
```

**当 SqTypeRow 为默认联合类型时，在以下情况下 query() 结果的元素类型为 SqRow：**
1. 查询已经加入了多表。
2. 查询有未知表或未知结果。

例：构建查询并获得结果：  
在查询中的第一个表是 cities，第二个表是 users。

```sql
SELECT * FROM cities JOIN users ON cities.id = users.city_id
```

使用 C++ 语言

```c++
	std::vector<Sq::Row> *vector;

	query->from("cities")->join("users", "cities.id", "=", "users.city_id");

	vector = storage->query<std::vector<Sq::Row>>(query);

	for (unsigned int index = 0;  index < vector->size();  index++) {
		// 从 std::vector 中获取 Sq::Row
		Sq::Row &row = vector->at(index);
		// 处理 Sq::Row 中的列
		for (unsigned int nth = 0;  nth < row.length;  nth++) {
			std::cout << row.cols[nth].name << " = ";
			// 列值类型由 'row.cols[nth].type' 决定
			if (row.cols[nth].type == SQ_TYPE_INT)
				std::cout << row.data[nth].int_ << std::endl;
			if (row.cols[nth].type == SQ_TYPE_STR)
				std::cout << row.data[nth].str  << std::endl;
			// 其他类型...
		}
	}
```

**如果您不想更改默认联合类型：**
1. 调用 sq_storage_setup_query() 来设置 SqQuery 和 SqTypeRow。
2. 调用 sq_storage_query() 时指定使用 SqTypeRow。
3. 如果您跳过第 1 步，则 [SqRow](SqRow.cn.md) 中的所有数据类型都是 C 字符串，因为 SqTypeRow 不知道列的类型。

函数 sq_storage_setup_query() 声明：

```c++
// C 函数
SqType* sq_storage_setup_query(SqStorage *storage, SqQuery *query, SqTypeJoint *typeJoint);

// C++ 方法
Sq::Type *StorageMethod::setupQuery(Sq::QueryMethod &query, Sq::TypeJointMethod *typeJoint);
```

它设置查询和 'typeJoint' 然后返回 SqType 以调用 sq_storage_query()。  
如果查询已加入多表，它将在查询中添加 "SELECT table.column AS 'table.column'"。  

| 返回值       | 描述                                                                       |
| ------------ | ---------------------------------------------------------------------------|
| NULL         | 如果找不到表 并且 'typeJoint' 不能解析未知的表类型。                       |
| 'typeJoint'  | 如果查询已加入多表。它将设置 'typeJoint' 和查询。                          |
| 其他表类型   | 如果查询只有 1个表。它将设置 'typeJoint' 但保持查询不变。在这种情况下，用户可以使用返回的类型或'typeJoint' 调用 sq_storage_query()。 |

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

## 输出 SqRow 到 Sqxc 链

SqTypeRow 派生自 SqType，所以用户可以调用 SqType::write() 将 SqRow 实例输出到 Sqxc 链。

```c
	typeRow->write(row, typeRow, sqxcJson);
```

用户也可以直接调用 sq_type_row_write() 来做同样的事情。  
SqRow 实例包含足够的信息来输出到 Sqxc 链，因此可以使用或不使用 SqTypeRow 来调用 sq_type_row_write()。

```c
	// 使用   SqTypeRow 调用 sq_type_row_write()
	sq_type_row_write(row, typeRow, sqxcJson);

	// 不使用 SqTypeRow 调用 sq_type_row_write()
	sq_type_row_write(row, NULL,    sqxcJson);
```
