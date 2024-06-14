[English](SqType.md)

# SqType

它定义了如何初始化、终结和转换 C 数据类型。  
[Sqxc](Sqxc.cn.md) 使用它在 C 语言和 SQL、JSON 等之间转换数据。[SqEntry](SqEntry.cn.md) 及其派生结构使用它来定义数据类型。

	SqType
	│
	└─── SqCommand    （在 sqxcsupport 库中定义）

结构定义：

```c
struct SqType
{
	unsigned int   size;           // 实例大小

	SqTypeFunc     init;           // 初始化实例
	SqTypeFunc     final;          // 终结实例

	SqTypeParseFunc   parse;       // 将 Sqxc(SQL/JSON) 数据解析为实例
	SqTypeWriteFunc   write;       // 将实例数据写入 Sqxc(SQL/JSON)

	// 在 C++ 中，必须使用 typeid(TypeName).name() 来生成类型名称，
	// 或者使用宏 SQ_GET_TYPE_NAME()
	const char    *name;

	// 如果当前 SqType 用于 C 结构类型，则 SqType::entry 是 SqEntry 指针数组。
	// 如果 SqType::n_entry == -1，     则 SqType::entry 不会被释放。
	SqEntry      **entry;          // 映射到 SqPtrArray::data
	int            n_entry;        // 映射到 SqPtrArray::length
	// *** 关于以上 2 个字段：
	// 1. 它们由宏 SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry) 展开
	// 2. 他们不能更改数据类型和顺序。

	// 如果 SqType 是动态且可释放的，则 SqType::bit_field 具有 SQB_TYPE_DYNAMIC。
	// 如果 SqType::entry 已排序，   则 SqType::bit_field 具有 SQB_TYPE_SORTED。
	unsigned int   bit_field;

	// SqType::on_destroy() 在程序释放 SqType 时被调用。
	// 这主要用于派生或定制 SqType。
	// SqType 的实例将传递给 SqType::on_destroy()
	SqDestroyFunc  on_destroy;     // 销毁 SqType 的通知程序。它可以是 NULL。
};
```

定义 SqType 的 bit_field：

| 名称                  | 描述                                   |
| --------------------- | -------------------------------------- |
| SQB_TYPE_DYNAMIC      | 类型可以改变和释放                     |
| SQB_TYPE_SORTED       | SqType::entry 按照 SqEntry::name 排序  |
| SQB_TYPE_QUERY_FIRST  | SqType::entry 具有仅查询列             |

* 当用户调用函数创建或初始化 SqType 时，SqType::bit_field 设置为 SQB_TYPE_DYNAMIC。
* 如果 SqType::bit_field 没有设置 SQB_TYPE_DYNAMIC，用户不能更改或释放 SqType。
* 用户必须使用位运算符来设置或清除 SqType::bit_field 中的位。
* 常量 SqType 必须与常量 SqEntry 一起使用。
* 动态 SqType 可以与动态或常量 SqEntry 一起使用。

## 0 库提供的 SqType

内置 SqType 及其数据类型：

| SqType          | C 数据类型   | SQL 数据类型      | 描述                         |
| --------------- | ------------ | ----------------- | ---------------------------- |
| SQ_TYPE_BOOL    | bool         | BOOLEAN           |                              |
| SQ_TYPE_BOOLEAN | bool         | BOOLEAN           | SQ_TYPE_BOOL 的别名          |
| SQ_TYPE_INT     | int          | INT               |                              |
| SQ_TYPE_INTEGER | int          | INT               | SQ_TYPE_INT 的别名           |
| SQ_TYPE_UINT    | unsigned int | INT (UNSIGNED)    |                              |
| SQ_TYPE_INT64   | int64_t      | BIGINT            |                              |
| SQ_TYPE_UINT64  | uint64_t     | BIGINT (UNSIGNED) |                              |
| SQ_TYPE_TIME    | time_t       | TIMESTAMP         |                              |
| SQ_TYPE_DOUBLE  | double       | DOUBLE            |                              |
| SQ_TYPE_STR     | char*        | VARCHAR           |                              |
| SQ_TYPE_STRING  | char*        | VARCHAR           | SQ_TYPE_STR 的别名           |
| SQ_TYPE_CHAR    | char*        | CHAR              | 为 SQL 数据类型 CHAR 定义    |

