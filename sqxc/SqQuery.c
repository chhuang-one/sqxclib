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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>      // vsnprintf

#include <SqQuery.h>

#ifdef _MSC_VER
#define strdup       _strdup
#define snprintf     _snprintf
#endif

#define SQ_QUERY_STR_SIZE_DEFAULT  256
#define SQ_QUERY_USE_OLD_CONDITION   1

static void sq_query_insert_column_list(SqQuery *query, SqQueryNode *parent, va_list arg_list);
static void sq_query_insert_table_node(SqQuery *query, SqQueryNode *parent, const char *table_name);
static void sq_query_free_all_node(SqQuery *query);

static SqQueryNode *sq_query_column_in(SqQuery *query, const char *column_name, int logi_type, char *value_str);
static SqQueryNode *sq_query_condition(SqQuery *query, SqQueryNode *parent, va_list arg_list);

static SqQueryNode *sq_query_node_new(SqQuery *query);
static void         sq_query_node_free(SqQueryNode *node, SqQuery *query);

#define sq_query_node_append(parent, node)     \
		sq_query_node_insert(parent, sq_query_node_last((parent)->children), node)
#define sq_query_node_prepend(parent, node)    \
		sq_query_node_insert(parent, NULL, node)

static SqQueryNode *sq_query_node_insert(SqQueryNode *parent, SqQueryNode *prev, SqQueryNode *node);
static SqQueryNode *sq_query_node_last(SqQueryNode *node);
static SqQueryNode *sq_query_node_find(SqQueryNode *parent, int sqn_cmd, SqQueryNode **insert_pos);

enum SqQueryNodeType {
	SQN_NONE,    // 0, empty string

	// --------------------------------
	// commands

	SQN_CREATE_TABLE,
	SQN_ALERT_TABLE,
	SQN_DROP_TABLE,
	SQN_TRUNCATE_TABLE,
	SQN_INSERT_INTO,
	SQN_UPDATE,
	SQN_DELETE,
	SQN_SELECT,           // INSERT INTO can use this

	SQN_COMMAND_END = SQN_SELECT,

	// --------------------------------
	// SELECT (these must keep order)

	SQN_FROM,
	SQN_JOIN,      // INNER JOIN
	SQN_LEFT_JOIN,
	SQN_RIGHT_JOIN,
	SQN_FULL_JOIN,
	SQN_CROSS_JOIN,
	SQN_SET,       // UPDATE
	SQN_WHERE,     // SELECT, UPDATE, DELETE
	SQN_GROUP_BY,
	SQN_HAVING,
	SQN_ORDER_BY,
	SQN_UNION,
	SQN_UNION_ALL,
	SQN_LIMIT,

	// --------------------------------

	SQN_DISTINCT,  // SELECT
	SQN_AS,        // SELECT...etc : alias name for table or column
	SQN_OR,        // WHERE, JOIN
	SQN_AND,       // WHERE, JOIN, BETWEEN
	SQN_NOT,       // WHERE
	SQN_EXISTS,    // WHERE
	SQN_ON,        // JOIN
//	SQN_IN,        // WHERE
//	SQN_BETWEEN,   // WHERE, JOIN
	SQN_LIKE,      // WHERE

	SQN_ASC,       // ORDER BY
	SQN_DESC,      // ORDER BY

	SQN_OFFSET,    // LIMIT

	SQN_VALUES,    // INSERT INTO

	// --------------------------------
	SQN_SYMBOL,                   // equal SQN_BRACKETS_L
	SQN_BRACKETS_L = SQN_SYMBOL,  // (
	SQN_BRACKETS_R,               // )
	SQN_COMMA,                    // ,
	SQN_ASTERISK,                 // *

	SQN_N_CODE,

	// --------------------------------
	SQN_VALUE      = (1<<16),
//	SQN_DEFAULT    = (1<<17),
};

struct SqQueryNested
{
	SqQueryNested  *outer;
	SqQueryNested  *inner;

	SqQueryNode    *parent;
	SqQueryNode    *command;    // SQL command (SELECT, UPDATE ...etc)
	SqQueryNode    *name;       // table, column, index, or database name
	SqQueryNode    *aliasable;  // sq_query_as()

	// current condition
	SqQueryNode    *joinon;     // JOIN table ON  AND  OR
};


static const char  *sqnword[SQN_N_CODE];

// ----------------------------------------------------------------------------
// SqQuery

SqQuery *sq_query_new(const char *table_name)
{
	SqQuery *query;

	query = malloc(sizeof(SqQuery));
	return sq_query_init(query, table_name);
}

void     sq_query_free(SqQuery *query)
{
	sq_query_final(query);
	free(query);
}

SqQuery *sq_query_init(SqQuery *query, const char *table_name)
{
	memset(query, 0, sizeof(SqQuery));

	// push top level
	sq_query_push_nested(query, &query->root);
	// set table_name
	if (table_name)
		sq_query_from(query, table_name);
	return query;
}

static void sq_query_clear_internal(SqQuery *query)
{
	// free SqQueryNested
	while(query->nested_cur)
		sq_query_pop_nested(query);
	// free SqQueryNode
	sq_query_free_all_node(query);
}

