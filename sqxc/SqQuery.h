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

#include <SqPtrArray.h>    // used by sq_query_get_table_as_names()

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqQuery           SqQuery;
typedef struct SqQueryNode       SqQueryNode;
typedef struct SqQueryNested     SqQueryNested;

// ----------------------------------------------------------------------------
// C data and functions declaration

#ifdef __cplusplus
extern "C" {
#endif

// join on, where, having
extern const int SQ_QUERYLOGI_OR;
extern const int SQ_QUERYLOGI_AND;
// order by
extern const int SQ_QUERYSORT_ASC;
extern const int SQ_QUERYSORT_DESC;

/*	SqQuery C functions


	** below functions support printf format string in 2nd argument:
		sq_query_printf(),
		sq_query_join(),
		sq_query_on(),     sq_query_or_on(),
		sq_query_where(),  sq_query_or_where(),
		sq_query_having(), sq_query_or_having(),

	If you want to use SQL Wildcard Characters '%' in these functions, you must print “%” using “%%”.

	// e.g. "WHERE id < 100"
	sq_query_where(query, "id < %d", 100);


	** below function must use with Subquery
		sq_query_where_exists(),

	// e.g. "WHERE EXISTS ( SELECT * FROM table WHERE id > 20 )"
	sq_query_where_exists(query);               // start of Subquery
	sq_query_from("table");
	sq_query_where("id > 20", NULL);
	sq_query_pop_nested(query);                 // end of Subquery


	** below function must use with other query
		sq_query_union(),
		sq_query_union_all()


	** below function support Subquery/Nested: (2nd argument must be NULL)
		sq_query_from();
		sq_query_join();
		sq_query_on();     sq_query_or_on();
		sq_query_where();  sq_query_or_where();
		sq_query_having(), sq_query_or_having(),

	// e.g. "WHERE (salary > 45 AND age < 21)"
	sq_query_where(query, NULL);                // start of Subquery/Nested
	sq_query_where(query, "salary", ">", "45");
	sq_query_where(query, "age", "<", "21");
	sq_query_pop_nested(query);                 // end of Subquery/Nested


	** below function support SQL raw statements:
		sq_query_raw(),
		sq_query_select_raw(),
		sq_query_on_raw(),     sq_query_or_on_raw(),
		sq_query_where_raw(),  sq_query_or_where_raw(),
		sq_query_having_raw(), sq_query_or_having_raw(),
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

// append raw SQL statement in current nested/subquery
void    sq_query_append(SqQuery *query, const char *raw, SqQueryNode *parent);
void    sq_query_printf(SqQuery *query, ...);

// void sq_query_raw(SqQuery *query, const char *raw);
#define sq_query_raw(query, raw)    \
		sq_query_append(query, raw, NULL)

// SQL: FROM
bool    sq_query_from(SqQuery *query, const char *table);

// bool sq_query_table(SqQuery *query, const char *table);
#define sq_query_table    sq_query_from

// SQL: AS
// call it after sq_query_from(), sq_query_join(), sq_query_select()
void    sq_query_as(SqQuery *query, const char *name);

// SQL: JOIN ON
void    sq_query_join(SqQuery *query, const char *table, ...);
void    sq_query_on_logical(SqQuery *query, int sqn_type, ...);
void    sq_query_on_logical_raw(SqQuery *query, int sqn_type, const char *raw);

#define sq_query_on(query, ...)        \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_on(query, ...)     \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

#define sq_query_on_raw(query, raw)        \
		sq_query_on_logical_raw(query, SQ_QUERYLOGI_AND, raw)
#define sq_query_or_on_raw(query, raw)     \
		sq_query_on_logical_raw(query, SQ_QUERYLOGI_OR,  raw)

// SQL: WHERE
void    sq_query_where_logical(SqQuery *query, int sqn_type, ...);
void    sq_query_where_logical_raw(SqQuery *query, int sqn_type, const char *raw);
bool    sq_query_where_exists(SqQuery *query);

#define sq_query_where(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_where(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

#define sq_query_where_raw(query, raw)        \
		sq_query_where_logical_raw(query, SQ_QUERYLOGI_AND, raw)
#define sq_query_or_where_raw(query, raw)     \
		sq_query_where_logical_raw(query, SQ_QUERYLOGI_OR,  raw)

// SQL: GROUP BY
// the last argument of sq_query_group_by() must be NULL.
// e.g. sq_query_group_by(query, column_name..., NULL)
void    sq_query_group_by(SqQuery *query, ...);

#define sq_query_group_by_raw(query, raw)    \
		sq_query_group_by(query, raw, NULL)

// SQL: HAVING
void    sq_query_having_logical(SqQuery *query, int sqn_type, ...);
void    sq_query_having_logical_raw(SqQuery *query, int sqn_type, const char *raw);

#define sq_query_having(query, ...)        \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_having(query, ...)     \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

#define sq_query_having_raw(query, raw)        \
		sq_query_having_logical_raw(query, SQ_QUERYLOGI_AND, raw)
#define sq_query_or_having_raw(query, raw)     \
		sq_query_having_logical_raw(query, SQ_QUERYLOGI_OR,  raw)

// SQL: SELECT
// the last argument of sq_query_select() must be NULL.
// e.g. sq_query_select(query, column_name..., NULL)
bool    sq_query_select(SqQuery *query, ...);
bool    sq_query_distinct(SqQuery *query);

#define sq_query_select_raw(query, raw)    \
		sq_query_select(query, raw, NULL)

// SQL: ORDER BY
// the last argument of sq_query_order_by() must be NULL.
// e.g. sq_query_order_by(query, column_name..., NULL)
void    sq_query_order_by(SqQuery *query, ...);
void    sq_query_order_sorted(SqQuery *query, int sqn_type);

#define sq_query_order_by_raw(query, raw)    \
		sq_query_order_by(query, raw, NULL)

#define sq_query_asc(query)     \
		sq_query_order_sorted(query, SQ_QUERYSORT_ASC)
#define sq_query_desc(query)    \
		sq_query_order_sorted(query, SQ_QUERYSORT_DESC)

void   *sq_query_union(SqQuery *query);
void    sq_query_union_all(SqQuery *query);

// SQL: DELETE FROM
// call this function at last (before generating SQL statement).
void    sq_query_delete(SqQuery *query);

// SQL: TRUNCATE TABLE
// call this function at last (before generating SQL statement).
void    sq_query_truncate(SqQuery *query);

// generate SQL statements
char   *sq_query_to_sql(SqQuery *query);


// get all of table_name and it's as_name in current SQL SELECT statement
// return number of tables in query.
// result array 'table_and_as_names':
//   element_0 = table1_name, element_1 = table1_as_name,
//   element_2 = table2_name, element_3 = table2_as_name, ...etc
//   elements are const string (const char*). User can't free elements in 'table_and_as_names'.
int     sq_query_get_table_as_names(SqQuery *query, SqPtrArray *table_and_as_names);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

struct Query;

/*	QueryMethod is used by SqQuery and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqQuery members.
 */
struct QueryMethod
{
	Sq::Query *operator->();

	Sq::Query& clear();

	template <typename... Args>
	Sq::Query& raw(const char *format, const Args... args);
	Sq::Query& raw(const char *raw_sql);

	Sq::Query& from(const char *table);
	Sq::Query& from(std::function<void()> func);

	Sq::Query& table(const char *table);
	Sq::Query& table(std::function<void()> func);

	Sq::Query& as(const char *name);

	// join(table, condition...)
	template <typename... Args>
	Sq::Query& join(const char *table, const Args... args);
	Sq::Query& join(std::function<void()> func);

	// on(condition, ...)
	template <typename... Args>
	Sq::Query& on(const char *condition, const Args ...args);
	Sq::Query& on(std::function<void()> func);

	Sq::Query& onRaw(const char *raw);

	// orOn(condition, ...)
	template <typename... Args>
	Sq::Query& orOn(const char *condition, const Args... args);
	Sq::Query& orOn(std::function<void()> func);

	Sq::Query& orOnRaw(const char *raw);

	// where(condition, ...)
	template <typename... Args>
	Sq::Query& where(const char *condition, const Args... args);
	Sq::Query& where(std::function<void()> func);

	Sq::Query& whereRaw(const char *raw);

	// orWhere(condition, ...)
	template <typename... Args>
	Sq::Query& orWhere(const char *condition, const Args... args);
	Sq::Query& orWhere(std::function<void()> func);

	Sq::Query& orWhereRaw(const char *raw);

	Sq::Query& whereExists(std::function<void()> func);

	// groupBy(column...)
	template <typename... Args>
	Sq::Query& groupBy(const Args... args);
	Sq::Query& groupByRaw(const char *raw);

	// having(condition, ...)
	template <typename... Args>
	Sq::Query& having(const char *condition, const Args... args);
	Sq::Query& having(std::function<void()> func);

	Sq::Query& havingRaw(const char *raw);

	// orHaving(condition, ...)
	template <typename... Args>
	Sq::Query& orHaving(const char *condition, const Args... args);
	Sq::Query& orHaving(std::function<void()> func);

	Sq::Query& orHavingRaw(const char *raw);

	// select(column...)
	template <typename... Args>
	Sq::Query& select(const Args... args);
	Sq::Query& selectRaw(const char *raw);
	Sq::Query& distinct();

	// orderBy(column...)
	template <typename... Args>
	Sq::Query& orderBy(const Args... args);
	Sq::Query& orderByRaw(const char *raw);
	Sq::Query& orderByDesc(const char *column_name);
	Sq::Query& asc();
	Sq::Query& desc();

	// union(lambda function)
	Sq::Query& union_(std::function<void()> func);
	Sq::Query& unionAll(std::function<void()> func);

	// call these function at last (before generating SQL statement).
	Sq::Query& delete_();
	Sq::Query& deleteFrom();
	Sq::Query& truncate();

	// generate SQL statement
	char *toSql();

	// deprecated functions
	Sq::Query& from(std::function<void(SqQuery& query)> func);
	Sq::Query& table(std::function<void(SqQuery& query)> func);
	Sq::Query& join(std::function<void(SqQuery& query)> func);
	Sq::Query& on(std::function<void(SqQuery& query)> func);
	Sq::Query& orOn(std::function<void(SqQuery& query)> func);
	Sq::Query& where(std::function<void(SqQuery& query)> func);
	Sq::Query& orWhere(std::function<void(SqQuery& query)> func);
	Sq::Query& whereExists(std::function<void(SqQuery& query)> func);
	Sq::Query& having(std::function<void(SqQuery& query)> func);
	Sq::Query& orHaving(std::function<void(SqQuery& query)> func);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
//  C/C++ structue definition

/*	SqQueryNode - store elements of SQL Statements

    SELECT DISTINCT name, age
    FROM  User AS c
    WHERE id > 40 AND age < 60


                 next          next
        SELECT -------> FROM -------> WHERE
          |              |              |
          |              |     children |
          |              |              |
          |     children |              v      next       next
          |              |          "id > 40" -----> AND -----> "age < 60"
          |              |
 children |              v     next     next
          |            "User" -----> AS -----> "c"
          v
      DISTINCT -----> "name" -----> , -----> "age"
                next          next     next

   --------------------------------------------------------
    SqQueryNode digram for nested query (subquery)

    SELECT ... FROM (SELECT ... FROM ... WHERE ...) AS "t1" WHERE ...

            next        next
    SELECT -----> FROM -----> WHERE
                   |
          children |
                   v    next      next
                  NONE -----> AS -----> "t1"
          children |
                   v          next          next
                   ( SELECT -------> FROM -------> WHERE )
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
	SqQueryNode   *node;          \
	SqQueryNode   *freed;         \
	void          *node_chunk;    \
	int            node_count;    \
	SqQueryNested *nested_cur;    \
	int            nested_count

#ifdef __cplusplus
struct SqQuery : Sq::QueryMethod             // <-- 1. inherit C++ member function(method)
#else
struct SqQuery
#endif
{
	SQ_QUERY_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ SqQuery members ------
	SqQueryNode    root;
	SqQueryNode   *node;
	SqQueryNode   *freed;      // freed SqQueryNode.
	void          *node_chunk;
	int            node_count;
	SqQueryNested *nested_cur;
	int            nested_count;
 */
};

// ----------------------------------------------------------------------------
// C++ other definitions

#ifdef __cplusplus

namespace Sq {

/* define QueryMethod functions. */

inline Sq::Query *QueryMethod::operator->() {
	return (Sq::Query*)this;
}

inline Sq::Query& QueryMethod::clear() {
	sq_query_clear((SqQuery*)this);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query& QueryMethod::raw(const char *format, const Args... args) {
	sq_query_printf((SqQuery*)this, format, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query& QueryMethod::raw(const char *raw_sql) {
	sq_query_raw((SqQuery*)this, raw_sql);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::from(const char *table) {
	sq_query_from((SqQuery*)this, table);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::from(std::function<void()> func) {
	sq_query_from((SqQuery*)this, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::table(const char *table) {
	sq_query_from((SqQuery*)this, table);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::table(std::function<void()> func) {
	sq_query_from((SqQuery*)this, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::as(const char *name) {
	sq_query_as((SqQuery*)this, name);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::join(const char *table, const Args... args) {
	sq_query_join((SqQuery*)this, table, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::join(std::function<void()> func) {
	sq_query_join((SqQuery*)this, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::on(const char *condition, const Args... args) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_AND, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::on(std::function<void()> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::onRaw(const char *raw) {
	sq_query_on_logical_raw((SqQuery*)this, SQ_QUERYLOGI_AND, raw);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::orOn(const char *condition, const Args... args) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_OR, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orOn(std::function<void()> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orOnRaw(const char *raw) {
	sq_query_on_logical_raw((SqQuery*)this, SQ_QUERYLOGI_OR, raw);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::where(const char *condition, const Args... args) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_AND, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::where(std::function<void()> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::whereRaw(const char *raw) {
	sq_query_where_logical_raw((SqQuery*)this, SQ_QUERYLOGI_AND, raw);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::orWhere(const char *condition, const Args... args) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_OR, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orWhere(std::function<void()> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orWhereRaw(const char *raw) {
	sq_query_where_logical_raw((SqQuery*)this, SQ_QUERYLOGI_OR, raw);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::whereExists(std::function<void()> func) {
	sq_query_where_exists((SqQuery*)this);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::groupBy(const Args... args) {
	sq_query_group_by((SqQuery*)this, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::groupByRaw(const char *raw) {
	sq_query_group_by((SqQuery*)this, raw, NULL);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::having(const char *condition, const Args... args) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_AND, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::having(std::function<void()> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::havingRaw(const char *raw) {
	sq_query_having_logical_raw((SqQuery*)this, SQ_QUERYLOGI_AND, raw);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::orHaving(const char *condition, const Args... args) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_OR, condition, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orHaving(std::function<void()> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orHavingRaw(const char *raw) {
	sq_query_having_logical_raw((SqQuery*)this, SQ_QUERYLOGI_OR, raw);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::select(const Args... args) {
	sq_query_select((SqQuery*)this, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::selectRaw(const char *raw) {
	sq_query_select((SqQuery*)this, raw, NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::distinct() {
	sq_query_distinct((SqQuery*)this);
	return *(Sq::Query*)this;
}

template <typename... Args>
inline Sq::Query&  QueryMethod::orderBy(const Args... args) {
	sq_query_order_by((SqQuery*)this, args..., NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orderByRaw(const char *raw) {
	sq_query_order_by((SqQuery*)this, raw, NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orderByDesc(const char *column_name) {
	sq_query_order_by((SqQuery*)this, column_name, NULL);
	sq_query_desc((SqQuery*)this);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::asc() {
	sq_query_asc((SqQuery*)this);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::desc() {
	sq_query_desc((SqQuery*)this);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::union_(std::function<void()> func) {
	sq_query_union((SqQuery*)this);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::unionAll(std::function<void()> func) {
	sq_query_union_all((SqQuery*)this);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::delete_() {
	sq_query_delete((SqQuery*)this);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::deleteFrom() {
	sq_query_delete((SqQuery*)this);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::truncate() {
	sq_query_truncate((SqQuery*)this);
	return *(Sq::Query*)this;
}

inline char *QueryMethod::toSql() {
	return sq_query_to_sql((SqQuery*)this);
}

// deprecated QueryMethod functions

inline Sq::Query&  QueryMethod::table(std::function<void(SqQuery& query)> func) {
	sq_query_from((SqQuery*)this, NULL);    // start of Subquery/Nested
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::join(std::function<void(SqQuery& query)> func) {
	sq_query_join((SqQuery*)this, NULL);    // start of Subquery/Nested
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::on(std::function<void(SqQuery& query)> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orOn(std::function<void(SqQuery& query)> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::where(std::function<void(SqQuery& query)> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orWhere(std::function<void(SqQuery& query)> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::whereExists(std::function<void(SqQuery& query)> func) {
	sq_query_where_exists((SqQuery*)this);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::having(std::function<void(SqQuery& query)> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orHaving(std::function<void(SqQuery& query)> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func(*(SqQuery*)this);
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

/* All derived struct/class must be C++11 standard-layout. */

struct Query : SqQuery {
	Query() {
		sq_query_init(this, NULL);
	}
	~Query() {
		sq_query_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_QUERY_H
