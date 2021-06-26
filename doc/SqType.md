# SqType

It define how to initialize, finalize, and convert C data type.
SqEntry use it to declare data type. Sqxc use it to convert data.

Built-in SqType with it's C data type

| SqType          | C data type  |
| --------------- | ------------ |
| SQ_TYPE_BOOL    | bool         |
| SQ_TYPE_INT     | int          |
| SQ_TYPE_UINT    | unsigned int |
| SQ_TYPE_INTPTR  | intptr_t     |
| SQ_TYPE_INT64   | int64_t      |
| SQ_TYPE_UINT64  | uint64_t     |
| SQ_TYPE_TIME    | time_t       |
| SQ_TYPE_DOUBLE  | double       |
| SQ_TYPE_STRING  | char*        |

SqType with it's C++ data type

| SqType                 | C++ data type  |
| ---------------------- | -------------- |
| SQ_TYPE_STD_STRING     | std::string    |
| Sq::TypeStl<Container> | STL containers |

Declaring bit_field in SqType

| name             | description                            | 
| ---------------- | -------------------------------------- |
| SQB_TYPE_DYNAMIC | type can be changed and freed          |
| SQB_TYPE_SORTED  | type->entry is sorted by SqEntry.name  |

* SQB_TYPE_DYNAMIC is for internal use only. User should NOT set/clear this bit.
* User can NOT change or free SqType if SqType.bit_field has NOT set SQB_TYPE_DYNAMIC.
* User must use bitwise operators to set/clear bits in SqType.bit_field.
* It is better to use constant or static SqEntry with constant or static SqType.
* Dynamic SqType has reference count. It increase when dynamic SqEntry use it.
* Dynamic SqEntry can use with dynamic, constant, or static SqType.

## 1. use SqType to define basic (not structured) data type
refer source code SqType-built-in.c to get more sample.

#### 1.1. Define constant basic (not structured) data type

use C99 designated initializer or C++ aggregate initialization to define static SqType.
```
const SqType type_int = {
	.size  = sizeof(int),
	.init  = NULL,            // initialize function
	.final = NULL,            // finalize function
	.parse = int_parse_function,
	.write = int_write_function,
};

```

#### 1.2. Define dynamic basic (not structured) data type

1. pass argument 'prealloc_size' = -1, 'entry_destroy_func' = NULL to sq_type_new()
2. assign size, init, final, parse, and write in SqType structure.

```c++
// sq_type_new() declarations:
// SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);

	SqType *type;

	type = sq_type_new(-1, NULL);    // C function

//	type = new Sq::Type;             // C++ function
//	type->initSelf(-1, NULL);

	type->size  = sizeof(int);
	type->init  = NULL;           // initialize function
	type->final = NULL;           // finalize function
	type->parse = int_parse_function;
	type->write = int_write_function;
```

## 2. use SqType to define structured data type
User can define a constant or dynamic SqType. If you define constant SqType for structure, it must use with SqEntry pointer array.

First, we define a custom structured data type:
```c
typedef struct User     User;

struct User {
	int    id;
	char  *name;
	char  *email;
};
```

#### 2.1. Define constant SqType with constant 'unsorted' SqEntry pointer array
use C99 designated initializer to define struct that has 'unsorted' SqEntry pointer array.

```c
// --- entryPointers is 'unsorted' SqEntry pointer array
static const SqEntry  *entryPointers[] = {
	&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
	&(SqEntry) {SQ_TYPE_STRING, "email", offsetof(User, email), 0},
};

// --- typeUser use 'unsorted' entryPointers
const SqType typeUser = {
	.size  = sizeof(User),
	.init  = NULL,
	.final = NULL,
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),
	.entry   = entryPointers,
	.n_entry = sizeof(entryPointers) / sizeof(SqEntry*),
	.bit_field = 0,
};
```

about above macro SQ_GET_TYPE_NAME(Type):
* It is used to get name of structured data type in C and C++ code.
* warning: You will get difference type name from C and C++ source code when you use gcc to compile because gcc's typeid(Type).name() will return strange name.

#### 2.2. Define constant SqType with constant 'sorted' SqEntry pointer array

use C99 designated initializer to define struct that has 'sorted' SqEntry pointer array.

