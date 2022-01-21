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

#ifndef SQ_TYPE_H
#define SQ_TYPE_H


/* about macro SQ_GET_TYPE_NAME(Type):
 * It is used to get name of structured data type in C and C++ code.
 * warning: You will get difference type name from C and C++ source code when you use gcc to compile
 *          because gcc's typeid(Type).name() will return strange name.
 */
#ifdef __cplusplus
#include <typeinfo>
#define SQ_GET_TYPE_NAME(Type)        ( (char*)typeid(Type).name() )
#else
#define SQ_GET_TYPE_NAME(Type)        #Type
#endif

#include <SqPtrArray.h>
#include <SqEntry.h>
#include <Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqType        SqType;

typedef void  (*SqTypeFunc)(void *instance, const SqType *type);
typedef int   (*SqTypeParseFunc)(void *instance, const SqType *type, Sqxc *xc_src);
typedef Sqxc *(*SqTypeWriteFunc)(void *instance, const SqType *type, Sqxc *xc_dest);

/*
	SqType initializer macro

	// sample 1:
	typedef struct User    User;

	const SqEntry *UserEntries[] = {...};
	const SqType   UserType1 = SQ_TYPE_INITIALIZER(User, UserEntries, 0);

	// sample 2: initializer with init/final func
	void  user_init(User *user);
	void  user_final(User *user);

	const SqType   UserType2 = SQ_TYPE_INITIALIZER_FULL(User, UserEntries, 0, user_init, user_final);
 */

#define SQ_TYPE_INITIALIZER(StructType, EntryPtrArray, bit_value)  \
{                                                                  \
	.size  = sizeof(StructType),                                   \
	.init  = (SqTypeFunc) NULL,                                    \
	.final = (SqTypeFunc) NULL,                                    \
	.parse = sq_type_object_parse,                                 \
	.write = sq_type_object_write,                                 \
	.name  = SQ_GET_TYPE_NAME(StructType),                         \
	.entry   = (SqEntry**) EntryPtrArray,                          \
	.n_entry = sizeof(EntryPtrArray) / sizeof(SqEntry*),           \
	.bit_field = bit_value,                                        \
	.ref_count = 0,                                                \
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
	.ref_count = 0,                                                \
}

/* SqType::bit_field - SQB_TYPE_xxxx */
#define SQB_TYPE_DYNAMIC     (1<<0)    // equal SQB_DYNAMIC, for internal use only
#define SQB_TYPE_SORTED      (1<<1)

// ----------------------------------------------------------------------------
// macro for maintaining C/C++ inline functions easily

// void SQ_TYPE_ERASE_ENTRY_ADDR(SqType *type, void **element_addr, int count)
#define SQ_TYPE_ERASE_ENTRY_ADDR(type, element_addr, count)    \
		{                                                      \
		if ((type)->bit_field & SQB_TYPE_DYNAMIC)              \
			sq_ptr_array_erase(&(type)->entry,                 \
					(int)((SqEntry**)element_addr - (type)->entry), count); \
		}

// void SQ_TYPE_STEAL_ENTRY_ADDR(SqType *type, void **element_addr, int count)
#define SQ_TYPE_STEAL_ENTRY_ADDR(type, element_addr, count)    \
		{                                                      \
		if ((type)->bit_field & SQB_TYPE_DYNAMIC)              \
			SQ_PTR_ARRAY_STEAL_ADDR(&(type)->entry, element_addr, count); \
		}

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

// create/destroy dynamic SqType.
// if 'prealloc_size' is 0, allocate default size.
// if user want create a basic (not structured) data type, pass 'prealloc_size' = -1 and 'entry_destroy_func' = NULL.
SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);

// these function only work if SqType.bit_field has SQB_TYPE_DYNAMIC
void     sq_type_ref(SqType *type);
void     sq_type_unref(SqType *type);

