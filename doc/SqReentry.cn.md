[English](SqReentry.md)

# SqReentry

SqReentry 用于更改先前定义的条目。它派生自 SqEntry 并添加了一个字段 'old_name' 用于记录名称更改（重命名或删除）。这主要用于数据库迁移。

	SqEntry
	│
	└─── SqReentry

## 改名的记录

要重命名条目，您可以在 SqReentry::old_name 中指定名称，并在 SqReentry::name 中指定新名称。  
  
例如 将 "name" 重命名为 "new name"。

```c
	SqReentry  reentry = {0};

	reentry.old_name = "name";
	reentry.name     = "new name";
```

## 删除的记录

要删除一个条目，您可以在 SqReentry::old_name 中指定要删除的名称，并在 SqReentry::name 中指定 NULL。  
  
例如 删除 city_id。

```c
	SqReentry  reentry = {0};

	reentry.old_name = "city_id";
	reentry.name     = NULL;
```
