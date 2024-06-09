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

/* When user migrate at runtime, column 'migrations.name' in database will be empty string
   because SqApp does NOT contain SqMigration.name string by default.
   Disable it to reduce code size of SqApp.
 */
#define SQ_APP_HAS_MIGRATION_NAME    0

// How to mix C and C++ - https://isocpp.org/wiki/faq/mixing-c-and-cpp
#ifdef __cplusplus
extern "C" {
#endif

extern const SqMigration *sqApp_migrations_default[];
extern const int          sqApp_n_migrations_default;

#include "migrations-declarations"

#ifdef __cplusplus
}
#endif

#endif  // MIGRATIONS_H
