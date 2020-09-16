/*
 *   Copyright (C) 2020 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
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

#include <Sqxc.h>
#include <SqJoint.h>
#include <SqTable.h>
#include <SqSchema.h>
#include <SqStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqQuery           SqQuery;
typedef struct SqQueryNode       SqQueryNode;
typedef struct SqQueryNested     SqQueryNested;

extern const uintptr_t SQ_QUERYLOGI_OR;
extern const uintptr_t SQ_QUERYLOGI_AND;

extern const uintptr_t SQ_QUERYSORT_ASC;
extern const uintptr_t SQ_QUERYSORT_DESC;

/* ----------------------------------------------------------------------------
	SqQueryNode - store elements of SQL Statements

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
 */

struct SqQueryNode
{
	uintptr_t      type;       // SqQueryNodeType
	SqQueryNode*   next;
	SqQueryNode*   children;   // arguments, nested, or inserted string
	char*          value;
};

/* ----------------------------------------------------------------------------
	SqQuery C functions


	** below functions support printf format string:
		sq_query_join(), sq_query_on(),     sq_query_or_on(),
		               sq_query_where(),  sq_query_or_where(),
		               sq_query_having(), sq_query_or_having(),

	// e.g. "WHERE id < 100"
	sq_query_where(query, "id < %d", 100);


	** below function must use with Subquery
		sq_query_where_exists()

	// e.g. "WHERE EXISTS ( SELECT * FROM table WHERE id > 20 )"
	sq_query_where_exists(query);               // start of Subquery
	sq_query_from("table");
	sq_query_where("id > 20", NULL);
	sq_query_pop_nested(query);                 // end of Subquery


	** below function support Subquery/Nested: (second argument must be NULL)
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


	** below function support SQL raw statements: (third argument must be NULL)
		sq_query_on(),     sq_query_or_on(),
		sq_query_where(),  sq_query_or_where(),
		sq_query_having(), sq_query_or_having(),

	// e.g. "WHERE id = 1"
	sq_query_where(query, "id = 1", NULL);


	** below function support SQL raw statements: (fourth argument must be NULL)
		sq_query_join()

	// e.g. "JOIN city ON city.id > 10"
	sq_query_join(query, "city", "city.id > 10", NULL);

 */

SqQuery* sq_query_init(SqQuery* query, SqTable* table);
SqQuery* sq_query_final(SqQuery* query);

// SqQuery* sq_query_new(SqTable* table);
#define sq_query_new(table)     sq_query_init((SqQuery*)malloc(sizeof(SqQuery)), table)
// void    sq_query_free(SqQuery* query);
#define sq_query_free(query)    free(sq_query_final(query))

/*
	sq_query_push_nested() was called by sq_query_where_exists(), sq_query_from(),
	                       sq_query_join(), sq_query_on(), sq_query_where() ...etc
	It usually doesn't call by user.
 */

// push/pop SqQueryNested
SqQueryNested* sq_query_push_nested(SqQuery* query, SqQueryNode* parent);
void           sq_query_pop_nested(SqQuery* query);

// SQL: FROM
bool    sq_query_from(SqQuery* query, const char* table);

#define sq_query_table    sq_query_from

// SQL: AS
// call it after sq_query_from(), sq_query_join(), sq_query_select()
void    sq_query_as(SqQuery* query, const char* name);

// SQL: JOIN ON
void    sq_query_join(SqQuery* query, const char* table, ...);
void    sq_query_on_logical(SqQuery* query, uintptr_t sqn_type, ...);

#define sq_query_on(query, ...)        \
		sq_query_on_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_on(query, ...)     \
		sq_query_on_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: WHERE
void    sq_query_where_logical(SqQuery* query, uintptr_t sqn_type, ...);
bool    sq_query_where_exists(SqQuery* query);

#define sq_query_where(query, ...)        \
		sq_query_where_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_where(query, ...)     \
		sq_query_where_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: GROUP BY
void    sq_query_group_by(SqQuery* query, ...);

// SQL: HAVING
void    sq_query_having_logical(SqQuery* query, uintptr_t sqn_type, ...);

#define sq_query_having(query, ...)        \
		sq_query_having_logical(query, SQ_QUERYLOGI_AND, __VA_ARGS__)
#define sq_query_or_having(query, ...)     \
		sq_query_having_logical(query, SQ_QUERYLOGI_OR,  __VA_ARGS__)

