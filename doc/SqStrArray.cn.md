[English](SqStrArray.md)

# SqStrArray

SqStrArray 是 [SqPtrArray](SqPtrArray.cn.md) 的派生类。  
SQ_TYPE_STR_ARRAY 是一个内置的静态常量类型，可以创建 SqStrArray 实例。

	SqPtrArray
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

## 函数和方法

用户可以使用与 [SqPtrArray](SqPtrArray.cn.md) 相同的函数和方法。  
  
SqStrArray 在追加和插入时会复制源数据，这一点与 [SqPtrArray](SqPtrArray.cn.md) 不同。

```c
	SqStrArray  array;
	const char *strs[] = {"One", "Two", "Three"};
	int         n_strs = sizeof(strs) / sizeof(char*);

	sq_str_array_init(&array, 0);

	sq_str_array_append_n(&array, strs, n_strs);

	// 將 'strs' 插入到 'array' 的索引 1
	sq_str_array_insert_n(&array, 1, strs, n_strs);
```
