[中文](SqType.cn.md)

# SqType

It define how to initialize, finalize, and convert C data type.
SqEntry use it to define data type. Sqxc use it to convert data.
  
Structure Definition:

```c
struct SqType
{
	unsigned int   size;           // instance size

	SqTypeFunc     init;           // initialize instance
	SqTypeFunc     final;          // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to assign "name"
	// or use macro SQ_GET_TYPE_NAME()
	char          *name;

	// SqType.entry is array of SqEntry pointer if current SqType is for C struct type.
	// SqType.entry isn't freed if SqType.n_entry == -1
	SqEntry      **entry;          // SqPtrArray.data
	int            n_entry;        // SqPtrArray.length
	// *** About above 2 fields:
	// 1. They are expanded by macro SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry)
	// 2. They can NOT change data type and order.

	// SqType.bit_field has SQB_TYPE_DYNAMIC if SqType is dynamic and freeable.
	// SqType.bit_field has SQB_TYPE_SORTED  if SqType.entry is sorted.
	unsigned int   bit_field;

	// This for derived or custom SqType.
	// Instance of SqType will be passed to SqType.on_destroy
	SqDestroyFunc  on_destroy;     // destroy notifier for SqType. It can be NULL.
};
```

Built-in SqType with it's data type

| SqType          | C data type  | SQL data type     |
| --------------- | ------------ | ----------------- |
| SQ_TYPE_BOOL    | bool         | BOOLEAN           |
| SQ_TYPE_INT     | int          | INT               |
| SQ_TYPE_UINT    | unsigned int | INT (UNSIGNED)    |
| SQ_TYPE_INTPTR  | intptr_t     | INT               |
| SQ_TYPE_INT64   | int64_t      | BIGINT            |
| SQ_TYPE_UINT64  | uint64_t     | BIGINT (UNSIGNED) |
| SQ_TYPE_TIME    | time_t       | TIMESTAMP         |
| SQ_TYPE_DOUBLE  | double       | DOUBLE            |
| SQ_TYPE_STR     | char*        | VARCHAR           |
| SQ_TYPE_STRING  | char*        | VARCHAR           |
| SQ_TYPE_CHAR    | char*        | CHAR              |

* Different SQL products may map these C data types to different SQL data types.
  
SqType with it's C container type

| SqType                | C data type    |
| --------------------- | -------------- |
| SQ_TYPE_PTR_ARRAY     | SqPtrArray     |
| SQ_TYPE_STR_ARRAY     | SqStrArray     |
| SQ_TYPE_INTPTR_ARRAY  | SqIntptrArray  |

SqType with it's C++ data type

| SqType                 | C++ data type  |
| ---------------------- | -------------- |
| SQ_TYPE_STD_STR        | std::string    |
| SQ_TYPE_STD_STRING     | std::string    |
| Sq::TypeStl<Container> | STL containers |

Define bit_field of SqType

| name             | description                            | 
| ---------------- | -------------------------------------- |
| SQB_TYPE_DYNAMIC | type can be changed and freed          |
| SQB_TYPE_SORTED  | type->entry is sorted by SqEntry.name  |

* SQB_TYPE_DYNAMIC is for internal use only. User should NOT set or clear this bit.
* User can NOT change or free SqType if SqType.bit_field has NOT set SQB_TYPE_DYNAMIC.
* User must use bitwise operators to set or clear bits in SqType.bit_field.
* It is better to use constant or static SqEntry with constant or static SqType.
* Dynamic SqEntry can use with dynamic, constant, or static SqType.

## 1 use SqType to define basic (not structured) data type
refer source code SqType-built-in.c to get more sample.

#### 1.1 Define constant basic (not structured) data type

use C99 designated initializer or C++ aggregate initialization to define static SqType.
```
const SqType type_int = {
	.size  = sizeof(int),
	.init  = NULL,                  // initialize function
	.final = NULL,                  // finalize function
	.parse = int_parse_function,    // Function that parse data from Sqxc instance
	.write = int_write_function,    // Function that write data to   Sqxc instance
};

```

#### 1.2 Define dynamic basic (not structured) data type

function sq_type_new() declarations:

```c++
SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);
```

1. pass argument 'prealloc_size' = -1, 'entry_destroy_func' = NULL to sq_type_new()
2. assign size, init, final, parse, and write in SqType structure.