// copy data from static SqType to dynamic SqType. 'type_dest' must be raw memory.
// if 'type_dest' is NULL, function will create dynamic SqType.
// return dynamic SqType.
SqType  *sq_type_copy_static(SqType *type_dest, const SqType *static_type_src, SqDestroyFunc entry_free_func);

// initialize/finalize self
// if 'prealloc_size' is 0, allocate default size.
// if user want create a basic (not structured) data type, pass 'prealloc_size' = -1 and 'entry_destroy_func' = NULL.
void     sq_type_init_self(SqType *type, int prealloc_size, SqDestroyFunc entry_destroy_func);
void     sq_type_final_self(SqType *type);

// initialize/finalize instance
void    *sq_type_init_instance(const SqType *type, void *instance, int is_pointer);
void     sq_type_final_instance(const SqType *type, void *instance, int is_pointer);

// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
void     sq_type_add_entry(SqType *type, const SqEntry *entry, int n_entry, size_t sizeof_entry);
// add entry from SqEntry pointer array to dynamic SqType.
void     sq_type_add_entry_ptrs(SqType *type, const SqEntry **entry_ptrs, int n_entry_ptrs);

// find SqEntry in SqType.entry.
// If cmp_func is NULL and SqType.entry is sorted, it will use binary search to find entry by name.
void   **sq_type_find_entry(const SqType *type, const void *key, SqCompareFunc cmp_func);

// sort SqType.entry by name if SqType is dynamic.
void     sq_type_sort_entry(SqType *type);

// calculate instance size for dynamic structured data type.
// if 'inner_entry' == NULL, it use all entries in SqType to calculate size.
// if user add 'inner_entry' to SqType, pass argument 'entry_removed' = false.
// if user remove 'inner_entry' from SqType, pass argument 'entry_removed' = true.
unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed);

/* SqType-built-in.c - SqTypeFunc and SqTypeXcFunc functions */

int   sq_type_int_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_int_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_uint_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_uint_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_int64_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_int64_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_uint64_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_uint64_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_time_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_time_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_double_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_double_write(void *instance, const SqType *type, Sqxc *xc_dest);

void  sq_type_string_final(void *instance, const SqType *type);
int   sq_type_string_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_string_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_object_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_object_write(void *instance, const SqType *type, Sqxc *xc_dest);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/*	TypeMethod : C++ struct is used by SqType and it's children.

	Note: If you add, remove, or change methods here, do the same things in SqType.
 */

struct TypeMethod {
	// these function only work if SqType.bit_field has SQB_TYPE_DYNAMIC
	void  ref();
	void  unref();

	// create dynamic SqType and copy data from static SqType
	SqType *copyStatic(SqDestroyFunc entry_free_func);

	// initialize/finalize self
	void  initSelf(int prealloc_size, SqDestroyFunc entry_destroy_func);
	void  initSelf(int prealloc_size, void (*entry_destroy_func)(SqEntry*) );
	void  finalSelf();

	// initialize/finalize instance
	void *initInstance(void *instance, int is_pointer = 0);
	void  finalInstance(void *instance, int is_pointer = 0);

	// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
	// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
	void  addEntry(const SqEntry *entry, int n_entry = 1, size_t sizeof_entry = 0);

	// add entry from SqEntry pointer array to dynamic SqType.
	void  addEntry(const SqEntry **entry_ptrs, int n_entry_ptrs = 1);

	// find SqEntry in SqType.entry.
	// If cmp_func is NULL and SqType.entry is sorted, it will use binary search to find entry by name.
	Sq::Entry **findEntry(const void *key, SqCompareFunc cmp_func = NULL);

	// sort SqType.entry by name if SqType is dynamic.
	void  sortEntry();

	// calculate size for dynamic SqType.
	// if "inner_entry" == NULL, it use all entries to calculate size.
	// otherwise it use "inner_entry" to calculate size.
	unsigned int  decideSize(const SqEntry *inner_entry = NULL, bool entry_removed = false);

