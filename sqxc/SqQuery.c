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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdint.h>
#include <stdarg.h>       // va_list, va_start, va_end, va_arg, etc.
#include <stdlib.h>
#include <string.h>
#include <stdio.h>        // vsnprintf(), fprintf(), stderr

#include <sqxc/SqQuery.h>

#ifdef _MSC_VER
#define strdup       _strdup
#define snprintf     _snprintf
#define strcasecmp   _stricmp
#endif

#define SQ_QUERY_STR_SIZE_DEFAULT     256

static void sq_query_insert_column_list(SqQuery *query, SqQueryNode *parent, va_list arg_list);
static void sq_query_insert_table_node(SqQuery *query, SqQueryNode *parent, const char *table_name);
static void sq_query_free_all_node(SqQuery *query);

static SqQueryNode *sq_query_column_in(SqQuery *query, const char *column_name, unsigned int logi_args, char *value_str);
static SqQueryNode *sq_query_condition(SqQuery *query, SqQueryNode *parent, unsigned int logi_args, va_list arg_list);

static SqQueryNode *sq_query_node_new(SqQuery *query);
static void         sq_query_node_free(SqQueryNode *node, SqQuery *query);

#define sq_query_node_append(parent, node)     \
		sq_query_node_insert(parent, sq_query_node_last((parent)->children), node)
#define sq_query_node_prepend(parent, node)    \
		sq_query_node_insert(parent, NULL, node)

static SqQueryNode *sq_query_node_insert(SqQueryNode *parent, SqQueryNode *prev, SqQueryNode *node);
static SqQueryNode *sq_query_node_last(SqQueryNode *node);
static SqQueryNode *sq_query_node_find(SqQueryNode *parent, const char *constString, SqQueryNode **insert_pos);

// ----------------------------------------------------------------------------
// constStringGroup will be defined later

#define STR_GROUP        constStringGroup
#define STR_GROUP_LEN    sizeof(constStringGroup)

#define IS_CONST_STR(addr)     ((addr) <  STR_GROUP+STR_GROUP_LEN && (addr) >= STR_GROUP)
#define NOT_CONST_STR(addr)    ((addr) >= STR_GROUP+STR_GROUP_LEN || (addr) <  STR_GROUP)

// ----------------------------------------------------------------------------
// string will be in constStringGroup

#define SQN_NONE_STRING           ""

#define SQN_CREATE_TABLE_STRING   "CREATE TABLE"
#define SQN_ALERT_TABLE_STRING    "ALERT TABLE"
#define SQN_DROP_TABLE_STRING     "DROP TABLE"
#define SQN_TRUNCATE_TABLE_STRING "TRUNCATE TABLE"
#define SQN_INSERT_INTO_STRING    "INSERT INTO"
#define SQN_UPDATE_STRING         "UPDATE"
#define SQN_DELETE_STRING         "DELETE"
#define SQN_SELECT_STRING         "SELECT"

#define SQN_FROM_STRING           "FROM"
#define SQN_JOIN_STRING           "JOIN"
#define SQN_LEFT_JOIN_STRING      "LEFT JOIN"
#define SQN_RIGHT_JOIN_STRING     "RIGHT JOIN"
#define SQN_FULL_JOIN_STRING      "FULL JOIN"
#define SQN_CROSS_JOIN_STRING     "CROSS JOIN"
// #define SQN_SET_STRING            "SET"        // UPDATE
#define SQN_WHERE_STRING          "WHERE"      // SELECT, UPDATE, DELETE
#define SQN_GROUP_BY_STRING       "GROUP BY"
#define SQN_HAVING_STRING         "HAVING"
#define SQN_ORDER_BY_STRING       "ORDER BY"
#define SQN_UNION_STRING          "UNION"
#define SQN_UNION_ALL_STRING      "UNION ALL"
#define SQN_LIMIT_STRING          "LIMIT"

#define SQN_DISTINCT_STRING       "DISTINCT"   // SELECT
#define SQN_AS_STRING             "AS"         // SELECT: alias name for table or column
#define SQN_OR_STRING             "OR"         // WHERE, JOIN
#define SQN_AND_STRING            "AND"        // WHERE, JOIN, BETWEEN
#define SQN_NOT_STRING            "NOT"        // WHERE
#define SQN_EXISTS_STRING         "EXISTS"     // WHERE
#define SQN_ON_STRING             "ON"         // JOIN
// #define SQN_IN_STRING             "IN"         // WHERE
// #define SQN_BETWEEN_STRING        "BETWEEN"    // WHERE, JOIN
// #define SQN_LIKE_STRING           "LIKE"       // WHERE

#define SQN_ASC_STRING            "ASC"        // ORDER BY
#define SQN_DESC_STRING           "DESC"       // ORDER BY

#define SQN_OFFSET_STRING         "OFFSET"     // LIMIT

// #define SQN_VALUES_STRING         "VALUES"     // INSERT INTO

#define SQN_BRACKETS_L_STRING     "("                 //
#define SQN_BRACKETS_R_STRING     ")"                 //
#define SQN_COMMA_STRING          ","                 //

// ----------------------------------------------------------------------------
// string position in constStringGroup

#define SQN_NONE_POS            0

