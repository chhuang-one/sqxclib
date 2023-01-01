[English](SqRelation.md)

# SqRelation

SqRelation 用于记录对象的关系。它目前仅供 SqdbSqlite 使用。
因为 SQLite 中没有 ALTER COLUMN，所以这个类可以记录用于重新创建表的关系。

## 初始化

因为每个 SqRelation 可以共享一个内存池来存储节点，所以在初始化的时候必须指定使用哪个内存池。

使用 C 语言

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

使用 C++ 语言

```c++
	capacity = 16;
	chunkSize = 128;
	SqRelationPool *pool = sq_relation_pool_create(chunkSize);

	// Sq::Relation 有构造函数和析构函数
	Sq::Relation  relationLocal;

	Sq::Relation *relation;
	relation = new Sq::Relation(pool, capacity);
	delete relation;
```

## 重置

clear() 可以清除当前实例中的所有关系。  
  
使用 C 语言

```c
	sq_relation_clear(relation);
```

使用 C++ 语言

```c++
	relation->clear();
```

## 添加关系

```c
// 此函数将 'to_object' 添加到 'from_object' 的相关列表中
// 它不检查 'from_object' 的相关列表中重复的 'to_object'
// 如果 'to_object' 为 NULL，它将添加 'from_object' 而不引用 'to_object'
// 如果 'no_reverse' == 1，不添加反向引用
// 如果 'no_reverse' == 0，添加反向引用（'to_object' 引用 'from_object'）
void  sq_relation_add(SqRelation *relation, const void *from_object, const void *to_object, int no_reverse);
```

## 删除关系

```c
// 如果 'to_object' 为 NULL，它将删除 'from_object' 引用的所有关系
// 如果 'no_reverse' == 1，不要触及反向引用
// 如果 'no_reverse' == 0，擦除反向引用
// 如果 'no_reverse' == -1，清除反向相关对象中的所有引用。 'to_object_free_func' 必须与此模式一起使用。
void  sq_relation_remove(SqRelation *relation, const void *from_object, const void *to_object, int no_reverse, SqDestroyFunc to_object_free_func);
```

## 替换关系

```c
// 它将 'old_object' 替换为 'new_object'。
// 如果 'no_reverse' == 1，不要替换反向引用中的 'old_object'
// 如果 'no_reverse' == 0，替换反向引用中的 'old_object'
void  sq_relation_replace(SqRelation *relation, const void *old_object, const void *new_object, int no_reverse);
```

## 查找关系

```c
// 如果没有找到关系则返回 NULL。 用户不能释放返回值。
// 如果 'to_object' 为NULL，它返回 'from_object' 对象的 SqRelationNode。
SqRelationNode *sq_relation_find(SqRelation *relation, const void *from_object, const void *to_object);
```