* 使用上述 SqType 时，用户可以不指定 SQL 数据类型，因为它们默认会映射到特定的 SQL 数据类型。
* 不同的数据库产品可能将这些 C 数据类型对应到不同的 SQL 数据类型。

SqType 及其 C/C++ 容器类型：

| SqType                 | C 数据类型     | C++ 数据类型   | 描述                         |
| ---------------------- | -------------- | -------------- | ---------------------------- |
| SQ_TYPE_ARRAY          | SqArray        | Sq::Array      | 任意元素的数组               |
| SQ_TYPE_INT_ARRAY      | SqIntArray     | Sq::IntArray   | 整数数组  （主要用于 JSON）  |
| SQ_TYPE_PTR_ARRAY      | SqPtrArray     | Sq::PtrArray   | 指针数组                     |
| SQ_TYPE_STR_ARRAY      | SqStrArray     | Sq::StrArray   | 字符串数组（主要用于 JSON）  |

SqType 及其 C/C++ 二进制数据类型：

| SqType                 | C 数据类型     | C++ 数据类型   | 描述                         |
| ---------------------- | -------------- | -------------- | ---------------------------- |
| SQ_TYPE_BUFFER         | SqBuffer       | Sq::Buffer     | 可以映射到 SQL 数据类型 BLOB |
| SQ_TYPE_BINARY         | SqBuffer       | Sq::Buffer     | SQ_TYPE_BUFFER 的别名        |
| SQ_TYPE_BLOB           | SqBuffer       | Sq::Buffer     | SQ_TYPE_BUFFER 的别名        |

SqType 及其 C++ 数据类型：

| SqType                  | C++ 数据类型             | 描述                           |
| ----------------------- | ------------------------ | ------------------------------ |
| SQ_TYPE_STD_STRING      | std::string              | 映射到 SQL 数据类型 CHAR, TEXT |
| SQ_TYPE_STD_STR         | std::string              | SQ_TYPE_STD_STRING 的别名      |
| SQ_TYPE_STD_VECTOR      | std::vector<char>        | 映射到 SQL 数据类型 BLOB       |
| SQ_TYPE_STD_VEC         | std::vector<char>        | SQ_TYPE_STD_VECTOR 的别名      |
| SQ_TYPE_STD_VECTOR_SIZE | std::vector<char> resize | 指定 std::vector<char> 的大小  |
| SQ_TYPE_STD_VEC_SIZE    | std::vector<char> resize | SQ_TYPE_STD_VECTOR_SIZE 的别名 |
| Sq::TypeStl<Container>  | STL 容器                 | 为 STL 容器创建 SqType         |

没有实例的 SqType：  
  
以下 SqType 不需要实例，只有 SQ_TYPE_UNKNOWN (SQ_TYPE_FAKE6 的别名) 提供了 SqType 的解析和写入函数。

| SqType                  | 别名                     | 描述                           |
| ----------------------- | ------------------------ | ------------------------------ |
| SQ_TYPE_FAKE0           | SQ_TYPE_CONSTRAINT       | 数据库迁移时使用               |
| SQ_TYPE_FAKE1           | SQ_TYPE_INDEX            | 数据库迁移时使用               |
| SQ_TYPE_FAKE2           |                          |                                |
| SQ_TYPE_FAKE3           |                          |                                |
| SQ_TYPE_FAKE4           | SQ_TYPE_TRACING          | SQLite 迁移时使用              |
| SQ_TYPE_FAKE5           | SQ_TYPE_UNSYNCED         | SQLite 迁移时使用              |
| SQ_TYPE_FAKE6           | SQ_TYPE_REENTRY          | SQLite 迁移时使用              |
| SQ_TYPE_UNKNOWN         | SQ_TYPE_FAKE6            | Sqxc 用它来跳过未知条目        |

## 1 定义原始数据类型

原始数据类型类似于整数和浮点数，并且不是结构或类。请参考源代码 SqType-built-in.c 以获得更多示例。

#### 1.1 使用常量 SqType 定义原始数据类型

使用 C99 指定初始化器或 C++ 聚合初始化来定义常量 SqType。

```
const SqType type_int = {
	.size  = sizeof(int),
	.init  = NULL,                  // 初始化函数
	.final = NULL,                  // 终结函数
	.parse = int_parse_function,    // 从 Sqxc 实例解析数据的函数
	.write = int_write_function,    // 将数据写入 Sqxc 实例的函数
};
```

