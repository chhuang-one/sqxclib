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
```
	sq_int_array_init
	sq_int_array_final
```

**分配 Allocate**
```
	sq_int_array_alloc
	sq_int_array_alloc_at
```

**添加 Append**
```
	sq_int_array_push
	sq_int_array_append
```

**插入 Insert**
```
	sq_int_array_push_to
	sq_int_array_insert
```

**排序 Sort**
```
	sq_int_array_sort
```

**查找 Find**
```
	sq_int_array_find
	sq_int_array_find_sorted
```

如果使用 C++ 语言，可以直接使用 Sq::Array<int>。Sq::IntArray 仍然被定义，它是 Sq::Array<int> 的别名。