```c++
	SqType *type;

	/* C function */
	type = sq_type_new(-1, NULL);

	/* C++ method */
//	type = new Sq::Type(-1, NULL);

	type->size  = sizeof(int);
	type->init  = NULL;                  // initialize function
	type->final = NULL;                  // finalize function
	type->parse = int_parse_function;    // Function that parse data from Sqxc instance
	type->write = int_write_function;    // Function that write data to   Sqxc instance
```

## 2 use SqType to define structured data type
User can define a constant or dynamic SqType. If you define constant SqType for structure, it must use with **pointer array** of SqEntry.

First, we define a custom structured data type:

```c
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct User     User;

struct User {
	int    id;
	char  *name;
	char  *email;
};
```

#### 2.1 constant SqType use 'unsorted' constant pointer array of SqEntry
1. use C99 designated initializer to define 'unsorted' **pointer array** of SqEntry.
2. define constant SqType to use **pointer array** of SqEntry.

```c
/* entryPointers is 'unsorted' pointer array of SqEntry */
static const SqEntry  *entryPointers[] = {
	&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STR,    "name",  offsetof(User, name),  0},
	&(SqEntry) {SQ_TYPE_STR,    "email", offsetof(User, email), 0},
};

/* typeUser use 'unsorted' entryPointers */
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
* warning: You will get different type name from C and C++ source code when you use gcc to compile because gcc's typeid(Type).name() will return strange name.

#### 2.2 constant SqType use 'sorted' constant pointer array of SqEntry
1. use C99 designated initializer to define 'sorted' **pointer array** of SqEntry.
2. define constant SqType to use **pointer array** of SqEntry.
3. SqType.bit_field must set SQB_TYPE_SORTED.

```c
/* sortedEntryPointers is 'sorted' entryPointers (sorted by name) */
static const SqEntry  *sortedEntryPointers[] = {
	&(SqEntry) {SQ_TYPE_STR,    "email", offsetof(User, email), 0},
	&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STR,    "name",  offsetof(User, name),  0},
};

/* sortedTypeUser use sortedEntryPointers (set SQB_TYPE_SORTED in SqType::bit_field) */
const SqType sortedTypeUser = {
	.size  = sizeof(User),
	.init  = NULL,
	.final = NULL,
	.parse = sq_type_object_parse,
	.write = sq_type_object_write,
	.name  = SQ_GET_TYPE_NAME(User),

	// --- The following are differences from typeUser
	.entry   = sortedEntryPointers,
	.n_entry = sizeof(sortedEntryPointers) / sizeof(SqEntry*),
	.bit_field = SQB_TYPE_SORTED,
	// Because sortedEntryPointers have been 'sorted' by SqEntry::name,
	// you can set SQB_TYPE_SORTED in SqType::bit_field
};
```

#### 2.3 constant SqType use constant pointer array of SqEntry (use C macro)

use macro SQ_TYPE_INITIALIZER() and SQ_TYPE_INITIALIZER_FULL() to define constant SqType.

```c
/* Because entryPointers is 'unsorted' entries, you can pass 0 to the last argument. */
const SqType  typeUserM = SQ_TYPE_INITIALIZER(User, entryPointers, 0);

/* Because sortedEntryPointers is 'sorted' entries, you can pass SQB_TYPE_SORTED to the last argument. */
const SqType  sortedTypeUserM = SQ_TYPE_INITIALIZER(User, sortedEntryPointers, SQB_TYPE_SORTED);
```

#### 2.4 dynamic SqType use constant pointer array of SqEntry

use C language to add static **pointer array** of SqEntry.

```c
	SqType  *type;
	int      n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);
	sq_type_add_entry_ptrs(type, entryPointers, n_entry);
```

use C++ language to add static **pointer array** of SqEntry.

```c++
	Sq::Type  *type;
	int        n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = new Sq::Type(8, sq_entry_free);
	type->addEntry(entryPointers, n_entry);
```

#### 2.5 dynamic SqType use dynamic SqEntry

use C language to add dynamic SqEntry.

```c
	SqType  *type;
	SqEntry *entry;

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	entry = sq_entry_new(SQ_TYPE_INT);
	sq_entry_set_name(entry, "id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;        // set bit in SqEntry.bit_field
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STR);
	sq_entry_set_name(entry, "name");
	entry->offset = offsetof(User, name);
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STR);
	sq_entry_set_name(entry, "email");
	entry->offset = offsetof(User, email);
	sq_type_add_entry(type, entry, 1, 0);
