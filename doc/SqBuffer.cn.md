[English](SqBuffer.md)

# SqBuffer

SqBuffer 提供一个可扩充的缓冲区。

## 初始化

SqBuffer 小而简单并可以直接放在栈内存中使用。它将所有成员重置为 0 进行初始化。

```c
	SqBuffer  buf = {0};

	SqBuffer *buf_in_heap = calloc(1, sizeof(SqBuffer));
```

它也提供正式的 C 函数和 C++ 方法。  
  
使用 C 语言

```c
	sq_buffer_init(&buf);
	sq_buffer_final(&buf);

	buf_in_heap = sq_buffer_new();
	sq_buffer_free(buf_in_heap);
```

使用 C++ 语言

```c++
	// Sq::Buffer 有构造函数和析构函数
	Sq::Buffer  buf;

	Sq::Buffer *bufInHeap = new Sq::Buffer;
	delete bufInHeap;
```

## 分配

C 函数 sq_buffer_alloc()，C++ 方法 alloc() 可以从 SqBuffer 中写入的位置开始分配内存。  
C 函数 sq_buffer_alloc_at()，C++ 重载方法 alloc() 可以从 SqBuffer 中的指定位置分配内存。  
如果没有足够的空间，SqBuffer 将扩展缓冲区。  
  
使用 C 语言

```c
	char *memory;
	unsigned int  length = 16;
	unsigned int  position = 8;

	memory = sq_buffer_alloc(buffer, length);

	// 从指定位置分配内存
	memory = sq_buffer_alloc_at(buffer, position, length);
```

使用 C++ 语言

```c++
	char *memory;
	unsigned int  length = 16;
	unsigned int  position = 8;

	memory = buffer->alloc(length);

	// 从指定位置分配内存
	memory = buffer->alloc(position, length);
```

## 写入

写入动作会先调用 sq_buffer_alloc() 然后将数据复制到 SqBuffer。  
  
使用 C 语言

```c
	// 写入一个字符
	sq_buffer_write_c(buffer, 'c');

	// 写入以 NULL 结尾的字符串
	sq_buffer_write(buffer, string);

	// 写入一个字符串并指定它的长度
	sq_buffer_write_len(buffer, string, length);
```

使用 C++ 语言

```c++
	// 写入一个字符
	buffer->write('c');

	// 写入以 NULL 结尾的字符串
	buffer->write(string);

	// 写入一个字符串并指定它的长度
	buffer->write(string, length);
```

## 插入

插入动作会先调用 sq_buffer_alloc_at() 在指定位置分配内存，然后将数据复制到 SqBuffer 中的指定位置。  
  
使用 C 语言

```c
	// 插入一个字符
	sq_buffer_insert_c(buffer, position, 'c');

	// 插入以 NULL 结尾的字符串
	sq_buffer_insert(buffer, position, string);

	// 插入一个字符串并指定它的长度
	sq_buffer_insert_len(buffer, position, string, length);
```

使用 C++ 语言

```c++
	// 插入一个字符
	buffer->insert(position, 'c');

	// 插入以 NULL 结尾的字符串
	buffer->insert(position, string);

	// 插入一个字符串并指定它的长度
	buffer->insert(position, string, length);
```

## 调整大小

```c++
	// C
	sq_buffer_resize(buffer, size);

	// C++
	buffer->resize(size);
```
