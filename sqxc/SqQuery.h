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


#ifndef SQ_QUERY_H
#define SQ_QUERY_H

#ifdef __cplusplus
#include <functional>
#endif

#include <stdbool.h>
#include <inttypes.h>      // PRId64, PRIu64

#include <SqPtrArray.h>    // used by sq_query_get_table_as_names()
#include <SqTable.h>       // used by sq_query_select_table_as()

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqQuery           SqQuery;
typedef struct SqQueryNode       SqQueryNode;
typedef struct SqQueryNested     SqQueryNested;

// ----------------------------------------------------------------------------
// C data and functions declaration

#ifdef __cplusplus
extern "C" {
#endif

// join on, where, having
#define SQ_QUERYLOGI_OR       0
#define SQ_QUERYLOGI_AND      1
#define SQ_QUERYLOGI_NOT      2
#define SQ_QUERYLOGI_OR_NOT   (SQ_QUERYLOGI_OR  | SQ_QUERYLOGI_NOT)
#define SQ_QUERYLOGI_AND_NOT  (SQ_QUERYLOGI_AND | SQ_QUERYLOGI_NOT)

// join
#define SQ_QUERYJOIN_INNER    0
#define SQ_QUERYJOIN_LEFT     1
#define SQ_QUERYJOIN_RIGHT    2
#define SQ_QUERYJOIN_FULL     3
#define SQ_QUERYJOIN_CROSS    4

// order by
#define SQ_QUERYSORT_ASC      0
#define SQ_QUERYSORT_DESC     1

// arguments: decide to use raw string or printf format string
#define SQ_QUERYARGS_MASK     (0xFF00)
#define SQ_QUERYARGS_RAW      (0x8000)

#define SQ_QUERYARGS_N_MASK   (0x3F00)
#define SQ_QUERYARGS_1        (0x0100)    // only 1 argument  in __VA_ARGS__
#define SQ_QUERYARGS_2        (0x0200)    //      2 arguments in __VA_ARGS__
#define SQ_QUERYARGS_3        (0x0300)    //      3 arguments in __VA_ARGS__

#define SQ_QUERYARGS_N_SET(n)       ((n) << 8)
#define SQ_QUERYARGS_N_GET(n)       ( ((n) & SQ_QUERYARGS_N_MASK) >> 8)

/*	SQ_QUERYARGS_DECIDE(...)  return SQ_QUERYARGS_1, SQ_QUERYARGS_2...etc

	Warning: SQ_QUERYARGS_COUNT() can't return 0.

	https://groups.google.com/g/comp.std.c/c/d-6Mj5Lko_s
 */

#define SQ_QUERYARGS_DECIDE(PlusOrMinus, ...)        ((SQ_QUERYARGS_COUNT(__VA_ARGS__) + (PlusOrMinus)) << 8)

/*      SQ_QUERYARGS_COUNT(...)     SQ_QUERYARGS_APART(_0, ## __VA_ARGS__, SQ_QUERYARGS_RSEQ())
*/
#define SQ_QUERYARGS_COUNT(...)     SQ_QUERYARGS_APART(__VA_ARGS__, SQ_QUERYARGS_RSEQ())

#define SQ_QUERYARGS_APART(...)     SQ_QUERYARGS_PICK(__VA_ARGS__)

#define SQ_QUERYARGS_PICK(     \
	 _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,  N,...)    N

/*      SQ_QUERYARGS_RSEQ()    \
	62, 61, 60, \
 */
#define SQ_QUERYARGS_RSEQ()    \
	63, 62, 61, 60, \
	59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
	49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
	39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
	 9,  8,  7,  6,  5,  4,  3,  2,  1,  0

/*	SqQuery C functions


	There are many functions can specify SQL condition and them also support printf format string.
	Please pass printf format string before passing value of condition.
	If you want to use SQL Wildcard Characters '%' in printf format string, you must print "%" using "%%".

	** below C functions support printf format string in 2nd argument:
		sq_query_raw(),
		sq_query_printf(),
		sq_query_on_raw(),        sq_query_or_on_raw(),
		sq_query_where_raw(),     sq_query_or_where_raw(),
		sq_query_where_not_raw(), sq_query_or_where_not_raw(),
		sq_query_having_raw(),    sq_query_or_having_raw(),

		These C function use macro to count number of arguments.
		If the 3rd argument is NOT exist, the 2st argument is handled as raw string.

	** below C functions support printf format string in 4th argument:
		sq_query_on(),            sq_query_or_on(),
		sq_query_where(),         sq_query_or_where(),
		sq_query_where_not(),     sq_query_or_where_not(),
		sq_query_having(),        sq_query_or_having(),

	** below C functions support printf format string in 5th argument:
		sq_query_join(),
		sq_query_left_join(),
		sq_query_right_join(),
		sq_query_full_join(),

	** other C functions that support printf format string:
		sq_query_where_between() series
		sq_query_where_in() series


	// e.g. "WHERE id < 100"
	// --- printf format string in 2nd argument ---
	sq_query_where_raw(query, "id < %d", 100);
	// --- printf format string in 4th argument ---
	sq_query_where(query, "id", "<", "%d", 100);


	** below function must use with other query
		sq_query_union(),
		sq_query_union_all(),


	** below function support subquery/brackets:
		sq_query_from_sub(),
		sq_query_join_sub(),
		sq_query_left_join_sub(),
		sq_query_right_join_sub(),
		sq_query_full_join_sub(),
		sq_query_cross_join_sub(),
		sq_query_on_sub(),           sq_query_or_on_sub(),
		sq_query_where_sub(),        sq_query_or_where_sub(),
		sq_query_where_not_sub(),    sq_query_or_where_not_sub(),
		sq_query_where_exists(),     sq_query_where_not_exists(),
		sq_query_having_sub(),       sq_query_or_having_sub(),

	// e.g. "WHERE (salary > 45 AND age < 21)"
	sq_query_where_sub(query);                  // start of brackets
		sq_query_where_raw(query, "salary > %d", 45);
		sq_query_where(query, "age", "<", "%d", 21);
	sq_query_end_sub(query);                    // end of brackets

	// e.g. "WHERE EXISTS ( SELECT * FROM table WHERE id > 20 )"
	sq_query_where_exists(query);               // start of subquery
		sq_query_from("table");
		sq_query_where_raw("id > 20");
	sq_query_end_sub(query);                    // end of subquery


	** below function support SQL raw statements:
		sq_query_raw(),
		sq_query_select_raw(),
		sq_query_on_raw(),        sq_query_or_on_raw(),
		sq_query_where_raw(),     sq_query_or_where_raw(),
		sq_query_where_not_raw(), sq_query_or_where_not_raw(),
		sq_query_having_raw(),    sq_query_or_having_raw(),
		sq_query_order_by_raw(),
		sq_query_group_by_raw(),

	// e.g. "WHERE id = 1"
	sq_query_where_raw(query, "id = 1");


 */

