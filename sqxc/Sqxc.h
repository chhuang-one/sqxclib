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

/*	The origin of the name 'Sqxc'
	Sqxc convert data between X and C language (X = SQL, JSON...etc)
	xc is an abbreviation. (sqxc namespace "Sq" + "xc" = Sqxc)

	SqxcSql     - convert to SQL (Sqdb)   - SqxcSql.c
	SqxcJsonc   - convert to/from JSON    - SqxcJsonc.c
	SqxcValue   - convert to C structure  - SqxcValue.c

	User can link multiple Sqxc element to convert different types of data.

	Default link direction of Sqxc chain:
	[Sqxc element 1] is head of Sqxc chain, 'peer' is single linked list, 'dest' is data flow.

	                    peer                      peer
	┌----------------┐  <---  ┌----------------┐  <---  ┌----------------┐
	| Sqxc element 3 |        | Sqxc element 2 |        | Sqxc element 1 |
	└----------------┘        └----------------┘  --->  └----------------┘
	        |                                     dest          ^
	        |           dest                                    |
	        └---------------------------------------------------┘

	Function insert() and steal() only link or unlink 'peer' ('peer' is single linked list),
	user may need link 'dest' ('dest' is data flow) by himself in Sqxc chain, especially custom data flow.

	===========================================================================
	sqxc_send() can send data(arguments) between Sqxc elements and change data flow (Sqxc.dest) at runtime.

	Data flow 1: sqxc_send() send from SQL result (column has JSON data) to C value
	If SqxcValue can't match current data type, it will forward data to SqxcJsoncParser.

	                 ┌-> SqxcJsoncParser -┐
	( input )        |                    |
	Sqdb.exec()    --┴--------------------┴--> SqxcValue ---> SqType.parse()


	Data flow 2: sqxc_send() send from C value to SQL (column has JSON data)
	If SqxcSql doesn't support current data type, it will forward data to SqxcJsoncWriter.

	                 ┌-> SqxcJsoncWriter -┐
	( output )       |                    |
	SqType.write() --┴--------------------┴--> SqxcSql   ---> Sqdb.exec()
 */

#ifndef SQXC_H
#define SQXC_H

#include <stddef.h>

#include <SqDefine.h>
#include <SqBuffer.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct Sqxc             Sqxc;
typedef struct SqxcInfo         SqxcInfo;
typedef struct SqxcNested       SqxcNested;

typedef struct SqEntry          SqEntry;    // define in SqEntry.h

/* --- define data type for Sqxc converter --- */

typedef enum {
	SQXC_TYPE_UNKNOWN  =  0,
	SQXC_TYPE_NULL     = (1 << 0),    // 0x0001 , Sqxc.value.pointer = NULL;
	SQXC_TYPE_BOOL     = (1 << 1),    // 0x0002
	SQXC_TYPE_INT      = (1 << 2),    // 0x0004
	SQXC_TYPE_UINT     = (1 << 3),    // 0x0008
	SQXC_TYPE_INT64    = (1 << 4),    // 0x0010
	SQXC_TYPE_UINT64   = (1 << 5),    // 0x0020
	SQXC_TYPE_TIME     = (1 << 6),    // 0x0040
	SQXC_TYPE_DOUBLE   = (1 << 7),    // 0x0080
	SQXC_TYPE_ARITHMETIC = 0xFF,

	SQXC_TYPE_STR      = (1 << 8),    // 0x0100
	SQXC_TYPE_STRING   = (1 << 8),    // 0x0100

	SQXC_TYPE_OBJECT   = (1 << 9),    // 0x0200
	SQXC_TYPE_ARRAY    = (1 << 10),   // 0x0400    // array or other storage structure
	SQXC_TYPE_NESTED   = SQXC_TYPE_OBJECT | SQXC_TYPE_ARRAY,    // 0x0600
	SQXC_TYPE_BASIC    =  0x7FF,

#if 1
	SQXC_TYPE_ALL      =  0x7FF,
#else
	// Text stream must be null-terminated string
	SQXC_TYPE_STREAM   = (1 << 11),   // 0x0800    // e.g. file stream
	SQXC_TYPE_ALL      =  0xFFF,
	SQXC_TYPE_STREAM_END = SQXC_TYPE_END | SQXC_TYPE_STREAM,    // reserve (unused now)
#endif

	// End of SQXC_TYPE_OBJECT, SQXC_TYPE_ARRAY.
	SQXC_TYPE_END      = (1 << 15),   // 0x8000

	SQXC_TYPE_OBJECT_END = SQXC_TYPE_END | SQXC_TYPE_OBJECT,
	SQXC_TYPE_ARRAY_END  = SQXC_TYPE_END | SQXC_TYPE_ARRAY,
} SqxcType;

