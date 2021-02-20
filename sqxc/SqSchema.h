/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#ifndef SQ_SCHEMA_H
#define SQ_SCHEMA_H

#ifdef __cplusplus
#include <typeinfo>
#endif

#include <stddef.h>
#include <stdlib.h>

#include <SqConfig.h>    // SQ_CONFIG_NAMING_CONVENTION
#include <SqPtrArray.h>
#include <SqTable.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqSchema       SqSchema;

// SqSchema::bit_field for internal use only
#define SQB_SCHEMA_INCLUDED                (1 << 15)

// ----------------------------------------------------------------------------
// C functions

SqSchema* sq_schema_new(const char* name);
void      sq_schema_free(SqSchema* schema);

void    sq_schema_init(SqSchema* schema, const char* name);
void    sq_schema_final(SqSchema* schema);

/*	sq_schema_create_full()
  if 'type_info' == NULL, program will use 'type_name' and 'instance_size' to create 'type_info'.
  You can pass 0 to 'instance_size' because program calculate 'instance_size' automatically.

  C Language
  #define SQ_CONFIG_NAMING_CONVENTION to enable "SQL_table_name" <-> "type_name" converting,
  program only need one of table_name, type_name, or type_info->name.

  C++ Language
  It doesn't need #define SQ_CONFIG_NAMING_CONVENTION because program
  use typeid(Type).name() to get "type_name".
 */
SqTable* sq_schema_create_full(SqSchema* schema,
                               const char* table_name,
                               const char* type_name,
                               const SqType* type_info,
                               size_t instance_size);

#define sq_schema_create_by_type(schema, table_name, type_info)  \
		sq_schema_create_full(schema, table_name, NULL, type_info, 0)

#define sq_schema_create(schema, table_name, StructType)  \
		sq_schema_create_full(schema, table_name, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType))

#ifdef SQ_CONFIG_NAMING_CONVENTION
#define sq_schema_create_as(schema, StructType)  \
		sq_schema_create_full(schema, NULL, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType))
#endif

SqTable* sq_schema_create_from_columns(SqSchema* schema,
                                       const char* table_name,
                                       const char* type_name,
                                       const SqColumn **columns,
                                       int n_columns);

SqTable* sq_schema_alter_from_columns(SqSchema* schema,
                                      const char* table_name,
                                      const SqColumn **columns,
                                      int n_columns);

SqTable* sq_schema_alter(SqSchema* schema, const char* table_name, const SqType* type_info);
void     sq_schema_drop(SqSchema* schema, const char* name);
void     sq_schema_rename(SqSchema* schema, const char* from, const char* to);

SqTable* sq_schema_find(SqSchema* schema, const char* table_name);

/* --------------------------------------------------------
	migration functions

	// --- if database_schema_version is 3, get current schema in database
	sq_schema_include(schema, schema_v2);
	sq_schema_include(schema, schema_v3);
	sq_schema_trace_foreign(schema);
	sq_schema_erase_records(schema, '=');

	// --- if the latest schema_version is 5, migrate to schema_v5
	// migrate schema_v4 and schema_v5 by SQL statement
	sq_schema_include(schema, schema_v4);
	sq_schema_include(schema, schema_v5);
	sq_schema_trace_foreign(schema);
	// === SQLite must rename and drop table here
	sq_schema_erase_records(schema, '<');
	// === SQLite must try to recreate or create table here

	// Other SQL product may need this
	sq_schema_arrange(schema, entries);
	// create table by SQL statement here

	// free temporary data after migration.
	sq_schema_complete(schema);
 */

// This used by migration: include and apply changes from 'schema_src'.
// It may move/steal tables and column from 'schema_src'.
int     sq_schema_include(SqSchema* schema, SqSchema* schema_src);

// It trace renamed (or dropped) table/column that was referenced by others and update others references.
// use this function after calling sq_schema_include()
int     sq_schema_trace_foreign(SqSchema* schema);

