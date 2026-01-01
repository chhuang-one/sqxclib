/*
 *   Copyright (C) 2021-2026 by C.H. Huang
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

#include <sqxc/SqConfig.h>
#include <sqxc/SqDefine.h>
#include <sqxc/SqError.h>

#include <sqxc/SqArray.h>
#include <sqxc/SqPtrArray.h>
#include <sqxc/SqStrArray.h>
#include <sqxc/SqBuffer.h>
#include <sqxc/SqConvert.h>

#include <sqxc/SqType.h>
#include <sqxc/SqTypeMapping.h>
#include <sqxc/SqEntry.h>

#include <sqxc/SqColumn.h>
#include <sqxc/SqTable.h>
#include <sqxc/SqSchema.h>
#include <sqxc/SqStorage.h>
#include <sqxc/SqQuery.h>
#include <sqxc/SqJoint.h>

// ------------------------------------
#include <sqxc/Sqdb.h>

#if SQ_CONFIG_HAVE_SQLITE
#include <sqxc/SqdbSqlite.h>
#endif

#if SQ_CONFIG_HAVE_MYSQL
#include <sqxc/SqdbMysql.h>
#endif

#if SQ_CONFIG_HAVE_POSTGRESQL
#include <sqxc/SqdbPostgre.h>
#endif

// ------------------------------------
#include <sqxc/Sqxc.h>

#include <sqxc/SqxcSql.h>
#include <sqxc/SqxcValue.h>

#if SQ_CONFIG_HAVE_JSON
#include <sqxc/SqxcJson.h>
#endif

#if SQ_CONFIG_HAVE_CJSON
#include <sqxc/SqxcCjson.h>
#endif

#if SQ_CONFIG_HAVE_JSONC
#include <sqxc/SqxcJsonc.h>
#endif

// ------------------------------------
// C++ STL container
#include <sqxc/SqType-stl-cxx.h>    // Sq::TypeStl<StlContainer>

// --------------------------------------------------------
// includes headers in sqxc/support
#include <sqxc/support/sqxcsupport.h>
// includes headers in sqxc/app
#include <sqxc/app/sqxcapp.h>
