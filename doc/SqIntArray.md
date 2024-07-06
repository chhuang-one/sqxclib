[中文](SqIntArray.cn.md)

# SqIntArray

SqIntArray is integer array. It is derived class of [SqArray](SqArray.md).  
SQ_TYPE_INT_ARRAY is a built-in static constant type that can create SqIntArray instances.

	SqArray
	│
	└─── SqIntArray

## Data members

Like [SqArray](SqArray.md), SqIntArray has the same hidden members in front of the array.  
  
SqIntArray Structure Definition:

```c
struct SqIntArray
{
	int          *data;
	unsigned int  length;
};
```

## Sort

use C language

```c
	int  compareFunc(const int *int1, const int *int2);

	sq_int_array_sort(array, (SqCompareFunc)compareFunc);
```

use C++ language  
  
In C++, it will generate default comparison function for integer.

```c++
	array->sort((SqCompareFunc)compareFunc);

	// use default comparison function.
	array->sort();
```

## Find

use C language

```c
	int  *key;
	int  *element;
	unsigned int  insertingIndex;

	// use integer pointer as key value
	element = sq_int_array_find(array, key, compareFunc);
	element = sq_int_array_find_sorted(array, key, compareFunc, &insertingIndex);
```

use C++ language  
  
When user sort/find integer in C++, it can directly use integer as key value and generate default comparison function.

```c++
	// use integer pointer as key value
	element = array->find(key, compareFunc);
	element = array->findSorted(key, compareFunc, &insertingIndex);

	// use integer as key value directly
	element = array->find(155, compareFunc);
	element = array->findSorted(155, compareFunc, &insertingIndex);

	// use default comparison function and use integer as key value directly
	element = array->find(155);
	element = array->findSorted(155, &insertingIndex);
```

## Other Functions and methods

User can use the same functions and methods as [SqArray](SqArray.md).  
SqIntArray still defines some sq_int_array_xxx() macros and functions for C language, which are correspond to sq_array_xxx() series.  
  
**Initialize**

```c
	sq_int_array_init(array, capacity);
	sq_int_array_final(array);
```

**Allocate**

```c
	int  *memory;

	memory = sq_int_array_alloc(array, count);
	memory = sq_int_array_alloc_at(array, index, count);
```

**Append**

```c
	int   ints[3] = {0};

	sq_int_array_push(array, 16384);
	sq_int_array_append(array, ints, sizeof(ints) / sizeof(int));
```

**Insert**

```c
	unsigned int  index = 5;

	sq_int_array_push_to(array, index, 16384);
	sq_int_array_insert(array, index, ints, sizeof(ints) / sizeof(int));
```

If you use C++ language, you can use Sq::Array<int> directly. Sq::IntArray is defined as alias of Sq::Array<int>.