#### 1.2 使用动态 SqType 定义原始数据类型

使用 sq_type_new() 为 SqType 创建动态原始数据类型。
函数 sq_type_new() 声明：

```c++
// prealloc_size：SqType::entry 的容量（SqType::entry 是 SqEntry 指针数组）。
// entry_destroy_func：SqType::entry 元素的 DestroyFunc。

SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);
```

例如: 为 SqType 创建原始数据类型。

```c++
	SqType *type;

	// 1. 创建 SqType 并传递参数 'prealloc_size' = -1、'entry_destroy_func' = NULL。

	/* C 函数 */
	type = sq_type_new(-1, NULL);

	/* C++ 方法 */
//	type = new Sq::Type(-1, NULL);

	// 2. 指定 SqType 结构中的 size，以及 init、final、parse 和 write 函数。
	type->size  = sizeof(int);
	type->init  = NULL;                  // 初始化函数
	type->final = NULL;                  // 终结函数
	type->parse = int_parse_function;    // 从 Sqxc 实例解析数据的函数
	type->write = int_write_function;    // 将数据写入 Sqxc 实例的函数
```

## 2 定义结构化数据类型

如果为结构数据类型定义 SqType，请使用 SqEntry 来定义结构中的字段。  
  
首先，我们定义一个自定义的结构化数据类型：

```c
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct User     User;

struct User {
	int    id;
	char  *name;
	char  *email;
};
```

#### 2.1 使用常量 SqType 定义结构化数据类型

与原始数据类型一样，常量 SqType 必须使用 C99 指定初始化程序或 C++ 聚合初始化。  
使用常量 SqType 定义结构化数据类型必须与 SqEntry 常量**指针数组**一起使用。  
  
要获取 C 和 C++ 代码中结构化数据类型的名称，请使用宏 SQ_GET_TYPE_NAME(Type)。  
注意：当你使用 gcc 编译时，你会从 C 和 C++ 源代码中得到不同的类型名称，因为 gcc 的 typeid(Type).name() 会返回奇怪的名称。  
  
如果 SqEntry 的指针数组已经按名称排序，则 SqType::bit_field 设置为 SQB_TYPE_SORTED。  
  
例如: 常量 SqType 使用 SqEntry "已排序" 的常量指针数组

```c
/* sortedEntryPointers 是 "已排序" 的 entryPointers（按名称排序） */
static const SqEntry  *sortedEntryPointers[] = {
	&(SqEntry) {SQ_TYPE_STR,    "email", offsetof(User, email), 0},
	&(SqEntry) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
	&(SqEntry) {SQ_TYPE_STR,    "name",  offsetof(User, name),  0},
};

/* sortedTypeUser 使用 sortedEntryPointers（在 SqType::bit_field 中设置 SQB_TYPE_SORTED） */
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
	// 因为 sortedEntryPointers 已经使用 SqEntry::name 排序了，
	// 您可以在 SqType::bit_field 中设置 SQB_TYPE_SORTED
};
```

例如: 常量 SqType 使用 SqEntry "未排序" 的常量指针数组  
  
因为 SqEntry 的指针数组没有按名称排序，SqType::bit_field 设置为 0。

```c
const SqType unsortedTypeUser = {
	/*  省略其他代码...  */

	.bit_field = 0,
};
```

宏 SQ_TYPE_INITIALIZER() 和 SQ_TYPE_INITIALIZER_FULL() 可以简单地定义常量 SqType。

```c
/* 因为 sortedEntryPointers   是 "已排序" 的条目，所以您可以将 SQB_TYPE_SORTED 传递给最后一个参数。 */
const SqType  sortedTypeUserM   = SQ_TYPE_INITIALIZER(User, sortedEntryPointers,   SQB_TYPE_SORTED);

/* 因为 unsortedEntryPointers 是 "未排序" 的条目，所以您可以将 0 传递给最后一个参数。 */
const SqType  unsortedTypeUserM = SQ_TYPE_INITIALIZER(User, unsortedEntryPointers, 0);
```

#### 2.2 使用动态 SqType 定义结构化数据类型

动态 SqType 可以添加动态或常量 SqEntry（或 SqEntry 的派生结构）。  
C 函数 sq_type_add_entry_ptrs() 及其 C++ 方法 addEntry() 可以添加 SqEntry 的**指针数组**。

