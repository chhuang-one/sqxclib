[English](SqPtrArray.md)

# SqPtrArray

SqPtrArray 是指针数组。它是 [SqArray](SqArray.cn.md) 的派生類。  
SQ_TYPE_PTR_ARRAY 是内置的静态常量类型，可以创建 SqPtrArray 实例。

	SqArray
	|
	└─── SqPtrArray
	     │
	     ├─── SqStrArray
	     │
	     └─── SqIntptrArray (deprecated)

[SqArray](SqArray.cn.md) 及其派生類共享數據結構和代碼以減少二進制大小。

## 数据成员

与 [SqArray](SqArray.cn.md) 一样，SqPtrArray 在数组前面也有相同的隐藏成员。  
  
SqPtrArray 结构定义：

```c
struct SqPtrArray
{
	void    **data;
	int       length;
};
```

SqPtrArray.data 前面有隐藏成员：  
capacity  是数组中实际分配的元素个数。（不包括数组前面的表头）  
clearFunc 是数组中清除元素的函数。  
  
下面是访问这些隐藏成员的代码：

```c++
	// C 函数
	capacity   = sq_ptr_array_capacity(array);
	clearFunc  = sq_ptr_array_clear_func(array);

	// C++ 方法
	capacity   = array->capacity();
	clearFunc  = array->clearFunc();
```

## 初始化

C 函数 sq_ptr_array_init()，C++ 构造函数可以初始化 SqPtrArray 的实例。  
  
使用 C 语言

```c
	// 如果你不需要释放元素的内存，'func' 可以是 NULL。
	SqDestroyFunc  func = free;

	SqPtrArray     arrayLocal;
	sq_ptr_array_init(&arrayLocal, capacity, func);
	sq_ptr_array_final(&arrayLocal);

	SqPtrArray    *array;
	array = sq_ptr_array_new(capacity, func);
	sq_ptr_array_free(array);
```

使用 C++ 语言

```c++
	// 如果你不需要释放元素的内存，'func' 可以是 NULL。
	SqDestroyFunc  func = free;

	// Sq::PtrArray 有构造函数和析构函数
	Sq::PtrArray   arrayLocal;

	Sq::PtrArray  *array;
	array = new Sq::PtrArray(capacity, func);
	delete array;
```

## 分配 Allocate

C 函数 sq_ptr_array_alloc()，C++ 方法 alloc() 可以从数组尾部开始分配元素。  
C 函数 sq_ptr_array_alloc_at()，C++ 重载方法 alloc() 可以从数组的指定索引开始分配元素。  
如果没有足够的空间，SqPtrArray 将扩展数组。  
  
使用 C 语言

```c
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = sq_ptr_array_alloc(array, length);

	// 从指定索引分配元素
	elements = sq_ptr_array_alloc_at(array, index, length);
```

使用 C++ 语言

```c++
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = array->alloc(length);

	// 从指定索引分配元素
	elements = array->alloc(index, length);
```

## 添加 Append

添加操作将首先调用 sq_ptr_array_alloc() 然后将元素复制到 SqPtrArray。  
  
使用 C 语言

```c
	// 添加一个元素
	sq_ptr_array_append(array, element);

	// 添加多个元素
	sq_ptr_array_append_n(array, elements, n_elements);
```

使用 C++ 语言

```c++
	// 添加一个元素
	array->append(element);

	// 添加多个元素
	array->append(elements, n_elements);
```

## 插入 Insert

插入动作会先调用 sq_ptr_array_alloc_at() 在指定索引分配元素，然后将数据复制到 SqPtrArray 中的指定索引。  
  
使用 C 语言

```c
	// 插入一个元素
	sq_ptr_array_insert(array, index, elements);

	// 插入多个元素
	sq_ptr_array_insert_n(array, index, elements, n_elements);
```

使用 C++ 语言

```c++
	// 插入一个元素
	array->insert(index, elements);

	// 插入多个元素
	array->insert(index, elements, n_elements);
```

## 删除 Erase / 窃取 Steal

erase() 通过调用清除函数从数组中删除元素。  
steal() 在不调用清除函数的情况下从数组中删除元素。  
  
使用 C 语言

```c
	// 删除元素
	sq_ptr_array_erase(array, index, n_elements);

	// 窃取元素
	sq_ptr_array_steal(array, index, n_elements);
```

使用 C++ 语言

```c++
	// 删除元素
	array->erase(index, n_elements);

	// 窃取元素
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

	sq_ptr_array_sort(array, straddr_compare);

	sq_ptr_array_find_sorted(array, &key, straddr_compare, &inserted_index);
```

使用 C++ 语言

```c++
	array->sort(straddr_compare);

	array->findSorted(&key, straddr_compare, &inserted_index);
```
