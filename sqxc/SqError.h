/*
 *   Copyright (C) 2020 by C.H. Huang
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


#define SQCODE_OK                    0
#define SQCODE_WARNING               0x1000
#define SQCODE_ERROR                 0x2000

// warning - parser
#define SQCODE_ENTRY_NOT_FOUND       3

// warning - 
#define SQCODE_REENTRY_DROPPED       55

// error
#define SQCODE_NOT_SUPPORT           4
#define SQCODE_EXEC_ERROR            5
#define SQCODE_STATIC_DATA           6
#define SQCODE_OPEN_FAIL             7

// error - parser
#define SQCODE_TYPE_NOT_MATCH        14
#define SQCODE_TYPE_NOT_SUPPORT      15
#define SQCODE_TYPE_END_ERROR        16
#define SQCODE_TOO_MANY_NESTED       17
#define SQCODE_NO_ELEMENT_TYPE       18

#define SQCODE_NO_MORE_DATA          25
#define SQCODE_UNCOMPLETED_JSON      26

// error - migration
#define SQCODE_REFERENCE_NOT_FOUND   100
#define SQCODE_REFERENCE_EACH_OTHER  101

#ifdef __cplusplus
}
#endif

#endif  // SQ_ERROR_H