SqQuery *sq_query_new(const char *table_name);
void     sq_query_free(SqQuery *query);

SqQuery *sq_query_init(SqQuery *query, const char *table_name);
SqQuery *sq_query_final(SqQuery *query);

// It reset SqQuery and remove all statements.
void    sq_query_clear(SqQuery *query);

/*	sq_query_push_nested() was called by sq_query_where_exists(), sq_query_from(),
	                       sq_query_join(), sq_query_on(), sq_query_where() ...etc
	It usually doesn't call by user.
 */

// push/pop SqQueryNested
SqQueryNested *sq_query_push_nested(SqQuery *query, SqQueryNode *parent);
void           sq_query_pop_nested(SqQuery *query);

// alias name for sq_query_push_nested() and sq_query_pop_nested()
#define sq_query_begin_sub    sq_query_push_nested
#define sq_query_end_sub      sq_query_pop_nested

// append raw SQL statement in current subquery/brackets
void    sq_query_append(SqQuery *query, unsigned int raw_args, ...);

// void sq_query_raw(SqQuery *query, const char *raw_or_format, ...);
#define sq_query_raw(query, ...)    \
		sq_query_append(query, SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)

// append printf format string to SQL statement in current subquery/brackets
// void sq_query_printf(SqQuery *query, const char *format, ...);
#define sq_query_printf(query, ...)    \
		sq_query_append(query, SQ_QUERYARGS_2, __VA_ARGS__)

// SQL: FROM
bool    sq_query_from(SqQuery *query, const char *table);

// bool sq_query_table(SqQuery *query, const char *table);
#define sq_query_table    sq_query_from

// bool sq_query_from_sub(SqQuery *query);
#define sq_query_from_sub(query)    \
		sq_query_from(query, NULL)

// SQL: AS
// call it after sq_query_from(), sq_query_join(), sq_query_select()
void    sq_query_as(SqQuery *query, const char *name);

// SQL: JOIN ON
void    sq_query_join_full(SqQuery *query, unsigned int join_args, const char *table, ...);
void    sq_query_on_logical(SqQuery *query, unsigned int logi_args, ...);

// void sq_query_join(SqQuery *query, const char *table, const char *column, const char *op_or_format, ...);
#define sq_query_join(query, table, ...)          \
		sq_query_join_full(query, SQ_QUERYJOIN_INNER | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), table, __VA_ARGS__)

