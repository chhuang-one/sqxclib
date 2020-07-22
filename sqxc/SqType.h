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

#ifndef SQ_TYPE_H
#define SQ_TYPE_H

/*	e.g. declare static unsorted SqType

	typedef struct User     User;

	struct User {
		int    id;
		char*  name;
		char*  email;
	};

	// --- UserFields is unsorted
	static const SqField  *UserFields[] = {
		&(SqField) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_HIDDEN},
		&(SqField) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
		&(SqField) {SQ_TYPE_STRING, "email", offsetof(User, email), SQB_HIDDEN_NULL},
	};

	// --- UserType use unsorted UserFields
	const SqType UserType = {
		sizeof(User),                          // size
		NULL,                                  // init
		NULL,                                  // final
		sq_type_object_parse,                  // parse
		sq_type_object_write,                  // write
		SQ_GET_TYPE_NAME(User),                // name
		UserFields,                            // map
		sizeof(UserFields)/sizeof(SqField*),   // map_length
		0                                      // bit_field
	};
 */

/*	e.g. declare static sorted SqType

	// *** Note:
	// * If UserFields is sorted by SqField::name,
	// * you can set SQB_TYPE_SORTED in SqType::bit_field. See below:

	// --- SortedFields is sorted UserFields (sorted by name)
	static const SqField  *SortedFields[] = {
		&(SqField) {SQ_TYPE_STRING, "email", offsetof(User, email), SQB_HIDDEN_NULL},
		&(SqField) {SQ_TYPE_INT,    "id",    offsetof(User, id),    SQB_PRIMARY | SQB_HIDDEN},
		&(SqField) {SQ_TYPE_STRING, "name",  offsetof(User, name),  0},
	};

	// --- SortedType use SortedFields (set SQB_TYPE_SORTED in SqType::bit_field)
	const SqType SortedType = {
		// Omitted above
		.map        = SortedFields,
		.map_length = sizeof(SortedFields)/sizeof(SqField*),
		.bit_field  = SQB_TYPE_SORTED
	};
 */

/*	We can call C++ constructor/destructor in init()/final()

	// ==== C++ constructor + C malloc()
	buffer = malloc(size);
	object = new (buffer) MyClass();

	// ==== C++ destructor  + C free()
	object->~MyClass();
	free(buffer);
 */

#ifdef __cplusplus
#include <typeinfo>
#define SQ_GET_TYPE_NAME(Type)        ( (char*)typeid(Type).name() )
#else
#define SQ_GET_TYPE_NAME(Type)        #Type
#endif

#include <SqPtrArray.h>
#include <Sqxc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SqType        SqType;
typedef struct SqField       SqField;

typedef void (*SqTypeFunc)(void* instance, SqType* type);
typedef int  (*SqTypeCxFunc)(void* instance, SqType* type, Sqxc* cx);

/* ----------------------------------------------------------------------------
	initializer macro for field->type

	// sample 1:
	typedef struct User    User;

	const SqField *UserFields[] = {...};
	const SqType   UserType1 = SQ_TYPE_INITIALIZER(User, UserFields, 0);

	// sample 2: initializer with init/final func
	void  user_init(User* user);
	void  user_final(User* user);

	const SqType   UserType2 = SQ_TYPE_INITIALIZER_FULL(User, UserFields, 0, user_init, user_final);
 */

#define SQ_TYPE_INITIALIZER(StructType, FieldPtrArray, bit_value)  \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.parse = sq_type_object_parse,                                 \
	.write = sq_type_object_write,                                 \
	.name  = SQ_GET_TYPE_NAME(StructType),                         \
	.map   = (SqField**) FieldPtrArray,                            \
	.map_length = sizeof(FieldPtrArray) / sizeof(SqField*),        \
	.bit_field  = bit_value,                                       \
}

#define SQ_TYPE_INITIALIZER_FULL(StructType, FieldPtrArray, bit_value, init_func, final_func) \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.init  = (SqTypeFunc) init_func,                               \
	.final = (SqTypeFunc) final_func,                              \
	.parse = sq_type_object_parse,                                 \
	.write = sq_type_object_write,                                 \
	.name  = SQ_GET_TYPE_NAME(StructType),                         \
	.map   = (SqField**) FieldPtrArray,                            \
	.map_length = sizeof(FieldPtrArray) / sizeof(SqField*),        \
	.bit_field  = bit_value,                                       \
}