/* --- control id that used by SqxcInfo.ctrl() --- */

typedef enum {
	// common
	SQXC_CTRL_READY,          // reset status
	SQXC_CTRL_FINISH,         // flush data

	// SqxcSql
	SQXC_SQL_CTRL_INSERT,     // const char *table_name
	SQXC_SQL_CTRL_UPDATE,     // const char *table_name

	SQXC_USER = 100,
} SqxcCtrlId;

typedef int   (*SqxcCtrlFunc)(Sqxc *xc, int ctrl_id, void *data);
typedef int   (*SqxcSendFunc)(Sqxc *xc, Sqxc *arguments_src);

// #if SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH
#define SQXC_IS_DOING_TYPE_MATCH(sqxc)    ((sqxc) == (sqxc)->nested->data3)
// #endif

/*
   macro for maintaining C/C++ inline functions easily

   These macro send data from data source side

	[ {"id": 100, "name": "Bob"} ]

	SQXC_SEND_ARRAY_BEG(sqxc, NULL);        //  [

	SQXC_SEND_OBJECT_BEG(sqxc, NULL);       //  {
	SQXC_SEND_INT(sqxc, "id", 100);         //  "id": 100
	SQXC_SEND_STRING(sqxc, "name", "Bob");  //  "name": "Bob"
	SQXC_SEND_OBJECT_END(sqxc, NULL);       //  }

	SQXC_SEND_ARRAY_END(sqxc, NULL);        //  ]
 */

// void sqxc_send_null(Sqxc *sqxc, const char *entry_name);
#define SQXC_SEND_NULL(sqxc, entry_name)                 \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_NULL;      \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.pointer = NULL;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_bool(Sqxc *sqxc, const char *entry_name, bool value);
#define SQXC_SEND_BOOL(sqxc, entry_name, value_)         \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_BOOL;      \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.boolean = value_;     \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_int(Sqxc **sqxc, const char *entry_name, int value);
#define SQXC_SEND_INT(sqxc, entry_name, value_)          \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_INT;       \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.integer = value_;     \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_uint(Sqxc **sqxc, const char *entry_name, unsigned int value);
#define SQXC_SEND_UINT(sqxc, entry_name, value_)         \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_UINT;      \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.uinteger = value_;    \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_int64(Sqxc **sqxc, const char *entry_name, int64_t value);
#define SQXC_SEND_INT64(sqxc, entry_name, value_)        \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_INT64;     \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.int64 = value_;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_uint64(Sqxc **sqxc, const char *entry_name, uint64_t value);
#define SQXC_SEND_UINT64(sqxc, entry_name, value_)       \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_UINT64;    \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.uint64 = value_;      \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_time(Sqxc **sqxc, const char *entry_name, time_t value);
#define SQXC_SEND_TIME(sqxc, entry_name, value_)         \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_TIME;      \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.rawtime = value_;     \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_double(Sqxc **sqxc, const char *entry_name, double value);
#define SQXC_SEND_DOUBLE(sqxc, entry_name, val)          \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_DOUBLE;    \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.double_ = val;        \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_str(Sqxc **sqxc, const char *entry_name, const char *value);
#define SQXC_SEND_STR(sqxc, entry_name, val)             \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_STR;       \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.str = (char*)val;     \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_string(Sqxc **sqxc, const char *entry_name, const char *value);
#define SQXC_SEND_STRING        SQXC_SEND_STR

// void sqxc_send_object_beg(Sqxc **sqxc, const char *entry_name);
#define SQXC_SEND_OBJECT_BEG(sqxc, entry_name)           \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_OBJECT;    \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.pointer = NULL;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_object_end(Sqxc **sqxc, const char *entry_name);
#define SQXC_SEND_OBJECT_END(sqxc, entry_name)           \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_OBJECT_END;\
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.pointer = NULL;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_array_beg(Sqxc **sqxc, const char *entry_name);
#define SQXC_SEND_ARRAY_BEG(sqxc, entry_name)            \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_ARRAY;     \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.pointer = NULL;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// void sqxc_send_array_end(Sqxc **sqxc, const char *entry_name);
#define SQXC_SEND_ARRAY_END(sqxc, entry_name)            \
		{                                                \
			((Sqxc*)(sqxc))->type = SQXC_TYPE_ARRAY_END; \
			((Sqxc*)(sqxc))->name = entry_name;          \
			((Sqxc*)(sqxc))->value.pointer = NULL;       \
			sqxc = sqxc_send((Sqxc*)(sqxc));             \
		}

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