SqQuery *sq_query_final(SqQuery *query)
{
	sq_query_clear_internal(query);
	// free str
	free(query->str);
	// return pointer
	return query;
}

void  sq_query_clear(SqQuery *query)
{
	sq_query_clear_internal(query);

	// sq_query_free_all_node() doesn't reset these
	query->used  = NULL;
	query->freed = NULL;
	query->node_chunk = NULL;
	query->node_count = 0;

	// sq_query_init() also do this
	sq_query_push_nested(query, &query->root);
}

void sq_query_append(SqQuery *query, const char *raw, SqQueryNode *parent)
{
	SqQueryNode   *node;

	if (parent == NULL)
		parent = query->nested_cur->parent;
	node = sq_query_node_new(query);
	node->type  = SQN_VALUE;
	node->value = strdup(raw);
	sq_query_node_append(parent, node);
}

void sq_query_printf(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node;

	va_start(arg_list, query);
	node = sq_query_condition(query, nested->parent, arg_list);
	va_end(arg_list);
	if (node)
		sq_query_node_append(nested->parent, node);
}

bool sq_query_from(SqQuery *query, const char *table)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->name;

	if (node)
		return false;
	// insert table node after command node
	node = sq_query_node_insert(nested->parent, nested->command, sq_query_node_new(query));
	if (nested->command && nested->command->type < SQN_DELETE)    // not DELETE or SELECT
		node->type = SQN_NONE;
	else
		node->type = SQN_FROM;
	nested->name = node;

	// append table name
	sq_query_insert_table_node(query, node, table);
	return true;
}

void sq_query_as(SqQuery *query, const char *name)
{
	SqQueryNode *node = query->nested_cur->aliasable;
	SqQueryNode *sub_node;

	if (node == NULL)
		return;
	// insert AS node
	sub_node = sq_query_node_new(query);
	sub_node->type = SQN_AS;
	sub_node->next = node->next;
	node->next = sub_node;
	node = sub_node;
	// insert alias name in children of above AS node
	sub_node = sq_query_node_new(query);
	sub_node->type = SQN_VALUE;
	sub_node->value = strdup(name);
	node->children = sub_node;
	// clear aliasable
	query->nested_cur->aliasable = NULL;
}

bool sq_query_select(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *sub_node;
	SqQueryNode   *select;

	select = nested->command;
	if (select == NULL) {
		// insert 'SELECT' node to beginning of list
		select = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		select->type = SQN_SELECT;
		nested->command = select;
		// The first children node is reserved for DISTINCT
		sub_node = sq_query_node_new(query);
		sub_node->type = SQN_NONE;
		select->children = sub_node;
		// default is 'SELECT *'
		sub_node->children = sq_query_node_new(query);
		sub_node = sub_node->children;
		sub_node->type = SQN_ASTERISK;
		// FROM
//		sub_node = nested->name;
//		if (sub_node && sub_node->type != SQN_FROM)
//			sub_node->type = SQN_FROM;
	}
	else if (select->type != SQN_SELECT)
		return false;

	va_start(arg_list, query);
	sq_query_insert_column_list(query, select, arg_list);
	va_end(arg_list);

	return true;
}

bool sq_query_distinct(SqQuery *query)
{
	SqQueryNode *command = query->nested_cur->command;

	if (command == NULL) {
		// if no 'SELECT' command, call sq_query_select() insert it.
		sq_query_select(query, NULL);
		command = query->nested_cur->command;
	}
	else if (command->type != SQN_SELECT)
		return false;
	// The first children node is reserved for DISTINCT by sq_query_select()
	command->children->type = SQN_DISTINCT;
	return true;
}

// 'clause_code' can be SQN_WHERE, SQN_HAVING, and SQN_ON.
// 'logi_type'   can be SQ_QUERYLOGI_OR, SQ_QUERYLOGI_AND, SQ_QUERYLOGI_NOT.
// if 'clause_code' is SQN_WHERE, the 'logi_type' can set SQ_QUERYLOGI_NOT.
static SqQueryNode *sq_query_clause_logical(SqQuery *query, SqQueryNode *parent, int clause_type, int logi_type)
{
	SqQueryNode   *clause = NULL;
	SqQueryNode   *node;

	if (parent == NULL)
		parent = query->nested_cur->parent;
	// insert clause in specify position
	clause = sq_query_node_find(parent, clause_type, &node);
	if (clause == NULL) {
		clause = sq_query_node_insert(parent, node, sq_query_node_new(query));
		clause->type = clause_type;
	}

	// append NONE, OR, AND in clause->children
	node = sq_query_node_new(query);
	if (clause->children)
		node->type = SQN_OR + (logi_type & ~(SQ_QUERYLOGI_NOT | SQ_QUERYARG_RAW));    // SQ_QUERYARG_RAW deprecated
	else
		node->type = SQN_NONE;
	sq_query_node_append(clause, node);

	// add or set NOT node.
	if (logi_type & SQ_QUERYLOGI_NOT) {
		if (node->type != SQN_NONE) {
			node->children = sq_query_node_new(query);
			node = node->children;
		}
		node->type = SQN_NOT;
	}

	// SQ_QUERYARG_RAW deprecated
	if (logi_type & SQ_QUERYARG_RAW) {
		node->children = sq_query_node_new(query);
		node = node->children;
		node->type = SQN_VALUE;
	}

	return node;
}