#define SQN_CREATE_TABLE_POS    sizeof(SQN_NONE_STRING)           + SQN_NONE_POS
#define SQN_ALERT_TABLE_POS     sizeof(SQN_CREATE_TABLE_STRING)   + SQN_CREATE_TABLE_POS
#define SQN_DROP_TABLE_POS      sizeof(SQN_ALERT_TABLE_STRING)    + SQN_ALERT_TABLE_POS
#define SQN_TRUNCATE_TABLE_POS  sizeof(SQN_DROP_TABLE_STRING)     + SQN_DROP_TABLE_POS
#define SQN_INSERT_INTO_POS     sizeof(SQN_TRUNCATE_TABLE_STRING) + SQN_TRUNCATE_TABLE_POS
#define SQN_UPDATE_POS          sizeof(SQN_INSERT_INTO_STRING)    + SQN_INSERT_INTO_POS
#define SQN_DELETE_POS          sizeof(SQN_UPDATE_STRING)         + SQN_UPDATE_POS
#define SQN_SELECT_POS          sizeof(SQN_DELETE_STRING)         + SQN_DELETE_POS

#define SQN_FROM_POS            sizeof(SQN_SELECT_STRING)         + SQN_SELECT_POS
#define SQN_JOIN_POS            sizeof(SQN_FROM_STRING)           + SQN_FROM_POS
#define SQN_LEFT_JOIN_POS       sizeof(SQN_JOIN_STRING)           + SQN_JOIN_POS
#define SQN_RIGHT_JOIN_POS      sizeof(SQN_LEFT_JOIN_STRING)      + SQN_LEFT_JOIN_POS
#define SQN_FULL_JOIN_POS       sizeof(SQN_RIGHT_JOIN_STRING)     + SQN_RIGHT_JOIN_POS
#define SQN_CROSS_JOIN_POS      sizeof(SQN_FULL_JOIN_STRING)      + SQN_FULL_JOIN_POS
// #define SQN_SET_POS
#define SQN_WHERE_POS           sizeof(SQN_CROSS_JOIN_STRING)     + SQN_CROSS_JOIN_POS
#define SQN_GROUP_BY_POS        sizeof(SQN_WHERE_STRING)          + SQN_WHERE_POS
#define SQN_HAVING_POS          sizeof(SQN_GROUP_BY_STRING)       + SQN_GROUP_BY_POS
#define SQN_ORDER_BY_POS        sizeof(SQN_HAVING_STRING)         + SQN_HAVING_POS
#define SQN_UNION_POS           sizeof(SQN_ORDER_BY_STRING)       + SQN_ORDER_BY_POS
#define SQN_UNION_ALL_POS       sizeof(SQN_UNION_STRING)          + SQN_UNION_POS
#define SQN_LIMIT_POS           sizeof(SQN_UNION_ALL_STRING)      + SQN_UNION_ALL_POS

#define SQN_DISTINCT_POS        sizeof(SQN_LIMIT_STRING)          + SQN_LIMIT_POS
#define SQN_AS_POS              sizeof(SQN_DISTINCT_STRING)       + SQN_DISTINCT_POS
#define SQN_OR_POS              sizeof(SQN_AS_STRING)             + SQN_AS_POS
#define SQN_AND_POS             sizeof(SQN_OR_STRING)             + SQN_OR_POS
#define SQN_NOT_POS             sizeof(SQN_AND_STRING)            + SQN_AND_POS
#define SQN_EXISTS_POS          sizeof(SQN_NOT_STRING)            + SQN_NOT_POS
#define SQN_ON_POS              sizeof(SQN_EXISTS_STRING)         + SQN_EXISTS_POS
// #define SQN_IN_POS
// #define SQN_BETWEEN_POS
// #define SQN_LIKE_POS

#define SQN_ASC_POS             sizeof(SQN_ON_STRING)             + SQN_ON_POS
#define SQN_DESC_POS            sizeof(SQN_ASC_STRING)            + SQN_ASC_POS
#define SQN_OFFSET_POS          sizeof(SQN_DESC_STRING)           + SQN_DESC_POS

// #define SQN_VALUES_POS

#define SQN_BRACKETS_L_POS      sizeof(SQN_OFFSET_STRING)         + SQN_OFFSET_POS
#define SQN_BRACKETS_R_POS      sizeof(SQN_BRACKETS_L_STRING)     + SQN_BRACKETS_L_POS
#define SQN_COMMA_POS           sizeof(SQN_BRACKETS_R_STRING)     + SQN_BRACKETS_R_POS

// SQN_SYMBOL_POS = SQN_BRACKETS_L
#define SQN_SYMBOL_POS          SQN_BRACKETS_L_POS

// ----------------------------------------------------------------------------
// string address in constStringGroup

#define SQN_NONE                (STR_GROUP + SQN_NONE_POS)

#define SQN_CREATE_TABLE        (STR_GROUP + SQN_CREATE_TABLE_POS)
#define SQN_ALERT_TABLE         (STR_GROUP + SQN_ALERT_TABLE_POS)
#define SQN_DROP_TABLE          (STR_GROUP + SQN_DROP_TABLE_POS)
#define SQN_TRUNCATE_TABLE      (STR_GROUP + SQN_TRUNCATE_TABLE_POS)
#define SQN_INSERT_INTO         (STR_GROUP + SQN_INSERT_INTO_POS)
#define SQN_UPDATE              (STR_GROUP + SQN_UPDATE_POS)
#define SQN_DELETE              (STR_GROUP + SQN_DELETE_POS)
#define SQN_SELECT              (STR_GROUP + SQN_SELECT_POS)

