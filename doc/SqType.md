[中文](SqType.cn.md)

# SqType

It define how to initialize, finalize, and convert C data type.  
[Sqxc](Sqxc.md) use it to convert data between C language and SQL, JSON, etc. [SqEntry](SqEntry.md) and its derived structures use it to define data type.

	SqType
	│
	└─── SqCommand    (define in sqxcsupport library)

Structure Definition:

```c
struct SqType
{
	unsigned int   size;           // instance size

	SqTypeFunc     init;           // initialize instance
	SqTypeFunc     final;          // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to generate type name,
	// or use macro SQ_GET_TYPE_NAME()
	const char    *name;

	// SqType::entry is array of SqEntry pointer if current SqType is for C struct type.
	// SqType::entry isn't freed if SqType::n_entry == -1
	SqEntry      **entry;          // maps to SqPtrArray::data
	unsigned int   n_entry;        // maps to SqPtrArray::length
	// *** About above 2 fields:
	// 1. They are expanded by macro SQ_ARRAY_MEMBERS(SqEntry*, entry, n_entry)
	// 2. They can NOT change data type and order.

	// SqType::bit_field has SQB_TYPE_DYNAMIC if SqType is dynamic and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED  if SqType::entry is sorted.
	unsigned int   bit_field;

	// SqType::on_destroy() is called when program releases SqType.
	// This is mainly used for deriving or customizing SqType.
	// Instance of SqType will be passed to SqType::on_destroy()
	SqDestroyFunc  on_destroy;     // destroy notifier for SqType. It can be NULL.
};
```

Define bit_field of SqType:

| name                  | description                              |
| --------------------- | ---------------------------------------- |
| SQB_TYPE_DYNAMIC      | type can be changed and freed            |
| SQB_TYPE_SORTED       | SqType::entry is sorted by SqEntry::name |
| SQB_TYPE_QUERY_FIRST  | SqType::entry has query-only columns     |

* When user call function to creates or initializes a SqType, SqType::bit_field is set to SQB_TYPE_DYNAMIC.
* User can NOT change or free SqType if SqType::bit_field has NOT set SQB_TYPE_DYNAMIC.
* User must use bitwise operators to set or clear bits in SqType::bit_field.
* Constant SqType must be used with constant SqEntry.
* Dynamic SqType can use with dynamic or constant SqEntry.

## 0 Library-provided SqType

Built-in SqType with it's data type:

| SqType          | C data type  | SQL data type     | description                      |
| --------------- | ------------ | ----------------- | -------------------------------- |
| SQ_TYPE_BOOL    | bool         | BOOLEAN           |                                  |
| SQ_TYPE_BOOLEAN | bool         | BOOLEAN           | alias of SQ_TYPE_BOOL            |
| SQ_TYPE_INT     | int          | INT               |                                  |
| SQ_TYPE_INTEGER | int          | INT               | alias of SQ_TYPE_INT             |
| SQ_TYPE_UINT    | unsigned int | INT (UNSIGNED)    |                                  |
| SQ_TYPE_INT64   | int64_t      | BIGINT            |                                  |
| SQ_TYPE_UINT64  | uint64_t     | BIGINT (UNSIGNED) |                                  |
| SQ_TYPE_TIME    | time_t       | TIMESTAMP         |                                  |
| SQ_TYPE_DOUBLE  | double       | DOUBLE            |                                  |
| SQ_TYPE_STR     | char*        | VARCHAR           |                                  |
| SQ_TYPE_STRING  | char*        | VARCHAR           | alias of SQ_TYPE_STR             |
| SQ_TYPE_CHAR    | char*        | CHAR              | defined for SQL data type CHAR   |

* When using above SqType, user don't need to specify SQL data type because they will map to specific SQL data type by default.
* Different Database products may map these C data types to different SQL data types.

SqType with it's C/C++ container type:

| SqType                 | C data type    | C++ data type  | description                      |
| ---------------------- | -------------- | -------------- | -------------------------------- |
| SQ_TYPE_ARRAY          | SqArray        | Sq::Array      | array of arbitrary elements      |
| SQ_TYPE_INT_ARRAY      | SqIntArray     | Sq::IntArray   | integer array (used by JSON)     |
| SQ_TYPE_PTR_ARRAY      | SqPtrArray     | Sq::PtrArray   | pointer array                    |
| SQ_TYPE_STR_ARRAY      | SqStrArray     | Sq::StrArray   | string  array (used by JSON)     |

