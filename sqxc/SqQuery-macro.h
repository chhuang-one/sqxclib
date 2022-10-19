/*
 *   Copyright (C) 2020-2022 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef SQ_QUERY_MACRO_H
#define SQ_QUERY_MACRO_H

#include <SqQuery.h>

/* C Macro functions to create SQL query easily, for example:

	SELECT * FROM Company
	WHERE salary > 2150 AND (id > 22 AND age < 10)

	SQ_QUERY_DO(query, {
		SQQ_FROM("Company");
		SQQ_WHERE("salary > %d", 2150);
		SQQ_WHERE_SUB({
			SQQ_WHERE("id",  ">", "22");
			SQQ_WHERE("age < 10");
		});
	});

	// --- or ---

	SQ_QUERY_C_DEF(query);

	SQQ_FROM("Company");
	SQQ_WHERE("salary > %d", 2150);
	SQQ_WHERE_SUB({
		SQQ_WHERE("id",  ">", "22");
		SQQ_WHERE("age < 10");
	});

	// *** use SQ_QUERY_C_SET(query) to set current query
 */


#define SQ_QUERY_DO(query, lambda)          \
		{                                   \
			SqQuery* query_cur_ = query;    \
			lambda;                         \
		}

// SQ_QUERY_C_DEF() define variable for these macro.
// Because some SQL has DECLARE command, I must avoid to use SQ_QUERY_DECLARE() here.
#define SQ_QUERY_C_DEF(initial_value)       \
			SqQuery* query_cur_ = initial_value

// SQ_QUERY_C_SET() set current query for these macro.
// Because SQL has SET command, I must avoid to use SQ_QUERY_DO() here.
#define SQ_QUERY_C_SET(query)               \
			query_cur_ = query

#define SQ_QUERY_C_CUR        query_cur_

/*
// --------------------------------------------------------
// use SqQuery instance with subquery (nested query)

#define SQ_QUERY_FROM_SUB(query, lambda)     \
		{                                    \
			SqQuery* query_cur_ = query;     \
			sq_query_from_sub(query_cur_);   \
			lambda;                          \
			sq_query_end_sub(query_cur_);    \
		}

#define SQ_QUERY_JOIN_SUB(query, lambda)     \
		{                                    \
			SqQuery* query_cur_ = query;     \
			sq_query_join_sub(query_cur_);   \
			lambda;                          \
			sq_query_end_sub(query_cur_);    \
		}

#define SQ_QUERY_ON_SUB(query, lambda)       \
		{                                    \
			SqQuery* query_cur_ = query;     \
			sq_query_on_sub(query_cur_);     \
			lambda;                          \
			sq_query_end_sub(query_cur_);    \
		}

#define SQ_QUERY_OR_ON_SUB(query, lambda)     \
		{                                     \
			SqQuery* query_cur_ = query;      \
			sq_query_or_on_sub(query_cur_);   \
			lambda;                           \
			sq_query_end_sub(query_cur_);     \
		}

#define SQ_QUERY_WHERE_SUB(query, lambda)     \
		{                                     \
			SqQuery* query_cur_ = query;      \
			sq_query_where_sub(query_cur_);   \
			lambda;                           \
			sq_query_end_sub(query_cur_);     \
		}

#define SQ_QUERY_OR_WHERE_SUB(query, lambda)     \
		{                                        \
			SqQuery* query_cur_ = query;         \
			sq_query_or_where_sub(query_cur_);   \
			lambda;                              \
			sq_query_end_sub(query_cur_);        \
		}

#define SQ_QUERY_WHERE_EXISTS(query, lambda)     \
		{                                        \
			SqQuery* query_cur_ = query;         \
			sq_query_where_exists(query_cur_);   \
			lambda;                              \
			sq_query_end_sub(query_cur_);        \
		}

#define SQ_QUERY_HAVING_SUB(query, lambda)       \
		{                                        \
			SqQuery* query_cur_ = query;         \
			sq_query_having_sub(query_cur_);     \
			lambda;                              \
			sq_query_end_sub(query_cur_);        \
		}

#define SQ_QUERY_OR_HAVING_SUB(query, lambda)     \
		{                                         \
			SqQuery* query_cur_ = query;          \
			sq_query_or_having_sub(query_cur_);   \
			lambda;                               \
			sq_query_end_sub(query_cur_);         \
		}
 */