// void sq_query_join_sub(SqQuery *query, ...);
#define sq_query_join_sub(query, ...)             \
		sq_query_join_full(query, SQ_QUERYJOIN_INNER | SQ_QUERYARGS_DECIDE(-1, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// void sq_query_left_join(SqQuery *query, const char *table, const char *column, const char *op_or_format, ...);
#define sq_query_left_join(query, table, ...)     \
		sq_query_join_full(query, SQ_QUERYJOIN_LEFT  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), table, __VA_ARGS__)

// void sq_query_left_join_sub(SqQuery *query, ...);
#define sq_query_left_join_sub(query, ...)        \
		sq_query_join_full(query, SQ_QUERYJOIN_LEFT  | SQ_QUERYARGS_DECIDE(-1, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// void sq_query_right_join(SqQuery *query, const char *table, const char *column, const char *op_or_format, ...);
#define sq_query_right_join(query, table, ...)    \
		sq_query_join_full(query, SQ_QUERYJOIN_RIGHT | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), table, __VA_ARGS__)

// void sq_query_right_join_sub(SqQuery *query, ...);
#define sq_query_right_join_sub(query, ...)       \
		sq_query_join_full(query, SQ_QUERYJOIN_RIGHT | SQ_QUERYARGS_DECIDE(-1, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// void sq_query_full_join(SqQuery *query, const char *table, const char *column, const char *op_or_format, ...);
#define sq_query_full_join(query, table, ...)     \
		sq_query_join_full(query, SQ_QUERYJOIN_FULL  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), table, __VA_ARGS__)

// void sq_query_full_join_sub(SqQuery *query, ...);
#define sq_query_full_join_sub(query, ...)        \
		sq_query_join_full(query, SQ_QUERYJOIN_FULL  | SQ_QUERYARGS_DECIDE(-1, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// void sq_query_cross_join(SqQuery *query, const char *table);
#define sq_query_cross_join(query, table)         \
		sq_query_join_full(query, SQ_QUERYJOIN_CROSS, table, NULL)

// void sq_query_cross_join_sub(SqQuery *query);
#define sq_query_cross_join_sub(query)            \
		sq_query_join_full(query, SQ_QUERYJOIN_CROSS, NULL,  NULL)

// void sq_query_on(SqQuery *query, const char *column, const char *op_or_format, ...);
#define sq_query_on(query, ...)            \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)
// void sq_query_or_on(SqQuery *query, const char *column, const char *op_or_format, ...);
#define sq_query_or_on(query, ...)         \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)

// void sq_query_on_raw(SqQuery *query, const char *raw_or_format, ...);
#define sq_query_on_raw(query, ...)        \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)
// void sq_query_or_on_raw(SqQuery *query, const char *raw_or_format, ...);
#define sq_query_or_on_raw(query, ...)     \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)

// void sq_query_on_sub(SqQuery *query, ...);
#define sq_query_on_sub(query, ...)        \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)
// void sq_query_or_on_sub(SqQuery *query, ...);
#define sq_query_or_on_sub(query, ...)     \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// SQL: WHERE
void    sq_query_where_logical(SqQuery *query, unsigned int logi_args, ...);

// void sq_query_where(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_where(query, ...)            \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)
// void sq_query_or_where(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_or_where(query, ...)         \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)

// void sq_query_where_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_where_raw(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)
// void sq_query_or_where_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_or_where_raw(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)

// void sq_query_where_sub(SqQuery *query, ...)
#define sq_query_where_sub(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)
// void sq_query_or_where_sub(SqQuery *query, ...)
#define sq_query_or_where_sub(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// SQL: WHERE NOT

// void sq_query_where_not(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_where_not(query, ...)            \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND_NOT | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)
// void sq_query_or_where_not(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_or_where_not(query, ...)         \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR_NOT  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)

// void sq_query_where_not_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_where_not_raw(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND_NOT | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)
// void sq_query_or_where_not_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_or_where_not_raw(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR_NOT  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)

// void sq_query_where_not_sub(SqQuery *query, ...)
#define sq_query_where_not_sub(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND_NOT | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)
// void sq_query_or_where_not_sub(SqQuery *query, ...)
#define sq_query_or_where_not_sub(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR_NOT  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// SQL: WHERE EXISTS
void    sq_query_where_exists_logical(SqQuery *query, unsigned int logi_args);

// void sq_query_where_exists(SqQuery *query);
#define sq_query_where_exists(query)          \
		sq_query_where_exists_logical(query, SQ_QUERYLOGI_AND)

// void sq_query_where_not_exists(SqQuery *query);
#define sq_query_where_not_exists(query)      \
		sq_query_where_exists_logical(query, SQ_QUERYLOGI_AND_NOT)

// SQL: WHERE column BETWEEN
void    sq_query_where_between_logical(SqQuery *query, const char *column_name, unsigned int logi_args, const char* format, ...);

// void sq_query_where_between(SqQuery *query, const char *column_name, const char *format, Value1, Value2);
#define sq_query_where_between(query, column_name, format, ...)           \
		sq_query_where_between_logical(query, column_name, SQ_QUERYLOGI_AND, format, __VA_ARGS__)

// void sq_query_where_not_between(SqQuery *query, const char *column_name, const char *format, Value1, Value2);
#define sq_query_where_not_between(query, column_name, format, ...)       \
		sq_query_where_between_logical(query, column_name, SQ_QUERYLOGI_AND_NOT, format, __VA_ARGS__)

// void sq_query_or_where_between(SqQuery *query, const char *column_name, const char *format, Value1, Value2);
#define sq_query_or_where_between(query, column_name, format, ...)        \
		sq_query_where_between_logical(query, column_name, SQ_QUERYLOGI_OR, format, __VA_ARGS__)

// void sq_query_or_where_not_between(SqQuery *query, const char *column_name, const char *format, Value1, Value2);
#define sq_query_or_where_not_between(query, column_name, format, ...)    \
		sq_query_where_between_logical(query, column_name, SQ_QUERYLOGI_OR_NOT, format, __VA_ARGS__)

// SQL: WHERE column IN
void    sq_query_where_in_logical(SqQuery *query, const char *column_name, unsigned int logi_args, int n_args, const char* format, ...);

// void sq_query_where_in(SqQuery *query, const char *column_name, int n_args, const char *format, ...);
#define sq_query_where_in(query, column_name, n_args, format, ...)           \
		sq_query_where_in_logical(query, column_name, SQ_QUERYLOGI_AND, (n_args) ? (n_args) : SQ_QUERYARGS_COUNT(__VA_ARGS__), format, __VA_ARGS__)

// void sq_query_where_not_in(SqQuery *query, const char *column_name, int n_args, const char *format, ...);
#define sq_query_where_not_in(query, column_name, n_args, format, ...)       \
		sq_query_where_in_logical(query, column_name, SQ_QUERYLOGI_AND_NOT, (n_args) ? (n_args) : SQ_QUERYARGS_COUNT(__VA_ARGS__), format, __VA_ARGS__)

// void sq_query_or_where_in(SqQuery *query, const char *column_name, int n_args, const char *format, ...);
#define sq_query_or_where_in(query, column_name, n_args, format, ...)        \
		sq_query_where_in_logical(query, column_name, SQ_QUERYLOGI_OR, (n_args) ? (n_args) : SQ_QUERYARGS_COUNT(__VA_ARGS__), format, __VA_ARGS__)

// void sq_query_or_where_not_in(SqQuery *query, const char *column_name, int n_args, const char *format, ...);
#define sq_query_or_where_not_in(query, column_name, n_args, format, ...)    \
		sq_query_where_in_logical(query, column_name, SQ_QUERYLOGI_OR_NOT,  (n_args) ? (n_args) : SQ_QUERYARGS_COUNT(__VA_ARGS__), format, __VA_ARGS__)

// SQL: WHERE column IS NULL
void    sq_query_where_null_logical(SqQuery *query, const char *column_name, unsigned int logi_args);

// void sq_query_where_null(SqQuery *query, const char *column_name);
#define sq_query_where_null(query, column_name)           \
		sq_query_where_null_logical(query, column_name, SQ_QUERYLOGI_AND);

// void sq_query_where_not_null(SqQuery *query, const char *column_name);
#define sq_query_where_not_null(query, column_name)       \
		sq_query_where_null_logical(query, column_name, SQ_QUERYLOGI_AND_NOT);

// void sq_query_or_where_null(SqQuery *query, const char *column_name);
#define sq_query_or_where_null(query, column_name)        \
		sq_query_where_null_logical(query, column_name, SQ_QUERYLOGI_OR);

// void sq_query_or_where_not_null(SqQuery *query, const char *column_name);
#define sq_query_or_where_not_null(query, column_name)    \
		sq_query_where_null_logical(query, column_name, SQ_QUERYLOGI_OR_NOT);

// SQL: GROUP BY
// the last argument of sq_query_group_by_list() must be NULL.
// e.g. sq_query_group_by_list(query, column_name..., NULL);
void    sq_query_group_by_list(SqQuery *query, ...);

// void sq_query_group_by(SqQuery *query, ...);
#define sq_query_group_by(query, ...)        \
		sq_query_group_by_list(query, __VA_ARGS__, NULL)

// void sq_query_group_by_raw(SqQuery *query, const char *raw);
#define sq_query_group_by_raw(query, raw)    \
		sq_query_group_by_list(query, raw, NULL)

// SQL: HAVING
void    sq_query_having_logical(SqQuery *query, unsigned int logi_args, ...);

// void sq_query_having(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_having(query, ...)            \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)
// void sq_query_or_having(SqQuery *query, const char *column, const char *op_or_format, ...)
#define sq_query_or_having(query, ...)         \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__), __VA_ARGS__)

// void sq_query_having_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_having_raw(query, ...)        \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)
// void sq_query_or_having_raw(SqQuery *query, const char *raw_or_format, ...)
#define sq_query_or_having_raw(query, ...)     \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__) | SQ_QUERYARGS_RAW, __VA_ARGS__)

// void sq_query_having_sub(SqQuery *query, ...)
#define sq_query_having_sub(query, ...)        \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)
// void sq_query_or_having_sub(SqQuery *query, ...)
#define sq_query_or_having_sub(query, ...)     \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR  | SQ_QUERYARGS_DECIDE( 0, __VA_ARGS__, NULL), ##__VA_ARGS__, NULL)

// SQL: SELECT
// the last argument of sq_query_select() must be NULL.
// e.g. sq_query_select_list(query, column_name..., NULL);
bool    sq_query_select_list(SqQuery *query, ...);
bool    sq_query_distinct(SqQuery *query);

// bool sq_query_select(SqQuery *query, ...);
#define sq_query_select(query, ...)        \
		sq_query_select_list(query, ##__VA_ARGS__, NULL)

// void sq_query_select_raw(SqQuery *query, const char *raw);
#define sq_query_select_raw(query, raw)    \
		sq_query_select_list(query, raw, NULL)

// SQL: ORDER BY
// the last argument of sq_query_order_by() must be NULL.
// e.g. sq_query_order_by_list(query, column_name..., NULL);
void    sq_query_order_by_list(SqQuery *query, ...);
void    sq_query_order_sorted(SqQuery *query, unsigned int sort_type);

// void sq_query_order_by(SqQuery *query, ...);
#define sq_query_order_by(query, ...)        \
		sq_query_order_by_list(query, __VA_ARGS__, NULL)

// void sq_query_order_by_raw(SqQuery *query, const char *raw);
#define sq_query_order_by_raw(query, raw)    \
		sq_query_order_by_list(query, raw, NULL)

// void sq_query_asc(SqQuery *query);
#define sq_query_asc(query)     \
		sq_query_order_sorted(query, SQ_QUERYSORT_ASC)
// void sq_query_desc(SqQuery *query);
#define sq_query_desc(query)    \
		sq_query_order_sorted(query, SQ_QUERYSORT_DESC)

void   *sq_query_union(SqQuery *query);
void    sq_query_union_all(SqQuery *query);

void   *sq_query_limit(SqQuery *query, int64_t count);
void    sq_query_offset(SqQuery *query, int64_t index);

// SQL: DELETE FROM
// call this function at last (before generating SQL statement).
void    sq_query_delete(SqQuery *query);

// SQL: TRUNCATE TABLE
// call this function at last (before generating SQL statement).
void    sq_query_truncate(SqQuery *query);

// generate SQL statements
// The result of sq_query_to_sql() must free when you don't need it.
char       *sq_query_to_sql(SqQuery *query);
// You can NOT free the result of sq_query_c(), it managed by SqQuery.
// After calling sq_query_c(), user can access SqQuery::str to reuse generated SQL statement.
const char *sq_query_c(SqQuery *query);

// const char *sq_query_last(SqQuery *query);
#define sq_query_last(query)        ((query)->str)

// get all of table_name and it's as_name in current SQL SELECT statement
// return number of tables in query.
// result array 'table_and_as_names':
//   element_0 = table1_name, element_1 = table1_as_name,
//   element_2 = table2_name, element_3 = table2_as_name, ...etc
//   elements are const string (const char*). User can't free elements in 'table_and_as_names'.
int   sq_query_get_table_as_names(SqQuery *query, SqPtrArray *table_and_as_names);

/*	select all columns in 'table', string format is - "table_as_name"."column" AS "table_as_name.column"
	parameter 'quotes' pointer to 'char array[2]'. If 'quotes' is NULL, it will default quote '"'. 
	If 'table_as_name' is NULL, it will use name of 'table' instead.

	This function used by SqTypeJoint and user custom type.
 */
void  sq_query_select_table_as(SqQuery *query, SqTable *table, const char *table_as_name, const char *quotes);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

/*	QueryMethod is used by SqQuery and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqQuery members.

	declare Sq::QueryMethod by using SqQuery-proxy.h
 */
#define SQPT_DECLARE
#define SQPT_USE_STRUCT
#define SQPT_NAME       QueryMethod
#define SQPT_RETURN     Query
#define SQPT_DATAPTR    this

#include <SqQuery-proxy.h>

#undef  SQPT_DECLARE
#undef  SQPT_USE_STRUCT
#undef  SQPT_NAME
#undef  SQPT_RETURN
#undef  SQPT_DATAPTR

// ----------------------------------------------------------------------------
//  C/C++ structure definition

/*	SqQueryNode - store elements of SQL Statements

    * If a clause has 1 children node only, It can omit the first NONE node.

    SELECT DISTINCT name, age AS a, email
    FROM  users  AS  u
    WHERE id > 40 AND age < 60


                  next             next
         SELECT -------> FROM --------------> WHERE
           |              |                     |
           |              |            children |
           |              |                     v             next
           |     children |                    NONE (or NOT) -----> AND
           |              |                     |                    |
           |              |                     |           children |
           |              |                     v                    v
  children |              v     next          "id > 40"           "age < 60"
           |             NONE  -----> AS
           |              |            |
           |              |            | children
           |              v            v
           |           "users"        "u"
           |
           v               next     next      next
       DISTINCT (or NONE) -----> , -----> AS -----> ,
           |                     |         |        |
  children |            children |         |        |
           v                     v         v        v
         "name"                "age"      "a"    "email"

   --------------------------------------------------------
    SqQueryNode digram for subquery/brackets

    SELECT ...
    FROM (SELECT ... FROM ... WHERE ...) AS  t
    WHERE ...

            next        next
    SELECT -----> FROM -----> WHERE
                   |
          children |
                   v    next
                  NONE -----> AS
                   |           |
                   |  children |
                   |           v
          children |          "t"
                   |
                   v          next          next
                   ( SELECT -------> FROM -------> WHERE )

   --------------------------------------------------------
    SqQueryNode digram for JOIN

    SELECT ... FROM ...
    JOIN (SELECT...) AS  t  ON  users.id = accounts.user_id  AND  ...

         next       next
    FROM ----> JOIN ----> ...
                |
       children |
                v    next        next
               NONE -----> AS   -----> ON
                |           |           |
                |           |  children |
                |           v           v
       children |          "t"         NONE -----> AND
                |                       |           |
                |                       |           v
                |                       |          ...
                |                       v
                |                      "users.id = accounts.user_id"
                |
                v          next          next
                ( SELECT -------> FROM -------> WHERE )

   --------------------------------------------------------
    SqQueryNode digram for ORDER BY, LIMIT, OFFSET

    WHERE  id < 10  OR NOT  city_id = 5
	ORDER BY  col1  ASC,  col2
	LIMIT  108  OFFSET  216

               next                      next
    WHERE --------------> ORDER BY  --------------->  LIMIT
      |                      |                          |
      |             children |                 children |
      v                      v   next                   v
     NONE ----> OR         NONE -----> ,              "108"
      |          |           |         |                |
      v          v           v         v                v
    "id < 10"   NOT        "col1"    "col2"           OFFSET
                 |           |                          |
                 v           v                          v
          "city_id = 5"     ASC (or DESC)             "216"

 */

struct SqQueryNode
{
	int            type;       // SqQueryNodeType
	SqQueryNode   *next;
	SqQueryNode   *children;   // arguments, nested, or inserted string
	char          *value;
};

/*	SqQuery is Query builder
 */
#define SQ_QUERY_MEMBERS          \
	SqQueryNode    root;          \
	SqQueryNode   *used;          \
	SqQueryNode   *freed;         \
	void          *node_chunk;    \
	int            node_count;    \
	SqQueryNested *nested_cur;    \
	int            nested_count;  \
	char          *str;           \
	int            length;        \
	int            allocated

#ifdef __cplusplus
struct SqQuery : Sq::QueryMethod             // <-- 1. inherit C++ member function(method)
#else
struct SqQuery
#endif
{
	SQ_QUERY_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ SqQuery members ------
	SqQueryNode    root;
	SqQueryNode   *used;          // used  SqQueryNode in node_chunk
	SqQueryNode   *freed;         // freed SqQueryNode (recycled)
	void          *node_chunk;    // The last of allocated chunk
	int            node_count;    // number of used nodes in chunks
	SqQueryNested *nested_cur;
	int            nested_count;

	// sq_query_c() and sq_query_to_sql() use these
	char          *str;
	int            length;
	int            allocated;
 */
};

// ----------------------------------------------------------------------------
// C++ definitions

/*
	define Sq::QueryMethod by using SqQuery-proxy.h
 */
#define SQPT_DEFINE
#define SQPT_USE_STRUCT
#define SQPT_NAME       QueryMethod
#define SQPT_RETURN     Query
#define SQPT_DATAPTR    this

#include <SqQuery-proxy.h>

#undef  SQPT_DEFINE
#undef  SQPT_USE_STRUCT
#undef  SQPT_NAME
#undef  SQPT_RETURN
#undef  SQPT_DATAPTR


#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

struct Query : SqQuery {
	Query(const char *table_name = NULL) {
		sq_query_init(this, table_name);
	}
	~Query() {
		sq_query_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C++ other definitions

/*	QueryProxy is used by convenient C++ class.

	define Sq::QueryProxy by using SqQuery-proxy.h
 */

#define SQPT_DECLARE
#define SQPT_DEFINE
#define SQPT_NAME       QueryProxy
#define SQPT_RETURN     QueryProxy
#define SQPT_DATAPTR    query

#include <SqQuery-proxy.h>

#undef  SQPT_DECLARE
#undef  SQPT_DEFINE
#undef  SQPT_NAME
#undef  SQPT_RETURN
#undef  SQPT_DATAPTR


#ifdef __cplusplus

namespace Sq {

/*	convenient C++ class for Sq::Storage

	// e.g. generate SQL statement "WHERE id < 15 OR city_id < 20"

	// 1. use operator() of Sq::where
	Sq::Where  where;
	std::cout << where("id", "<", "%d", 15).orWhereRaw("city_id < %d", 20).c();

	// 2. use parameter pack constructor
	std::cout << Sq::whereRaw("id < %d", 15).orWhereRaw("city_id < %d", 20).c();

	// 3. use default constructor and operator()
	std::cout << Sq::whereRaw()("id < %d", 15).orWhereRaw("city_id < %d", 20).c();
 */

/*
	QueryProxy
	|
	+--- Where
	|
	+--- WhereNot
	|
	+--- WhereRaw
	|
	+--- WhereNotRaw
	|
	+--- WhereExists
	|
	+--- WhereNotExists
	|
	+--- WhereBetween
	|
	+--- WhereNotBetween
	|
	+--- WhereIn
	|
	+--- WhereNotIn
	|
	+--- WhereNull
	|
	`--- WhereNotNull
 */

class Where : public Sq::QueryProxy
{
public:
	// constructor
	Where(std::function<void(SqQuery *query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_sub(query);     // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);       // end of subquery/brackets
	}
	Where(std::function<void(SqQuery &query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_sub(query);     // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);       // end of subquery/brackets
	}
	template <typename Lambda>
	Where(const char *column, Lambda func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_sub(query, column, "=");    // start of subquery/brackets
		func(*query);                              // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                   // end of subquery/brackets
	}
	Where(const char *column, int value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, "=", "%d", value);
	}
	Where(const char *column, int64_t value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, "=", "%" PRId64, value);
	}
	Where(const char *column, double value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, "=", "%f", value);
	}
	Where(const char *column, const char *value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, "=", "%s", value);
	}
	template <typename Lambda>
	Where(const char *column, const char *op, Lambda func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_sub(query, column, op);     // start of subquery/brackets
		func(*query);                              // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                   // end of subquery/brackets
	}
	Where(const char *column, const char *op, int value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, op, "%d", value);
	}
	Where(const char *column, const char *op, int64_t value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, op, "%" PRId64, value);
	}
	Where(const char *column, const char *op, double value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, op, "%f", value);
	}
	//// This method must match overloaded function. It can NOT use template specialization.
	Where(const char *column, const char *op_or_format, const char *value) {
		// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, op_or_format, value);
	}
	template <typename... Args>
	Where(const char *column, const char *op, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where(query, column, op, format, args...);
	}
	Where(const char *raw) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_raw(query, raw);
	}
	Where() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~Where() {
		sq_query_free(query);
	}

	// operator
	Where &operator()(std::function<void(SqQuery *query)> func) {
		sq_query_where_sub(query);     // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);       // end of subquery/brackets
		return *this;
	}
	Where &operator()(std::function<void(SqQuery &query)> func) {
		sq_query_where_sub(query);     // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);       // end of subquery/brackets
		return *this;
	}
	template <typename Lambda>
	Where &operator()(const char *column, Lambda func) {
		sq_query_where_sub(query, column, "=");    // start of subquery/brackets
		func(*query);                              // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                   // end of subquery/brackets
		return *this;
	}
	Where &operator()(const char *column, int value) {
		sq_query_where(query, column, "=", "%d", value);
		return *this;
	}
	Where &operator()(const char *column, int64_t value) {
		sq_query_where(query, column, "=", "%" PRId64, value);
		return *this;
	}
	Where &operator()(const char *column, double value) {
		sq_query_where(query, column, "=", "%f", value);
		return *this;
	}
	Where &operator()(const char *column, const char *value) {
		sq_query_where(query, column, "=", "%s", value);
		return *this;
	}
	template <typename Lambda>
	Where &operator()(const char *column, const char *op, Lambda func) {
		sq_query_where_sub(query, column, op);     // start of subquery/brackets
		func(*query);                              // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                   // end of subquery/brackets
		return *this;
	}
	Where &operator()(const char *column, const char *op, int value) {
		sq_query_where(query, column, op, "%d", value);
		return *this;
	}
	Where &operator()(const char *column, const char *op, int64_t value) {
		sq_query_where(query, column, op, "%" PRId64, value);
		return *this;
	}
	Where &operator()(const char *column, const char *op, double value) {
		sq_query_where(query, column, op, "%f", value);
		return *this;
	}
	Where &operator()(const char *column, const char *op_or_format, const char *value) {
		// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
		sq_query_where(query, column, op_or_format, value);
		return *this;
	}
	template <typename... Args>
	Where &operator()(const char *column, const char *op, const char *format, const Args... args) {
		sq_query_where(query, column, op, format, args...);
		return *this;
	}
	Where &operator()(const char *raw) {
		sq_query_where_raw(query, raw);
		return *this;
	}
};

class WhereNot : public Sq::QueryProxy
{
public:
	// constructor
	WhereNot(std::function<void(SqQuery *query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_sub(query);      // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	WhereNot(std::function<void(SqQuery &query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_sub(query);      // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	template <typename Lambda>
	WhereNot(const char *column, Lambda func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_sub(query, column, "=");    // start of subquery/brackets
		func(*query);                                  // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                       // end of subquery/brackets
	}
	WhereNot(const char *column, int value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, "=", "%d", value);
	}
	WhereNot(const char *column, int64_t value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, "=", "%" PRId64, value);
	}
	WhereNot(const char *column, double value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, "=", "%f", value);
	}
	WhereNot(const char *column, const char *value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, "=", "%s", value);
	}
	template <typename Lambda>
	WhereNot(const char *column, const char *op, Lambda func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_sub(query, column, op);     // start of subquery/brackets
		func(*query);                                  // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                       // end of subquery/brackets
	}
	WhereNot(const char *column, const char *op, int value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, op, "%d", value);
	}
	WhereNot(const char *column, const char *op, int64_t value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, op, "%" PRId64, value);
	}
	WhereNot(const char *column, const char *op, double value) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, op, "%f", value);
	}
	WhereNot(const char *column, const char *op_or_format, const char *value) {
		// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, op_or_format, value);
	}
	template <typename... Args>
	WhereNot(const char *column, const char *op, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not(query, column, op, format, args...);
	}
	WhereNot(const char *raw) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_raw(query, raw);
	}
	WhereNot() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNot() {
		sq_query_free(query);
	}

	// operator
	WhereNot &operator()(std::function<void(SqQuery *query)> func) {
		sq_query_where_not_sub(query);      // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
	WhereNot &operator()(std::function<void(SqQuery &query)> func) {
		sq_query_where_not_sub(query);      // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
	template <typename Lambda>
	WhereNot &operator()(const char *column, Lambda func) {
		sq_query_where_not_sub(query, column, "=");    // start of subquery/brackets
		func(*query);                                  // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                       // end of subquery/brackets
		return *this;
	}
	WhereNot &operator()(const char *column, int value) {
		sq_query_where_not(query, column, "=", "%d", value);
		return *this;
	}
	WhereNot &operator()(const char *column, int64_t value) {
		sq_query_where_not(query, column, "=", "%" PRId64, value);
		return *this;
	}
	WhereNot &operator()(const char *column, double value) {
		sq_query_where_not(query, column, "=", "%f", value);
		return *this;
	}
	WhereNot &operator()(const char *column, const char *value) {
		sq_query_where_not(query, column, "=", "%s", value);
		return *this;
	}
	template <typename Lambda>
	WhereNot &operator()(const char *column, const char *op, Lambda func) {
		sq_query_where_not_sub(query, column, op);     // start of subquery/brackets
		func(*query);                                  // std::function< void (SqQuery &query) >
		sq_query_end_sub(query);                       // end of subquery/brackets
		return *this;
	}
	WhereNot &operator()(const char *column, const char *op, int value) {
		sq_query_where_not(query, column, op, "%d", value);
		return *this;
	}
	WhereNot &operator()(const char *column, const char *op, int64_t value) {
		sq_query_where_not(query, column, op, "%" PRId64, value);
		return *this;
	}
	WhereNot &operator()(const char *column, const char *op, double value) {
		sq_query_where_not(query, column, op, "%f", value);
		return *this;
	}
	WhereNot &operator()(const char *column, const char *op_or_format, const char *value) {
		// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
		sq_query_where_not(query, column, op_or_format, value);
		return *this;
	}
	template <typename... Args>
	WhereNot &operator()(const char *column, const char *op, const char *format, const Args... args) {
		sq_query_where_not(query, column, op, format, args...);
		return *this;
	}
	WhereNot &operator()(const char *raw) {
		sq_query_where_not_raw(query, raw);
		return *this;
	}
};

class WhereRaw : public Sq::QueryProxy
{
public:
	// constructor
	template <typename... Args>
	WhereRaw(const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_raw(query, format, args...);
	}
	WhereRaw(const char *raw) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_raw(query, raw);
	}
	WhereRaw() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereRaw() {
		sq_query_free(query);
	}

	// operator
	template <typename... Args>
	WhereRaw &operator()(const char *format, const Args... args) {
		sq_query_where_raw(query, format, args...);
		return *this;
	}
	WhereRaw &operator()(const char *raw) {
		sq_query_where_raw(query, raw);
		return *this;
	}
};

