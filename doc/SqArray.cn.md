[English](SqArray.md)

# SqArray

SqArray 是任意元素的数组。因为有些结构（例如 SqType）需要从静态转换为动态，所以它的数据成员必须与这些结构兼容。  
SQ_TYPE_ARRAY 是内置的静态常量类型，可以创建 SqArray 实例。

	SqArray
	|
	├─── SqIntArray
	|
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
	uint8_t **data;
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

C 函数 sq_array_alloc()，C++ 方法 alloc() 可以从数组尾部开始分配元素。  
C 函数 sq_array_alloc_at()，C++ 重载方法 alloc() 可以从数组的指定索引开始分配元素。  
如果没有足够的空间，SqArray 将扩展数组。  
  
使用 C 语言

```c
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = sq_array_alloc(array, length);

	// 从指定索引分配元素
	elements = sq_array_alloc_at(array, index, length);
```

使用 C++ 语言

```c++
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = array->alloc(length);

	// 从指定索引分配元素
	elements = array->allocAt(index, length);
```

## 添加 Append

添加操作将首先调用 sq_array_alloc() 然后将元素复制到 SqArray。  
  
使用 C 语言

```c
	// 添加一个元素
	*sq_array_alloc(array, 1) = element;

	// 添加多个元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_APPEND(array, ElementType, elements, n_elements);
```

使用 C++ 语言

```c++
	// 添加一个元素
	*array->alloc() = element;

	// 添加多个元素
	array->append(elements, n_elements);
```

## 插入 Insert

插入动作会先调用 sq_array_alloc_at() 在指定索引分配元素，然后将数据复制到 SqArray 中的指定索引。  
  
使用 C 语言

```c
	// 插入一个元素
	*sq_array_alloc_at(array, index, 1) = elements;

	// 插入多个元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_INSERT(array, ElementType, index, elements, n_elements);
```

使用 C++ 语言

```c++
	// 插入一个元素
	*array->allocAt(index) = elements;

	// 插入多个元素
	array->insert(index, elements, n_elements);
```

## 删除 Steal

steal() 从数组中删除元素。  
  
使用 C 语言

```c
	// 删除元素 (您必须在此处指定 ElementType)
	SQ_ARRAY_STEAL(array, ElementType, index, n_elements);
```

使用 C++ 语言

```c++
	// 删除元素
	array->steal(index, n_elements);
```

## 排序 Sort / 查找 Find

sort()、find() 和 findSorted() 需要 "比较函数" 才能工作。  
findSorted() 使用二进制搜索在已排序数组中查找元素，如果没有找到则输出插入索引。  
  
使用 C 语言

```c
	// comparison function 
	int   straddr_compare(const char **straddr1, const char **straddr2);

	int   inserted_index;
	char *key = "str";

	// 您必须在此处指定 ElementType
	SQ_ARRAY_SORT(array, ElementType, straddr_compare);

	char **elementPtr;
	elementPtr = sq_array_find(array, &key, straddr_compare);
	elementPtr = sq_array_find_sorted(array, &key, straddr_compare, &inserted_index);
```

使用 C++ 语言

```c++
	array->sort(straddr_compare);

	char **elementPtr;
	elementPtr = array->find(&key, straddr_compare);
	elementPtr = array->findSorted(&key, straddr_compare, &inserted_index);
```

如果在 C++ 中为 Sq::Array 模板指定算术类型，它会生成静态比较函数。  
在这种情况下，您可以在没有比较函数的情况下调用 sort()、findSorted()...等。

```c++
	Sq::Array<int>  intArray;
	int   key = 31;

	array->sort();

	int  *elementPtr;
	elementPtr = array->find(key);
	elementPtr = array->findSorted(key, &inserted_index);
```
