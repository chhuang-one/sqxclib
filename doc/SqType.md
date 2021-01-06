# SqType

It define how to initialize, finalize, and convert C data type.
SqEntry use it to declare data type. Sqxc use it to convert data.

Build-in static SqType with it's C data type

| SqType          | C data type  |
|-----------------|--------------|
| SQ_TYPE_BOOL    | bool         |
| SQ_TYPE_INT     | int          |
| SQ_TYPE_UINT    | unsigned int |
| SQ_TYPE_INTPTR  | intptr_t     |
| SQ_TYPE_INT64   | int64_t      |
| SQ_TYPE_UINT64  | uint64_t     |
| SQ_TYPE_TIME    | time_t       |
| SQ_TYPE_DOUBLE  | double       |
| SQ_TYPE_STRING  | char*        |

| SqType             | C++ data type |
|--------------------|-------------- |
| SQ_TYPE_STD_STRING | std::string   |


### Define SqType with custom C data type
User can define a custom SqType statically or dynamically. for example:

First, we define a custom C data type:
```C
	typedef struct User     User;

	struct User {
		int    id;
		char*  name;
		char*  email;
	};
```

---
1. Define static SqType with 'unsorted' entries

use C99 designated initializer to define struct that has 'unsorted' entries.

```C
	// --- UserEntries is 'unsorted'
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

---
2. Define static SqType with 'sorted' entries

use C99 designated initializer to define struct that has 'sorted' entries.

```C
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
		// You can set SQB_TYPE_SORTED in SqType::bit_field
		// because SortedUserEntries have been 'sorted' by SqEntry::name.
	};
```

---
3. Define dynamic SqType with static entries

```C
	SqType*  type = sq_type_new(0, NULL);
	int   n_entry = sizeof(UserEntries) / sizeof(SqEntry*);

	for(int index;  index < n_entry;  index++)
		sq_type_add_entry(type, UserEntries[index], 1);
```

---
4. Define dynamic SqType

```C
	SqType*  type = sq_type_new(0, NULL);
	SqEntry* entry;

	entry = sq_entry_new(SQ_TYPE_INT);
	entry->name = strdup("id");
	entry->offset = offsetof(User, id);
	entry->bit_field = SQB_PRIMARY | SQB_HIDDEN;
	sq_type_add_entry(type, entry, 1);

	entry = sq_entry_new(SQ_TYPE_STRING);
	entry->name = strdup("name");
	entry->offset = offsetof(User, name);
	entry->bit_field = 0;
	sq_type_add_entry(type, entry, 1);

	entry = sq_entry_new(SQ_TYPE_STRING);
	sq_type_add_entry(type, entry, 1);
	entry->name = strdup("email");
	entry->offset = offsetof(User, email);
	entry->bit_field = SQB_HIDDEN_NULL;
	sq_type_add_entry(type, entry, 1);
```

