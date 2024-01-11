/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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

#define SQPT_DECLARE
#define SQPT_DEFINE
#define SQPT_USE_STRUCT
#define SQPT_NAME       QueryProxy
#define SQPT_RETURN     QueryProxy
#define SQPT_DATAPTR    query
 */


// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#if defined(__cplusplus) && defined(SQPT_DECLARE)

namespace Sq {

struct Query;

#ifdef SQPT_USE_STRUCT
// struct QueryMethod
struct SQPT_NAME {

#else
// class  QueryProxy
class  SQPT_NAME {

protected:
	Sq::Query *SQPT_DATAPTR;

public:
	// return data pointer of QueryProxy
	Sq::Query    *query();
#endif  // SQPT_USE_STRUCT

// struct QueryMethod SQPT_RETURN is 'Query'
// class  QueryProxy  SQPT_RETURN is 'QueryProxy'
	SQPT_RETURN  *operator->();

	SQPT_RETURN  &clear();

	template <typename... Args>
	SQPT_RETURN  &raw(const char *format, const Args... args);
	SQPT_RETURN  &raw(const char *raw_sql);

	template <typename... Args>
	SQPT_RETURN  &printf(const char *format, const Args... args);

	SQPT_RETURN  &from(const char *table);
	SQPT_RETURN  &from(std::function<void()> func);
	SQPT_RETURN  &from(std::function<void(SqQuery &query)> func);

	SQPT_RETURN  &table(const char *table);
	SQPT_RETURN  &table(std::function<void()> func);
	SQPT_RETURN  &table(std::function<void(SqQuery &query)> func);

	SQPT_RETURN  &as(const char *name);