/* Sqxc C functions */

Sqxc   *sqxc_new(const SqxcInfo *xcinfo);
void    sqxc_free(Sqxc *xc);

void    sqxc_init(Sqxc *xc, const SqxcInfo *xcinfo);
void    sqxc_final(Sqxc *xc);

// create Sqxc chain for input/output
// parameter order from destination to source, return destination Sqxc element.
Sqxc   *sqxc_new_chain(const SqxcInfo *dest_xc_info, ...);
// free Sqxc chain
void    sqxc_free_chain(Sqxc *xc);

// insert a new Sqxc element into the chain at the given position.
// If position is negative, or is larger than the number of element in the Sqxc chain, 'xc_element ' is appended to the Sqxc chain.
// if 'xc_element' is NULL, return Sqxc element at the given position.
// Return the new start of the Sqxc chain
Sqxc   *sqxc_insert(Sqxc *xc, Sqxc *xc_element, int position);

// steal 'xc_element' from 'xc' chain, it doesn't free 'xc_element'
// Return the new start of the Sqxc chain
Sqxc   *sqxc_steal(Sqxc *xc, Sqxc *xc_element);

// Return the found Sqxc element, or NULL if it is not found
Sqxc   *sqxc_find(Sqxc *xc, const SqxcInfo *info);

//Sqxc *sqxc_nth(Sqxc *sqxc, int position);
#define sqxc_nth(sqxc, position)    sqxc_insert(sqxc, NULL, position)

#define sqxc_get_buffer(xc)         (SqBuffer*)(&((Sqxc*)(xc))->buf)

#define sqxc_ctrl(xc, code, data)   ((Sqxc*)(xc))->info->ctrl(xc, code, (void*)(data))

// sqxc_broadcast() broadcast Sqxc chain.
// It will call Sqxc.ctrl() from src to dest in chain of Sqxc.
int     sqxc_broadcast(Sqxc *xc, int id, void *data);

// sqxc_ready() broadcast Sqxc chain to ready
// void sqxc_ready(Sqxc *xc, void *data);
#define sqxc_ready(xc, data)    \
		sqxc_broadcast((Sqxc*)xc, SQXC_CTRL_READY, data)

// sqxc_finish() broadcast Sqxc chain to flush data
// void sqxc_finish(Sqxc *xc, void *error);
#define sqxc_finish(xc, data)   \
		sqxc_broadcast((Sqxc*)xc, SQXC_CTRL_FINISH, data)

/* sqxc_send_to() pass data 'arguments_src' to user specified Sqxc element */
#define sqxc_send_to(xc, arguments_src) \
		((Sqxc*)xc)->info->send((Sqxc*)xc, arguments_src)

/* sqxc_send() is called by data source side.
   It send data to Sqxc element and try to match type in Sqxc chain.

   Because different data type is processed by different Sqxc element,
   It returns current Sqxc elements.
 */
Sqxc   *sqxc_send(Sqxc *xc);

/* --- functions for nested object/array --- */

void        sqxc_clear_nested(Sqxc *xc);
void        sqxc_erase_nested(Sqxc *xc, SqxcNested *nested);
SqxcNested *sqxc_push_nested(Sqxc *xc);
void        sqxc_pop_nested(Sqxc *xc);

// void     sqxc_remove_nested(Sqxc *xc, SqxcNested *nested);
#define sqxc_remove_nested        sqxc_erase_nested

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

struct Xc;

/*	XcMethod is used by Sqxc and it's children.

	It's derived struct/class must be C++11 standard-layout and has Sqxc members.
 */
struct XcMethod
{
	Sq::Buffer *buffer(void);

	void   free();
	void   freeChain();

	Sq::Xc  *insert(XcMethod *xc_element, int position = -1);
	Sq::Xc  *steal(XcMethod *xc_element);
	Sq::Xc  *find(const SqxcInfo *info);
	Sq::Xc  *nth(int position);

	int    ctrl(int id, void *data);
	int    broadcast(int id, void *data);
	int    ready(void *data = NULL);
	int    finish(void *data = NULL);

