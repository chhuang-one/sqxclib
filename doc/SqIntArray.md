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
SqIntArray still defines some sq_int_array_xxx() macros and functions for C language, which are correspond to sq_array_xxx() series.  
  
**Initialize**
```
	sq_int_array_init
	sq_int_array_final
```

**Allocate**
```
	sq_int_array_alloc
	sq_int_array_alloc_at
```

**Append**
```
	sq_int_array_push
	sq_int_array_append
```

**Insert**
```
	sq_int_array_push_to
	sq_int_array_insert
```

**Sort**
```
	sq_int_array_sort
```

**Find**
```
	sq_int_array_find
	sq_int_array_find_sorted
```

If you use C++ language, you can use Sq::Array<int> directly. Sq::IntArray is still defined, which is aliases for the Sq::Array<int>.