	// join(table, condition...)
	SQPT_RETURN  &join(std::function<void()> func);
	SQPT_RETURN  &join(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &join(const char *table, std::function<void()> func);
	SQPT_RETURN  &join(const char *table, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &join(const char *table, const char *column, std::function<void()> func);
	SQPT_RETURN  &join(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &join(const char *table, const char *column, int value);
	SQPT_RETURN  &join(const char *table, const char *column, int64_t value);
	SQPT_RETURN  &join(const char *table, const char *column, double value);
	SQPT_RETURN  &join(const char *table, const char *column, const char *value);
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, int value);
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, int64_t value);
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &join(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &join(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// leftJoin(table, condition...)
	SQPT_RETURN  &leftJoin(std::function<void()> func);
	SQPT_RETURN  &leftJoin(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &leftJoin(const char *table, std::function<void()> func);
	SQPT_RETURN  &leftJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, std::function<void()> func);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, int value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, int64_t value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, double value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, int value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, int64_t value);
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &leftJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// rightJoin(table, condition...)
	SQPT_RETURN  &rightJoin(std::function<void()> func);
	SQPT_RETURN  &rightJoin(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &rightJoin(const char *table, std::function<void()> func);
	SQPT_RETURN  &rightJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, std::function<void()> func);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, int value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, int64_t value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, double value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, int value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, int64_t value);
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &rightJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// fullJoin(table, condition...)
	SQPT_RETURN  &fullJoin(std::function<void()> func);
	SQPT_RETURN  &fullJoin(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &fullJoin(const char *table, std::function<void()> func);
	SQPT_RETURN  &fullJoin(const char *table, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, std::function<void()> func);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, int value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, int64_t value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, double value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, int value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, int64_t value);
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &fullJoin(const char *table, const char *column, const char *op, const char *format, const Args... args);

	// crossJoin(table)
	SQPT_RETURN  &crossJoin(const char *table);
	SQPT_RETURN  &crossJoin(std::function<void()> func);
	SQPT_RETURN  &crossJoin(std::function<void(SqQuery &query)> func);

	// on(condition, ...)
	SQPT_RETURN  &on(std::function<void()> func);
	SQPT_RETURN  &on(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &on(const char *column, std::function<void()> func);
	SQPT_RETURN  &on(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &on(const char *column, int value);
	SQPT_RETURN  &on(const char *column, int64_t value);
	SQPT_RETURN  &on(const char *column, double value);
	SQPT_RETURN  &on(const char *column, const char *value);
	SQPT_RETURN  &on(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &on(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &on(const char *column, const char *op, int value);
	SQPT_RETURN  &on(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &on(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &on(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &on(const char *column, const char *op, const char *format, const Args ...args);

	SQPT_RETURN  &on(const char *raw);
	SQPT_RETURN  &onRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &onRaw(const char *format, const Args ...args);

	// orOn(condition, ...)
	SQPT_RETURN  &orOn(std::function<void()> func);
	SQPT_RETURN  &orOn(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orOn(const char *column, std::function<void()> func);
	SQPT_RETURN  &orOn(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orOn(const char *column, int value);
	SQPT_RETURN  &orOn(const char *column, int64_t value);
	SQPT_RETURN  &orOn(const char *column, double value);
	SQPT_RETURN  &orOn(const char *column, const char *value);
	SQPT_RETURN  &orOn(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &orOn(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orOn(const char *column, const char *op, int value);
	SQPT_RETURN  &orOn(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &orOn(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &orOn(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &orOn(const char *column, const char *op, const char *format, const Args ...args);

	SQPT_RETURN  &orOn(const char *raw);
	SQPT_RETURN  &orOnRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &orOnRaw(const char *format, const Args... args);

	// where(condition, ...)
	SQPT_RETURN  &where(std::function<void()> func);
	SQPT_RETURN  &where(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &where(const char *column, std::function<void()> func);
	SQPT_RETURN  &where(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &where(const char *column, int value);
	SQPT_RETURN  &where(const char *column, int64_t value);
	SQPT_RETURN  &where(const char *column, double value);
	SQPT_RETURN  &where(const char *column, const char *value);
	SQPT_RETURN  &where(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &where(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &where(const char *column, const char *op, int value);
	SQPT_RETURN  &where(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &where(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &where(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &where(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &where(const char *raw);
	SQPT_RETURN  &whereRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &whereRaw(const char *format, const Args... args);

	// orWhere(condition, ...)
	SQPT_RETURN  &orWhere(std::function<void()> func);
	SQPT_RETURN  &orWhere(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhere(const char *column, std::function<void()> func);
	SQPT_RETURN  &orWhere(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhere(const char *column, int value);
	SQPT_RETURN  &orWhere(const char *column, int64_t value);
	SQPT_RETURN  &orWhere(const char *column, double value);
	SQPT_RETURN  &orWhere(const char *column, const char *value);
	SQPT_RETURN  &orWhere(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &orWhere(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhere(const char *column, const char *op, int value);
	SQPT_RETURN  &orWhere(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &orWhere(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &orWhere(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &orWhere(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &orWhere(const char *raw);
	SQPT_RETURN  &orWhereRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &orWhereRaw(const char *format, const Args... args);

	// whereNot(condition, ...)
	SQPT_RETURN  &whereNot(std::function<void()> func);
	SQPT_RETURN  &whereNot(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &whereNot(const char *column, std::function<void()> func);
	SQPT_RETURN  &whereNot(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &whereNot(const char *column, int value);
	SQPT_RETURN  &whereNot(const char *column, int64_t value);
	SQPT_RETURN  &whereNot(const char *column, double value);
	SQPT_RETURN  &whereNot(const char *column, const char *value);
	SQPT_RETURN  &whereNot(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &whereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &whereNot(const char *column, const char *op, int value);
	SQPT_RETURN  &whereNot(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &whereNot(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &whereNot(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &whereNot(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &whereNot(const char *raw);
	SQPT_RETURN  &whereNotRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &whereNotRaw(const char *format, const Args... args);

	// orWhereNot(condition, ...)
	SQPT_RETURN  &orWhereNot(std::function<void()> func);
	SQPT_RETURN  &orWhereNot(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhereNot(const char *column, std::function<void()> func);
	SQPT_RETURN  &orWhereNot(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhereNot(const char *column, int value);
	SQPT_RETURN  &orWhereNot(const char *column, int64_t value);
	SQPT_RETURN  &orWhereNot(const char *column, double value);
	SQPT_RETURN  &orWhereNot(const char *column, const char *value);
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, int value);
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &orWhereNot(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &orWhereNot(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &orWhereNot(const char *raw);
	SQPT_RETURN  &orWhereNotRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotRaw(const char *format, const Args... args);

	// whereExists
	SQPT_RETURN  &whereExists(std::function<void()> func);
	SQPT_RETURN  &whereExists(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &whereNotExists(std::function<void()> func);
	SQPT_RETURN  &whereNotExists(std::function<void(SqQuery &query)> func);

	// whereBetween
	SQPT_RETURN  &whereBetween(const char *columnName, int value1, int value2);
	SQPT_RETURN  &whereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQPT_RETURN  &whereBetween(const char *columnName, double value1, double value2);
	SQPT_RETURN  &whereBetween(const char *columnName, const char value1, const char value2);
	SQPT_RETURN  &whereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQPT_RETURN  &whereBetween(const char *columnName, const char *format, const Args... args);

	// whereNotBetween
	SQPT_RETURN  &whereNotBetween(const char *columnName, int value1, int value2);
	SQPT_RETURN  &whereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQPT_RETURN  &whereNotBetween(const char *columnName, double value1, double value2);
	SQPT_RETURN  &whereNotBetween(const char *columnName, const char value1, const char value2);
	SQPT_RETURN  &whereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQPT_RETURN  &whereNotBetween(const char *columnName, const char *format, const Args... args);

	// orWhereBetween
	SQPT_RETURN  &orWhereBetween(const char *columnName, int value1, int value2);
	SQPT_RETURN  &orWhereBetween(const char *columnName, int64_t value1, int64_t value2);
	SQPT_RETURN  &orWhereBetween(const char *columnName, double value1, double value2);
	SQPT_RETURN  &orWhereBetween(const char *columnName, const char value1, const char value2);
	SQPT_RETURN  &orWhereBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQPT_RETURN  &orWhereBetween(const char *columnName, const char *format, const Args... args);

	// orWhereNotBetween
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, int value1, int value2);
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2);
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, double value1, double value2);
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, const char value1, const char value2);
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, const char *value1, const char *value2);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotBetween(const char *columnName, const char *format, const Args... args);

	// whereIn
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereIn(const char *columnName, int n_args, const char *format, const Args... args);

	// whereNotIn
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &whereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	// orWhereIn
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereIn(const char *columnName, int n_args, const char *format, const Args... args);

	// orWhereNotIn
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, int firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, int64_t firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, double firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, const char firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, const char *firstValue, const Args... args);
	template <typename... Args>
	SQPT_RETURN  &orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args);

	// whereNull
	SQPT_RETURN  &whereNull(const char *columnName);
	SQPT_RETURN  &whereNotNull(const char *columnName);
	SQPT_RETURN  &orWhereNull(const char *columnName);
	SQPT_RETURN  &orWhereNotNull(const char *columnName);

	// groupBy(column...)
	template <typename... Args>
	SQPT_RETURN  &groupBy(const char *columnName, const Args... args);
	SQPT_RETURN  &groupBy(const char *raw);
	SQPT_RETURN  &groupByRaw(const char *raw);

	// having(condition, ...)
	SQPT_RETURN  &having(std::function<void()> func);
	SQPT_RETURN  &having(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &having(const char *column, std::function<void()> func);
	SQPT_RETURN  &having(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &having(const char *column, int value);
	SQPT_RETURN  &having(const char *column, int64_t value);
	SQPT_RETURN  &having(const char *column, double value);
	SQPT_RETURN  &having(const char *column, const char *value);
	SQPT_RETURN  &having(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &having(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &having(const char *column, const char *op, int value);
	SQPT_RETURN  &having(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &having(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &having(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &having(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &having(const char *raw);
	SQPT_RETURN  &havingRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &havingRaw(const char *format, const Args... args);

	// orHaving(condition, ...)
	SQPT_RETURN  &orHaving(std::function<void()> func);
	SQPT_RETURN  &orHaving(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orHaving(const char *column, std::function<void()> func);
	SQPT_RETURN  &orHaving(const char *column, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orHaving(const char *column, int value);
	SQPT_RETURN  &orHaving(const char *column, int64_t value);
	SQPT_RETURN  &orHaving(const char *column, double value);
	SQPT_RETURN  &orHaving(const char *column, const char *value);
	SQPT_RETURN  &orHaving(const char *column, const char *op, std::function<void()> func);
	SQPT_RETURN  &orHaving(const char *column, const char *op, std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &orHaving(const char *column, const char *op, int value);
	SQPT_RETURN  &orHaving(const char *column, const char *op, int64_t value);
	SQPT_RETURN  &orHaving(const char *column, const char *op, double value);
	//// This method must match overloaded function. It can NOT use template specialization.
	SQPT_RETURN  &orHaving(const char *column, const char *op_or_format, const char *value);
	template <typename... Args>
	SQPT_RETURN  &orHaving(const char *column, const char *op, const char *format, const Args... args);

	SQPT_RETURN  &orHaving(const char *raw);
	SQPT_RETURN  &orHavingRaw(const char *raw);
	template <typename... Args>
	SQPT_RETURN  &orHavingRaw(const char *format, const Args... args);

	// select(column...)
	template <typename... Args>
	SQPT_RETURN  &select(const char *columnName, const Args... args);
	SQPT_RETURN  &select(const char *raw);
	SQPT_RETURN  &selectRaw(const char *raw);
	SQPT_RETURN  &distinct();

	// orderBy(column...)
	template <typename... Args>
	SQPT_RETURN  &orderBy(const char *columnName, const Args... args);
	SQPT_RETURN  &orderBy(const char *raw);
	SQPT_RETURN  &orderByRaw(const char *raw);
	SQPT_RETURN  &orderByDesc(const char *columnName);
	SQPT_RETURN  &asc();
	SQPT_RETURN  &desc();

	// union(lambda function)
	SQPT_RETURN  &union_(std::function<void()> func);
	SQPT_RETURN  &union_(std::function<void(SqQuery &query)> func);
	SQPT_RETURN  &unionAll(std::function<void()> func);
	SQPT_RETURN  &unionAll(std::function<void(SqQuery &query)> func);

	SQPT_RETURN  &limit(int64_t count);
	SQPT_RETURN  &offset(int64_t index);

	// call these function at last (before generating SQL statement).
	SQPT_RETURN  &delete_();
	SQPT_RETURN  &deleteFrom();
	SQPT_RETURN  &truncate();

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

#endif  // __cplusplus && SQPT_DECLARE

// ----------------------------------------------------------------------------
// C++ definitions

#if defined(__cplusplus) && defined(SQPT_DEFINE)

namespace Sq {

/*
	define functions.

	struct QueryMethod SQPT_DATAPTR is 'this'
	class  QueryProxy  SQPT_DATAPTR is 'data'
 */

#ifndef SQPT_USE_STRUCT
// return data pointer of QueryProxy
inline Sq::Query  *SQPT_NAME::query() {
	return (Sq::Query*)SQPT_DATAPTR;
}
#endif  // SQPT_USE_STRUCT

inline SQPT_RETURN  *SQPT_NAME::operator->() {
	return (SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::clear() {
	sq_query_clear((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}

template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::raw(const char *format, const Args... args) {
	sq_query_printf((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::raw(const char *raw_sql) {
	sq_query_raw((SqQuery*)SQPT_DATAPTR, raw_sql);
	return *(SQPT_RETURN*)this;
}

template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::printf(const char *format, const Args... args) {
	sq_query_printf((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::from(const char *table) {
	sq_query_from((SqQuery*)SQPT_DATAPTR, table);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::from(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::from(std::function<void(SqQuery &query)> func) {
	sq_query_from_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::table(const char *table) {
	sq_query_from((SqQuery*)SQPT_DATAPTR, table);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::table(std::function<void()> func) {
	sq_query_from_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::table(std::function<void(SqQuery &query)> func) {
	sq_query_from_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::as(const char *name) {
	sq_query_as((SqQuery*)SQPT_DATAPTR, name);
	return *(SQPT_RETURN*)this;
}

// join(table, condition...)
inline SQPT_RETURN  &SQPT_NAME::join(std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, NULL);          // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR);            // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, NULL);          // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, int value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, int64_t value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, double value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, int value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, double value) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::join(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_join((SqQuery*)SQPT_DATAPTR, table, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

// leftJoin(table, condition...)
inline SQPT_RETURN  &SQPT_NAME::leftJoin(std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, int value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, int64_t value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, double value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_left_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::leftJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_left_join((SqQuery*)SQPT_DATAPTR, table, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

// rightJoin(table, condition...)
inline SQPT_RETURN  &SQPT_NAME::rightJoin(std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table);    // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table);    // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, int value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, int64_t value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, double value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_right_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::rightJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_right_join((SqQuery*)SQPT_DATAPTR, table, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

// fullJoin(table, condition...)
inline SQPT_RETURN  &SQPT_NAME::fullJoin(std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table);     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                  // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table, column, "=");     // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, int value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, int64_t value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, double value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, std::function<void()> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_full_join_sub((SqQuery*)SQPT_DATAPTR, table, column, op);      // start of subquery/brackets
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                               // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, int value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, int64_t value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, double value) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::fullJoin(const char *table, const char *column, const char *op, const char *format, const Args... args) {
	sq_query_full_join((SqQuery*)SQPT_DATAPTR, table, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

// crossJoin(table)
inline SQPT_RETURN  &SQPT_NAME::crossJoin(const char *table) {
	sq_query_cross_join((SqQuery*)SQPT_DATAPTR, table);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::crossJoin(std::function<void()> func) {
//	sq_query_cross_join_sub((SqQuery*)SQPT_DATAPTR);      // start of subquery/brackets
	sq_query_cross_join((SqQuery*)SQPT_DATAPTR, NULL);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::crossJoin(std::function<void(SqQuery &query)> func) {
//	sq_query_cross_join_sub((SqQuery*)SQPT_DATAPTR);      // start of subquery/brackets
	sq_query_cross_join((SqQuery*)SQPT_DATAPTR, NULL);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}

// on(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::on(std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, NULL);      // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, NULL);      // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, int value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, int64_t value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, double value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, std::function<void()> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_on_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, int value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, int64_t value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, double value) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::on(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_on((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::on(const char *raw) {
	sq_query_on_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::onRaw(const char *raw) {
	sq_query_on_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::onRaw(const char *format, const Args... args) {
	sq_query_on_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// orOn(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::orOn(std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, int value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, int64_t value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, double value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_on_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, int value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, int64_t value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, double value) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orOn(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_on((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::orOn(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orOnRaw(const char *raw) {
	sq_query_or_on_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orOnRaw(const char *format, const Args... args) {
	sq_query_or_on_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// where(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::where(std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, int value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, int64_t value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, double value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, std::function<void()> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_where_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                   // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, int value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, int64_t value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, double value) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::where(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_where((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::where(const char *raw) {
	sq_query_where_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereRaw(const char *raw) {
	sq_query_where_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereRaw(const char *format, const Args... args) {
	sq_query_where_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhere(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::orWhere(std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, NULL);      // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, NULL);      // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                      // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                      // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, int value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, int64_t value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, double value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                      // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                      // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, int value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, int64_t value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, double value) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_where((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::orWhere(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereRaw(const char *raw) {
	sq_query_or_where_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereRaw(const char *format, const Args... args) {
	sq_query_or_where_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// whereNot(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::whereNot(std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, int value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, int64_t value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, double value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, std::function<void()> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_where_not_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, int value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, int64_t value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, double value) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_where_not((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::whereNot(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotRaw(const char *raw) {
	sq_query_where_not_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotRaw(const char *format, const Args... args) {
	sq_query_where_not_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhereNot(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, int value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, int64_t value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, double value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_where_not_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                          // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, int value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, int64_t value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, double value) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_where_not((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::orWhereNot(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotRaw(const char *raw) {
	sq_query_or_where_not_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotRaw(const char *format, const Args... args) {
	sq_query_or_where_not_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// whereExists
inline SQPT_RETURN  &SQPT_NAME::whereExists(std::function<void()> func) {
	sq_query_where_exists((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereExists(std::function<void(SqQuery &query)> func) {
	sq_query_where_exists((SqQuery*)SQPT_DATAPTR);        // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotExists(std::function<void()> func) {
	sq_query_where_not_exists((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotExists(std::function<void(SqQuery &query)> func) {
	sq_query_where_not_exists((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}

// whereBetween
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, int value1, int value2) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%d", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, double value1, double value2) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%f", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_between((SqQuery*)SQPT_DATAPTR, columnName, format, args...);
	return *(SQPT_RETURN*)this;
}

// whereNotBetween
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%d", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%f", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhereBetween
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%d", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, "%f", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_between((SqQuery*)SQPT_DATAPTR, columnName, format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhereNotBetween
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, int value1, int value2) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%d", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, int64_t value1, int64_t value2) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%" PRId64, value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, double value1, double value2) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "%f", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, const char value1, const char value2) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "'%c'", value1, value2);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, const char *value1, const char *value2) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, "'%s'", value1, value2);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotBetween(const char *columnName, const char *format, const Args... args) {
	sq_query_or_where_not_between((SqQuery*)SQPT_DATAPTR, columnName, format, args...);
	return *(SQPT_RETURN*)this;
}

// whereIn
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQPT_RETURN*)this;
}

// whereNotIn
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::whereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_AND_NOT,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhereIn
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQPT_RETURN*)this;
}

// orWhereNotIn
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, int firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%d", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, int64_t firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%" PRId64, firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, double firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "%f", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, const char firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "'%c'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, const char *firstValue, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          sizeof...(args)+1, "'%s'", firstValue, args...);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orWhereNotIn(const char *columnName, int n_args, const char *format, const Args... args) {
	sq_query_where_in_logical((SqQuery*)SQPT_DATAPTR, columnName, SQ_QUERYLOGI_OR_NOT,
	                          (n_args) ? n_args : sizeof...(args), format, args...);
	return *(SQPT_RETURN*)this;
}

// whereNull
inline SQPT_RETURN  &SQPT_NAME::whereNull(const char *columnName) {
	sq_query_where_null((SqQuery*)SQPT_DATAPTR, columnName);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::whereNotNull(const char *columnName) {
	sq_query_where_not_null((SqQuery*)SQPT_DATAPTR, columnName);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNull(const char *columnName) {
	sq_query_or_where_null((SqQuery*)SQPT_DATAPTR, columnName);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orWhereNotNull(const char *columnName) {
	sq_query_or_where_not_null((SqQuery*)SQPT_DATAPTR, columnName);
	return *(SQPT_RETURN*)this;
}

// groupBy(column...)
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::groupBy(const char *columnName, const Args... args) {
	sq_query_group_by_list((SqQuery*)SQPT_DATAPTR, columnName, args..., NULL);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::groupBy(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::groupByRaw(const char *raw) {
	sq_query_group_by_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}

// having(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::having(std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR);    // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, int value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, int64_t value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, double value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, std::function<void()> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_having_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, int value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, int64_t value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, double value) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::having(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_having((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::having(const char *raw) {
	sq_query_having_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::havingRaw(const char *raw) {
	sq_query_having_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::havingRaw(const char *format, const Args... args) {
	sq_query_having_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

// orHaving(condition, ...)
inline SQPT_RETURN  &SQPT_NAME::orHaving(std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR);       // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, NULL);     // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);             // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR, column, "=");    // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", NULL);  // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, int value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, int64_t value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, double value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", "%f", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, "=", "%s", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, std::function<void()> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func();
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, std::function<void(SqQuery &query)> func) {
//	sq_query_or_having_sub((SqQuery*)SQPT_DATAPTR, column, op);     // start of subquery/brackets
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, NULL);   // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_end_sub((SqQuery*)SQPT_DATAPTR);                       // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, int value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, "%d", value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, int64_t value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, "%" PRId64, value);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, double value) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, "%f", value);
	return *(SQPT_RETURN*)this;
}
//// This method must match overloaded function. It can NOT use template specialization.
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op_or_format, const char *value) {
	// 3 arguments special case: "column", "%s", "valueStr"   or   "column", ">", "valueStr"
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op_or_format, value);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *column, const char *op, const char *format, const Args... args) {
	sq_query_or_having((SqQuery*)SQPT_DATAPTR, column, op, format, args...);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::orHaving(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orHavingRaw(const char *raw) {
	sq_query_or_having_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orHavingRaw(const char *format, const Args... args) {
	sq_query_or_having_raw((SqQuery*)SQPT_DATAPTR, format, args...);
	return *(SQPT_RETURN*)this;
}

template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::select(const char *columnName, const Args... args) {
	sq_query_select_list((SqQuery*)SQPT_DATAPTR, columnName, args..., NULL);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::select(const char *raw) {
	sq_query_select_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::selectRaw(const char *raw) {
	sq_query_select_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::distinct() {
	sq_query_distinct((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}

template <typename... Args>
inline SQPT_RETURN  &SQPT_NAME::orderBy(const char *columnName, const Args... args) {
	sq_query_order_by_list((SqQuery*)SQPT_DATAPTR, columnName, args..., NULL);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orderBy(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orderByRaw(const char *raw) {
	sq_query_order_by_raw((SqQuery*)SQPT_DATAPTR, raw);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::orderByDesc(const char *columnName) {
	sq_query_order_by_list((SqQuery*)SQPT_DATAPTR, columnName, NULL);
	sq_query_desc((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::asc() {
	sq_query_asc((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::desc() {
	sq_query_desc((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::union_(std::function<void()> func) {
	sq_query_union((SqQuery*)SQPT_DATAPTR);         // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::union_(std::function<void(SqQuery &query)> func) {
	sq_query_union((SqQuery*)SQPT_DATAPTR);         // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::unionAll(std::function<void()> func) {
	sq_query_union_all((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	func();
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::unionAll(std::function<void(SqQuery &query)> func) {
	sq_query_union_all((SqQuery*)SQPT_DATAPTR);     // start of subquery/brackets
	func(*(SqQuery*)SQPT_DATAPTR);
	sq_query_pop_nested((SqQuery*)SQPT_DATAPTR);    // end of subquery/brackets
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::limit(int64_t count) {
	sq_query_limit((SqQuery*)SQPT_DATAPTR, count);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::offset(int64_t index) {
	sq_query_offset((SqQuery*)SQPT_DATAPTR, index);
	return *(SQPT_RETURN*)this;
}

inline SQPT_RETURN  &SQPT_NAME::delete_() {
	sq_query_delete((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::deleteFrom() {
	sq_query_delete((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}
inline SQPT_RETURN  &SQPT_NAME::truncate() {
	sq_query_truncate((SqQuery*)SQPT_DATAPTR);
	return *(SQPT_RETURN*)this;
}

inline int   SQPT_NAME::getCommand() {
	return sq_query_get_command((SqQuery*)SQPT_DATAPTR);
}

inline char *SQPT_NAME::toSql() {
	return sq_query_to_sql((SqQuery*)SQPT_DATAPTR);
}
inline const char *SQPT_NAME::c() {
	return sq_query_c((SqQuery*)SQPT_DATAPTR);
}

inline const char *SQPT_NAME::last() {
	return sq_query_last((SqQuery*)SQPT_DATAPTR);
}

};  // namespace Sq

#endif  // __cplusplus && SQPT_DEFINE