```

use C++ language to add dynamic SqEntry.

```c++
	Sq::Type  *type;
	Sq::Entry *entry;

	type = new Sq::Type(8, sq_entry_free);

	entry = new Sq::Entry(SQ_TYPE_INT);
	entry->setName("id");
	entry->offset = offsetof(User, id);
	entry->bit_field |= SQB_HIDDEN;    // set bit in SqEntry.bit_field
	type->addEntry(entry);

	entry = new Sq::Entry(SQ_TYPE_STR);
	entry->setName("name");
	entry->offset = offsetof(User, name);
	type->addEntry(entry);

	entry = new Sq::Entry(SQ_TYPE_STR);
	entry->setName("email");
	entry->offset = offsetof(User, email);
	type->addEntry(entry);
```

#### 2.6 calculate instance size for dynamic structured data type

* User can use C function sq_type_decide_size(), C++ method decideSize() to calculate instance size.
* You don't need to call function to calculate instance size after adding entry because program will do it automatically.
* You must call function to calculate instance size after removing entry from type.
  
function sq_type_decide_size() declarations:

```c++
/* C function */
unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed);

/* C++ method */
unsigned int  Sq::Type::decideSize(const SqEntry *inner_entry, bool entry_removed);
```

* if 'inner_entry' == NULL, it use all entries in SqType to recalculate size.
* if user add 'inner_entry' to SqType, pass argument 'entry_removed' = false.
* if user remove 'inner_entry' from SqType, pass argument 'entry_removed' = true.

#### 2.7 find & remove entry from dynamic SqType

use C language to find & remove SqEntry

```c
	SqEntry **entry_addr;

	entry_addr = sq_type_find_entry(type, "entry_name", NULL);
	sq_type_erase_entry_addr(type, entry_addr, 1);

	// remove entry from type and keep entry in memory
//	sq_type_steal_entry_addr(type, entry_addr, 1);

	sq_type_decide_size(type, NULL, true);
//	sq_type_decide_size(type, *entry_addr, true);
```

use C++ language to find & remove SqEntry

```c++
	Sq::Entry **entry_addr;

	entry_addr = type->findEntry(type, "entry_name");
	type->eraseEntry(type, entry_addr);

	// remove entry from type and keep entry in memory
//	type->stealEntry(type, entry_addr);

	type->decideSize(NULL, true);
//	type->decideSize(*entry_addr, true);
```

## 3 How to support new container type

User must implement 4 functions to support a new type and must handle data of SQXC_TYPE_XXXX in parser and writer of type.  
All container type like array, vector, or list...etc corresponds to SQXC_TYPE_ARRAY.  
All structured type corresponds to SQXC_TYPE_OBJECT.  

#### 3.1 declare SqType for container

e.g. MyList is list data type

```c++
// This is header file

/* MyList is a list data type

struct MyListNode {
	MyListNode *next;    // pointer to next MyListNode
	void       *data;    // pointer to element data
};

struct MyList {
	MyListNode *head;    // pointer to first MyListNode
}
 */

#ifdef __cplusplus    // mix C and C++
extern "C" {
#endif

// SqType_MyList_ is defined in source file.
extern const SqType                 SqType_MyList_

#define SQ_TYPE_MY_LIST           (&SqType_MyList_)

#ifdef __cplusplus    // mix C and C++
}  // extern "C"
#endif
```

#### 3.2 implement SqType interface for container

sqxc_push_nested() is used to push current SqType and it's instance to stack.  
sqxc_pop_nested()  is used to restore previous SqType and it's instance from stack.  
  
call sqxc_push_nested() when you parse SQXC_TYPE_OBJECT or SQXC_TYPE_ARRAY.  
call sqxc_pop_nested()  when you parse SQXC_TYPE_OBJECT_END or SQXC_TYPE_ARRAY_END.

```c++
// This is source file

static void sq_type_my_list_init(void *mylist, const SqType *type)
{
	// initialize MyList instance
}

static void sq_type_my_list_final(void *mylist, const SqType *type)
{
	// finalize MyList instance
}

