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

#ifndef SQ_TOOL_CONFIG_H
#define SQ_TOOL_CONFIG_H

#ifndef SQ_TOOL_PATH_BASE
#define SQ_TOOL_PATH_BASE            NULL
#endif

#define SQ_TOOL_PATH_DATABASE        "/database"
#define SQ_TOOL_PATH_MIGRATIONS      SQ_TOOL_PATH_DATABASE "/migrations"
#define SQ_TOOL_PATH_APP             "/sqxcapp"
#define SQ_TOOL_PATH_TEMPLATES       SQ_TOOL_PATH_APP  "/templates"


#endif  // SQ_TOOL_CONFIG_H