	// erase entry in SqType if SqType is dynamic.
	void     eraseEntry(void **element_addr, int count = 1);
	void     eraseEntry(SqEntry **element_addr, int count = 1);

	// steal entry in SqType if SqType is dynamic.
	void     stealEntry(void **element_addr, int count = 1);
	void     stealEntry(SqEntry **element_addr, int count = 1);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
	SqType - define how to initialize, finalize, and convert instance.
	         User can define constant or dynamic SqType.

	SqType must have no base struct because I need use aggregate initialization with it.
 */

#define SQ_TYPE_MEMBERS           \
	unsigned int      size;       \
	SqTypeFunc        init;       \
	SqTypeFunc        final;      \
	SqTypeParseFunc   parse;      \
	SqTypeWriteFunc   write;      \
	char             *name;       \
	SqEntry         **entry;      \
	int               n_entry;    \
	uint16_t          bit_field;  \
	uint16_t          ref_count

struct SqType
{
//	SQ_TYPE_MEMBERS;
/*	// ------ SqType members ------  */
	unsigned int   size;        // instance size

	SqTypeFunc     init;        // initialize instance
	SqTypeFunc     final;       // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to assign "name"
	// or use macro SQ_GET_TYPE_NAME()
	char          *name;

//	SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry);
//	// ------ SqPtrArray members ------
	SqEntry      **entry;
	int            n_entry;

//	SqType.entry is array of SqEntry pointer if current SqType is for C struct.
//	SqType.entry can't be freed if SqType.n_entry == -1

	// SqType::bit_field has SQB_TYPE_DYNAMIC if this is dynamic SqType and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED if SqType::entry is sorted.
	uint16_t       bit_field;
	uint16_t       ref_count;    // reference count for dynamic SqType only

#ifdef __cplusplus
	/* Note: If you add, remove, or change methods here, do the same things in Sq::TypeMethod. */

	// these function only work if SqType.bit_field has SQB_TYPE_DYNAMIC
	void  ref() {
		sq_type_ref((SqType*)this);
	}
	void  unref() {
		sq_type_unref((SqType*)this);
	}

	// create dynamic SqType and copy data from static SqType
	SqType *copyStatic(SqDestroyFunc entry_free_func) {
		return sq_type_copy_static(NULL, (const SqType*)this, entry_free_func);
	}

	// initialize/finalize self
	void  initSelf(int prealloc_size, SqDestroyFunc entry_destroy_func) {
		sq_type_init_self((SqType*)this, prealloc_size, entry_destroy_func);
	}
	void  initSelf(int prealloc_size, void (*entry_destroy_func)(SqEntry*) ) {
		sq_type_init_self((SqType*)this, prealloc_size, (SqDestroyFunc)entry_destroy_func);
	}
	void  finalSelf() {
		sq_type_final_self((SqType*)this);
	}

	// initialize/finalize instance
	void *initInstance(void *instance, int is_pointer = 0) {
		return sq_type_init_instance((const SqType*)this, instance, is_pointer);
	}
	void  finalInstance(void *instance, int is_pointer = 0) {
		sq_type_final_instance((const SqType*)this, instance, is_pointer);
	}

	// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
	// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
	void  addEntry(const SqEntry *entry, int n_entry = 1, size_t sizeof_entry = 0) {
		sq_type_add_entry((SqType*)this, entry, n_entry, sizeof_entry);
	}
	// add entry from SqEntry pointer array to dynamic SqType.
	void  addEntry(const SqEntry **entry_ptrs, int n_entry_ptrs = 1) {
		sq_type_add_entry_ptrs((SqType*)this, entry_ptrs, n_entry_ptrs);
	}