#define SQN_FROM                (STR_GROUP + SQN_FROM_POS)
#define SQN_JOIN                (STR_GROUP + SQN_JOIN_POS)
#define SQN_LEFT_JOIN           (STR_GROUP + SQN_LEFT_JOIN_POS)
#define SQN_RIGHT_JOIN          (STR_GROUP + SQN_RIGHT_JOIN_POS)
#define SQN_FULL_JOIN           (STR_GROUP + SQN_FULL_JOIN_POS)
#define SQN_CROSS_JOIN          (STR_GROUP + SQN_CROSS_JOIN_POS)
// #define SQN_SET                 (STR_GROUP + SQN_SET_POS)
#define SQN_WHERE               (STR_GROUP + SQN_WHERE_POS)
#define SQN_GROUP_BY            (STR_GROUP + SQN_GROUP_BY_POS)
#define SQN_HAVING              (STR_GROUP + SQN_HAVING_POS)
#define SQN_ORDER_BY            (STR_GROUP + SQN_ORDER_BY_POS)
#define SQN_UNION               (STR_GROUP + SQN_UNION_POS)
#define SQN_UNION_ALL           (STR_GROUP + SQN_UNION_ALL_POS)
#define SQN_LIMIT               (STR_GROUP + SQN_LIMIT_POS)

#define SQN_DISTINCT            (STR_GROUP + SQN_DISTINCT_POS)
#define SQN_AS                  (STR_GROUP + SQN_AS_POS)
#define SQN_OR                  (STR_GROUP + SQN_OR_POS)
#define SQN_AND                 (STR_GROUP + SQN_AND_POS)
#define SQN_NOT                 (STR_GROUP + SQN_NOT_POS)
#define SQN_EXISTS              (STR_GROUP + SQN_EXISTS_POS)
#define SQN_ON                  (STR_GROUP + SQN_ON_POS)
// #define SQN_IN                  (STR_GROUP + SQN_IN_POS)
// #define SQN_BETWEEN             (STR_GROUP + SQN_BETWEEN_POS)
// #define SQN_LIKE                (STR_GROUP + SQN_LIKE_POS)

#define SQN_ASC                 (STR_GROUP + SQN_ASC_POS)
#define SQN_DESC                (STR_GROUP + SQN_DESC_POS)
#define SQN_OFFSET              (STR_GROUP + SQN_OFFSET_POS)

// #define SQN_VALUES              (STR_GROUP + SQN_VALUES_POS)

#define SQN_BRACKETS_L          (STR_GROUP + SQN_BRACKETS_L_POS)
#define SQN_BRACKETS_R          (STR_GROUP + SQN_BRACKETS_R_POS)
#define SQN_COMMA               (STR_GROUP + SQN_COMMA_POS)

// SQN_SYMBOL = SQN_BRACKETS_L
#define SQN_SYMBOL              SQN_BRACKETS_L

// ----------------------------------------------------------------------------
// string group - Make sure all SqQuery words are placed together in memory

static const char constStringGroup[] =
{
	SQN_NONE_STRING           "\0"

	SQN_CREATE_TABLE_STRING   "\0"
	SQN_ALERT_TABLE_STRING    "\0"
	SQN_DROP_TABLE_STRING     "\0"
	SQN_TRUNCATE_TABLE_STRING "\0"
	SQN_INSERT_INTO_STRING    "\0"
	SQN_UPDATE_STRING         "\0"
	SQN_DELETE_STRING         "\0"
	SQN_SELECT_STRING         "\0"

	SQN_FROM_STRING           "\0"
	SQN_JOIN_STRING           "\0"
	SQN_LEFT_JOIN_STRING      "\0"
	SQN_RIGHT_JOIN_STRING     "\0"
	SQN_FULL_JOIN_STRING      "\0"
	SQN_CROSS_JOIN_STRING     "\0"
//	SQN_SET_STRING            "\0"
	SQN_WHERE_STRING          "\0"
	SQN_GROUP_BY_STRING       "\0"
	SQN_HAVING_STRING         "\0"
	SQN_ORDER_BY_STRING       "\0"
	SQN_UNION_STRING          "\0"
	SQN_UNION_ALL_STRING      "\0"
	SQN_LIMIT_STRING          "\0"

	SQN_DISTINCT_STRING       "\0"
	SQN_AS_STRING             "\0"
	SQN_OR_STRING             "\0"
	SQN_AND_STRING            "\0"
	SQN_NOT_STRING            "\0"
	SQN_EXISTS_STRING         "\0"
	SQN_ON_STRING             "\0"
//	SQN_IN_STRING             "\0"
//	SQN_BETWEEN_STRING        "\0"
//	SQN_LIKE_STRING           "\0"

	SQN_ASC_STRING            "\0"
	SQN_DESC_STRING           "\0"

	SQN_OFFSET_STRING         "\0"

//	SQN_VALUES_STRING         "\0"

	// SQN_SYMBOL = SQN_BRACKETS_L
	SQN_BRACKETS_L_STRING     "\0"
	SQN_BRACKETS_R_STRING     "\0"
	SQN_COMMA_STRING          "\0"
};

// ----------------------------------------------------------------------------
// SqQueryNested

struct SqQueryNested
{
	SqQueryNested  *outer;
	SqQueryNested  *inner;