void sq_query_where_logical(SqQuery *query, int logi_type, ...)
{
	va_list        arg_list;
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_type);

	va_start(arg_list, logi_type);
	if (logi_type & SQ_QUERYARG_RAW) {
		char *raw = va_arg(arg_list, char*);
		node->value = strdup(raw);
	}
	else
		node->children = sq_query_condition(query, node, arg_list);
	va_end(arg_list);
}

void sq_query_where_exists_logical(SqQuery *query, int logi_type)
{
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_type);

	node->children = sq_query_node_new(query);
	node = node->children;
	node->type = SQN_EXISTS;

	sq_query_push_nested(query, node);
}

void sq_query_where_between_logical(SqQuery *query, const char *column_name, int logi_type, const char* format, ...)
{
	va_list        arg_list;
	va_list        arg_copy;
	char          *str;
	int            length;
	union {
		char          *format;
		SqQueryNode   *node;
	} temp;

	// generate new printf format string
	length = strlen(format) *2 + 13 + 1;    // strlen("BETWEEN  AND ") + '\0'
	temp.format = malloc(length);
	strcpy(temp.format, "BETWEEN ");
	strcpy(temp.format +8,  format);    // str + strlen("BETWEEN ")
	strcat(temp.format +8,  " AND ");
	strcat(temp.format +13, format);    // str + strlen("BETWEEN  AND ")

	va_start(arg_list, format);
	va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
	length = _vscprintf(temp.format, arg_copy) + 1;
#else				// for C99 standard
	length = vsnprintf(NULL, 0, temp.format, arg_copy) + 1;
#endif
	va_end(arg_copy);
	str = malloc(length);
	vsnprintf(str, length, temp.format, arg_list);
	va_end(arg_list);
	// release generated format string of this function
	free(temp.format);

	temp.node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_type & (~SQ_QUERYLOGI_NOT));
	temp.node->children = sq_query_column_in(query, column_name, logi_type, str);
}

void sq_query_where_in_logical(SqQuery *query, const char *column_name, int logi_type, int n_args, const char* format, ...)
{
	va_list        arg_list;
	va_list        arg_copy;
	char          *str;
	int            length;
	int            format_len;
	union {
		char          *format;
		SqQueryNode   *node;
	} temp;

	// generate new printf format string
	format_len = strlen(format);
	length = (format_len+1) *n_args + 5 + 1;    // strlen("IN ()") + '\0'
	temp.format = malloc(length);
	strcpy(temp.format, "IN (");
	//   char *cur = temp.format +strlen("IN (");
	for (char *cur = temp.format +4;  n_args > 0;  n_args--) {
		strcpy(cur, format);
		cur += format_len;
		if (n_args > 1)
			*cur++ = ',';
		else {
			*cur++ = ')';
			*cur = 0;
			break;
		}
	}

	va_start(arg_list, format);
	va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
	length = _vscprintf(temp.format, arg_copy) + 1;
#else				// for C99 standard
	length = vsnprintf(NULL, 0, temp.format, arg_copy) + 1;
#endif
	va_end(arg_copy);
	str = malloc(length);
	vsnprintf(str, length, temp.format, arg_list);
	va_end(arg_list);
	// release generated format string of this function
	free(temp.format);

	temp.node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_type & (~SQ_QUERYLOGI_NOT));
	temp.node->children = sq_query_column_in(query, column_name, logi_type, str);
}

void sq_query_where_null_logical(SqQuery *query, const char *column_name, int logi_type)
{
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_type & (~SQ_QUERYLOGI_NOT));
	node->children = sq_query_column_in(query, column_name, 0, NULL);
	// 'node' pointer node of column_name
	node = node->children;
	// 'node' pointer node of value
	node = node->children;

	if (logi_type & SQ_QUERYLOGI_NOT)
		node->value = strdup("IS NOT NULL");
	else
		node->value = strdup("IS NULL");
}

void sq_query_join_full(SqQuery *query, int join_type, const char *table, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *joinon = nested->joinon;
	SqQueryNode   *node;

	if (joinon == NULL)
		sq_query_node_find(nested->parent, SQN_JOIN, &node);
	else
		node = joinon;

	// insert new JOIN node
	joinon = sq_query_node_insert(nested->parent, node, sq_query_node_new(query));
	joinon->type = SQN_JOIN + join_type;
	nested->joinon = joinon;
	// insert table node.  If table is NULL, it has call sq_query_push_nested()
	sq_query_insert_table_node(query, joinon, table);
	if (table == NULL)
		return;

	// ON
	if (join_type != SQ_QUERYJOIN_CROSS) {
		node = sq_query_clause_logical(query, joinon, SQN_ON, SQ_QUERYLOGI_AND);
		va_start(arg_list, table);
		node->children = sq_query_condition(query, node, arg_list);
		va_end(arg_list);
	}
}