class WhereNotRaw : public Sq::QueryProxy
{
public:
	// constructor
	template <typename... Args>
	WhereNotRaw(const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_raw(query, format, args...);
	}
	WhereNotRaw(const char *raw) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_raw(query, raw);
	}
	WhereNotRaw() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNotRaw() {
		sq_query_free(query);
	}

	// operator
	template <typename... Args>
	WhereNotRaw &operator()(const char *format, const Args... args) {
		sq_query_where_not_raw(query, format, args...);
		return *this;
	}
	WhereNotRaw &operator()(const char *raw) {
		sq_query_where_not_raw(query, raw);
		return *this;
	}
};

class WhereExists : public Sq::QueryProxy
{
public:
	// constructor
	WhereExists(std::function<void(SqQuery *query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_exists(query);       // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	WhereExists(std::function<void(SqQuery &query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_exists(query);       // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	WhereExists() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereExists() {
		sq_query_free(query);
	}

	// operator
	WhereExists &operator()(std::function<void(SqQuery *query)> func) {
		sq_query_where_exists(query);       // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
	WhereExists &operator()(std::function<void(SqQuery &query)> func) {
		sq_query_where_exists(query);       // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
};

class WhereNotExists : public Sq::QueryProxy
{
public:
	// constructor
	WhereNotExists(std::function<void(SqQuery *query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_exists(query);   // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	WhereNotExists(std::function<void(SqQuery &query)> func) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_exists(query);   // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
	}
	WhereNotExists() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNotExists() {
		sq_query_free(query);
	}

	// operator
	WhereNotExists &operator()(std::function<void(SqQuery *query)> func) {
		sq_query_where_not_exists(query);   // start of subquery/brackets
		func(query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
	WhereNotExists &operator()(std::function<void(SqQuery &query)> func) {
		sq_query_where_not_exists(query);   // start of subquery/brackets
		func(*query);
		sq_query_end_sub(query);            // end of subquery/brackets
		return *this;
	}
};

class WhereBetween : public Sq::QueryProxy
{
public:
	// constructor
	WhereBetween(const char *columnName, int value1, int value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, "%d", value1, value2);
	}
	WhereBetween(const char *columnName, int64_t value1, int64_t value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, "%" PRId64, value1, value2);
	}
	WhereBetween(const char *columnName, double value1, double value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, "%f", value1, value2);
	}
	WhereBetween(const char *columnName, const char value1, const char value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, "'%c'", value1, value2);
	}
	WhereBetween(const char *columnName, const char *value1, const char *value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, "'%s'", value1, value2);
	}
	template <typename... Args>
	WhereBetween(const char *columnName, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_between(query, columnName, format, args...);
	}
	WhereBetween() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereBetween() {
		sq_query_free(query);
	}

	// operator
	WhereBetween &operator()(const char *columnName, int value1, int value2) {
		sq_query_where_between(query, columnName, "%d", value1, value2);
		return *this;
	}
	WhereBetween &operator()(const char *columnName, int64_t value1, int64_t value2) {
		sq_query_where_between(query, columnName, "%" PRId64, value1, value2);
		return *this;
	}
	WhereBetween &operator()(const char *columnName, double value1, double value2) {
		sq_query_where_between(query, columnName, "%f", value1, value2);
		return *this;
	}
	WhereBetween &operator()(const char *columnName, const char value1, const char value2) {
		sq_query_where_between(query, columnName, "'%c'", value1, value2);
		return *this;
	}
	WhereBetween &operator()(const char *columnName, const char *value1, const char *value2) {
		sq_query_where_between(query, columnName, "'%s'", value1, value2);
		return *this;
	}
	template <typename... Args>
	WhereBetween &operator()(const char *columnName, const char *format, const Args... args) {
		sq_query_where_between(query, columnName, format, args...);
		return *this;
	}
};

class WhereNotBetween : public Sq::QueryProxy
{
public:
	// constructor
	WhereNotBetween(const char *columnName, int value1, int value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, "%d", value1, value2);
	}
	WhereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, "%" PRId64, value1, value2);
	}
	WhereNotBetween(const char *columnName, double value1, double value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, "%f", value1, value2);
	}
	WhereNotBetween(const char *columnName, const char value1, const char value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, "'%c'", value1, value2);
	}
	WhereNotBetween(const char *columnName, const char *value1, const char *value2) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, "'%s'", value1, value2);
	}
	template <typename... Args>
	WhereNotBetween(const char *columnName, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_between(query, columnName, format, args...);
	}
	WhereNotBetween() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNotBetween() {
		sq_query_free(query);
	}

	// operator
	WhereNotBetween &operator()(const char *columnName, int value1, int value2) {
		sq_query_where_not_between(query, columnName, "%d", value1, value2);
		return *this;
	}
	WhereNotBetween &operator()(const char *columnName, int64_t value1, int64_t value2) {
		sq_query_where_not_between(query, columnName, "%" PRId64, value1, value2);
		return *this;
	}
	WhereNotBetween &operator()(const char *columnName, double value1, double value2) {
		sq_query_where_not_between(query, columnName, "%f", value1, value2);
		return *this;
	}
	WhereNotBetween &operator()(const char *columnName, const char value1, const char value2) {
		sq_query_where_not_between(query, columnName, "'%c'", value1, value2);
		return *this;
	}
	WhereNotBetween &operator()(const char *columnName, const char *value1, const char *value2) {
		sq_query_where_not_between(query, columnName, "'%s'", value1, value2);
		return *this;
	}
	template <typename... Args>
	WhereNotBetween &operator()(const char *columnName, const char *format, const Args... args) {
		sq_query_where_not_between(query, columnName, format, args...);
		return *this;
	}
};

