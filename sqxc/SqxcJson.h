/*
 *   Copyright (C) 2025-2026 by C.H. Huang
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

#ifndef SQXC_JSON_H
#define SQXC_JSON_H

/* SqxcJson is default JSON converter. It is alias for SqxcCjson or SqxcJsonc.
   If both exist, Sqxc's default JSON converter uses SqxcCjson, but SqxcJsonc will still compile.
   Developer can set SQ_CONFIG_SQXC_CJSON_ONLY_IF_POSSIBLE in SqConfig.h to disable SqxcJsonc if SqxcCjson is available.
 */

#include <sqxc/SqConfig.h>
#if SQ_CONFIG_HAVE_CJSON


// ----------------------------------------------------------------------------
// SqxcCjson: use cJSON library
#include <sqxc/SqxcCjson.h>

typedef SqxcCjson        SqxcJson;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

#define SQXC_INFO_JSON_PARSER     (&sqxcInfo_CjsonParser)
#define SQXC_INFO_JSON_WRITER     (&sqxcInfo_CjsonWriter)
#define sqxc_json_parser_new()        sqxc_new(SQXC_INFO_CJSON_PARSER)
#define sqxc_json_writer_new()        sqxc_new(SQXC_INFO_CJSON_WRITER)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {
	/* All derived struct/class must be C++11 standard-layout. */
	typedef XcCjsonParser    XcJsonParser;
	typedef XcCjsonWriter    XcJsonWriter;
};  // namespace Sq

#endif  // __cplusplus


#elif SQ_CONFIG_HAVE_JSONC    // SQ_CONFIG_HAVE_CJSON


// ----------------------------------------------------------------------------
// SqxcJsonc: use json-c library
#include <sqxc/SqxcJsonc.h>

typedef SqxcJsonc        SqxcJson;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

#define SQXC_INFO_JSON_PARSER     (&sqxcInfo_JsoncParser)
#define SQXC_INFO_JSON_WRITER     (&sqxcInfo_JsoncWriter)
#define sqxc_json_parser_new()        sqxc_new(SQXC_INFO_JSONC_PARSER)
#define sqxc_json_writer_new()        sqxc_new(SQXC_INFO_JSONC_WRITER)

#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {
	/* All derived struct/class must be C++11 standard-layout. */
	typedef XcJsoncParser    XcJsonParser;
	typedef XcJsoncWriter    XcJsonWriter;
};  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_CONFIG_HAVE_CJSON

#endif  // SQXC_JSON_H