	SqQueryNode    *parent;
	SqQueryNode    *command;    // SQL command (SELECT, UPDATE, etc.)
	SqQueryNode    *name;       // table, column, index, or database name
	SqQueryNode    *aliasable;  // sq_query_as()

	// current condition
	SqQueryNode    *joinon;     // JOIN table ON  AND  OR
};

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

static void sq_query_node_set_raw(SqQueryNode *node, unsigned int raw_args, va_list arg_list)
{
	va_list        arg_copy;
	const char    *str;
	int            length;

	str = va_arg(arg_list, char*);
	if ((raw_args & SQ_QUERYARGS_N_MASK) == SQ_QUERYARGS_1)
		node->str = strdup(str);
	else {
		va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
		length = _vscprintf(str, arg_copy) + 1;
#else				// for C99 standard
		length = vsnprintf(NULL, 0, str, arg_copy) + 1;
#endif
		va_end(arg_copy);
		node->str = malloc(length);
		vsnprintf(node->str, length, str, arg_list);
	}
}

void sq_query_append(SqQuery *query, unsigned int raw_args, ...)
{
	va_list        arg_list;
	SqQueryNode   *node;

	node = sq_query_node_append(query->nested_cur->parent, sq_query_node_new(query));
	va_start(arg_list, raw_args);
	sq_query_node_set_raw(node, raw_args, arg_list);
	va_end(arg_list);
}

bool sq_query_from(SqQuery *query, const char *table)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->name;

	if (node)
		return false;
	// insert table node after command node
	node = sq_query_node_insert(nested->parent, nested->command, sq_query_node_new(query));
	// command exists and is not DELETE or SELECT
	if (nested->command && nested->command->str < SQN_DELETE)
		node->str = (char*)SQN_NONE;
	else
		node->str = (char*)SQN_FROM;
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
	sub_node->str = (char*)SQN_AS;
	sub_node->next = node->next;
	node->next = sub_node;
	node = sub_node;
	// insert alias name in children of above AS node
	sub_node = sq_query_node_new(query);
	sub_node->str = strdup(name);
	node->children = sub_node;
	// clear aliasable
	query->nested_cur->aliasable = NULL;
}

bool sq_query_select_list(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *sub_node;
	SqQueryNode   *select;

	select = nested->command;
	if (select == NULL) {
		// insert 'SELECT' node to beginning of list
		select = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		select->str = (char*)SQN_SELECT;
		nested->command = select;
		// The first children node is reserved for DISTINCT
		sub_node = sq_query_node_new(query);
		sub_node->str = (char*)SQN_NONE;
		select->children = sub_node;
	}
	else if (select->str != SQN_SELECT) {
#ifndef NDEBUG
		fprintf(stderr, "%s: current query is not SELECT statement.\n",
		        "sq_query_select_list()");
#endif
		return false;
	}

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
	else if (command->str != SQN_SELECT) {
#ifndef NDEBUG
		fprintf(stderr, "%s: current query is not SELECT statement.\n",
		        "sq_query_distinct()");
#endif
		return false;
	}
	// The first children node is reserved for DISTINCT by sq_query_select()
	command->children->str = (char*)SQN_DISTINCT;
	return true;
}

// 'clauseStr' can be SQN_WHERE, SQN_HAVING, and SQN_ON.
// 'logi_args' can be SQ_QUERYLOGI_OR, SQ_QUERYLOGI_AND, SQ_QUERYLOGI_NOT.
// if 'clause_code' is SQN_WHERE, the 'logi_args' can set SQ_QUERYLOGI_NOT.
static SqQueryNode *sq_query_clause_logical(SqQuery *query, SqQueryNode *parent, const char *clauseStr, unsigned int logi_args)
{
	// for SQ_QUERYLOGI_X series
	const uint16_t nodeStrLogiPos[] = {
		SQN_OR_POS,     // SQ_QUERYLOGI_OR       0
		SQN_AND_POS,    // SQ_QUERYLOGI_AND      1
		SQN_NOT_POS,    // SQ_QUERYLOGI_NOT      2
	};
	SqQueryNode   *clause = NULL;
	SqQueryNode   *node;

	if (parent == NULL)
		parent = query->nested_cur->parent;
	// insert clause in specify position
	clause = sq_query_node_find(parent, clauseStr, &node);
	if (clause == NULL) {
		clause = sq_query_node_insert(parent, node, sq_query_node_new(query));
		clause->str = (char*)clauseStr;
	}

	// append NONE, OR, AND in clause->children
	node = sq_query_node_new(query);
	if (clause->children)
		node->str = (char*)STR_GROUP + nodeStrLogiPos[logi_args & ~(SQ_QUERYLOGI_NOT | SQ_QUERYARGS_MASK)];
	else
		node->str = (char*)SQN_NONE;
	sq_query_node_append(clause, node);

	// add or set NOT node.
	if (logi_args & SQ_QUERYLOGI_NOT) {
		if (node->str != SQN_NONE) {
			node->children = sq_query_node_new(query);
			node = node->children;
		}
		node->str = (char*)SQN_NOT;
	}

	// create & insert node for RAW
	if (logi_args & SQ_QUERYARGS_RAW) {
		node->children = sq_query_node_new(query);
		node = node->children;
	}

	return node;
}

