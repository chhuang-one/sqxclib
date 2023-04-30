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

## Functions and methods

User can use the same functions and methods as [SqPtrArray](SqPtrArray.md).  
  
SqStrArray will copy source data when appending and inserting, which is different from [SqPtrArray](SqPtrArray.md).

```c
	SqStrArray  array;
	const char *strs[] = {"One", "Two", "Three"};
	int         n_strs = sizeof(strs) / sizeof(char*);

	sq_str_array_init(&array, 0);

	sq_str_array_append_n(&array, strs, n_strs);

	// insert 'strs' to index 1 of array
	sq_str_array_insert_n(&array, 1, strs, n_strs);
```