/*
// --------------------------------------------------------
// default value for macro...
#define SQQ_FROM_0()      sq_query_from(query_cur_, NULL)
#define SQQ_FROM_1(A)     sq_query_from(query_cur_, A)
#define SQQ_FROM_X(x,A,FUNC, ...)  FUNC

#define SQQ_FROM(...)                            \
			SQQ_FROM_X(,##__VA_ARGS__,           \
			          SQQ_FROM_1(__VA_ARGS__),   \
			          SQQ_FROM_0(__VA_ARGS__)    \
			         )
 */

#define SQQ_FROM(table)  \
		sq_query_from(query_cur_, table)

#define SQQ_FROM_SUB(lambda)               \
		sq_query_from_sub(query_cur_);     \
		lambda;                            \
		sq_query_end_sub(query_cur_)

#define SQQ_AS(alias_name)  \
		sq_query_as(query_cur_, alias_name)

// --------------------------------------------------------

#define SQQ_JOIN(table, ...)   \
		sq_query_join(query_cur_, table, __VA_ARGS__)

#define SQQ_JOIN_SUB(lambda)                          \
		sq_query_join_sub(query_cur_);                \
		lambda;                                       \
		sq_query_end_sub(query_cur_)

#define SQQ_LEFT_JOIN(table, ...)   \
		sq_query_left_join(query_cur_, table, __VA_ARGS__)

#define SQQ_LEFT_JOIN_SUB(lambda)                     \
		sq_query_left_join_sub(query_cur_);           \
		lambda;                                       \
		sq_query_end_sub(query_cur_)

#define SQQ_RIGHT_JOIN(table, ...)   \
		sq_query_right_join(query_cur_, table, __VA_ARGS__)

#define SQQ_RIGHT_JOIN_SUB(lambda)                    \
		sq_query_right_join_sub(query_cur_);          \
		lambda;                                       \
		sq_query_end_sub(query_cur_)

#define SQQ_FULL_JOIN(table, ...)   \
		sq_query_full_join(query_cur_, table, __VA_ARGS__)

#define SQQ_FULL_JOIN_SUB(lambda)                     \
		sq_query_full_join_sub(query_cur_);           \
		lambda;                                       \
		sq_query_end_sub(query_cur_)

#define SQQ_CROSS_JOIN(table)   \
		sq_query_cross_join(query_cur_, table)

#define SQQ_CROSS_JOIN_SUB(lambda)                    \
		sq_query_cross_join_sub(query_cur_);          \
		lambda;                                       \
		sq_query_end_sub(query_cur_)

#define SQQ_ON(...)   \
		sq_query_on(query_cur_, __VA_ARGS__)
#define SQQ_ON_RAW(raw, ...)    \
		sq_query_on_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_ON_SUB(lambda)                \
		sq_query_on_sub(query_cur_);      \
		lambda;                           \
		sq_query_end_sub(query_cur_)

#define SQQ_OR_ON(...)   \
		sq_query_or_on(query_cur_, __VA_ARGS__)
#define SQQ_OR_ON_RAW(raw, ...)    \
		sq_query_or_on_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_OR_ON_SUB(lambda)              \
		sq_query_or_on_sub(query_cur_);    \
		lambda;                            \
		sq_query_end_sub(query_cur_)

// --------------------------------------------------------

#define SQQ_WHERE(...)    \
		sq_query_where(query_cur_, __VA_ARGS__)
#define SQQ_WHERE_RAW(raw, ...)    \
		sq_query_where_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_WHERE_SUB(lambda)               \
		sq_query_where_sub(query_cur_);     \
		lambda;                             \
		sq_query_end_sub(query_cur_)

#define SQQ_OR_WHERE(...)    \
		sq_query_or_where(query_cur_, __VA_ARGS__)
#define SQQ_OR_WHERE_RAW(raw, ...)    \
		sq_query_or_where_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_OR_WHERE_SUB(lambda)              \
		sq_query_or_where_sub(query_cur_);    \
		lambda;                               \
		sq_query_end_sub(query_cur_)


#define SQQ_WHERE_NOT(...)    \
		sq_query_where_not(query_cur_, __VA_ARGS__)
#define SQQ_WHERE_NOT_RAW(raw, ...)    \
		sq_query_where_not_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_WHERE_NOT_SUB(lambda)                   \
		sq_query_where_not_sub(query_cur_);         \
		lambda;                                     \
		sq_query_end_sub(query_cur_)

#define SQQ_OR_WHERE_NOT(...)    \
		sq_query_or_where_not(query_cur_, __VA_ARGS__)
#define SQQ_OR_WHERE_NOT_RAW(raw, ...)    \
		sq_query_or_where_not_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_OR_WHERE_NOT_SUB(lambda)                \
		sq_query_or_where_not_sub(query_cur_);      \
		lambda;                                     \
		sq_query_end_sub(query_cur_)


