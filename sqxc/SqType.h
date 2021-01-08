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

#ifndef SQ_TYPE_H
#define SQ_TYPE_H

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
typedef struct SqEntry       SqEntry;

typedef void  (*SqTypeFunc)(void* instance, const SqType* type);
typedef int   (*SqTypeParseFunc)(void* instance, const SqType* type, Sqxc* xc_src);
typedef Sqxc* (*SqTypeWriteFunc)(void* instance, const SqType* type, Sqxc* xc_dest);

/* ----------------------------------------------------------------------------
	SqType initializer macro

	// sample 1:
	typedef struct User    User;

	const SqEntry *UserEntries[] = {...};
	const SqType   UserType1 = SQ_TYPE_INITIALIZER(User, UserEntries, 0);

	// sample 2: initializer with init/final func
	void  user_init(User* user);
	void  user_final(User* user);

	const SqType   UserType2 = SQ_TYPE_INITIALIZER_FULL(User, UserEntries, 0, user_init, user_final);
 */

#define SQ_TYPE_INITIALIZER(StructType, EntryPtrArray, bit_value)  \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.parse = sq_type_object_parse,                                 \
	.write = sq_type_object_write,                                 \
	.name  = SQ_GET_TYPE_NAME(StructType),                         \
	.entry   = (SqEntry**) EntryPtrArray,                          \
	.n_entry = sizeof(EntryPtrArray) / sizeof(SqEntry*),           \
	.bit_field = bit_value,                                        \
}

#define SQ_TYPE_INITIALIZER_FULL(StructType, EntryPtrArray, bit_value, init_func, final_func) \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.init  = (SqTypeFunc) init_func,                               \
	.final = (SqTypeFunc) final_func,                              \
	.parse = sq_type_object_parse,                                 \
	.write = sq_type_object_write,                                 \
	.name  = SQ_GET_TYPE_NAME(StructType),                         \
	.entry   = (SqEntry**) EntryPtrArray,                          \
	.n_entry = sizeof(EntryPtrArray) / sizeof(SqEntry*),           \
	.bit_field = bit_value,                                        \
}

// ----------------------------------------------------------------------------
// SqType::bit_field - SQB_TYPE_xxxx
#define SQB_TYPE_DYNAMIC     (1<<0)    // equal SQB_DYNAMIC, for internal use only
#define SQB_TYPE_SORTED      (1<<1)

// ----------------------------------------------------------------------------
// SqType-built-in.c - built-in types

enum {
	SQ_TYPE_INDEX_BOOL,
	SQ_TYPE_INDEX_INT,
	SQ_TYPE_INDEX_UINT,
	SQ_TYPE_INDEX_INTPTR,
	SQ_TYPE_INDEX_INT64,
	SQ_TYPE_INDEX_UINT64,
	SQ_TYPE_INDEX_TIME,
	SQ_TYPE_INDEX_DOUBLE,
	SQ_TYPE_INDEX_STRING,
};

#define SQ_TYPE_BOOL       ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_BOOL])
#define SQ_TYPE_INT        ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INT])
#define SQ_TYPE_UINT       ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_UINT])
#define SQ_TYPE_INTPTR     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INTPTR])
#define SQ_TYPE_INT64      ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_INT64])
#define SQ_TYPE_UINT64     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_UINT64])
#define SQ_TYPE_TIME       ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_TIME])
#define SQ_TYPE_DOUBLE     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_DOUBLE])
#define SQ_TYPE_STRING     ((SqType*)&SqType_BuiltIn_[SQ_TYPE_INDEX_STRING])

// std::is_integral<Type>::value == true
#define SQ_TYPE_INTEGER_BEG       SQ_TYPE_BOOL
#define SQ_TYPE_INTEGER_END       SQ_TYPE_TIME

// std::is_arithmetic<Type>::value == true
#define SQ_TYPE_ARITHMETIC_BEG    SQ_TYPE_BOOL
#define SQ_TYPE_ARITHMETIC_END    SQ_TYPE_DOUBLE

#define SQ_TYPE_BUILTIN_BEG       SQ_TYPE_BOOL
#define SQ_TYPE_BUILTIN_END       SQ_TYPE_STRING

