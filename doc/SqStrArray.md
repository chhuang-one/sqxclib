[中文](SqStrArray.cn.md)

# SqStrArray

SqStrArray is C string array. It is derived class of [SqPtrArray](SqPtrArray.md).  
SQ_TYPE_STR_ARRAY is a built-in static constant type that can create SqStrArray instances.

	SqArray
	│
	└─── SqPtrArray
	     │
	     └─── SqStrArray

## Data members

Like [SqPtrArray](SqPtrArray.md), SqStrArray has the same hidden members in front of the array.  
  
SqStrArray Structure Definition:

```c
struct SqStrArray
{
	char    **data;
	int       length;
};
```

## Append / Insert

SqStrArray (C++ Sq::StrArray) will copy string when appending and inserting, which is different from [SqPtrArray](SqPtrArray.md).  
If you don't want to copy string, please use [SqArray](SqArray.md) or [SqPtrArray](SqPtrArray.md).  
  
use C language

```c
	SqStrArray  array;
	const char *strs[] = {"One", "Two", "Three"};
	int         n_strs = sizeof(strs) / sizeof(char*);

	sq_str_array_init(&array, 0);

	// append multiple strings
	sq_str_array_append(&array, strs, n_strs);
	// append a string
	sq_str_array_push(&array, "Five");

	// insert a string to index 3 of array
	sq_str_array_push_to(&array, 3, "Four");
	// insert multiple strings to index 1 of array
	sq_str_array_insert(&array, 1, strs, n_strs);
```

use C++ language

```c++
	Sq::StrArray  array;

	// append multiple strings
	array.append(strs, n_strs);
	// append a string
	array.append("Five");

	// insert a string to index 3 of array
	array.insert(3, "Four");
	// insert multiple strings to index 1 of array
	array.insert(1, strs, n_strs);
```

## Sort

use C language

```c
	int  compareFunc(const char **str1, const char **str2);

	sq_str_array_sort(array, (SqCompareFunc)compareFunc);
```

use C++ language  
  
In C++, it will generate static compare function for C string.

```c++
	array->sort((SqCompareFunc)compareFunc);

	// use generated static compare function.
	array->sort();
```

## Find

use C language

```c
	char **key;
	char **element;
	int    insertingIndex;

	// use string pointer as key value
	element = sq_str_array_find(array, key, compareFunc);
	element = sq_str_array_find_sorted(array, key, compareFunc, &insertingIndex);
```

use C++ language  
  
When user sort/find string in C++, it can directly use string as key value and generate static comparison function.

```c++
	// use string pointer as key value
	element = array->find(key, compareFunc);
	element = array->findSorted(key, compareFunc, &insertingIndex);

	// use string as key value directly
	element = array->find("key-string", compareFunc);
	element = array->findSorted("key-string", compareFunc, &insertingIndex);

	// use generated static compare function and use string as key value directly
	element = array->find("key-string");
	element = array->findSorted("key-string", &insertingIndex);
```

## Other functions and methods

User can use the same functions and methods as [SqPtrArray](SqPtrArray.md).  
SqStrArray still defines some sq_str_array_xxx() macros and functions for C language, which are correspond to sq_ptr_array_xxx() series.  
  
**Initialize**

```c
	sq_str_array_init(array, capacity);
	sq_str_array_final(array);
```

**Allocate**

```c
	char **memory;

	memory = sq_str_array_alloc(array, count);
	memory = sq_str_array_alloc_at(array, index, count);
```

**Erase**

```c
	sq_str_array_erase(array, index, count);
	sq_str_array_steal(array, index, count);
```
