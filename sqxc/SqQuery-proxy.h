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


#ifndef SQ_QUERY_PROXY_H
#define SQ_QUERY_PROXY_H

#ifndef SQ_QUERY_H
#include <SqQuery.h>
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

class QueryProxy {
protected:
	Sq::Query*  query;

public:
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
	// leftJoin(table, condition...)
	template <typename... Args>
	Sq::Query& leftJoin(const char *table, const Args... args);
	Sq::Query& leftJoin(std::function<void()> func);
	// rightJoin(table, condition...)
	template <typename... Args>
	Sq::Query& rightJoin(const char *table, const Args... args);
	Sq::Query& rightJoin(std::function<void()> func);
	// fullJoin(table, condition...)
	template <typename... Args>
	Sq::Query& fullJoin(const char *table, const Args... args);
	Sq::Query& fullJoin(std::function<void()> func);
	// crossJoin(table)
	Sq::Query& crossJoin(const char *table);
	Sq::Query& crossJoin(std::function<void()> func);

	// on(condition, ...)
	template <typename... Args>
	Sq::Query& on(const char *condition, const Args ...args);
	Sq::Query& on(std::function<void()> func);

	Sq::Query& on(const char *raw);
	Sq::Query& onRaw(const char *raw);

	// orOn(condition, ...)
	template <typename... Args>
	Sq::Query& orOn(const char *condition, const Args... args);
	Sq::Query& orOn(std::function<void()> func);

	Sq::Query& orOn(const char *raw);
	Sq::Query& orOnRaw(const char *raw);

	// where(condition, ...)
	template <typename... Args>
	Sq::Query& where(const char *condition, const Args... args);
	Sq::Query& where(std::function<void()> func);

	Sq::Query& where(const char *raw);
	Sq::Query& whereRaw(const char *raw);

	// orWhere(condition, ...)
	template <typename... Args>
	Sq::Query& orWhere(const char *condition, const Args... args);
	Sq::Query& orWhere(std::function<void()> func);

	Sq::Query& orWhere(const char *raw);
	Sq::Query& orWhereRaw(const char *raw);

	Sq::Query& whereExists(std::function<void()> func);

	// groupBy(column...)
	template <typename... Args>
	Sq::Query& groupBy(const char *column_name, const Args... args);
	Sq::Query& groupBy(const char *raw);
	Sq::Query& groupByRaw(const char *raw);

	// having(condition, ...)
	template <typename... Args>
	Sq::Query& having(const char *condition, const Args... args);
	Sq::Query& having(std::function<void()> func);

	Sq::Query& having(const char *raw);
	Sq::Query& havingRaw(const char *raw);

	// orHaving(condition, ...)
	template <typename... Args>
	Sq::Query& orHaving(const char *condition, const Args... args);
	Sq::Query& orHaving(std::function<void()> func);

	Sq::Query& orHaving(const char *raw);
	Sq::Query& orHavingRaw(const char *raw);

	// select(column...)
	template <typename... Args>
	Sq::Query& select(const char *column_name, const Args... args);
	Sq::Query& select(const char *raw);
	Sq::Query& selectRaw(const char *raw);
	Sq::Query& distinct();

	// orderBy(column...)
	template <typename... Args>
	Sq::Query& orderBy(const char *column_name, const Args... args);
	Sq::Query& orderBy(const char *raw);
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
	char       *toSql();
	const char *c();