#define SQ_TYPE_BUILTIN_INDEX(type)  (type - SQ_TYPE_BUILTIN_BEG)

// condition
#define SQ_TYPE_IS_INT(type)     \
		( (type)<=SQ_TYPE_INTEGER_END && (type)>=SQ_TYPE_INTEGER_BEG )
#define SQ_TYPE_NOT_INT(type)    \
		( (type)> SQ_TYPE_INTEGER_END || (type)< SQ_TYPE_INTEGER_BEG )

#define SQ_TYPE_IS_ARITHMETIC(type)     \
		( (type)<=SQ_TYPE_BUILTIN_END && (type)>=SQ_TYPE_ARITHMETIC_BEG )
#define SQ_TYPE_NOT_ARITHMETIC(type)    \
		( (type)> SQ_TYPE_BUILTIN_END || (type)< SQ_TYPE_ARITHMETIC_BEG )

#define SQ_TYPE_IS_BUILTIN(type)     \
		( (type)<=SQ_TYPE_BUILTIN_END && (type)>=SQ_TYPE_BUILTIN_BEG )
#define SQ_TYPE_NOT_BUILTIN(type)    \
		( (type)> SQ_TYPE_BUILTIN_END || (type)< SQ_TYPE_BUILTIN_BEG )

/* define SqType for SqPtrArray (SqType-PtrArray.c)
   User must assign element type in SqType.entry and set SqType.n_entry to -1.

	SqType* typePtrArray = sq_type_copy_static(SQ_TYPE_PTR_ARRAY);
	typePtrArray->entry = (SqEntry**) element_SqType;
	typePtrArray->n_entry = -1;
 */
#define SQ_TYPE_PTR_ARRAY     ((SqType*)&SqType_PtrArray_)

/* implement string (char*) array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_STRING_ARRAY directly. */
#define SQ_TYPE_STRING_ARRAY  ((SqType*)&SqType_StringArray_)

/* implement intptr_t array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_INTPTR_ARRAY directly. */
#define SQ_TYPE_INTPTR_ARRAY  ((SqType*)&SqType_IntptrArray_)

// ----------------------------------------------------------------------------
// macro for accessing variable of SqType
#define sq_type_get_ptr_array(type)    ((SqPtrArray*)&type->entry)

/* ----------------------------------------------------------------------------
    SqType - define how to initialize, finalize, and convert instance.
             User can define constant or dynamic SqType.
 */

struct SqType
{
	uintptr_t      size;        // instance size

	SqTypeFunc     init;        // initialize instance
	SqTypeFunc     final;       // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to assign "name"
	// or use macro SQ_GET_TYPE_NAME()
	char*          name;

//	SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry);
//	// ------ SqPtrArray members ------
	SqEntry**      entry;
	int            n_entry;

//	SqType.entry is array of SqEntry pointer if current SqType is for C struct.
//	SqType.entry can't be freed if SqType.n_entry == -1

	// SqType::bit_field has SQB_TYPE_DYNAMIC if this is dynamic SqType and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED if SqType::entry is sorted.
	uint16_t       bit_field;
	uint16_t       ref_count;    // reference count for dynamic SqType only
};

// extern
extern  const  SqType      SqType_BuiltIn_[];
extern  const  SqType      SqType_PtrArray_;
extern  const  SqType      SqType_StringArray_;
extern  const  SqType      SqType_IntptrArray_;

// create/destroy dynamic SqType.
// if prealloc_size is 0, allocate default size.
SqType*  sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);

// these function only work if SqType.bit_field has SQB_TYPE_DYNAMIC
void     sq_type_ref(SqType* type);
void     sq_type_unref(SqType* type);

// create dynamic SqType and copy data from static SqType
SqType*  sq_type_copy_static(const SqType *type, SqDestroyFunc entry_free_func);

// initialize/finalize instance
void*    sq_type_init_instance(const SqType *type, void* instance, int is_pointer);
void     sq_type_final_instance(const SqType *type, void* instance, int is_pointer);

// add SqEntry to dynamic SqType.
void     sq_type_add_entry(SqType* type, const SqEntry *entry, int n_entry);