// ----------------------------------------------------------------------------
// SqType::bit_field - SQB_TYPE_xxxx
#define SQB_TYPE_DYNAMIC     (1<<0)    // for internal use only
#define SQB_TYPE_SORTED      (1<<1)

// --------------------------------------------------------
// SqType-built-in.c - built-in types

enum {
	SQ_TYPE_INDEX_INT,
	SQ_TYPE_INDEX_UINT,
	SQ_TYPE_INDEX_INTPTR,
	SQ_TYPE_INDEX_INT64,
	SQ_TYPE_INDEX_UINT64,
	SQ_TYPE_INDEX_DOUBLE,
	SQ_TYPE_INDEX_TIME,
	SQ_TYPE_INDEX_STRING,
};

#define SQ_TYPE_INT        ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INT])
#define SQ_TYPE_UINT       ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_UINT])
#define SQ_TYPE_INTPTR     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INTPTR])
#define SQ_TYPE_INT64      ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INT64])
#define SQ_TYPE_UINT64     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_UINT64])
#define SQ_TYPE_DOUBLE     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_DOUBLE])
#define SQ_TYPE_TIME       ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_TIME])
#define SQ_TYPE_STRING     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_STRING])

#define SQ_TYPE_INTEGER_BEG       SQ_TYPE_INT
#define SQ_TYPE_INTEGER_END       SQ_TYPE_UINT64

// std::is_arithmetic<Type>::value == true
#define SQ_TYPE_ARITHMETIC_BEG    SQ_TYPE_INT
#define SQ_TYPE_ARITHMETIC_END    SQ_TYPE_TIME

#define SQ_TYPE_BUILTIN_BEG       SQ_TYPE_INT
#define SQ_TYPE_BUILTIN_END       SQ_TYPE_STRING

#define SQ_TYPE_IS_INT(type)     \
		( (type)<=SQ_TYPE_INTEGER_END && (type)>=SQ_TYPE_INTEGER_BEG )
#define SQ_TYPE_NOT_INT(type)    \
		( (type)> SQ_TYPE_INTEGER_END || (type)< SQ_TYPE_INTEGER_BEG )

#define SQ_TYPE_IS_ARITHMETIC(type)    \
		( (type)<=SQ_TYPE_BUILTIN_END && (type)>=SQ_TYPE_ARITHMETIC_BEG )
#define SQ_TYPE_NOT_ARITHMETIC(type)    \
		( (type)> SQ_TYPE_BUILTIN_END || (type)< SQ_TYPE_ARITHMETIC_BEG )

#define SQ_TYPE_IS_BUILTIN(type)     \
		( (type)<=SQ_TYPE_BUILTIN_END && (type)>=SQ_TYPE_BUILTIN_BEG )
#define SQ_TYPE_NOT_BUILTIN(type)    \
		( (type)> SQ_TYPE_BUILTIN_END || (type)< SQ_TYPE_BUILTIN_BEG )

#define SQ_TYPE_BUILTIN_INDEX(type)  (type - SQ_TYPE_BUILTIN_BEG)

/* TODO: use std::string to implement SQ_TYPE_STD_STRING
#define SQ_TYPE_STD_STRING    NULL
 */

/* declare SqType for SqPtrArray (SqType-PtrArray.c)
   User must add a SqField that declare type of element.

	SqType*  myPtrArray   = sq_type_copy_static(SQ_TYPE_PTR_ARRAY);
	SqField* elementField = sq_field_new(MyObjectType);
	sq_type_insert_field(myPtrArray, elementField);
 */
#define SQ_TYPE_PTR_ARRAY     ((SqType*)&SqType_PtrArray_)

/* implement array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_STRING_ARRAY directly. */
#define SQ_TYPE_STRING_ARRAY  ((SqType*)&SqType_StringArray_)

/* implement array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_INTPTR_ARRAY directly. */
#define SQ_TYPE_INTPTR_ARRAY  ((SqType*)&SqType_IntptrArray_)

/* TODO: use std::vector to implement SQ_FIELD_STD_VECTOR
#define SQ_TYPE_STD_VECTOR    ((SqType*)&SqType_StdVector_)
 */
