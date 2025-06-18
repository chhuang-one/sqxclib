/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

	Before including this file, you can define the settings as follows:

	#define SQ_QUERY_METHOD_H_FOR_PROXY    1
	#define SQ_QUERY_METHOD_H_DECLARE      1
	#define SQ_QUERY_METHOD_H_DEFINE       1

	SQ_QUERY_METHOD_H_FOR_PROXY: To declare or define class QueryProxy
	SQ_QUERY_METHOD_H_DECLARE:   use declarations in SqQueryMethod.h
	SQ_QUERY_METHOD_H_DEFINE:    use definitions  in SqQueryMethod.h
 */

#if SQ_QUERY_METHOD_H_FOR_PROXY == 1
// class  QueryProxy
#define SQQM_NAME       QueryProxy
#define SQQM_RETURN     QueryProxy
#define SQQM_DATAPTR    data
#else
// struct QueryMethod
#define SQQM_NAME       QueryMethod
#define SQQM_RETURN     Query
#define SQQM_DATAPTR    this
#endif

#ifndef SQ_QUERY_METHOD_H_DECLARE
#define SQ_QUERY_METHOD_H_DECLARE    1
#endif

#ifndef SQ_QUERY_METHOD_H_DEFINE
#define SQ_QUERY_METHOD_H_DEFINE     1
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#if defined(__cplusplus) && (SQ_QUERY_METHOD_H_DECLARE == 1)

namespace Sq {

struct Query;

#if SQ_QUERY_METHOD_H_FOR_PROXY == 1

class  QueryProxy
{

protected:
	Sq::Query *SQQM_DATAPTR;

public:
	// return data pointer of QueryProxy
	Sq::Query    *query();

#else

struct QueryMethod
{

#endif  // End of SQ_QUERY_METHOD_H_FOR_PROXY

// struct QueryMethod SQQM_RETURN is 'Query'
// class  QueryProxy  SQQM_RETURN is 'QueryProxy'
	SQQM_RETURN  *operator->();

	SQQM_RETURN  &clear();

	template <typename... Args>
	SQQM_RETURN  &raw(const char *format, const Args... args);
	SQQM_RETURN  &raw(const char *raw_sql);

	// deprecated
	template <typename... Args>
	SQQM_RETURN  &printf(const char *format, const Args... args);

	SQQM_RETURN  &from(const char *table);
	SQQM_RETURN  &from(std::function<void()> func);
	SQQM_RETURN  &from(std::function<void(SqQuery &query)> func);

	SQQM_RETURN  &table(const char *table);
	SQQM_RETURN  &table(std::function<void()> func);
	SQQM_RETURN  &table(std::function<void(SqQuery &query)> func);

	SQQM_RETURN  &as(const char *name);

	// join(table, condition...)
	SQQM_RETURN  &join(std::function<void()> func);
	SQQM_RETURN  &join(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &join(const char *table, std::function<void()> func);
	SQQM_RETURN  &join(const char *table, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &join(const char *table, const char *column, std::function<void()> func);
	SQQM_RETURN  &join(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &join(const char *table, const char *column, int value);
	SQQM_RETURN  &join(const char *table, const char *column, int64_t value);
	SQQM_RETURN  &join(const char *table, const char *column, double value);
	SQQM_RETURN  &join(const char *table, const char *column, const char *value);
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, int value);
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, int64_t value);
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &join(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &join(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// leftJoin(table, condition...)
	SQQM_RETURN  &leftJoin(std::function<void()> func);
	SQQM_RETURN  &leftJoin(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &leftJoin(const char *table, std::function<void()> func);
	SQQM_RETURN  &leftJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, std::function<void()> func);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, int value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, int64_t value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, double value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, int value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, int64_t value);
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &leftJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// rightJoin(table, condition...)
	SQQM_RETURN  &rightJoin(std::function<void()> func);
	SQQM_RETURN  &rightJoin(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &rightJoin(const char *table, std::function<void()> func);
	SQQM_RETURN  &rightJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, std::function<void()> func);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, int value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, int64_t value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, double value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, int value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, int64_t value);
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &rightJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// fullJoin(table, condition...)
	SQQM_RETURN  &fullJoin(std::function<void()> func);
	SQQM_RETURN  &fullJoin(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &fullJoin(const char *table, std::function<void()> func);
	SQQM_RETURN  &fullJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, std::function<void()> func);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, int value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, int64_t value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, double value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, int value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, int64_t value);
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &fullJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// crossJoin(table)
	SQQM_RETURN  &crossJoin(const char *table);
	SQQM_RETURN  &crossJoin(std::function<void()> func);
	SQQM_RETURN  &crossJoin(std::function<void(SqQuery &query)> func);