void sq_query_where_logical(SqQuery *query, unsigned int logi_args, ...)
{
	va_list        arg_list;
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_args);

	va_start(arg_list, logi_args);
	if (logi_args & SQ_QUERYARGS_RAW)
		sq_query_node_set_raw(node, logi_args, arg_list);
	else
		node->children = sq_query_condition(query, node, logi_args, arg_list);
	va_end(arg_list);
}

void sq_query_where_exists_logical(SqQuery *query, unsigned int logi_args)
{
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_args);

	node->children = sq_query_node_new(query);
	node = node->children;
	node->str = (char*)SQN_EXISTS;

	sq_query_push_nested(query, node);
}

void sq_query_where_between_logical(SqQuery *query, const char *column_name, unsigned int logi_args, const char* format, ...)
{
	va_list        arg_list;
	va_list        arg_copy;
	char          *str;
	size_t         length;
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

	temp.node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_args & (~SQ_QUERYLOGI_NOT));
	temp.node->children = sq_query_column_in(query, column_name, logi_args, str);
}

void sq_query_where_in_logical(SqQuery *query, const char *column_name, unsigned int logi_args, int n_args, const char* format, ...)
{
	va_list        arg_list;
	va_list        arg_copy;
	char          *str;
	size_t         length;
	size_t         format_len;
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

	temp.node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_args & (~SQ_QUERYLOGI_NOT));
	temp.node->children = sq_query_column_in(query, column_name, logi_args, str);
}

void sq_query_where_null_logical(SqQuery *query, const char *column_name, unsigned int logi_args)
{
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, NULL, SQN_WHERE, logi_args & (~SQ_QUERYLOGI_NOT));
	node->children = sq_query_column_in(query, column_name, 0, NULL);
	// 'node' pointer node of column_name
	node = node->children;
	// 'node' pointer node of value
	node = node->children;

	if (logi_args & SQ_QUERYLOGI_NOT)
		node->str = strdup("IS NOT NULL");
	else
		node->str = strdup("IS NULL");
}

void sq_query_join_full(SqQuery *query, unsigned int join_args, const char *table, ...)
{
	// for SQ_QUERYJOIN_X series
	const uint8_t nodeStrJoinPos[] = {
		SQN_JOIN_POS,          // SQ_QUERYJOIN_INNER maps to SQN_JOIN
		SQN_LEFT_JOIN_POS,     // SQ_QUERYJOIN_LEFT
		SQN_RIGHT_JOIN_POS,    // SQ_QUERYJOIN_RIGHT
		SQN_FULL_JOIN_POS,     // SQ_QUERYJOIN_FULL
		SQN_CROSS_JOIN_POS,    // SQ_QUERYJOIN_CROSS
	};
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
	joinon->str = (char*)STR_GROUP + nodeStrJoinPos[join_args & ~SQ_QUERYARGS_MASK];
	nested->joinon = joinon;
	// insert table node.  If table is NULL, it has call sq_query_push_nested()
	sq_query_insert_table_node(query, joinon, table);
	if (table == NULL)
		return;

	// ON
	if ((join_args & ~SQ_QUERYARGS_MASK) != SQ_QUERYJOIN_CROSS) {
		node = sq_query_clause_logical(query, joinon, SQN_ON, SQ_QUERYLOGI_AND);
		va_start(arg_list, table);
		node->children = sq_query_condition(query, node, join_args, arg_list);
		va_end(arg_list);
	}
}

void sq_query_on_logical(SqQuery *query, unsigned int logi_args, ...)
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
	temp.node = sq_query_clause_logical(query, temp.joinon, SQN_ON, logi_args);

	va_start(arg_list, logi_args);
	if (logi_args & SQ_QUERYARGS_RAW)
		sq_query_node_set_raw(temp.node, logi_args, arg_list);
	else
		temp.node->children = sq_query_condition(query, temp.node, logi_args, arg_list);
	va_end(arg_list);
}

void sq_query_group_by_list(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *groupby;
	SqQueryNode   *node;

	groupby = sq_query_node_find(nested->parent, SQN_GROUP_BY, &node);
	if (groupby == NULL) {
		groupby = sq_query_node_insert(nested->parent, node, sq_query_node_new(query));
		groupby->str = (char*)SQN_GROUP_BY;
	}

	va_start(arg_list, query);
	sq_query_insert_column_list(query, groupby, arg_list);
	va_end(arg_list);
	// for sq_query_as()
	nested->aliasable = NULL;
}

void sq_query_having_logical(SqQuery *query, unsigned int logi_args, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node;

	node = sq_query_clause_logical(query, nested->parent, SQN_HAVING, logi_args);

	va_start(arg_list, logi_args);
	if (logi_args & SQ_QUERYARGS_RAW)
		sq_query_node_set_raw(node, logi_args, arg_list);
	else
		node->children = sq_query_condition(query, node, logi_args, arg_list);
	va_end(arg_list);
}

void sq_query_order_by_list(SqQuery *query, ...)
{
	va_list        arg_list;
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *orderby;
	SqQueryNode   *node;

	orderby = sq_query_node_find(nested->parent, SQN_ORDER_BY, &node);
	if (orderby == NULL) {
		orderby = sq_query_node_insert(nested->parent, node, sq_query_node_new(query));
		orderby->str = (char*)SQN_ORDER_BY;
	}

	va_start(arg_list, query);
	sq_query_insert_column_list(query, orderby, arg_list);
	va_end(arg_list);
	// for sq_query_as()
	nested->aliasable = NULL;
}