```c
// --- sortedEntryPointers is 'sorted' entryPointers (sorted by name)
static const SqEntry  *sortedEntryPointers[] = {
	&(SqEntry) {SQ_TYPE_STRING, "email", offsetof(User, email), 0},
	&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
};

// --- sortedTypeUser use sortedEntryPointers (set SQB_TYPE_SORTED in SqType::bit_field)
const SqType sortedTypeUser = {
	.size  = sizeof(User),
	.init  = NULL,
	.final = NULL,
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),

	// --- The following is the difference from typeUser
	.entry   = sortedEntryPointers,
	.n_entry = sizeof(sortedEntryPointers) / sizeof(SqEntry*),
	.bit_field = SQB_TYPE_SORTED,
	// Because sortedEntryPointers have been 'sorted' by SqEntry::name,
	// you can set SQB_TYPE_SORTED in SqType::bit_field
};
```

#### 2.3. Define constant SqType with constant SqEntry pointer array (use C macro)

use macro SQ_TYPE_INITIALIZER() and SQ_TYPE_INITIALIZER_FULL()
```c
// Because entryPointers is 'unsorted' entries, you can pass 0 to last argument.
const SqType  typeUserM = SQ_TYPE_INITIALIZER(User, entryPointers, 0);

// Because sortedEntryPointers is 'sorted' entries, you can pass SQB_TYPE_SORTED to last argument.
const SqType  sortedTypeUserM = SQ_TYPE_INITIALIZER(User, sortedEntryPointers, SQB_TYPE_SORTED);
```

#### 2.4. Define dynamic SqType with constant SqEntry pointer array

use C function sq_type_add_entry_ptrs() to add static SqEntry pointer array.
```c
	SqType  *type;
	int   n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);
	sq_type_add_entry_ptrs(type, entryPointers, n_entry);
```

use C++ function addEntry() to add static SqEntry pointer array.
```c++
	Sq::Type  *type;
	int   n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = new Sq::Type;
	type->initSelf(8, sq_entry_free);
	type->addEntry(entryPointers, n_entry);
```

#### 2.5. Define dynamic SqType with dynamic SqEntry

use C function sq_type_add_entry() to add dynamic SqEntry.
```c
	SqType  *type;
	SqEntry *entry;

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	entry = sq_entry_new(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;        // set bit in SqEntry.bit_field
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STRING);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	sq_type_add_entry(type, entry, 1, 0);
```

use C++ functions to add dynamic SqEntry.
```c++
	Sq::Type  *type;
	Sq::Entry *entry;

	type = new Sq::Type;
	type->initSelf(8, sq_entry_free);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;    // set bit in SqEntry.bit_field
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	type->addEntry(entry);

	entry = new Sq::Entry;
	entry->init(SQ_TYPE_STRING);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	type->addEntry(entry);
```

## 3. calculate instance size for dynamic structured data type

* User can use C function sq_type_decide_size(), C++ function decideSize() to calculate instance size.
* You don't need to call function to calculate instance size after adding entry because program will do it automatically.
* You must call function to calculate instance size after removing entry from type.

```c++
// sq_type_decide_size() declarations:
// int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed);

	sq_type_decide_size(type, NULL, false);    // C function
	type->decideSize(NULL, false);             // C++ function
```

* if 'inner_entry' == NULL, it use all entries in SqType to calculate size.
* if user add 'inner_entry' to SqType, pass argument 'entry_removed' = false.
* if user remove 'inner_entry' from SqType, pass argument 'entry_removed' = true.

## 4. find & remove entry from dynamic SqType that defined structured data type

use C function to find & remove SqEntry

```c
	SqEntry **entry_addr;

	entry_addr = sq_type_find_entry(type, "entry_name", NULL);
	sq_type_erase_entry_addr(type, entry_addr, 1);

	// remove entry from type and keep entry in memory
//	sq_type_steal_entry_addr(type, entry_addr, 1);

	sq_type_decide_size(type, NULL, true);
//	sq_type_decide_size(type, *entry_addr, true);
```

use C++ function to find & remove SqEntry

```c++
	SqEntry **entry_addr;

	entry_addr = type->findEntry(type, "entry_name");
	type->eraseEntry(type, entry_addr);

	// remove entry from type and keep entry in memory
//	type->stealEntry(type, entry_addr);

	type->decideSize(NULL, true);
//	type->decideSize(*entry_addr, true);
```

## 5. reference count in dynamic SqType

It will increase reference count of SqType if a dynamic SqEntry use a dynamic SqType.  
User can call function to increase reference count.  

```c++
	sq_type_ref(type);    // C function

	type->ref();          // C++ function
```

It will decrease reference count of SqType if user free a dynamic SqEntry that having a dynamic SqType.  
User can call function to decrease reference count.

```c++
	sq_type_unref(type);    // C function

	type->unref();          // C++ function
```

If an dynamic SqType's reference count reaches zero, it will be destroyed.