使用 C 语言添加 SqEntry 的**指针数组**。

```c
	SqType  *type;
	int      n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	sq_type_add_entry_ptrs(type, entryPointers, n_entry);
```

使用 C++ 语言添加 SqEntry 的**指针数组**。

```c++
	Sq::Type  *type;
	int        n_entry = sizeof(entryPointers) / sizeof(SqEntry*);

	type = new Sq::Type(8, sq_entry_free);

	type->addEntry(entryPointers, n_entry);
```

C 函数 sq_type_add_entry() 及其 C++ 方法可以添加单个或多个 SqEntry 实例。
sq_type_add_entry() 的最后一个参数是 SqEntry 实例的大小。它用于添加 SqEntry 派生结构数组，因为该函数需要派生结构的大小来确定元素大小。如果不是派生结构，则设置为 0。  
  
使用 C 语言添加 SqEntry 实例（或实例数组）:

```c
	SqType  *type;
	SqEntry *entry;

	type = sq_type_new(8, (SqDestroyFunc)sq_entry_free);

	entry = sq_entry_new(SQ_TYPE_STR);
	sq_entry_set_name(entry, "name");
	entry->offset = offsetof(User, name);

	sq_type_add_entry(type, entry, 1, 0);
```

使用 C++ 语言添加 SqEntry 实例（或实例数组）:

```c++
	Sq::Type  *type;
	Sq::Entry *entry;

	type = new Sq::Type(8, sq_entry_free);

	entry = new Sq::Entry(SQ_TYPE_STR);
	entry->setName("name");
	entry->offset = offsetof(User, name);

	type->addEntry(entry);
```

#### 2.3 计算动态结构化数据类型的实例大小

* 用户可以使用 C 函数 sq_type_decide_size()、C++ 方法 decideSize() 来计算实例大小。
* 添加 SqEntry 后，您无需调用函数来计算实例大小，因为程序会自动执行此操作。
* 从类型中删除 SqEntry 后，您必须调用函数来计算实例大小。
  
函数 sq_type_decide_size() 声明：

```c++
/* C 函数 */
unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed);

/* C++ 方法 */
unsigned int  Sq::Type::decideSize(const SqEntry *inner_entry, bool entry_removed);
```

* 如果 'inner_entry' == NULL，它使用 SqType 中的所有 SqEntry 来重新计算大小。
* 如果用户将 'inner_entry' 添加到 SqType，则传递参数 'entry_removed' = false。
* 如果用户从 SqType 中删除 'inner_entry'，则传递参数 'entry_removed' = true。

#### 2.4 从动态 SqType 中查找并删除 SqEntry

使用 C 语言查找和删除 SqEntry

```c
	SqEntry **entry_addr;

	entry_addr = sq_type_find_entry(type, "entry_name", NULL);
	sq_type_erase_entry_addr(type, entry_addr, 1);

	// 从 SqType 中删除 SqEntry 并将 entry 保留在内存中
//	sq_type_steal_entry_addr(type, entry_addr, 1);

	sq_type_decide_size(type, NULL, true);
//	sq_type_decide_size(type, *entry_addr, true);
```

使用 C++ 语言查找和删除 SqEntry

```c++
	Sq::Entry **entry_addr;

	entry_addr = type->findEntry(type, "entry_name");
	type->eraseEntry(type, entry_addr);

	// 从 SqType 中删除 SqEntry 并将 entry 保留在内存中
//	type->stealEntry(type, entry_addr);

	type->decideSize(NULL, true);
//	type->decideSize(*entry_addr, true);
```

#### 2.5 复制 SqType

由于常量 SqType 不能直接修改，所以修改前必须复制它。  
函数 sq_type_copy() 可以复制 SqType。 它的声明：

```c
SqType  *sq_type_copy(SqType *type_dest, const SqType *type_src,
                      SqDestroyFunc entry_free_func,
                      SqCopyFunc    entry_copy_func);
```

它将数据从 'type_src' 复制到 'type_dest'，'type_dest' 必须是原始内存。  
如果 'entry_copy_func' 为 NULL， 'type_dest' 将共享 'type_src' 中的 SqEntry 实例。  
如果 'type_dest' 为 NULL，它将为 'type_dest' 分配内存。  
返回 'type_dest' 或新创建的 SqType。  
  
