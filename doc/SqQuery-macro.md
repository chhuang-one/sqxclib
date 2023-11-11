[中文](SqQuery-macro.cn.md)

## Use macro to produce query (Experimental)

macro SQ_QUERY_DO() is used to build query. The last parameter in macro is like lambda function.

```c
#include <sqxclib.h>
#include <SqQuery-macro.h>    // sqxclib.h doesn't contain special macros

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
