/*
 *   Copyright (C) 2020-2024 by C.H. Huang
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
 * warning: You will get different type name from C and C++ source code when you use gcc to compile
 *          because gcc's typeid(Type).name() will return strange name.
 */
#ifdef __cplusplus
#include <typeinfo>
#define SQ_GET_TYPE_NAME(Type)        typeid(Type).name()
#else
#define SQ_GET_TYPE_NAME(Type)        #Type
#endif

#include <SqPtrArray.h>
#include <SqEntry.h>       // typedef struct SqType
#include <Sqxc.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

//typedef struct SqType        SqType;

typedef void  (*SqTypeFunc)(void *instance, const SqType *type);
typedef int   (*SqTypeParseFunc)(void *instance, const SqType *type, Sqxc *xc_src);
typedef Sqxc *(*SqTypeWriteFunc)(void *instance, const SqType *type, Sqxc *xc_dest);

/*	SqType initializer macro

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
	sizeof(StructType),                                            \
	(SqTypeFunc) NULL,                                             \
	(SqTypeFunc) NULL,                                             \
	sq_type_object_parse,                                          \
	sq_type_object_write,                                          \
	SQ_GET_TYPE_NAME(StructType),                                  \
	(SqEntry**) EntryPtrArray,                                     \
	sizeof(EntryPtrArray) / sizeof(SqEntry*),                      \
	bit_value,                                                     \
	NULL,                                                          \
}

#define SQ_TYPE_INITIALIZER_FULL(StructType, EntryPtrArray, bit_value, init_func, final_func) \
{                                                                  \
	sizeof(StructType),                                            \
	(SqTypeFunc) init_func,                                        \
	(SqTypeFunc) final_func,                                       \
	sq_type_object_parse,                                          \
	sq_type_object_write,                                          \
	SQ_GET_TYPE_NAME(StructType),                                  \
	(SqEntry**) EntryPtrArray,                                     \
	sizeof(EntryPtrArray) / sizeof(SqEntry*),                      \
	bit_value,                                                     \
	NULL,                                                          \
}

/* SqType::bit_field - SQB_TYPE_xxxx */
#define SQB_TYPE_DYNAMIC                  (1<<0)    // equal SQB_DYNAMIC, for internal use only
#define SQB_TYPE_SORTED                   (1<<1)
#define SQB_TYPE_PARSE_UNKNOWN            (1<<2)    // This type can handle unknown entry

#ifdef SQ_CONFIG_QUERY_ONLY_COLUMN
#define SQB_TYPE_QUERY_FIRST              (1<<3)    // SQL: It has query-only columns
#endif

#define SQB_TYPE_RESERVE_BEG              (1<<4)
#define SQB_TYPE_RESERVE_END              (1<<7)

/* macro for accessing variable of SqType */
#define sq_type_entry_array(type)         ((SqPtrArray*)&(type)->entry)
// deprecated
// sq_type_get_ptr_array(type) is alias of sq_type_get_array(type)
#define sq_type_get_ptr_array             sq_type_entry_array

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

// create/destroy dynamic SqType.
// prealloc_size : capacity of SqType::entry (SqType::entry is SqEntry pointer array).
// entry_destroy_func : DestroyFunc of SqType::entry's elements.
// if 'prealloc_size' is 0, allocate default size.
// if user want create a basic (not structured) data type, pass 'prealloc_size' = -1 and 'entry_destroy_func' = NULL.
SqType  *sq_type_new(int prealloc_size, SqDestroyFunc entry_destroy_func);
void     sq_type_free(SqType *type);

// This function is used to copy SqType.
// It copy data from 'type_src' to 'type_dest', 'type_dest' must be raw memory.
// If 'entry_copy_func' is NULL, 'type_dest' will share SqEntry instances from 'type_src'.
// If 'type_dest' is NULL, it will allocate memory for 'type_dest'.
// return 'type_dest' or newly created SqType.
SqType  *sq_type_copy(SqType *type_dest, const SqType *type_src,
                      SqDestroyFunc entry_free_func,
                      SqCopyFunc    entry_copy_func);
// deprecated
// This function is used to copy SqType from static instance.
//SqType  *sq_type_copy_static(SqType *type_dest, const SqType *type_src, SqDestroyFunc entry_free_func);
#define sq_type_copy_static(type_dest, type_src, entry_free_func)    \
        sq_type_copy(type_dest, type_src, entry_free_func, NULL)