	void        clearNested(void);
	SqxcNested *push(void);
	void        pop(void);

	int    send(XcMethod *arguments_src);

	/* --- These are called by data source side. --- */

	Sq::Xc  *send(void);
	Sq::Xc  *sendNull(const char *entry_name);
	Sq::Xc  *sendBool(const char *entry_name, bool value);
	Sq::Xc  *sendInt(const char *entry_name, int value);
	Sq::Xc  *sendUint(const char *entry_name, unsigned int value);
	Sq::Xc  *sendInt64(const char *entry_name, int64_t value);
	Sq::Xc  *sendUint64(const char *entry_name, uint64_t value);
	Sq::Xc  *sendTime(const char *entry_name, time_t value);
	Sq::Xc  *sendDouble(const char *entry_name, double value);
	Sq::Xc  *sendString(const char *entry_name, const char *value);
	Sq::Xc  *sendStr(const char *entry_name, const char *value);

	Sq::Xc  *sendObjectBeg(const char *entry_name);
	Sq::Xc  *sendObjectEnd(const char *entry_name);

	Sq::Xc  *sendArrayBeg(const char *entry_name);
	Sq::Xc  *sendArrayEnd(const char *entry_name);
};

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/* SqxcInfo */

struct SqxcInfo
{
	size_t       size;
	SqInitFunc   init;
	SqFinalFunc  final;

	SqxcCtrlFunc ctrl;
	SqxcSendFunc send;
};

/*	SqxcNested - stack data for Sqxc parser or writer

	SqxcNested.data3 has special usage that can run a bit faster when doing object and array type match.
	1. Enable SQ_CONFIG_SQXC_NESTED_FAST_TYPE_MATCH In SqConfig.h
	2. When SqxcNested.data3 pointer to it's Sqxc and Sqxc.send() return SQCODE_TYPE_NOT_MATCH,
	   current SqxcNested is NOT ready to parse object or array, it is doing type match.
	3. When SqxcNested.data3 pointer to SqxcNested.data,
	   current SqxcNested is ready to parse object or array, type has been matched.

	SqxcNested.data3 pointer to other address (or NULL) if you don't use fast type match.

	Because fast type match may change in future, I recommend that user don't use it in their custom parser.
 */

struct SqxcNested
{
	SqxcNested  *outer;
	SqxcNested  *inner;

	void  *data;
	void  *data2;
	void  *data3;
};

/*	Sqxc - Interface for data parse and write

   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

#define SQXC_MEMBERS                   \
	const SqxcInfo  *info;             \
	Sqxc            *peer;             \
	Sqxc            *dest;             \
	SqxcNested  *nested;               \
	int          nested_count;         \
	char        *buf;                  \
	int          buf_size;             \
	int          buf_writed;           \
	uint16_t     supported_type;       \
/*	uint16_t     outputable_type; */   \
/*	uint16_t     required_type;   */   \
	uint16_t     code;                 \
	uint16_t     type;                 \
	const char  *name;                 \
	SqValue      value;                \
	SqEntry     *entry;                \
	void       **error

