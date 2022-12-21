[English](SqEntry.md)

# SqEntry
SqEntry 定义结构类型的字段。它必须使用 [SqType](SqType.cn.md) 来定义字段的数据类型。

	SqEntry
	│
	└─── SqReentry
	     │
	     ├─── SqTable
	     │
	     └─── SqColumn

结构定义：

```c
struct SqEntry
{
	const SqType *type;        // 字段类型
	const char   *name;        // 字段名称
	size_t        offset;      // 结构中字段的偏移量
	unsigned int  bit_field;   // 下面解释
};

// 因为使用 'const' 来定义字符串 'char*' 和类型 'SqType*'，
// 所以 C++ 用户可以轻松初始化静态 SqEntry。
```

声明 SqEntry 的 bit_field

| 名称            | 描述                                          | 
| --------------- | --------------------------------------------- |
| SQB_DYNAMIC     | entry 可以更改和释放                          |
| SQB_POINTER     | entry 的实例是指针                            |
| SQB_HIDDEN      | (JSON) 转换器不会输出该条目的值               |
| SQB_HIDDEN_NULL | 如果值为 NULL，(JSON) 转换器将不会输出        |

* SQB_DYNAMIC 仅供内部使用。用户不应设置或清除该位。
* 如果 SqEntry.bit_field 没有设置 SQB_DYNAMIC，用户不能更改或释放 SqEntry。
* 用户必须使用位运算符来设置或清除 SqEntry.bit_field 中的位。
* 最好将常量(或静态)的 SqEntry 与常量(或静态)的 SqType 一起使用。
* 动态 SqEntry 可以与动态、常量或静态 SqType 一起使用。

## 定义结构化数据类型
您可以查看 [SqType](SqType.cn.md) 以获取更多样本。

#### 1 定义由常量 SqType 使用的 SqEntry 常量指针数组
* 注意: 如果为结构定义了常量 SqType，它必须与 SqEntry 的**指针数组**一起使用。

```c
static const SqEntry  entryArray[2] = {
	{SQ_TYPE_UINT,   "bit_field",  offsetof(YourStruct, bit_field),  0},
	{SQ_TYPE_STR,    "name",       offsetof(YourStruct, name),       SQB_HIDDEN_NULL},
};

static const SqEntry *entryPointerArray[2] = {
	& entryArray[0],
	& entryArray[1],
};

/* 如果 'entryPointerArray' 未按名称排序，则将 0 传递给最后一个参数。
   否则使用 SQB_TYPE_SORTED 替换最后一个参数中的 0。
 */
const SqType type = SQ_TYPE_INITIALIZER(YourStruct, entryPointerArray, 0);
```

#### 2 定义由动态 SqType 使用的 SqEntry 常量数组

e.g. 创建使用常量 SqEntry 的动态 SqType

使用 C 语言

```c
	SqType *type = sq_type_new(0, (SqDestroyFunc)sq_entry_free);

	// 从指针数组中添加 2 个条目
	sq_type_add_entry_ptrs(type, entryPointerArray, 2);

	// 从数组中添加 2 个条目（不是指针数组）
//	sq_type_add_entry(type, entryArray, 2, 0);
```

使用 C++ 语言

```c++
	Sq::Type *type = new Sq::Type(0, sq_entry_free);

	// 从指针数组中添加 2 个条目
	type->addEntry(entryPointerArray, 2);

	// 从数组中添加 2 个条目（不是指针数组）
//	type->addEntry(entryArray, 2);
```

#### 3 创建由动态 SqType 使用的动态 SqEntry

使用 C 语言

```c
	SqEntry *entry = sq_entry_new(SQ_TYPE_STR);

	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // 设置 SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // 清除 SqEntry.bit_field

	sq_type_add_entry(type, entry, 1, 0);
```

使用 C++ 语言

```c++
	Sq::Entry *entry = new Sq::Entry(SQ_TYPE_STR);

	entry->name = strdup("name");
	entry->offset = offsetof(YourStruct, name);
	entry->bit_field |= SQB_HIDDEN_NULL;    // 设置 SqEntry.bit_field
//	entry->bit_field &= ~SQB_HIDDEN_NULL;   // 清除 SqEntry.bit_field

	type->addEntry(entry);
```
