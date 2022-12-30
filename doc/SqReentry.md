[中文](SqReentry.cn.md)

# SqReentry

SqReentry is used to change previously-defined entries. It derives from SqEntry and adds a field 'old_name' for recording name changes (rename or remove).

	SqEntry
	│
	└─── SqReentry

## Record of rename

e.g. rename "name" to "new name".

```c
	SqReentry  reentry = {0};

	reentry.old_name = "name";
	reentry.name     = "new name";
```

## Record of remove

e.g. delete city_id.

```c
	SqReentry  reentry = {0};

	reentry.old_name = "city_id";
	reentry.name     = NULL;
```
