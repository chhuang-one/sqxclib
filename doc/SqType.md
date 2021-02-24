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

### Define SqType with custom data type
User can define a constant or dynamic SqType. for example:

First, we define a custom data type:
```c
	typedef struct User     User;

	struct User {
		int    id;
		char*  name;
		char*  email;
	};
```

##### 1. Define constant SqType with constant 'unsorted' entries

use C99 designated initializer to define struct that has 'unsorted' entries.

```c
	// --- UserEntries is 'unsorted' SqEntry pointer array
	static const SqEntry  *UserEntries[] = {
		&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
		&(SqEntry) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
		&(SqEntry) {SQ_TYPE_STRING, "email", offsetof(User, email), SQB_HIDDEN_NULL},
	};

	// --- UserType use 'unsorted' UserEntries
	const SqType UserType = {
		.size  = sizeof(User),
		.init  = NULL,
		.final = NULL,
		.parse = sq_type_object_parse,
		.write = sq_type_object_write,
		.name  = SQ_GET_TYPE_NAME(User),
		.entry   = UserEntries,
		.n_entry = sizeof(UserEntries) / sizeof(SqEntry*),
		.bit_field = 0,
	};
```

##### 2. Define constant SqType with constant 'sorted' entries

use C99 designated initializer to define struct that has 'sorted' entries.

```c
	// --- SortedUserEntries is 'sorted' UserEntries (sorted by name)
	static const SqEntry  *SortedUserEntries[] = {
		&(SqEntry) {SQ_TYPE_STRING, "email", offsetof(User, email), SQB_HIDDEN_NULL},
		&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
		&(SqEntry) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
	};

	// --- SortedUserType use SortedUserEntries (set SQB_TYPE_SORTED in SqType::bit_field)
	const SqType SortedUserType = {
		.size  = sizeof(User),
		.init  = NULL,
		.final = NULL,
		.parse = sq_type_object_parse,
		.write = sq_type_object_write,
		.name  = SQ_GET_TYPE_NAME(User),

		// --- The following is the difference from UserType
		.entry   = SortedUserEntries,
		.n_entry = sizeof(SortedUserEntries) / sizeof(SqEntry*),
		.bit_field = SQB_TYPE_SORTED,
		// Because SortedUserEntries have been 'sorted' by SqEntry::name,
		// you can set SQB_TYPE_SORTED in SqType::bit_field
	};
```

##### 3. Define constant SqType with constant entries (use C macro)

use macro SQ_TYPE_INITIALIZER() and SQ_TYPE_INITIALIZER_FULL()
```c
	// UserEntries is 'unsorted' entries that defined in above example.
	const SqType  UserTypeM = SQ_TYPE_INITIALIZER(User, UserEntries, 0);

	// SortedUserEntries is 'sorted' entries that defined in above example.
	const SqType  SortedUserTypeM = SQ_TYPE_INITIALIZER(User, SortedUserEntries, SQB_TYPE_SORTED);
```

##### 4. Define dynamic SqType with constant entries

use sq_type_add_entry_ptrs() to add static SqEntry pointer array.
```c
	SqType*  type = sq_type_new(0, NULL);
	int   n_entry = sizeof(UserEntries) / sizeof(SqEntry*);

	sq_type_add_entry_ptrs(type, UserEntries, n_entry);
```

##### 5. Define dynamic SqType with dynamic entries

use sq_type_add_entry() to add dynamic SqEntry array.
```c
	SqType*  type = sq_type_new(0, NULL);
	SqEntry* entry;

	entry = sq_entry_new(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field = SQB_PRIMARY | SQB_HIDDEN;
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	entry->bit_field = 0;
	sq_type_add_entry(type, entry, 1, 0);

	entry = sq_entry_new(SQ_TYPE_STRING);
	sq_type_add_entry(type, entry, 1, 0);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	entry->bit_field = SQB_HIDDEN_NULL;
	sq_type_add_entry(type, entry, 1, 0);
```
