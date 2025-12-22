/*
 *   Copyright (C) 2021-2025 by C.H. Huang
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

#include <stdlib.h>       // qsort(), bsearch(), malloc(), free()

#include <sqxc/support/SqPairs.h>

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

bool  sq_pairs_add(SqPairs *pairs, void *key, void *value)
{
	SqPair *element;
#if SQ_PAIRS_CHECK_IF_KEY_EXIST
	unsigned int  index;

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key,
	                               pairs->key_compare_func,
	                               &index);
	if (element) {
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(element->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(element->value);
		pairs->bit_field |= SQB_PAIRS_EXIST;
	}
	else {
		element = (SqPair*)sq_array_alloc_at(pairs, index, 1);
		pairs->bit_field &= ~SQB_PAIRS_EXIST;
	}

	element->key   = key;
	element->value = value;
	return (pairs->bit_field & SQB_PAIRS_EXIST) != 0;

#else

	pairs->bit_field &= ~SQB_PAIRS_SORTED;
	element = (SqPair*)sq_array_alloc(pairs, 1);
	element->key   = key;
	element->value = value;
	return false;

#endif
}

void *sq_pairs_get(SqPairs *pairs, void *key)
{
	SqPair *element;

#if SQ_PAIRS_CHECK_IF_KEY_EXIST == 0
	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);
#endif

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element) {
		pairs->bit_field |= SQB_PAIRS_FOUND;
		return element->value;
	}
	pairs->bit_field &= ~SQB_PAIRS_FOUND;
	return NULL;
}

bool  sq_pairs_erase(SqPairs *pairs, void *key)
{
	SqPair *element;

#if SQ_PAIRS_CHECK_IF_KEY_EXIST == 0
	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);
#endif

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element) {
		if (pairs->key_destroy_func)
			pairs->key_destroy_func(element->key);
		if (pairs->value_destroy_func)
			pairs->value_destroy_func(element->value);
		sq_array_steal_addr(pairs, SqPair, element, 1);
		return true;
	}
	return false;
}

bool  sq_pairs_steal(SqPairs *pairs, void *key)
{
	SqPair *element;

#if SQ_PAIRS_CHECK_IF_KEY_EXIST == 0
	if ((pairs->bit_field & SQB_PAIRS_SORTED) == 0)
		sq_pairs_sort(pairs);
#endif

	element = SQ_ARRAY_FIND_SORTED(pairs, SqPair, &key, pairs->key_compare_func, NULL);
	if (element) {
		sq_array_steal_addr(pairs, SqPair, element, 1);
		return true;
	}
	return false;
}

#if SQ_PAIRS_CHECK_IF_KEY_EXIST == 0
void  sq_pairs_sort(SqPairs *pairs)
{
	qsort(pairs->data, pairs->length, sizeof(SqPair), pairs->key_compare_func);
	pairs->bit_field |= SQB_PAIRS_SORTED;
}
#endif  // SQ_PAIRS_CHECK_IF_KEY_EXIST