	// find SqEntry in SqType.entry.
	// If cmp_func is NULL and SqType.entry is sorted, it will use binary search to find entry by name.
	Sq::Entry **findEntry(const void *key, SqCompareFunc cmp_func = NULL) {
		return (Sq::Entry**)sq_type_find_entry((const SqType*)this, key, cmp_func);
	}
	// sort SqType.entry by name if SqType is dynamic.
	void  sortEntry() {
		sq_type_sort_entry((SqType*)this);
	}
	// calculate size for dynamic SqType.
	// if "inner_entry" == NULL, it use all entries to calculate size.
	// otherwise it use "inner_entry" to calculate size.
	unsigned int  decideSize(const SqEntry *inner_entry = NULL, bool entry_removed = false) {
		return sq_type_decide_size((SqType*)this, inner_entry, entry_removed);
	}

	// erase entry in SqType if SqType is dynamic.
	void     eraseEntry(void **element_addr, int count = 1) {
		SQ_TYPE_ERASE_ENTRY_ADDR((SqType*)this, element_addr, count);
	}
	void     eraseEntry(SqEntry **element_addr, int count = 1) {
		SQ_TYPE_ERASE_ENTRY_ADDR((SqType*)this, element_addr, count);
	}
	void     eraseEntry(Sq::Entry **element_addr, int count = 1) {
		SQ_TYPE_ERASE_ENTRY_ADDR((SqType*)this, (SqEntry**)element_addr, count);
	}
	// steal entry in SqType if SqType is dynamic.
	void     stealEntry(void **element_addr, int count = 1) {
		SQ_TYPE_STEAL_ENTRY_ADDR((SqType*)this, element_addr, count);
	}
	void     stealEntry(SqEntry **element_addr, int count = 1) {
		SQ_TYPE_STEAL_ENTRY_ADDR((SqType*)this, element_addr, count);
	}
	void     stealEntry(Sq::Entry **element_addr, int count = 1) {
		SQ_TYPE_STEAL_ENTRY_ADDR((SqType*)this, (SqEntry**)element_addr, count);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C definitions: define C extern data and related macros.

#ifdef __cplusplus
extern "C" {
#endif

/* SqType-built-in.c - built-in types */
extern  const  SqType      SqType_BuiltIn_[];
extern  const  SqType      SqType_PtrArray_;
extern  const  SqType      SqType_StringArray_;
extern  const  SqType      SqType_IntptrArray_;

enum {
	SQ_TYPE_BOOL_INDEX,
	SQ_TYPE_INT_INDEX,
	SQ_TYPE_UINT_INDEX,
	SQ_TYPE_INTPTR_INDEX,
	SQ_TYPE_INT64_INDEX,
	SQ_TYPE_UINT64_INDEX,
	SQ_TYPE_TIME_INDEX,
	SQ_TYPE_DOUBLE_INDEX,
	SQ_TYPE_STRING_INDEX,
	SQ_TYPE_CHAR_INDEX,
};

#define SQ_TYPE_BOOL       (&SqType_BuiltIn_[SQ_TYPE_BOOL_INDEX])
#define SQ_TYPE_INT        (&SqType_BuiltIn_[SQ_TYPE_INT_INDEX])
#define SQ_TYPE_UINT       (&SqType_BuiltIn_[SQ_TYPE_UINT_INDEX])
#define SQ_TYPE_INTPTR     (&SqType_BuiltIn_[SQ_TYPE_INTPTR_INDEX])
#define SQ_TYPE_INT64      (&SqType_BuiltIn_[SQ_TYPE_INT64_INDEX])
#define SQ_TYPE_UINT64     (&SqType_BuiltIn_[SQ_TYPE_UINT64_INDEX])
#define SQ_TYPE_TIME       (&SqType_BuiltIn_[SQ_TYPE_TIME_INDEX])
#define SQ_TYPE_DOUBLE     (&SqType_BuiltIn_[SQ_TYPE_DOUBLE_INDEX])
#define SQ_TYPE_STRING     (&SqType_BuiltIn_[SQ_TYPE_STRING_INDEX])
// ---- SQ_TYPE for SQL ----
#define SQ_TYPE_CHAR       (&SqType_BuiltIn_[SQ_TYPE_CHAR_INDEX])
/* update below definition if you insert type in SqType_BuiltIn_[] */

// std::is_integral<Type>::value == true
#define SQ_TYPE_INTEGER_BEG       SQ_TYPE_BOOL
#define SQ_TYPE_INTEGER_END       SQ_TYPE_TIME

// std::is_arithmetic<Type>::value == true
#define SQ_TYPE_ARITHMETIC_BEG    SQ_TYPE_BOOL
#define SQ_TYPE_ARITHMETIC_END    SQ_TYPE_DOUBLE

#define SQ_TYPE_BUILTIN_BEG       SQ_TYPE_BOOL
#define SQ_TYPE_BUILTIN_END       SQ_TYPE_CHAR

#define SQ_TYPE_BUILTIN_INDEX(type)  ((type) - SQ_TYPE_BUILTIN_BEG)

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

	SqType *typePtrArray = sq_type_copy_static(NULL, SQ_TYPE_PTR_ARRAY, NULL);
	typePtrArray->entry = (SqEntry**) element_SqType;
	typePtrArray->n_entry = -1;
 */
#define SQ_TYPE_PTR_ARRAY     (&SqType_PtrArray_)

/* implement string (char*) array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_STRING_ARRAY directly. */
#define SQ_TYPE_STRING_ARRAY  (&SqType_StringArray_)

/* implement intptr_t array by SqPtrArray (SqType-PtrArray.c)
   User can use SQ_TYPE_INTPTR_ARRAY directly. */
#define SQ_TYPE_INTPTR_ARRAY  (&SqType_IntptrArray_)

/* macro for accessing variable of SqType */
#define sq_type_get_ptr_array(type)    ((SqPtrArray*)&(type)->entry)

/* Fake type for user-defined special type (SqType-fake.c) */
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
		( (SqType*)(type)<=SQ_TYPE_FAKE6 && (SqType*)(type)>=SQ_TYPE_FAKE0 )
#define SQ_TYPE_NOT_FAKE(type)    \
		( (SqType*)(type)> SQ_TYPE_FAKE6 || (SqType*)(type)< SQ_TYPE_FAKE0 )

struct SqTypeFake {
	SqType *nth[SQ_TYPE_N_FAKE];
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
void  sq_type_erase_entry_addr(SqType *type, void **element_addr, int count)
{
	SQ_TYPE_ERASE_ENTRY_ADDR(type, element_addr, count);
}

#ifdef __cplusplus  // C++
inline
#else               // C99
static inline
#endif
void  sq_type_steal_entry_addr(SqType *type, void **element_addr, int count)
{
	SQ_TYPE_STEAL_ENTRY_ADDR(type, element_addr, count);
}

#else   // __STDC_VERSION__ || __cplusplus

// C functions
void  sq_type_erase_entry_addr(SqType *type, void **element_addr, int count);
void  sq_type_steal_entry_addr(SqType *type, void **element_addr, int count);

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

extern  const  SqType      SqType_StdString_;    // C++ std::string

#define SQ_TYPE_STD_STRING    ((SqType*)&SqType_StdString_)

/*
#include <SqType-stl-cpp.h>
 */

namespace Sq {

// these function only work if SqType.bit_field has SQB_TYPE_DYNAMIC
inline void  TypeMethod::ref() {
	sq_type_ref((SqType*)this);
}
inline void  TypeMethod::unref() {
	sq_type_unref((SqType*)this);
}

// create dynamic SqType and copy data from static SqType
inline SqType *TypeMethod::copyStatic(SqDestroyFunc entry_free_func) {
	return sq_type_copy_static(NULL, (const SqType*)this, entry_free_func);
}

// initialize/finalize self
inline void  TypeMethod::initSelf(int prealloc_size, SqDestroyFunc entry_destroy_func) {
	sq_type_init_self((SqType*)this, prealloc_size, entry_destroy_func);
}
inline void  TypeMethod::initSelf(int prealloc_size, void (*entry_destroy_func)(SqEntry*) ) {
	sq_type_init_self((SqType*)this, prealloc_size, (SqDestroyFunc)entry_destroy_func);
}
inline void  TypeMethod::finalSelf() {
	sq_type_final_self((SqType*)this);
}

// initialize/finalize instance
inline void *TypeMethod::initInstance(void *instance, int is_pointer) {
	return sq_type_init_instance((const SqType*)this, instance, is_pointer);
}
inline void  TypeMethod::finalInstance(void *instance, int is_pointer) {
	sq_type_final_instance((const SqType*)this, instance, is_pointer);
}

// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
inline void  TypeMethod::addEntry(const SqEntry *entry, int n_entry, size_t sizeof_entry) {
	sq_type_add_entry((SqType*)this, entry, n_entry, sizeof_entry);
}
// add entry from SqEntry pointer array to dynamic SqType.
inline void  TypeMethod::addEntry(const SqEntry **entry_ptrs, int n_entry_ptrs) {
	sq_type_add_entry_ptrs((SqType*)this, entry_ptrs, n_entry_ptrs);
}

// find SqEntry in SqType.entry.
// If cmp_func is NULL and SqType.entry is sorted, it will use binary search to find entry by name.
inline Sq::Entry **TypeMethod::findEntry(const void *key, SqCompareFunc cmp_func) {
	return (Sq::Entry**)sq_type_find_entry((const SqType*)this, key, cmp_func);
}
// sort SqType.entry by name if SqType is dynamic.
inline void  TypeMethod::sortEntry() {
	sq_type_sort_entry((SqType*)this);
}
// calculate size for dynamic SqType.
// if "inner_entry" == NULL, it use all entries to calculate size.
// otherwise it use "inner_entry" to calculate size.
inline unsigned int  TypeMethod::decideSize(const SqEntry *inner_entry, bool entry_removed) {
	return sq_type_decide_size((SqType*)this, inner_entry, entry_removed);
}

// erase entry in SqType if SqType is dynamic.
inline void     TypeMethod::eraseEntry(void **element_addr, int count) {
	SQ_TYPE_ERASE_ENTRY_ADDR((SqType*)this, element_addr, count);
}
inline void     TypeMethod::eraseEntry(SqEntry **element_addr, int count) {
	SQ_TYPE_ERASE_ENTRY_ADDR((SqType*)this, element_addr, count);
}
// steal entry in SqType if SqType is dynamic.
inline void     TypeMethod::stealEntry(void **element_addr, int count) {
	SQ_TYPE_STEAL_ENTRY_ADDR((SqType*)this, element_addr, count);
}
inline void     TypeMethod::stealEntry(SqEntry **element_addr, int count) {
	SQ_TYPE_STEAL_ENTRY_ADDR((SqType*)this, element_addr, count);
}

/*
// sample code for eraseEntry() and stealEntry()

	SqEntry** entryAddr;

	entryAddr = type->findEntry("field_name");
	if (entryAddr) {
		type->eraseEntry(entryAddr);
//		type->stealEntry(entryAddr);
	}
 */

/* All derived struct/class must be C++11 standard-layout. */

struct Type : SqType {
	Type() {}
	Type(int prealloc_size, SqDestroyFunc entry_destroy_func) {
		sq_type_init_self(this, prealloc_size, entry_destroy_func);
	}
	Type(int prealloc_size, void (*entry_destroy_func)(SqEntry*)) {
		sq_type_init_self(this, prealloc_size, (SqDestroyFunc)entry_destroy_func);
	}
	~Type(void) {
		sq_type_final_self(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TYPE_H
