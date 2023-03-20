/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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

#include <SqPtrArray.h>
#include <SqRelation.h>
#include <SqTable.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqSchema       SqSchema;

// SQL common bit_field
#define SQB_SCHEMA_CHANGED                 SQB_CHANGED         // SQL: column or table has been changed.

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
  If 'table_type' == NULL, program will use 'type_name' and 'instance_size' to create 'table_type'.
  If 'table_type' is dynamic SqType, it will be freed when program free table.
  You can pass 0 to 'instance_size' because program calculate 'instance_size' automatically.

  You can use sq_str_table_name() and sq_str_type_name() in sqxcsupport if you want to
  convert table name (snake case) and type name (upper camel case) at run time.

  C++ program can use typeid(Type).name() to get "type_name".
 */
SqTable *sq_schema_create_full(SqSchema     *schema,
                               const char   *table_name,
                               const char   *type_name,
                               const SqType *table_type,
                               size_t        instance_size);

// SqTable *sq_schema_create_by_type(SqSchema *schema, const char *table_name, const SqType *table_type);
#define sq_schema_create_by_type(schema, table_name, table_type)  \
		sq_schema_create_full(schema, table_name, NULL, table_type, 0)

// SqTable *sq_schema_create(SqSchema *schema, const char *table_name, StructType);
#define sq_schema_create(schema, table_name, StructType)  \
		sq_schema_create_full(schema, table_name, SQ_GET_TYPE_NAME(StructType), NULL, sizeof(StructType))

SqTable *sq_schema_alter(SqSchema *schema, const char *table_name, const SqType *table_type);
void     sq_schema_drop(SqSchema *schema, const char *name);
void     sq_schema_rename(SqSchema *schema, const char *from, const char *to);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	SchemaMethod is used by SqSchema and it's children.

	It's derived struct/class must be C++11 standard-layout and has SqSchema members.
 */
struct SchemaMethod
{
	Sq::Table *create(const char   *table_name,
	                  const char   *type_name,
	                  const SqType *table_type = NULL,
	                  size_t        instance_size = 0);

	Sq::Table *create(const char           *table_name,
	                  const char           *type_name,
	                  const Sq::TypeMethod *table_type,
	                  size_t                instance_size);

	Sq::Table *create(const char *table_name, const SqType *table_type);
	Sq::Table *create(const char *table_name, const Sq::TypeMethod *table_type);

	template <class StructType>
	Sq::Table *create(const char *name);

	Sq::Table *alter(const char *name, const SqType *table_type = NULL);
	Sq::Table *alter(const char *name, const Sq::TypeMethod *table_type);
	void       add(Sq::TableMethod *table);
	void       drop(const char *name);
	void       rename(const char *from, const char *to);

	Sq::Table *find(const char *table_name);
};

};  // namespace Sq

#endif  // __cplusplus


// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqSchema

	SqEntry
	|
	`--- SqSchema
 */

#define SQ_SCHEMA_MEMBERS          \
	SQ_ENTRY_MEMBERS;              \
	int             version;       \
	SqDestroyFunc   on_destory;    \
	SqRelationPool *relation_pool; \
	SqRelation     *relation

#ifdef __cplusplus
struct SqSchema : Sq::SchemaMethod       // <-- 1. inherit C++ member function(method)
#else
struct SqSchema
#endif
{
	SQ_SCHEMA_MEMBERS;                   // <-- 2. inherit member variable
/*	// ------ SqEntry members ------
	const SqType   *type;        // tables and their migrations data.
	const char     *name;        // schema name. It can be NULL.
	size_t          offset;
	unsigned int    bit_field;

	// ------ SqSchema members ------
	int             version;

	SqDestroyFunc   on_destory;       // on destroy callback. It is used by derived Sqdb.
	SqRelationPool *relation_pool;    // relation pool for (SQLite) migration
	SqRelation     *relation;         // relation of tables
 */
};

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
SqTable *sq_schema_find(SqSchema *schema, const char *table_name)
{
	return (SqTable*)sq_entry_find((SqEntry*)schema, table_name, NULL);
}

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

// C functions
SqTable *sq_schema_find(SqSchema *schema, const char *table_name);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define SchemaMethod functions. */

inline Sq::Table *SchemaMethod::create(const char   *table_name,
                                       const char   *type_name,
                                       const SqType *table_type,
                                       size_t        instance_size)
{
	return (Sq::Table*)sq_schema_create_full((SqSchema*)this, table_name, type_name, table_type, instance_size);
}
inline Sq::Table *SchemaMethod::create(const char           *table_name,
                                       const char           *type_name,
                                       const Sq::TypeMethod *table_type,
                                       size_t                instance_size)
{
	return (Sq::Table*)sq_schema_create_full((SqSchema*)this, table_name, type_name, (const SqType*)table_type, instance_size);
}

inline Sq::Table *SchemaMethod::create(const char *table_name, const SqType *table_type) {
	return (Sq::Table*)sq_schema_create_full((SqSchema*)this, table_name, NULL, table_type, 0);
}
inline Sq::Table *SchemaMethod::create(const char *table_name, const Sq::TypeMethod *table_type) {
	return (Sq::Table*)sq_schema_create_full((SqSchema*)this, table_name, NULL, (const SqType*)table_type, 0);
}

template <class StructType>
inline Sq::Table *SchemaMethod::create(const char *name) {
	return (Sq::Table*)sq_schema_create_full((SqSchema*)this, name, typeid(StructType).name(), NULL, sizeof(StructType));
}

inline Sq::Table *SchemaMethod::alter(const char *name, const SqType *table_type) {
	return (Sq::Table*)sq_schema_alter((SqSchema*)this, name, table_type);
}
inline Sq::Table *SchemaMethod::alter(const char *name, const Sq::TypeMethod *table_type) {
	return (Sq::Table*)sq_schema_alter((SqSchema*)this, name, (const SqType*)table_type);
}
inline void  SchemaMethod::add(Sq::TableMethod *table) {
	// add table in schema->type
	sq_type_add_entry((SqType*) ((SqSchema*)this)->type, (SqEntry*)table, 1, 0);
	((SqSchema*)this)->bit_field |= SQB_CHANGED;
}
inline void  SchemaMethod::drop(const char *name) {
	sq_schema_drop((SqSchema*)this, name);
}
inline void  SchemaMethod::rename(const char *from, const char *to) {
	sq_schema_rename((SqSchema*)this, from, to);
}

inline Sq::Table *SchemaMethod::find(const char *table_name) {
	return (Sq::Table*)sq_schema_find((SqSchema*)this, table_name);
}

/* All derived struct/class must be C++11 standard-layout. */

struct Schema : SqSchema {
	Schema(const char *name = NULL) {
		sq_schema_init(this, name);
	}
	Schema(int version) {
		sq_schema_init(this, NULL);
		this->version = version;
	}
	~Schema() {
		sq_schema_final(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_SCHEMA_H
