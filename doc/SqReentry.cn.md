[English](SqReentry.md)

# SqReentry

SqReentry 用于更改先前定义的条目。它派生自 SqEntry 并添加了一个字段 'old_name' 用于记录名称更改（重命名或删除）。

	SqEntry
	│
	└─── SqReentry

## 改名的记录

例如 将 "name" 重命名为 "new name"。

```c
	SqReentry  reentry = {0};

	reentry.old_name = "name";
	reentry.name     = "new name";
```

## 删除的记录

例如 删除 city_id。

```c
	SqReentry  reentry = {0};

	reentry.old_name = "city_id";
	reentry.name     = NULL;
```
