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
	Sq::Query *query;

public:
	QueryProxy *operator->();

	QueryProxy &clear();

	template <typename... Args>
	QueryProxy &raw(const char *format, const Args... args);
	QueryProxy &raw(const char *raw_sql);

	QueryProxy &from(const char *table);
	QueryProxy &from(std::function<void()> func);

	QueryProxy &table(const char *table);
	QueryProxy &table(std::function<void()> func);

	QueryProxy &as(const char *name);

	// join(table, condition...)
	template <typename... Args>
	QueryProxy &join(const char *table, const Args... args);
	QueryProxy &join(std::function<void()> func);
	// leftJoin(table, condition...)
	template <typename... Args>
	QueryProxy &leftJoin(const char *table, const Args... args);
	QueryProxy &leftJoin(std::function<void()> func);
	// rightJoin(table, condition...)
	template <typename... Args>
	QueryProxy &rightJoin(const char *table, const Args... args);
	QueryProxy &rightJoin(std::function<void()> func);
	// fullJoin(table, condition...)
	template <typename... Args>
	QueryProxy &fullJoin(const char *table, const Args... args);
	QueryProxy &fullJoin(std::function<void()> func);
	// crossJoin(table)
	QueryProxy &crossJoin(const char *table);
	QueryProxy &crossJoin(std::function<void()> func);

	// on(condition, ...)
	template <typename... Args>
	QueryProxy &on(const char *condition, const Args ...args);
	QueryProxy &on(std::function<void()> func);

	QueryProxy &on(const char *raw);
	QueryProxy &onRaw(const char *raw);

	// orOn(condition, ...)
	template <typename... Args>
	QueryProxy &orOn(const char *condition, const Args... args);
	QueryProxy &orOn(std::function<void()> func);

	QueryProxy &orOn(const char *raw);
	QueryProxy &orOnRaw(const char *raw);

	// where(condition, ...)
	template <typename... Args>
	QueryProxy &where(const char *condition, const Args... args);
	QueryProxy &where(std::function<void()> func);

	QueryProxy &where(const char *raw);
	QueryProxy &whereRaw(const char *raw);

	// orWhere(condition, ...)
	template <typename... Args>
	QueryProxy &orWhere(const char *condition, const Args... args);
	QueryProxy &orWhere(std::function<void()> func);

	QueryProxy &orWhere(const char *raw);
	QueryProxy &orWhereRaw(const char *raw);

	// whereExists
	QueryProxy &whereExists(std::function<void()> func);
	QueryProxy &whereNotExists(std::function<void()> func);

	// whereBetween
	QueryProxy &whereBetween(const char *columnName, int value1, int value2);
	QueryProxy &whereBetween(const char *columnName, int64_t value1, int64_t value2);
	QueryProxy &whereBetween(const char *columnName, double value1, double value2);
	QueryProxy &whereBetween(const char *columnName, const char value1, const char value2);
	QueryProxy &whereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	QueryProxy &whereBetween(const char *columnName, const char *format, const Args... args);

	QueryProxy &whereNotBetween(const char *columnName, int value1, int value2);
	QueryProxy &whereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	QueryProxy &whereNotBetween(const char *columnName, double value1, double value2);
	QueryProxy &whereNotBetween(const char *columnName, const char value1, const char value2);
	QueryProxy &whereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	QueryProxy &whereNotBetween(const char *columnName, const char *format, const Args... args);

	QueryProxy &orWhereBetween(const char *columnName, int value1, int value2);
	QueryProxy &orWhereBetween(const char *columnName, int64_t value1, int64_t value2);
	QueryProxy &orWhereBetween(const char *columnName, double value1, double value2);
	QueryProxy &orWhereBetween(const char *columnName, const char value1, const char value2);
	QueryProxy &orWhereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	QueryProxy &orWhereBetween(const char *columnName, const char *format, const Args... args);

	QueryProxy &orWhereNotBetween(const char *columnName, int value1, int value2);
	QueryProxy &orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	QueryProxy &orWhereNotBetween(const char *columnName, double value1, double value2);
	QueryProxy &orWhereNotBetween(const char *columnName, const char value1, const char value2);
	QueryProxy &orWhereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	QueryProxy &orWhereNotBetween(const char *columnName, const char *format, const Args... args);

	// groupBy(column...)
	template <typename... Args>
	QueryProxy &groupBy(const char *column_name, const Args... args);
	QueryProxy &groupBy(const char *raw);
	QueryProxy &groupByRaw(const char *raw);

	// having(condition, ...)
	template <typename... Args>
	QueryProxy &having(const char *condition, const Args... args);
	QueryProxy &having(std::function<void()> func);

	QueryProxy &having(const char *raw);
	QueryProxy &havingRaw(const char *raw);

	// orHaving(condition, ...)
	template <typename... Args>
	QueryProxy &orHaving(const char *condition, const Args... args);
	QueryProxy &orHaving(std::function<void()> func);

	QueryProxy &orHaving(const char *raw);
	QueryProxy &orHavingRaw(const char *raw);

	// select(column...)
	template <typename... Args>
	QueryProxy &select(const char *column_name, const Args... args);
	QueryProxy &select(const char *raw);
	QueryProxy &selectRaw(const char *raw);
	QueryProxy &distinct();

	// orderBy(column...)
	template <typename... Args>
	QueryProxy &orderBy(const char *column_name, const Args... args);
	QueryProxy &orderBy(const char *raw);
	QueryProxy &orderByRaw(const char *raw);
	QueryProxy &orderByDesc(const char *column_name);
	QueryProxy &asc();
	QueryProxy &desc();

	// union(lambda function)
	QueryProxy &union_(std::function<void()> func);
	QueryProxy &unionAll(std::function<void()> func);

	QueryProxy &limit(int64_t count);
	QueryProxy &offset(int64_t index);

	// call these function at last (before generating SQL statement).
	QueryProxy &delete_();
	QueryProxy &deleteFrom();
	QueryProxy &truncate();

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

