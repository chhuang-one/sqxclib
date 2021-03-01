# SqEntry
SqEntry define constant or dynamic field in structure.
It must use SqType to declare type of field.

```c
// use 'const char*' to declare string and use 'const SqType*' to declare type,
// C++ user can initialize static structure easily.

struct SqEntry
{
	const SqType* type;        // field type
	const char*   name;        // field name
	size_t        offset;      // offset of field in structure/class
	unsigned int  bit_field;   // declare below
};
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
* Dynamic SqType has reference count. It increase when dynamic SqEntry use it.
* It is better to use constant or static SqEntry with constant or static SqType.
* Dynamic SqEntry can use with dynamic, constant, or static SqType.

##### 1. define constant SqEntry pointer array that used by constant SqType
* Note: This is SqEntry pointer array. If you define constant SqType for structure, it must use with SqEntry pointer array.

```c
static const SqEntry *entryPointers[2] = {
	&(SqEntry) {SQ_TYPE_UINT,   "bit_field",  offsetof(SqEntry, bit_field),  SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STRING, "name",       offsetof(SqEntry, name),       0},
};

const SqType type = SQ_TYPE_INITIALIZER(entryPointers, 2, 0);
```

##### 2. define constant SqEntry array that used by dynamic SqType
* Note: This is SqEntry array (NOT pointer array)

```c
static const SqEntry entries[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(SqEntry, bit_field),  SQB_HIDDEN},
	{SQ_TYPE_STRING, "name",       offsetof(SqEntry, name),       0},
};

sq_type_add_entry(type, entries, 2, 0);
```

##### 3. define dynamic SqEntry that used by dynamic SqType
There is only one dynamic entry is defined here

```c
SqEntry *entry = sq_entry_new(SQ_TYPE_STRING);
entry->name = strdup("name");
entry->offset = offsetof(SqEntry, name);
entry->bit_field = SQB_HIDDEN_NULL;

sq_type_add_entry(type, entry, 1, 0);
```

