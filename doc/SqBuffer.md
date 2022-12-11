[中文](SqBuffer.cn.md)

# SqBuffer

SqBuffer provides an expandable buffer.

## Initialize

SqBuffer is small and simple. It can be used directly in the stack memory and reset all members as 0 to initialize.

```c
	SqBuffer  buf = {0};

	SqBuffer *buf_in_heap = calloc(1, sizeof(SqBuffer));
```

It also provides formal C functions and C++ methods.  
  
use C language

```c
	sq_buffer_init(&buf);
	sq_buffer_final(&buf);

	buf_in_heap = sq_buffer_new();
	sq_buffer_free(buf_in_heap);
```

use C++ language

```c++
	// Sq::Buffer has constructor and destructor
	Sq::Buffer  buf;

	Sq::Buffer *bufInHeap = new Sq::Buffer;
	delete bufInHeap;
```

## Allocate

C function sq_buffer_alloc(), C++ method alloc() can allocate memory from writed position in SqBuffer. SqBuffer will expand buffer if it does not have enough space.  
  
use C language

```c
	char *memory;
	int   length = 16;
	int   position = 8;

	memory = sq_buffer_alloc(buffer, length);

	// allocate memory from specified position
	memory = sq_buffer_alloc_at(buffer, position, length);
```

use C++ language

```c++
	char *memory;
	int   length = 16;
	int   position = 8;

	memory = buffer->alloc(length);

	// allocate memory from specified position
	memory = buffer->alloc(position, length);
```

## Write

The writing action will first call sq_buffer_alloc() then copy the data to the SqBuffer.  
  
use C language

```c
	// write a character
	sq_buffer_write_c(buffer, 'c');

	// write NULL-terminated string
	sq_buffer_write(buffer, string);

	// write a string and specify its length
	sq_buffer_write_n(buffer, string, length);
```

use C++ language

```c++
	// write a character
	buffer->write('c');

	// write NULL-terminated string
	buffer->write(string);

	// write a string and specify its length
	buffer->write(string, length);
```

## Insert

The inserting action will first call sq_buffer_alloc_at() then copy the data to the SqBuffer.  
  
use C language

```c
	// insert a character
	sq_buffer_insert_c(buffer, position, 'c');

	// insert NULL-terminated string
	sq_buffer_insert(buffer, position, string);

	// insert a string and specify its length
	sq_buffer_insert_n(buffer, position, string, length);
```

use C++ language

```c++
	// insert a character
	buffer->insert(position, 'c');

	// insert NULL-terminated string
	buffer->insert(position, string);

	// insert a string and specify its length
	buffer->insert(position, string, length);
```

## Resize

```c++
	// C
	sq_buffer_resize(buffer, size);

	// C++
	buffer->resize(size);
```