// SQL: SELECT
bool    sq_query_select(SqQuery* query, ...);
bool    sq_query_distinct(SqQuery* query);

// SQL: ORDER BY
void    sq_query_order_by(SqQuery* query, ...);
void    sq_query_order_sorted(SqQuery* query, uintptr_t sqn_type);

#define sq_query_order_by_asc(query)     \
		sq_query_order_sorted(query, SQ_QUERYSORT_ASC)
#define sq_query_order_by_desc(query)    \
		sq_query_order_sorted(query, SQ_QUERYSORT_DESC)
#define sq_query_order_by_default(query) \
		sq_query_order_sorted(query, 0)

#define sq_query_asc     sq_query_order_by_asc
#define sq_query_desc    sq_query_order_by_desc

// SQL: DELETE FROM
void    sq_query_delete(SqQuery* query);

// SQL: TRUNCATE TABLE
void    sq_query_truncate(SqQuery* query);

// SQL statements
char*   sq_query_to_sql(SqQuery* query);

// va_list
void    sq_query_join_va(SqQuery* query, const char* table,
                         const char* condition, va_list arg_list);
void    sq_query_on_logical_va(SqQuery* query, uintptr_t sqn_type,
                               const char* condition, va_list arg_list);
void    sq_query_where_logical_va(SqQuery* query, uintptr_t sqn_type,
                                  const char* condition, va_list arg_list);
void    sq_query_group_by_va(SqQuery* query,
                             const char* column, va_list arg_list);
void    sq_query_having_logical_va(SqQuery* query, uintptr_t sqn_type,
                                   const char* condition, va_list arg_list);
bool    sq_query_select_va(SqQuery* query,
                           const char* column, va_list arg_list);
void    sq_query_order_by_va(SqQuery* query,
                             const char* column, va_list arg_list);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// SqQuery

struct SqQuery
{
	SqStorage*     database;
	SqSchema*      schema;
	SqTable*       table_;

	SqQueryNode    root;
	SqQueryNode*   node;
	SqQueryNode*   freed;      // freed SqQueryNode.
	void*          node_chunk;
	int            node_count;

	SqQueryNested* nested_cur;
	int            nested_count;

#ifdef __cplusplus
	// C++11 standard-layout

	SqQuery()
		{ sq_query_init(this, NULL); }
	~SqQuery()
		{ sq_query_final(this); }

	SqQuery* operator->()
		{ return this; }

	// ----------------------------------------------------