使用 C 语言复制 SqType

```c
	// 复制常量 SqType - SQ_TYPE_ARRAY
	// 这主要用于修改常量 SqType。
	type = sq_type_copy(NULL, SQ_TYPE_ARRAY,
	                    (SqDestroyFunc) NULL,
	                    (SqCopyFunc)    NULL);

	// 从 SqTable::type 复制常量 SqType
	// 新实例 'type' 共享来自 'table->type' 的 SqColumn 实例
	// 这主要用于迁移时修改常量 SqType（来自 SqTable::type）。
	type = sq_type_copy(NULL, table->type,
	                    (SqDestroyFunc) sq_column_free,
	                    (SqCopyFunc)    NULL);

	// 从 SqTable::type 完整复制 SqType
	// 新实例 'type' 从 'table->type' 复制 SqColumn 实例
	type = sq_type_copy(NULL, table->type,
	                    (SqDestroyFunc) sq_column_free,
	                    (SqCopyFunc)    sq_column_copy);
```

#### 2.6 将动态更改为常量

C 函数 sq_type_use_constant()、 C++ 方法 useConstant() 可以从 SqType::bit_field 中清除 SQB_TYPE_DYNAMIC。
这些通常与全局 SqType 一起使用。  
  
使用 C 语言

```c
	// 'globalType' 是全局变量并且不是常量。
	SqType *type = globalType;

	// 初始化
	sq_type_init_self(type, 0, NULL);
	sq_type_use_constant(type);
```

使用 C++ 语言

```c++
	// 'globalType' 是全局变量并且不是常量。
	Sq::Type *type = globalType;

	// 初始化
	type->initSelf(0, NULL);
	type->useConstant();
```

## 3 如何支持新的容器类型

用户必须实现 4 个函数来支持新类型，并且必须在类型的解析器和写入器中处理 SQXC_TYPE_XXXX 的数据。  
所有容器类型，如数组、向量或列表...等 都对应于 SQXC_TYPE_ARRAY。  
所有结构化类型对应于 SQXC_TYPE_OBJECT。  

#### 3.1 为容器声明 SqType

例如: MyList 是列表数据类型

```c++
// 这是头文件

/* MyList 是列表数据类型

struct MyListNode {
	MyListNode *next;    // 指向下一个 MyListNode 的指针
	void       *data;    // 指向元素数据的指针
};

struct MyList {
	MyListNode *head;    // 指向第一个 MyListNode 的指针
}
 */

#ifdef __cplusplus    // 混合 C 和 C++
extern "C" {
#endif

// sqType_MyList 在源代码文件中定义。
extern const SqType                 sqType_MyList

#define SQ_TYPE_MY_LIST           (&sqType_MyList)

#ifdef __cplusplus    // 混合 C 和 C++
}  // extern "C"
#endif
```

#### 3.2 实现容器的 SqType 接口

sqxc_push_nested() 用于将当前 SqType 及其实例推送到堆栈。  
sqxc_pop_nested()  用于从堆栈中恢复上个 SqType 及其实例。  
  
解析 SQXC_TYPE_OBJECT 或 SQXC_TYPE_ARRAY 时调用 sqxc_push_nested()。  
解析 SQXC_TYPE_OBJECT_END 或 SQXC_TYPE_ARRAY_END 时调用 sqxc_pop_nested()。

