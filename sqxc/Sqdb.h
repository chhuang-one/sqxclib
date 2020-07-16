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

#ifndef SQDB_H
#define SQDB_H

//#include <SqPtrArray.h>
#include <SqSchema.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Sqdb             Sqdb;
typedef struct SqdbInfo         SqdbInfo;
typedef struct SqdbConfig       SqdbConfig;

typedef bool (*SqdbTableFunc)(Sqdb* db, SqTable* table);
typedef bool (*SqdbExecFunc)(Sqdb* db, const char* sql, void** error);

struct Sqdb {
	SqdbInfo*      info;

};

struct SqdbInfo {
	struct {
		unsigned int mysql:1;
		unsigned int sqlite:1;

		unsigned int can_alter:1;        // ALTER COLUMN
		unsigned int can_modify:1;       // MODIFY COLUMN
	} column;

	// schema
	SqdbTableFunc    create;
	SqdbTableFunc    drop;
	SqdbTableFunc    alter;

	// SQL
	SqdbExecFunc     exec;

	// Connect
};

void sqdb_info_init_sqlite(SqdbInfo* dbinfo);
void sqdb_info_init_mysql(SqdbInfo* dbinfo);
void sqdb_table_to_sql(Sqdb* db, SqTable* table, SqBuffer* buffer);
void sqdb_schema_to_sql(Sqdb* db, SqSchema* schema, SqBuffer* buffer);

void sqdb_create_table(Sqdb* db, SqTable* table);
void sqdb_rename_table(Sqdb* db, SqTable* table);
void sqdb_drop_table(Sqdb* db, SqTable* table);
void sqdb_alter_table(Sqdb* db, SqTable* table);
void sqdb_exec(Sqdb* db, const char* sql, void** error);

void sqdb_open();
void sqdb_connect();
void sqdb_close();


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQDB_H
