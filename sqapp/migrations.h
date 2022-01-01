/*
 *   Copyright (C) 2021-2022 by C.H. Huang
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

#ifndef MIGRATIONS_H
#define MIGRATIONS_H

#include <SqMigration.h>

// How to mix C and C++ - https://isocpp.org/wiki/faq/mixing-c-and-cpp
#ifdef __cplusplus
extern "C" {
#endif

extern const SqMigration *migrations[];
extern const int          n_migrations;

#include "migrations-declarations"

#ifdef __cplusplus
}
#endif

#endif  // MIGRATIONS_H
