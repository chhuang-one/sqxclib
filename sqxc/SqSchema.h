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

#ifndef SQ_SCHEMA_H
#define SQ_SCHEMA_H

#ifdef __cplusplus
#include <typeinfo>
#endif

#include <stddef.h>
#include <stdlib.h>

#include <SqConfig.h>    // SQ_CONFIG_NAMING_CONVENTION
#include <SqPtrArray.h>
#include <SqRelation.h>
#include <SqTable.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqSchema       SqSchema;

// SqSchema::bit_field
// Can't steal column from this schema. It is not database no-steal
#define SQB_SCHEMA_NO_STEAL                (1 << 15)

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

SqSchema *sq_schema_new(const char *name);
void      sq_schema_free(SqSchema *schema);

void    sq_schema_init(SqSchema *schema, const char *name);
void    sq_schema_final(SqSchema *schema);

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
SqTable *sq_schema_create_full(SqSchema *schema,
                               const char *table_name,
                               const char *type_name,
                               const SqType *type_info,
                               size_t instance_size);

#define sq_schema_create_by_type(schema, table_name, type_info)  \
		sq_schema_create_full(schema, table_name, NULL, type_info, 0)

#define sq_schema_create(schema, table_name, StructType)  \
		sq_schema_create_full(schema, table_name, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType))

#ifdef SQ_CONFIG_NAMING_CONVENTION
#define sq_schema_create_as(schema, StructType)  \
		sq_schema_create_full(schema, NULL, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType))
#endif

SqTable *sq_schema_alter(SqSchema *schema, const char *table_name, const SqType *type_info);
void     sq_schema_drop(SqSchema *schema, const char *name);
void     sq_schema_rename(SqSchema *schema, const char *from, const char *to);

SqTable *sq_schema_find(SqSchema *schema, const char *table_name);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
	SqSchema

	SqEntry
	|
	`--- SqSchema
 */

struct SqSchema
{
	SQ_ENTRY_MEMBERS;
/*	// ------ SqEntry members ------
	const SqType *type;        // type information for this entry
	const char   *name;
	size_t        offset;      // migration use this. Number of tables have existed in database
	unsigned int  bit_field;
 */

	// ------ SqSchema members ------
	SqRelationPool *relation_pool;    // relation pool for (SQLite) migration
	SqRelation     *relation;         // relation of tables

	int             version;
#ifdef __cplusplus
	// C++11 standard-layout
	// ----------------------------------------------------
/*
	void *operator new(size_t size) {
		return malloc(size);
	}
	void operator delete(void *instance) {
		free(instance);
	}
 */
	SqSchema(const char *name = NULL) {
		sq_schema_init(this, name);
	}
	~SqSchema() {
		sq_schema_final(this);
	}

	SqTable *create(const char *table_name,
	                const char *type_name,
	                const SqType *type_info = NULL,
	                size_t instance_size = 0) {
		return sq_schema_create_full(this, table_name, type_name, type_info, instance_size);
	}

	SqTable *create(const char *table_name, const SqType *type_info) {
		return sq_schema_create_full(this, table_name, NULL, type_info, 0);
	}

	template <class StructType>
	SqTable *create(const char *name) {
		return sq_schema_create_full(this, name, typeid(StructType).name(), NULL, sizeof(StructType));
	}

	SqTable *alter(const char *name, const SqType *type_info = NULL) {
		return sq_schema_alter(this, name, type_info);
	}
	void  drop(const char *name) {
		sq_schema_drop(this, name);
	}
	void  rename(const char *from, const char *to) {
		sq_schema_rename(this, from, to);
	}

	SqTable *find(const char *table_name) {
		return sq_schema_find(this, table_name);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq
{

// These are for directly use only. You can NOT derived it.
typedef struct SqSchema     Schema;

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_SCHEMA_H