void sq_query_on_logical(SqQuery *query, int logi_type, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	union {
		SqQueryNode   *joinon;
		SqQueryNode   *node;
	} temp;

	temp.joinon = nested->joinon;
	if (temp.joinon == NULL)
		temp.joinon = nested->parent;
	temp.node = sq_query_clause_logical(query, temp.joinon, SQN_ON, logi_type);

	va_start(arg_list, logi_type);
	if (logi_type & SQ_QUERYARG_RAW) {
		char *raw = va_arg(arg_list, char*);
		temp.node->value = strdup(raw);
	}
	else
		temp.node->children = sq_query_condition(query, temp.node, arg_list);
	va_end(arg_list);
}

void sq_query_group_by(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *groupby;
	SqQueryNode   *node;

	groupby = sq_query_node_find(nested->parent, SQN_GROUP_BY, &node);
	if (groupby == NULL) {
		groupby = sq_query_node_insert(nested->parent, node, sq_query_node_new(query));
		groupby->type = SQN_GROUP_BY;
	}

	va_start(arg_list, query);
	sq_query_insert_column_list(query, groupby, arg_list);
	va_end(arg_list);
	// for sq_query_as()
	nested->aliasable = NULL;
}

void sq_query_having_logical(SqQuery *query, int logi_type, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, nested->parent, SQN_HAVING, logi_type);

	va_start(arg_list, logi_type);
	if (logi_type & SQ_QUERYARG_RAW) {
		char *raw = va_arg(arg_list, char*);
		node->value = strdup(raw);
	}
	else
		node->children = sq_query_condition(query, node, arg_list);
	va_end(arg_list);
}

void sq_query_order_by(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *orderby;
	SqQueryNode   *node;

	orderby = sq_query_node_find(nested->parent, SQN_ORDER_BY, &node);
	if (orderby == NULL) {
		orderby = sq_query_node_insert(nested->parent, node, sq_query_node_new(query));
		orderby->type = SQN_ORDER_BY;
	}

	va_start(arg_list, query);
	sq_query_insert_column_list(query, orderby, arg_list);
	va_end(arg_list);
	// for sq_query_as()
	nested->aliasable = NULL;
}

void sq_query_order_sorted(SqQuery *query, int sort_type)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *orderby;
	SqQueryNode   *node;

	orderby = sq_query_node_find(nested->parent, SQN_ORDER_BY, &node);
	if (orderby == NULL)
		return;
	node = sq_query_node_last(orderby->children);
	if (node == NULL)
		return;
	// if column_name is existed.
	if (node->children) {
		node = node->children;
		// set order node
		if (node->children == NULL)
			node->children = sq_query_node_new(query);
		node->children->type = SQN_ASC + sort_type;
	}
}

void *sq_query_union(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *union_node;

	union_node = sq_query_node_append(nested->parent, sq_query_node_new(query));
	union_node->type = SQN_UNION;

	sq_query_push_nested(query, union_node);
	return union_node;
}

void sq_query_union_all(SqQuery *query)
{
	((SqQueryNode*)sq_query_union(query))->type = SQN_UNION_ALL;
}

void *sq_query_limit(SqQuery *query, int64_t count)
{
	SqQueryNested *nested = query->nested_cur;
	union {
		SqQueryNode *node;
		SqQueryNode *count;
	} limit;
	union {
		SqQueryNode *node;
		int          len;
	} temp;

	limit.node = sq_query_node_find(nested->parent, SQN_LIMIT, &temp.node);
	if (limit.node) {
		limit.count = limit.node->children;
		free(limit.count->value);
	}
	else {
		limit.node = sq_query_node_insert(nested->parent, temp.node, sq_query_node_new(query));
		limit.node->type = SQN_LIMIT;
		limit.node->children = sq_query_node_new(query);
		limit.count = limit.node->children;
		limit.count->type = SQN_VALUE;
	}
	temp.len = snprintf(NULL, 0, "%"PRId64, count) + 1;
	limit.count->value = malloc(temp.len);
	snprintf(limit.count->value, temp.len, "%"PRId64, count);

	return limit.count;
}

void sq_query_offset(SqQuery *query, int64_t index)
{
	SqQueryNested *nested = query->nested_cur;
	union {
		SqQueryNode *node;
		SqQueryNode *count;
		int          len;
	} limit;
	union {
		SqQueryNode *node;
		SqQueryNode *index;
	} offset;

	limit.node = sq_query_node_find(nested->parent, SQN_LIMIT, NULL);
	if (limit.node == NULL)
		limit.count = sq_query_limit(query, 1);
	else
		limit.count = limit.node->children;

	if (limit.count->children) {
		offset.node  = limit.count->children;
		offset.index = offset.node->children;
		free(offset.index->value);
	}
	else {
		limit.count->children = sq_query_node_new(query);
		offset.node = limit.count->children;
		offset.node->type = SQN_OFFSET;
		offset.node->children = sq_query_node_new(query);
		offset.index = offset.node->children;
		offset.index->type = SQN_VALUE;
	}

	limit.len = snprintf(NULL, 0, "%"PRId64, index) + 1;
	offset.index->value = malloc(limit.len);
	snprintf(offset.index->value, limit.len, "%"PRId64, index);
}

