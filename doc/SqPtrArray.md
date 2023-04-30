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

## Allocate

C function sq_ptr_array_alloc(), C++ method alloc() can allocate elements from rear of array.  
C function sq_ptr_array_alloc_at() and C++ overloaded method alloc() can allocate elements from the specified index of the array.  
SqPtrArray will expand array if it does not have enough space.  
  
use C language

```c
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = sq_ptr_array_alloc(array, length);

	// allocate elements from specified index
	elements = sq_ptr_array_alloc_at(array, index, length);
```

use C++ language

```c++
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = array->alloc(length);

	// allocate elements from specified index
	elements = array->alloc(index, length);
```

## Append

The appending action will first call sq_ptr_array_alloc() then copy data to the SqPtrArray.  
  
use C language

```c
	// append a element
	sq_ptr_array_append(array, element);

	// append multiple elements
	sq_ptr_array_append_n(array, elements, n_elements);
```

use C++ language

```c++
	// append a element
	array->append(element);

	// append multiple elements
	array->append(elements, n_elements);
```

## Insert

The inserting action will first call sq_ptr_array_alloc_at() to alloc elements in specified index then copy data to the specified index in SqPtrArray.  
  
use C language

```c
	// insert a element
	sq_ptr_array_insert(array, index, elements);

	// insert multiple elements
	sq_ptr_array_insert_n(array, index, elements, n_elements);
```

use C++ language

```c++
	// insert a element
	array->insert(index, elements);

	// insert multiple elements
	array->insert(index, elements, n_elements);
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

## Sort / Find

sort(), find(), and findSorted() require a "comparison function" to work.  
findSorted() finds an element in a sorted array using binary search and output index of inserting if no found.  
  
use C language

```c
	// comparison function 
	int   straddr_compare(const char **straddr1, const char **straddr2);

	int   inserted_index;
	char *key = "str";

	sq_ptr_array_sort(array, straddr_compare);

	sq_ptr_array_find_sorted(array, &key, straddr_compare, &inserted_index);
```

use C++ language

```c++
	array->sort(straddr_compare);

	array->findSorted(&key, straddr_compare, &inserted_index);
```
