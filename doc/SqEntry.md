# SqEntry
SqEntry define constant or dynamic field in structure/class.
It must use SqType to declare type of field.

```c
struct SqEntry
{
	SqType*      type;        // field type
	const char*  name;        // field name
	size_t       offset;      // offset of field in structure/class
	unsigned int bit_field;   // declare below
};
```

Declaring bit_field in SqEntry

| name            | description                                   | 
| --------------- | --------------------------------------------- |
| SQB_DYNAMIC     | entry can be changed and freed                |
| SQB_POINTER     | entry's instance is pointer                   |
| SQB_HIDDEN      | Converter will not output value in this entry |
| SQB_HIDDEN_NULL | Converter will not output if value is NULL    |

SqEntry and it's derived structure must use with SqType.
When user define constant SqEntry, it usually define array of SqEntry for a structure.

##### 1. define constant SqEntry pointer array that used by constant type
Note: This is SqEntry pointer array

```c
static const SqEntry *entry_pointers[2] = {
	&(SqEntry) {SQ_TYPE_UINT,   "bit_field",  offsetof(SqEntry, bit_field),  SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STRING, "name",       offsetof(SqEntry, name),       0},
};

const SqType type = SQ_TYPE_INITIALIZER(entry_pointers, 2, 0);
```

##### 2. define constant SqEntry array that used by dynamic type
Note: This is SqEntry array (NOT pointer array)

```c
static const SqEntry entries[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(SqEntry, bit_field),  SQB_HIDDEN},
	{SQ_TYPE_STRING, "name",       offsetof(SqEntry, name),       0},
};

sq_type_add_entry(type, entries, 2, 0);
```

##### 3. define dynamic SqEntry that used by dynamic type
There is only one dynamic entry is defined here

```c
SqEntry *entry = sq_entry_new(SQ_TYPE_STRING);
entry->name = strdup("name");
entry->offset = offsetof(SqEntry, name);
entry->bit_field = SQB_HIDDEN_NULL;

sq_type_add_entry(type, entry, 1, 0);
```