class WhereIn : public Sq::QueryProxy
{
public:
	// constructor
	template <typename... Args>
	WhereIn(const char *columnName, int firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%d", firstValue, args...);
	}
	template <typename... Args>
	WhereIn(const char *columnName, int64_t firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	}
	template <typename... Args>
	WhereIn(const char *columnName, double firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%f", firstValue, args...);
	}
	template <typename... Args>
	WhereIn(const char *columnName, const char firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "'%c'", firstValue, args...);
	}
	template <typename... Args>
	WhereIn(const char *columnName, const char *firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "'%s'", firstValue, args...);
	}
	template <typename... Args>
	WhereIn(const char *columnName, int n_args, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          (n_args) ? n_args : sizeof...(args), format, args...);
	}
	WhereIn() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereIn() {
		sq_query_free(query);
	}

	// operator
	template <typename... Args>
	WhereIn &operator()(const char *columnName, int firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%d", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereIn &operator()(const char *columnName, int64_t firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereIn &operator()(const char *columnName, double firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "%f", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereIn &operator()(const char *columnName, const char firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "'%c'", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereIn &operator()(const char *columnName, const char *firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          sizeof...(args)+1, "'%s'", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereIn &operator()(const char *columnName, int n_args, const char *format, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND,
		                          (n_args) ? n_args : sizeof...(args), format, args...);
		return *this;
	}
};

