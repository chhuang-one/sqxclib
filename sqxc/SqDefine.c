/*
 *   Copyright (C) 2020-2023 by C.H. Huang
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

#include <SqDefine.h>

// This function is used by SqPairs. actual parameter type:
//int sq_compare_str(const char **strAddr1, const char **strAddr2);
int   sq_compare_str(const void  *strAddr1, const void  *strAddr2)
{
	return strcmp(*(char**)strAddr1, *(char**)strAddr2);
}

// This function is used by SqxcSql. actual parameter type:
//int sq_compare_ptr(const void **ptrAddr1, const void **ptrAddr2);
int   sq_compare_ptr(const void  *ptrAddr1, const void  *ptrAddr2)
{
	ptrAddr1 = *(void**)ptrAddr1;
	ptrAddr2 = *(void**)ptrAddr2;

	if (ptrAddr1 < ptrAddr2)
		return -1;
	if (ptrAddr1 > ptrAddr2)
		return  1;
	return 0;
}
