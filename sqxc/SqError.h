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

#ifndef SQ_ERROR_H
#define SQ_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

// internal result code type is uint16_t

#define SQCODE_STATUS                (0)
#define SQCODE_WARNING               (2000)
#define SQCODE_ERROR                 (3000)

#define SQCODE_OK                    (0   + SQCODE_STATUS)

// sqxc_ctrl() result code
#define SQCODE_NOT_SUPPORT           (2   + SQCODE_ERROR)

// parser
#define SQCODE_TYPE_NOT_MATCH        (11  + SQCODE_STATUS)
#define SQCODE_TYPE_NOT_SUPPORT      (12  + SQCODE_WARNING)
#define SQCODE_TYPE_END_ERROR        (13  + SQCODE_ERROR)
#define SQCODE_ENTRY_NOT_FOUND       (21  + SQCODE_WARNING)
#define SQCODE_TOO_MANY_NESTED       (24  + SQCODE_ERROR)
#define SQCODE_NO_ELEMENT_TYPE       (25  + SQCODE_ERROR)

// migration
#define SQCODE_REENTRY_DROPPED       (31  + SQCODE_WARNING)
#define SQCODE_CAN_NOT_SYNC          (32  + SQCODE_ERROR)    // schema version is older than database
#define SQCODE_REFERENCE_NOT_FOUND   (41  + SQCODE_ERROR)
#define SQCODE_DB_SCHEMA_VERSION_0   (42  + SQCODE_ERROR)    // SqApp
#define SQCODE_DB_WRONG_MIGRATIONS   (43  + SQCODE_ERROR)    // SqApp
#define SQCODE_DB_NO_MIGRATIONS      (44  + SQCODE_ERROR)    // SqApp

// SQL
#define SQCODE_OPEN_FAILED           (51  + SQCODE_ERROR)
#define SQCODE_EXEC_ERROR            (52  + SQCODE_ERROR)
#define SQCODE_NO_DATA               (53  + SQCODE_ERROR)    // if the result set is empty.

// JSON
#define SQCODE_JSON_CONTINUE         (61  + SQCODE_STATUS)
#define SQCODE_JSON_ERROR            (62  + SQCODE_ERROR)

// deprecated
#define SQCODE_DB_VERSION_0          SQCODE_DB_SCHEMA_VERSION_0
#define SQCODE_DB_VERSION_MISMATCH   SQCODE_DB_WRONG_MIGRATIONS
#define SQCODE_OPEN_FAIL             SQCODE_OPEN_FAILED
#define SQCODE_UNCOMPLETED_JSON      SQCODE_JSON_CONTINUE


#ifdef __cplusplus
}
#endif

#endif  // SQ_ERROR_H