void sq_query_delete(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->command;

	if (node == NULL) {
		node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		node->type = SQN_DELETE;
		nested->command = node;
	}
}

void sq_query_truncate(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->command;

	if (node == NULL) {
		node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		node->type = SQN_TRUNCATE_TABLE;
		nested->command = node;
	}
	node = nested->name;
	if (node && node->type == SQN_FROM)
		node->type = SQN_NONE;
}

// ----------------------------------------------------------------------------
// static functions

// used by SELECT, GROUP BY, ORDER BY
static void sq_query_insert_column_list(SqQuery *query, SqQueryNode *node, va_list arg_list)
{
	const char   *name;
	SqQueryNode  *sub_node;

	// get last column
	sub_node = sq_query_node_last(node->children);
	if (sub_node == NULL) {
		sub_node = sq_query_node_new(query);
		sub_node->type = SQN_NONE;
		node->children = sub_node;
	}
	node = sub_node;

	while ( (name = va_arg(arg_list, const char*)) ) {
		sub_node = node->children;
		// replace '*' if user specify column for sq_query_select()
		if (sub_node && sub_node->type == SQN_ASTERISK) {
			sub_node->type  = SQN_VALUE;
			sub_node->value = strdup(name);
			continue;
		}
		// add ',' if user specify multiple column
		// if current 'node' was not just created.
		if (node->children != NULL) {
			sub_node = sq_query_node_new(query);
			sub_node->type = SQN_COMMA;
			node->next = sub_node;
			node = sub_node;
		}
		// add column name in children node.
		sub_node = sq_query_node_new(query);
		sub_node->type = SQN_VALUE;
		sub_node->value = strdup(name);
		node->children = sub_node;
		// for sq_query_as()
		query->nested_cur->aliasable = node;
	}
}

// used by FROM, JOIN.
static void sq_query_insert_table_node(SqQuery *query, SqQueryNode *node, const char *table_name)
{
	node->children = sq_query_node_new(query);
	node = node->children;
	node->type = SQN_NONE;
	query->nested_cur->aliasable = node;    // for sq_query_as()

	// table name
	if (table_name == NULL)
		sq_query_push_nested(query, node);
	else {
		node->children = sq_query_node_new(query);
		node = node->children;
		node->type = SQN_VALUE;
		node->value = strdup(table_name);
	}
}

// used by BETWEEN and IN
static SqQueryNode *sq_query_column_in(SqQuery *query, const char *column_name, int logi_type, char *value_str)
{
	SqQueryNode   *column_node;
	SqQueryNode   *node;

	// column node
	column_node = sq_query_node_new(query);
	column_node->type = SQN_VALUE;
	column_node->value = strdup(column_name);
	node = column_node;
	// NOT node
	if (logi_type & SQ_QUERYLOGI_NOT) {
		node->children = sq_query_node_new(query);
		node = node->children;
		node->type = SQN_NOT;
	}
	// BETWEEN (or IN) value node
	node->children = sq_query_node_new(query);
	node = node->children;
	node->type = SQN_VALUE;
	node->value = value_str;

	return column_node;
}

