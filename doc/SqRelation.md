[中文](SqRelation.cn.md)

# SqRelation

SqRelation is used to record relation of object. It is currently used by SqdbSqlite only.
Because there is no ALTER COLUMN in SQLite, this class can record the relationship used to recreate the table.

## Initialize

Because each SqRelation can share a memory pool to store nodes, it must to specify which memory pool to use when initializing.

use C language

```c
	capacity = 16;
	chunkSize = 128;
	SqRelationPool *pool = sq_relation_pool_create(chunkSize);

	SqRelation     relationLocal;
	sq_relation_init(&relationLocal, pool, capacity);
	sq_relation_final(&relationLocal);

	SqRelation    *relation;
	relation = sq_relation_new(pool, capacity);
	sq_relation_free(relation);
```

use C++ language

```c++
	capacity = 16;
	chunkSize = 128;
	SqRelationPool *pool = sq_relation_pool_create(chunkSize);

	// Sq::Relation has constructor and destructor
	Sq::Relation  relationLocal;

	Sq::Relation *relation;
	relation = new Sq::Relation(pool, capacity);
	delete relation;
```

## Reset

clear() can clear all relation in current instance.  
  
use C language

```c
	sq_relation_clear(relation);
```

use C++ language

```c++
	relation->clear();
```

## Add relation

```c
// This function prepend 'to_object' to related list of 'from_object'
// it does NOT check duplicate 'to_object' in related list of 'from_object'
// if 'to_object' is NULL, it will add 'from_object' without reference to 'to_object'
// if 'no_reverse' ==  1, don't add reverse reference
// if 'no_reverse' ==  0, add reverse reference ('to_object' reference to 'from_object')
void  sq_relation_add(SqRelation *relation, const void *from_object, const void *to_object, int no_reverse);
```

## Remove relation

```c
// if 'to_object' is NULL, it will erase all relation that referenced by 'from_object'
// if 'no_reverse' ==  1, don't touch reverse reference
// if 'no_reverse' ==  0, erase reverse reference
// if 'no_reverse' == -1, erase all references in reverse related objects. 'to_object_free_func' must use with this mode.
void  sq_relation_remove(SqRelation *relation, const void *from_object, const void *to_object, int no_reverse, SqDestroyFunc to_object_free_func);
```

## Remove empty relation

remove object that doesn't reference to any object.

```c
void  sq_relation_remove_empty(SqRelation *relation);
```

## Replace relation

```c
// it replace 'old_object' to 'new_object'.
// if 'no_reverse' ==  1, don't replace 'old_object' in reverse reference
// if 'no_reverse' ==  0, replace 'old_object' in reverse reference
void  sq_relation_replace(SqRelation *relation, const void *old_object, const void *new_object, int no_reverse);
```

## Find relation

sq_relation_find() can get related list of object. Return type SqRelationNode is singly linked list.

```c
// return NULL if relation not found. User can NOT free returned value.
// if 'to_object' is NULL, it return SqRelationNode of 'from_object' object.
SqRelationNode *sq_relation_find(SqRelation *relation, const void *from_object, const void *to_object);
```
