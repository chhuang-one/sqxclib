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

#include <stdarg.h>
#include <stdbool.h>

#include <SqPtrArray.h>    // used by sq_query_get_table_as_names()

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqQuery           SqQuery;
typedef struct SqQueryNode       SqQueryNode;
typedef struct SqQueryNested     SqQueryNested;

extern const uintptr_t SQ_QUERYLOGI_OR;
extern const uintptr_t SQ_QUERYLOGI_AND;

extern const uintptr_t SQ_QUERYSORT_ASC;
extern const uintptr_t SQ_QUERYSORT_DESC;

// ----------------------------------------------------------------------------
// C data and functions declaration

#ifdef __cplusplus
extern "C" {
#endif

/*	SqQuery C functions


	** below functions support printf format string in 2nd argument:
		sq_query_join(), sq_query_on(),     sq_query_or_on(),
		                 sq_query_where(),  sq_query_or_where(),
		                 sq_query_having(), sq_query_or_having(),

	If the 3rd argument of above C functions is NULL, the 2nd argument is handled as raw string.
	If you want to use SQL Wildcard Characters '%' in these functions, you must print “%” using “%%”.

	// e.g. "WHERE id < 100"
	sq_query_where(query, "id < %d", 100);


	** below function must use with Subquery
		sq_query_where_exists()

	// e.g. "WHERE EXISTS ( SELECT * FROM table WHERE id > 20 )"
	sq_query_where_exists(query);               // start of Subquery
	sq_query_from("table");
	sq_query_where("id > 20", NULL);
	sq_query_pop_nested(query);                 // end of Subquery


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


	** below function support SQL raw statements: (3rd argument must be NULL)
		sq_query_select(),
		sq_query_on(),     sq_query_or_on(),
		sq_query_where(),  sq_query_or_where(),
		sq_query_having(), sq_query_or_having(),
		sq_query_order_by(),
		sq_query_group_by(),

	// e.g. "WHERE id = 1"
	sq_query_where(query, "id = 1", NULL);


	** below function support SQL raw statements: (4th argument must be NULL)
		sq_query_join()

	// e.g. "JOIN city ON city.id > 10"
	sq_query_join(query, "city", "city.id > 10", NULL);

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

// SQL: FROM
bool    sq_query_from(SqQuery *query, const char *table);

// bool sq_query_table(SqQuery *query, const char *table);
#define sq_query_table    sq_query_from

// SQL: AS
// call it after sq_query_from(), sq_query_join(), sq_query_select()
void    sq_query_as(SqQuery *query, const char *name);

// SQL: JOIN ON
void    sq_query_join(SqQuery *query, const char *table, ...);
void    sq_query_on_logical(SqQuery *query, uintptr_t sqn_type, ...);

#define sq_query_on(query, ...)        \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_on(query, ...)     \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: WHERE
void    sq_query_where_logical(SqQuery *query, uintptr_t sqn_type, ...);
bool    sq_query_where_exists(SqQuery *query);

#define sq_query_where(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_where(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: GROUP BY
void    sq_query_group_by(SqQuery *query, ...);

// SQL: HAVING
void    sq_query_having_logical(SqQuery *query, uintptr_t sqn_type, ...);

#define sq_query_having(query, ...)        \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_having(query, ...)     \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: SELECT
bool    sq_query_select(SqQuery *query, ...);
bool    sq_query_distinct(SqQuery *query);

// SQL: ORDER BY
void    sq_query_order_by(SqQuery *query, ...);
void    sq_query_order_sorted(SqQuery *query, uintptr_t sqn_type);

#define sq_query_order_by_asc(query)     \
		sq_query_order_sorted(query, SQ_QUERYSORT_ASC)
#define sq_query_order_by_desc(query)    \
		sq_query_order_sorted(query, SQ_QUERYSORT_DESC)
#define sq_query_order_by_default(query) \
		sq_query_order_sorted(query, 0)

#define sq_query_asc     sq_query_order_by_asc
#define sq_query_desc    sq_query_order_by_desc

// SQL: DELETE FROM
// call this function at last (before generating SQL statement).
void    sq_query_delete(SqQuery *query);

// SQL: TRUNCATE TABLE
// call this function at last (before generating SQL statement).
void    sq_query_truncate(SqQuery *query);

// generate SQL statements
char   *sq_query_to_sql(SqQuery *query);

// va_list
void    sq_query_join_va(SqQuery *query, const char *table,
                         const char *condition, va_list arg_list);
void    sq_query_on_logical_va(SqQuery *query, uintptr_t sqn_type,
                               const char *condition, va_list arg_list);
void    sq_query_where_logical_va(SqQuery *query, uintptr_t sqn_type,
                                  const char *condition, va_list arg_list);
void    sq_query_group_by_va(SqQuery *query,
                             const char *column, va_list arg_list);
void    sq_query_having_logical_va(SqQuery *query, uintptr_t sqn_type,
                                   const char *condition, va_list arg_list);
bool    sq_query_select_va(SqQuery *query,
                           const char *column, va_list arg_list);
void    sq_query_order_by_va(SqQuery *query,
                             const char *column, va_list arg_list);

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

	Sq::Query& from(const char *table);
	Sq::Query& from(std::function<void()> func);

	Sq::Query& table(const char *table);
	Sq::Query& table(std::function<void()> func);

	Sq::Query& as(const char *name);

	Sq::Query& join(const char *table, const char *condition = NULL, ...);
	Sq::Query& join(std::function<void()> func);

	Sq::Query& on(const char *condition, ...);
	Sq::Query& on(std::function<void()> func);

	Sq::Query& onRaw(const char *raw);

	Sq::Query& orOn(const char *condition, ...);
	Sq::Query& orOn(std::function<void()> func);

	Sq::Query& orOnRaw(const char *raw);

	Sq::Query& where(const char *condition, ...);
	Sq::Query& where(std::function<void()> func);

	Sq::Query& whereRaw(const char *raw);

	Sq::Query& orWhere(const char *condition, ...);
	Sq::Query& orWhere(std::function<void()> func);

	Sq::Query& orWhereRaw(const char *raw);

	Sq::Query& whereExists(std::function<void()> func);

	Sq::Query& groupBy(const char *column, ...);
	Sq::Query& groupByRaw(const char *raw);

	Sq::Query& having(const char *condition, ...);
	Sq::Query& having(std::function<void()> func);

	Sq::Query& havingRaw(const char *raw);

	Sq::Query& orHaving(const char *condition, ...);
	Sq::Query& orHaving(std::function<void()> func);

	Sq::Query& orHavingRaw(const char *raw);

	Sq::Query& select(const char *column, ...);
	Sq::Query& selectRaw(const char *raw);
	Sq::Query& distinct();

	Sq::Query& orderBy(const char *column, ...);
	Sq::Query& orderByRaw(const char *raw);
	Sq::Query& asc();
	Sq::Query& desc();

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
	uintptr_t      type;       // SqQueryNodeType
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

inline Sq::Query&  QueryMethod::join(const char *table, const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_join_va((SqQuery*)this, table, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::join(std::function<void()> func) {
	sq_query_join((SqQuery*)this, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::on(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_on_logical_va((SqQuery*)this, SQ_QUERYLOGI_AND, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::on(std::function<void()> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::onRaw(const char *raw) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_AND, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orOn(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_on_logical_va((SqQuery*)this, SQ_QUERYLOGI_OR, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orOn(std::function<void()> func) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orOnRaw(const char *raw) {
	sq_query_on_logical((SqQuery*)this, SQ_QUERYLOGI_OR, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::where(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_where_logical_va((SqQuery*)this, SQ_QUERYLOGI_AND, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::where(std::function<void()> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::whereRaw(const char *raw) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_AND, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orWhere(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_where_logical_va((SqQuery*)this, SQ_QUERYLOGI_OR, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orWhere(std::function<void()> func) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orWhereRaw(const char *raw) {
	sq_query_where_logical((SqQuery*)this, SQ_QUERYLOGI_OR, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::whereExists(std::function<void()> func) {
	sq_query_where_exists((SqQuery*)this);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::groupBy(const char *column, ...) {
	va_list  arg_list;
	va_start(arg_list, column);
	sq_query_group_by_va((SqQuery*)this, column, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::groupByRaw(const char *raw) {
	sq_query_group_by_va((SqQuery*)this, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::having(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_having_logical_va((SqQuery*)this, SQ_QUERYLOGI_AND, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::having(std::function<void()> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::havingRaw(const char *raw) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_AND, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orHaving(const char *condition, ...) {
	va_list  arg_list;
	va_start(arg_list, condition);
	sq_query_having_logical_va((SqQuery*)this, SQ_QUERYLOGI_OR, condition, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orHaving(std::function<void()> func) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested((SqQuery*)this);    // end of Subquery/Nested
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::orHavingRaw(const char *raw) {
	sq_query_having_logical((SqQuery*)this, SQ_QUERYLOGI_OR, raw, NULL);
	return *(Sq::Query*)this;
}

inline Sq::Query&  QueryMethod::select(const char *column, ...) {
	va_list  arg_list;
	va_start(arg_list, column);
	sq_query_select_va((SqQuery*)this, column, arg_list);
	va_end(arg_list);
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

inline Sq::Query&  QueryMethod::orderBy(const char *column, ...) {
	va_list  arg_list;
	va_start(arg_list, column);
	sq_query_order_by_va((SqQuery*)this, column, arg_list);
	va_end(arg_list);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::orderByRaw(const char *raw) {
	sq_query_order_by((SqQuery*)this, raw, NULL);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::asc() {
	sq_query_order_by_asc((SqQuery*)this);
	return *(Sq::Query*)this;
}
inline Sq::Query&  QueryMethod::desc() {
	sq_query_order_by_desc((SqQuery*)this);
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