#define SQQ_WHERE_EXISTS(lambda)                \
		sq_query_where_exists(query_cur_);      \
		lambda;                                 \
		sq_query_end_sub(query_cur_)

#define SQQ_WHERE_NOT_EXISTS(lambda)            \
		sq_query_where_not_exists(query_cur_);  \
		lambda;                                 \
		sq_query_end_sub(query_cur_)


#define SQQ_WHERE_BETWEEN(column, format, ...)    \
		sq_query_where_between(query_cur_, column, format, __VA_ARGS__)

#define SQQ_WHERE_NOT_BETWEEN(column, format, ...)    \
		sq_query_where_not_between(query_cur_, column, format, __VA_ARGS__)

#define SQQ_OR_WHERE_BETWEEN(column, format, ...)    \
		sq_query_or_where_between(query_cur_, column, format, __VA_ARGS__)

#define SQQ_OR_WHERE_NOT_BETWEEN(column, format, ...)    \
		sq_query_or_where_not_between(query_cur_, column, format, __VA_ARGS__)


#define SQQ_WHERE_IN(column, n_args, format, ...)    \
		sq_query_where_in(query_cur_, column, n_args, format, __VA_ARGS__)

#define SQQ_WHERE_NOT_IN(column, n_args, format, ...)    \
		sq_query_where_not_in(query_cur_, column, n_args, format, __VA_ARGS__)

#define SQQ_OR_WHERE_IN(column, n_args, format, ...)    \
		sq_query_or_where_in(query_cur_, column, n_args, format, __VA_ARGS__)

#define SQQ_OR_WHERE_NOT_IN(column, n_args, format, ...)    \
		sq_query_or_where_not_in(query_cur_, column, n_args, format, __VA_ARGS__)


#define SQQ_WHERE_NULL(column)    \
		sq_query_where_null(query_cur_, column)

#define SQQ_WHERE_NOT_NULL(column)    \
		sq_query_where_not_null(query_cur_, column)

#define SQQ_OR_WHERE_NULL(column)    \
		sq_query_or_where_null(query_cur_, column)

#define SQQ_OR_WHERE_NOT_NULL(column)    \
		sq_query_or_where_not_null(query_cur_, column)

// --------------------------------------------------------

#define SQQ_GROUP_BY(...)  \
		sq_query_group_by(query_cur_, __VA_ARGS__, NULL)
#define SQQ_GROUP_BY_RAW(raw)    \
		sq_query_group_by_raw(query_cur_, raw)

// --------------------------------------------------------

#define SQQ_HAVING(...)  \
		sq_query_having(query_cur_, __VA_ARGS__)
#define SQQ_HAVING_RAW(raw, ...)    \
		sq_query_having_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_HAVING_SUB(lambda)              \
		sq_query_having_sub(query_cur_);    \
		lambda;                             \
		sq_query_end_sub(query_cur_)

#define SQQ_OR_HAVING(...)  \
		sq_query_or_having(query_cur_, __VA_ARGS__)
#define SQQ_OR_HAVING_RAW(raw, ...)    \
		sq_query_or_having_raw(query_cur_, raw, ##__VA_ARGS__)

#define SQQ_OR_HAVING_SUB(lambda)             \
		sq_query_or_having_sub(query_cur_);   \
		lambda;                               \
		sq_query_end_sub(query_cur_)

// --------------------------------------------------------

#define SQQ_SELECT(...)   \
		sq_query_select(query_cur_, __VA_ARGS__, NULL)
#define SQQ_SELECT_RAW(raw)    \
		sq_query_select_raw(query_cur_, raw)

#define SQQ_DISTINCT()   \
		sq_query_distinct(query_cur_)

// --------------------------------------------------------

#define SQQ_ORDER_BY(...)   \
		sq_query_order_by(query_cur_, __VA_ARGS__, NULL)
#define SQQ_ORDER_BY_RAW(raw)    \
		sq_query_order_by_raw(query_cur_, raw)

#define SQQ_ASC()   \
		sq_query_asc(query_cur_)

#define SQQ_DESC()   \
		sq_query_desc(query_cur_)

// --------------------------------------------------------

#define SQQ_DELETE()  \
		sq_query_delete(query_cur_);

#define SQQ_TRUNCATE()  \
		sq_query_truncate(query_cur_);

// --------------------------------------------------------

#define SQQ_TO_SQL()  \
		sq_query_to_sql(query_cur_)

#endif  // SQ_QUERY_MACRO_H
