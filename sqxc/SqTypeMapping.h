/*
 *   Copyright (C) 2023 by C.H. Huang
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

#ifndef SQ_TYPE_MAPPING_H
#define SQ_TYPE_MAPPING_H

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structure, macro, enumeration.


// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SQ_SQL_TYPE_UNKNOWN = 0,

	// Order of below SQ_SQL_TYPE must the same as order of built-in SQ_TYPE.
	SQ_SQL_TYPE_BOOLEAN,             // SQ_TYPE_BOOL
	SQ_SQL_TYPE_INT,                 // SQ_TYPE_INT
	SQ_SQL_TYPE_INT_UNSIGNED,        // SQ_TYPE_UINT
	SQ_SQL_TYPE_UNSIGNED_INT       = SQ_SQL_TYPE_INT_UNSIGNED,       // alias
	SQ_SQL_TYPE_BIGINT,              // SQ_TYPE_INT64
	SQ_SQL_TYPE_BIGINT_UNSIGNED,     // SQ_TYPE_UINT64
	SQ_SQL_TYPE_UNSIGNED_BIGINT    = SQ_SQL_TYPE_BIGINT_UNSIGNED,    // alias
	SQ_SQL_TYPE_TIMESTAMP,           // SQ_TYPE_TIME
	SQ_SQL_TYPE_DOUBLE,              // SQ_TYPE_DOUBLE
	SQ_SQL_TYPE_VARCHAR,             // SQ_TYPE_STR
	SQ_SQL_TYPE_CHAR,                // SQ_TYPE_CHAR

	// other SQL types...
	SQ_SQL_TYPE_TEXT,                // SQ_TYPE_STR or SQ_TYPE_STD_STR
	SQ_SQL_TYPE_BINARY,              // BLOB...etc
	SQ_SQL_TYPE_BLOB               = SQ_SQL_TYPE_BINARY,             // alias
	SQ_SQL_TYPE_DECIMAL,

	// other INT series
	SQ_SQL_TYPE_TINYINT,
	SQ_SQL_TYPE_TINYINT_UNSIGNED,
	SQ_SQL_TYPE_UNSIGNED_TINYINT   = SQ_SQL_TYPE_TINYINT_UNSIGNED,   // alias
	SQ_SQL_TYPE_SMALLINT,
	SQ_SQL_TYPE_SMALLINT_UNSIGNED,
	SQ_SQL_TYPE_UNSIGNED_SMALLINT  = SQ_SQL_TYPE_SMALLINT_UNSIGNED,  // alias
	SQ_SQL_TYPE_MEDIUMINT,
	SQ_SQL_TYPE_MEDIUMINT_UNSIGNED,
	SQ_SQL_TYPE_UNSIGNED_MEDIUMINT = SQ_SQL_TYPE_MEDIUMINT_UNSIGNED, // alias

	// other TEXT series
	SQ_SQL_TYPE_TINYTEXT,
	SQ_SQL_TYPE_MEDIUMTEXT,
	SQ_SQL_TYPE_LONGTEXT,
} SqSqlType;

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ declarations: declare C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

// define Sq::SqlType
typedef SqSqlType        SqlType;

};  // namespace Sq

#endif  // __cplusplus

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structure


// ----------------------------------------------------------------------------
// C definitions: define C extern data and related macros.

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_TYPE_MAPPING_H

