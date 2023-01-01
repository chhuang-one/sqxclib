/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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

#include <stdlib.h>     // qsort(), bsearch(), malloc(), free()

#include <SqPairs.h>

/*	SqPair: This is element type in array
	        size of this structure == size of 2 pointers because SqPair has 2 pointers
 */
typedef struct SqPair        SqPair;

struct SqPair
{
	void  *key;
	void  *value;
};

#define PAIR_N_PTR           2        // SqPair has 2 pointers
#define N_PAIRS_ALLOCATED    16       // number of pairs allocated at startup

//                                (pairs->x2length / PAIR_N_PTR)
#define SQ_PAIRS_LENGTH(pairs)    (pairs->x2length >> 1)
#define SQ_PAIRS_ELEMENT_SIZE     (sizeof(void*) * PAIR_N_PTR)

#define sq_pairs_search(pairs, key, compare_func)       \
		bsearch( (void*)(key),                          \
		         pairs->data, SQ_PAIRS_LENGTH(pairs),   \
		         SQ_PAIRS_ELEMENT_SIZE, (SqCompareFunc)compare_func)


void  sq_pairs_init(SqPairs *pairs, SqCompareFunc key_compare_func)
{
	sq_ptr_array_init(pairs, N_PAIRS_ALLOCATED * PAIR_N_PTR, NULL);
	pairs->sorted = 0;
	pairs->key_compare_func   = key_compare_func;
	pairs->key_destroy_func   = NULL;
	pairs->value_destroy_func = NULL;
}

void  sq_pairs_final(SqPairs *pairs)
{
	SqPair *element;
	int     n_element = SQ_PAIRS_LENGTH(pairs);

	for (int i = 0;  i < n_element;  i += PAIR_N_PTR) {
		element = (SqPair*)(pairs->data + i);
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(element->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(element->value);
	}
	sq_ptr_array_final(pairs);
}

void  sq_pairs_add(SqPairs *pairs, void *key, void *value)
{
	SqPair *element;

	pairs->sorted  = 0;
	element = (SqPair*)sq_ptr_array_alloc(pairs, PAIR_N_PTR);
	element->key   = key;
	element->value = value;
}

void  sq_pairs_erase(SqPairs *pairs, void *key)
{
	SqPair *element;

	if (pairs->sorted == 0)
		sq_pairs_sort(pairs);

	element = sq_pairs_search(pairs, &key, pairs->key_compare_func);
	if (element) {
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(element->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(element->value);
		sq_ptr_array_erase_addr(pairs, element, PAIR_N_PTR);
	}
}

void    sq_pairs_steal(SqPairs *pairs, void *key)
{
	SqPair *element;

	if (pairs->sorted == 0)
		sq_pairs_sort(pairs);

	element = sq_pairs_search(pairs, &key, pairs->key_compare_func);
	if (element)
		sq_ptr_array_erase_addr(pairs, element, PAIR_N_PTR);
}

void *sq_pairs_find(SqPairs *pairs, void *key)
{
	SqPair *element;

	if (pairs->sorted == 0)
		sq_pairs_sort(pairs);

	element = sq_pairs_search(pairs, &key, pairs->key_compare_func);
	if (element)
		return element->value;
	return NULL;
}

void  sq_pairs_sort(SqPairs *pairs)
{
	qsort(pairs->data, SQ_PAIRS_LENGTH(pairs), SQ_PAIRS_ELEMENT_SIZE,
	      (SqCompareFunc)pairs->key_compare_func);
	pairs->sorted = 1;
}

// ----------------------------------------------------------------------------
// SqPair SqCompareFunc

int  sq_pairs_cmp_string(const char **key1, const char **key2)
{
	return strcmp(*key1, *key2);
}
