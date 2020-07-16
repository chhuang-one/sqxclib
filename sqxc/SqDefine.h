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

#ifndef SQ_DEFINE_H
#define SQ_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

// #define SQ_HAVE_NAMING_CONVENTION

typedef void  (*SqInitFunc)(void* value);
typedef void  (*SqFinalFunc)(void* value);
typedef void  (*SqDestroyFunc)(void* value);
typedef int   (*SqCompareFunc)(const void* a, const void* b);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SQ_DEFINE_H
