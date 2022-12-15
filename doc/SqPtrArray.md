[中文](SqPtrArray.cn.md)

# SqPtrArray

SqPtrArray is pointer array. Because some structures (e.g. SqType) need to be converted from static to dynamic, its data members must be compatible with these structures.  
SQ_TYPE_PTR_ARRAY is a built-in static constant type that can create SqPtrArray instances.

	SqPtrArray
	│
	├─── SqIntptrArray
	│
	└─── SqStringArray

SqIntptrArray and SqStringArray are derived classes of SqPtrArray. They share data structures and code to reduce binary size.  
SqIntptrArray is integer array, SqStringArray is string array.

## Data members

SqPtrArray is small and simple. It can be used directly in the stack memory. It appears to have only 2 members, but in fact the other members are hidden in front of the array.  
  
SqPtrArray Structure Definition:

```c
struct SqPtrArray
{
	void    **data;
	int       length;
};
```

There are 3 hidden members in front of SqPtrArray.data:  
index -1 is number of elements in front of array. (This value is 3 currently)  
Index -2 is the number of elements actually allocated in the array. (excluding the elements in front of the array)  
index -3 is destroy function for elements in array.  
  
Below is the code to access these hidden members:

```c++
	SqPtrArray    *array;

	intptr_t       headerLength;
	intptr_t       capacity;
	SqDestroyFunc  destroyFunc;


	headerLength = (intptr_t) array->data[-1];
	capacity     = (intptr_t) array->data[-2];
	destroyFunc  = (SqDestroyFunc) array->data[-3];
```

There are C functions and C++ methods to access these hidden members.

```c++
	// C functions
	headerLength = sq_ptr_array_header_length(array);
	capacity     = sq_ptr_array_allocated(array);
	destroyFunc  = sq_ptr_array_destroy_func(array);

	// C++ methods
	headerLength = array->headerLength();
	capacity     = array->allocated();
	destroyFunc  = array->destroyFunc();
```

## Initialize

C function sq_ptr_array_init(), C++ method init() can initialize instance of SqPtrArray.  
  
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

#### Initialize derived class

sq_ptr_array_init_full() can specify number of elements in front of array. If you define a derived class of SqPtrArray and add more hidden members to it, you can call this function and increase the value of 'headerLength'.

```c
	sq_ptr_array_init_full(array, capacity, headerLength, func);
```

## Allocate

C function sq_ptr_array_alloc(), C++ method alloc() can allocate elements from rear of array. SqPtrArray will expand array if it does not have enough space.  
  
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

	elements = sq_ptr_array_alloc(array, length);

	// allocate elements from specified index
	elements = sq_ptr_array_alloc_at(array, index, length);
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

erase() removes elements from array with calling the destroy function.  
steal() removes elements from array without calling the destroy function.  
  
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