class WhereNotIn : public Sq::QueryProxy
{
public:
	// constructor
	template <typename... Args>
	WhereNotIn(const char *columnName, int firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%d", firstValue, args...);
	}
	template <typename... Args>
	WhereNotIn(const char *columnName, int64_t firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	}
	template <typename... Args>
	WhereNotIn(const char *columnName, double firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%f", firstValue, args...);
	}
	template <typename... Args>
	WhereNotIn(const char *columnName, const char firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "'%c'", firstValue, args...);
	}
	template <typename... Args>
	WhereNotIn(const char *columnName, const char *firstValue, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "'%s'", firstValue, args...);
	}
	template <typename... Args>
	WhereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          (n_args) ? n_args : sizeof...(args), format, args...);
	}
	WhereNotIn() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNotIn() {
		sq_query_free(query);
	}

	// operator
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, int firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%d", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, int64_t firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, double firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "%f", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, const char firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "'%c'", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, const char *firstValue, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          sizeof...(args)+1, "'%s'", firstValue, args...);
		return *this;
	}
	template <typename... Args>
	WhereNotIn &operator()(const char *columnName, int n_args, const char *format, const Args... args) {
		sq_query_where_in_logical(query, columnName, SQ_QUERYLOGI_AND_NOT,
		                          (n_args) ? n_args : sizeof...(args), format, args...);
		return *this;
	}
};

