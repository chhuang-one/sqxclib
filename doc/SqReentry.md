[中文](SqReentry.cn.md)

# SqReentry

SqReentry is used to change previously-defined entries. It derives from SqEntry and adds a field 'old_name' for recording name changes (rename or remove). This is mainly used by database migration.

	SqEntry
	│
	└─── SqReentry

## Record of rename

To rename a entry, you can specify name in SqReentry::old_name and new name in SqReentry::name.  
  
e.g. rename "name" to "new name".

```c
	SqReentry  reentry = {0};

	reentry.old_name = "name";
	reentry.name     = "new name";
```

## Record of remove

To remove a entry, you can specify name that you want to remove in SqReentry::old_name and NULL in SqReentry::name.  
  
e.g. delete city_id.

```c
	SqReentry  reentry = {0};

	reentry.old_name = "city_id";
	reentry.name     = NULL;
```