static SqQueryNode *sq_query_condition(SqQuery *query, SqQueryNode *node, va_list arg_list)
{
	va_list   arg_copy;
	char     *args[3];
	union {
		int       length;
		char     *dest;
	} mem;
	union {
		int       length;
		int       index;
		char     *cur;
	} temp;

	// ====== first argument ======
	args[0] = va_arg(arg_list, char*);
	// It use subquery if 1st argument is NULL.
	if (args[0] == NULL) {
		sq_query_push_nested(query, node);
		return NULL;
	}
	// create node for condition
	node = sq_query_node_new(query);
	node->type = SQN_VALUE;
	// search % sign if args[0] is printf format string
	for (temp.cur = args[0];  *temp.cur;  temp.cur++) {
		if (*temp.cur == '%') {
			if (temp.cur[1] == '%')    // escape % sign
				temp.cur++;
			else
				break;
		}
	}
	// if argv[0] is printf format string
	if (*temp.cur == '%') {
		va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
		mem.length = _vscprintf(args[0], arg_copy) + 1;
#else				// for C99 standard
		mem.length = vsnprintf(NULL, 0, args[0], arg_copy) + 1;
#endif
		va_end(arg_copy);
		node->value = malloc(mem.length);
		vsnprintf(node->value, mem.length, args[0], arg_list);
		return node;
	}
	mem.length = temp.cur - args[0] + 1;           // + ' '
#if SQ_QUERY_USE_OLD_CONDITION
	// ====== second argument ======
	args[1] = va_arg(arg_list, char*);
	if (args[1]) {
		// search operators if args[1] is not printf format string
		temp.cur = strpbrk(args[1], "!=<>");
		if (temp.cur == NULL && strcasecmp(args[1], "LIKE") == 0)
			temp.cur = args[1];
	}
	// if args[1] is NULL or value string
	if (args[1] == NULL || temp.cur == NULL) {
#ifndef NDEBUG
		if (args[1] && strchr(args[1], '%') == NULL)
			fprintf(stderr, "sq_query_condition(): Please pass printf format string before passing value of condition.\n");
#endif
		args[2] = args[1];    // move 2nd argument to 3rd argument
		args[1] = "=";        // set  2nd argument to equal operator
		mem.length += 2;      // + '=' + ' '
	}
	// ====== third argument ======
	else {
		args[2] = va_arg(arg_list, char*);
		// count length of second argument
		mem.length += (int)strlen(args[1]) + 1;    // + ' '
	}
#else
	// count length of argv[0] if is NOT printf format string
//	mem.length = strlen(args[0]) + 1;              // + ' '
	// ====== second argument ======
	args[1] = va_arg(arg_list, char*);
	if (args[1]) {
		// search % sign if args[1] is printf format string
		for (temp.cur = args[1];  *temp.cur;  temp.cur++)
			if (*temp.cur == '%')
				break;
	}
	// if args[1] is NULL or printf format string
	if (args[1] == NULL || *temp.cur == '%') {
		args[2] = args[1];    // move 2nd argument to 3rd argument
		args[1] = "=";        // set  2nd argument to equal operator
		mem.length += 2;      // + '=' + ' '
	}
	// ====== third argument ======
	else {
		args[2] = va_arg(arg_list, char*);
		// count length of second argument
		mem.length += temp.cur - args[1] + 1;      // + ' '
	}
#endif

	if (args[2]) {
#ifndef NDEBUG
		if (strchr(args[2], '%') == NULL)
			fprintf(stderr, "sq_query_condition(): Please pass printf format string before passing value of condition.\n");
#endif
		va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
		temp.length = _vscprintf(args[2], arg_copy) +1;
#else				// for C99 standard
		temp.length = vsnprintf(NULL, 0, args[2], arg_copy) +1;
#endif
		va_end(arg_copy);
		node->value = malloc(mem.length + temp.length);
		vsnprintf(node->value + mem.length, temp.length, args[2], arg_list);
	}
	// It use subquery if 4th argument is NULL.
	else {
		node->value = malloc(mem.length);
		sq_query_push_nested(query, node);
	}

	mem.dest = node->value;
	for (temp.index = 0;  temp.index < 2;  temp.index++) {
		while (*args[temp.index])
			*mem.dest++ = *args[temp.index]++;
		*mem.dest++ = ' ';
	}
	if (args[2] == NULL)
		*(mem.dest-1) = 0;
	return node;
}

// ------------------------------------
// sq_query_to_sql()

static void node_to_buf(SqQueryNode *node, SqQuery *query)
{
	char *src;
	char *dest;
	int   value_len;

	for (;  node;  node = node->next) {
		if (node->type < SQN_N_CODE) {
			node->value = (char*) sqnword[node->type];
			if (node->type == SQN_COMMA)
				query->length--;
		}
		src = node->value;
		if (*src) {
			value_len = (int)strlen(src) + 1;    // + " "
			if (query->length +value_len >= query->allocated) {
				query->allocated *= 2;
				query->str = realloc(query->str, query->allocated);
			}
			dest = query->str + query->length;
			query->length += value_len; 
			while (*src)
				*dest++ = *src++;
			*dest++ = ' ';
		}
		node_to_buf(node->children, query);
	}
}

const char *sq_query_c(SqQuery *query)
{
	union {
		SqQueryNested *nested;
		char          *dest;
	} temp;

	temp.nested = query->nested_cur;
	if (temp.nested->name) {
		if (temp.nested->command == NULL)
			sq_query_select(query, NULL);
	}

	if (query->str == NULL) {
		query->str = malloc(SQ_QUERY_STR_SIZE_DEFAULT);
		query->allocated = SQ_QUERY_STR_SIZE_DEFAULT;
	}
	query->length = 0;
	node_to_buf(query->root.children, query);

	temp.dest = query->str + query->length;
	if (query->length > 0)
		temp.dest--;
	*temp.dest = 0;

	return query->str;
}

char *sq_query_to_sql(SqQuery *query)
{
	char *result;

	sq_query_c(query);
	result = realloc(query->str, query->length);
	query->str = NULL;
	query->length = 0;
//	query->allocated = 0;

	return result;
}

// parent->type must be SQN_FROM or SQN_JOIN
static const char *get_table(SqQueryNode *parent)
{
	SqQueryNode  *child;

	// parent pointer to NONE node
	parent = parent->children;
	//
	child = parent->children;
	if (child->type == SQN_VALUE)
		return child->value;
	else {
		// nested query
		child = sq_query_node_find(parent, SQN_FROM, NULL);
		if (child)
			return get_table(child);
	}
	return NULL;
}

