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

/*
	SqField
	|
	`--- SqSchema
 */

#ifndef SQ_SCHEMA_H
#define SQ_SCHEMA_H

#ifdef __cplusplus
#include <typeinfo>
#endif

#include <stddef.h>
#include <stdlib.h>

#include <SqPtrArray.h>
#include <SqTable.h>

/*	DataBase object - [Server Name].[DataBase Name].[Schema].[Table Name]
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqSchema       SqSchema;

// SqSchema* sq_schema_new(const char* name);
#define sq_schema_new(name)    \
		sq_schema_init(malloc(sizeof(SqSchema)), name)

// void sq_schema_free(SqSchema* schema);
#define sq_schema_free(schema)  \
		free(sq_schema_final(schema))

SqSchema* sq_schema_init(SqSchema* schema, const char* name);
SqSchema* sq_schema_final(SqSchema* schema);

/*	sq_schema_create_full()
  if "type_info" == NULL, program will use "type_name" and "type_size" to create "type_info".
  You can pass 0 to "type_size" because program calculate type_size automatically.

  C Language
  #define SQ_HAVE_NAMING_CONVENTION to enable "table_name" <-> "type_name" converting,
  program only need one of table_name, type_name, or type_info->name.

  C++ Language
  It doesn't need #define SQ_HAVE_NAMING_CONVENTION because program
  use typeid(Type).name() to get "type_name".
 */
SqTable* sq_schema_create_full(SqSchema* schema,
                               const char* table_name,
                               const SqType* type_info,
                               const char* type_name,
                               size_t instance_size);

#define sq_schema_create_by_type(schema, table_name, type_info)  \
		sq_schema_create_full(schema, table_name, type_info, NULL, 0)

#define sq_schema_create(schema, table_name, StructType)  \
		sq_schema_create_full(schema, table_name, NULL, SQ_GET_TYPE_NAME(StructType), sizeof(StructType))

#ifdef SQ_HAVE_NAMING_CONVENTION
#define sq_schema_create_as(schema, StructType)  \
		sq_schema_create_full(schema, NULL, NULL, SQ_GET_TYPE_NAME(StructType), sizeof(StructType))
#endif

SqTable* sq_schema_alter(SqSchema* schema, const char* table_name, const SqType* type_info);
void     sq_schema_drop(SqSchema* schema, const char* name);
void     sq_schema_rename(SqSchema* schema, const char* from, const char* to);

SqTable* sq_schema_find(SqSchema* schema, const char* table_name);
SqTable* sq_schema_find_type(SqSchema* schema, const char* type_name);

// This used by migration. It may steal tables from schema_src
int     sq_schema_accumulate(SqSchema* schema_cur, SqSchema* schema_src);

#ifdef __cplusplus
}  // extern "C"
#endif

struct SqSchema
{
	SQ_FIELD_MEMBERS;
/*	// ------ SqField members ------
	SqType*      type;     // type information for this field
	char*        name;
	size_t       offset;
	unsigned int bit_field;
 */

	// table_types is sorted by SqTable.type.name
	SqPtrArray   table_types;
	bool         table_types_sorted;

	int          version;

#ifdef __cplusplus
	// C++11 standard-layout
	// ----------------------------------------------------

	void* operator new(size_t size)
		{ return malloc(size); }
	void operator delete(void* instance)
		{ free(instance); }
	SqSchema(const char* name)
		{ sq_schema_init(this, name); }
	~SqSchema()
		{ sq_schema_final(this); }

	SqTable* create(const char* name,
	                const SqType* type_info,
	                const char* type_name = NULL,
	                size_t instance_size = 0)
		{ return sq_schema_create_full(this, name, type_info, type_name, instance_size); }

	template <class StructType>
	SqTable* create(const char* name)
		{ return sq_schema_create_full(this, name, NULL, typeid(StructType).name(), sizeof(StructType)); }

	SqTable* alter(const char* name, const SqType* type_info = NULL)
		{ return sq_schema_alter(this, name, type_info); }
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
