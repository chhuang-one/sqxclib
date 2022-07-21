[中文](SqEntry.cn.md)

# SqEntry
SqEntry define field in structure.
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
	size_t        offset;      // offset of field in structure
	unsigned int  bit_field;   // explain below
};

// Note: use 'const char*' to declare string and use 'const SqType*' to declare type,
//       C++ user can initialize static SqEntry easily.
```

Declaring bit_field of SqEntry

| name            | description                                   | 
| --------------- | --------------------------------------------- |
| SQB_DYNAMIC     | entry can be changed and freed                |
| SQB_POINTER     | entry's instance is pointer                   |
| SQB_HIDDEN      | Converter will not output value of this entry |
| SQB_HIDDEN_NULL | Converter will not output if value is NULL    |

* SQB_DYNAMIC is for internal use only. User should NOT set/clear this bit.
* User can NOT change or free SqEntry if SqEntry.bit_field has NOT set SQB_DYNAMIC.
* User must use bitwise operators to set/clear bits in SqEntry.bit_field.
* It is better to use constant (or static) SqEntry with constant (or static) SqType.
* Dynamic SqEntry can use with dynamic, constant, or static SqType.

## Define structured data type
You can see [SqType](SqType.md) to get more sample.

#### 1. define constant pointer array of SqEntry that used by constant SqType
* Note: If you define constant SqType for structure, it must use with **pointer array** of SqEntry.

```c
static const SqEntry  entryArray[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	{SQ_TYPE_STRING, "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

static const SqEntry *entryPointerArray[2] = {
	& entryArray[0],
	& entryArray[1],
};

/* If 'entryPointerArray' is NOT sorted by name, pass 0 to last argument.
   Otherwise use SQB_TYPE_SORTED to replace 0 in last argument.
 */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, entryPointerArray, 0);
```

#### 2. define constant SqEntry array that used by dynamic SqType

e.g. create dynamic SqType that using constant SqEntry

use C language

```c
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_entry_free);

	// add 2 entries from pointer array
	sq_type_add_entry_ptrs(type, entryPointerArray, 2);

	// add 2 entries from array (NOT pointer array)
//	sq_type_add_entry(type, entryArray, 2, 0);
```

use C++ language

```c++
	Sq::Type *type = new Sq::Type(0, sq_entry_free);

	// add 2 entries from pointer array
	type->addEntry(entryPointerArray, 2);

	// add 2 entries from array (NOT pointer array)
//	type->addEntry(entryArray, 2);
```

#### 3. create dynamic SqEntry that used by dynamic SqType

use C language

```c
	SqEntry *entry = sq_entry_new(SQ_TYPE_STRING);

	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // set bit in SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // clear bit in SqEntry.bit_field

	sq_type_add_entry(type, entry, 1, 0);
```

use C++ language

```c++
	Sq::Entry *entry = new Sq::Entry(SQ_TYPE_STRING);

	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // set bit in SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // clear bit in SqEntry.bit_field

	type->addEntry(entry);
```