// array[0] = table1_name, array[1] = table1_as_name,
// array[2] = table2_name, array[3] = table2_as_name, ...etc
int  sq_query_get_table_as_names(SqQuery *query, SqPtrArray *table_and_as_names)
{
	SqQueryNode  *qnode;
	SqQueryNode  *child;
	const char   *table_name;

	// FROM table1_name AS table1_as_name
	// JOIN table2_name AS table2_as_name
	for (qnode = query->root.children; qnode;  qnode = qnode->next) {
		if (qnode->type != SQN_FROM) {
			if (qnode->type < SQN_JOIN || qnode->type > SQN_CROSS_JOIN)
				continue;
		}
		// table name
		table_name = get_table(qnode);
		if (table_name == NULL)
			return 0;
		sq_ptr_array_append(table_and_as_names, table_name);

		// AS name
		child = sq_query_node_find(qnode, SQN_AS, NULL);
		if (child && child->children)
			sq_ptr_array_append(table_and_as_names, child->children->value);
		else
			sq_ptr_array_append(table_and_as_names, NULL);
	}
	return table_and_as_names->length / 2;
}

void sq_query_select_table_as(SqQuery *query, SqTable *table, const char *table_as_name, const char *quotes)
{
	SqType   *type = (SqType*)table->type;
	SqColumn *column;
	char     *buffer = NULL;
	int       buf_len;

	if (table_as_name == NULL)
		table_as_name = table->name;
	if (quotes == NULL)
		quotes = "\"\"";
	for (int index = 0;  index < type->n_entry;  index++) {
		column = (SqColumn*)type->entry[index];
		if (SQ_TYPE_IS_FAKE(column->type))
			continue;
		buf_len = snprintf(NULL, 0, "%c%s%c.%c%s%c AS %c%s.%s%c",
				quotes[0], table_as_name, quotes[1],
				quotes[0], column->name,  quotes[1],
				quotes[0], table_as_name, column->name, quotes[1]) + 1;
		buffer = realloc(buffer, buf_len);
		snprintf(buffer, buf_len, "%c%s%c.%c%s%c AS %c%s.%s%c",
				quotes[0], table_as_name, quotes[1],
				quotes[0], column->name,  quotes[1],
				quotes[0], table_as_name, column->name, quotes[1]);
		sq_query_select(query, buffer, NULL);
	}
	free(buffer);
}

// ----------------------------------------------------------------------------
// push/pop SqQueryNested

#define NESTED_CHUNK_SIZE    (1 << 2)    // 4
#define NESTED_CHUNK_MASK    (NESTED_CHUNK_SIZE -1)

SqQueryNested *sq_query_push_nested(SqQuery *query, SqQueryNode *parent)
{
	SqQueryNested *outer = query->nested_cur;
	SqQueryNested *nested;

	// alloc multiple SqQueryNested each time
	if ((query->nested_count & NESTED_CHUNK_MASK) == 0)
		nested = malloc(sizeof(SqQueryNested) * NESTED_CHUNK_SIZE);
	else
		nested = query->nested_cur +1;
	query->nested_count++;

	if (outer)
		outer->inner = nested;
	nested->outer = outer;
	nested->inner = NULL;
	nested->parent = parent;
	nested->command = NULL;
	nested->name = NULL;
	nested->aliasable = NULL;
	nested->joinon = NULL;
	query->nested_cur = nested;
	return nested;
}

/* static */
void  sq_query_pop_nested(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node;

	if (nested) {
		// is not empty subquery?
		node = nested->parent->children;
		if (node) {
			// subquery has command?
			if (nested->name) {
				if (nested->command == NULL)
					sq_query_select(query, NULL);
			}
			else if (node->type >= SQN_WHERE)
				node->type = SQN_NONE;

			if (nested->parent->type != SQN_UNION && nested->parent->type != SQN_UNION_ALL) {
				// insert '(' to beginning of list
				node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
				node->type = SQN_BRACKETS_L;
				// append ')' to ending of list
				node = sq_query_node_append(nested->parent, sq_query_node_new(query));
				node->type = SQN_BRACKETS_R;
			}
		}
		// pop nested from stack
		nested = nested->outer;
		if (nested)
			nested->inner = NULL;
		// free multiple SqQueryNested each time
		query->nested_count--;
		if ((query->nested_count & NESTED_CHUNK_MASK) == 0)
			free(query->nested_cur);
		query->nested_cur = nested;
	}
}

// ----------------------------------------------------------------------------
// SqQueryNode

#define NODE_CHUNK_SIZE   (1 << 5)    // 32
#define NODE_CHUNK_MASK   (NODE_CHUNK_SIZE -1)

struct NodeChunk
{
	struct NodeChunk *prev;
	SqQueryNode nodes[NODE_CHUNK_SIZE];
};

static void sq_query_free_all_node(SqQuery *query)
{
	struct NodeChunk  *chunk = query->node_chunk;
	struct NodeChunk  *prev;

	sq_query_node_free(query->root.children, query);
	query->root.children = NULL;
	// free chunk of SqQueryNode
	while (chunk) {
		prev = chunk->prev;
		free(chunk);
		chunk = prev;
	}

	/* sq_query_clear() will reset these
	query->used  = NULL;
	query->freed = NULL;
	query->node_chunk = NULL;
	query->node_count = 0;
	 */
}