/* TODO: use std::vector to implement SQ_TYPE_STD_LIST
#define SQ_TYPE_STD_LIST      ((SqType*)&SqType_StdList_)
 */

// ----------------------------------------------------------------------------
// macro for accessing variable of SqType
#define sq_type_get_array(type)    ((SqPtrArray*)&type->map)

/* ----------------------------------------------------------------------------
	SqType - declare how to create/free/parse/write instance
	         User can declare dynamic or static type that used by SqField
 */

struct SqType
{
	uintptr_t      size;        // instance size

	SqTypeFunc     init;        // initialize instance
	SqTypeFunc     final;       // finalize instance

	SqTypeCxFunc   parse;       // parse SQL/JSON data to instance
	SqTypeCxFunc   write;       // write instance data tp SQL/JSON

	// In C++, you must use typeid(TypeName).name() to assign "name"
	// or use macro SQ_GET_TYPE_NAME()
	char*          name;

//	SQ_PTR_ARRAY_MEMBERS(SqField*, map, map_length);
//	// ------ SqPtrArray members ------
	SqField**      map;
	int            map_length;

	// SqType::bit_field has SQB_TYPE_DYNAMIC if this is dynamic SqType.
	// SqType::bit_field has SQB_TYPE_SORTED if SqType::map is sorted.
	unsigned int   bit_field;
};

// extern
extern  const  SqType      SqType_BuiltIn_[];
extern  const  SqType      SqType_PtrArray_;
extern  const  SqType      SqType_StringArray_;
extern  const  SqType      SqType_IntptrArray_;

// create/destroy dynamic SqType.
// if prealloc_size is 0, allocate default size.
SqType*  sq_type_new(int prealloc_size, SqDestroyFunc field_destroy_func);
void     sq_type_free(SqType* type);

// create dynamic SqType and copy data from static SqType
SqType*  sq_type_copy_static(const SqType* type);

// initialize/finalize instance
void*    sq_type_init_instance(SqType* type, void* instance, int is_pointer);
void     sq_type_final_instance(SqType* type, void* instance, int is_pointer);

// insert SqField to dynamic SqType.
void     sq_type_insert_field(SqType* type, const SqField* field);

/* remove SqField from dynamic SqType.
   if cmp_func == NULL, it will compare key and field->name.
   calling the SqField's destroy function if "do_destroy" is 1.
   return pointer of removed (or destroyed) SqField.
   return NULL if key not found or type is static SqType.
 */
SqField* sq_type_remove_field(SqType* type, const void* key, SqCompareFunc cmp_func, int do_destroy);

// SqField* sq_type_erase_field(SqType* type, const void* key, SqCompareFunc cmp_func);
#define sq_type_erase_field(type, key, cmp_func)   \
		sq_type_remove_field(type, key, cmp_func, 1)

// SqField* sq_type_steal_field(SqType* type, const void* key, SqCompareFunc cmp_func);
#define sq_type_steal_field(type, key, cmp_func)   \
		sq_type_remove_field(type, key, cmp_func, 0)

// find SqField in SqType->map.
// If cmp_func is NULL and SqType is dynamic type, it will sort fields by field's name before finding.
SqField* sq_type_find_field(SqType* type, const void* key, SqCompareFunc cmp_func);

// calculate size for dynamic SqType.
// if "inner_field" == NULL, it use all fields to calculate size.
// otherwise it use "inner_field" to calculate size.
int      sq_type_decide_size(SqType* type, const SqField* inner_field);

// --------------------------------------------------------
// SqType-built-in.c - SqTypeFunc and SqTypeCxFunc functions

int  sq_type_int_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_int_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_uint_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_uint_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_int64_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_int64_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_uint64_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_uint64_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_time_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_time_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_double_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_double_write(void* instance, SqType* type, Sqxc* xcsrc);

void sq_type_string_final(void* instance, SqType* type);
int  sq_type_string_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_string_write(void* instance, SqType* type, Sqxc* xcsrc);

int  sq_type_object_parse(void* instance, SqType* type, Sqxc* xcsrc);
int  sq_type_object_write(void* instance, SqType* type, Sqxc* xcsrc);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_TYPE_H