inline QueryProxy *QueryProxy::operator->() {
	return this;
}

inline QueryProxy &QueryProxy::clear() {
	sq_query_clear(query);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::raw(const char *format, const Args... args) {
	sq_query_printf(query, format, args...);
	return *this;
}
inline QueryProxy &QueryProxy::raw(const char *raw_sql) {
	sq_query_raw(query, raw_sql);
	return *this;
}

inline QueryProxy &QueryProxy::from(const char *table) {
	sq_query_from(query, table);
	return *this;
}
inline QueryProxy &QueryProxy::from(std::function<void()> func) {
	sq_query_from(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}
inline QueryProxy &QueryProxy::table(const char *table) {
	sq_query_from(query, table);
	return *this;
}
inline QueryProxy &QueryProxy::table(std::function<void()> func) {
	sq_query_from(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::as(const char *name) {
	sq_query_as(query, name);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::join(const char *table, const Args... args) {
	sq_query_join(query, table, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::join(std::function<void()> func) {
	sq_query_join(query, NULL);          // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::leftJoin(const char *table, const Args... args) {
	sq_query_left_join(query, table, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::leftJoin(std::function<void()> func) {
	sq_query_left_join(query, NULL);     // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::rightJoin(const char *table, const Args... args) {
	sq_query_right_join(query, table, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::rightJoin(std::function<void()> func) {
	sq_query_right_join(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::fullJoin(const char *table, const Args... args) {
	sq_query_full_join(query, table, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::fullJoin(std::function<void()> func) {
	sq_query_full_join(query, NULL);     // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *this;
}
inline QueryProxy &QueryProxy::crossJoin(const char *table) {
	sq_query_cross_join(query, table);
	return *this;
}
inline QueryProxy &QueryProxy::crossJoin(std::function<void()> func) {
	sq_query_cross_join(query, NULL);    // start of Subquery/Nested
	func();
	sq_query_pop_nested(query);          // end of Subquery/Nested
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::on(const char *condition, const Args... args) {
	sq_query_on_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::on(std::function<void()> func) {
	sq_query_on_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::on(const char *raw) {
	sq_query_on_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::onRaw(const char *raw) {
	sq_query_on_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::orOn(const char *condition, const Args... args) {
	sq_query_on_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::orOn(std::function<void()> func) {
	sq_query_on_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::orOn(const char *raw) {
	sq_query_or_on_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::orOnRaw(const char *raw) {
	sq_query_or_on_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::where(const char *condition, const Args... args) {
	sq_query_where_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::where(std::function<void()> func) {
	sq_query_where_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::where(const char *raw) {
	sq_query_where_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::whereRaw(const char *raw) {
	sq_query_where_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::orWhere(const char *condition, const Args... args) {
	sq_query_where_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::orWhere(std::function<void()> func) {
	sq_query_where_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::orWhere(const char *raw) {
	sq_query_or_where_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereRaw(const char *raw) {
	sq_query_or_where_raw(query, raw);
	return *this;
}

inline QueryProxy &QueryProxy::whereExists(std::function<void()> func) {
	sq_query_where_exists(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}
inline QueryProxy &QueryProxy::whereNotExists(std::function<void()> func) {
	sq_query_where_not_exists(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::whereBetween(const char *columnName, int value1, int value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%d", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%" PRId64, value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereBetween(const char *columnName, double value1, double value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%f", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_between((SqQuery*)this, columnName, "'%c'", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%s", value1, value2);
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::whereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_between((SqQuery*)this, columnName, format, args...);
	return *this;
}

inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%d", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%" PRId64, value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%f", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "'%c'", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%s", value1, value2);
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::whereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_not_between((SqQuery*)this, columnName, format, args...);
	return *this;
}

inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, int value1, int value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%d", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%" PRId64, value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, double value1, double value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%f", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_between((SqQuery*)this, columnName, "'%c'", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_between((SqQuery*)this, columnName, "%s", value1, value2);
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::orWhereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_between((SqQuery*)this, columnName, format, args...);
	return *this;
}

inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%d", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%" PRId64, value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%f", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "'%c'", value1, value2);
	return *this;
}
inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_not_between((SqQuery*)this, columnName, "%s", value1, value2);
	return *this;
}
template <typename... Args>
inline QueryProxy &QueryProxy::orWhereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_not_between((SqQuery*)this, columnName, format, args...);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::groupBy(const char *column_name, const Args... args) {
	sq_query_group_by(query, column_name, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::groupBy(const char *raw) {
	sq_query_group_by_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::groupByRaw(const char *raw) {
	sq_query_group_by_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::having(const char *condition, const Args... args) {
	sq_query_having_logical(query, SQ_QUERYLOGI_AND, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::having(std::function<void()> func) {
	sq_query_having_logical(query, SQ_QUERYLOGI_AND, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::having(const char *raw) {
	sq_query_having_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::havingRaw(const char *raw) {
	sq_query_having_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::orHaving(const char *condition, const Args... args) {
	sq_query_having_logical(query, SQ_QUERYLOGI_OR, condition, args...);
	return *this;
}
inline QueryProxy &QueryProxy::orHaving(std::function<void()> func) {
	sq_query_having_logical(query, SQ_QUERYLOGI_OR, NULL);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::orHaving(const char *raw) {
	sq_query_or_having_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::orHavingRaw(const char *raw) {
	sq_query_or_having_raw(query, raw);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::select(const char *column_name, const Args... args) {
	sq_query_select(query, column_name, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::select(const char *raw) {
	sq_query_select_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::selectRaw(const char *raw) {
	sq_query_select_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::distinct() {
	sq_query_distinct(query);
	return *this;
}

template <typename... Args>
inline QueryProxy &QueryProxy::orderBy(const char *column_name, const Args... args) {
	sq_query_order_by(query, column_name, args..., NULL);
	return *this;
}
inline QueryProxy &QueryProxy::orderBy(const char *raw) {
	sq_query_order_by_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::orderByRaw(const char *raw) {
	sq_query_order_by_raw(query, raw);
	return *this;
}
inline QueryProxy &QueryProxy::orderByDesc(const char *column_name) {
	sq_query_order_by(query, column_name, NULL);
	sq_query_desc(query);
	return *this;
}
inline QueryProxy &QueryProxy::asc() {
	sq_query_asc(query);
	return *this;
}
inline QueryProxy &QueryProxy::desc() {
	sq_query_desc(query);
	return *this;
}

inline QueryProxy &QueryProxy::union_(std::function<void()> func) {
	sq_query_union(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}
inline QueryProxy &QueryProxy::unionAll(std::function<void()> func) {
	sq_query_union_all(query);
	func();
	sq_query_pop_nested(query);    // end of Subquery/Nested
	return *this;
}

inline QueryProxy &QueryProxy::limit(int64_t count) {
	sq_query_limit(query, count);
	return *this;
}
inline QueryProxy &QueryProxy::offset(int64_t index) {
	sq_query_offset(query, index);
	return *this;
}

inline QueryProxy &QueryProxy::delete_() {
	sq_query_delete(query);
	return *this;
}
inline QueryProxy &QueryProxy::deleteFrom() {
	sq_query_delete(query);
	return *this;
}
inline QueryProxy &QueryProxy::truncate() {
	sq_query_truncate(query);
	return *this;
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
