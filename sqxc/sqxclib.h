/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

#include <SqConfig.h>
#include <SqDefine.h>
#include <SqError.h>

#include <SqArray.h>
#include <SqPtrArray.h>
#include <SqStrArray.h>
#include <SqBuffer.h>
#include <SqConvert.h>

#include <SqType.h>
#include <SqTypeMapping.h>
#include <SqEntry.h>

#include <SqColumn.h>
#include <SqTable.h>
#include <SqSchema.h>
#include <SqStorage.h>
#include <SqQuery.h>
#include <SqJoint.h>

// ------------------------------------
#include <Sqdb.h>

#if SQ_CONFIG_HAVE_SQLITE
#include <SqdbSqlite.h>
#endif

#if SQ_CONFIG_HAVE_MYSQL
#include <SqdbMysql.h>
#endif

#if SQ_CONFIG_HAVE_POSTGRESQL
#include <SqdbPostgre.h>
#endif

// ------------------------------------
#include <Sqxc.h>

#include <SqxcSql.h>
#include <SqxcValue.h>

#if SQ_CONFIG_HAVE_CJSON
#include <SqxcCjson.h>
#endif

#if SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

// ------------------------------------
// library: sqxcpp
#include <SqType-stl-cpp.h>    // Sq::TypeStl<StlContainer>
