[中文](SqArray.cn.md)

# SqArray

SqArray is array of arbitrary elements. Because some structures (e.g. SqType) need to be converted from static to dynamic, its data members must be compatible with these structures.  
SQ_TYPE_ARRAY is a built-in static constant type that can create SqArray instances.

	SqArray
	│
	├─── SqIntArray
	│
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
	uint8_t  *data;
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
  
use C language

```c++
	SqArray *array;

	// C macro (You must specify ElementType here)
	element = sq_array_at(array, ElementType, index);

	// C data member (You must specify ElementType here)
	element = ((ElementType*)array->data)[index];
```

use C++ language

```c++
	Sq::Array<ElementType> *array;

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
	ElementType *elements;
	int   length = 16;
	int   index  = 8;

	// allocate elements from rear
	elements = (ElementType*)sq_array_alloc(array, length);

	// allocate elements from specified index
	elements = (ElementType*)sq_array_alloc_at(array, index, length);
```

use C++ language

```c++
	ElementType *elements;
	int   length = 16;
	int   index  = 8;

	// allocate elements from rear
	elements = array->alloc(length, length);

	// allocate elements from specified index
	elements = array->allocAt(index, length);
```

## Append

The appending action will first call sq_array_alloc() to allocate elements in rear then copy data to the SqArray.  
  
use C language

```c
	ElementType  elementValue;
	ElementType *elements;

	// If ElementType is arithmetic type (or pointer type), you can use this to
	// append a element (You must specify ElementType here)
	sq_array_push(array, ElementType, elementValue);

	// append multiple elements (You must specify ElementType here)
	SQ_ARRAY_APPEND(array, ElementType, elements, n_elements);
```

use C++ language

```c++
	// If ElementType is arithmetic type (or pointer type), you can use this to
	// append a element
	array->append(elementValue);

	// append multiple elements
	array->append(elements, n_elements);
```

## Insert

The inserting action will first call sq_array_alloc_at() to allocate elements in specified index then copy data to the specified index in SqArray.  
  
use C language

```c
	ElementType  elementValue;
	ElementType *elements;

	// If ElementType is arithmetic type (or pointer type), you can use this to
	// insert a element (You must specify ElementType here)
	sq_array_push_to(array, ElementType, index, elementValue);

	// insert multiple elements (You must specify ElementType here)
	SQ_ARRAY_INSERT(array, ElementType, index, elements, n_elements);
```

use C++ language

```c++
	// If ElementType is arithmetic type (or pointer type), you can use this to
	// insert a element
	array->insert(index, elementValue);

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

sort()       sort elements of array.  
find()       finds an element in a unsorted array.  
findSorted() finds an element in a sorted array using binary search and output index of inserting if no found.  
  
The above 3 functions need the "comparison function" to work.  
  
use C language

```c
	// comparison function
	int   elementCompare(ElementType *element1, ElementType *element2);

	//  You must specify ElementType here
	SQ_ARRAY_SORT(array, ElementType, elementCompare);

	ElementType *key = pointerToKey;
	ElementType *element;
	int          insertingIndex;
	//  You must specify ElementType here
	element = SQ_ARRAY_FIND(array, ElementType, key, elementCompare);
	element = SQ_ARRAY_FIND_SORTED(array, ElementType, key, elementCompare, &insertingIndex);
```

use C++ language

```c++
	array->sort(elementCompare);

	ElementType *key = pointerToKey;
	ElementType *element;
	int          insertingIndex;
	element = array->find(key, elementCompare);
	element = array->findSorted(key, elementCompare, &insertingIndex);
```

If you specify arithmetic type (or C string type) to Sq::Array template in C++, it will generate static compare function.  
In this case, you can call sort(), findSorted()...etc without comparison function.

```c++
	Sq::Array<int>  intArray;

	intArray->sort();

	int   key = 31;
	int  *element;
	element = intArray->find(key);
	element = intArray->findSorted(key, &insertingIndex);
```