// find SqEntry in SqType.entry.
// If cmp_func is NULL and SqType.entry is sorted, it will use binary search to find entry by name.
void**   sq_type_find_entry(const SqType *type, const void *key, SqCompareFunc cmp_func);

// sort SqType.entry by name if SqType is dynamic.
void     sq_type_sort_entry(SqType *type);

// calculate size for dynamic SqType.
// if "inner_entry" == NULL, it use all entries to calculate size.
// otherwise it use "inner_entry" to calculate size.
int      sq_type_decide_size(SqType* type, const SqEntry *inner_entry);

// --------------------------------------------------------
// SqType-built-in.c - SqTypeFunc and SqTypeXcFunc functions

int   sq_type_int_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_int_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_uint_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_uint_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_int64_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_int64_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_uint64_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_uint64_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_time_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_time_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_double_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_double_write(void* instance, const SqType *type, Sqxc* xc_dest);

void  sq_type_string_final(void* instance, const SqType *type);
int   sq_type_string_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_string_write(void* instance, const SqType *type, Sqxc* xc_dest);

int   sq_type_object_parse(void* instance, const SqType *type, Sqxc* xc_src);
Sqxc* sq_type_object_write(void* instance, const SqType *type, Sqxc* xc_dest);

// ----------------------------------------------------------------------------
// Fake type for user-defined special type (SqType-fake.c)

#define SQ_TYPE_N_FAKE     6
#define SQ_TYPE_FAKE0      ((SqType*)&SqType_Fake_.nth[0])
#define SQ_TYPE_FAKE1      ((SqType*)&SqType_Fake_.nth[1])
#define SQ_TYPE_FAKE2      ((SqType*)&SqType_Fake_.nth[2])
#define SQ_TYPE_FAKE3      ((SqType*)&SqType_Fake_.nth[3])
#define SQ_TYPE_FAKE4      ((SqType*)&SqType_Fake_.nth[4])
#define SQ_TYPE_FAKE5      ((SqType*)&SqType_Fake_.nth[5])
#define SQ_TYPE_FAKE6      ((SqType*)&SqType_Fake_.dummy)

// condition
#define SQ_TYPE_IS_FAKE(type)     \
		( (type)<=SQ_TYPE_FAKE6 && (type)>=SQ_TYPE_FAKE0 )
#define SQ_TYPE_NOT_FAKE(type)    \
		( (type)> SQ_TYPE_FAKE6 || (type)< SQ_TYPE_FAKE0 )

struct SqTypeFake {
	SqType* nth[SQ_TYPE_N_FAKE];
	SqType  dummy;
};

extern  const  struct SqTypeFake   SqType_Fake_;

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ inline functions

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// C99 or C++ inline functions

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_type_erase_entry_addr(SqType* type, void** element_addr, int count)
{
	if (type->bit_field & SQB_TYPE_DYNAMIC)
		sq_ptr_array_erase(&type->entry, (SqEntry**)element_addr - type->entry, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_type_steal_entry_addr(SqType* type, void** element_addr, int count)
{
	void* array = &type->entry;

	if (type->bit_field & SQB_TYPE_DYNAMIC)
		SQ_PTR_ARRAY_STEAL_ADDR(array, element_addr, count);
}

#else  // __STDC_VERSION__ || __cplusplus

// C functions
void  sq_type_erase_entry_addr(SqType* type, void** element_addr, int count);
void  sq_type_steal_entry_addr(SqType* type, void** element_addr, int count);

#endif  // __STDC_VERSION__ || __cplusplus

/* ----------------------------------------------------------------------------
   C++ type

	We can call C++ constructor/destructor in init()/final()

	// ==== C++ constructor + C malloc()
	buffer = malloc(size);
	object = new (buffer) MyClass();

	// ==== C++ destructor  + C free()
	object->~MyClass();
	free(buffer);
 */

#ifdef __cplusplus

extern  const  SqType      SqType_StdString_;    // C++ std::string

#define SQ_TYPE_STD_STRING    ((SqType*)&SqType_StdString_)

/* 
#include <SqType-stl-cpp.h>
 */
#endif  // __cplusplus


#endif  // SQ_TYPE_H