// initialize/finalize SqType itself
// prealloc_size : capacity of SqType::entry (SqType::entry is SqEntry pointer array).
// entry_destroy_func : DestroyFunc of SqType::entry's elements.
// if 'prealloc_size' is 0, allocate default size.
// if user want create a basic (not structured) data type, pass 'prealloc_size' = -1 and 'entry_destroy_func' = NULL.
void     sq_type_init_self(SqType *type, int prealloc_size, SqDestroyFunc entry_destroy_func);
void     sq_type_final_self(SqType *type);

// initialize/finalize instance
void    *sq_type_init_instance(const SqType *type, void *instance, int is_pointer);
void     sq_type_final_instance(const SqType *type, void *instance, int is_pointer);

// clear entry from SqEntry array in dynamic SqType.
void     sq_type_clear_entry(SqType *type);

// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
void     sq_type_add_entry(SqType *type, const SqEntry *entry, int n_entry, size_t sizeof_entry);
// add entry from pointer array of SqEntry to dynamic SqType.
void     sq_type_add_entry_ptrs(SqType *type, const SqEntry **entry_ptrs, int n_entry_ptrs);

// alias of sq_type_erase_entry_addr()
// void  sq_type_remove_entry_addr(SqType *type, SqEntry **inner_entry_addr, int count);
#define  sq_type_remove_entry_addr        sq_type_erase_entry_addr

void     sq_type_erase_entry_addr(SqType *type, SqEntry **inner_entry_addr, int count);
void     sq_type_steal_entry_addr(SqType *type, SqEntry **inner_entry_addr, int count);

// find SqEntry in SqType::entry.
// If 'compareFunc' is NULL and SqType::entry is sorted, it will use binary search to find entry by name.
void   **sq_type_find_entry(const SqType *type, const void *key, SqCompareFunc compareFunc);

//void **sq_type_find_entry_addr(const SqType *type, const void *key, SqCompareFunc compareFunc);
#define  sq_type_find_entry_addr    sq_type_find_entry

// sort SqType::entry by name if SqType is dynamic.
void     sq_type_sort_entry(SqType *type);

// calculate instance size for dynamic structured data type.
// if you add 'inner_entry' to SqType, pass argument 'entry_removed' = false.
// if you remove 'inner_entry' from SqType, pass argument 'entry_removed' = true.
// if you recalculate instance size of SqType, pass 'inner_entry' = NULL and don't care argument 'entry_removed'.
unsigned int  sq_type_decide_size(SqType *type, const SqEntry *inner_entry, bool entry_removed);

// for internal use only
void     sq_type_set_str_addr(SqType *type, char **str_addr, const char *str_src);

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

void  sq_type_str_final(void *instance, const SqType *type);
int   sq_type_str_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_str_write(void *instance, const SqType *type, Sqxc *xc_dest);

int   sq_type_object_parse(void *instance, const SqType *type, Sqxc *xc_src);
Sqxc *sq_type_object_write(void *instance, const SqType *type, Sqxc *xc_dest);

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

struct Type;

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqType - define how to initialize, finalize, and convert instance.
	         User can define constant or dynamic SqType.

	Because 'const' is used to define string 'char*',
	C++ user can initialize static structure easily.

	SqType must have no base struct because I need use aggregate initialization with it.
 */

#define SQ_TYPE_MEMBERS           \
	unsigned int      size;       \
	SqTypeFunc        init;       \
	SqTypeFunc        final;      \
	SqTypeParseFunc   parse;      \
	SqTypeWriteFunc   write;      \
	const char       *name;       \
	SqEntry         **entry;      \
	int               n_entry;    \
	unsigned int      bit_field;  \
	SqDestroyFunc     on_destroy

