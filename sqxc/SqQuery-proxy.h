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


/*	This header file is used by SqQuery.h

	Before including this file, you must define the settings as follows:

#define SQ_QUERY_TYPE_DECLARE
#define SQ_QUERY_TYPE_DEFINE
#define SQ_QUERY_TYPE_USE_STRUCT
#define SQ_QUERY_TYPE_NAME       QueryProxy
#define SQ_QUERY_TYPE_DATAPTR    query
#define SQ_QUERY_TYPE_RETURN     QueryProxy
 */


// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#if defined(__cplusplus) && defined(SQ_QUERY_TYPE_DECLARE)

namespace Sq {

struct Query;

#ifdef SQ_QUERY_TYPE_USE_STRUCT
// struct QueryMethod
struct SQ_QUERY_TYPE_NAME {

#else
// class  QueryProxy
class  SQ_QUERY_TYPE_NAME {

protected:
	Sq::Query *SQ_QUERY_TYPE_DATAPTR;

public:
#endif  // SQ_QUERY_TYPE_USE_STRUCT

// struct QueryMethod SQ_QUERY_TYPE_RETURN is 'Query'
// class  QueryProxy  SQ_QUERY_TYPE_RETURN is 'QueryProxy'
	SQ_QUERY_TYPE_RETURN  *operator->();

	SQ_QUERY_TYPE_RETURN  &clear();

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &raw(const char *format, const Args... args);
	SQ_QUERY_TYPE_RETURN  &raw(const char *raw_sql);

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &printf(const char *format, const Args... args);