#ifdef __cplusplus
struct Sqxc : Sq::XcMethod               // <-- 1. inherit C++ member function(method)
#else
struct Sqxc
#endif
{
	SQXC_MEMBERS;                        // <-- 2. inherit member variable
/*	// ------ Sqxc members ------
	const SqxcInfo  *info;

	// Sqxc chain
	Sqxc        *peer;     // pointer to other Sqxc elements (single linked list)
	Sqxc        *dest;     // pointer to current destination in Sqxc chain (data flow)

	// stack of SqxcNested
	SqxcNested  *nested;          // current nested object/array
	int          nested_count;

	// ------------------------------------------
	// Buffer - common buffer for type conversion. To resize this buf:
	// buf = realloc(buf, buf_size);

//	SQ_BUFFER_MEMBERS(buf, buf_size, buf_writed);
	char        *buf;
	int          buf_size;
	int          buf_writed;

	// ------------------------------------------
	// properties

	uint16_t     supported_type;  // supported SqxcType (bit field) for inputting, it can change at runtime.
//	uint16_t     outputable_type; // supported SqxcType (bit field) for outputting, it can change at runtime.

	// ------------------------------------------
	// arguments that used by SqxcInfo->send()

	// output arguments
//	uint16_t     required_type;   // required SqxcType (bit field) if 'code' == SQCODE_TYPE_NOT_MATCH
	uint16_t     code;            // error code (SQCODE_xxxx)

	// input arguments
	uint16_t     type;            // input SqxcType
	const char  *name;
	SqValue      value;           // union SqValue defined in SqDefine.h

	// special input arguments
	SqEntry     *entry;           // SqxcJsonc and SqxcSql use it to decide output. this can be NULL (optional).

	// input / output arguments
	void       **error;
 */
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* define XcMethod functions. */

inline Sq::Buffer *XcMethod::buffer(void) {
	return (Sq::Buffer*)sqxc_get_buffer(this);
}

inline void   XcMethod::free() {
	return sqxc_free((Sqxc*)this);
}
inline void   XcMethod::freeChain() {
	return sqxc_free_chain((Sqxc*)this);
}

inline Sq::Xc  *XcMethod::insert(XcMethod *xc_element, int position) {
	return (Sq::Xc*)sqxc_insert((Sqxc*)this, (Sqxc*)xc_element, position);
}
inline Sq::Xc  *XcMethod::steal(XcMethod *xc_element) {
	return (Sq::Xc*)sqxc_steal((Sqxc*)this, (Sqxc*)xc_element);
}
inline Sq::Xc  *XcMethod::find(const SqxcInfo *info) {
	return (Sq::Xc*)sqxc_find((Sqxc*)this, info);
}
inline Sq::Xc  *XcMethod::nth(int position) {
	return (Sq::Xc*)sqxc_nth((Sqxc*)this, position);
}

inline int  XcMethod::ctrl(int id, void *data) {
	return sqxc_ctrl((Sqxc*)this, id, data);
}
inline int  XcMethod::broadcast(int id, void *data) {
	return sqxc_broadcast((Sqxc*)this, id, data);
}
inline int  XcMethod::ready(void *data) {
	return sqxc_ready((Sqxc*)this, data);
}
inline int  XcMethod::finish(void *data) {
	return sqxc_finish((Sqxc*)this, data);
}

inline void        XcMethod::clearNested(void) {
	sqxc_clear_nested((Sqxc*)this);
}
inline SqxcNested *XcMethod::push(void) {
	return sqxc_push_nested((Sqxc*)this);
}
inline void        XcMethod::pop(void) {
	sqxc_pop_nested((Sqxc*)this);
}

inline int  XcMethod::send(XcMethod *arguments_src) {
	return ((Sqxc*)this)->info->send((Sqxc*)this, (Sqxc*)arguments_src);
}

inline Sq::Xc  *XcMethod::send(void) {
	return (Sq::Xc*)sqxc_send((Sqxc*)this);
}
inline Sq::Xc  *XcMethod::sendNull(const char *entry_name) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_NULL(xc, entry_name);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendBool(const char *entry_name, bool value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_BOOL(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendInt(const char *entry_name, int value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_INT(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendUint(const char *entry_name, unsigned int value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_UINT(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendInt64(const char *entry_name, int64_t value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_INT64(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendUint64(const char *entry_name, uint64_t value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_UINT64(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendTime(const char *entry_name, time_t value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_TIME(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendDouble(const char *entry_name, double value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_DOUBLE(xc, entry_name, value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendString(const char *entry_name, const char *value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_STR(xc, entry_name, (char*)value);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendStr(const char *entry_name, const char *value) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_STR(xc, entry_name, (char*)value);
	return (Sq::Xc*)xc;
}

inline Sq::Xc  *XcMethod::sendObjectBeg(const char *entry_name) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_OBJECT_BEG(xc, entry_name);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendObjectEnd(const char *entry_name) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_OBJECT_END(xc, entry_name);
	return (Sq::Xc*)xc;
}

inline Sq::Xc  *XcMethod::sendArrayBeg(const char *entry_name) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_ARRAY_BEG(xc, entry_name);
	return (Sq::Xc*)xc;
}
inline Sq::Xc  *XcMethod::sendArrayEnd(const char *entry_name) {
	Sqxc *xc = (Sqxc*)this;
	SQXC_SEND_ARRAY_END(xc, entry_name);
	return (Sq::Xc*)xc;
}

/* All derived struct/class must be C++11 standard-layout. */
struct Xc : Sqxc {
	~Xc() {
		sqxc_final(this);
	}
};

typedef struct SqxcInfo         XcInfo;
typedef struct SqxcNested       XcNested;

typedef        SqxcType         XcType;

};  // namespace Sq

#endif  // __cplusplus

#endif  // SQXC_H