SqType with it's C/C++ binary data type:

| SqType                 | C data type    | C++ data type  | description                      |
| ---------------------- | -------------- | -------------- | -------------------------------- |
| SQ_TYPE_BUFFER         | SqBuffer       | Sq::Buffer     | It can map to SQL data type BLOB |
| SQ_TYPE_BINARY         | SqBuffer       | Sq::Buffer     | alias of SQ_TYPE_BUFFER          |
| SQ_TYPE_BLOB           | SqBuffer       | Sq::Buffer     | alias of SQ_TYPE_BUFFER          |

SqType with it's C++ data type:

| SqType                  | C++ data type            | description                       |
| ----------------------- | ------------------------ | --------------------------------- |
| SQ_TYPE_STD_STRING      | std::string              | map to SQL data type CHAR, TEXT   |
| SQ_TYPE_STD_STR         | std::string              | alias of SQ_TYPE_STD_STRING       |
| SQ_TYPE_STD_VECTOR      | std::vector<char>        | map to SQL data type BLOB         |
| SQ_TYPE_STD_VEC         | std::vector<char>        | alias of SQ_TYPE_STD_VECTOR       |
| SQ_TYPE_STD_VECTOR_SIZE | std::vector<char> resize | specify size of std::vector<char> |
| SQ_TYPE_STD_VEC_SIZE    | std::vector<char> resize | alias of SQ_TYPE_STD_VECTOR_SIZE  |
| Sq::TypeStl<Container>  | STL containers           | create SqType for STL container   |

SqType without instance:  
  
The following SqType do not require instance, and only SQ_TYPE_UNKNOWN (alias of SQ_TYPE_FAKE6) provides SqType's parse and write functions.

| SqType                  | alias                    | description                       |
| ----------------------- | ------------------------ | --------------------------------- |
| SQ_TYPE_FAKE0           | SQ_TYPE_CONSTRAINT       | used by database migration        |
| SQ_TYPE_FAKE1           | SQ_TYPE_INDEX            | used by database migration        |
| SQ_TYPE_FAKE2           |                          |                                   |
| SQ_TYPE_FAKE3           |                          |                                   |
| SQ_TYPE_FAKE4           | SQ_TYPE_TRACING          | used by SQLite migration          |
| SQ_TYPE_FAKE5           | SQ_TYPE_UNSYNCED         | used by SQLite migration          |
| SQ_TYPE_FAKE6           | SQ_TYPE_REENTRY          | used by SQLite migration          |
| SQ_TYPE_UNKNOWN         | SQ_TYPE_FAKE6            | Sqxc use it to skip unknown entry |

## 1 Define primitive data types

Primitive data types are like integer and float and are not structure or class. Please refer source code SqType-built-in.c to get more example.

#### 1.1 Define primitive data type with constant SqType

To define constant SqType, use C99 designated initializer or C++ aggregate initialization.

```
const SqType type_int = {
	.size  = sizeof(int),
	.init  = NULL,                  // initialize function
	.final = NULL,                  // finalize function
	.parse = int_parse_function,    // Function that parse data from Sqxc instance
	.write = int_write_function,    // Function that write data to   Sqxc instance
};
```

#### 1.2 Define primitive data type with dynamic SqType

Using sq_type_new() to create dynamic primitive data type for SqType.
Function sq_type_new() declarations:

```c++
// prealloc_size : capacity of SqType::entry (SqType::entry is SqEntry pointer array).
// entry_destroy_func : DestroyFunc of SqType::entry's elements.

SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);
```

e.g. Create primitive data type for SqType.

```c++
	SqType *type;

	// 1. Create SqType with argument 'prealloc_size' = -1, 'entry_destroy_func' = NULL.

	/* C function */
	type = sq_type_new(-1, NULL);

	/* C++ method */
//	type = new Sq::Type(-1, NULL);

	// 2. Specify size, and the functions init, final, parse, and write in the SqType structure.
	type->size  = sizeof(int);
	type->init  = NULL;                  // initialize function
	type->final = NULL;                  // finalize function
	type->parse = int_parse_function;    // Function that parse data from Sqxc instance
	type->write = int_write_function;    // Function that write data to   Sqxc instance
```

