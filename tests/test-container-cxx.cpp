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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <type_traits>  // is_standard_layout<>
#include <iostream>     // cout

#include <SqArray.h>
#include <SqPtrArray.h>
#include <SqStrArray.h>
#include <SqBuffer.h>

using namespace std;

// ----------------------------------------------------------------------------

void test_str_array()
{
	Sq::StrArray *array;
	const char   *strs[] = {"One", "Two", "Three"};
	const int     n_strs = sizeof(strs) / sizeof(char*);
	char        **element;
	int           index;

	array = new Sq::StrArray();
	array->append(strs, n_strs);
	// C++ foreach (lambda)
	array->foreach([](char *element) {
		printf(" - %s", element);
	});
	puts("");

	// Sq::StrArray must copy strings from 'strarray' to array->data
	assert(memcmp(array->data, strs, sizeof(strs)) != 0);

	puts("--- test Sq::StrArray.sort() ---");
	array->append("Abc");
	array->sort();
	// C++ foreach (lambda)
	array->foreach([](char *element) {
		printf(" - %s", element);
	});
	puts("\n");

	element = array->findSorted("Abc");
	assert(array->data == element);

	delete array;

	// --------------------------------
	// Sq::Array<char*> will NOT copy string.

	Sq::Array<char*> array2;
	Sq::Array<char*>::iterator cur;

	array2.append(strs, n_strs);
	array2.insert(1, "Xyz");
	array2.append("z..z..Z");

	for (cur = array2.begin();  cur < array2.end();  cur++)
		std::cout << " - " << *cur;
	std::cout << std::endl;

	assert(strcmp(array2.data[1], "Xyz") == 0);

	puts("--- test Sq::Array<char*>.sort() ---");
	array2.sort();

	for (cur = array2.begin();  cur < array2.end();  cur++)
		std::cout << " - " << *cur;
	std::cout << std::endl;

	element = array2.find("One");
	assert(strcmp(*element, "One") == 0);

	element = array2.findSorted("Two", &index);
	assert(strcmp(*element, "Two") == 0);
	assert(index == 2);
}

void test_int_array_cpp()
{
	Sq::Array<int> iarray(0);
	Sq::Array<int>::iterator cur;
	int  data[]  = {13, 10, 19, 17};
	int  data2[] = {28, 25, 27, 26};
	int  index;
	int *intaddr;

	// append & insert
	iarray.append(data, sizeof(data)  / sizeof(int));
	iarray.insert(2, data2, sizeof(data2) / sizeof(int));
	*iarray.allocAt(0) = 4;

	for (cur = iarray.begin(); cur < iarray.end();  cur++) {
		if (cur != iarray.begin())
			std::cout << ", ";
		std::cout << *cur;
	}
	std::cout << endl;

	assert(iarray.data[0] ==  4);
	assert(iarray.data[6] == 26);
	assert(iarray.at(2)   == 10);
	assert(iarray[7]      == 19);

	// sort & search
	iarray.sort();

	for (cur = iarray.begin(); cur < iarray.end();  cur++) {
		if (cur != iarray.begin())
			std::cout << ", ";
		std::cout << *cur;
	}
	std::cout << endl;

	intaddr = iarray.search(data2[1]);
	assert(*intaddr == data2[1]);

	intaddr = iarray.findSorted(data[3], &index);
	assert(*intaddr == data[3]);
	assert(index == 3);

	// steal
	iarray.steal(intaddr);
	assert(*intaddr == 19);
}

void test_intx3_array()
{
	typedef int IntX3[3];
	typedef SQ_ARRAY_STRUCT(IntX3) IntX3Array;

	IntX3        ix3, *pix3;
	IntX3Array  *ix3array;
	SqArray      array;

	sq_array_init(&array, sizeof(IntX3), 1);

	// print array header
	printf("element_size = %d, capacity = %d\n",
		sq_array_element_size(&array), 
		sq_array_capacity(&array));

	ix3[0] = 21;
	ix3[1] = 32;
	ix3[2] = 13;
	SQ_ARRAY_APPEND(&array, IntX3, &ix3, 1);
	ix3[0] = 64;
	ix3[1] = 75;
	ix3[2] = 56;
	SQ_ARRAY_INSERT(&array, IntX3, 0, &ix3, 1);

	pix3 = sq_array_addr(&array, IntX3, 1);
	assert(pix3[0][0] == 21);

	ix3[0] = 122;
	ix3[1] = 155;
	ix3[2] = 130;
	SQ_ARRAY_APPEND(&array, IntX3, &ix3, 1);
	ix3[0] = 256;
	ix3[1] = 203;
	ix3[2] = 286;
	SQ_ARRAY_INSERT(&array, IntX3, 2, &ix3, 1);

	SQ_ARRAY_STEAL(&array, IntX3, 1, 2);

	for (int i = 0;  i < array.length;  i++) {
		pix3 = sq_array_addr(&array, IntX3, i);
		printf("%d, %d, %d" "\n", pix3[0][0], pix3[0][1], pix3[0][2]);
	}

	ix3array = (IntX3Array*)&array;
	ix3array->data[0][0] = 105;
	pix3 = sq_array_addr(&array, IntX3, 0);
	assert(pix3[0][0] == 105);

	sq_array_final(&array);

	// C++ test
	// Sq::Array<IntX3>
	Sq::Array<IntX3> testix3;

	ix3[0] = 1064;
	ix3[1] = 1275;
	ix3[2] = 1156;
	testix3.append(ix3);
	assert(testix3.data[0][2] == 1156);

	ix3[0] = 2122;
	ix3[1] = 2155;
	ix3[2] = 2130;
	testix3.insert(0, ix3);
	assert(testix3.data[0][2] == 2130);
}

void test_ptr_array()
{
	Sq::PtrArray array;
	void *ptrs[] = {(void*)test_ptr_array, (void*)test_str_array,
	                (void*)test_int_array_cpp, (void*)&array};

	array.append((void*)test_intx3_array);
	array.insert(0 ,ptrs, sizeof(ptrs) / sizeof(void*));

	assert(memcmp(array.data, ptrs, sizeof(ptrs)) == 0);
	assert(array.data[4] == (void*)test_intx3_array);
}

void test_array()
{
	test_ptr_array();
	test_str_array();
	test_int_array_cpp();
	test_intx3_array();
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
	sq_buffer_insert_len(buf, 2, "xyz", 3);
//	sq_buffer_write(buf, "abc");
	sq_buffer_write_len(buf, "abc", 3);
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
	test_array();
	test_buffer();

	cout << "is_arithmetic : " << std::is_arithmetic<time_t>::value << endl;
//	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
	return EXIT_SUCCESS;
}