void sq_query_order_sorted(SqQuery *query, unsigned int sort_type)
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
		node->children->str = (char*) ((sort_type == 0) ? SQN_ASC : SQN_DESC);
	}
}

void *sq_query_union(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *union_node;

	union_node = sq_query_node_append(nested->parent, sq_query_node_new(query));
	union_node->str = (char*)SQN_UNION;

	sq_query_push_nested(query, union_node);
	return union_node;
}

void sq_query_union_all(SqQuery *query)
{
	((SqQueryNode*)sq_query_union(query))->str = (char*)SQN_UNION_ALL;
}

void *sq_query_limit(SqQuery *query, int64_t count)
{
	SqQueryNested *nested = query->nested_cur;
	union {
		SqQueryNode *node;
		SqQueryNode *child;    // limit.child->str  will be set to SQL LIMIT count
	} limit;
	union {
		SqQueryNode *node;
		int          len;
	} temp;

	limit.node = sq_query_node_find(nested->parent, SQN_LIMIT, &temp.node);
	if (limit.node) {
		limit.child = limit.node->children;
		free(limit.child->str);
	}
	else {
		limit.node = sq_query_node_insert(nested->parent, temp.node, sq_query_node_new(query));
		limit.node->str = (char*)SQN_LIMIT;
		limit.node->children = sq_query_node_new(query);
		limit.child = limit.node->children;
	}
	temp.len = snprintf(NULL, 0, "%"PRId64, count) + 1;
	limit.child->str = malloc(temp.len);
	snprintf(limit.child->str, temp.len, "%"PRId64, count);

	return limit.child;
}

void sq_query_offset(SqQuery *query, int64_t index)
{
	SqQueryNested *nested = query->nested_cur;
	union {
		SqQueryNode *node;
		SqQueryNode *child;    // limit.child->str  will be set to SQL LIMIT count
		int          len;
	} limit;
	union {
		SqQueryNode *node;
		SqQueryNode *child;    // offset.child->str will be set to SQL OFFSET index
	} offset;

	limit.node = sq_query_node_find(nested->parent, SQN_LIMIT, NULL);
	if (limit.node == NULL)
		limit.child = sq_query_limit(query, 1);
	else
		limit.child = limit.node->children;

	if (limit.child->children) {
		offset.node  = limit.child->children;
		offset.child = offset.node->children;
		free(offset.child->str);
	}
	else {
		limit.child->children = sq_query_node_new(query);
		offset.node = limit.child->children;
		offset.node->str = (char*)SQN_OFFSET;
		offset.node->children = sq_query_node_new(query);
		offset.child = offset.node->children;
	}

	limit.len = snprintf(NULL, 0, "%"PRId64, index) + 1;
	offset.child->str = malloc(limit.len);
	snprintf(offset.child->str, limit.len, "%"PRId64, index);
}

void sq_query_delete(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->command;

	if (node == NULL) {
		node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		node->str = (char*)SQN_DELETE;
		nested->command = node;
	}
}

void sq_query_truncate(SqQuery *query)
{
	SqQueryNested *nested = query->nested_cur;
	SqQueryNode   *node = nested->command;

	if (node == NULL) {
		node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
		node->str = (char*)SQN_TRUNCATE_TABLE;
		nested->command = node;
	}
	node = nested->name;
	if (node && node->str == SQN_FROM)
		node->str = (char*)SQN_NONE;
}

int  sq_query_get_command(SqQuery *query)
{
#define SQN_N_CMDS    ( sizeof(nodeStrCmdPos) / sizeof(nodeStrCmdPos[0]) )
	// for SQ_QUERY_CMD_X series
	const uint8_t nodeStrCmdPos[] = {
		SQN_NONE_POS,            // SQ_QUERY_CMD_NONE         maps to SQN_NONE
		SQN_CREATE_TABLE_POS,    // SQ_QUERY_CMD_CREATE_TABLE maps to SQN_CREATE_TABLE
		SQN_ALERT_TABLE_POS,     // SQ_QUERY_CMD_ALERT_TABLE
		SQN_DROP_TABLE_POS,      // SQ_QUERY_CMD_DROP_TABLE
		SQN_TRUNCATE_TABLE_POS,  // SQ_QUERY_CMD_TRUNCATE_TABLE
		SQN_INSERT_INTO_POS,     // SQ_QUERY_CMD_INSERT_INTO
		SQN_UPDATE_POS,          // SQ_QUERY_CMD_UPDATE
		SQN_DELETE_POS,          // SQ_QUERY_CMD_DELETE
		SQN_SELECT_POS,          // SQ_QUERY_CMD_SELECT
	};
	SqQueryNode   *command = query->nested_cur->command;

	// index of nodeStrCmdPos is the same as index of SQ_QUERY_CMD_X series
	if (command) {
		for (int i = 0;  i < SQN_N_CMDS;  i++)
			if (command->str == STR_GROUP + nodeStrCmdPos[i])
				return i;
	}
	return SQ_QUERY_CMD_NONE;
}

// ------------------------------------
// sq_query_to_sql()

