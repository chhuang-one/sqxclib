[English](SqStrArray.md)

# SqStrArray

SqStrArray 是 C 字符串数组。它是 [SqPtrArray](SqPtrArray.cn.md) 的派生类。  
SQ_TYPE_STR_ARRAY 是一个内置的静态常量类型，可以创建 SqStrArray 实例。

	SqArray
	│
	└─── SqPtrArray
	     │
	     └─── SqStrArray

## 数据成员

与 [SqPtrArray](SqPtrArray.cn.md) 一样，SqStrArray 在数组前面也有相同的隐藏成员。  
  
SqStrArray 结构定义:

```c
struct SqStrArray
{
	char    **data;
	int       length;
};
```

## 添加 Append / 插入 Insert

SqStrArray (C++ Sq::StrArray) 在添加和插入时会复制字符串，这一点与 [SqPtrArray](SqPtrArray.cn.md) 不同。  
如果您不想复制字符串，请使用 [SqArray](SqArray.cn.md)。  
  
使用 C 语言

```c
	SqStrArray  array;
	const char *strs[] = {"One", "Two", "Three"};
	int         n_strs = sizeof(strs) / sizeof(char*);

	sq_str_array_init(&array, 0);

	// 附加多个字符串
	sq_str_array_append(&array, strs, n_strs);
	// 附加一个字符串
	sq_str_array_push(&array, "Five");

	// 將一个字符串插入數組的索引 3
	sq_str_array_push_to(&array, 3, "Four");
	// 将多个字符串插入数组的索引 1
	sq_str_array_insert(&array, 1, strs, n_strs);
```

使用 C++ 语言

```c++
	Sq::StrArray  array;

	// 附加多个字符串
	array.append(strs, n_strs);
	// 附加一个字符串
	array.append("Five");

	// 將一个字符串插入數組的索引 3
	array.insert(3, "Four");
	// 将多个字符串插入数组的索引 1
	array.insert(1, strs, n_strs);
```

## 其他函数和方法

用户可以使用与 [SqPtrArray](SqPtrArray.cn.md) 相同的函数和方法。  
SqStrArray 仍然为 C 语言定义了一些 sq_str_array_xxx() 宏和函数，对应 sq_ptr_array_xxx() 系列。  
  
**初始化 Initialize**

```c
	sq_str_array_init(array, capacity);
	sq_str_array_final(array);
```

**分配 Allocate**

```c
	char **memory;

	memory = sq_str_array_alloc(array, count);
	memory = sq_str_array_alloc_at(array, index, count);
```

**排序 Sort**

```c
	int  compareFunc(const char **str1, const char **str2);

	sq_str_array_sort(array, (SqCompareFunc)compareFunc);
```

**查找 Find**

```c
	char **key;
	char **element;
	int    insertingIndex;

	element = sq_str_array_find(array, key, compareFunc);
	element = sq_str_array_find_sorted(array, key, compareFunc, &insertingIndex);
```

**删除 Erase**

```c
	sq_str_array_erase(array, index, count);
	sq_str_array_steal(array, index, count);
```