/* erase renamed & dropped records after calling sq_schema_include() and sq_schema_trace_foreign()
   if database schema version <  current schema version, pass 'version_comparison' = '<'
   if database schema version == current schema version, pass 'version_comparison' = '='

   To pass '<' to 'version_comparison':
   1. If you don't need calling sq_schema_trace_foreign() any more
   2. before you call sq_schema_arrange()

   If you pass '<' to 'version_comparison' , it will affect performance of sq_schema_trace_foreign().
   return number of old tables after erasing.
 */
int     sq_schema_erase_records(SqSchema* schema, char version_comparison);
int     sq_schema_erase_records_of_table(SqSchema* schema, char version_comparison);

/* call this function before creating SQL table after sq_schema_erase_records(schema, '<')
   if table has no foreign key, this function move it to front.
   if table references most tables, this function move it to end.
   if table references each other, table->extra->foreigns.length > 0
   output arranged tables in 'entries'
 */
void    sq_schema_arrange(SqSchema* schema, SqPtrArray* entries);

// call this function after creating/altering SQL table.
// It will free temporary data (e.g. table->foreigns)
void    sq_schema_complete(SqSchema* schema);

#ifdef __cplusplus
}  // extern "C"
#endif

/* ----------------------------------------------------------------------------
	SqSchema

	SqEntry
	|
	`--- SqSchema
 */

struct SqSchema
{
	SQ_ENTRY_MEMBERS;
/*	// ------ SqEntry members ------
	SqType*      type;        // type information for this entry
	const char*  name;
	size_t       offset;      // sq_schema_trace_foreign() and migration use this
	unsigned int bit_field;
 */

	// ------ SqSchema members ------
	int          version;

#ifdef __cplusplus
	// C++11 standard-layout
	// ----------------------------------------------------
/*
	void* operator new(size_t size)
		{ return malloc(size); }
	void operator delete(void* instance)
		{ free(instance); }
 */
	SqSchema(const char* name = NULL) {
		sq_schema_init(this, name);
	}
	~SqSchema() {
		sq_schema_final(this);
	}

	SqTable* create(const char* table_name,
	                const char* type_name,
	                const SqType* type_info = NULL,
	                size_t instance_size = 0)
		{ return sq_schema_create_full(this, table_name, type_name, type_info, instance_size); }

	SqTable* create(const char* table_name, const SqType* type_info)
		{ return sq_schema_create_full(this, table_name, NULL, type_info, 0); }

	template <class StructType>
	SqTable* create(const char* name)
		{ return sq_schema_create_full(this, name, typeid(StructType).name(), NULL, sizeof(StructType)); }

	SqTable* create(const char* table_name, const char* type_name, const SqColumn** columns, int n_columns) {
		return sq_schema_create_from_columns(this, table_name, type_name, columns, n_columns);
	}
	template <class StructType>
	SqTable* create(const char* table_name, const SqColumn** columns, int n_columns) {
		return sq_schema_create_from_columns(this, table_name, typeid(StructType).name(), columns, n_columns);
	}

	SqTable* alter(const char* name, const SqColumn** columns, int n_columns) {
		return sq_schema_alter_from_columns(this, name, columns, n_columns);
	}

	SqTable* alter(const char* name, const SqType* type_info = NULL)
		{ return sq_schema_alter(this, name, type_info); }
	void  drop(const char* name)
		{ sq_schema_drop(this, name); }
	void  rename(const char* from, const char* to)
		{ sq_schema_rename(this, from, to); }

	SqTable* find(const char* table_name)
		{ return sq_schema_find(this, table_name); }

	int   include(SqSchema* schema_src)
		{ return sq_schema_include(this, schema_src); }
	int   traceForeign(void)
		{ return sq_schema_trace_foreign(this); }
	void  clearRecords(char version_comparison)
		{ sq_schema_erase_records(this, version_comparison); }
	void  clearRecordsOfTable(char version_comparison)
		{ sq_schema_erase_records_of_table(this, version_comparison); }
	void  arrange(SqPtrArray* entries)
		{ sq_schema_arrange(this, entries); }
	void  complete(void)
		{ sq_schema_complete(this); }
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ namespace

#ifdef __cplusplus

namespace Sq
{

// These are for directly use only. You can NOT derived it.
typedef struct SqSchema     Schema;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_SCHEMA_H
