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

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <type_traits>  // is_standard_layout<>
#include <iostream>     // cout

#include <SqPtrArray.h>
#include <SqBuffer.h>

using namespace std;

// ----------------------------------------------------------------------------

void test_intptr_array()
{
	Sq::IntptrArray *array;
	intptr_t   intarray[] = {0, 3, 2, 5};
	intptr_t   result[] = {0, 301, 351, 2, 5};

	array = new Sq::IntptrArray();
	array->insert(0, intarray, 4);

	sq_ptr_array_alloc_at(array, 2, 2);
	array->data[2] = 301;
	array->data[3] = 351;

	sq_ptr_array_erase(array, 1, 1);

	// C++ foreach (lambda)
	array->foreach([](intptr_t element) {
		printf(" - %d", (int)element);
	});
	puts("");
	// C foreach
	sq_intptr_array_foreach(array, element) {
		printf(" - %d", (int)element);
	}
	puts("");

	assert(memcmp(array->data, result, sizeof(result)) == 0);
	delete array;
}

void test_string_array()
{
	Sq::StringArray *array;
	const char  *strarray[] = {"One", "Two"};

	array = new Sq::StringArray();
	array->append(strarray, 2);
	// C++ foreach (lambda)
	array->foreach([](char *element) {
		printf(" - %s", element);
	});
	puts("");
	// C foreach
	sq_string_array_foreach(array, element) {
		printf(" - %s", element);
	}
	puts("");

	// Sq::StringArray must copy strings from 'strarray' to array->data
	assert(memcmp(array->data, strarray, sizeof(strarray)) != 0);
	delete array;
}

void test_ptr_array()
{
	test_intptr_array();
	test_string_array();
}

// ----------------------------------------------------------------------------

void test_buffer(void)
{
	SqBuffer *buf;
	int       len;
	const char *result = "13xqyz254abcg";

	buf = sq_buffer_new();

	len = snprintf(NULL, 0, "%d", 13254);
	snprintf(sq_buffer_alloc(buf, len), len+1, "%d", 13254);

//	sq_buffer_insert(buf, 2, "xyz");
	sq_buffer_insert_n(buf, 2, "xyz", 3);
//	sq_buffer_write(buf, "abc");
	sq_buffer_write_n(buf, "abc", 3);
	sq_buffer_write_c(buf, 'g');
	sq_buffer_insert_c(buf, 3, 'q');

	sq_buffer_write_c(buf, 0);
	puts(buf->mem);

	assert(memcmp(buf->mem, result, strlen(result)) == 0);
	sq_buffer_free(buf);
}

// ----------------------------------------------------------------------------

int main(void)
{
	test_ptr_array();
	test_buffer();

	cout << "is_arithmetic : " << std::is_arithmetic<time_t>::value << endl;
//	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
	return EXIT_SUCCESS;
}
