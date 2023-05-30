[中文](SqPtrArray.cn.md)

# SqPtrArray

SqPtrArray is pointer array. It is derived classes of [SqArray](SqArray.md).  
SQ_TYPE_PTR_ARRAY is a built-in static constant type that can create SqPtrArray instances.

	SqArray
	|
	└─── SqPtrArray
	     │
	     ├─── SqStrArray
	     │
	     └─── SqIntptrArray (deprecated)

[SqArray](SqArray.md) and it's derived classes share data structures and code to reduce binary size.

## Data members

Like [SqArray](SqArray.md), SqPtrArray has the same hidden members in front of the array.  
  
SqPtrArray Structure Definition:

```c
struct SqPtrArray
{
	void    **data;
	int       length;
};
```

There are hidden members in front of SqPtrArray.data:  
capacity  is the number of elements actually allocated in the array. (excluding the header in front of the array)  
clearFunc is a function that clears elements in an array.  
  
Below is the code to access these hidden members:

```c++
	// C functions
	capacity   = sq_ptr_array_capacity(array);
	clearFunc  = sq_ptr_array_clear_func(array);

	// C++ methods
	capacity   = array->capacity();
	clearFunc  = array->clearFunc();
```

## Initialize

C function sq_ptr_array_init(), C++ constructor can initialize instance of SqPtrArray.  
  
use C language

```c
	// 'func' can be NULL if you don't need to free the element's memory.
	SqDestroyFunc  func = free;

	SqPtrArray     arrayLocal;
	sq_ptr_array_init(&arrayLocal, capacity, func);
	sq_ptr_array_final(&arrayLocal);

	SqPtrArray    *array;
	array = sq_ptr_array_new(capacity, func);
	sq_ptr_array_free(array);
```

use C++ language

```c++
	// 'func' can be NULL if you don't need to free the element's memory.
	SqDestroyFunc  func = free;

	// Sq::PtrArray has constructor and destructor
	Sq::PtrArray   arrayLocal;

	Sq::PtrArray  *array;
	array = new Sq::PtrArray(capacity, func);
	delete array;
```

## Functions and methods

User can use the same functions and methods as [SqArray](SqArray.md).  
SqPtrArray still defines some sq_ptr_array_xxx() macros and functions for C language, which are correspond to sq_array_xxx() series.  
  
**Allocate**
```
	sq_ptr_array_alloc
	sq_ptr_array_alloc_at
```

**Append**
```
	sq_ptr_array_push
	sq_ptr_array_append
```

**Insert**
```
	sq_ptr_array_push_to
	sq_ptr_array_insert
```

**Sort**
```
	sq_ptr_array_sort
```

**Find**
```
	sq_ptr_array_find
	sq_ptr_array_find_sorted
```

## Erase / Steal

erase() removes elements from array with calling the clear function.  
steal() removes elements from array without calling the clear function.  
  
use C language

```c
	// erase elements
	sq_ptr_array_erase(array, index, n_elements);

	// steal elements
	sq_ptr_array_steal(array, index, n_elements);
```

use C++ language

```c++
	// erase elements
	array->erase(index, n_elements);

	// steal elements
	array->steal(index, n_elements);
```
