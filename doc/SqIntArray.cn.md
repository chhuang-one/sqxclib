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
  
SqIntArray 仍然定义了一些 sq_int_array_xxx() 宏，它们是 sq_array_xxx() 系列的别名。  
  
使用 C 语言

```c
	SqIntArray  array;
	int         iarray[] = {0, 3, 2, 5};
	int         len = sizeof(iarray) / sizeof(int);

	sq_array_init(&array, sizeof(int), capacity);
	SQ_ARRAY_APPEND(&array, int, iarray, len);
```

如果使用 C++ 语言，可以直接使用 Sq::Array<int>。Sq::IntArray 仍然被定义，它是 Sq::Array<int> 的别名。
