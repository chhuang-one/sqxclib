[English](SqIntptrArray.md)

# SqIntptrArray

SqIntptrArray 是 [SqPtrArray](SqPtrArray.cn.md) 的派生类。  
SQ_TYPE_INTPTR_ARRAY 是一个内置的静态常量类型，可以创建 SqIntptrArray 实例。

	SqPtrArray
	│
	├─── SqIntptrArray
	│
	└─── SqStringArray

## 数据成员

与 [SqPtrArray](SqPtrArray.cn.md) 一样，SqIntptrArray 在数组前面也有相同的隐藏成员。  
因为 SqIntptrArray 使用与 SqPtrArray 相同的数据结构和代码，所以数组的数据类型使用 'intptr_t' 来代替 'int'。  
  
SqIntptrArray 结构定义:

```c
struct SqIntptrArray
{
	intptr_t *data;
	int       length;
};
```

## 函数和方法

用户可以使用与 [SqPtrArray](SqPtrArray.cn.md) 相同的函数和方法。  
  
SqIntptrArray 仍然定义了 sq_intptr_array_xxx() 宏，它们是 sq_ptr_array_xxx() 系列的别名。

```c
	SqIntptrArray  array;
	intptr_t       intarray[] = {0, 3, 2, 5};

	sq_intptr_array_init(&array, capacity);
	sq_intptr_array_append_n(&array, intarray, 4);
```