## 2 Define structured data type

If you define SqType for structure data type, use SqEntry to define fields in structure.  
  
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

#### 2.1 Define structured data type with constant SqType

Like primitive data types, constant SqType must use C99 designated initializer or C++ aggregate initialization.  
Using constant SqType to define structured data type must be used with constant **pointer array** of SqEntry.
  
To get name of structured data type in C and C++ code, use the macro SQ_GET_TYPE_NAME(Type).  
Note: You will get different type name from C and C++ source code when you use gcc to compile because gcc's typeid(Type).name() will return strange name.  
  
If pointer array of SqEntry has sorted by name, then SqType::bit_field is set to SQB_TYPE_SORTED.  
  
e.g. constant SqType use "sorted" constant pointer array of SqEntry

```c
/* sortedEntryPointers is "sorted" entryPointers (sorted by name) */
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

	.entry   = sortedEntryPointers,
	.n_entry = sizeof(sortedEntryPointers) / sizeof(SqEntry*),
	.bit_field = SQB_TYPE_SORTED,
	// Because sortedEntryPointers have been sorted by SqEntry::name,
	// you can set SQB_TYPE_SORTED in SqType::bit_field
};
```

e.g. constant SqType use "unsorted" constant pointer array of SqEntry  
  
SqType::bit_field set to 0 because pointer array of SqEntry is not sorted by name.

```c
const SqType unsortedTypeUser = {
	/*  Omit other codes...  */

	.bit_field = 0,
};
```

There are macros SQ_TYPE_INITIALIZER() and SQ_TYPE_INITIALIZER_FULL() can define constant SqType simplely.

```c
/* Because sortedEntryPointers   is "sorted"   entries, you can pass SQB_TYPE_SORTED to the last argument. */
const SqType  sortedTypeUserM   = SQ_TYPE_INITIALIZER(User, sortedEntryPointers,   SQB_TYPE_SORTED);

/* Because unsortedEntryPointers is "unsorted" entries, you can pass 0 to the last argument. */
const SqType  unsortedTypeUserM = SQ_TYPE_INITIALIZER(User, unsortedEntryPointers, 0);
```

#### 2.2 Define structured data type with dynamic SqType

Dynamic SqType can add both dynamic or constant SqEntry (or derived structure of SqEntry).  
The C function sq_type_add_entry_ptrs() and its C++ method addEntry() can add **pointer array** of SqEntry.

use C language to add **pointer array** of SqEntry.

```c
	SqType  *type;
	unsigned int  n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	sq_type_add_entry_ptrs(type, entryPointers, n_entry);
```

use C++ language to add **pointer array** of SqEntry.

```c++
	Sq::Type  *type;
	unsigned int  n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = new Sq::Type(8, sq_entry_free);

	type->addEntry(entryPointers, n_entry);
```

The C function sq_type_add_entry() and its C++ method addEntry() can add single or multiple SqEntry instances.
The last parameter of sq_type_add_entry() is size of SqEntry instance. It is used to add an array of SqEntry derived structures because the function needs size of the derived structure to determine element size. If not a derived structure, set to 0.  
  
use C language to add SqEntry instance (or instance array):

```c
	SqType  *type;
	SqEntry *entry;

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	entry = sq_entry_new(SQ_TYPE_STR);
	sq_entry_set_name(entry, "name");
	entry->offset = offsetof(User, name);

	sq_type_add_entry(type, entry, 1, 0);
```

use C++ language to add SqEntry instance (or instance array):

```c++
	Sq::Type  *type;
	Sq::Entry *entry;

	type = new Sq::Type(8, sq_entry_free);

	entry = new Sq::Entry(SQ_TYPE_STR);
	entry->setName("name");
	entry->offset = offsetof(User, name);

	type->addEntry(entry);
```

#### 2.3 calculate instance size for dynamic structured data type

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

#### 2.4 find & remove entry from dynamic SqType

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

#### 2.5 Copy SqType

Because constant SqType can't be modified directly, user must copy it before modifying.  
function sq_type_copy() can copy SqType. It's declarations:

