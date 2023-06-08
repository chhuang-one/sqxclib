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

/*	SqPair: This is element type of array in SqPairs.
 */
typedef struct SqPair        SqPair;

struct SqPair
{
	void  *key;
	void  *value;
};

#define SQ_PAIRS_CAPACITY    16       // number of pairs allocated at startup


void  sq_pairs_init(SqPairs *pairs, SqCompareFunc key_compare_func)
{
	sq_array_init(pairs, sizeof(SqPair), SQ_PAIRS_CAPACITY);
	pairs->bit_field = 0;
	pairs->key_compare_func   = key_compare_func;
	pairs->key_destroy_func   = NULL;
	pairs->value_destroy_func = NULL;
}

void  sq_pairs_final(SqPairs *pairs)
{
	SqPair *cur = (SqPair*)pairs->data,
	       *end = cur + pairs->length;

	for (;  cur < end;  cur++) {
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(cur->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(cur->value);
	}
	sq_array_final(pairs);
}

void  sq_pairs_add(SqPairs *pairs, void *key, void *value)
{
	SqPair *element;

	pairs->bit_field &= ~SQB_PAIRS_SORTED;
	element = (SqPair*)sq_array_alloc(pairs, 1);
	element->key   = key;
	element->value = value;
}

void *sq_pairs_get(SqPairs *pairs, void *key)
{
	SqPair *element;

	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element) {
		pairs->bit_field |= SQB_PAIRS_FOUND;
		return element->value;
	}
	pairs->bit_field &= ~SQB_PAIRS_FOUND;
	return NULL;
}

void  sq_pairs_erase(SqPairs *pairs, void *key)
{
	SqPair *element;

	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element) {
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(element->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(element->value);
		SQ_ARRAY_STEAL_ADDR(pairs, SqPair, element, 1);
	}
}

void    sq_pairs_steal(SqPairs *pairs, void *key)
{
	SqPair *element;

	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element)
		SQ_ARRAY_STEAL_ADDR(pairs, SqPair, element, 1);
}

void  sq_pairs_sort(SqPairs *pairs)
{
	qsort(pairs->data, pairs->length, sizeof(SqPair),
	      (SqCompareFunc)pairs->key_compare_func);
	pairs->bit_field |= SQB_PAIRS_SORTED;
}

// ----------------------------------------------------------------------------
// SqPair SqCompareFunc

int  sq_pairs_cmp_string(const char **key1, const char **key2)
{
	return strcmp(*key1, *key2);
}