static int  sq_type_my_list_parse(void *mylist, const SqType *type, Sqxc *src)
{
	SqxcValue    *xc_value = (SqxcValue*)src->dest;
	SqxcNested   *nested   = xc_value->nested;
	const SqType *element_type;

	// You can assign element type in SqType.entry
	element_type = (SqType*)type->entry;

	// Start of Container
	if (nested->data != mylist) {
		// do type match
		if (src->type != SQXC_TYPE_ARRAY)
			return (src->code = SQCODE_TYPE_NOT_MATCH);
		// ready to parse
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = mylist;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
	// End of Container
	// sqxc_value_send() have handled SQXC_TYPE_ARRAY_END. User doesn't need to handle it.

	// parse elements of MyList
	void *element = calloc(1, element_type->size);
	src->name = NULL;    // set "name" before calling parse()
	src->code = element_type->parse(element, element_type, src);
	my_list_append(mylist, element);
	return src->code;
}

static Sqxc *sq_type_my_list_write(void *mylist, const SqType *type, Sqxc *dest)
{
	const SqType *element_type;
	const char   *container_name = dest->name;

	// You can assign element type in SqType.entry
	element_type = (SqType*)type->entry;

	// Begin of Container
	dest->type = SQXC_TYPE_ARRAY;
//	dest->name = container_name;    // "name" was set by caller of this function
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	// output elements of MyList
	for (MyListNode *node = mylist->head;  node;  node = node->next) {
		void *element = node->data;
		dest->name = NULL;      // set "name" before calling write()
		dest = element_type->write(element, element_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	// End of Container
	dest->type = SQXC_TYPE_ARRAY_END;
	dest->name = container_name;
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	return dest;
}

// used by header file.
const SqType SqType_MyList_ =
{
	sizeof(MyList),                // size
	sq_type_my_list_init,          // init
	sq_type_my_list_final,         // final
	sq_type_my_list_parse,         // parse
	sq_type_my_list_write,         // write

	NULL,                          // name
	(SqEntry**) SQ_TYPE__yours_,   // entry   : You can assign element type in SqType.entry
	-1,                            // n_entry : SqType.entry isn't freed if SqType.n_entry == -1
	0,                             // bit_field
	NULL,                          // on_destroy
};
```

## 4 derived SqType (dynamic)

This defines new structure that derived from SqType.  
If you want add members in derived SqType, you can use SqType.on_destroy callback function to release them.

```c++
// If you use C language, please use 'typedef' to give a struct type a new name.
typedef struct MyType   MyType;

// structure that derived from SqType
#ifdef __cplusplus
struct MyType : Sq::TypeMethod        // <-- 1. inherit C++ member function(method)
#else
struct MyType
#endif
{
	SQ_TYPE_MEMBERS;                  // <-- 2. inherit member variable

	int   mydata;                     // <-- 3. Add variable and non-virtual function in derived struct.
	void *mypointer;

#ifdef __cplusplus
	// define C++ constructor and destructor here if you use C++ language.
	MyType() {
		my_type_init(this);
	}
	~MyType() {
		my_type_final(this);
	}
#endif
};

// This is callback function of destroy notifier.
void    my_type_on_destroy(MyType *mytype) {
	// release resource of derived struct
	free(mytype->mypointer);
}

void    my_type_init(MyType *type)
{
	sq_type_init_self((SqType*)mytype, 0, NULL);
	// initialize members of SqType here.
	// ...

	// set callback function of destroy notifier.
	// This notification will be emitted when sq_type_final_self() is called.
	mytype->on_destroy = my_type_on_destroy;

	// initialize variable of derived struct
	mytype->mydata = 10;
	mytype->mypointer = malloc(512);
}

void    my_type_final(MyType *type)
{
	// sq_type_final_self() will emit destroy notifier
	sq_type_final_self((SqType*)type);
}

SqType *my_type_new()
{
	MyType *mytype = malloc(sizeof(MyType));

	my_type_init(mytype);
	// return type is SqType, because MyType can be free by sq_type_free().
	return (SqType*)mytype;
}
```

## 5 free dynamic SqType

sq_type_free() can destroy dynamic SqType (SqType.bit_field has SQB_TYPE_DYNAMIC). It can emit destroy notifier.

```c++
	/* C function */
	sq_type_free(type);

	/* C++ method */
	delete type;
```