static SqQueryNode *sq_query_node_new(SqQuery *query)
{
	SqQueryNode *node;
	struct NodeChunk  *chunk;

	// reuse freed(recycled) SqQueryNode
	if (query->freed) {
		node = query->freed;
		query->freed = node->next;
	}
	else {
		// get unused SqQueryNode in node_chunk
		if ((query->node_count & NODE_CHUNK_MASK))
			query->used++;
		// allocate multiple SqQueryNode each time
		else {
			chunk = malloc(sizeof(struct NodeChunk));
			chunk->prev = query->node_chunk;
			query->node_chunk = chunk;
			query->used = chunk->nodes;
		}
		query->node_count++;
		node = query->used;
	}

//	node->type = 0;
	node->next = NULL;
	node->children = NULL;
//	node->value = NULL;

	return node;
}

static void sq_query_node_free(SqQueryNode *node, SqQuery *query)
{
	SqQueryNode  *next;

	for (;  node;  node = next) {
		if (node->type == SQN_VALUE)
			free(node->value);
		if (node->children)
			sq_query_node_free(node->children, query);
		// recycle SqQueryNode by adding it to freed list
		next = node->next;
		node->next = query->freed;
		query->freed = node;
	}
}

static SqQueryNode *sq_query_node_last(SqQueryNode *node)
{
	SqQueryNode *prev;

	for(prev = NULL;  node;  node = node->next)
		prev = node;
	return prev;
}

static SqQueryNode *sq_query_node_insert(SqQueryNode *parent, SqQueryNode *prev, SqQueryNode *node)
{
	if (prev == NULL) {
		node->next = parent->children;
		parent->children = node;
	}
	else {
		node->next = prev->next;
		prev->next = node;
	}
	return node;
}

// insert_pos: 'sqn_cmd' must insert in this position
static SqQueryNode *sq_query_node_find(SqQueryNode *node, int sqn_cmd, SqQueryNode **insert_pos)
{
	SqQueryNode *prev = NULL;

	for (node = node->children;  node;  node = node->next) {
		if (node->type >= sqn_cmd) {
			if (node->type == sqn_cmd)
				break;
			else if (sqn_cmd < SQN_SYMBOL && node->type < SQN_SYMBOL) {
				node = NULL;
				break;
			}
		}
		prev = node;
	}

	// if 'sqn_cmd' not found, 'sqn_cmd' must insert in this position
	if (insert_pos)
		insert_pos[0] = prev;
	return node;
}

// ----------------------------------------------------------------------------

static const char *sqnword[SQN_N_CODE] = {
	"",                  // SQN_NONE

	"CREATE TABLE",      // SQN_CREATE_TABLE
	"ALERT TABLE",       // SQN_ALERT_TABLE
	"DROP TABLE",        // SQN_DROP_TABLE
	"TRUNCATE TABLE",    // SQN_TRUNCATE_TABLE
	"INSERT INTO",       // SQN_INSERT_INTO
	"UPDATE",            // SQN_UPDATE
	"DELETE",            // SQN_DELETE
	"SELECT",            // SQN_SELECT

	"FROM",              // SQN_FROM
	"JOIN",              // SQN_JOIN
	"LEFT JOIN",         // SQN_LEFT_JOIN
	"RIGHT JOIN",        // SQN_RIGHT_JOIN
	"FULL JOIN",         // SQN_FULL_JOIN
	"CROSS JOIN",        // SQN_CROSS_JOIN
	"SET",               // SQN_SET        // UPDATE
	"WHERE",             // SQN_WHERE      // SELECT, UPDATE, DELETE
	"GROUP BY",          // SQN_GROUP_BY
	"HAVING",            // SQN_HAVING
	"ORDER BY",          // SQN_ORDER_BY
	"UNION",             // SQN_UNION
	"UNION ALL",         // SQN_UNION_ALL
	"LIMIT",             // SQN_LIMIT

	"DISTINCT",          // SQN_DISTINCT   // SELECT
	"AS",                // SQN_AS         // SELECT: alias name for table or column
	"OR",                // SQN_OR         // WHERE, JOIN
	"AND",               // SQN_AND        // WHERE, JOIN, BETWEEN
	"NOT",               // SQN_NOT        // WHERE
	"EXISTS",            // SQN_EXISTS     // WHERE
	"ON",                // SQN_ON         // JOIN
//	"IN",                // SQN_IN         // WHERE
//	"BETWEEN",           // SQN_BETWEEN    // WHERE, JOIN
	"LIKE",              // SQN_LIKE       // WHERE

	"ASC",               // SQN_ASC        // ORDER BY
	"DESC",              // SQN_DESC       // ORDER BY

	"OFFSET",            // SQN_OFFSET     // SQN_LIMIT

	"VALUES",            // SQN_VALUES     // INSERT INTO

	                     // SQN_SYMBOL     // equal SQN_BRACKETS_L
	"(",                 // SQN_BRACKETS_L
	")",                 // SQN_BRACKETS_R
	",",                 // SQN_COMMA
	"*",                 // SQN_ASTERISK
};

