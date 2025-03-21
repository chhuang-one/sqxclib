/*
 *   Copyright (C) 2020-2025 by C.H. Huang
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

#ifndef SQXC_SQL_H
#define SQXC_SQL_H

#include <stdint.h>

#include <SqPtrArray.h>
#include <Sqxc.h>
#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.

typedef struct SqxcSql        SqxcSql;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo      sqxcInfo_SQL;
#define SQXC_INFO_SQL    (&sqxcInfo_SQL)

#define sqxc_sql_new()        sqxc_new(SQXC_INFO_SQL)

// macro for accessing variable of SqxcSql

#define sqxc_sql_id(xcsql)         ( ((SqxcSql*)xcsql)->id )
#define sqxc_sql_changes(xcsql)    ( ((SqxcSql*)xcsql)->changes )
#define sqxc_sql_condition(xcsql)  ( ((SqxcSql*)xcsql)->condition )
#define sqxc_sql_set_db(xcsql, sqdb)         \
		{	((SqxcSql*)xcsql)->db = sqdb;    \
			((SqxcSql*)xcsql)->quote[0] = (sqdb)->info->quote.identifier[0];   \
			((SqxcSql*)xcsql)->quote[1] = (sqdb)->info->quote.identifier[1];   \
		}

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure

/*	SqxcSql - Sqxc data convert to SQL statement. (destination of output chain)

	Sqxc
	|
	`--- SqxcSql

	                  +-> SqxcJsoncWriter -+
	( output )        |                    |       (SQL statement)
	SqType::write() --+--------------------+-> SqxcSql   ---> sqdb_exec()

   The correct way to derive Sqxc:  (conforming C++11 standard-layout)
   1. Use Sq::XcMethod to inherit member function(method).
   2. Use SQXC_MEMBERS to inherit member variable.
   3. Add variable and non-virtual function in derived struct.
   ** This can keep std::is_standard_layout<>::value == true
 */

#ifdef __cplusplus
struct SqxcSql : Sq::XcMethod            // <-- 1. inherit C++ member function(method)
#else
struct SqxcSql
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
	size_t       buf_size;
	size_t       buf_writed;

	// ------------------------------------------
	// properties

	uint16_t     supported_type;  // supported SqxcType (bit field) for inputting, it can change at runtime.
//	uint16_t     outputable_type; // supported SqxcType (bit field) for outputting, it can change at runtime.

	// ------------------------------------------
	// arguments that used by SqxcInfo::send()

	// output arguments
//	uint16_t     required_type;   // required SqxcType (bit field) if 'code' == SQCODE_TYPE_NOT_MATCHED
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

	// ------ SqxcSql members ------     // <-- 3. Add variable and non-virtual function in derived struct.

	// output
	Sqdb        *db;

	char         quote[2];

	// controlled variable
	int          mode;        // 1 == INSERT, 0 == UPDATE

	// variable for UPDATE command
	const char  *condition;   // WHERE condition.
	SqPtrArray   columns;     // UPDATE column list
	bool         columns_sorted;

	// Sqdb result variable
	int64_t      id;          // the last inserted row id.
	int64_t      changes;     // number of rows changed, deleted, or inserted.

	// runtime variable
	uint16_t     outer_type;  // SQXC_TYPE_OBJECT, SQXC_TYPE_ARRAY or SQXC_TYPE_UNKNOWN
	int          row_count;   // used by INSERT
	int          col_count;   // used by INSERT and UPDATE
	size_t       buf_reuse;   // used by INSERT and UPDATE

	SqBuffer     values_buf;  // used by INSERT INTO VALUES
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* All derived struct/class must be C++11 standard-layout. */

struct XcSql : SqxcSql
{
	// constructor
	XcSql() {
		sqxc_init((Sqxc*)this, SQXC_INFO_SQL);
	}
	// destructor
	~XcSql() {
		sqxc_final((Sqxc*)this);
	}

	void setDb(Sqdb *db) {
		sqxc_sql_set_db((SqxcSql*)this, db);
	}
};

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQXC_SQL_H
