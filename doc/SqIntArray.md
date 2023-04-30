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
	int      *data;
	int       length;
};
```

## Functions and methods

User can use the same functions and methods as [SqArray](SqArray.md).  
  
SqIntArray still defines some sq_int_array_xxx() macros, which are aliases for the sq_array_xxx() series.  
  
use C language

```c
	SqIntArray  array;
	int         iarray[] = {0, 3, 2, 5};
	int         len = sizeof(iarray) / sizeof(int);

	sq_array_init(&array, sizeof(int), capacity);
	SQ_ARRAY_APPEND(&array, int, iarray, len);
```

If you use C++ language, you can use Sq::Array<int> directly. Sq::IntArray is still defined, which is aliases for the Sq::Array<int>.