struct SqType
{
	SQ_TYPE_MEMBERS;
/*	// ------ SqType members ------
	unsigned int   size;           // instance size

	SqTypeFunc     init;           // initialize instance
	SqTypeFunc     final;          // finalize instance

	SqTypeParseFunc   parse;       // parse Sqxc(SQL/JSON) data to instance
	SqTypeWriteFunc   write;       // write instance data to Sqxc(SQL/JSON)

	// In C++, you must use typeid(TypeName).name() to generate type name,
	// or use macro SQ_GET_TYPE_NAME()
	const char    *name;

	// SqType::entry is array of SqEntry pointer if current SqType is for C struct type.
	// SqType::entry isn't freed if SqType::n_entry == -1
	SqEntry      **entry;          // maps to SqPtrArray::data
	int            n_entry;        // maps to SqPtrArray::length
	// *** About above 2 fields:
	// 1. They are expanded by macro SQ_PTR_ARRAY_MEMBERS(SqEntry*, entry, n_entry)
	// 2. They can NOT change data type and order.

	// SqType::bit_field has SQB_TYPE_DYNAMIC if SqType is dynamic and freeable.
	// SqType::bit_field has SQB_TYPE_SORTED  if SqType::entry is sorted.
	unsigned int   bit_field;

	// SqType::on_destroy() is called when program releases SqType.
	// This is mainly used for deriving or customizing SqType.
	// Instance of SqType will be passed to SqType::on_destroy()
	SqDestroyFunc  on_destroy;     // destroy notifier for SqType. It can be NULL.
 */

#ifdef __cplusplus
	/* Note: If you add, remove, or change methods here, do the same things in Sq::TypeMethod. */

