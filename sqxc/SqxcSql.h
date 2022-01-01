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

#ifndef SQXC_SQL_H
#define SQXC_SQL_H

#include <SqPtrArray.h>
#include <Sqxc.h>
#include <Sqdb.h>

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqxcSql        SqxcSql;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

extern const SqxcInfo        *SQXC_INFO_SQL;

#define sqxc_sql_new()        sqxc_new(SQXC_INFO_SQL)

// macro for accessing variable of SqxcSqlite

#define sqxc_sql_id(xcsql)    ((SqxcSql*)xcsql)->id
#define sqxc_sql_set_db(xcsql, sqdb)         \
		{	((SqxcSql*)xcsql)->db = sqdb;    \
			((SqxcSql*)xcsql)->quote[0] = (sqdb)->info->quote.identifier[0];   \
			((SqxcSql*)xcsql)->quote[1] = (sqdb)->info->quote.identifier[1];   \
		}

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/*
	SqxcSql - Sqxc data convert to SQL statement. (destination of output chain)

	Sqxc
	|
	`--- SqxcSql

	                 +-> SqxcJsonWriter --+
	( output )       |                    |       (SQL statement)
	SqType.write() --+--------------------+-> SqxcSql   ---> Sqdb.exec()
	                 |                    |
	                 +--> SqxcXmlWriter --+

	Note: SqxcXmlWriter doesn't implement yet because it is rarely used.

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
	Sqxc        *peer;     // pointer to other Sqxc elements
	Sqxc        *dest;     // pointer to current destination in Sqxc chain

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
	union {
		bool          boolean;
		int           integer;
		int           int_;
		unsigned int  uinteger;
		unsigned int  uint;
		int64_t       int64;
		int64_t       uint64;
		time_t        rawtime;
		double        fraction;
		double        double_;
		const char   *string;
		const char   *stream;     // Text stream must be null-terminated string
		void         *pointer;
	} value;

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
	unsigned int mode;        // 1 == INSERT, 0 == UPDATE
	int          id;          // inserted id; update id if 'condition' == NULL
	char        *condition;   // WHERE condition if mode == 0 (UPDATE)

	// runtime variable
	uint16_t     outer_type;  // SQXC_TYPE_OBJECT, SQXC_TYPE_ARRAY or SQXC_TYPE_NONE
	int          row_count;   // used by INSERT
	int          col_count;   // used by INSERT and UPDATE
	int          buf_reuse;   // used by INSERT and UPDATE

	SqBuffer     values_buf;  // used by INSERT INTO VALUES
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

// conforming C++11 standard-layout
// These are for directly use only. You can NOT derived it.
struct XcSql : SqxcSql
{
	XcSql() {
		sqxc_init((Sqxc*)this, SQXC_INFO_SQL);
	}
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
