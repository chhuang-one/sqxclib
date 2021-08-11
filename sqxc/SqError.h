/*
 *   Copyright (C) 2020-2021 by C.H. Huang
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

#ifndef SQ_ERROR_H
#define SQ_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

// error code type is uint16_t

#define SQCODE_OK                    0

// error
#define SQCODE_ERROR                 1
#define SQCODE_NOT_SUPPORT           2

// parser - warning
#define SQCODE_ENTRY_NOT_FOUND       11
// parser - error
#define SQCODE_TYPE_NOT_MATCH        21
#define SQCODE_TYPE_NOT_SUPPORT      22
#define SQCODE_TYPE_END_ERROR        23
#define SQCODE_TOO_MANY_NESTED       24
#define SQCODE_NO_ELEMENT_TYPE       25

// migration - warning
#define SQCODE_REENTRY_DROPPED       31
#define SQCODE_CAN_NOT_SYNC          32    // schema version is older than database
// migration - error
#define SQCODE_REFERENCE_NOT_FOUND   41
#define SQCODE_REFERENCE_EACH_OTHER  42

// SQL - error
#define SQCODE_OPEN_FAIL             51
#define SQCODE_EXEC_ERROR            52
#define SQCODE_NO_DATA               53    // if the result set is empty.

// JSON error
#define SQCODE_UNCOMPLETED_JSON      61


#ifdef __cplusplus
}
#endif

#endif  // SQ_ERROR_H
