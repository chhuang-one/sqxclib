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

#ifndef SQ_APP_CONFIG_H
#define SQ_APP_CONFIG_H

#ifndef SQ_APP_CONFIG_FILE


// ----------------------------------------------------------------------------
// --- SQL products --- You can enable only one SQL products here

#define DB_SQLITE      1
// #define DB_MYSQL       1
// #define DB_POSTGRE     1

// ----------------------------------------------------------------------------
// --- Database Configuration ---

// common configuration values
#define DB_DATABASE    "sqxcapp-example"

// --- MySQL / MariaDB ---
// connection configuration values
#if DB_MYSQL
#define DB_HOST        "localhost"
#define DB_PORT        3306
#define DB_USERNAME    "root"
#define DB_PASSWORD    ""
#endif  // DB_MYSQL

// --- PostgreSQL ---
// connection configuration values
#if DB_POSTGRE
#define DB_HOST        "localhost"
#define DB_PORT        5432
#define DB_USERNAME    "postgres"
#define DB_PASSWORD    ""
#endif  // DB_POSTGRE

// --- SQLite ---
// file configuration values
#if DB_SQLITE
#define DB_FOLDER      "."
#define DB_EXTENSION   "db"
#endif  // DB_SQLITE


#else   // SQ_APP_CONFIG_FILE has been defined, it will include user's configuration file.
#include SQ_APP_CONFIG_FILE
#endif  // SQ_APP_CONFIG_FILE

#endif  // SQ_APP_CONFIG_H