class WhereNull : public Sq::QueryProxy
{
public:
	// constructor
	WhereNull(const char *columnName) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_null(query, columnName);
	}
	WhereNull() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNull() {
		sq_query_free(query);
	}

	// operator
	WhereNull &operator()(const char *columnName) {
		sq_query_where_null(query, columnName);
		return *this;
	}
};

class WhereNotNull : public Sq::QueryProxy
{
public:
	// constructor
	WhereNotNull(const char *columnName) {
		query = (Sq::Query*)sq_query_new(NULL);
		sq_query_where_not_null(query, columnName);
	}
	WhereNotNull() {
		query = (Sq::Query*)sq_query_new(NULL);
	}

	// destructor
	~WhereNotNull() {
		sq_query_free(query);
	}

	// operator
	WhereNotNull &operator()(const char *columnName) {
		sq_query_where_not_null(query, columnName);
		return *this;
	}
};

typedef Where              where;
typedef WhereNot           whereNot;

typedef WhereRaw           whereRaw;
typedef WhereNotRaw        whereNotRaw;

typedef WhereExists        whereExists;
typedef WhereNotExists     whereNotExists;

typedef WhereBetween       whereBetween;
typedef WhereNotBetween    whereNotBetween;

typedef WhereIn            whereIn;
typedef WhereNotIn         whereNotIn;

typedef WhereNull          whereNull;
typedef WhereNotNull       whereNotNull;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_QUERY_H
