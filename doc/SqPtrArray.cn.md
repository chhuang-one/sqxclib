[English](SqPtrArray.md)

# SqPtrArray

SqPtrArray 是指针数组。它是 [SqArray](SqArray.cn.md) 的派生类。  
SQ_TYPE_PTR_ARRAY 是内置的静态常量类型，可以创建 SqPtrArray 实例。

	SqArray
	│
	└─── SqPtrArray
	     │
	     └─── SqStrArray

[SqArray](SqArray.cn.md) 及其派生类共享数据结构和代码以减少二进制大小。

## 数据成员

与 [SqArray](SqArray.cn.md) 一样，SqPtrArray 在数组前面也有相同的隐藏成员。  
  
SqPtrArray 结构定义：

```c
struct SqPtrArray
{
	void         **data;
	unsigned int   length;
};
```

SqPtrArray::data 前面有隐藏成员：  
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

## 删除 Erase / 窃取 Steal

SqPtrArray 增加了 erase() 函数，这一点不同于 [SqArray](SqArray.cn.md)。  
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

## 其他函数和方法

用户可以使用与 [SqArray](SqArray.cn.md) 相同的函数和方法。  
SqPtrArray 仍然为 C 语言定义了一些 sq_ptr_array_xxx() 宏和函数，对应 sq_array_xxx() 系列。  
  
**分配 Allocate**

```c
	void **memory;

	memory = sq_ptr_array_alloc(array, count);
	memory = sq_ptr_array_alloc_at(array, index, count);
```

**添加 Append**

```c
	void  *ptrs[3] = {NULL};

	sq_ptr_array_push(array, NULL);
	sq_ptr_array_append(array, ptrs, sizeof(ptrs) / sizeof(void*));
```

**插入 Insert**

```c
	unsigned int  index = 5;

	sq_ptr_array_push_to(array, index, NULL);
	sq_ptr_array_insert(array, index, ptrs, sizeof(ptrs) / sizeof(void*));
```

**排序 Sort**

```c
	int  compareFunc(const void **ptr1, const void **ptr2);

	sq_ptr_array_sort(array, (SqCompareFunc)compareFunc);
```

**查找 Find**

```c
	void **key;
	void **element;
	unsigned int  insertingIndex;

	element = sq_ptr_array_find(array, key, compareFunc);
	element = sq_ptr_array_find_sorted(array, key, compareFunc, &insertingIndex);
```