	// return generated SQL statement
	const char *str();
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions

#ifdef __cplusplus

namespace Sq {

/* define QueryPtr functions. */

inline Sq::Query *QueryProxy::operator->() {
	return query;
}

inline Sq::Query& QueryProxy::clear() {
	sq_query_clear(query);
	return *query;
}

template <typename... Args>
inline Sq::Query& QueryProxy::raw(const char *format, const Args... args) {
	sq_query_printf(query, format, args...);
	return *query;
}
inline Sq::Query& QueryProxy::raw(const char *raw_sql) {
	sq_query_raw(query, raw_sql);
	return *query;
}

inline Sq::Query&  QueryProxy::from(const char *table) {
	sq_query_from(query, table);
	return *query;
}
inline Sq::Query&  QueryProxy::from(std::function<void()> func) {
	sq_query_from(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}
inline Sq::Query&  QueryProxy::table(const char *table) {
	sq_query_from(query, table);
	return *query;
}
inline Sq::Query&  QueryProxy::table(std::function<void()> func) {
	sq_query_from(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::as(const char *name) {
	sq_query_as(query, name);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::join(const char *table, const Args... args) {
	sq_query_join(query, table, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::join(std::function<void()> func) {
	sq_query_join(query, NULL);          // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *query;
}
template <typename... Args>
inline Sq::Query&  QueryProxy::leftJoin(const char *table, const Args... args) {
	sq_query_left_join(query, table, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::leftJoin(std::function<void()> func) {
	sq_query_left_join(query, NULL);     // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *query;
}
template <typename... Args>
inline Sq::Query&  QueryProxy::rightJoin(const char *table, const Args... args) {
	sq_query_right_join(query, table, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::rightJoin(std::function<void()> func) {
	sq_query_right_join(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *query;
}
template <typename... Args>
inline Sq::Query&  QueryProxy::fullJoin(const char *table, const Args... args) {
	sq_query_full_join(query, table, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::fullJoin(std::function<void()> func) {
	sq_query_full_join(query, NULL);     // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *query;
}
inline Sq::Query&  QueryProxy::crossJoin(const char *table) {
	sq_query_cross_join(query, table);
	return *query;
}
inline Sq::Query&  QueryProxy::crossJoin(std::function<void()> func) {
	sq_query_cross_join(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::on(const char *condition, const Args... args) {
	sq_query_on_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::on(std::function<void()> func) {
	sq_query_on_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::on(const char *raw) {
	sq_query_on_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::onRaw(const char *raw) {
	sq_query_on_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::orOn(const char *condition, const Args... args) {
	sq_query_on_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::orOn(std::function<void()> func) {
	sq_query_on_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::orOn(const char *raw) {
	sq_query_or_on_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::orOnRaw(const char *raw) {
	sq_query_or_on_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::where(const char *condition, const Args... args) {
	sq_query_where_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::where(std::function<void()> func) {
	sq_query_where_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::where(const char *raw) {
	sq_query_where_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::whereRaw(const char *raw) {
	sq_query_where_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::orWhere(const char *condition, const Args... args) {
	sq_query_where_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::orWhere(std::function<void()> func) {
	sq_query_where_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::orWhere(const char *raw) {
	sq_query_or_where_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::orWhereRaw(const char *raw) {
	sq_query_or_where_raw(query, raw);
	return *query;
}

inline Sq::Query&  QueryProxy::whereExists(std::function<void()> func) {
	sq_query_where_exists(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::groupBy(const char *column_name, const Args... args) {
	sq_query_group_by(query, column_name, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::groupBy(const char *raw) {
	sq_query_group_by_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::groupByRaw(const char *raw) {
	sq_query_group_by_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::having(const char *condition, const Args... args) {
	sq_query_having_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::having(std::function<void()> func) {
	sq_query_having_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::having(const char *raw) {
	sq_query_having_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::havingRaw(const char *raw) {
	sq_query_having_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::orHaving(const char *condition, const Args... args) {
	sq_query_having_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *query;
}
inline Sq::Query&  QueryProxy::orHaving(std::function<void()> func) {
	sq_query_having_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::orHaving(const char *raw) {
	sq_query_or_having_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::orHavingRaw(const char *raw) {
	sq_query_or_having_raw(query, raw);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::select(const char *column_name, const Args... args) {
	sq_query_select(query, column_name, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::select(const char *raw) {
	sq_query_select_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::selectRaw(const char *raw) {
	sq_query_select_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::distinct() {
	sq_query_distinct(query);
	return *query;
}

template <typename... Args>
inline Sq::Query&  QueryProxy::orderBy(const char *column_name, const Args... args) {
	sq_query_order_by(query, column_name, args..., NULL);
	return *query;
}
inline Sq::Query&  QueryProxy::orderBy(const char *raw) {
	sq_query_order_by_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::orderByRaw(const char *raw) {
	sq_query_order_by_raw(query, raw);
	return *query;
}
inline Sq::Query&  QueryProxy::orderByDesc(const char *column_name) {
	sq_query_order_by(query, column_name, NULL);
	sq_query_desc(query);
	return *query;
}
inline Sq::Query&  QueryProxy::asc() {
	sq_query_asc(query);
	return *query;
}
inline Sq::Query&  QueryProxy::desc() {
	sq_query_desc(query);
	return *query;
}

inline Sq::Query&  QueryProxy::union_(std::function<void()> func) {
	sq_query_union(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}
inline Sq::Query&  QueryProxy::unionAll(std::function<void()> func) {
	sq_query_union_all(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *query;
}

inline Sq::Query&  QueryProxy::delete_() {
	sq_query_delete(query);
	return *query;
}
inline Sq::Query&  QueryProxy::deleteFrom() {
	sq_query_delete(query);
	return *query;
}
inline Sq::Query&  QueryProxy::truncate() {
	sq_query_truncate(query);
	return *query;
}

inline char *QueryProxy::toSql() {
	return sq_query_to_sql(query);
}
inline const char *QueryProxy::c() {
	return sq_query_c(query);
}

inline const char *QueryProxy::str() {
	return query->str;
}

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_QUERY_PROXY_H
