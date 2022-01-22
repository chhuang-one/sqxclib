# SqEntry
SqEntry define constant or dynamic field in structure.
It must use [SqType](SqType.md) to declare data type of field.

	SqEntry
	│
	└─── SqReentry
	     │
	     ├─── SqTable
	     │
	     └─── SqColumn

Structure Definition:

```c
struct SqEntry
{
	const SqType *type;        // field type
	const char   *name;        // field name
	size_t        offset;      // offset of field in structure/class
	unsigned int  bit_field;   // declare below
};

// Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
//       C++ user can initialize static structure easily.
```

Declaring bit_field in SqEntry

| name            | description                                   | 
| --------------- | --------------------------------------------- |
| SQB_DYNAMIC     | entry can be changed and freed                |
| SQB_POINTER     | entry's instance is pointer                   |
| SQB_HIDDEN      | Converter will not output value in this entry |
| SQB_HIDDEN_NULL | Converter will not output if value is NULL    |

* SQB_DYNAMIC is for internal use only. User should NOT set/clear this bit.
* User can NOT change or free SqEntry if SqEntry.bit_field has NOT set SQB_DYNAMIC.
* User must use bitwise operators to set/clear bits in SqEntry.bit_field.
* It is better to use constant or static SqEntry with constant or static SqType.
* Dynamic SqType has reference count. It increase when dynamic SqEntry use it.
* Dynamic SqEntry can use with dynamic, constant, or static SqType.

## Define structured data type
See [SqType](SqType.md) to get more sample.

#### 1. define constant SqEntry pointer array that used by constant SqType
* Note: This is SqEntry pointer array. If you define constant SqType for structure, it must use with SqEntry pointer array.

```c
static const SqEntry *entryPointers[2] = {
	&(SqEntry) {SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	&(SqEntry) {SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

/* If your entryPointers are not sorted by name, pass 0 to last argument. */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, entryPointers, 0);

/* If your entryPointers are sorted by name, pass SQB_TYPE_SORTED to last argument. */
// const SqType type = SQ_TYPE_INITIALIZER(YourStruct, entryPointers, SQB_TYPE_SORTED);
```

#### 2. define constant SqEntry array that used by dynamic SqType
* Note: This is SqEntry array (NOT pointer array)

```c++
static const SqEntry entries[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	{SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

	// add 2 elements from array
	sq_type_add_entry(type, entries, 2, 0);    // C function
//	type->addEntry(entries, 2);                // C++ function
```

#### 3. define dynamic SqEntry that used by dynamic SqType

use C function to add one dynamic entry
```c
	SqEntry *entry = sq_entry_new(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // set bit in SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // clear bit in SqEntry.bit_field

	sq_type_add_entry(type, entry, 1, 0);
```

use C++ function to add one dynamic entry
```c++
	SqEntry *entry = new Sq::Entry(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // set bit in SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // clear bit in SqEntry.bit_field

	type->addEntry(entry);
```
