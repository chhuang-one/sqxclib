[中文](SqPairs.cn.md)

# SqPairs

SqPairs is array of key-value pairs. It only provides simple functions because this library need a build-in small and simple key-value pairs.  
There are some limitations in use:
1. It doesn't check for duplicate keys in the array.
2. Data types in key-value pair only support pointer or integer (only intptr_t or uintptr_t).

## Initialize

A comparison function must be specified during initialization. The following two members can be specified if necessary:  
key_destroy_func   : destroy function of key in key-value pairs  
value_destroy_func : destroy function of value in key-value pairs  
  
e.g. To initialize SqPairs that key type is integer (intptr_t), value type is string (char*).
  
use C language

```c
	// comparison function
	int  compareFunc(const int *int1, const int *int2);

	SqPairs     pairsIntStr;

	// key type is integer (intptr_t), value type is string (char*)
	sq_pairs_init(&pairsIntStr, (SqCompareFunc)compareFunc);

	// call free() when value in key-value pairs is destroying
	pairsIntStr.value_destroy_func = free;

	// finalize SqPairs
	// It will call pairsIntStr.value_destroy_func to free value in key-value pairs.
	sq_pairs_final(&pairsIntStr);
```

use C++ language

```c++
	// key type is integer (intptr_t), value type is string (char*)
	Sq::Pairs<intptr_t, char*>   pairsIntStr((SqCompareFunc)compareFunc);

	// call free() when value in key-value pairs is destroying
	pairsIntStr.value_destroy_func = free;

	// When life cycle of SqPairs ends,
	// it will call pairsIntStr.value_destroy_func to free value in key-value pairs.
```

## Add

It add key-value pair to array, but doesn't check for duplicate keys in the array.  
  
use C language  
  
SqPairs has function sq_pairs_add() and below 3 convenient macro to add key-value pair.  
sq_pairs_add_int()  : key and value types are integer.  
sq_pairs_add_intx() : key type is integer, value type is not integer.  
sq_pairs_add_xint() : key type is not integer, value type is integer.  

```c
	// directly use sq_pairs_add() function
	sq_pairs_add(&pairsIntStr, (void*)(intptr_t)1354, strdup("value1354"));

	// use convenient macro ( key type is integer, value type is not integer )
	sq_pairs_add_intx(&pairsIntStr, 1354, strdup("value1354"));
```

use C++ language  
  
Sq::Pairs use template functions to add key-value pair.

```c++
	// key type is integer, value type is not integer
	pairsIntStr.add(1354, strdup("value1354"));
```

## Get

The user must specify the key to get the value in the key-value pair. get() will directly returns value in key-value pair.  
If get() not found the key in the array:
1. This method will return NULL or 0. You can use it if you never add NULL or 0.
2. C function sq_pairs_is_found(), C++ method isFound() will return false.
  
use C language  
  
SqPairs has function sq_pairs_get() and below 3 convenient macro to get value in key-value pair.  
sq_pairs_get_int()  : key and value types are integer.  
sq_pairs_get_intx() : key type is integer, value type is not integer.  
sq_pairs_get_xint() : key type is not integer, value type is integer.  

```c
	char *valueStr;

	// directly use sq_pairs_get() function
	valueStr = sq_pairs_get(&pairsIntStr, (void*)(intptr_t)1354);

	// 1. check return value is NULL or 0. ( if you never add NULL or 0 )
	if (valueStr == NULL)
		puts("not found.");

	// use convenient macro ( key type is integer, value type is not integer )
	valueStr = sq_pairs_get_intx(&pairsIntStr, 1354);

	// 2. use sq_pairs_is_found() to check if a key-value pair is found
	if (sq_pairs_is_found(&pairsIntStr) == false)
		puts("not found.");
```

use C++ language  
  
Sq::Pairs use template functions to get the value in the key-value pair.

```c++
	// key type is integer, value type is not integer
	valueStr = pairsIntStr.get(1354);

	// 2. use isFound() to check if a key-value pair is found
	if (pairsIntStr.isFound() == false)
		std::cout << "not found." << std::endl;
```

## Erase / Steal

erase() removes elements from array with calling the destroy function.  
steal() removes elements from array without calling the destroy function.  
They will return true if the key was found and removed from array.  
  
use C language  
  
Like sq_pairs_get(), sq_pairs_steal() and sq_pairs_erase() also has convenient macro:  
sq_pairs_steal_int() : key type is integer.  
sq_pairs_erase_int() : key type is integer.  

```c
	// directly use sq_pairs_steal() function
	sq_pairs_steal(&pairsIntStr, (void*)(intptr_t)1354);

	// use convenient macro ( key type is integer )
	sq_pairs_steal_int(&pairsIntStr, 1354);


	// directly use sq_pairs_erase() function
	sq_pairs_erase(&pairsIntStr, (void*)(intptr_t)1354);

	// use convenient macro ( key type is integer )
	sq_pairs_erase_int(&pairsIntStr, 1354);
```

use C++ language  
  
Sq::Pairs use template functions steal() and erase().

```c++
	// key type is integer
	pairsIntStr.steal(1354);

	pairsIntStr.erase(1354);
```

## Sort

sort() function can sort key-value pairs in array. It uses the comparison function specified at initialization.  
In most case, user do not need to call this function because SqPairs sort automatically when calling sq_pairs_get() if array is unsorted.  
  
use C language

```c
	sq_pairs_sort(&pairsIntStr);
```

use C++ language

```c++
	pairsIntStr.sort();
```

If you specify intptr_t type (or C string type) as key type to Sq::Pairs template in C++, it can use default or custom comparison function.

```c++
	// key type is integer (intptr_t), value type is string (char*).
	Sq::Pairs<intptr_t, char*>  pairsIntStr;

	// key type is string (char*), value type is integer (intptr_t).
	Sq::Pairs<char*, intptr_t>  pairsStrInt;
```
