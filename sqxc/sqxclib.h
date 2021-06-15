/*
 *   Copyright (C) 2021 by C.H. Huang
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

#include <SqConfig.h>
#include <SqDefine.h>
#include <SqError.h>

#include <SqPtrArray.h>
#include <SqBuffer.h>

#include <SqType.h>
#include <SqEntry.h>

#include <SqUtil.h>
#include <SqTable.h>
#include <SqSchema.h>
#include <SqStorage.h>
#include <SqQuery.h>
#include <SqJoint.h>

// ------------------------------------
#include <Sqdb.h>

#ifdef SQ_CONFIG_HAVE_SQLITE
#include <SqdbSqlite.h>
#endif

#ifdef SQ_CONFIG_HAVE_MYSQL
#include <SqdbMysql.h>
#endif

// ------------------------------------
#include <Sqxc.h>

#include <SqxcSql.h>
#include <SqxcValue.h>

#ifdef SQ_CONFIG_HAVE_JSONC
#include <SqxcJsonc.h>
#endif

// ------------------------------------
#include <SqType-stl-cpp.h>    // Sq::TypeStl<StlContainer>
