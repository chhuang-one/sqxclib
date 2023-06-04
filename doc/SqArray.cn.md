[English](SqArray.md)

# SqArray

SqArray 是任意元素的数组。因为有些结构（例如 SqType）需要从静态转换为动态，所以它的数据成员必须与这些结构兼容。  
SQ_TYPE_ARRAY 是内置的静态常量类型，可以创建 SqArray 实例。

	SqArray
	│
	├─── SqIntArray
	│
	└─── SqPtrArray
	     │
	     ├─── SqStrArray
	     │
	     └─── SqIntptrArray (deprecated)

[SqIntArray](SqIntArray.cn.md) 和 [SqPtrArray](SqPtrArray.cn.md) 是 SqArray 的派生类。它们共享数据结构和代码以减少二进制大小。  
[SqIntArray](SqIntArray.cn.md) 是整数数组，[SqPtrArray](SqPtrArray.cn.md) 是指针数组。

## 数据成员

SqArray 小而简单。可以直接在栈内存中使用。它看起来只有 2 个成员，但实际上其他成员都隐藏在数组的前面。  
  
SqArray 结构定义：

```c
struct SqArray
{
	uint8_t  *data;
	int       length;
};
```

SqArray.data 前面有隐藏成员：  
capacity    是数组中实际分配的元素个数。（不包括数组前面的表头）  
elementSize 是數組中的元素大小。  
  
下面是访问这些隐藏成员的代码：

```c++
	// C 函数
	capacity     = sq_array_capacity(array);
	elementSize  = sq_array_element_size(array);

	// C++ 方法
	capacity     = array->capacity();
	elementSize  = array->elementSize();
```

## 初始化

C 函数 sq_array_init()，C++ 构造函数可以初始化 SqArray 的实例。  
在下面的示例中，ElementType 是 SqArray 的元素類型。  
  
使用 C 语言

```c
	SqArray     arrayLocal;
	sq_array_init(&arrayLocal, sizeof(ElementType), capacity);
	sq_array_final(&arrayLocal);

	SqArray    *array;
	array = sq_array_new(sizeof(ElementType), capacity);
	sq_array_free(array);
```

使用 C++ 语言

```c++
	// Sq::Array 有构造函数和析构函数
	Sq::Array<ElementType>   arrayLocal;

	Sq::Array<ElementType>  *array;
	array = new Sq::Array<ElementType>(capacity);
	delete array;
```

## 访问元素 Access element

返回 SqArray 中指定位置的元素。  
  
使用 C 语言

```c++
	SqArray *array;

	// C 宏 (您必须在此处指定 ElementType)
	element = sq_array_at(array, ElementType, index);

	// C 数据成员 (您必须在此处指定 ElementType)
	element = ((ElementType*)array->data)[index];
```

使用 C++ 语言

```c++
	Sq::Array<ElementType> *array;

	// C++ 方法
	element = array->at(index);

	// C++ 数据成员
	element = array->data[index];
```

## 分配 Allocate

C 函数 sq_array_alloc()，C++ 方法 alloc() 可以从数组后面开始分配元素。  
C 函数 sq_array_alloc_at()，C++ 重载方法 alloc() 可以从数组的指定索引开始分配元素。  
如果没有足够的空间，SqArray 将扩展数组。  
  
使用 C 语言

```c
	ElementType *elements;
	int   count = 16;
	int   index = 8;

	// 从后面分配元素
	elements = (ElementType*)sq_array_alloc(array, count);

	// 从指定索引分配元素
	elements = (ElementType*)sq_array_alloc_at(array, index, count);
```

使用 C++ 语言

```c++
	ElementType *elements;
	int   count = 16;
	int   index = 8;

	// 从后面分配元素
	elements = array->alloc(count);

	// 从指定索引分配元素
	elements = array->allocAt(index, count);
```

## 添加 Append

添加操作将首先调用 sq_array_alloc() 在后面分配元素，然后将元素复制到 SqArray。  
  
使用 C 语言

```c
	ElementType  elementValue;
	ElementType *elements;

	// 如果 ElementType 是算术类型（或指针类型），您可以使用它来
	// 添加一个元素 (您必须在此处指定 ElementType)
	sq_array_push(array, ElementType, elementValue);

	// 添加多个元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_APPEND(array, ElementType, elements, n_elements);
```

使用 C++ 语言

```c++
	// 添加一个元素
	array->append(elementValue);

	// 添加多个元素
	array->append(elements, n_elements);
```

## 插入 Insert

插入动作会先调用 sq_array_alloc_at() 在指定索引分配元素，然后将数据复制到 SqArray 中的指定索引。  
  
使用 C 语言

```c
	ElementType  elementValue;
	ElementType *elements;

	// 如果 ElementType 是算术类型（或指针类型），您可以使用它来
	// 插入一个元素 (您必须在此处指定 ElementType)
	sq_array_push_to(array, ElementType, index, elementValue);

	// 插入多个元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_INSERT(array, ElementType, index, elements, n_elements);
```

使用 C++ 语言

```c++
	// 插入一个元素
	array->insert(index, elementValue);

	// 插入多个元素
	array->insert(index, elements, n_elements);
```

## 删除 Steal

steal() 从数组中删除元素。  
  
使用 C 语言

```c
	// 按索引删除元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_STEAL(array, ElementType, index, n_elements);

	// 按地址删除元素 (您必须在此处指定 ElementType)
	// 警告：请确认元素的地址在当前数组中。
	ElementType *element;
	SQ_ARRAY_STEAL_ADDR(array, ElementType, element, n_elements);
```

使用 C++ 语言

```c++
	// 按索引删除元素
	array->steal(index, n_elements);

	// 按地址删除元素 (您必须在此处指定 ElementType)
	// 警告：请确认元素的地址在当前数组中。
	ElementType *element;
	array->steal(element, n_elements);
```

## 排序 Sort / 查找 Find

sort()       对数组元素进行排序。  
find()       在未排序的数组中查找元素。  
findSorted() 使用二进制搜索在已排序数组中查找元素，如果没有找到则输出插入索引。  
  
以上 3 个函数需要 "比较函数" 才能工作。  
  
使用 C 语言

```c
	// 比較函數
	int   elementCompare(ElementType *element1, ElementType *element2);

	// 您必须在此处指定 ElementType
	SQ_ARRAY_SORT(array, ElementType, elementCompare);

	ElementType *key = pointerToKey;
	ElementType *element;
	int          insertingIndex;
	// 您必须在此处指定 ElementType
	element = SQ_ARRAY_FIND(array, ElementType, key, elementCompare);
	element = SQ_ARRAY_FIND_SORTED(array, ElementType, key, elementCompare, &insertingIndex);
```

使用 C++ 语言

```c++
	array->sort(elementCompare);

	ElementType *key = pointerToKey;
	ElementType *element;
	int          insertingIndex;
	element = array->find(key, elementCompare);
	element = array->findSorted(key, elementCompare, &insertingIndex);
```

如果在 C++ 中为 Sq::Array 模板指定算术类型（或 C 字符串類型），它可以直接使用常量值作为键值并生成静态比较函数。  
在这种情况下，您可以在没有比较函数的情况下调用 sort()、findSorted()...等。

```c++
	Sq::Array<int>  intArray;

	intArray->sort();

	int   key = 31;
	int  *element;
	element = intArray->find(key);
	element = intArray->findSorted(key, &insertingIndex);
```