	// on(condition, ...)
	SQQM_RETURN  &on(std::function<void()> func);
	SQQM_RETURN  &on(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &on(const char *column, std::function<void()> func);
	SQQM_RETURN  &on(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &on(const char *column, int value);
	SQQM_RETURN  &on(const char *column, int64_t value);
	SQQM_RETURN  &on(const char *column, double value);
	SQQM_RETURN  &on(const char *column, const char *value);
	SQQM_RETURN  &on(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &on(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &on(const char *column, const char *op, int value);
	SQQM_RETURN  &on(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &on(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &on(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &on(const char *column, const char *op, const char *format, const Args ...args);

	SQQM_RETURN  &on(const char *raw);
	SQQM_RETURN  &onRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &onRaw(const char *format, const Args ...args);

	// orOn(condition, ...)
	SQQM_RETURN  &orOn(std::function<void()> func);
	SQQM_RETURN  &orOn(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orOn(const char *column, std::function<void()> func);
	SQQM_RETURN  &orOn(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orOn(const char *column, int value);
	SQQM_RETURN  &orOn(const char *column, int64_t value);
	SQQM_RETURN  &orOn(const char *column, double value);
	SQQM_RETURN  &orOn(const char *column, const char *value);
	SQQM_RETURN  &orOn(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &orOn(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orOn(const char *column, const char *op, int value);
	SQQM_RETURN  &orOn(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &orOn(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &orOn(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &orOn(const char *column, const char *op, const char *format, const Args ...args);

	SQQM_RETURN  &orOn(const char *raw);
	SQQM_RETURN  &orOnRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &orOnRaw(const char *format, const Args... args);

	// where(condition, ...)
	SQQM_RETURN  &where(std::function<void()> func);
	SQQM_RETURN  &where(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &where(const char *column, std::function<void()> func);
	SQQM_RETURN  &where(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &where(const char *column, int value);
	SQQM_RETURN  &where(const char *column, int64_t value);
	SQQM_RETURN  &where(const char *column, double value);
	SQQM_RETURN  &where(const char *column, const char *value);
	SQQM_RETURN  &where(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &where(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &where(const char *column, const char *op, int value);
	SQQM_RETURN  &where(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &where(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &where(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &where(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &where(const char *raw);
	SQQM_RETURN  &whereRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &whereRaw(const char *format, const Args... args);

	// orWhere(condition, ...)
	SQQM_RETURN  &orWhere(std::function<void()> func);
	SQQM_RETURN  &orWhere(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhere(const char *column, std::function<void()> func);
	SQQM_RETURN  &orWhere(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhere(const char *column, int value);
	SQQM_RETURN  &orWhere(const char *column, int64_t value);
	SQQM_RETURN  &orWhere(const char *column, double value);
	SQQM_RETURN  &orWhere(const char *column, const char *value);
	SQQM_RETURN  &orWhere(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &orWhere(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhere(const char *column, const char *op, int value);
	SQQM_RETURN  &orWhere(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &orWhere(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &orWhere(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &orWhere(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &orWhere(const char *raw);
	SQQM_RETURN  &orWhereRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &orWhereRaw(const char *format, const Args... args);

	// whereNot(condition, ...)
	SQQM_RETURN  &whereNot(std::function<void()> func);
	SQQM_RETURN  &whereNot(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &whereNot(const char *column, std::function<void()> func);
	SQQM_RETURN  &whereNot(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &whereNot(const char *column, int value);
	SQQM_RETURN  &whereNot(const char *column, int64_t value);
	SQQM_RETURN  &whereNot(const char *column, double value);
	SQQM_RETURN  &whereNot(const char *column, const char *value);
	SQQM_RETURN  &whereNot(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &whereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &whereNot(const char *column, const char *op, int value);
	SQQM_RETURN  &whereNot(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &whereNot(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &whereNot(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &whereNot(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &whereNot(const char *raw);
	SQQM_RETURN  &whereNotRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &whereNotRaw(const char *format, const Args... args);

	// orWhereNot(condition, ...)
	SQQM_RETURN  &orWhereNot(std::function<void()> func);
	SQQM_RETURN  &orWhereNot(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhereNot(const char *column, std::function<void()> func);
	SQQM_RETURN  &orWhereNot(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhereNot(const char *column, int value);
	SQQM_RETURN  &orWhereNot(const char *column, int64_t value);
	SQQM_RETURN  &orWhereNot(const char *column, double value);
	SQQM_RETURN  &orWhereNot(const char *column, const char *value);
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, int value);
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &orWhereNot(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &orWhereNot(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &orWhereNot(const char *raw);
	SQQM_RETURN  &orWhereNotRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotRaw(const char *format, const Args... args);

	// whereExists
	SQQM_RETURN  &whereExists(std::function<void()> func);
	SQQM_RETURN  &whereExists(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &whereNotExists(std::function<void()> func);
	SQQM_RETURN  &whereNotExists(std::function<void(SqQuery &query)> func);

	// whereBetween
	SQQM_RETURN  &whereBetween(const char *columnName, int value1, int value2);
	SQQM_RETURN  &whereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQQM_RETURN  &whereBetween(const char *columnName, double value1, double value2);
	SQQM_RETURN  &whereBetween(const char *columnName, const char value1, const char value2);
	SQQM_RETURN  &whereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQQM_RETURN  &whereBetween(const char *columnName, const char *format, const Args... args);

	// whereNotBetween
	SQQM_RETURN  &whereNotBetween(const char *columnName, int value1, int value2);
	SQQM_RETURN  &whereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQQM_RETURN  &whereNotBetween(const char *columnName, double value1, double value2);
	SQQM_RETURN  &whereNotBetween(const char *columnName, const char value1, const char value2);
	SQQM_RETURN  &whereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQQM_RETURN  &whereNotBetween(const char *columnName, const char *format, const Args... args);

	// orWhereBetween
	SQQM_RETURN  &orWhereBetween(const char *columnName, int value1, int value2);
	SQQM_RETURN  &orWhereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQQM_RETURN  &orWhereBetween(const char *columnName, double value1, double value2);
	SQQM_RETURN  &orWhereBetween(const char *columnName, const char value1, const char value2);
	SQQM_RETURN  &orWhereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQQM_RETURN  &orWhereBetween(const char *columnName, const char *format, const Args... args);

	// orWhereNotBetween
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, int value1, int value2);
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, double value1, double value2);
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, const char value1, const char value2);
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotBetween(const char *columnName, const char *format, const Args... args);

	// whereIn
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereIn(const char *columnName, int n_args, const char *format, const Args... args);

	// whereNotIn
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &whereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	// orWhereIn
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereIn(const char *columnName, int n_args, const char *format, const Args... args);

	// orWhereNotIn
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQQM_RETURN  &orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	// whereNull
	SQQM_RETURN  &whereNull(const char *columnName);
	SQQM_RETURN  &whereNotNull(const char *columnName);
	SQQM_RETURN  &orWhereNull(const char *columnName);
	SQQM_RETURN  &orWhereNotNull(const char *columnName);

	// groupBy(column...)
	template <typename... Args>
	SQQM_RETURN  &groupBy(const char *columnName, const Args... args);
	SQQM_RETURN  &groupBy(const char *raw);
	SQQM_RETURN  &groupByRaw(const char *raw);

	// having(condition, ...)
	SQQM_RETURN  &having(std::function<void()> func);
	SQQM_RETURN  &having(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &having(const char *column, std::function<void()> func);
	SQQM_RETURN  &having(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &having(const char *column, int value);
	SQQM_RETURN  &having(const char *column, int64_t value);
	SQQM_RETURN  &having(const char *column, double value);
	SQQM_RETURN  &having(const char *column, const char *value);
	SQQM_RETURN  &having(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &having(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &having(const char *column, const char *op, int value);
	SQQM_RETURN  &having(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &having(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &having(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &having(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &having(const char *raw);
	SQQM_RETURN  &havingRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &havingRaw(const char *format, const Args... args);

	// orHaving(condition, ...)
	SQQM_RETURN  &orHaving(std::function<void()> func);
	SQQM_RETURN  &orHaving(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orHaving(const char *column, std::function<void()> func);
	SQQM_RETURN  &orHaving(const char *column, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orHaving(const char *column, int value);
	SQQM_RETURN  &orHaving(const char *column, int64_t value);
	SQQM_RETURN  &orHaving(const char *column, double value);
	SQQM_RETURN  &orHaving(const char *column, const char *value);
	SQQM_RETURN  &orHaving(const char *column, const char *op, std::function<void()> func);
	SQQM_RETURN  &orHaving(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &orHaving(const char *column, const char *op, int value);
	SQQM_RETURN  &orHaving(const char *column, const char *op, int64_t value);
	SQQM_RETURN  &orHaving(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQQM_RETURN  &orHaving(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQQM_RETURN  &orHaving(const char *column, const char *op, const char *format, const Args... args);

	SQQM_RETURN  &orHaving(const char *raw);
	SQQM_RETURN  &orHavingRaw(const char *raw);
	template <typename... Args>
	SQQM_RETURN  &orHavingRaw(const char *format, const Args... args);

	// select(column...)
	template <typename... Args>
	SQQM_RETURN  &select(const char *columnName, const Args... args);
	SQQM_RETURN  &select(const char *raw);
	SQQM_RETURN  &selectRaw(const char *raw);
	SQQM_RETURN  &distinct();

	// orderBy(column...)
	template <typename... Args>
	SQQM_RETURN  &orderBy(const char *columnName, const Args... args);
	SQQM_RETURN  &orderBy(const char *raw);
	SQQM_RETURN  &orderByRaw(const char *raw);
	SQQM_RETURN  &orderByDesc(const char *columnName);
	SQQM_RETURN  &asc();
	SQQM_RETURN  &desc();

	// union(lambda function)
	SQQM_RETURN  &union_(std::function<void()> func);
	SQQM_RETURN  &union_(std::function<void(SqQuery &query)> func);
	SQQM_RETURN  &unionAll(std::function<void()> func);
	SQQM_RETURN  &unionAll(std::function<void(SqQuery &query)> func);

	SQQM_RETURN  &limit(int64_t count);
	SQQM_RETURN  &offset(int64_t index);

	// call these function at last (before generating SQL statement).
	SQQM_RETURN  &delete_();
	SQQM_RETURN  &deleteFrom();
	SQQM_RETURN  &truncate();

	// return SQL command (SQ_QUERY_CMD_XXXX series) in query.
	// return SQ_QUERY_CMD_NONE if no SQL command in query.
	int         getCommand();

	// generate SQL statement
	char       *toSql();
	const char *c();

	// return generated SQL statement
	const char *last();
};

};  // namespace Sq

#endif  // __cplusplus && SQ_QUERY_METHOD_H_DECLARE == 1

// ----------------------------------------------------------------------------
// C++ definitions

#if defined(__cplusplus) && (SQ_QUERY_METHOD_H_DEFINE == 1)

namespace Sq {

/*
	define functions.

	struct QueryMethod SQQM_DATAPTR is 'this'
	class  QueryProxy  SQQM_DATAPTR is 'data'
 */

#if SQ_QUERY_METHOD_H_FOR_PROXY == 1
// return data pointer of QueryProxy
inline Sq::Query  *SQQM_NAME::query() {
	return (Sq::Query*)SQQM_DATAPTR;
}
#endif  // SQ_QUERY_METHOD_H_FOR_PROXY

inline SQQM_RETURN  *SQQM_NAME::operator->() {
	return (SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::clear() {
	sq_query_clear((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}

template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::raw(const char *format, const Args... args) {
	sq_query_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::raw(const char *raw_sql) {
	sq_query_raw((SqQuery*)SQQM_DATAPTR, raw_sql);
	return *(SQQM_RETURN*)this;
}

// deprecated
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::printf(const char *format, const Args... args) {
	sq_query_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::from(const char *table) {
	sq_query_from((SqQuery*)SQQM_DATAPTR, table);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::from(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::from(std::function<void(SqQuery &query)> func) {
	sq_query_from_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::table(const char *table) {
	sq_query_from((SqQuery*)SQQM_DATAPTR, table);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::table(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::table(std::function<void(SqQuery &query)> func) {
	sq_query_from_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::as(const char *name) {
	sq_query_as((SqQuery*)SQQM_DATAPTR, name);
	return *(SQQM_RETURN*)this;
}

// join(table, condition...)
inline SQQM_RETURN  &SQQM_NAME::join(std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, NULL);          // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR);            // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, NULL);          // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, int value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, int64_t value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, double value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, int value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, double value) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::join(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_join((SqQuery*)SQQM_DATAPTR, table, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

// leftJoin(table, condition...)
inline SQQM_RETURN  &SQQM_NAME::leftJoin(std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, int value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, int64_t value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, double value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::leftJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_left_join((SqQuery*)SQQM_DATAPTR, table, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

// rightJoin(table, condition...)
inline SQQM_RETURN  &SQQM_NAME::rightJoin(std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table);    // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table);    // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, int value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, int64_t value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, double value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::rightJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_right_join((SqQuery*)SQQM_DATAPTR, table, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

// fullJoin(table, condition...)
inline SQQM_RETURN  &SQQM_NAME::fullJoin(std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table);     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                  // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, int value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, int64_t value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, double value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQQM_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                               // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::fullJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_full_join((SqQuery*)SQQM_DATAPTR, table, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

// crossJoin(table)
inline SQQM_RETURN  &SQQM_NAME::crossJoin(const char *table) {
	sq_query_cross_join((SqQuery*)SQQM_DATAPTR, table);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::crossJoin(std::function<void()> func) {
//	sq_query_cross_join_sub((SqQuery*)SQQM_DATAPTR);      // start of subquery/brackets
	sq_query_cross_join((SqQuery*)SQQM_DATAPTR, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::crossJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_cross_join_sub((SqQuery*)SQQM_DATAPTR);      // start of subquery/brackets
	sq_query_cross_join((SqQuery*)SQQM_DATAPTR, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}

// on(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::on(std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, NULL);      // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, NULL);      // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, int value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, int64_t value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, double value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, int value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, int64_t value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, double value) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::on(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_on((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::on(const char *raw) {
	sq_query_on_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::onRaw(const char *raw) {
	sq_query_on_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::onRaw(const char *format, const Args... args) {
	sq_query_on_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// orOn(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::orOn(std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, int value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, int64_t value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, double value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, int value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, int64_t value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, double value) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orOn(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_on((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::orOn(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orOnRaw(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orOnRaw(const char *format, const Args... args) {
	sq_query_or_on_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// where(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::where(std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, int value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, int64_t value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, double value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                   // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, int value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, int64_t value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, double value) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::where(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_where((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::where(const char *raw) {
	sq_query_where_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereRaw(const char *raw) {
	sq_query_where_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereRaw(const char *format, const Args... args) {
	sq_query_where_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhere(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::orWhere(std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, NULL);      // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, NULL);      // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                      // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                      // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, int value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, int64_t value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, double value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                      // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                      // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, int value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, int64_t value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, double value) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_where((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::orWhere(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereRaw(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereRaw(const char *format, const Args... args) {
	sq_query_or_where_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// whereNot(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::whereNot(std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, int value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, int64_t value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, double value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, int value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, int64_t value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, double value) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_where_not((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::whereNot(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotRaw(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotRaw(const char *format, const Args... args) {
	sq_query_where_not_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhereNot(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, int value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, int64_t value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, double value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                          // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, int value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, int64_t value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, double value) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_where_not((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::orWhereNot(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotRaw(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotRaw(const char *format, const Args... args) {
	sq_query_or_where_not_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// whereExists
inline SQQM_RETURN  &SQQM_NAME::whereExists(std::function<void()> func) {
	sq_query_where_exists((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereExists(std::function<void(SqQuery &query)> func) {
	sq_query_where_exists((SqQuery*)SQQM_DATAPTR);        // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotExists(std::function<void()> func) {
	sq_query_where_not_exists((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotExists(std::function<void(SqQuery &query)> func) {
	sq_query_where_not_exists((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}

// whereBetween
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, int value1, int value2) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%d", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, double value1, double value2) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%f", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_between((SqQuery*)SQQM_DATAPTR, columnName, format, args...);
	return *(SQQM_RETURN*)this;
}

// whereNotBetween
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%d", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%f", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhereBetween
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%d", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, "%f", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_between((SqQuery*)SQQM_DATAPTR, columnName, format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhereNotBetween
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%d", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "%f", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_not_between((SqQuery*)SQQM_DATAPTR, columnName, format, args...);
	return *(SQQM_RETURN*)this;
}

// whereIn
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQQM_RETURN*)this;
}

// whereNotIn
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::whereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhereIn
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQQM_RETURN*)this;
}

// orWhereNotIn
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQQM_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQQM_RETURN*)this;
}

// whereNull
inline SQQM_RETURN  &SQQM_NAME::whereNull(const char *columnName) {
	sq_query_where_null((SqQuery*)SQQM_DATAPTR, columnName);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::whereNotNull(const char *columnName) {
	sq_query_where_not_null((SqQuery*)SQQM_DATAPTR, columnName);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNull(const char *columnName) {
	sq_query_or_where_null((SqQuery*)SQQM_DATAPTR, columnName);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orWhereNotNull(const char *columnName) {
	sq_query_or_where_not_null((SqQuery*)SQQM_DATAPTR, columnName);
	return *(SQQM_RETURN*)this;
}

// groupBy(column...)
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::groupBy(const char *columnName, const Args... args) {
	sq_query_group_by_list((SqQuery*)SQQM_DATAPTR, columnName, args..., NULL);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::groupBy(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::groupByRaw(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}

// having(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::having(std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR);    // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, int value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, int64_t value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, double value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, int value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, int64_t value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, double value) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::having(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_having((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::having(const char *raw) {
	sq_query_having_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::havingRaw(const char *raw) {
	sq_query_having_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::havingRaw(const char *format, const Args... args) {
	sq_query_having_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

// orHaving(condition, ...)
inline SQQM_RETURN  &SQQM_NAME::orHaving(std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR);       // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);             // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, int value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, int64_t value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, double value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", "%f", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, "=", "%s", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQQM_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_end_sub((SqQuery*)SQQM_DATAPTR);                       // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, int value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, "%d", value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, int64_t value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, "%" PRId64, value);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, double value) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, "%f", value);
	return *(SQQM_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op_or_format, value);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_having((SqQuery*)SQQM_DATAPTR, column, op, format, args...);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::orHaving(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orHavingRaw(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orHavingRaw(const char *format, const Args... args) {
	sq_query_or_having_raw((SqQuery*)SQQM_DATAPTR, format, args...);
	return *(SQQM_RETURN*)this;
}

template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::select(const char *columnName, const Args... args) {
	sq_query_select_list((SqQuery*)SQQM_DATAPTR, columnName, args..., NULL);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::select(const char *raw) {
	sq_query_select_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::selectRaw(const char *raw) {
	sq_query_select_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::distinct() {
	sq_query_distinct((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}

template <typename... Args>
inline SQQM_RETURN  &SQQM_NAME::orderBy(const char *columnName, const Args... args) {
	sq_query_order_by_list((SqQuery*)SQQM_DATAPTR, columnName, args..., NULL);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orderBy(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orderByRaw(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQQM_DATAPTR, raw);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::orderByDesc(const char *columnName) {
	sq_query_order_by_list((SqQuery*)SQQM_DATAPTR, columnName, NULL);
	sq_query_desc((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::asc() {
	sq_query_asc((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::desc() {
	sq_query_desc((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::union_(std::function<void()> func) {
	sq_query_union((SqQuery*)SQQM_DATAPTR);         // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::union_(std::function<void(SqQuery &query)> func) {
	sq_query_union((SqQuery*)SQQM_DATAPTR);         // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::unionAll(std::function<void()> func) {
	sq_query_union_all((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::unionAll(std::function<void(SqQuery &query)> func) {
	sq_query_union_all((SqQuery*)SQQM_DATAPTR);     // start of subquery/brackets
	func(*(SqQuery*)SQQM_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQQM_DATAPTR);    // end of subquery/brackets
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::limit(int64_t count) {
	sq_query_limit((SqQuery*)SQQM_DATAPTR, count);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::offset(int64_t index) {
	sq_query_offset((SqQuery*)SQQM_DATAPTR, index);
	return *(SQQM_RETURN*)this;
}

inline SQQM_RETURN  &SQQM_NAME::delete_() {
	sq_query_delete((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::deleteFrom() {
	sq_query_delete((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}
inline SQQM_RETURN  &SQQM_NAME::truncate() {
	sq_query_truncate((SqQuery*)SQQM_DATAPTR);
	return *(SQQM_RETURN*)this;
}

inline int   SQQM_NAME::getCommand() {
	return sq_query_get_command((SqQuery*)SQQM_DATAPTR);
}

inline char *SQQM_NAME::toSql() {
	return sq_query_to_sql((SqQuery*)SQQM_DATAPTR);
}
inline const char *SQQM_NAME::c() {
	return sq_query_c((SqQuery*)SQQM_DATAPTR);
}

inline const char *SQQM_NAME::last() {
	return sq_query_last((SqQuery*)SQQM_DATAPTR);
}

};  // namespace Sq

#endif  // __cplusplus && SQ_QUERY_METHOD_H_DEFINE == 1

#undef SQQM_NAME
#undef SQQM_RETURN
#undef SQQM_DATAPTR

#undef SQ_QUERY_METHOD_H_FOR_PROXY
#undef SQ_QUERY_METHOD_H_DECLARE
#undef SQ_QUERY_METHOD_H_DEFINE
