[中文](SqArray.cn.md)

# SqArray

SqArray is array of arbitrary elements. Because some structures (e.g. SqType) need to be converted from static to dynamic, its data members must be compatible with these structures.  
SQ_TYPE_ARRAY is a built-in static constant type that can create SqArray instances.

	SqArray
	|
	├─── SqIntArray
	|
	└─── SqPtrArray
	     │
	     ├─── SqStrArray
	     │
	     └─── SqIntptrArray (deprecated)

[SqIntArray](SqIntArray.md) and [SqPtrArray](SqPtrArray.md) are derived classes of SqArray. They share data structures and code to reduce binary size.  
[SqIntArray](SqIntArray.md) is integer array, [SqPtrArray](SqPtrArray.md) is pointer array.

## Data members

SqArray is small and simple. It can be used directly in the stack memory. It appears to have only 2 members, but in fact the other members are hidden in front of the array.  
  
SqArray Structure Definition:

```c
struct SqArray
{
	uint8_t **data;
	int       length;
};
```

There are hidden members in front of SqArray.data:  
capacity    is the number of elements actually allocated in the array. (excluding the header in front of the array)  
elementSize is element size in the array.  
  
Below is the code to access these hidden members:

```c++
	// C functions
	capacity     = sq_array_capacity(array);
	elementSize  = sq_array_element_size(array);

	// C++ methods
	capacity     = array->capacity();
	elementSize  = array->elementSize();
```

## Initialize

C function sq_array_init(), C++ constructor can initialize instance of SqArray.  
In below samples, ElementType is element type of SqArray.  
  
use C language

```c
	SqArray     arrayLocal;
	sq_array_init(&arrayLocal, sizeof(ElementType), capacity);
	sq_array_final(&arrayLocal);

	SqArray    *array;
	array = sq_array_new(sizeof(ElementType), capacity);
	sq_array_free(array);
```

use C++ language

```c++
	// Sq::Array has constructor and destructor
	Sq::Array<ElementType>   arrayLocal;

	Sq::Array<ElementType>  *array;
	array = new Sq::Array<ElementType>(capacity);
	delete array;
```

## Access element

Returns a element at specified position in the SqArray.

```c++
	// C macro (You must specify ElementType here)
	element = sq_array_at(array, ElementType, index);

	// C++ method
	element = array->at(index);

	// C++ data member
	element = array->data[index];
```

## Allocate

C function sq_array_alloc(), C++ method alloc() can allocate elements from rear of array.  
C function sq_array_alloc_at() and C++ overloaded method alloc() can allocate elements from the specified index of the array.  
SqArray will expand array if it does not have enough space.  
  
use C language

```c
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = sq_array_alloc(array, length);

	// allocate elements from specified index
	elements = sq_array_alloc_at(array, index, length);
```

use C++ language

```c++
	void *elements;
	int   length = 16;
	int   index  = 8;

	elements = array->alloc(length);

	// allocate elements from specified index
	elements = array->allocAt(index, length);
```

## Append

The appending action will first call sq_array_alloc() then copy data to the SqArray.  
  
use C language

```c
	// append a element
	*sq_array_alloc(array, 1) = element;

	// append multiple elements (You must specify ElementType here)
	SQ_ARRAY_APPEND(array, ElementType, elements, n_elements);
```

use C++ language

```c++
	// append a element
	*array->alloc() = element;

	// append multiple elements
	array->append(elements, n_elements);
```

## Insert

The inserting action will first call sq_array_alloc_at() to alloc elements in specified index then copy data to the specified index in SqArray.  
  
use C language

```c
	// insert a element
	*sq_array_alloc_at(array, index, 1) = elements;

	// insert multiple elements (You must specify ElementType here)
	SQ_ARRAY_INSERT(array, ElementType, index, elements, n_elements);
```

use C++ language

```c++
	// insert a element
	*array->allocAt(index) = elements;

	// insert multiple elements
	array->insert(index, elements, n_elements);
```

## Steal

steal() removes elements from array.  
  
use C language

```c
	// remove elements (You must specify ElementType here)
	SQ_ARRAY_STEAL(array, ElementType, index, n_elements);
```

use C++ language

```c++
	// remove elements
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

	//  You must specify ElementType here
	SQ_ARRAY_SORT(array, ElementType, straddr_compare);

	sq_array_find_sorted(array, &key, straddr_compare, &inserted_index);
```

use C++ language

```c++
	array->sort(straddr_compare);

	array->findSorted(&key, straddr_compare, &inserted_index);
```

If you specify arithmetic type to Sq::Array template in C++, it will gererate static compare function.  
In this case, you can call sort(), findSorted()...etc without comparison function.

```c++
	Sq::Array<int>  intArray;

	array->sort();

	array->findSorted(&key, &inserted_index);
```
