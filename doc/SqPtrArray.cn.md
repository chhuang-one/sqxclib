[English](SqPtrArray.md)

# SqPtrArray

SqPtrArray 是指针数组。因为有些结构（例如 SqType）需要从静态转换为动态，所以它的数据成员必须与这些结构兼容。  
SQ_TYPE_PTR_ARRAY 是内置的静态常量类型，可以创建 SqPtrArray 实例。

	SqPtrArray
	│
	├─── SqIntptrArray
	│
	└─── SqStringArray

[SqIntptrArray](SqIntptrArray.cn.md) 和 SqStringArray 是 SqPtrArray 的派生类。它们共享数据结构和代码以减少二进制大小。  
[SqIntptrArray](SqIntptrArray.cn.md) 是整数数组，SqStringArray 是字符串数组。

## 数据成员

SqPtrArray 小而简单。可以直接在栈内存中使用。它看起来只有 2 个成员，但实际上其他成员都隐藏在数组的前面。
  
SqPtrArray 结构定义：

```c
struct SqPtrArray
{
	void    **data;
	int       length;
};
```

SqPtrArray.data 前面有 3 个隐藏成员：  
索引 -1 是数组前面的元素数。（目前这个值为3）  
索引 -2 是数组中实际分配的元素个数。（不包括数组前面的元素）  
索引 -3 是数组元素的销毁函数。  
  
下面是访问这些隐藏成员的代码：

```c++
	SqPtrArray    *array;

	intptr_t       headerLength;
	intptr_t       capacity;
	SqDestroyFunc  destroyFunc;


	headerLength = (intptr_t) array->data[-1];
	capacity     = (intptr_t) array->data[-2];
	destroyFunc  = (SqDestroyFunc) array->data[-3];
```

有 C 函数和 C++ 方法可以访问这些隐藏成员。

```c++
	// C 函数
	headerLength = sq_ptr_array_header_length(array);
	capacity     = sq_ptr_array_allocated(array);
	destroyFunc  = sq_ptr_array_destroy_func(array);

	// C++ 方法
	headerLength = array->headerLength();
	capacity     = array->allocated();
	destroyFunc  = array->destroyFunc();
```

## 初始化

C 函数 sq_ptr_array_init()，C++ 方法 init() 可以初始化 SqPtrArray 的实例。  
  
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

#### 初始化派生类

sq_ptr_array_init_full() 可以指定数组前面的元素个数。如果你定义一个 SqPtrArray 的派生类，并向它添加更多的隐藏成员，你可以调用这个函数并增加 'headerLength' 的值。

```c
	sq_ptr_array_init_full(array, capacity, headerLength, func);
```

## 分配 Allocate

C 函数 sq_ptr_array_alloc()，C++ 方法 alloc() 可以从数组尾部开始分配元素。 如果没有足够的空间，SqPtrArray 将扩展数组。  
  
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

	elements = sq_ptr_array_alloc(array, length);

	// 从指定索引分配元素
	elements = sq_ptr_array_alloc_at(array, index, length);
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

erase() 通过调用 destroy 函数从数组中删除元素。  
steal() 在不调用 destroy 函数的情况下从数组中删除元素。  
  
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
