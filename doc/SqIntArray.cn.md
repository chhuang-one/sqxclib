[English](SqIntArray.md)

# SqIntArray

SqIntArray 是整数数组。它是 [SqArray](SqArray.cn.md) 的派生类。  
SQ_TYPE_INT_ARRAY 是一个内置的静态常量类型，可以创建 SqIntArray 实例。

	SqArray
	│
	└─── SqIntArray

## 数据成员

与 [SqArray](SqArray.cn.md) 一样，SqIntArray 在数组前面也有相同的隐藏成员。  
  
SqIntArray 结构定义:

```c
struct SqIntArray
{
	int      *data;
	int       length;
};
```

## 函数和方法

用户可以使用与 [SqArray](SqArray.cn.md) 相同的函数和方法。  
SqIntArray 仍然为 C 语言定义了一些 sq_int_array_xxx() 宏和函数，对应 sq_array_xxx() 系列。  
  
**初始化 Initialize**

```c
	sq_int_array_init(array, capacity);
	sq_int_array_final(array);
```

**分配 Allocate**

```c
	int  *memory;

	memory = sq_int_array_alloc(array, count);
	memory = sq_int_array_alloc_at(array, index, count);
```

**添加 Append**

```c
	int   ints[3] = {0};

	sq_int_array_push(array, 16384);
	sq_int_array_append(array, ints, sizeof(ints) / sizeof(int));
```

**插入 Insert**

```c
	int   index = 5;

	sq_int_array_push_to(array, index, 16384);
	sq_int_array_insert(array, index, ints, sizeof(ints) / sizeof(int));
```

**排序 Sort**

```c
	int  compareFunc(const int *int1, const int *int2);

	sq_int_array_sort(array, (SqCompareFunc)compareFunc);
```

**查找 Find**

```c
	int  *key;
	int  *element;
	int   insertingIndex;

	element = sq_int_array_find(array, key, compareFunc);
	element = sq_int_array_find_sorted(array, key, compareFunc, &insertingIndex);
```

如果使用 C++ 语言，可以直接使用 Sq::Array<int>。Sq::IntArray 仍然被定义，它是 Sq::Array<int> 的别名。