```c
SqType  *sq_type_copy(SqType *type_dest, const SqType *type_src,
                      SqDestroyFunc entry_free_func,
                      SqCopyFunc    entry_copy_func);
```

It copy data from 'type_src' to 'type_dest', 'type_dest' must be raw memory.  
If 'entry_copy_func' is NULL, 'type_dest' will share SqEntry instances from 'type_src'.  
If 'type_dest' is NULL, it will allocate memory for 'type_dest'.  
return 'type_dest' or newly created SqType.  
  
use C language to copy SqType

```c
	// copy constant SqType - SQ_TYPE_ARRAY
	// This is mainly used to modify constant SqType.
	type = sq_type_copy(NULL, SQ_TYPE_ARRAY,
	                    (SqDestroyFunc) NULL,
	                    (SqCopyFunc)    NULL);

	// copy constant SqType from SqTable::type
	// The new instance 'type' shares SqColumn instances in 'table->type'
	// This is mainly used to modify constant SqType (from SqTable::type) when migrating.
	type = sq_type_copy(NULL, table->type,
	                    (SqDestroyFunc) sq_column_free,
	                    (SqCopyFunc)    NULL);

	// full copy SqType from SqTable::type
	// The new instance 'type' copies SqColumn instances from 'table->type'
	type = sq_type_copy(NULL, table->type,
	                    (SqDestroyFunc) sq_column_free,
	                    (SqCopyFunc)    sq_column_copy);
```

#### 2.6 Change dynamic to constant

C function sq_type_use_constant(), C++ method useConstant() can clear SQB_TYPE_DYNAMIC from SqType::bit_field.
These are typically used with global SqType.  
  
use C language

```c
	// 'globalType' is a global variable and not a constant.
	SqType *type = globalType;

	// initialize
	sq_type_init_self(type, 0, NULL);
	sq_type_use_constant(type);
```

use C++ language

```c++
	// 'globalType' is a global variable and not a constant.
	Sq::Type *type = globalType;

	// initialize
	type->initSelf(0, NULL);
	type->useConstant();
```

## 3 How to support new container type

User must implement 4 functions to support a new type and must handle data of SQXC_TYPE_XXXX in parser and writer of type.  
All container type like array, vector, list, etc. corresponds to SQXC_TYPE_ARRAY.  
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

// sqType_MyList is defined in source file.
extern const SqType                 sqType_MyList

#define SQ_TYPE_MY_LIST           (&sqType_MyList)

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

	// You can assign element type in SqType::entry
	element_type = (SqType*)type->entry;

	// Start of Container
	if (nested->data != mylist) {
		// do type match
		if (src->type != SQXC_TYPE_ARRAY)
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
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

	// You can assign element type in SqType::entry
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
const SqType sqType_MyList =
{
	sizeof(MyList),                // size
	sq_type_my_list_init,          // init
	sq_type_my_list_final,         // final
	sq_type_my_list_parse,         // parse
	sq_type_my_list_write,         // write

	NULL,                          // name
	(SqEntry**) SQ_TYPE__yours_,   // entry   : You can assign element type in SqType::entry
	-1,                            // n_entry : SqType::entry isn't freed if SqType::n_entry == -1
	0,                             // bit_field
	NULL,                          // on_destroy
};
```

## 4 derived SqType (dynamic)

This defines new structure that derived from SqType.  
If you want add members in derived SqType, you can use SqType::on_destroy callback function to release them.

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

sq_type_free() can destroy dynamic SqType (SqType::bit_field has SQB_TYPE_DYNAMIC). It can emit destroy notifier.

```c++
	/* C function */
	sq_type_free(type);

	/* C++ method */
	delete type;
```

## 6 Output instance to Sqxc chain

User can call SqType::write() to output instance to Sqxc chain.  
  
e.g. output MyList (container type defined above) instance to Sqxc chain.  
  
use C functions

```c
	MyList *instance = list;
	Sqxc   *xc       = (Sqxc*)xcJson;
	SqType *type     = SQ_TYPE_MY_LIST;

	type->write(instance, type, xc);
```

use C++ methods

```c++
	MyList   *instance = list;
	Sq::Xc   *xc       = (Sq::Xc*)xcJson;
	Sq::Type *type     = SQ_TYPE_MY_LIST;

	type->write(instance, type, xc);
```