	// copy
	Sq::Type *copy(SqDestroyFunc entry_free_func, SqCopyFunc entry_copy_func = NULL) {
		return (Sq::Type*)sq_type_copy(NULL, (const SqType*)this, entry_free_func, entry_copy_func);
	}
	// deprecated
	// This method is only used to copy SqType from static instance.
	Sq::Type *copyStatic(SqDestroyFunc entry_free_func) {
		return (Sq::Type*)sq_type_copy(NULL, (const SqType*)this, entry_free_func, NULL);
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

	// clear entry from SqEntry array in dynamic SqType.
	void  clearEntry() {
		sq_type_clear_entry((SqType*)this);
	}

	void  addEntry(const SqEntry &entry) {
		sq_type_add_entry((SqType*)this, &entry, 1, 0);
	}
	void  addEntry(const Sq::EntryMethod &entry) {
		sq_type_add_entry((SqType*)this, (const SqEntry*)&entry, 1, 0);
	}
	// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
	// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
	void  addEntry(const SqEntry *entry, int n_entry = 1, size_t sizeof_entry = 0) {
		sq_type_add_entry((SqType*)this, entry, n_entry, sizeof_entry);
	}
	void  addEntry(const Sq::EntryMethod *entry, int n_entry = 1, size_t sizeof_entry = 0) {
		sq_type_add_entry((SqType*)this, (const SqEntry*)entry, n_entry, sizeof_entry);
	}
	// add entry from pointer array of SqEntry to dynamic SqType.
	void  addEntry(const SqEntry **entry_ptrs, int n_entry_ptrs = 1) {
		sq_type_add_entry_ptrs((SqType*)this, entry_ptrs, n_entry_ptrs);
	}
	void  addEntry(const Sq::EntryMethod **entry_ptrs, int n_entry_ptrs = 1) {
		sq_type_add_entry_ptrs((SqType*)this, (const SqEntry**)entry_ptrs, n_entry_ptrs);
	}

	// find SqEntry in SqType::entry.
	// If compareFunc is NULL and SqType::entry is sorted, it will use binary search to find entry by name.
	Sq::Entry **findEntry(const void *key, SqCompareFunc compareFunc = NULL) {
		return (Sq::Entry**)sq_type_find_entry((const SqType*)this, key, compareFunc);
	}
	// sort SqType::entry by name if SqType is dynamic.
	void  sortEntry() {
		sq_type_sort_entry((SqType*)this);
	}

	// use all entries to recalculate instance size of SqType.
	unsigned int  decideSize() {
		return sq_type_decide_size((SqType*)this, NULL, false);
	}
	// use 'inner_entry' to calculate instance size of SqType.
	unsigned int  decideSize(const SqEntry *inner_entry, bool entry_removed = false) {
		return sq_type_decide_size((SqType*)this, inner_entry, entry_removed);
	}
	unsigned int  decideSize(const Sq::EntryMethod *inner_entry, bool entry_removed = false) {
		return sq_type_decide_size((SqType*)this, (const SqEntry*)inner_entry, entry_removed);
	}

	// alias of eraseEntry()
	void     removeEntry(void **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     removeEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     removeEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     removeEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}

	// erase entry in SqType if SqType is dynamic.
	void     eraseEntry(void **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     eraseEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     eraseEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     eraseEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	// steal entry in SqType if SqType is dynamic.
	void     stealEntry(void **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     stealEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     stealEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     stealEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C definitions: define C extern data and related macros.

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SQ_TYPE_BOOL_INDEX,
	SQ_TYPE_BOOLEAN_INDEX = SQ_TYPE_BOOL_INDEX,  // alias of SQ_TYPE_BOOL_INDEX
	SQ_TYPE_INT_INDEX,
	SQ_TYPE_UINT_INDEX,
	SQ_TYPE_INT64_INDEX,
	SQ_TYPE_UINT64_INDEX,
	SQ_TYPE_TIME_INDEX,
	SQ_TYPE_DOUBLE_INDEX,
	SQ_TYPE_STR_INDEX,
	SQ_TYPE_STRING_INDEX = SQ_TYPE_STR_INDEX,    // alias of SQ_TYPE_STR_INDEX
	SQ_TYPE_CHAR_INDEX,

	SQ_TYPE_N_BUILT_IN,
};

/* SqType-built-in.c - built-in types */
extern  const  SqType      SqType_BuiltIn_[SQ_TYPE_N_BUILT_IN];
extern  const  SqType      SqType_Buffer_;        // SqBuffer
extern  const  SqType      SqType_Array_;         // SqArray
extern  const  SqType      SqType_IntArray_;
extern  const  SqType      SqType_PtrArray_;
extern  const  SqType      SqType_StrArray_;

#define SQ_TYPE_BOOL       (&SqType_BuiltIn_[SQ_TYPE_BOOL_INDEX])
#define SQ_TYPE_BOOLEAN    SQ_TYPE_BOOL          // alias of SQ_TYPE_BOOL
#define SQ_TYPE_INT        (&SqType_BuiltIn_[SQ_TYPE_INT_INDEX])
#define SQ_TYPE_INTEGER    SQ_TYPE_INT           // alias of SQ_TYPE_INT
#define SQ_TYPE_UINT       (&SqType_BuiltIn_[SQ_TYPE_UINT_INDEX])
#define SQ_TYPE_INT64      (&SqType_BuiltIn_[SQ_TYPE_INT64_INDEX])
#define SQ_TYPE_UINT64     (&SqType_BuiltIn_[SQ_TYPE_UINT64_INDEX])
#define SQ_TYPE_TIME       (&SqType_BuiltIn_[SQ_TYPE_TIME_INDEX])
#define SQ_TYPE_DOUBLE     (&SqType_BuiltIn_[SQ_TYPE_DOUBLE_INDEX])
#define SQ_TYPE_STR        (&SqType_BuiltIn_[SQ_TYPE_STR_INDEX])
#define SQ_TYPE_STRING     SQ_TYPE_STR           // alias of SQ_TYPE_STR
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

/* define SqType for SqBuffer (SqType-buffer.c)
 */
#define SQ_TYPE_BUFFER            (&SqType_Buffer_)     // SqBuffer
#define SQ_TYPE_BINARY            SQ_TYPE_BUFFER        // alias of SQ_TYPE_BUFFER
#define SQ_TYPE_BLOB              SQ_TYPE_BUFFER        // alias of SQ_TYPE_BUFFER

/* define SqType for SqArray (SqType-array.c)
   User must assign element type in SqType::entry and set SqType::n_entry to -1.

	SqType *typeArray = sq_type_copy(NULL, SQ_TYPE_ARRAY, NULL, NULL);
	typeArray->entry = (SqEntry**) element_SqType;
	typeArray->n_entry = -1;
 */
#define SQ_TYPE_ARRAY         (&SqType_Array_)

/* implement int array by SqArray (SqType-array.c)
   User can use SQ_TYPE_INT_ARRAY directly. */
#define SQ_TYPE_INT_ARRAY     (&SqType_IntArray_)

/* define SqType for SqPtrArray (SqType-array.c)
   User must assign element type in SqType::entry and set SqType::n_entry to -1.

	SqType *typePtrArray = sq_type_copy(NULL, SQ_TYPE_PTR_ARRAY, NULL, NULL);
	typePtrArray->entry = (SqEntry**) element_SqType;
	typePtrArray->n_entry = -1;
 */
#define SQ_TYPE_PTR_ARRAY     (&SqType_PtrArray_)

/* implement string (char*) array by SqPtrArray (SqType-array.c)
   User can use SQ_TYPE_STR_ARRAY directly. */
#define SQ_TYPE_STR_ARRAY     (&SqType_StrArray_)

// alias of SQ_TYPE_STR_ARRAY
#define SQ_TYPE_STRING_ARRAY  SQ_TYPE_STR_ARRAY

/* Fake type for user-defined special type (SqType-fake.c) */
#define SQ_TYPE_N_FAKE     6
#define SQ_TYPE_FAKE0      ((SqType*)&SqType_Fake_.nth[0])
#define SQ_TYPE_FAKE1      ((SqType*)&SqType_Fake_.nth[1])
#define SQ_TYPE_FAKE2      ((SqType*)&SqType_Fake_.nth[2])
#define SQ_TYPE_FAKE3      ((SqType*)&SqType_Fake_.nth[3])
#define SQ_TYPE_FAKE4      ((SqType*)&SqType_Fake_.nth[4])
#define SQ_TYPE_FAKE5      ((SqType*)&SqType_Fake_.nth[5])
#define SQ_TYPE_FAKE6      ((SqType*)&SqType_Fake_.unknown)
#define SQ_TYPE_UNKNOWN    ((SqType*)&SqType_Fake_.unknown)

// condition
#define SQ_TYPE_IS_FAKE(type)     \
		( (SqType*)(type)<=SQ_TYPE_FAKE6 && (SqType*)(type)>=SQ_TYPE_FAKE0 )
#define SQ_TYPE_NOT_FAKE(type)    \
		( (SqType*)(type)> SQ_TYPE_FAKE6 || (SqType*)(type)< SQ_TYPE_FAKE0 )

struct SqTypeFake {
	// SQ_TYPE_FAKEx
	SqType *nth[SQ_TYPE_N_FAKE];
	// add spacing, this avoid that nth[SQ_TYPE_N_FAKE-1].write pointer to unknown.parse
	void   *spacing;
	// SQ_TYPE_UNKNOWN
	SqType  unknown;
};

extern  const  struct SqTypeFake   SqType_Fake_;

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define global inline function

#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || defined(__cplusplus)
// define inline functions here if compiler supports inline function.

#else   // __STDC_VERSION__ || __cplusplus
// declare functions here if compiler does NOT support inline function.

#endif  // __STDC_VERSION__ || __cplusplus

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

// C++ data type for SQL CHAR, VARCHAR, TEXT, or BLOB.
extern  const  SqType      SqType_StdString_;       // std::string
extern  const  SqType      SqType_StdVector_;       // std::vector<char>
extern  const  SqType      SqType_StdVectorSize_;   // std::vector<char> resize

// C++ std::string
#define SQ_TYPE_STD_STRING         ((SqType*)&SqType_StdString_)
// alias of SQ_TYPE_STD_STRING
#define SQ_TYPE_STD_STR            SQ_TYPE_STD_STRING

// C++ std::vector<char>
#define SQ_TYPE_STD_VECTOR         ((SqType*)&SqType_StdVector_)
// alias of SQ_TYPE_STD_VECTOR
#define SQ_TYPE_STD_VEC            SQ_TYPE_STD_VECTOR

// C++ std::vector<char> resize
// SQ_TYPE_STD_VECTOR_SIZE will specify size of BLOB by calling std::vector<char>::resize()
// when parsing integer value.
#define SQ_TYPE_STD_VECTOR_SIZE    ((SqType*)&SqType_StdVectorSize_)
// alias of SQ_TYPE_STD_VECTOR_SIZE
#define SQ_TYPE_STD_VEC_SIZE       SQ_TYPE_STD_VECTOR_SIZE

/*
#include <SqType-stl-cpp.h>
 */

namespace Sq {

/*	TypeMethod is used by SqType's children.

	It's derived struct/class must be C++11 standard-layout and has SqType members.

	Note: If you add, remove, or change methods here, do the same things in SqType.
 */
struct TypeMethod
{
	// copy
	Sq::Type *copy(SqDestroyFunc entry_free_func, SqCopyFunc entry_copy_func = NULL) {
		return (Sq::Type*)sq_type_copy(NULL, (const SqType*)this, entry_free_func, entry_copy_func);
	}
	// deprecated
	// This method is used to copy SqType from static instance.
	Sq::Type *copyStatic(SqDestroyFunc entry_free_func) {
		return (Sq::Type*)sq_type_copy(NULL, (const SqType*)this, entry_free_func, NULL);
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

	// clear entry from SqEntry array in dynamic SqType.
	void  clearEntry() {
		sq_type_clear_entry((SqType*)this);
	}

	void  addEntry(const SqEntry &entry) {
		sq_type_add_entry((SqType*)this, &entry, 1, 0);
	}
	void  addEntry(const Sq::EntryMethod &entry) {
		sq_type_add_entry((SqType*)this, (const SqEntry*)&entry, 1, 0);
	}
	// add entry from SqEntry array (NOT pointer array) to dynamic SqType.
	// if 'sizeof_entry' == 0, 'sizeof_entry' will equal sizeof(SqEntry)
	void  addEntry(const SqEntry *entry, int n_entry = 1, size_t sizeof_entry = 0) {
		sq_type_add_entry((SqType*)this, entry, n_entry, sizeof_entry);
	}
	void  addEntry(const Sq::EntryMethod *entry, int n_entry = 1, size_t sizeof_entry = 0) {
		sq_type_add_entry((SqType*)this, (const SqEntry*)entry, n_entry, sizeof_entry);
	}
	// add entry from pointer array of SqEntry to dynamic SqType.
	void  addEntry(const SqEntry **entry_ptrs, int n_entry_ptrs = 1) {
		sq_type_add_entry_ptrs((SqType*)this, entry_ptrs, n_entry_ptrs);
	}
	void  addEntry(const Sq::EntryMethod **entry_ptrs, int n_entry_ptrs = 1) {
		sq_type_add_entry_ptrs((SqType*)this, (const SqEntry**)entry_ptrs, n_entry_ptrs);
	}

	// find SqEntry in SqType::entry.
	// If compareFunc is NULL and SqType::entry is sorted, it will use binary search to find entry by name.
	Sq::Entry **findEntry(const void *key, SqCompareFunc compareFunc = NULL) {
		return (Sq::Entry**)sq_type_find_entry((const SqType*)this, key, compareFunc);
	}
	// sort SqType::entry by name if SqType is dynamic.
	void  sortEntry() {
		sq_type_sort_entry((SqType*)this);
	}

	// use all entries to recalculate instance size of SqType.
	unsigned int  decideSize() {
		return sq_type_decide_size((SqType*)this, NULL, false);
	}
	// use 'inner_entry' to calculate instance size of SqType.
	unsigned int  decideSize(const SqEntry *inner_entry, bool entry_removed = false) {
		return sq_type_decide_size((SqType*)this, inner_entry, entry_removed);
	}
	unsigned int  decideSize(const Sq::EntryMethod *inner_entry, bool entry_removed = false) {
		return sq_type_decide_size((SqType*)this, (const SqEntry*)inner_entry, entry_removed);
	}

	// alias of eraseEntry()
	void     removeEntry(void **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     removeEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     removeEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     removeEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}

	// erase entry in SqType if SqType is dynamic.
	void     eraseEntry(void **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     eraseEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     eraseEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     eraseEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_erase_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	// steal entry in SqType if SqType is dynamic.
	void     stealEntry(void **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     stealEntry(SqEntry **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, inner_entry_addr, count);
	}
	void     stealEntry(Sq::Entry **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
	void     stealEntry(Sq::EntryMethod **inner_entry_addr, int count = 1) {
		sq_type_steal_entry_addr((SqType*)this, (SqEntry**)inner_entry_addr, count);
	}
};

/* All derived struct/class must be C++11 standard-layout. */

struct Type : SqType
{
	// constructor
	Type() {}
	Type(int prealloc_size, SqDestroyFunc entry_destroy_func) {
		sq_type_init_self(this, prealloc_size, entry_destroy_func);
	}
	Type(int prealloc_size, void (*entry_destroy_func)(SqEntry*)) {
		sq_type_init_self(this, prealloc_size, (SqDestroyFunc)entry_destroy_func);
	}
	// destructor
	~Type(void) {
		sq_type_final_self(this);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TYPE_H