	SqQuery& from(const char* table)
		{ sq_query_from(this, table);  return *this; }
	SqQuery& from(std::function<void()> func) {
		sq_query_from(this, NULL);    // start of Subquery/Nested
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& from(std::function<void(SqQuery& query)> func) {
		sq_query_from(this, NULL);    // start of Subquery/Nested
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& table(const char* table)
		{ sq_query_from(this, table);  return *this; }
	SqQuery& table(std::function<void()> func) {
		sq_query_from(this, NULL);    // start of Subquery/Nested
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& table(std::function<void(SqQuery& query)> func) {
		sq_query_from(this, NULL);    // start of Subquery/Nested
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& as(const char* name)
		{ sq_query_as(this, name);  return *this; }

	// ----------------------------------------------------
	SqQuery& join(const char* table, const char* condition = NULL, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_join_va(this, table, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& join(std::function<void()> func) {
		sq_query_join(this, NULL);    // start of Subquery/Nested
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& join(std::function<void(SqQuery& query)> func) {
		sq_query_join(this, NULL);    // start of Subquery/Nested
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& on(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_on_logical_va(this, SQ_QUERYLOGI_AND, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& on(std::function<void()> func) {
		sq_query_on_logical(this, SQ_QUERYLOGI_AND, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& on(std::function<void(SqQuery& query)> func) {
		sq_query_on_logical(this, SQ_QUERYLOGI_AND, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& onRaw(const char* raw) {
		sq_query_on_logical(this, SQ_QUERYLOGI_AND, raw, NULL);
		return *this;
	}

	SqQuery& orOn(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_on_logical_va(this, SQ_QUERYLOGI_OR, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& orOn(std::function<void()> func) {
		sq_query_on_logical(this, SQ_QUERYLOGI_OR, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& orOn(std::function<void(SqQuery& query)> func) {
		sq_query_on_logical(this, SQ_QUERYLOGI_OR, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& orOnRaw(const char* raw) {
		sq_query_on_logical(this, SQ_QUERYLOGI_OR, raw, NULL);
		return *this;
	}

	// ----------------------------------------------------
	SqQuery& where(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_where_logical_va(this, SQ_QUERYLOGI_AND, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& where(std::function<void()> func) {
		sq_query_where_logical(this, SQ_QUERYLOGI_AND, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& where(std::function<void(SqQuery& query)> func) {
		sq_query_where_logical(this, SQ_QUERYLOGI_AND, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& whereRaw(const char* raw) {
		sq_query_where_logical(this, SQ_QUERYLOGI_AND, raw, NULL);
		return *this;
	}

	SqQuery& orWhere(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_where_logical_va(this, SQ_QUERYLOGI_OR, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& orWhere(std::function<void()> func) {
		sq_query_where_logical(this, SQ_QUERYLOGI_OR, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& orWhere(std::function<void(SqQuery& query)> func) {
		sq_query_where_logical(this, SQ_QUERYLOGI_OR, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& orWhereRaw(const char* raw) {
		sq_query_where_logical(this, SQ_QUERYLOGI_OR, raw, NULL);
		return *this;
	}

	SqQuery& whereExists(std::function<void()> func) {
		sq_query_where_exists(this);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& whereExists(std::function<void(SqQuery& query)> func) {
		sq_query_where_exists(this);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	// ----------------------------------------------------
	SqQuery& groupBy(const char* column, ...) {
		va_list  arg_list;
		va_start(arg_list, column);
		sq_query_group_by_va(this, column, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& groupByRaw(const char* raw) {
		sq_query_group_by_va(this, raw, NULL);
		return *this;
	}

	// ----------------------------------------------------
	SqQuery& having(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_having_logical_va(this, SQ_QUERYLOGI_AND, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& having(std::function<void()> func) {
		sq_query_having_logical(this, SQ_QUERYLOGI_AND, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& having(std::function<void(SqQuery& query)> func) {
		sq_query_having_logical(this, SQ_QUERYLOGI_AND, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& havingRaw(const char* raw) {
		sq_query_having_logical(this, SQ_QUERYLOGI_AND, raw, NULL);
		return *this;
	}

	SqQuery& orHaving(const char* condition, ...) {
		va_list  arg_list;
		va_start(arg_list, condition);
		sq_query_having_logical_va(this, SQ_QUERYLOGI_OR, condition, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& orHaving(std::function<void()> func) {
		sq_query_having_logical(this, SQ_QUERYLOGI_OR, NULL);
		func();
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}
	SqQuery& orHaving(std::function<void(SqQuery& query)> func) {
		sq_query_having_logical(this, SQ_QUERYLOGI_OR, NULL);
		func(*this);
		sq_query_pop_nested(this);    // end of Subquery/Nested
		return *this;
	}

	SqQuery& orHavingRaw(const char* raw) {
		sq_query_having_logical(this, SQ_QUERYLOGI_OR, raw, NULL);
		return *this;
	}

	// ----------------------------------------------------
	SqQuery& select(const char* column, ...) {
		va_list  arg_list;
		va_start(arg_list, column);
		sq_query_select_va(this, column, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& selectRaw(const char* raw) {
		sq_query_select(this, raw, NULL);
		return *this;
	}
	SqQuery& distinct()
		{ sq_query_distinct(this);  return *this; }

	// ----------------------------------------------------
	SqQuery& orderBy(const char* column, ...) {
		va_list  arg_list;
		va_start(arg_list, column);
		sq_query_order_by_va(this, column, arg_list);
		va_end(arg_list);
		return *this;
	}
	SqQuery& orderByRaw(const char* raw) {
		sq_query_order_by(this, raw, NULL);
		return *this;
	}
	SqQuery& asc()
		{ sq_query_order_by_asc(this);  return *this; }
	SqQuery& desc()
		{ sq_query_order_by_desc(this);  return *this; }

	// ----------------------------------------------------
	SqQuery& delete_()
		{ sq_query_delete(this);  return *this; }
	SqQuery& deleteFrom()
		{ sq_query_delete(this);  return *this; }
	SqQuery& truncate()
		{ sq_query_truncate(this);  return *this; }

	// ----------------------------------------------------
	inline char* toSql()
		{ return sq_query_to_sql(this); }
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{
// These are for directly use only. You can NOT derived it.
typedef struct SqQuery          Query;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQ_QUERY_H