	SQ_QUERY_TYPE_RETURN  &from(const char *table);
	SQ_QUERY_TYPE_RETURN  &from(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &table(const char *table);
	SQ_QUERY_TYPE_RETURN  &table(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &as(const char *name);

	// join(table, condition...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &join(const char *table, const Args... args);
	SQ_QUERY_TYPE_RETURN  &join(std::function<void()> func);
	// leftJoin(table, condition...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &leftJoin(const char *table, const Args... args);
	SQ_QUERY_TYPE_RETURN  &leftJoin(std::function<void()> func);
	// rightJoin(table, condition...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &rightJoin(const char *table, const Args... args);
	SQ_QUERY_TYPE_RETURN  &rightJoin(std::function<void()> func);
	// fullJoin(table, condition...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &fullJoin(const char *table, const Args... args);
	SQ_QUERY_TYPE_RETURN  &fullJoin(std::function<void()> func);
	// crossJoin(table)
	SQ_QUERY_TYPE_RETURN  &crossJoin(const char *table);
	SQ_QUERY_TYPE_RETURN  &crossJoin(std::function<void()> func);

	// on(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &on(const char *condition, const Args ...args);
	SQ_QUERY_TYPE_RETURN  &on(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &on(const char *raw);
	SQ_QUERY_TYPE_RETURN  &onRaw(const char *raw);

	// orOn(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orOn(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &orOn(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &orOn(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orOnRaw(const char *raw);

	// where(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &where(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &where(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &where(const char *raw);
	SQ_QUERY_TYPE_RETURN  &whereRaw(const char *raw);

	// orWhere(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhere(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &orWhere(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &orWhere(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orWhereRaw(const char *raw);

	// whereNot(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNot(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &whereNot(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &whereNot(const char *raw);
	SQ_QUERY_TYPE_RETURN  &whereNotRaw(const char *raw);

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNot(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &orWhereNot(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &orWhereNot(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orWhereNotRaw(const char *raw);

	// whereExists
	SQ_QUERY_TYPE_RETURN  &whereExists(std::function<void()> func);
	SQ_QUERY_TYPE_RETURN  &whereNotExists(std::function<void()> func);

	// whereBetween
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, int value1, int value2);
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, double value1, double value2);
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, const char value1, const char value2);
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereBetween(const char *columnName, const char *format, const Args... args);

	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, int value1, int value2);
	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, double value1, double value2);
	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, const char value1, const char value2);
	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotBetween(const char *columnName, const char *format, const Args... args);

	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, int value1, int value2);
	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, double value1, double value2);
	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, const char value1, const char value2);
	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereBetween(const char *columnName, const char *format, const Args... args);

	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, int value1, int value2);
	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, double value1, double value2);
	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, const char value1, const char value2);
	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotBetween(const char *columnName, const char *format, const Args... args);

	// whereIn
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, int first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, int64_t first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, double first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, const char first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, const char *first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereIn(const char *columnName, int n_args, const char *format, const Args... args);

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, int first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, int64_t first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, double first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, const char first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, const char *first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &whereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, int first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, int64_t first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, double first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, const char first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, const char *first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereIn(const char *columnName, int n_args, const char *format, const Args... args);

	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, int first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, int64_t first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, double first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, const char first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, const char *first_value, const Args... args);
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	// whereNull
	SQ_QUERY_TYPE_RETURN  &whereNull(const char *columnName);
	SQ_QUERY_TYPE_RETURN  &whereNotNull(const char *columnName);
	SQ_QUERY_TYPE_RETURN  &orWhereNull(const char *columnName);
	SQ_QUERY_TYPE_RETURN  &orWhereNotNull(const char *columnName);

	// groupBy(column...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &groupBy(const char *columnName, const Args... args);
	SQ_QUERY_TYPE_RETURN  &groupBy(const char *raw);
	SQ_QUERY_TYPE_RETURN  &groupByRaw(const char *raw);

	// having(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &having(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &having(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &having(const char *raw);
	SQ_QUERY_TYPE_RETURN  &havingRaw(const char *raw);

	// orHaving(condition, ...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orHaving(const char *condition, const Args... args);
	SQ_QUERY_TYPE_RETURN  &orHaving(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &orHaving(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orHavingRaw(const char *raw);

	// select(column...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &select(const char *columnName, const Args... args);
	SQ_QUERY_TYPE_RETURN  &select(const char *raw);
	SQ_QUERY_TYPE_RETURN  &selectRaw(const char *raw);
	SQ_QUERY_TYPE_RETURN  &distinct();

	// orderBy(column...)
	template <typename... Args>
	SQ_QUERY_TYPE_RETURN  &orderBy(const char *columnName, const Args... args);
	SQ_QUERY_TYPE_RETURN  &orderBy(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orderByRaw(const char *raw);
	SQ_QUERY_TYPE_RETURN  &orderByDesc(const char *columnName);
	SQ_QUERY_TYPE_RETURN  &asc();
	SQ_QUERY_TYPE_RETURN  &desc();

	// union(lambda function)
	SQ_QUERY_TYPE_RETURN  &union_(std::function<void()> func);
	SQ_QUERY_TYPE_RETURN  &unionAll(std::function<void()> func);

	SQ_QUERY_TYPE_RETURN  &limit(int64_t count);
	SQ_QUERY_TYPE_RETURN  &offset(int64_t index);

	// call these function at last (before generating SQL statement).
	SQ_QUERY_TYPE_RETURN  &delete_();
	SQ_QUERY_TYPE_RETURN  &deleteFrom();
	SQ_QUERY_TYPE_RETURN  &truncate();

	// generate SQL statement
	char       *toSql();
	const char *c();

	// return generated SQL statement
	const char *last();
};

};  // namespace Sq

#endif  // __cplusplus && SQ_QUERY_TYPE_DECLARE

// ----------------------------------------------------------------------------
// C++ definitions

#if defined(__cplusplus) && defined(SQ_QUERY_TYPE_DEFINE)

namespace Sq {

/*
	define functions.
	
	struct QueryMethod SQ_QUERY_TYPE_DATAPTR is 'this'
	class  QueryProxy  SQ_QUERY_TYPE_DATAPTR is 'query'
 */

inline SQ_QUERY_TYPE_RETURN  *SQ_QUERY_TYPE_NAME::operator->() {
	return (SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::clear() {
	sq_query_clear((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::raw(const char *format, const Args... args) {
	sq_query_printf((SqQuery*)SQ_QUERY_TYPE_DATAPTR, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::raw(const char *raw_sql) {
	sq_query_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw_sql);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::printf(const char *format, const Args... args) {
	sq_query_printf((SqQuery*)SQ_QUERY_TYPE_DATAPTR, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::from(const char *table) {
	sq_query_from((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::from(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);            // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::table(const char *table) {
	sq_query_from((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::table(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);            // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::as(const char *name) {
	sq_query_as((SqQuery*)SQ_QUERY_TYPE_DATAPTR, name);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::join(const char *table, const Args... args) {
	sq_query_join((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::join(std::function<void()> func) {
	sq_query_join_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);            // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::leftJoin(const char *table, const Args... args) {
	sq_query_left_join((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::leftJoin(std::function<void()> func) {
	sq_query_left_join_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::rightJoin(const char *table, const Args... args) {
	sq_query_right_join((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::rightJoin(std::function<void()> func) {
	sq_query_right_join_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);      // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::fullJoin(const char *table, const Args... args) {
	sq_query_full_join((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::fullJoin(std::function<void()> func) {
	sq_query_full_join_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::crossJoin(const char *table) {
	sq_query_cross_join((SqQuery*)SQ_QUERY_TYPE_DATAPTR, table);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::crossJoin(std::function<void()> func) {
	sq_query_cross_join_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);      // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::on(const char *condition, const Args... args) {
	sq_query_on((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::on(std::function<void()> func) {
	sq_query_on_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);        // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::on(const char *raw) {
	sq_query_on_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::onRaw(const char *raw) {
	sq_query_on_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orOn(const char *condition, const Args... args) {
	sq_query_or_on((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orOn(std::function<void()> func) {
	sq_query_or_on_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);     // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orOn(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orOnRaw(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::where(const char *condition, const Args... args) {
	sq_query_where((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::where(std::function<void()> func) {
	sq_query_where_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);     // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::where(const char *raw) {
	sq_query_where_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereRaw(const char *raw) {
	sq_query_where_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhere(const char *condition, const Args... args) {
	sq_query_or_where((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhere(std::function<void()> func) {
	sq_query_or_where_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);        // start of Subquery/Nested
	func();
	sq_query_pop_nested((SqQuery*)SQ_QUERY_TYPE_DATAPTR);          // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhere(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereRaw(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNot(const char *condition, const Args... args) {
	sq_query_where_not((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNot(std::function<void()> func) {
	sq_query_where_not_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNot(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotRaw(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNot(const char *condition, const Args... args) {
	sq_query_or_where_not((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNot(std::function<void()> func) {
	sq_query_or_where_not_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);    // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNot(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotRaw(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereExists(std::function<void()> func) {
	sq_query_where_exists((SqQuery*)SQ_QUERY_TYPE_DATAPTR);        // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotExists(std::function<void()> func) {
	sq_query_where_not_exists((SqQuery*)SQ_QUERY_TYPE_DATAPTR);    // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, int value1, int value2) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%d", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, double value1, double value2) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%f", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%d", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%f", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%d", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%f", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%d", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "%f", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_not_between((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, int first_value, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%d", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, int64_t first_value, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%" PRId64, first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, double first_value, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%f", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, const char first_value, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%c'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, const char *first_value, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%s'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, int first_value, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%d", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, int64_t first_value, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%" PRId64, first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, double first_value, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%f", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, const char first_value, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%c'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, const char *first_value, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%s'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, int first_value, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%d", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, int64_t first_value, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%" PRId64, first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, double first_value, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%f", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, const char first_value, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%c'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, const char *first_value, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%s'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_or_where_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, int first_value, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%d", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, int64_t first_value, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%" PRId64, first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, double first_value, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "%f", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, const char first_value, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%c'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, const char *first_value, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, sizeof...(args)+1, "'%s'", first_value, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_or_where_not_in((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNull(const char *columnName) {
	sq_query_where_null((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::whereNotNull(const char *columnName) {
	sq_query_where_not_null((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNull(const char *columnName) {
	sq_query_or_where_null((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orWhereNotNull(const char *columnName) {
	sq_query_or_where_not_null((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::groupBy(const char *columnName, const Args... args) {
	sq_query_group_by((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::groupBy(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::groupByRaw(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::having(const char *condition, const Args... args) {
	sq_query_having((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::having(std::function<void()> func) {
	sq_query_having_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);    // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::having(const char *raw) {
	sq_query_having_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::havingRaw(const char *raw) {
	sq_query_having_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orHaving(const char *condition, const Args... args) {
	sq_query_or_having((SqQuery*)SQ_QUERY_TYPE_DATAPTR, condition, args...);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orHaving(std::function<void()> func) {
	sq_query_or_having_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);       // start of Subquery/Nested
	func();
	sq_query_end_sub((SqQuery*)SQ_QUERY_TYPE_DATAPTR);             // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orHaving(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orHavingRaw(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::select(const char *columnName, const Args... args) {
	sq_query_select((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::select(const char *raw) {
	sq_query_select_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::selectRaw(const char *raw) {
	sq_query_select_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::distinct() {
	sq_query_distinct((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

template <typename... Args>
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orderBy(const char *columnName, const Args... args) {
	sq_query_order_by((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, args..., NULL);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orderBy(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orderByRaw(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQ_QUERY_TYPE_DATAPTR, raw);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::orderByDesc(const char *columnName) {
	sq_query_order_by((SqQuery*)SQ_QUERY_TYPE_DATAPTR, columnName, NULL);
	sq_query_desc((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::asc() {
	sq_query_asc((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::desc() {
	sq_query_desc((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::union_(std::function<void()> func) {
	sq_query_union((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	func();
	sq_query_pop_nested((SqQuery*)SQ_QUERY_TYPE_DATAPTR);    // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::unionAll(std::function<void()> func) {
	sq_query_union_all((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	func();
	sq_query_pop_nested((SqQuery*)SQ_QUERY_TYPE_DATAPTR);    // end of Subquery/Nested
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::limit(int64_t count) {
	sq_query_limit((SqQuery*)SQ_QUERY_TYPE_DATAPTR, count);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::offset(int64_t index) {
	sq_query_offset((SqQuery*)SQ_QUERY_TYPE_DATAPTR, index);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::delete_() {
	sq_query_delete((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::deleteFrom() {
	sq_query_delete((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}
inline SQ_QUERY_TYPE_RETURN  &SQ_QUERY_TYPE_NAME::truncate() {
	sq_query_truncate((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
	return *(SQ_QUERY_TYPE_RETURN*)this;
}

inline char *SQ_QUERY_TYPE_NAME::toSql() {
	return sq_query_to_sql((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
}
inline const char *SQ_QUERY_TYPE_NAME::c() {
	return sq_query_c((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
}

inline const char *SQ_QUERY_TYPE_NAME::last() {
	return sq_query_last((SqQuery*)SQ_QUERY_TYPE_DATAPTR);
}

};  // namespace Sq

#endif  // __cplusplus && SQ_QUERY_TYPE_DEFINE