static void node_to_buf(SqQueryNode *node, SqQuery *query)
{
	char   *src;
	char   *dest;
	size_t  value_len;

	for (;  node;  node = node->next) {
		// no space before comma
		if (node->str == SQN_COMMA)
			query->length--;
		src = node->str;
		if (*src) {
			value_len = strlen(src) + 1;    // + " "
			if (query->length +value_len >= query->allocated) {
				query->allocated *= 2;
				query->str = realloc(query->str, query->allocated);
			}
			dest = query->str + query->length;
			query->length += (uint32_t)value_len;
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
	// query has table name
	if (temp.nested->name) {
		// query has table name but no command, it must add 'SELECT *'.
		if (temp.nested->command == NULL)
			sq_query_select_list(query, "*", NULL);
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

// ----------------------------------------------------------------------------
// static functions for above functions

// used by SELECT, GROUP BY, ORDER BY
static void sq_query_insert_column_list(SqQuery *query, SqQueryNode *node, va_list arg_list)
{
	const char   *name;
	SqQueryNode  *sub_node;

	// get last column
	sub_node = sq_query_node_last(node->children);
	if (sub_node == NULL) {
		sub_node = sq_query_node_new(query);
		sub_node->str = (char*)SQN_NONE;
		node->children = sub_node;
	}
	node = sub_node;

	while ( (name = va_arg(arg_list, const char*)) ) {
		// add ',' if user specify multiple column
		// if current 'node' was not just created.
		if (node->children != NULL) {
			sub_node = sq_query_node_new(query);
			sub_node->str = (char*)SQN_COMMA;
			node->next = sub_node;
			node = sub_node;
		}
		// add column name in children node.
		sub_node = sq_query_node_new(query);
		sub_node->str = strdup(name);
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
	node->str = (char*)SQN_NONE;
	query->nested_cur->aliasable = node;    // for sq_query_as()

	// table name
	if (table_name == NULL)
		sq_query_push_nested(query, node);
	else {
		node->children = sq_query_node_new(query);
		node = node->children;
		node->str = strdup(table_name);
	}
}

// used by BETWEEN and IN
static SqQueryNode *sq_query_column_in(SqQuery *query, const char *column_name, unsigned int logi_args, char *value_str)
{
	SqQueryNode   *column_node;
	SqQueryNode   *node;

	// column node
	column_node = sq_query_node_new(query);
	column_node->str = strdup(column_name);
	node = column_node;
	// NOT node
	if (logi_args & SQ_QUERYLOGI_NOT) {
		node->children = sq_query_node_new(query);
		node = node->children;
		node->str = (char*)SQN_NOT;
	}
	// BETWEEN (or IN) value node
	node->children = sq_query_node_new(query);
	node = node->children;
	node->str = value_str;

	return column_node;
}

static SqQueryNode *sq_query_condition(SqQuery *query, SqQueryNode *node, unsigned int logi_args, va_list arg_list)
{
	va_list   arg_copy;
	char     *args[3];
	union {
		size_t    length;
		char     *dest;
	} mem;
	union {
		size_t    length;
		int       index;
		char     *cur;
	} temp;

	// handle C++  special case: get number of arguments in arg_list
	logi_args = SQ_QUERYARGS_N_GET(logi_args);

	// ====== args[0] is 1st argument in arg_list ======
	args[0] = va_arg(arg_list, char*);
	// It use subquery if 1st argument is NULL.
	if (args[0] == NULL) {
		sq_query_push_nested(query, node);
		return NULL;
	}
	// count length of argv[0]
	mem.length = strlen(args[0]) + 1;              // + ' '
	// create node for condition
	node = sq_query_node_new(query);
	// ====== args[1] is 2nd argument in arg_list ======
	args[1] = va_arg(arg_list, char*);
	if (args[1]) {
		// search % sign if args[1] is printf format string
		for (temp.cur = args[1];  *temp.cur;  temp.cur++)
			if (*temp.cur == '%')
				break;
	}
	// if args[1] is NULL, printf format string, or raw string (C++ special case)
	if (args[1] == NULL || *temp.cur == '%' || logi_args == 2) {
		args[2] = args[1];    // move 2nd argument to 3rd argument
		args[1] = "=";        // set  2nd argument to equal operator
		mem.length += 2;      // + '=' + ' '
	}
	// ====== args[2] is 3rd argument in arg_list ======
	else {
		args[2] = va_arg(arg_list, char*);
		// count length of 2nd argument in arg_list if '%' sign not found
		mem.length += (int)(temp.cur - args[1]) + 1;      // + ' '
	}

	// It is printf format string if 3rd argument in arg_list is not NULL.
	if (args[2]) {
		// handle C++  special case: 2nd or 3rd argument in arg_list is raw string
		// 2 arguments special case: "column", "valueStr"         or   "column", "strHas%sign"
		// 3 arguments special case: "column", ">", "valueStr"    or   "column", ">", "strHas%sign"
		if (logi_args == 2 || (logi_args == 3 && *temp.cur != '%')) {
			temp.length = strlen(args[2]) +1;
			node->str = malloc(mem.length + temp.length);
			strncpy(node->str + mem.length, args[2], temp.length);
		}
		else {
			va_copy(arg_copy, arg_list);
#ifdef _MSC_VER		// for MS C only
			temp.length = _vscprintf(args[2], arg_copy) +1;
#else				// for C99 standard
			temp.length = vsnprintf(NULL, 0, args[2], arg_copy) +1;
#endif
			va_end(arg_copy);
			node->str = malloc(mem.length + temp.length);
			vsnprintf(node->str + mem.length, temp.length, args[2], arg_list);
		}
	}
	// It is subquery if 3rd argument in arg_list is NULL.
	else {
		node->str = malloc(mem.length);
		sq_query_push_nested(query, node);
	}

	mem.dest = node->str;
	for (temp.index = 0;  temp.index < 2;  temp.index++) {
		while (*args[temp.index])
			*mem.dest++ = *args[temp.index]++;
		*mem.dest++ = ' ';
	}
	if (args[2] == NULL)    // Null-terminated
		*(mem.dest-1) = 0;
	return node;
}

// ----------------------------------------------------------------------------
// If you set SQ_QUERY_USE_ALONE to 1, you can use SqQuery alone.
#if SQ_QUERY_USE_ALONE == 0

// parent->str must be SQN_FROM or SQN_JOIN
static const char *get_table(SqQueryNode *parent)
{
	SqQueryNode  *child;

	// parent pointer to NONE node
	parent = parent->children;
	// child can be table name or nested query
	child = parent->children;
	if (NOT_CONST_STR(child->str))
		return child->str;    // return table name
	else {
		// nested query
		child = sq_query_node_find(parent, SQN_FROM, NULL);
		if (child)
			return get_table(child);
	}
	return NULL;
}

// array[0] = table1_name, array[1] = table1_as_name,
// array[2] = table2_name, array[3] = table2_as_name, etc.
int  sq_query_get_table_as_names(SqQuery *query, SqPtrArray *table_and_as_names)
{
	SqQueryNode  *qnode;
	SqQueryNode  *child;
	const char   *table_name;

	// FROM table1_name AS table1_as_name
	// JOIN table2_name AS table2_as_name
	for (qnode = query->root.children;  qnode;  qnode = qnode->next) {
		if (qnode->str != SQN_FROM) {
			if (qnode->str < SQN_JOIN || qnode->str > SQN_CROSS_JOIN)
				continue;
		}
		// table name
		table_name = get_table(qnode);
		if (table_name == NULL)
			return 0;
		sq_ptr_array_push(table_and_as_names, table_name);

		// AS name
		child = sq_query_node_find(qnode, SQN_AS, NULL);
		if (child && child->children)
			sq_ptr_array_push(table_and_as_names, child->children->str);
		else
			sq_ptr_array_push(table_and_as_names, NULL);
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

#if SQ_CONFIG_QUERY_ONLY_COLUMN
	if (table->type->bit_field & SQB_TYPE_QUERY_FIRST) {
		for (unsigned int index = 0;  index < type->n_entry;  index++) {
			column = (SqColumn*)type->entry[index];
			if (column->bit_field & SQB_COLUMN_QUERY) {
				buf_len = snprintf(NULL, 0, "%s AS %c%s.%s%c",
						column->name,
						quotes[0], table_as_name, column->name, quotes[1]) + 1;
				buffer = realloc(buffer, buf_len);
				snprintf(buffer, buf_len, "%s AS %c%s.%s%c",
						column->name,
						quotes[0], table_as_name, column->name, quotes[1]);
				sq_query_select(query, buffer);
			}
		}
	}
#endif

	for (unsigned int index = 0;  index < type->n_entry;  index++) {
		column = (SqColumn*)type->entry[index];
#if SQ_CONFIG_QUERY_ONLY_COLUMN
		if (column->bit_field & SQB_COLUMN_QUERY)
			continue;
#endif
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
		sq_query_select(query, buffer);
	}
	free(buffer);
}

#endif  // SQ_QUERY_USE_ALONE

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
			// subquery has table name
			if (nested->name) {
				// subquery has table name but no command, it must add 'SELECT *'.
				if (nested->command == NULL)
					sq_query_select_list(query, "*", NULL);
			}
			else if (node->str >= SQN_WHERE)
				node->str = (char*)SQN_NONE;

			if (nested->parent->str != SQN_UNION && nested->parent->str != SQN_UNION_ALL) {
				// insert '(' to beginning of list
				node = sq_query_node_prepend(nested->parent, sq_query_node_new(query));
				node->str = (char*)SQN_BRACKETS_L;
				// append ')' to ending of list
				node = sq_query_node_append(nested->parent, sq_query_node_new(query));
				node->str = (char*)SQN_BRACKETS_R;
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

//	node->str = NULL;
	node->next = NULL;
	node->children = NULL;

	return node;
}

static void sq_query_node_free(SqQueryNode *node, SqQuery *query)
{
	SqQueryNode  *next;

	for (;  node;  node = next) {
		if (NOT_CONST_STR(node->str))
			free(node->str);
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

// insert_pos: 'constString' must insert in this position
static SqQueryNode *sq_query_node_find(SqQueryNode *node, const char *constString, SqQueryNode **insert_pos)
{
	SqQueryNode *prev = NULL;

	for (node = node->children;  node;  node = node->next) {
		// STR_GROUP pointer to 'constStringGroup'
		// if node->str is not const string or is symbol const string
		if (node->str < STR_GROUP || node->str >= STR_GROUP + SQN_SYMBOL_POS)
			continue;

		if (node->str == constString)
			break;
		else if (node->str > constString) {
			node = NULL;
			break;
		}
		prev = node;
	}

	// if 'constString' not found, 'constString' must insert in this position
	if (insert_pos)
		insert_pos[0] = prev;
	return node;
}
