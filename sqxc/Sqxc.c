/*
 *   Copyright (C) 2020 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <SqError.h>
#include <Sqxc.h>

#define NESTED_OUTER_ROOT    (&sqxc_nested_root)

static SqxcNested sqxc_nested_root = {0};

// ----------------------------------------------------------------------------
// Sqxc functions

Sqxc*  sqxc_new(const SqxcInfo* xcinfo)
{
	Sqxc* xc;
	SqInitFunc init;

	xc = (Sqxc*)calloc(1, xcinfo->size);
	xc->nested = NESTED_OUTER_ROOT;
	init = xcinfo->init;
	if (init)
		init(xc);
	xc->info = xcinfo;
	return xc;
}

void  sqxc_free(Sqxc* xc)
{
	SqFinalFunc final;

	final = xc->info->final;
	if (final)
		final(xc);
	// free memory
	while (xc->nested != NESTED_OUTER_ROOT)
		sqxc_pop_nested(xc);
	free(xc->buf);
	free(xc);
}

Sqxc* sqxc_new_chain(const SqxcInfo* info, ...)
{
	va_list    arg_list;
	Sqxc*      xc = NULL;
	Sqxc*      cur;

	cur = xc = sqxc_new(info);

	va_start(arg_list, info);
	for (;;) {
		info = va_arg(arg_list, SqxcInfo*);
		if (info == NULL)
			break;
		// append new element
		cur->peer = sqxc_new(info);
		cur->peer->dest = xc;
		cur = cur->peer;
	};
	va_end(arg_list);

	return xc;
}

void  sqxc_free_chain(Sqxc* xc)
{
	Sqxc*  peer;

	// free Sqxc from first to last.
	for (;  xc;  xc = peer) {
		peer = xc->peer;
		sqxc_free(xc);
	}
}

Sqxc*   sqxc_insert(Sqxc* xc, Sqxc* xc_element, int position)
{
	Sqxc* cur;
	Sqxc* prev = NULL;

	for (cur = xc;  cur;  cur = cur->peer, position--) {
		if (position == 0)
			break;
		prev = cur;
	}

	// if 'xc_element' is NULL, return Sqxc element at the given position.
	if (xc_element == NULL)
		return cur;
	// insert a new Sqxc element into the chain
	if (prev)
		prev->peer = xc_element;
	if (cur)
		xc_element->peer = cur;
	// set default destination
	xc_element->dest = xc;
	return (prev) ? xc : xc_element;
}

Sqxc*   sqxc_steal(Sqxc* xc, Sqxc* xc_element)
{
	Sqxc* cur;
	Sqxc* prev = NULL;

	for (cur = xc;  cur;  cur = cur->peer) {
		if (cur == xc_element) {
			if (prev)
				prev->peer = cur->peer;
			cur->peer = NULL;
		}
		prev = cur;
	}
	return (prev) ? xc : xc->peer;
}

Sqxc*   sqxc_find(Sqxc* xc, const SqxcInfo* info)
{
	for (;  xc;  xc = xc->peer) {
		if (xc->info == info)
			return xc;
	}
	return NULL;
}

int   sqxc_broadcast(Sqxc* xc, int id, void* data)
{
	int   code = SQCODE_OK;

	// call SqxcInfo.ctrl()
	for (;  xc;  xc = xc->peer) {
		if (xc->info->ctrl) {
			xc->code = xc->info->ctrl(xc, id, data);
			if (xc->code != SQCODE_OK)
				code = xc->code;
		}
	}
	return code;
}

Sqxc*  sqxc_send(Sqxc* xc)
{
	Sqxc*    cur;
	uint16_t code;

/*	if (xc->error == ERROR_MUST_STOP) {
		xc->code = error_code;
		return xc;
	}
 */

/*	xc->required_type = SQXC_TYPE_ALL;  */
	for (cur = xc;  cur;  cur = cur->peer) {
		if ((cur->supported_type & xc->type) == 0)
			continue;
/*		if ((cur->outputable_type & xc->required_type) == 0)
			continue;
 */
		// change destination before calling send()
		if (cur != xc)
			cur->dest = xc;
		code = cur->info->send(cur, xc);
		if (code == SQCODE_OK) {
			// change current Sqxc element
			if (cur->nested_count == 0 && cur->dest)
				cur = cur->dest;
			cur->code = code;    // set code for convenient
			return cur;
		}
	}
	// No Sqxc element support required type
	xc->code = SQCODE_TYPE_NOT_SUPPORT;
	return xc;
}

// ----------------------------------------------------------------------------
// push/pop nested object/array

#define NESTED_CHUNK_SIZE    (1 << 3)    // 8
#define NESTED_CHUNK_MASK    (NESTED_CHUNK_SIZE -1)

SqxcNested* sqxc_push_nested(Sqxc* xc)
{
	SqxcNested* outer = xc->nested;
	SqxcNested* nested;

/*	// every outer has one inner at the same time
	if (outer != NESTED_OUTER_ROOT && outer->inner) {
		xc->code = SQCODE_TOO_MANY_NESTED;
		return NULL;
	}
 */

	// alloc multiple SqxcNested each time
	if ((xc->nested_count & NESTED_CHUNK_MASK) == 0)
		nested = (SqxcNested*)malloc(sizeof(SqxcNested) * NESTED_CHUNK_SIZE);
	else
		nested = xc->nested +1;
	xc->nested_count++;

	// link
	if (outer != NESTED_OUTER_ROOT)
		outer->inner = nested;
	nested->outer = outer;
	nested->inner = NULL;
	xc->nested = nested;
	return nested;
}

void sqxc_pop_nested(Sqxc* xc)
{
	SqxcNested* outer;

	if (xc->nested != NESTED_OUTER_ROOT) {
		outer = xc->nested->outer;
		if (outer != NESTED_OUTER_ROOT)
			outer->inner = NULL;
		// free multiple SqxcNested each time
		xc->nested_count--;
		if ((xc->nested_count & NESTED_CHUNK_MASK) == 0)
			free(xc->nested);
		xc->nested = outer;
	}
}
