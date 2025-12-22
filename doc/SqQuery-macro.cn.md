[English](SqQuery-macro.md)

## 使用宏生成查询 (实验性功能)

宏 SQ_QUERY_DO() 用于构建查询。宏中的最后一个参数类似于 lambda 函数。

```c
#include <sqxc/sqxclib.h>
#include <sqxc/SqQuery-macro.h>    // sqxclib.h 不包含特殊的宏

	SQ_QUERY_DO(query, {
		SQQ_SELECT("id", "age");
		SQQ_FROM("companies");
		SQQ_JOIN_SUB({
			SQQ_FROM("city");
			SQQ_WHERE("id", "<", "%d", 100);
		});
		SQQ_AS("c");
		SQQ_ON_RAW("c.id = companies.city_id");
		SQQ_WHERE_RAW("age > %d", 5);
	});
```