```c++
// 这是源代码文件

static void sq_type_my_list_init(void *mylist, const SqType *type)
{
	// 初始化 MyList 实例
}

static void sq_type_my_list_final(void *mylist, const SqType *type)
{
	// 终结 MyList 实例
}

static int  sq_type_my_list_parse(void *mylist, const SqType *type, Sqxc *src)
{
	SqxcValue    *xc_value = (SqxcValue*)src->dest;
	SqxcNested   *nested   = xc_value->nested;
	const SqType *element_type;

	// 您可以在 SqType::entry 中指定元素类型
	element_type = (SqType*)type->entry;

	// 容器的开始
	if (nested->data != mylist) {
		// 做类型匹配
		if (src->type != SQXC_TYPE_ARRAY)
			return (src->code = SQCODE_TYPE_NOT_MATCHED);
		// 准备解析
		nested = sqxc_push_nested((Sqxc*)xc_value);
		nested->data = mylist;
		nested->data2 = (void*)type;
		return (src->code = SQCODE_OK);
	}
	// 容器结束
	// sqxc_value_send() 已处理 SQXC_TYPE_ARRAY_END。用户无需处理。

	// 解析 MyList 的元素
	void *element = calloc(1, element_type->size);
	src->name = NULL;    // 在调用 parse() 之前设置 "名称"
	src->code = element_type->parse(element, element_type, src);
	my_list_append(mylist, element);
	return src->code;
}

static Sqxc *sq_type_my_list_write(void *mylist, const SqType *type, Sqxc *dest)
{
	const SqType *element_type;
	const char   *container_name = dest->name;

	// 您可以在 SqType::entry 中指定元素类型
	element_type = (SqType*)type->entry;

	// 容器的开始
	dest->type = SQXC_TYPE_ARRAY;
//	dest->name = container_name;    // "name" 是由此函数的调用者设置的
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	if (dest->code != SQCODE_OK)
		return dest;

	// 输出 MyList 的元素
	for (MyListNode *node = mylist->head;  node;  node = node->next) {
		void *element = node->data;
		dest->name = NULL;      // 在调用 write() 之前设置 "name"
		dest = element_type->write(element, element_type, dest);
		if (dest->code != SQCODE_OK)
			return dest;
	}

	// 容器结束
	dest->type = SQXC_TYPE_ARRAY_END;
	dest->name = container_name;
//	dest->value.pointer = NULL;
	dest = sqxc_send(dest);
	return dest;
}

// 由头文件使用。
const SqType sqType_MyList =
{
	sizeof(MyList),                // size
	sq_type_my_list_init,          // init
	sq_type_my_list_final,         // final
	sq_type_my_list_parse,         // parse
	sq_type_my_list_write,         // write

	NULL,                          // name
	(SqEntry**) SQ_TYPE__yours_,   // entry   : 您可以在 SqType::entry 中指定元素类型
	-1,                            // n_entry : 如果 SqType::n_entry == -1，则不会释放 SqType::entry
	0,                             // bit_field
	NULL,                          // on_destroy
};
```

#### 4 派生 SqType (动态)

这定义从 SqType 派生的新结构。  
如果要在派生的 SqType 中添加成员，你可以使用 SqType::on_destroy 回调函数释放它们。

```c++
// 如果您使用 C 语言，请使用 'typedef' 为结构类型赋予新名称。
typedef struct MyType   MyType;

// 源自 SqType 的结构
#ifdef __cplusplus
struct MyType : Sq::TypeMethod        // <-- 1. 继承 C++ 成员函数（方法）
#else
struct MyType
#endif
{
	SQ_TYPE_MEMBERS;                  // <-- 2. 继承成员变量

	int   mydata;                     // <-- 3. 在派生结构中添加变量和非虚函数。
	void *mypointer;

#ifdef __cplusplus
	// 如果您使用 C++ 语言，请在此处定义 C++ 构造函数和析构函数。
	MyType() {
		my_type_init(this);
	}
	~MyType() {
		my_type_final(this);
	}
#endif
};

// 这是销毁通知器的回调函数。
void    my_type_on_destroy(MyType *mytype) {
	// 释放派生结构的资源
	free(mytype->mypointer);
}

void    my_type_init(MyType *type)
{
	sq_type_init_self((SqType*)mytype, 0, NULL);
	// 在这里初始化 SqType 的成员。
	// ...

	// 设置销毁通知器的回调函数。
	// 调用 sq_type_final_self() 时将发出此通知。
	mytype->on_destroy = my_type_on_destroy;

	// 初始化派生结构的变量
	mytype->mydata = 10;
	mytype->mypointer = malloc(512);
}

void    my_type_final(MyType *type)
{
	// sq_type_final_self() 将发出销毁通知
	sq_type_final_self((SqType*)type);
}

SqType *my_type_new()
{
	MyType *mytype = malloc(sizeof(MyType));

	my_type_init(mytype);
	// 返回类型是 SqType，因为 MyType 可以通过 sq_type_free() 释放。
	return (SqType*)mytype;
}
```

## 5 释放动态 SqType

sq_type_free() 可以释放动态 SqType（SqType::bit_field 有 SQB_TYPE_DYNAMIC）。它可以发出销毁通知程序。

```c++
	/* C 函数 */
	sq_type_free(type);

	/* C++ 方法 */
	delete type;
```
