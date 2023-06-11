[English](SqPairs.md)

# SqPairs

SqPairs 是键值对数组。因为此库需要内置小而简单的键值对所以它只提供简单的功能。  
在使用上有一些限制：
1. 它不检查数组中是否有重复键。
2. 键值对中的数据类型只支持指针或整数（仅限 intptr_t 或 uintptr_t）。

## 初始化

初始化时必须指定比较函数。如果需要可以指定以下两个成员：  
key_destroy_func   : 销毁键值对中键的函数  
value_destroy_func : 销毁键值对中值的函数  
  
例如 初始化键类型为整数（intptr_t），值类型为字符串（char*）的 SqPairs。  
  
使用 C 语言

```c
	// 比较函数
	int  compareFunc(const int *int1, const int *int2);

	SqPairs     pairsIntStr;

	// 键类型为整数（intptr_t），值类型为字符串（char*）
	sq_pairs_init(&pairsIntStr, (SqCompareFunc)compareFunc);

	// 当键值对中的值被销毁时调用 free()
	pairsIntStr.value_destroy_func = free;

	// 终结 SqPairs
	// 它将调用 pairsIntStr.value_destroy_func 以释放键值对中的值。
	sq_pairs_final(&pairsIntStr);
```

使用 C++ 语言

```c++
	// 键类型为整数（intptr_t），值类型为字符串（char*）
	Sq::Pairs<intptr_t, char*>   pairsIntStr((SqCompareFunc)compareFunc);

	// 当键值对中的值被销毁时调用 free()
	pairsIntStr.value_destroy_func = free;

	// 当 SqPairs 的生命周期结束时，
	// 它会调用 pairsIntStr.value_destroy_func 以释放键值对中的值。
```

## 添加 Add

它将键值对添加到数组，但不检查数组中是否有重复键。  
  
使用 C 语言  
  
SqPairs 有函数 sq_pairs_add() 和下面 3 个方便的宏来添加键值对。  
sq_pairs_add_int()  : 键和值类型是整数。  
sq_pairs_add_intx() : 键类型是整数，值类型不是整数。  
sq_pairs_add_xint() : 键类型不是整数，值类型是整数。  

```c
	// 直接使用 sq_pairs_add() 函数
	sq_pairs_add(&pairsIntStr, (void*)(intptr_t)1354, strdup("value1354"));

	// 使用方便的宏 ( 键类型是整数，值类型不是整数 )
	sq_pairs_add_intx(&pairsIntStr, 1354, strdup("value1354"));
```

使用 C++ 语言  
  
Sq::Pairs 使用模板函数添加键值对。

```c++
	// 键类型是整数，值类型不是整数
	pairsIntStr.add(1354, strdup("value1354"));
```

## 获取 Get

用户必须指定键才能获取键值对中的值。 get() 将直接返回键值对中的值。  
如果 get() 未在数组中找到键：
1. 此方法将返回 NULL 或 0 。如果您从不添加 NULL 或 0 ，则可以使用它。
2. C 函数 sq_pairs_is_found()，C++ 方法 isFound() 将返回 false 。
  
使用 C 语言  
  
SqPairs 有函数 sq_pairs_get() 和下面 3 个方便的宏来获取键值对中的值。  
sq_pairs_get_int()  : 键和值类型是整数。  
sq_pairs_get_intx() : 键类型是整数，值类型不是整数。  
sq_pairs_get_xint() : 键类型不是整数，值类型是整数。  

```c
	char *valueStr;

	// 直接使用 sq_pairs_get() 函数
	valueStr = sq_pairs_get(&pairsIntStr, (void*)(intptr_t)1354);

	// 1. 检查返回值为 NULL 或 0 ( 如果您从不添加 NULL 或 0 )
	if (valueStr == NULL)
		puts("not found.");

	// 使用方便的宏 ( 键类型是整数，值类型不是整数 )
	valueStr = sq_pairs_get_intx(&pairsIntStr, 1354);

	// 2. 使用 sq_pairs_is_found() 检查是否找到键值对
	if (sq_pairs_is_found(&pairsIntStr) == false)
		puts("not found.");
```

使用 C++ 语言  
  
Sq::Pairs 使用模板函数获取键值对中的值。

```c++
	// 键类型是整数，值类型不是整数
	valueStr = pairsIntStr.get(1354);

	// 2. 使用 isFound() 检查是否找到键值对
	if (pairsIntStr.isFound() == false)
		std::cout << "not found." << std::endl;
```

## 删除 Erase / 窃取 Steal

erase() 通过调用销毁函数从数组中删除元素。  
steal() 在不调用销毁函数的情况下从数组中删除元素。  
  
使用 C 语言  
  
和 sq_pairs_get() 一样，sq_pairs_steal() 和 sq_pairs_erase() 也有方便的宏：  
sq_pairs_steal_int() : 键类型是整数。  
sq_pairs_erase_int() : 键类型是整数。  

```c
	// 直接使用 sq_pairs_steal() 函数
	sq_pairs_steal(&pairsIntStr, (void*)(intptr_t)1354);

	// 使用方便的宏 ( 键类型是整数 )
	sq_pairs_steal_int(&pairsIntStr, 1354);


	// 直接使用 sq_pairs_erase() 函数
	sq_pairs_erase(&pairsIntStr, (void*)(intptr_t)1354);

	// 使用方便的宏 ( 键类型是整数 )
	sq_pairs_erase_int(&pairsIntStr, 1354);
```

使用 C++ 语言
  
Sq::Pairs 使用模板函数 steal() 和 erase()。

```c++
	// 键类型是整数
	pairsIntStr.steal(1354);

	pairsIntStr.erase(1354);
```

## 排序 Sort

sort() 函数可以对数组中的键值对进行排序。它使用初始化时指定的比较函数。  
在大多数情况下，用户不需要调用此函数，因为如果数组未排序，SqPairs 会在调用 sq_pairs_get() 时自动排序。  
  
使用 C 语言

```c
	sq_pairs_sort(&pairsIntStr);
```

使用 C++ 语言

```c++
	pairsIntStr.sort();
```

如果在 C++ 将 intptr_t 类型（或 C 字符串类型）指定为 Sq::Pairs 模板的键类型，它可以使用默认或自定义比较函数。

```c++
	// 键类型为整数（intptr_t），值类型为字符串（char*）。
	Sq::Pairs<intptr_t, char*>  pairsIntStr;

	// 键类型为字符串（char*），值类型为整数（intptr_t）。
	Sq::Pairs<char*, intptr_t>  pairsStrInt;
```
